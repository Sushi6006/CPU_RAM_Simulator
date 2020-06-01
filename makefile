build:
	gcc -o scheduler output.c processlist.c memory.c scheduler.c -Wall -lm

run_test:
	./scheduler -f processes.txt -a ff -s 100 -m p

run_test2:
	./scheduler -f processes.txt -a cs -s 100 -m p -q 10

run_comp:
	python3 runner.py

clean:
	rm -f scheduler

everything:
	rm -f scheduler
	gcc -o scheduler output.c processlist.c scheduler.c -Wall -lm

gitpush:
	git add -A
	git commit -m "$(m)"
	git push

wordcount:
	wc -w report.txt

# some tests
mem_allo_test:
	rm -f mem_test
	gcc -o mem_test output.c processlist.c memory.c scheduler.c mem_allo_test.c -Wall -lm
	./mem_test
	

test:
	# give permission using chmod first, then run the script from joshcarp/scheduler_test
	chmod 755 -R scripts/
	./scripts/test.sh