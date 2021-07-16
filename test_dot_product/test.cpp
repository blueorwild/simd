#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <x86intrin.h>

#ifdef DOT_SSE
    #define DOT    dot_sse
    double dot_sse(const float* a, const float* b, uint32_t len){
        double res = 0;
        // for align 4
        uint32_t n1 = len & 0X03;
        for(uint32_t i = 0; i < n1; ++i){
            res += a[i] * b[i];
        }
        __m128 xmm0, xmm1, xmm2;
        const int mask = 0Xf1;
        for(uint32_t i = n1; i < len; i += 4){
            xmm0 = _mm_loadu_ps(a + i);
            xmm1 = _mm_loadu_ps(b + i);
            xmm2 = _mm_dp_ps(xmm0, xmm1, mask);
            res += xmm2[0];
        }
        return res;
    }
#elif defined DOT_AVX
    #define DOT    dot_avx
    double dot_avx(const float* a, const float* b, uint32_t len){
        double res = 0;
        // for align 8
        uint32_t n1 = len & 0X07;
        for(uint32_t i = 0; i < n1; ++i){
            res += a[i] * b[i];
        }
        __m256 xmm0, xmm1, xmm2;
        const int mask = 0Xff;
        for(uint32_t i = n1; i < len; i += 8){
            xmm0 = _mm256_loadu_ps(a + i);
            xmm1 = _mm256_loadu_ps(b + i);
            xmm2 = _mm256_dp_ps(xmm0, xmm1, mask);
            res += xmm2[0] + xmm2[4];
        }
        return res;
    }
#else
    #define DOT    dot
    double dot(const float* a, const float* b, uint32_t len){
        double res = 0;
        for(uint32_t i = 0; i < len; ++i){
            res += a[i] * b[i];
        }
        return res;
    }
#endif

int main(int argc, char** argv) {
    srand(12345);

    float* buffA = (float*) malloc(47065 * 4);
    float* buffB = (float*) malloc(47065 * 4);
    for(int i = 0; i < 47065; ++i){
        buffA[i] = rand() % 100;
        buffB[i] = rand() % 100;
    }
    
    double res = 0;
    struct timespec t1 = {0,0}, t2 = {0,0};
    clock_gettime(CLOCK_REALTIME, &t1); 
    for(size_t loop = 0; loop<100000; ++loop) {
        res += DOT((float*)buffA, (float*)buffB, 47065);
    }
    clock_gettime(CLOCK_REALTIME, &t2); 
    double time = (t2.tv_sec - t1.tv_sec) + (t2.tv_nsec - t1.tv_nsec)/1000000000.0;
    printf("Execution time = %2.3lf seconds\n", time);
    printf("res is : %lf\n", res);

    free(buffA);
    free(buffB);

    return 0;
}