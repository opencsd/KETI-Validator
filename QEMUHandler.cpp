#include "QEMUHandler.hpp"
//QEMU 결과 받는 부분

void QEMUHandler::initQEMUHandler(){
    std::thread QEMUHandler_Input_Thread(&QEMUHandler::inputQEMUHandler,this);
    QEMUHandler_Input_Thread.detach();
}

void QEMUHandler::inputQEMUHandler(){
    unordered_map <int,int> rowCountMap;
    int server_fd, client_fd;
    int opt = 1;
    struct  sockaddr_in serv_addr, client_addr;
    socklen_t addrlen = sizeof(client_addr);
    static char cMsg[] = "ok";

    if((server_fd = socket(AF_INET, SOCK_STREAM,0)) == -1){
        std::cout<<"[QEMU Handler] SOCKET CREATION FAILED "<<server_fd<<std::endl;
        //exit(EXIT_FAILURE);
    }
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
        perror("SET SOCKET OPT");
        exit(EXIT_FAILURE);
    }
    memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(40001);
    if (bind(server_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
		perror("bind");
		exit(EXIT_FAILURE);
	} 

	if (listen(server_fd, NCONNECTION) < 0){
		perror("listen");
		exit(EXIT_FAILURE);
	}
    while(1){
        std::cout<<"[QEMU Handler] RUN QEMU HANDLER"<<std::endl;
        // std::this_thread::sleep_for(std::chrono::milliseconds{300});
		if ((client_fd = accept(server_fd, (struct sockaddr*)&client_addr, (socklen_t*)&addrlen)) < 0){
			perror("accept");
        	exit(EXIT_FAILURE);
		}
		std::string json = "";\
        int njson;
		size_t ljson;

		recv(client_fd , &ljson, sizeof(ljson), 0);
        char buffer[ljson];
        memset(buffer, 0, ljson);
		while(1) {
			if ((njson = recv(client_fd, buffer, BUFF_SIZE-1, 0)) == -1) {
				perror("read");
				exit(1);
			}
			ljson -= njson;
		    buffer[njson] = '\0';
			json += buffer;

		    if (ljson == 0)
				break;
		}
        
         std::cout<<"[QEMU Handler] RECEV JSON : \n"<<json<<std::endl;
         // std::this_thread::sleep_for(std::chrono::milliseconds{300});
         BlockResult block = BlockResult(json.c_str());
         int newID = block.query_id * 100 + block.work_id;
         rowCountMap[newID]+= block.row_count;
         std::cout<<"[QEMU Handler] Query,Work ID: "<<newID<<", CURRENT BLOCK COUNT : "<<rowCountMap[newID]<<std::endl;
         // std::this_thread::sleep_for(std::chrono::milliseconds{300});
         if(rowCountMap[newID] >= 100000 || rowCountMap[newID] == 5 || rowCountMap[newID] == 25 || rowCountMap[newID] == 1250){
            resultMap[newID] = block;
         }
        // send(client_fd, cMsg, strlen(cMsg), 0);

		// char data[BUFF_SIZE];
        // char* dataiter = data;
		// memset(data, 0, BUFF_SIZE);
        // int ndata = 0;
        // size_t ldata = 0;
        // recv(client_fd , &ldata, sizeof(ldata),0);
        // std::cout<<"RECEV LDATA : "<<ldata<<std::endl;
		// while(1) {
		// 	if ((ndata = recv(client_fd , dataiter, ldata,0)) == -1) {
		// 		perror("read");
		// 		exit(1);
		// 	}
        //     dataiter += ndata;
		// 	ldata -= ndata;
        //     std::cout<<"LEFT ldata : "<<ldata<<std::endl;
		//     if (ldata == 0)
		// 		break;
		// }
        // std::cout<<"DATA ITER : "<<data<<std::endl;
        // send(client_fd, cMsg, strlen(cMsg), 0);
        // PushQueue(BlockResult(json.c_str(), data));
        close(client_fd);		
	}   
}

//

double QEMUHandler::calculateQEMU(int queryID, int workID){
    int returnCount = 0;
    while(1){
        int newID = 100 * queryID + workID;
        std::cout<<"[QEMU Handler] SEEKING ID "<<newID<<std::endl;
        if(resultMap.find(newID) != resultMap.end()){
            return resultMap[newID].executionTime;
            break;
        }
        
        else{
            returnCount++;
            // std::this_thread::sleep_for(std::chrono::milliseconds{300});
        }

        if(returnCount >= 3){
            return resultMap[newID].executionTime;
            break;
        }
    }
}
