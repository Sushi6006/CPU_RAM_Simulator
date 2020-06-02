# COMP30023-2020-Project-2
Project done by Leyan LIN in May, 2020

## File Description
- `README.md` is a readme
- `makefile` is a makefile
- `report.txt` is my report
- `res` is a folder of provided stuff
- **input files**
  - `benchmark-cm.txt` is provided as required
  - `benchmark-cs.txt` is provided as required
  - `processes.txt` is the test given in description
  - `processes2.txt` is my own test case
  - `rr_memoryoverlap.txt`, `reordering.txt` and `timejumps.txt` are given in tests
- **source files**
  - `scheduler.c` is the main file
  - `scheduler.h` contains scheduling, allocation, processes related stuff
  - `processlist.c` and `processlist.h` manages the data types and functions relating to processes
  - `memory.c` and `memory.h` manages the data types and functions relating to memory
  - `output.c` and `output.h` are for outputs
- **misc**
  - `mem_allo_test.c` is for testing memory allocation before implement it
  - `comparator.py` is a comparator for different algorithms
  - `runner.py` is a runner to test the program and print the output in a file
  - *from github.com/joshcarp/scheduler_tester*
    - `dockerfile`
    - `scripts`
    - `tests`