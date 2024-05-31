SHELL = /usr/bin/env bash
NAME = proj3
LOGIN = xduric05

all:
	latex $(NAME).tex
	latex $(NAME).tex
	dvips -t a4 $(NAME).dvi
	ps2pdf -sPAPERSIZE=a4 $(NAME).ps    

zip:
	zip $(LOGIN)-fit.zip *.tex Makefile *.eps

clean:
	rm -f $(NAME).{aux,dvi,log,pdf,ps,out} $(LOGIN)-fit.zip
