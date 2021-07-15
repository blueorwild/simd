#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#ifdef FAST_MEMCMP
 #include "rte_memcmp.h"
 #define MEMCMP      rte_memcmp
#else
 #define MEMCMP      memcmp
#endif

// #define KEYWORD     "connection"
// #define KEYWORD_LEN 10
#define KEYWORD     "This call causes the data contained in the indicated user buffe"
#define KEYWORD_LEN 63

int main(int argc, char** argv) {
    if(argc < 2) return -1;

    FILE* fp = fopen(argv[1], "r");
    if(NULL == fp) return -1;
    fseek(fp, 0, SEEK_END);
    size_t buff_len = ftell(fp);
    rewind(fp);
    char* buff = (char*) malloc(buff_len);
    if(NULL == buff) return -1;
    size_t n = fread(buff, 1, buff_len, fp);

    size_t cnt = 0;
    struct timespec t1 = {0,0}, t2 = {0,0};
    clock_gettime(CLOCK_REALTIME, &t1); 
    for(size_t loop = 0; loop<10000; ++loop) {
        for(size_t i=0; i<n; i++) {
            if(0 == MEMCMP(buff+i, KEYWORD, KEYWORD_LEN))
                cnt ++;
        }
    }
    clock_gettime(CLOCK_REALTIME, &t2); 
    double time = (t2.tv_sec - t1.tv_sec) + (t2.tv_nsec - t1.tv_nsec)/1000000000.0;
    printf("Execution time = %2.3lf seconds\n", time);
    printf("hit count: %u\n", cnt);

    free(buff);
    fclose(fp);

    return 0;
}