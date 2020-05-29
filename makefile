build:
	gcc -o scheduler output.c processlist.c scheduler.c -Wall -lm

run_test:
	./scheduler -f processes.txt -a ff -s 200 -m p

run_test2:
	./scheduler -f processes.txt -a csp -s 200 -m p -q 10

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