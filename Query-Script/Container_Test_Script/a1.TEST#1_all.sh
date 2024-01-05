ipcrm --all=msg

start=`date +%s.%N`
curl -X GET -H "Content-Type: application/json" http://10.0.5.120:11111/start

pid1=`ps -aux | grep /offloading-container | awk '{print $2}' | head -1`
pid2=`ps -aux | grep /merging-container | awk '{print $2}' | head -1`
pid3=`ps -aux | grep /monitoring-container | awk '{print $2}' | head -1`
pid4=`ps -aux | grep /interface-container | awk '{print $2}' | head -1`

utime1=`cat /proc/$pid1/stat | awk '{print $14}'`
stime1=`cat /proc/$pid1/stat | awk '{print $14}'`

utime2=`cat /proc/$pid2/stat | awk '{print $14}'`
stime2=`cat /proc/$pid2/stat | awk '{print $14}'`

utime3=`cat /proc/$pid3/stat | awk '{print $14}'`
stime3=`cat /proc/$pid3/stat | awk '{print $14}'`

utime4=`cat /proc/$pid4/stat | awk '{print $14}'`
stime4=`cat /proc/$pid4/stat | awk '{print $14}'`

before=0
temp=$(expr $utime1 + $stime1 )
before=$(expr $before + $temp)

temp=$(expr $utime2 + $stime2 )
before=$(expr $before + $temp)

temp=$(expr $utime3 + $stime3 )
before=$(expr $before + $temp)

temp=$(expr $utime4 + $stime4 )
before=$(expr $before + $temp)


#### CSD Query Processing
./runquery 1 6


finish=`date +%s.%N`
ptime=$( echo "$finish - $start" | bc -l )
curl -X GET -H "Content-Type: application/json" http://10.0.5.120:11111/returncsd
# throughput=$( echo "scale=2; $size/$ptime" |bc )

utime1=`cat /proc/$pid1/stat | awk '{print $14}'`
stime1=`cat /proc/$pid1/stat | awk '{print $14}'`

utime2=`cat /proc/$pid2/stat | awk '{print $14}'`
stime2=`cat /proc/$pid2/stat | awk '{print $14}'`

utime3=`cat /proc/$pid3/stat | awk '{print $14}'`
stime3=`cat /proc/$pid3/stat | awk '{print $14}'`

utime4=`cat /proc/$pid4/stat | awk '{print $14}'`
stime4=`cat /proc/$pid4/stat | awk '{print $14}'`

after=0
temp=$(expr $utime1 + $stime1 )
after=$(expr $after + $temp)

temp=$(expr $utime2 + $stime2 )
after=$(expr $after + $temp)

temp=$(expr $utime3 + $stime3 )
after=$(expr $after + $temp)

temp=$(expr $utime4 + $stime4 )
after=$(expr $after + $temp)

echo ""
echo "=> Data Processing Time : $ptime sec"
# echo "=> CSD Data Throughput : $throughput (MB/s)"
echo "=> CSD CPU USAGE : $(expr $after - $before) (clock ticks)"
echo ""
