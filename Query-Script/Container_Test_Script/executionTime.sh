ipcrm --all=msg
start=`date +%s.%N`

#### CSD Query Processing
./runquery 1 $1

finish=`date +%s.%N`
diff=$( echo "$finish - $start" | bc -l )
echo ""
echo "=> CSD Query Processing Time : $diff (sec)"
echo ""
