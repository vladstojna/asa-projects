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

# Repetitions for each size
opsNo=2
reps=1

# Bounds and increment for consecutive tests
lower=50
incr=50
upper=1000
upperVG=1000

# Directories
currentDir=$(pwd);

# What do to
work="$2"

# Generator variables
gen="${currentDir}/gerador"

# Public tests variables
testsZip="bigIO.zip"
tests="bigio"

bin="p2";
time="/usr/bin/time"
valgrind="valgrind --tool=massif"

# Compile project
(g++ -Wall -O3 -o $bin -lm $1)


# Runs tests
if [ "$work" == "tests" ]; then

	(unzip $testsZip -d $tests)

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


# Running time tests
if [ "$work" != "mem" ]; then

	message="Running time tests; sparse graphs"

	printf "[vertexNumber] [time]\n"

	for ops in $(seq $opsNo); do
		# Runs tests for plotting; sparse graphs, then dense graphs
		echo $message;
		for size in $(seq $lower $incr $upper); do
			($gen $size $ops > graph-${size}-${ops}.in)
			for i in $(seq $reps); do
				res=$( $time -f "%e" ./$bin < graph-${size}-${ops}.in 2>&1 > /dev/null )
				firstline=$(head graph-${size}-${ops}.in | awk '{print $1 " " $2; exit}')
				vert=$(( ${firstline% *} * ${firstline#* } ))
				( printf "%s %s %s\n" $vert $res )
			done
		done
		rm -f graph-*.in
		message="Running time tests; dense graphs"
		sleep 1
	done

fi

sleep 1

# Running memory tests
if [ "$work" != "time" ]; then

	echo "Running memory tests; dense graphs only"

	printf "[vertexNumber] [totalMemory] [time]\n"

	# Runs tests for plotting; #edges = #vertices
	for size in $(seq $lower $incr $upperVG); do

		($gen $size 2 > graph-${size}-2.in)
		($valgrind ./$bin   < graph-${size}-2.in > /dev/null)

		firstline=$(head graph-${size}-${ops}.in | awk '{print $1 " " $2; exit}')
		vert=$(( ${firstline% *} * ${firstline#* } ))

		memTotal=0;
		timeRes=$(cat massif.* | grep -B 4 "peak" | grep "time" | awk -F '=' '{print $2}')
		for element in $(cat massif.* | grep -B 3 "peak" | grep -v "peak" | tr "\n" "\t"); do
			memory=${element##*=}
			memTotal=$(( memTotal + memory ))
		done
		rm -f massif.*
		( printf "%s %s %s\n" $vert $memTotal $timeRes )
	done

	rm -f graph-*.in

fi

# Remove compiled file
rm -f $bin