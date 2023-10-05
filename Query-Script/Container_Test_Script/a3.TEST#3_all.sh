ipcrm --all=msg
size=7.48

start=`date +%s.%N`
curl -X GET -H "Content-Type: application/json" http://10.0.5.120:11111/start

pid1=`ps -aux | grep /storage-engine-container | awk '{print $2}' | head -1`

utime1=`cat /proc/$pid1/stat | awk '{print $14}'`
stime1=`cat /proc/$pid1/stat | awk '{print $14}'`

before=0
temp=$(expr $utime1 + $stime1 )
before=$(expr $before + $temp)


#### CSD Query Processing
./runquery 1 4


finish=`date +%s.%N`
ptime=$( echo "$finish - $start" | bc -l )
curl -X GET -H "Content-Type: application/json" http://10.0.5.120:11111/returncsd
# throughput=$( echo "scale=2; $size/$ptime" |bc )

utime1=`cat /proc/$pid1/stat | awk '{print $14}'`
stime1=`cat /proc/$pid1/stat | awk '{print $14}'`

after=0
temp=$(expr $utime1 + $stime1 )
after=$(expr $after + $temp)

echo ""
echo "=> Data Processing Time : $ptime sec"
# echo "=> CSD Data Throughput : $throughput (MB/s)"
echo "=> CSD CPU USAGE : $(expr $after - $before) (clock ticks)"
echo ""
