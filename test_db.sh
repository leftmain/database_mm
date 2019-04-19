#!/bin/bash

n=1000000
#n=5000000

path="/home/username/"
datadir="${path}big_files"
data="\
nvg_${n}.txt \
"

cmddir="${path}Downloads/prog_files"
commands="\
commands.txt \
commands1.txt \
commands2.txt \
commands3.txt \
commands4.txt \
commands5.txt \
"


for data in ${data}
  do \
    for cmd in ${commands}
      do \
        data_file="${datadir}/${data}"
        cmd_file="${cmddir}/${cmd}"
        echo "Data=${data_file} Commands=${cmd_file}"
        cat ${cmd_file} | ./a.out ${data_file} | grep '^ *Student' | wc
#        cat ${cmd_file} | ./a ${data_file} | grep '^ *Student' | wc >> res.txt
#        cat ${cmd_file} | ./aa ${data_file} | grep '^ *Student' | wc >> res1.txt
      done
  done

#diff -C0 res.txt res1.txt
#rm res.txt res1.txt

