from subprocess import *

# create commands
cmd = "make output".split()

# run commands
res = Popen(cmd, stdout=PIPE, stderr=STDOUT)

# get output
out, _ = res.communicate()

# decode
out = out.decode("utf-8").split("\n")

content = ""

start_found = False
for line in out:
    if line == "OK -- ./scheduler found":
        start_found = True
        continue
    if start_found:
        content += line + "\n"

f = open("test_output.txt", "w")
f.write(content[1:])