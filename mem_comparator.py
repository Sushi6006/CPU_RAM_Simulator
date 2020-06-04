from subprocess import *
from itertools import zip_longest

# create commands
p_cmd = "./scheduler -f benchmark-cm.txt -a rr -s 100 -m p".split()
v_cmd = "./scheduler -f benchmark-cm.txt -a rr -s 100 -m v -q 10".split()
c_cmd = "./scheduler -f benchmark-cm.txt -a rr -s 100 -m cm".split()

# run commands
p_res = Popen(p_cmd, stdout=PIPE, stderr=STDOUT)
v_res = Popen(v_cmd, stdout=PIPE, stderr=STDOUT)
c_res = Popen(c_cmd, stdout=PIPE, stderr=STDOUT)

# get output
p_out, _ = p_res.communicate()
v_out, _ = v_res.communicate()
c_out, _ = c_res.communicate()

# print(p_out.decode("utf-8"))

# split into lines
p_out = p_out.decode("utf-8").split('\n')
v_out = v_out.decode("utf-8").split('\n')
c_out = c_out.decode("utf-8").split('\n')

# output
# print(f'{" ":6}{"p":40}     {"v":40}     {"c":40}')

for i, (p, v, c) in enumerate(zip_longest(p_out, v_out, c_out)):
    p = "" if p == None else p
    v = "" if v == None else v
    c = "" if c == None else c
    
    print(f'{i + 1:>2}.   {p[:40]:<40}     {v[:40]:<40}     {c[:40]:<40}')
