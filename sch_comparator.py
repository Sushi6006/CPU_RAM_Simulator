from subprocess import *
from itertools import zip_longest

# create commands
ff_cmd = "./scheduler -f benchmark-cs.txt -a ff -s 100 -m p".split()
rr_cmd = "./scheduler -f benchmark-cs.txt -a rr -s 100 -m p -q 10".split()
cs_cmd = "./scheduler -f benchmark-cs.txt -a cs -s 100 -m p".split()

# run commands
ff_res = Popen(ff_cmd, stdout=PIPE, stderr=STDOUT)
rr_res = Popen(rr_cmd, stdout=PIPE, stderr=STDOUT)
cs_res = Popen(cs_cmd, stdout=PIPE, stderr=STDOUT)

# get output
ff_out, _ = ff_res.communicate()
rr_out, _ = rr_res.communicate()
cs_out, _ = cs_res.communicate()

# print(ff_out.decode("utf-8"))

# split into lines
ff_out = ff_out.decode("utf-8").split('\n')
rr_out = rr_out.decode("utf-8").split('\n')
cs_out = cs_out.decode("utf-8").split('\n')

# output
# print(f'{" ":6}{"ff":40}     {"rr":40}     {"cs":40}')

for i, (ff, rr, cs) in enumerate(zip_longest(ff_out, rr_out, cs_out)):
    ff = "" if ff == None else ff
    rr = "" if rr == None else rr
    cs = "" if cs == None else cs
    
    print(f'{i + 1:>2}.   {ff[:40]:<40}     {rr[:40]:<40}     {cs[:40]:<40}')
