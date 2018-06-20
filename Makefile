arthurKing:arthurKing.c arthurKing.h
	@echo 'Compilation : RUNNING.'
	@gcc arthurKing.c -o arthurKing -pthread
	@echo 'Compilation : DONE'

clean:
	rm -rf *.o
