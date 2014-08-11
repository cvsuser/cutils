WORK_PATH := ./
LIB_PATH := 

INCLUDE = -I./

CC       := g++
LIBS     := pthread
INCS     := 
CFLAGS   := -g -Wall -O2 $(addprefix -I,$(INCS)) $(INCLUDE)
CXXFLAGS := $(CFLAGS)
SRCS	 := $(filter-out $(wildcard *_test.cpp), $(wildcard *.cpp))
OBJS	 := $(patsubst %.cpp,%.o,$(SRCS))
DEPS	 := $(patsubst %.o,%.d,$(OBJS))
COMM_LIBS:= 
TESTHARNESS = $(WORK_PATH)/testharness.o

TARGET   := libutils.a
TESTS    := $(patsubst %.cpp,%,$(wildcard *_test.cpp))

all: $(TARGET)
	@echo "build utils.a done"

$(TARGET): $(TESTHARNESS)
	ar q $(TARGET) $(TESTHARNESS) 





check: all $(TESTS)
	for t in $(TESTS); do echo "***** Running $$t"; ./$$t || exit 1; done


%_test : %_test.o
	$(CC) $(CFLAGS) -static -Wno-strict-aliasing -o $@ $^ -lpthread $(COMM_LIBS) $(TARGET)


clean:
	rm $(OBJS) $(TARGET) $(TESTS)
