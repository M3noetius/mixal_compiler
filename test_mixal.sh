#!/bin/bash

RED='\033[0;31m'
GREEN='\033[1;32m'
NC='\033[0m'

for code in $(ls tests); do
	if [ "$code" = "linked" ]; then
		continue;
	fi

	a=$(cat tests/$code)
	echo -e "${GREEN}########### $code ###############${NC}"
	echo "$a"
	echo ""
	echo "$a" | ./compiler 2> /dev/null > "tests/linked/$code.mixal"   
	echo ""
	#cat "tests/linked/$code.mixal"
	echo ""
	mixasm "tests/linked/$code.mixal" 2> /dev/null

	echo -e "${RED}--------- MIXVM OUTPUT ----------${NC}"
	echo ""
	mixvm -r -d "tests/linked/$code.mix" | grep "+\|-"	
	echo ""
	echo ""
	echo ""
	

done
