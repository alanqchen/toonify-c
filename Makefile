#Makefile based on Makefile from Brian Duffy
IDIR = ./include
ODIR = ./bin
SDIR = ./src
OUT = toonify
CFLAGS = -std=c++11 -I$(IDIR)
LINKER = `Magick++-config --cxxflags --cppflags`
LINKERPOST = `Magick++-config --ldflags --libs`
CC = g++
HEAD = $(wildcard $(IDIR)/*.h)
OBJ = $(patsubst $(SDIR)/%.cpp, $(ODIR)/%.o, $(wildcard $(SDIR)/*.cpp))

make: $(OUT)

$(ODIR)/%.o: $(SDIR)/%.cpp $(HEAD) $(ODIR)
	$(CC) $(CFLAGS) $(LINKER) -c $< -o $@ $(LINKERPOST)

$(OUT): $(OBJ)
	$(CC) $(CFLAGS) $(LINKER) $^ -o $@ $(LINKERPOST)

$(ODIR):
	mkdir $(ODIR)

clean:
	!rm -rf $(OUT) $(ODIR)
