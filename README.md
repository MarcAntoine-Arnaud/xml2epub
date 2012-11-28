xml2epub
========

latex-inspired-xml-markup-language to epub,azw,azw2,html,pdf converter

This code is still heavily in development. If you want to give it a spin then checkout the development branch.

BUILD INSTRUCTIONS
===================
checkout development branch


type-in: make (and keep your fingers crossed)

USAGE
=====

Convert XML to latex:

./xml2epub -l true < example.xml > output.tex

Convert XML to html:

./xml2epub -l false < example.xml > output.tex

more formats to come, see

./xml2epub --help


BUILD DEPENDENCIES
===================

tidy (http://tidy.sourceforge.net/)

libxml++ (http://libxmlplusplus.sourceforge.net/)

FUTURE: zipios++

RUNTIME DEPENDENCIES
=====================
xelatex with unicode-math support (you NEED texlive>=2012!!!!)

gs,pdf2ps,ps2eps etc. (ghostscript and ghostscript helper tools)

gnuplot (with epslatex terminal)



Later the above runtime dependencies will be integrates into the code base