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
TESTS    := ut_test skiplis_test hash_test

all: $(TARGET) $(UTEST)
	@echo "build utils.a done"

$(TARGET): $(TESTHARNESS)
	ar q $(TARGET) $(TESTHARNESS) 


check: all $(TESTS)
	for t in $(TESTS); do echo "***** Running $$t"; ./$$t || exit 1; done

ut_test: ut_test.o
	$(CC) $(CFLAGS) -static -Wno-strict-aliasing -o $@ $^ -lpthread $(COMM_LIBS) $(TARGET)

skiplist_test: skiplist_test.o
	$(CC) $(CFLAGS) -static -Wno-strict-aliasing -o $@ $^ -lpthread $(COMM_LIBS) $(TARGET)

hash_test : hash_test.o
	$(CC) $(CFLAGS) -static -Wno-strict-aliasing -o $@ $^ -lpthread $(COMM_LIBS) $(TARGET)

clean:
	rm $(OBJS) $(TARGET) $(TESTS)
