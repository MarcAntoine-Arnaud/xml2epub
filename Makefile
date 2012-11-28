CXX=g++
XML_CFLAGS=$(shell PKG_CONFIG_PATH=/home/fr810/Documents/LocalLinux/share/pkgconfig:/home/fr810/Documents/LocalLinux/lib/pkgconfig pkg-config libxml++-2.6 --cflags)
CFLAGS=-O0 -g -isystem=/home/fr810/Documents/LocalLinux/include $(XML_CFLAGS) -I/home/fr810/Documents/LocalLinux/include/tidy
XML_LDFLAGS=$(shell PKG_CONFIG_PATH=/home/fr810/Documents/LocalLinux/share/pkgconfig:/home/fr810/Documents/LocalLinux/lib/pkgconfig pkg-config libxml++-2.6 --libs)
LDFLAGS=-L/home/fr810/Documents/LocalLinux/lib -lboost_program_options $(XML_LDFLAGS) -ltidy
CXXFLAGS=

TARGET=xml2epub

SRC=main.cc html.cc latex.cc plot.cc
OBJ=$(SRC:.cc=.o)
DEP=$(SRC:.cc=.d)

$(TARGET) :  $(OBJ)
	$(CXX) $(CFLAGS) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.d : %.cc
	set -e; rm -f $@; \
	echo -n "./" > $@.$$$$; \
	$(CXX) -MM $(CFLAGS) $(CXXFLAGS) $< >> $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

include $(DEP)

%.o : %.cc
	$(CXX) -c -o $@ $(CFLAGS) $(CXXFLAGS) $<

clean :
	rm -rf $(OBJ)
	rm -rf $(DEP)
	rm -rf $(TARGET)
