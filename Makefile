CXX=c++
CXXFLAGS=-Wall -std=c++17 -I"./include"
LDFLAGS=-lstdc++fs
TARGET=bfc
SRC=./src/bfc.cpp
PREFIX=/usr/local
BIN=$(PREFIX)/bin

all:
	$(CXX) $(CXXFLAGS) $(SRC) $(LDFLAGS) -o $(TARGET)

install:
	cp -f $(TARGET) $(BIN)/

uninstall:
	rm -f $(BIN)/$(TARGET)

clean:
	rm -f $(TARGET)
