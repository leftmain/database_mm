#!/bin/bash

#n=1000000
n=5000000

dir="/Users/admin/Downloads/users_data"
data="\
a${n}.txt \
a${n}_sort_g.txt \
a${n}_sort_n.txt \
a${n}_sort_v.txt \
"

commands="\
commands0.txt \
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
        data_file="${dir}/${data}"
        cmd_file="${dir}/${cmd}"
        echo "Data=${data_file} Commands=${cmd_file}"
        cat ${cmd_file} | ./a ${data_file} | grep '^ *Student' | wc
      done
  done
