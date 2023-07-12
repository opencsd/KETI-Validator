#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>
#include <unistd.h>

struct CpuUsage {
unsigned long long utime;
unsigned long long stime;
};

struct NetworkInterface{
  std::string name;
  unsigned long long rxBytes;
  unsigned long long txBytes;
};

struct CPUStats {
    unsigned long long user;
    unsigned long long nice;
    unsigned long long system;
    unsigned long long idle;
    unsigned long long iowait;
    unsigned long long irq;
    unsigned long long softirq;
    unsigned long long steal;
    unsigned long long guest;
    unsigned long long guestNice;
};

unsigned long long rxBytesBeforeEno;
unsigned long long txBytesBeforeEno;
unsigned long long rxBytesBeforeLo;
unsigned long long txBytesBeforeLo;

unsigned long long rxBytesAfterEno;
unsigned long long txBytesAfterEno;
unsigned long long rxBytesAfterLo;
unsigned long long txBytesAfterLo;
pid_t pid = 0;

CpuUsage previousCpuUsage;
CpuUsage afterCpuUsage;

unsigned long long totalCPUTimeBefore = 0;
unsigned long long totalCPUTimeAfter = 0;

double startEnergyCpu0 = 0;
double startEnergyCpu1 = 0;
double endEnergyCpu0 = 0;
double endEnergyCpu1 = 0;

auto start = std::chrono::high_resolution_clock::now();
auto end = std::chrono::high_resolution_clock::now();

CPUStats getCPUStats() {
    CPUStats stats;

    std::ifstream file("/proc/stat");
    if (!file) {
        std::cerr << "Failed to open /proc/stat" << std::endl;
        return stats;
    }

    std::string line;
    std::getline(file, line);
    std::istringstream iss(line);
    std::string cpuLabel;
    iss >> cpuLabel >> stats.user >> stats.nice >> stats.system >> stats.idle >> stats.iowait >> stats.irq >> stats.softirq >> stats.steal >> stats.guest >> stats.guestNice;

    return stats;
}

unsigned long long getTotalCPUTime(const CPUStats& stats) {
    return stats.user + stats.nice + stats.system + stats.idle + stats.iowait + stats.irq + stats.softirq + stats.steal + stats.guest + stats.guestNice;
}


std::vector<NetworkInterface> getNetworkInterfaces() {
    std::vector<NetworkInterface> interfaces;

    std::ifstream file("/proc/net/dev");
    if (!file) {
        std::cerr << "Failed to open /proc/net/dev" << std::endl;
        return interfaces;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.find(":") != std::string::npos) {
            std::istringstream iss(line);
            std::string name;
            unsigned long long rxBytes, txBytes;
            char colon;
            iss >> name >> colon >> rxBytes >> txBytes;

            NetworkInterface interface;
            interface.name = name.substr(0, name.size() - 1);
            interface.rxBytes = rxBytes;
            interface.txBytes = txBytes;

            interfaces.push_back(interface);
        }
    }

    return interfaces;
}



unsigned long long getInterfaceRxBytes(const std::vector<NetworkInterface>& interfaces, const std::string& interfaceName) {
    for (const auto& interface : interfaces) {
        if (interface.name == interfaceName) {
            return interface.rxBytes;
        }
    }
    return 0;
}

unsigned long long getInterfaceTxBytes(const std::vector<NetworkInterface>& interfaces, const std::string& interfaceName) {
    for (const auto& interface : interfaces) {
        if (interface.name == interfaceName) {
            return interface.txBytes;
        }
    }
    return 0;
}

double readEnergyUsage(const std::string& filePath) {
    std::ifstream file(filePath);
    std::string line;
    std::getline(file, line);
    file.close();

    return std::stoull(line) / 1000000.0;
}

std::string executeCommand(const std::string& command) {
  std::string result;
  std::cout<<command<<"\n";
  FILE* pipe = popen(command.c_str(),"r");
    if (pipe) {
        char buffer[128];
        while (!feof(pipe)) {
            if (fgets(buffer, 128, pipe) != nullptr)
                result += buffer;
        }
        pclose(pipe);
    }
    return result;
}

CpuUsage GetProcessCpuUsage(int pid) { // get cpu usage by tick
CpuUsage cpuUsage{0, 0};
std::ifstream file("/proc/" + std::to_string(pid) + "/stat");
std::string line;

if (std::getline(file, line)) {
    std::istringstream iss(line);
    std::string token;

    // Extract the required fields from the stat file
    for (int i = 1; i <= 13; ++i)
        iss >> token;
    iss >> cpuUsage.utime;
    for (int i = 15; i <= 16; ++i)
        iss >> token;
    iss >> cpuUsage.stime;
}
return cpuUsage;
}

float CalculateCpuUsagePercentage(const CpuUsage& previousUsage, const CpuUsage& currentUsage, int intervalMilliseconds) {
unsigned long long totalTicksDiff = currentUsage.utime + currentUsage.stime - previousUsage.utime - previousUsage.stime;
std::cout<<"Total Ticks Difference : "<<totalTicksDiff<<"\n";
float cpuUsagePercentage = 100.0f * (totalTicksDiff / static_cast<float>(intervalMilliseconds));
return cpuUsagePercentage;
}


