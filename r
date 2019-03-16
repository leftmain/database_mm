#!/bin/bash
./a < ~/Downloads/prog_files/$1 > res.txt
diff ~/Downloads/prog_files/$1 res.txt
#make
#./a
