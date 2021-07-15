#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#ifdef FAST_MEMCPY
 #include "rte_memcpy.h"
 #define MEMCPY      rte_memcpy
#else
 #define MEMCPY      memcpy
#endif

// #define BUF_SIZE 30
#define BUF_SIZE 63
// #define BUF_SIZE 444

int main(int argc, char** argv) {
    if(argc < 2) return -1;

    FILE* fp = fopen(argv[1], "r");
    if(NULL == fp) return -1;
    fseek(fp, 0, SEEK_END);
    size_t buff_len = ftell(fp);
    rewind(fp);
    char* buff = (char*) malloc(buff_len);
    if(NULL == buff) return -1;
    size_t n = fread(buff, 1, buff_len, fp) - BUF_SIZE;

    char* buf = (char*)malloc(BUF_SIZE);
    int64_t res = 0;   // just for avoiding O3 optimize
    struct timespec t1 = {0,0}, t2 = {0,0};
    clock_gettime(CLOCK_REALTIME, &t1); 
    for(size_t loop = 0; loop<100000; ++loop) {
        for(size_t i = 0; i < n; i += BUF_SIZE) {
            MEMCPY(buf, buff+i, BUF_SIZE);
            res += buf[0];
        }
    }
    clock_gettime(CLOCK_REALTIME, &t2); 
    double time = (t2.tv_sec - t1.tv_sec) + (t2.tv_nsec - t1.tv_nsec)/1000000000.0;
    printf("Execution time = %2.3lf seconds\n", time);
    printf("res is : %lld\n", res);

    free(buff);
    free(buf);
    fclose(fp);

    return 0;
}