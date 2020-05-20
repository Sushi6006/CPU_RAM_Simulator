build:
	gcc -o scheduler scheduler.c -Wall

run:
	./scheduler

clean:
	rm -f scheduler

everything:
	rm -f scheduler
	gcc -o scheduler scheduler.c -Wall
	./scheduler

# TODO: try to make it enter username and password automatically
gitpush:
	git add -A
	git commit -m "$(m)"
	git push