void CalculateStart(){
    start = std::chrono::high_resolution_clock::now();
    CPUStats statsBefore = getCPUStats();
    totalCPUTimeBefore = getTotalCPUTime(statsBefore);

    startEnergyCpu0 = readEnergyUsage("/sys/class/powercap/intel-rapl:0:0/energy_uj");
    startEnergyCpu1 = readEnergyUsage("/sys/class/powercap/intel-rapl:1:0/energy_uj");

    std::vector<NetworkInterface> interfacesBefore = getNetworkInterfaces();
    rxBytesBeforeEno = getInterfaceRxBytes(interfacesBefore, "eno1");
    txBytesBeforeEno = getInterfaceTxBytes(interfacesBefore, "eno1");
    rxBytesBeforeLo = getInterfaceRxBytes(interfacesBefore, "lo");
    txBytesBeforeLo = getInterfaceTxBytes(interfacesBefore, "lo");
    pid_t pid = getpid();
    previousCpuUsage = GetProcessCpuUsage(pid);
    
    std::cout<<"Query Analyze : Scan 2, Aggregation 1, Join 1\n";
    std::cout<<"Analyzing Scan Snippets\n";
    std::cout<<"Scan Snippet 1\n"<<"Scan table : orders, "<<"table row : 150000\n"<<"Column Projection : 3, Where : 0, Operator : 16\nout row : 150000\n";
    std::cout<<"Scan Snippet 2\n"<<"Scan table : lineitem, "<<"table row : 600000\n"<<"Column Projection : 2, Where : 5, Operator : 0\nout row : 2830\n";
    std::cout<<"Analyzing Join Snippets\n";
    std::cout<<"Join Snippet 1\n"<<"Join type : Hash Join, target table row : 2830, 150000\n";
    std::cout<<"Analyzing Aggregation Snippets\n";
    std::cout<<"Aggregation Snippet 1\n"<<"Column Projection : 3, group by : l_shipmode\n"<<"order by : 1, "<<"Sum, Avg, Count : 2, out row : 2\n";

    double CSDFreq = 1300000000.0;
    double CSDScan = 33.2488;
    double CSDSend = 674.6128;
    double CSDWhere = 1300.667;
    double CSDDefaultOper = 692.0665;
    double CSDOper = 73.7955;

    double StorageFreq = 1900000000.0;
    double StorageScan = 13707.677;
    double StorageSum = 1148.92714;
    double StorageSend = 18092.523;


}

void CalculateEnd(){
    end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout<<"Query execute time : "<<duration.count()<<"s\n";

    CPUStats statsAfter = getCPUStats();
    totalCPUTimeAfter = getTotalCPUTime(statsAfter);
    unsigned long long cpuTimeUsed = totalCPUTimeAfter - totalCPUTimeBefore;
    unsigned cpuUsage = static_cast<double>(cpuTimeUsed) / ((duration.count()) * sysconf(_SC_CLK_TCK)) * 100;
    std::cout<<"CPU Usage of Host "<<": " <<cpuUsage <<" %\n";

    afterCpuUsage = GetProcessCpuUsage(pid);
    float cpuUsagePercentage = CalculateCpuUsagePercentage(previousCpuUsage, afterCpuUsage, duration.count());
    unsigned long long totalTicksDiff = afterCpuUsage.utime + afterCpuUsage.stime - previousCpuUsage.utime - previousCpuUsage.stime;
    std::cout << "CPU Usage of Process " << pid << ": " << cpuUsagePercentage << "%" << std::endl;
    std::cout << "CPU Use Tick " <<pid<<": "<<totalTicksDiff <<" ns"<<std::endl;

    endEnergyCpu0 = readEnergyUsage("/sys/class/powercap/intel-rapl:0:0/energy_uj");
    endEnergyCpu1 = readEnergyUsage("/sys/class/powercap/intel-rapl:1:0/energy_uj");
    double powerUsage = (endEnergyCpu0 - startEnergyCpu0) + (endEnergyCpu1 - startEnergyCpu1);
    std::cout<< "Energy Usage of Query : "<<powerUsage<<" W"<<std::endl;

    std::vector<NetworkInterface> interfacesAfter = getNetworkInterfaces();
    rxBytesAfterEno = getInterfaceRxBytes(interfacesAfter, "eno1"); 
    rxBytesAfterLo = getInterfaceRxBytes(interfacesAfter, "lo");
    txBytesAfterEno = getInterfaceTxBytes(interfacesAfter, "eno1");
    txBytesAfterLo = getInterfaceTxBytes(interfacesAfter, "lo");

    unsigned long long rxBytesUsedEno = rxBytesAfterEno - rxBytesBeforeEno;
    unsigned long long rxBytesUsedLo = rxBytesAfterLo - rxBytesBeforeLo;
    unsigned long long txBytesUsedEno = txBytesAfterEno - txBytesBeforeEno;
    unsigned long long txBytesUsedLo = txBytesAfterLo - txBytesBeforeLo;

    std::cout << "Network Usage during process execution:" << std::endl;
    std::cout << "RX bytes used Eno: " << rxBytesAfterEno << std::endl;
    std::cout << "RX bytes used Lo : " << rxBytesAfterLo << std::endl;
    std::cout << "TX bytes used Eno: " << txBytesAfterEno << std::endl;
    std::cout << "TX bytes used Lo : " << txBytesAfterLo << std::endl;
    return;
}