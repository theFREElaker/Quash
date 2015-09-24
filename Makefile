quash: inputredirect.c Jobs.c Jobs.h
	gcc -g  inputredirect.c -o quash

clean:
	rm quash
