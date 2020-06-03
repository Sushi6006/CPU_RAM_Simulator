build:
	gcc -o scheduler output.c processlist.c memory.c scheduler.c -Wall -lm

run:
	./scheduler -f rr_memoryoverlap.txt -a rr -s 20 -m v -q 10

clean:
	rm -f scheduler

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

compare:
	python3 comparator.py

test:
	python3 runner.py

output:
	# give permission using chmod first, then run the script from joshcarp/scheduler_test
	chmod 755 -R scripts/
	./scripts/test.sh