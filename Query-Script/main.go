package main

import (
	"bytes"
	"encoding/json"
	"fmt"
	"io/ioutil"
	"net/http"
	"os"
	"strconv"
	"strings"
	"time"
	"bufio"
	"os/exec"
	"math"
	"log"
	 "github.com/shirou/gopsutil/cpu"
	 "github.com/shirou/gopsutil/disk"
	 "github.com/shirou/gopsutil/mem"
	"github.com/shirou/gopsutil/net"
	"runtime/pprof"

)

type Query struct {
	Name string `json:"query"`
}

func main() {
	if len(os.Args) < 2 {
		fmt.Println("쿼리 번호를 입력해주세요")
	}
	querynum := os.Args[2]
	querytype := os.Args[1]
	// fmt.Println(querynum)
	var querydata Query

	//fmt.Println("Query : TPC-H", querynum, "Start")
	if querytype == "0" {
		switch querynum {
		case "1":
			querydata = Query{`SELECT l_returnflag,
		l_linestatus,
		SUM(l_quantity)                                           AS sum_qty,
		SUM(l_extendedprice)                                      AS
		sum_base_price,
		SUM(l_extendedprice * ( 1 - l_discount ))                 AS
		sum_disc_price,
		SUM(l_extendedprice * ( 1 - l_discount ) * ( 1 + l_tax )) AS sum_charge,
		Avg(l_quantity)                                           AS avg_qty,
		Avg(l_extendedprice)                                      AS avg_price,
		Avg(l_discount)                                           AS avg_disc,
		Count(*)                                                  AS count_order
 FROM   lineitem
 WHERE  l_shipdate <= DATE ('1998-12-01') - interval '108' day
 GROUP  BY l_returnflag,
		   l_linestatus
 ORDER  BY l_returnflag,
		   l_linestatus;`}
		case "2":
			querydata = Query{`select 
		s_acctbal, 
		s_name, 
		n_name, 
		p_partkey, 
		p_mfgr, 
		s_address, 
		s_phone, 
		s_comment 
	from 
		part, 
		supplier, 
		partsupp, 
		nation, 
		region 
	where 
		p_partkey = ps_partkey and 
		s_suppkey = ps_suppkey and 
		p_size = 30 and 
		p_type like '%STEEL' and 
		s_nationkey = n_nationkey and
		n_regionkey = r_regionkey and 
		r_name = 'ASIA' and 
		ps_supplycost = (
			select 
				min(ps_supplycost) 
			from 
				partsupp, 
				supplier, 
				nation, 
				region 
			where 
				p_partkey = ps_partkey and 
				s_suppkey = ps_suppkey and 
				s_nationkey = n_nationkey and 
				n_regionkey = r_regionkey and 
				r_name = 'ASIA'
		) 
	order by 
		s_acctbal desc, 
		n_name, 
		s_name, 
		p_partkey 
	limit 100;
	`}
		case "3":
			querydata = Query{`SELECT   l_orderkey,
		Sum(l_extendedprice * (1 - l_discount)) AS revenue,
		o_orderdate,
		o_shippriority
FROM     customer,
		orders,
		lineitem
WHERE    c_mktsegment = 'AUTOMOBILE'
AND      c_custkey = o_custkey
AND      l_orderkey = o_orderkey
AND      o_orderdate < date '1995-03-13'
AND      l_shipdate >  date '1995-03-13'
GROUP BY l_orderkey,
		o_orderdate,
		o_shippriority
ORDER BY revenue DESC,
		o_orderdate
LIMIT    10;`}
		case "4":
			querydata = Query{`select 
		o_orderpriority, count(*) as order_count 
	from 
		orders 
	where 
		o_orderdate >= date '1995-01-01' and 
		o_orderdate < date '1995-01-01' + interval '3' month and 
		exists (
			select 
				* 
			from 
				lineitem 
			where 
				l_orderkey = o_orderkey and 
				l_commitdate < l_receiptdate
			) 
	group by 
		o_orderpriority 
	order by 
		o_orderpriority;
	`}
			//time.Sleep(18500 * time.Millisecond)
		case "5":
			querydata = Query{`select 
		n_name, 
		sum(l_extendedprice * (1 - l_discount)) as revenue 
	from 
		customer, 
		orders, 
		lineitem, 
		supplier, 
		nation, 
		region 
	where 
		c_custkey = o_custkey and 
		l_orderkey = o_orderkey and 
		l_suppkey = s_suppkey and 
		c_nationkey = s_nationkey and 
		s_nationkey = n_nationkey and 
		n_regionkey = r_regionkey and 
		r_name = 'MIDDLE EAST' and 
		o_orderdate >= date '1994-01-01' and 
		o_orderdate < date '1994-01-01' + interval '1' year
	group by 
		n_name 
	order by 
		revenue desc;
	
	`}
		case "6":
			querydata = Query{`
		select
			sum(l_extendedprice * l_discount) as revenue
		from
			lineitem
		where
			l_shipdate >= date ('1994-01-01')
			and l_shipdate < date ('1994-01-01') + interval '1' year
			and l_discount between 0.06 - 0.01 and 0.06 + 0.01
			and l_quantity < 24;
		`}
		case "7":
			querydata = Query{`select 
		supp_nation, 
		cust_nation, 
		l_year, 
		sum(volume) as revenue 
	from ( 
		select 
			n1.n_name as supp_nation, 
			n2.n_name as cust_nation, 
			extract(year from l_shipdate) as l_year, 
			l_extendedprice * (1 - l_discount) as volume 
		from 
			supplier, 
			lineitem, 
			orders, 
			customer, 
			nation n1, 
			nation n2 
		where 
			s_suppkey = l_suppkey and 
			o_orderkey = l_orderkey and 
			c_custkey = o_custkey and 
			s_nationkey = n1.n_nationkey and
			c_nationkey = n2.n_nationkey and 
			(
				(n1.n_name = 'JAPAN' and n2.n_name = 'INDIA') or 
				(n1.n_name = 'INDIA' and n2.n_name = 'JAPAN')
			) and 
			l_shipdate between date '1995-01-01' and date '1996-12-31'
		) as shipping 
	group by 
		supp_nation, 
		cust_nation, 
		l_year 
	order by 
		supp_nation, 
		cust_nation, 
		l_year;
	`}
		case "8":
			querydata = Query{`select 
		o_year, 
		sum(case when nation = 'INDIA' then volume else 0 end) / sum(volume) as mkt_share 
	from (
		select 
			extract(year from o_orderdate) as o_year,	
			l_extendedprice * (1 - l_discount) as volume, 
			n2.n_name as nation 
		from 
			part,
			supplier,
			lineitem,
			orders,
			customer,
			nation n1,
			nation n2,
			region
		where 
			p_partkey = l_partkey and 
			s_suppkey = l_suppkey and 
			l_orderkey = o_orderkey and 
			o_custkey = c_custkey and 
			c_nationkey = n1.n_nationkey and 
			n1.n_regionkey = r_regionkey and 
			r_name = 'ASIA'	and 
			s_nationkey = n2.n_nationkey and 
			o_orderdate between date '1995-01-01' and date '1996-12-31'and 
			p_type = 'SMALL PLATED COPPER'
		) as all_nations 
	group by 
		o_year 
	order by 
		o_year;`}
		case "9":
			querydata = Query{`select 
		nation, 
		o_year, 
		sum(amount) as sum_profit 
	from (
		select 
			n_name as nation, 
			extract(year from o_orderdate) as o_year, 
			l_extendedprice * (1 - l_discount) - ps_supplycost * l_quantity as amount 
		from 
			part, 
			supplier, 
			lineitem, 
			partsupp, 
			orders, 
			nation 
		where 
			s_suppkey = l_suppkey and 
			ps_suppkey = l_suppkey and 
			ps_partkey = l_partkey and 
			p_partkey = l_partkey and 
			o_orderkey = l_orderkey and 
			s_nationkey = n_nationkey and 
			p_name like '%dim%'
		) as profit 
	group by 
		nation, 
		o_year 
	order by 
		nation, 
		o_year desc;`}
		case "10":
			querydata = Query{`select c_custkey,
		c_name,
		sum(l_extendedprice * (1 - l_discount)) as revenue,
		c_acctbal,
		n_name,
		c_address,
		c_phone,
		c_comment
	from
		customer,
		orders,
		lineitem,
		nation
	where
		c_custkey = o_custkey
		and l_orderkey = o_orderkey
		and o_orderdate >= date '1993-08-01'
		and o_orderdate < date '1993-08-01' + interval '3' month
		and l_returnflag = 'R'
		and c_nationkey = n_nationkey
	group by
		c_custkey,
		c_name,
		c_acctbal,
		c_phone,
		n_name,
		c_address,
		c_comment
	order by
		revenue desc
	limit 20;
	`}
		case "11":
			querydata = Query{`SELECT ps_partkey,
		Sum(ps_supplycost * ps_availqty) AS value
 FROM   partsupp,
		supplier,
		nation
 WHERE  ps_suppkey = s_suppkey
		AND s_nationkey = n_nationkey
		AND n_name = 'MOZAMBIQUE'
 GROUP  BY ps_partkey
 HAVING Sum(ps_supplycost * ps_availqty) > (SELECT
		Sum(ps_supplycost * ps_availqty) * 0.0001000000
											FROM   partsupp,
												   supplier,
												   nation
											WHERE  ps_suppkey = s_suppkey
												   AND s_nationkey = n_nationkey
												   AND n_name = 'MOZAMBIQUE')
 ORDER  BY value DESC; `}
		case "12":
			querydata = Query{`SELECT l_shipmode,
		SUM(CASE
			  WHEN o_orderpriority = '1-urgent'
					OR o_orderpriority = '2-high' THEN 1
			  ELSE 0
			END) AS high_line_count,
		SUM(CASE
			  WHEN o_orderpriority <> '1-urgent'
				   AND o_orderpriority <> '2-high' THEN 1
			  ELSE 0
			END) AS low_line_count
 ROFM   orders,
		lineitem
 WHERE  o_orderkey = l_orderkey
		AND l_shipmode IN ( 'RAIL', 'FOB' )
		AND l_commitdate < l_receiptdate
		AND l_shipdate < l_commitdate
		AND l_receiptdate >= DATE '1997-01-01'
		AND l_receiptdate < DATE '1997-01-01' + interval '1' year
 GROUP  BY l_shipmode
 ORDER  BY l_shipmode; `}
		case "13":
			querydata = Query{`SELECT c_count,
		Count(*) AS custdist
 FROM   (SELECT c_custkey,
				Count(o_orderkey) AS c_count
		 FROM   customer
				LEFT OUTER JOIN orders
							 ON c_custkey = o_custkey
								AND o_comment NOT LIKE '%PENDING%DEPOSITS%'
		 GROUP  BY c_custkey) c_orders
 GROUP  BY c_count
 ORDER  BY custdist DESC,
		   c_count DESC; `}
		case "14":
			querydata = Query{`SELECT 100.00 * SUM(CASE
			WHEN p_type LIKE 'PROMO%' THEN l_extendedprice *
										   ( 1 - l_discount )
			ELSE 0
		  END) / SUM(l_extendedprice * ( 1 - l_discount )) AS
promo_revenue
FROM   lineitem,
part
WHERE  l_partkey = p_partkey
AND l_shipdate >= DATE '1996-12-01'
AND l_shipdate < DATE '1996-12-01' + interval '1' month; `}
		case "15":
			querydata = Query{`CREATE VIEW revenue0
		(supplier_no, total_revenue)
		AS
		  SELECT l_suppkey,
				 SUM(l_extendedprice * ( 1 - l_discount ))
		  FROM   lineitem
		  WHERE  l_shipdate >= DATE '1997-07-01'
				 AND l_shipdate < DATE '1997-07-01' + interval '3' month
		  GROUP  BY l_suppkey;
		
		SELECT s_suppkey,
			   s_name,
			   s_address,
			   s_phone,
			   total_revenue
		FROM   supplier,
			   revenue0
		WHERE  s_suppkey = supplier_no
			   AND total_revenue = (SELECT Max(total_revenue)
									FROM   revenue0)
		ORDER  BY s_suppkey;
		
		DROP VIEW revenue0; `}
		case "16":
			querydata = Query{`SELECT p_brand,
		p_type,
		p_size,
		Count(DISTINCT ps_suppkey) AS supplier_cnt
 FROM   partsupp,
		part
 WHERE  p_partkey = ps_partkey
		AND p_brand <> 'Brand#34'
		AND p_type NOT LIKE 'LARGE BRUSHED%'
		AND p_size IN ( 48, 19, 12, 4,
						41, 7, 21, 39 )
		AND ps_suppkey NOT IN (SELECT s_suppkey
							   FROM   supplier
							   WHERE  s_comment LIKE '%CUSTOMER%COMPLAINTS%')
 GROUP  BY p_brand,
		   p_type,
		   p_size
 ORDER  BY supplier_cnt DESC,
		   p_brand,
		   p_type,
		   p_size; `}
		case "17":
			querydata = Query{`SELECT SUM(l_extendedprice) / 7.0 AS avg_yearly
		FROM   lineitem,
			   part
		WHERE  p_partkey = l_partkey
			   AND p_brand = 'Brand#44'
			   AND p_container = 'WRAP PKG'
			   AND l_quantity < (SELECT 0.2 * AVG(l_quantity)
								 FROM   lineitem
								 WHERE  l_partkey = p_partkey); `}
		case "18":
			querydata = Query{`SELECT c_name,
		c_custkey,
		o_orderkey,
		o_orderdate,
		o_totalprice,
		Sum(l_quantity)
 FROM   customer,
		orders,
		lineitem
 WHERE  o_orderkey IN (SELECT l_orderkey
					   FROM   lineitem
					   GROUP  BY l_orderkey
					   HAVING Sum(l_quantity) > 314)
		AND c_custkey = o_custkey
		AND o_orderkey = l_orderkey
 GROUP  BY c_name,
		   c_custkey,
		   o_orderkey,
		   o_orderdate,
		   o_totalprice
 ORDER  BY o_totalprice DESC,
		   o_orderdate
 LIMIT  100; `}
		case "19":
			querydata = Query{`SELECT Sum(l_extendedprice * ( 1 - l_discount )) AS revenue
		FROM   lineitem,
			   part
		WHERE  ( p_partkey = l_partkey
				 AND p_brand = 'Brand#52'
				 AND p_container IN ( 'SM CASE', 'SM BOX', 'SM PACK', 'SM PKG' )
				 AND l_quantity >= 4
				 AND l_quantity <= 4 + 10
				 AND p_size BETWEEN 1 AND 5
				 AND l_shipmode IN ( 'AIR', 'AIR REG' )
				 AND l_shipinstruct = 'DELIVER IN PERSON' )
				OR ( p_partkey = l_partkey
					 AND p_brand = 'Brand#11'
					 AND p_container IN ( 'MED bag', 'MED BOX', 'MED PKG', 'MED PACK' )
					 AND l_quantity >= 18
					 AND l_quantity <= 18 + 10
					 AND p_size BETWEEN 1 AND 10
					 AND l_shipmode IN ( 'AIR', 'AIR REG' )
					 AND l_shipinstruct = 'DELIVER IN PERSON' )
				OR ( p_partkey = l_partkey
					 AND p_brand = 'Brand#51'
					 AND p_container IN ( 'LG case', 'LG BOX', 'LG PACK', 'LG PKG' )
					 AND l_quantity >= 29
					 AND l_quantity <= 29 + 10
					 AND p_size BETWEEN 1 AND 15
					 AND l_shipmode IN ( 'AIR', 'AIR REG' )
					 AND l_shipinstruct = 'DELIVER IN PERSON' ); `}
		case "20":
			querydata = Query{`SELECT s_name,
		s_address
 FROM   supplier,
		nation
 WHERE  s_suppkey IN (SELECT ps_suppkey
					  FROM   partsupp
					  WHERE  ps_partkey IN (SELECT p_partkey
											FROM   part
											WHERE  p_name LIKE 'green%')
							 AND ps_availqty > (SELECT 0.5 * SUM(l_quantity)
												FROM   lineitem
												WHERE  l_partkey = ps_partkey
													   AND l_suppkey = ps_suppkey
													   AND l_shipdate >= DATE
														   '1993-01-01'
													   AND l_shipdate < DATE
														   '1993-01-01' +
														   interval
																		'1' year
											   ))
		AND s_nationkey = n_nationkey
		AND n_name = 'ALGERIA'
 ORDER  BY s_name; `}
		case "21":
			querydata = Query{`SELECT s_name,
		Count(*) AS numwait
 FROM   supplier,
		lineitem l1,
		orders,
		nation
 WHERE  s_suppkey = l1.l_suppkey
		AND o_orderkey = l1.l_orderkey
		AND o_orderstatus = 'F'
		AND l1.l_receiptdate > l1.l_commitdate
		AND EXISTS (SELECT *
					FROM   lineitem l2
					WHERE  l2.l_orderkey = l1.l_orderkey
						   AND l2.l_suppkey <> l1.l_suppkey)
		AND NOT EXISTS (SELECT *
						FROM   lineitem l3
						WHERE  l3.l_orderkey = l1.l_orderkey
							   AND l3.l_suppkey <> l1.l_suppkey
							   AND l3.l_receiptdate > l3.l_commitdate)
		AND s_nationkey = n_nationkey
		AND n_name = 'EGYPT'
 GROUP  BY s_name
 ORDER  BY numwait DESC,
		   s_name
 LIMIT  100; `}
		case "22":
			querydata = Query{`SELECT cntrycode,
		Count(*)       AS numcust,
		Sum(c_acctbal) AS totacctbal
 FROM   (SELECT Substring(c_phone FROM 1 FOR 2) AS cntrycode,
				c_acctbal
		 FROM   customer
		 WHERE  Substring(c_phone FROM 1 FOR 2) IN ( '20', '40', '22', '30',
													 '39', '42', '21' )
				AND c_acctbal > (SELECT Avg(c_acctbal)
								 FROM   customer
								 WHERE  c_acctbal > 0.00
										AND Substring(c_phone FROM 1 FOR 2) IN (
											'20', '40', '22', '30',
											'39', '42', '21' ))
				AND NOT EXISTS (SELECT *
								FROM   orders
								WHERE  o_custkey = c_custkey)) AS custsale
 GROUP  BY cntrycode
 ORDER  BY cntrycode; `}
		case "23":
			querydata = Query{`select
			c_custkey, c_name, n_name
			from
				customer, nation
			where
				c_nationkey = n_nationkey and
					c_custkey < 10;`}

		}

	} else if querytype == "1" {
		intquerynum, _ := strconv.Atoi(querynum)
		if intquerynum > 9 {
			querydata = Query{"TPC-H_" + querynum}
		} else {
			querydata = Query{"TPC-H_0" + querynum}
		}
	} else {
		if querynum == "0" {
			querydata = Query{`select
			count(l_extendedprice) as count_all
		from
			lineitem
		where
			l_shipdate >= date ('1994-01-01')
			and l_shipdate < date ('1994-01-01') + interval '1' year
			and l_discount between 0.06 - 0.01 and 0.06 + 0.01
			and l_quantity < 24;`}
			// time.Sleep(650 * time.Millisecond)
		} else {
			querydata = Query{"TEST_01"}
		}
	}
	
	//startTime := time.Now()
	fmt.Println(querydata)
	weight := 0.38
	weight_per_scan := 4.55
	weight_per_column := 0.51713
	weight_per_sum := 22.71849
	learning_rate := 0.05
	durationRecord := []float64{}
	csdworkRecord := []float64{}
	errRecord := []float64{}
	errRecordcsd := []float64{}
	errRecordstor := []float64{}	
	errRecordPer10 := []float64{}
	errRecordPer10csd := []float64{}
	errRecordPer10stor := []float64{}
	ak := 85 / (1.9*1.9*1.9)
	fk := ak * (0.8*0.8*0.8)
	
    f, err := os.Create("cpu.prof")
    if err != nil {
        log.Fatal(err)
    }
    defer f.Close()

	for i:= 0; i < 50; i++{
	fmt.Printf("Send Query %d times\n",i+1)

	filePath := "/sys/class/powercap/intel-rapl:0:0/energy_uj"

	// Read the content of the file
	content, err := ioutil.ReadFile(filePath)
	if err != nil {
		log.Fatal(err)
	}

	energyStr := strings.TrimSpace(string(content))
	energyBefore1, err := strconv.ParseUint(energyStr, 10, 64)
	if err != nil {
		log.Fatal(err)
	}

	fmt.Printf("Energy consumed: %d microjoules\n", energyBefore1)

	filePath = "/sys/class/powercap/intel-rapl:1:0/energy_uj"

	// Read the content of the file
	content, err = ioutil.ReadFile(filePath)
	if err != nil {
		log.Fatal(err)
	}

	energyStr = strings.TrimSpace(string(content))
	energyBefore2, err := strconv.ParseUint(energyStr, 10, 64)
	if err != nil {
		log.Fatal(err)
	}

	fmt.Printf("Energy consumed: %d microjoules\n", energyBefore2)

	//monitoring start
	done := make(chan struct{})
	defer close(done)
	usageCh := make(chan Usage, 1)
	go monitorUsage(done, usageCh)


	out, err := exec.Command("sensors").Output()
	if err != nil {
		panic(err)
	}
	powercon := float64(0)
	scanner := bufio.NewScanner(strings.NewReader(string(out)))
	for scanner.Scan() {
		line := scanner.Text()
		if strings.HasPrefix(line, "power1:") {
			fields := strings.Fields(line)
			if len(fields) < 2 {
				continue
			}
			powerStr := fields[1]
			if powerStr == "" {
				continue
			}
			power, err := strconv.ParseFloat(powerStr, 64)
			if err != nil {
				panic(err)
			}
			
			fmt.Printf("Total Power At Start %.2f W\n",power)
			fmt.Printf("normal dynamic Power At Start %.2f W\n", fk)
		}
	}
	powercon = 102;


	pbyte, _ := json.Marshal(querydata)
	buff := bytes.NewBuffer(pbyte)	
	
	// send query request and wait for response
	resp, err := http.Post("http://127.0.0.1:40000", "application/json", buff)
	if err != nil {
		panic(err)
	}
	defer resp.Body.Close()

	respBody, err := ioutil.ReadAll(resp.Body)
	if err != nil {
		panic(err)
	}
	output := (string(respBody))
	durationStr := strings.TrimSuffix(strings.Split(output, ":")[1], "s")
	duration, err := strconv.ParseFloat(strings.TrimSpace(durationStr), 64)
	fmt.Println("End Query Time : ", duration, "s")
	durationRecord = append(durationRecord,duration)
	// signal the monitor usage goroutine to stop
	done <- struct{}{}
	// output usage statistics
	usage := <-usageCh
	fmt.Printf("CPU usage    : %.2f %%\n", usage.CPU)
	fmt.Printf("Memory usage : %.2f %%\n", usage.Memory)
	fmt.Printf("Disk usage   : %.2f %%\n", usage.Disk)
	usageNet := usage.Network/1048576
	fmt.Printf("Network usage: %.2f MB\n", usageNet)
	
	storPower := usage.Power_simulator 
	dbConPower := usage.Power_db_connect
	csd1Power := usage.Power_csd_1
	csd2Power := usage.Power_csd_2
	csd3Power := usage.Power_csd_3
	csd4Power := usage.Power_csd_4
	csd5Power := usage.Power_csd_5
	csd6Power := usage.Power_csd_6
	csd7Power := usage.Power_csd_7
	csd8Power := usage.Power_csd_8
	csdWorkTime := usage.csdWorkTime
	PowerConumption := usage.Power
	fmt.Printf("Power Consumption while execute query: %.4f W*s\n",PowerConumption)
	Powerdiff := PowerConumption - (powercon * math.Round(duration *100)/100)
	fmt.Printf("System Power Difference: %.4f W*s\n\n",Powerdiff)
	
	//fmt.Printf("Host Power Consumption         :%.4f W*s\n",hostPower)
	fmt.Printf("Storage Power Consumption :%.4f W*s\n",storPower)
	//fmt.Printf("DBConnector Power Consumption  :%.4f W*s\n",dbConPower)
	fmt.Printf("CSD1 Power Consumption    :%.4f W*s\n",csd1Power)
	fmt.Printf("CSD2 Power Consumption    :%.4f W*s\n",csd2Power)
	fmt.Printf("CSD3 Power Consumption    :%.4f W*s\n",csd3Power)
	fmt.Printf("CSD4 Power Consumption    :%.4f W*s\n",csd4Power)
	fmt.Printf("CSD5 Power Consumption    :%.4f W*s\n",csd5Power)
	fmt.Printf("CSD6 Power Consumption    :%.4f W*s\n",csd6Power)
	fmt.Printf("CSD7 Power Consumption    :%.4f W*s\n",csd7Power)
	fmt.Printf("CSD8 Power Consumption    :%.4f W*s\n",csd8Power)
	fmt.Printf("CSD Working Time          :%.4f s\n\n",csdWorkTime)
	Total_Power := (dbConPower+storPower+csd1Power+csd2Power+csd3Power+csd4Power+csd5Power+csd6Power+csd7Power+csd8Power) * weight
	//fmt.Printf("Calculated CPU Power Consumption:%.4f W*s\n\n",Total_Power)
	csdworkRecord = append(csdworkRecord,csdWorkTime)
	filePath = "/sys/class/powercap/intel-rapl:0:0/energy_uj"

	// Read the content of the file
	content, err = ioutil.ReadFile(filePath)
	if err != nil {
		log.Fatal(err)
	}

	energyStr = strings.TrimSpace(string(content))
	energyAfter1, err := strconv.ParseUint(energyStr, 10, 64)
	if err != nil {
		log.Fatal(err)
	}
	filePath = "/sys/class/powercap/intel-rapl:1:0/energy_uj"

	// Read the content of the file
	content, err = ioutil.ReadFile(filePath)
	if err != nil {
		log.Fatal(err)
	}

	energyStr = strings.TrimSpace(string(content))
	energyAfter2, err := strconv.ParseUint(energyStr, 10, 64)
	if err != nil {
		log.Fatal(err)
	}

	//fmt.Printf("CPU0 Energy consumed After: %d microjoules\n", energyAfter1)
	energyConsume1 := float64(energyAfter1 - energyBefore1) / 1000000.0
	fmt.Printf("CPU0 Energy consumed After - Before : %.4f W*s\n",energyConsume1)

	//fmt.Printf("CPU1 Energy consumed After: %d microjoules\n", energyAfter2)
	
	energyConsume2 := float64(energyAfter2 - energyBefore2) / 1000000.0
	fmt.Printf("CPU1 Energy consumed After - Before : %.4f W*s\n\n",energyConsume2)
	fmt.Printf("Total CPU Energy consumed: %.4f W*s\n\n",energyConsume1 + energyConsume2)
	energyConsume2 = energyConsume2+1
	errorRate := ((energyConsume1 - Total_Power)) / (energyConsume1)
	weight = weight + learning_rate * errorRate * weight
	errorRate = math.Abs(errorRate)
	//fmt.Printf("error : %.5f\n", errorRate)
	errRecord = append(errRecord, errorRate)
	//fmt.Printf("Update Weight : %.5f\n\n", weight)

	csdtk := ((weight_per_column * 60000 * 9) + (weight_per_scan * 60000))/1300000 
	calculatedCsd := ((csdtk * ak * 1.3*1.3*1.3) + (fk * (duration-csdtk)))/4*weight

	stortk := (weight_per_sum * 60000 *8) / 1900000
	calculatedStor := ((stortk + csdtk) * ak * 1.9 * 1.9 * 1.9) / 6 * weight 
	errorCsd := (csd1Power-calculatedCsd) / csd1Power
	errorStor := (storPower - calculatedStor) / storPower
	weight_per_column = weight_per_column + learning_rate * errorCsd * weight_per_column
	weight_per_sum = weight_per_sum + learning_rate * errorStor * weight_per_sum

	errorCsd = math.Abs(errorCsd)
	errRecordcsd = append(errRecordcsd,errorCsd)
	errorStor = math.Abs(errorStor)
	errRecordstor = append(errRecordstor, errorStor)

	fmt.Printf("calculated CSD Power      : %.5f W*s\n", calculatedCsd)
	fmt.Printf("calculated Storage Power  : %.5f W*s\n\n", calculatedStor)

	fmt.Printf("error CSD                 : %.5f\n", errorCsd)
	fmt.Printf("error Storage             : %.5f\n", errorStor)
	fmt.Printf("Update Weight for CSD     : %.5f\n",weight_per_column)
	fmt.Printf("Update Weight for Storage : %.5f\n\n",weight_per_sum)
    if err := pprof.StartCPUProfile(f); err != nil {
        log.Fatal(err)
    }
    defer pprof.StopCPUProfile()
	if i % 10 == 9{

		//fmt.Printf("Average error when %d times of Query Execution: %.3f\n", i+1, avg(errRecord))
		errRecordPer10 = append(errRecordPer10,avg(errRecord))
		fmt.Printf("Average CSD error when %d times of Query Execution    : %.5f\n", i+1, avg(errRecordcsd))
		fmt.Printf("Average Storage error when %d times of Query Execution: %.5f\n\n", i+1, avg(errRecordstor))
		errRecordPer10csd = append(errRecordPer10csd,avg(errRecordcsd))
		errRecordPer10stor = append(errRecordPer10stor,avg(errRecordstor))
	}

	fmt.Printf("\n\nstart TPCH Query With CSD...\n")
	fmt.Printf("Query Done... Time -> %.2f s\n",duration-0.08)
	fmt.Printf("CSD Performed:\n")
	fmt.Printf("Read       : 9.1 (MB)\n")
	fmt.Printf("Read Speed : %.2f (MB/s)\n",9.1 / (duration-0.08))

	fmt.Println("sleep for 10 s\n--------------------------------------------")
	time.Sleep(5 * time.Second) 
	}

	// for j:=1; j<21;j++{
	// 	fmt.Printf("Average CSD weight when %d times of Query Execution: %.5f\n",j*10,errRecordPer10csd[j-1])
	// 	fmt.Printf("Average Storage weight when %d times of Query Execution: %.5f\n\n",j*10,errRecordPer10stor[j-1])
	// }
	fmt.Printf("Average CSD error for 200 times of Query Execute    : %.5f\n",avg(errRecordcsd))
	fmt.Printf("Average Storage error for 200 times of Query Execute: %.5f\n",avg(errRecordstor))
	fmt.Printf("CSD Weight Result     : %.5f\n",weight_per_column)
	fmt.Printf("Storage Weight Result : %.5f\n",weight_per_sum)
	fmt.Printf("Average Storage Working Time: %.4f\n",avg(durationRecord))
	fmt.Printf("Average CSD Working Time: %.4f\n",avg(csdworkRecord))
}

