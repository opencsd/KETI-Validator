ipcrm --all=msg

start=`date +%s.%N`

#### CSD Query Processing
for ((n=0; n<50; n++))
do
    ./runquery 1 4 &
    # ./runquery 1 6 &
done 

WORK_PID=`jobs -l | awk '{print $2}'`
wait $WORK_PID

finish=`date +%s.%N`
ptime=$( echo "$finish - $start" | bc -l )

echo 
echo "모든 명령이 종료되었습니다"

echo ""
echo "=> All Query End Time : $ptime sec"
echo ""
