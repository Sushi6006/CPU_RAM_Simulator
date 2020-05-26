build:
	gcc -o scheduler scheduler.c  dbgoutput.c -Wall

run:
	./scheduler

clean:
	rm -f scheduler

everything:
	rm -f scheduler
	gcc -o scheduler scheduler.c -Wall

gitpush:
	git add -A
	git commit -m "$(m)"
	git push