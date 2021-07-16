#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <x86intrin.h>

#ifdef ADD_SSE
    #define ADD    add_sse
    void add_sse(uint32_t* a, uint32_t* b, uint32_t* c, uint32_t len){
        // for align 4
        uint32_t n1 = len & 0X03;
        for(uint32_t i = 0; i < n1; ++i){
            c[i] = a[i] + b[i];
        }
        __m128i xmm0, xmm1, xmm2;
        for(uint32_t i = n1; i < len; i += 4){
            xmm0 = _mm_loadu_si128((const __m128i *)(a + i));
            xmm1 = _mm_loadu_si128((const __m128i *)(b + i));
            xmm2 = _mm_add_epi32(xmm0, xmm1);
            _mm_storeu_si128((__m128i *)(c + i), xmm2);
        }
    }
#elif defined ADD_AVX
    #define ADD    add_avx
    void add_avx(uint32_t* a, uint32_t* b, uint32_t* c, uint32_t len){
        // for align 8
        uint32_t n1 = len & 0X07;
        for(uint32_t i = 0; i < n1; ++i){
            c[i] = a[i] + b[i];
        }
        __m256i xmm0, xmm1, xmm2;
        for(uint32_t i = n1; i < len; i += 8){
            xmm0 = _mm256_loadu_si256((const __m256i *)(a + i));
            xmm1 = _mm256_loadu_si256((const __m256i *)(b + i));
            xmm2 = _mm256_add_epi32(xmm0, xmm1);
            _mm256_storeu_si256((__m256i *)(c + i), xmm2);
        }
    }
#else
    #define ADD    add
    void add(uint32_t* a, uint32_t* b, uint32_t* c, uint32_t len){
        for(uint32_t i = 0; i < len; ++i){
            c[i] = a[i] + b[i];
        }
    }
#endif

uint64_t SUM(uint32_t* num, uint32_t len){
    uint64_t res = 0;
    for(uint32_t i = 0; i < len; ++i){
        res += num[i];
    }
    return res;
}

int main(int argc, char** argv) {
    if(argc < 2) return -1;

    FILE* fp = fopen(argv[1], "r");
    if(NULL == fp) return -1;
    fseek(fp, 0, SEEK_END);
    size_t buff_len = ftell(fp);
    buff_len = buff_len & 0Xfffffff4;

    rewind(fp);
    char* buffA = (char*) malloc(buff_len);
    char* buffB = (char*) malloc(buff_len);
    char* buffC = (char*) malloc(buff_len);
    memset(buffC, '\0', buff_len);
    if(NULL == buffA) return -1;
    size_t n = fread(buffA, 1, buff_len, fp) >> 2;
    rewind(fp);
    fread(buffB, 1, buff_len, fp);

    uint64_t res = 0;
    struct timespec t1 = {0,0}, t2 = {0,0};
    clock_gettime(CLOCK_REALTIME, &t1); 
    for(size_t loop = 0; loop<100000; ++loop) {
        ADD((uint32_t*)buffA, (uint32_t*)buffB, (uint32_t*)buffC, n);
        res += buffC[0];
    }
    clock_gettime(CLOCK_REALTIME, &t2); 
    double time = (t2.tv_sec - t1.tv_sec) + (t2.tv_nsec - t1.tv_nsec)/1000000000.0;
    printf("Execution time = %2.3lf seconds\n", time);
    res += SUM((uint32_t*)(buffC), n);
    printf("res is : %lld\n", res);

    free(buffA);
    free(buffB);
    free(buffC);
    fclose(fp);

    return 0;
}