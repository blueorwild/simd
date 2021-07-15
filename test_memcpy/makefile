CC=g++
CFLAGS= -O3 #-g
TARGET=test test_fast test_fast_avx2 #test_fast_avx512f

default: $(TARGET)

test: test.cpp
	$(CC) -o $@ $(CFLAGS) $^

test_fast: test.cpp
	$(CC) -o $@ $(CFLAGS) -march=x86-64 -msse4.2 -DFAST_MEMCPY $^

test_fast_avx2: test.cpp
	$(CC) -o $@ $(CFLAGS) -march=x86-64 -mavx2 -DRTE_MACHINE_CPUFLAG_AVX2 -DFAST_MEMCPY $^

# test_fast_avx512f: test.cpp
# 	$(CC) -o $@ $(CFLAGS) -march=x86-64 -mavx512f -DRTE_MACHINE_CPUFLAG_AVX512F -DFAST_MEMCPY $^

clean:
	@rm -f $(TARGET)