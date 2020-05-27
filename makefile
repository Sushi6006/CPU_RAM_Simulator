build:
	gcc -o scheduler output.c processlist.c scheduler.c -Wall -lm

run_test:
	./scheduler -f processes.txt -a ff -s 200 -m p

run_test2:
	./scheduler -f processes2.txt -a of -s 200 -m p

clean:
	rm -f scheduler

everything:
	rm -f scheduler
	gcc -o scheduler scheduler.c -Wall

gitpush:
	git add -A
	git commit -m "$(m)"
	git push