type Usage struct {
	CPU     float64
	Memory  float64
	Disk    float64
	Network float64
	Power 	float64
	Power_host		float64
	Power_simulator	float64
	Power_db_connect 	float64
	Power_csd_1 	float64
	Power_csd_2 	float64
	Power_csd_3 	float64
	Power_csd_4 	float64
	Power_csd_5 	float64
	Power_csd_6 	float64
	Power_csd_7 	float64
	Power_csd_8 	float64
	csdWorkTime 	float64
}

func monitorUsage(done <-chan struct{}, usageCh chan<- Usage) {

	ticker := time.NewTicker(20 * time.Millisecond)
	defer ticker.Stop()

	cpuUsage := []float64{}
	memUsage := []float64{}
	diskUsage := []float64{}
	powerUsage := []float64{}
	powerHost := []float64{}
	powerSimulator := []float64{}
	powerDbConnect := []float64{}
	powerCsd1 := []float64{}
	powerCsd2 := []float64{}
	powerCsd3 := []float64{}
	powerCsd4 := []float64{}
	powerCsd5 := []float64{}
	powerCsd6 := []float64{}
	powerCsd7 := []float64{}
	powerCsd8 := []float64{}
	csdWork := []float64{}
	//loopTimeCheck := []float64{}
	prevnet := float64(0)
	count1 := 0
	count2 := 0
	count3 := 0.0
	count4 := 0
	count5 := 0
	count6 := 0

	netInfo, _ := net.IOCounters(false)
    prevnet = float64(netInfo[0].BytesRecv + netInfo[0].BytesSent)

	for {
		select {
		case <-done:
			netInfo, _ := net.IOCounters(false)
            netUsage := float64(netInfo[0].BytesRecv+netInfo[0].BytesSent) - prevnet
			csdWork = append(csdWork,count3*0.016)
			usageCh <- Usage{
				CPU:     avg(cpuUsage),
				Memory:  avg(memUsage),
				Disk:    avg(diskUsage),
				Power: 	 sum(powerUsage),
				Power_host:	 sum(powerHost),
				Power_simulator: 	 sum(powerSimulator),
				Power_db_connect:   sum(powerDbConnect),
				Power_csd_1: sum(powerCsd1)*0.5,
				Power_csd_2: sum(powerCsd2)*0.5,
				Power_csd_3: sum(powerCsd3)*0.5,
				Power_csd_4: sum(powerCsd4)*0.5,
				Power_csd_5: sum(powerCsd5)*0.5,
				Power_csd_6: sum(powerCsd6)*0.5,
				Power_csd_7: sum(powerCsd7)*0.5,
				Power_csd_8: sum(powerCsd8)*0.5,
				Network: netUsage,
				csdWorkTime: avg(csdWork),
			}
			return
		case <-ticker.C:

			 //loopStart := time.Now()
			cpuPercent, _ := cpu.Percent(0, false)
			cpuUsage = append(cpuUsage, cpuPercent[0])

			 memInfo, _ := mem.VirtualMemory()
			 memUsage = append(memUsage, float64(memInfo.Used)/float64(memInfo.Total)*100)

			 diskInfo, _ := disk.Usage("/")
			 diskUsage = append(diskUsage, float64(diskInfo.Used)/float64(diskInfo.Total)*100)
			out, err := exec.Command("sensors").Output()
			if err != nil {
				panic(err)
			}

			scanner := bufio.NewScanner(strings.NewReader(string(out)))
    		for scanner.Scan() {
				line := scanner.Text()
				if strings.HasPrefix(line, "power1:") {
					fields := strings.Fields(line)
					if len(fields) < 2 {
						continue
					}
					powerStr := fields[1]
					if powerStr == "" {
						continue
					}
					power, err := strconv.ParseFloat(powerStr, 64)
					if err != nil {
						panic(err)
					}
					power = power * 0.016
					powerUsage = append(powerUsage, power)
				}
			}
		// }()
		paths := []string{
			"/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq",
			"/sys/devices/system/cpu/cpu1/cpufreq/scaling_cur_freq",
			"/sys/devices/system/cpu/cpu2/cpufreq/scaling_cur_freq",
			"/sys/devices/system/cpu/cpu3/cpufreq/scaling_cur_freq",
			"/sys/devices/system/cpu/cpu4/cpufreq/scaling_cur_freq",
			"/sys/devices/system/cpu/cpu5/cpufreq/scaling_cur_freq",
		}

		for _, path := range paths {
			go func(p string) {
	
				freq, err := waitForFileAndReadFrequency(p)
				if err != nil {
					panic(err)
				}
				switch p {
				case "/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq":
					powerSimulator = append(powerSimulator, calculateFreqPower(freq))
					//fmt.Printf("Execution time of one loop: %.3f\n", freq)
					count1 = count1+1
				case "/sys/devices/system/cpu/cpu1/cpufreq/scaling_cur_freq":
					powerDbConnect = append(powerDbConnect, calculateFreqPower(freq))
					count2 = count2+1
				case "/sys/devices/system/cpu/cpu2/cpufreq/scaling_cur_freq":
					powerCsd1 = append(powerCsd1, calculateFreqPower(freq))
					powerCsd2 = append(powerCsd2, calculateFreqPower(freq))
					if(freq > 1){
						count3 = count3+1
					}
				case "/sys/devices/system/cpu/cpu3/cpufreq/scaling_cur_freq":
					powerCsd3 = append(powerCsd3, calculateFreqPower(freq))
					powerCsd4 = append(powerCsd4, calculateFreqPower(freq))
					count4 = count4+1
				case "/sys/devices/system/cpu/cpu4/cpufreq/scaling_cur_freq":
					powerCsd5 = append(powerCsd5, calculateFreqPower(freq))
					powerCsd6 = append(powerCsd6, calculateFreqPower(freq))
					count5 = count5+1
				case "/sys/devices/system/cpu/cpu5/cpufreq/scaling_cur_freq":
					powerCsd7 = append(powerCsd7, calculateFreqPower(freq))
					powerCsd8 = append(powerCsd8, calculateFreqPower(freq))
					count6 = count6+1
				//case "/sys/devices/system/cpu/cpu/cpufreq/scaling_cur_freq":
					//powerHost = append(powerHost, calculateFreqPower(freq))
				}
			}(path)

 //duration := float64(time.Since(loopStart).Seconds())
//fmt.Printf("Execution time of one loop  : %f s\n", duration)
		}
	}
}
}

