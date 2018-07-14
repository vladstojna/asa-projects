#!/bin/bash

# Parameter verification
if [ "$#" -ne "2" ]; then
	echo "Usage: $0 [project] [tests to run]";
	echo " [project]      : name of .cpp file to compile"
	echo " [tests to run] : {tests}, {time}, {mem}, {both}"
	echo -e "\t{tests}    - run public tests only"
	echo -e "\t{time}     - run time tests only"
	echo -e "\t{mem}      - run memory tests only"
	echo -e "\t{both}     - run time + memory tests"
	exit
fi

# Variable declaration

# Starting values for graph size
vertNo=500000
edgeNo=500000
sccs=125000
reps=1

# Bounds and increment for consecutive tests
lower=100
incr=100000
upper=1000000
upperVG=1000000

# Directories
currentDir=$(pwd);

# What do to
work="$2"

# Generator variables
genZip=gerador.zip
gen="${currentDir}/gerador"

# Public tests variables
testsZip="students-tests.zip"
tests="students-tests"

bin="p1";
time="/usr/bin/time"
valgrind="valgrind --tool=massif"

# Compile project
(g++ -Wall -O3 -o $bin -lm $1)


# Runs tests
if [ "$work" == "tests" ]; then

	(unzip $testsZip)

	for t in $(ls ${tests}/*.in); do
		./$bin < ${t} > ${t%.*}.myout
		diff=$(diff ${t%.*}.out ${t%.*}.myout)
		if [[ -z "${diff}" ]]; then
			echo "${t##*/} successful"
		else
			echo "${t##*/} FAILED"
		fi
	done

	rm -rf $tests

	rm -f $bin

	# If running tests only, script ends here
	exit

fi

# Unzips graph generator
(unzip $genZip)

# Running time tests
if [ "$work" != "mem" ]; then

	sleep 1

	echo "Running time tests; variable edges"

	# Runs tests for plotting; variable edges
	for edge in $(seq $lower $incr $upper); do
		($gen $vertNo $edge $sccs > graph-${vertNo}-${edge}-${sccs}-def.in)
		for i in $(seq $reps); do
			res=$( $time -f "%e" ./$bin < graph-${vertNo}-${edge}-${sccs}-def.in 2>&1 > /dev/null )
			sum=$(( vertNo + edge ))
			( printf "%s %s\n" $sum $res )
		done
	done

	rm -f graph-*.in

	sleep 1

	echo "Running time tests; variable vertices"

	# Runs tests for plotting; variable vertices
	for vert in $(seq $lower $incr $upper); do
		sccs=$((vert / 4))
		($gen $vert $edgeNo $sccs > graph-${vert}-${edgeNo}-${sccs}-def.in)
		for i in $(seq $reps); do
			res=$($time -f "%e" ./$bin < graph-${vert}-${edgeNo}-${sccs}-def.in 2>&1 > /dev/null)
			sum=$(( vert + edgeNo ))
			( printf "%s %s\n" $sum $res )
		done
	done

	rm -f graph-*.in

	sleep 1

	echo "Running time tests; #edges = #vertices"

	# Runs tests for plotting; #edges = #vertices
	for q in $(seq $lower $incr $upper); do
		sccs=$((q / 5))
		($gen $q $q $sccs > graph-${q}-${q}-${sccs}-def.in)
		for i in $(seq $reps); do
			res=$($time -f "%e" ./$bin < graph-${q}-${q}-${sccs}-def.in 2>&1 > /dev/null)
			sum=$(( q + q ))
			( printf "%s %s\n" $sum $res )
		done
	done

	rm -f graph-*.in

fi

# Running memory tests
if [ "$work" != "time" ]; then

	sleep 1

	echo "Running memory tests; #edges = #vertices"

	# Runs tests for plotting; #edges = #vertices
	for q in $(seq $lower $incr $upperVG); do
		sccs=$((q / 5))

		($gen $q $q $sccs > graph-${q}-${q}-${sccs}-def.in)
		($valgrind ./$bin   < graph-${q}-${q}-${sccs}-def.in)

		sum=$(( q + q ))
		memTotal=0;
		timeRes=$(cat massif.* | grep -B 4 "peak" | grep "time" | awk -F '=' '{print $2}')
		for element in $(cat massif.* | grep -B 3 "peak" | grep -v "peak" | tr "\n" "\t"); do
			memory=${element##*=}
			memTotal=$(( memTotal + memory ))
		done
		rm -f massif.*
		( printf "%s %s %s\n" $sum $memTotal $timeRes )
	done

	rm -f graph-*.in

fi

# Remove generator
rm -f $gen

# Remove compiled file
rm -f $bin