CC := g++
CFLAGS := -Wall -g --pedantic -Os
AR := /usr/bin/ar -crs
TARGET := Gainable
RM := /usr/bin/rm
SRCS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp,%.o,$(SRCS))
BIB := gpioPinlib.a

# cosmetique dans ce cas
ASTYLE := /usr/bin/astyle -d -xC80 -xL  -s2 --style=google


all: $(TARGET) $(BIB)

$(BIB): BB_DS18B20.o  gpioPin.o
	$(AR) $@ $^ 

$(TARGET):$(TARGET).o $(BIB) 
	$(CC) -o $@ $^  -lgpiod -lpthread
%.o: %.cpp
	$(CC) $(CFLAGS) -c $<
clean:
	$(RM) -rf $(TARGET) *.o
# cosmetique, inutile dans ce cas
joli:
	$(ASTYLE)   *.cpp *.hpp