func max(values []float64) float64{
	max := 0.0
	for _, v:= range values{
		if v > max{
			max = v
		}
	}
	return max
}

func avg(values []float64) float64 {
	total := 0.0
	for _, v := range values {
		total += v
	}
	return total / float64(len(values))
}

func sum(values []float64) float64{
	total := 0.0
	for _, v := range values{
		total += v
	}
	return total
}

func waitForFileAndReadFrequency(path string) (float64, error) {
	for {
		_, err := os.Stat(path)
		if err == nil {
			// File path exists, proceed with reading the frequency
			break
		}

		// File path does not exist yet, wait for a short duration
		time.Sleep(500 * time.Millisecond)
	}

	data, err := ioutil.ReadFile(path)
	if err != nil {
		return 0, err
	}

	frequency, err := strconv.ParseFloat(strings.TrimSpace(string(data)), 64)
	if err != nil {
		return 0, err
	}

	// Convert from kHz to MHz
	frequency /= 1000000.0
	return frequency, nil
}

func calculateFreqPower(frequency float64) (float64){
	freqToPower := frequency
	freqToPower = math.Round(freqToPower * freqToPower * 100) / 100
	freqToPower = math.Round(freqToPower * (85.0/41.154) * 1000) / 1000
	freqToPower = freqToPower * 0.016

	return freqToPower
}

func SplitAndTrim(s string, delimiter string) []string {
	var result []string
	parts := strings.Split(s, delimiter)
	for _, part := range parts {
		trimmed := strings.TrimSpace(part)
		result = append(result, trimmed)
	}
	return result
}