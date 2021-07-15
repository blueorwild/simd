CC=g++
CFLAGS= -O3 #-g
TARGET=test test_fast test_fast_avx2

default: $(TARGET)

test: test.cpp
	$(CC) -o $@ $(CFLAGS) $^

test_fast: test.cpp
	$(CC) -o $@ $(CFLAGS) -march=x86-64 -msse4.2 -DFAST_MEMCMP $^

test_fast_avx2: test.cpp
	$(CC) -o $@ $(CFLAGS) -march=x86-64 -mavx2 -DRTE_MACHINE_CPUFLAG_AVX2 -DFAST_MEMCMP $^

clean:
	@rm -f $(TARGET)