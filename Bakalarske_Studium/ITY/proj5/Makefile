SHELL = /usr/bin/env bash
NAME = proj5
LOGIN = xduric05

all:
	pdflatex $(NAME).tex
	pdflatex $(NAME).tex    

zip:
	zip $(LOGIN)-fit.zip *.tex Makefile *.png

clean:
	rm -f $(NAME).{aux,dvi,log,pdf,ps,out,nav,snm,toc} *.pdf $(LOGIN)-fit.zip
