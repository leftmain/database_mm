#!/bin/bash
if [ "x$1" = "x" ]; then
	./a < ~/Downloads/commands.txt > res.txt
	diff ~/Downloads/commands.txt res.txt
else
	./a < ~/Downloads/prog_files/$1 > res.txt
	diff ~/Downloads/prog_files/$1 res.txt
fi
#make
#./a
