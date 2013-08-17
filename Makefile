PREFIX=/usr/local

setcolors: setcolors.c
	gcc -Wall setcolors.c -o setcolors

install: setcolors setcolors.1
	install -Dm 755 setcolors "${DESTDIR}${PREFIX}/bin/setcolors"
	install -Dm 644 setcolors.1 "${DESTDIR}${PREFIX}/share/man/man1/setcolors.1"

clean:
	rm setcolors
