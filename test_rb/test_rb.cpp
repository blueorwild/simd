#include<iostream>
#include<vector>
#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>
#include <unistd.h>
#include <assert.h>

using std::cout;
using std::endl;
using std::vector;


#ifdef RB1
 #define READ_BUFFER      ReadBuffer1
	static uint64_t ReadBuffer1(const char *buffer, uint32_t &index) {
		uint64_t pow = 0;
		uint64_t value = 0;
		do {
			// (buffer[index] & 0x7F) * (n * 128) which n start with 1
			value = value + ((uint64_t)(buffer[index] & 0x7F) << (pow * 7));
			pow += 1;
		} while ((buffer[index++] & 0x80) != 0);
		return value;
	}
#elif defined RB2
 #define READ_BUFFER      ReadBuffer2
	static uint64_t ReadBuffer2(const char *buffer, uint32_t &index) {
		uint64_t value = (buffer[index] & 0x7F);
		int i = 7;
		while ((buffer[index++] & 0x80) != 0 && i<= 63 ) {
			value += ((uint64_t)(buffer[index] & 0x7F) << i);
			i += 7;
		}
		return value;
	}
#else
 #define READ_BUFFER      ReadBuffer3
	static uint64_t ReadBuffer3(const char *buffer, uint32_t &index) {
		uint64_t value = (buffer[index] & 0x7F);
		if ((buffer[index++] & 0x80) == 0) { return value; }

		value += ((uint64_t)(buffer[index] & 0x7F) << 7);
		if ((buffer[index++] & 0x80) == 0) { return value; }

		value += ((uint64_t)(buffer[index] & 0x7F) << 14);
		if ((buffer[index++] & 0x80) == 0) { return value; }

		value += ((uint64_t)(buffer[index] & 0x7F) << 21);
		if ((buffer[index++] & 0x80) == 0) { return value; }

		value += ((uint64_t)(buffer[index] & 0x7F) << 28);
		if ((buffer[index++] & 0x80) == 0) { return value; }

		value += ((uint64_t)(buffer[index] & 0x7F) << 35);
		if ((buffer[index++] & 0x80) == 0) { return value; }

		value += ((uint64_t)(buffer[index] & 0x7F) << 42);
		if ((buffer[index++] & 0x80) == 0) { return value; }

		value += ((uint64_t)(buffer[index] & 0x7F) << 49);
		if ((buffer[index++] & 0x80) == 0) { return value; }

		value += ((uint64_t)(buffer[index] & 0x7F) << 56);
		if ((buffer[index++] & 0x80) == 0) { return value; }

		value += ((uint64_t)(buffer[index] & 0x7F) << 63);
		while ((buffer[index++] & 0x80) != 0) {}
		return value;
	}

#endif

static uint32_t WriteBuffer(char *buffer, uint64_t value) {
	uint32_t len = 0;
	do {
		// value % 128
		char cur = value & 0x7F;
		// value / 128
		value = value >> 7;
		if (value > 0)
			cur |= 0x80;
		buffer[len++] = cur;
	} while (value > 0);
	return len;
}

void testRead(){
    uint64_t nums[] = {0, 0x7F, 0x80, 0x100, 0x3fff, 0x4000, 0x800000, 0x100000000, 0x800000000000, 0x3fffffffffff,
                       0x1FFFFFFFFFFFF, 0x2000000000000, 0x100000000000000, 0x7FFFFFFFFFFFFFFF, 0x8000000000000000,
                       UINT64_MAX};
	
	struct timespec t1 = {0,0}, t2 = {0,0};
	clock_gettime(CLOCK_REALTIME, &t1); 
	for (uint32_t i = 0; i< 10000000; ++i){
		for (uint32_t j = 0; j < sizeof(nums)/sizeof(nums[0]); ++j) {
			char buffer[20];
			uint32_t index = 0;
			uint32_t len = WriteBuffer(buffer, nums[j]);
			uint64_t value = READ_BUFFER(buffer, index);
			assert(nums[j] == value);
			assert(len == index);
		}
	}
    clock_gettime(CLOCK_REALTIME, &t2); 
    double time = (t2.tv_sec - t1.tv_sec) + (t2.tv_nsec - t1.tv_nsec)/1000000000.0;
    printf("Execution time = %2.3lf seconds\n", time);
}

int main() {
	testRead();
	return 0;
}