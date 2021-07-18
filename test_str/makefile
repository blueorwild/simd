CC=g++
CFLAGS= -O3 #-g
TARGET=test test_fast

default: $(TARGET)

test: test.cpp
	$(CC) -o $@ $(CFLAGS) -march=x86-64 -msse4.2 $^

test_fast: test.cpp
	$(CC) -o $@ $(CFLAGS) -march=x86-64 -msse4.2 -DSTR_SSE $^

clean:
	@rm -f $(TARGET)