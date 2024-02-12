CC := g++
CFLAGS := -Wall -g --pedantic -Os
AR := /usr/bin/ar -crs
TARGET := gainable
RM := /usr/bin/rm
RACINE := /home/pi/.local/
BIN=$(RACINE)/bin/
LIB=$(RACINE)/lib/
INC=$(RACINE)/include/
# copie: verbeuse et uniquement les plus recents
CP := /usr/bin/cp -uv 
SRCS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp,%.o,$(SRCS))
BIB := gpioPinlib.a

# cosmetique dans ce cas
ASTYLE := /usr/bin/astyle -d -xC80 -xL  -s2 --style=goog

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
	
install: $(TARGET)
	$(CP) $^ $(BIN)/.
	$(CP) *.hpp $(INC)/.
	$(CP) *.a $(LIB)/.
	@echo
	@echo  Installation terminée . N\'oubliez pas de mettre
	@echo  \* DS18B20.conf \*
	@echo dans le répertoire où $(TARGET) doit opèrer /home/pi/.local/bin/
