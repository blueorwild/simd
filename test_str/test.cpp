#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <x86intrin.h>

struct SIZED_STRING{
    const char* value;
    uint32_t len;
    SIZED_STRING() : value(NULL), len(0) {}
    SIZED_STRING(const char *value, uint32_t len) : value(value), len(len) {}
    void Print(){
        printf("len is: %d , str is : ",len);
        for(int i = 0; i < len; ++i){
            printf("%c", value[i]);
        }
        printf("\n");
    }
};

#ifdef STR_SSE
    #define SPLIT    split_sse
    // split str to two str(first and second) by delim
    bool split_sse(SIZED_STRING str, SIZED_STRING delim, SIZED_STRING &first, 
            SIZED_STRING &second, bool matchStr){
        uint32_t delimLen = delim.len, strLen = str.len;
        char *pStart = const_cast<char *>(str.value);
        char *pCur = pStart, *pEnd = pStart + strLen;

        if (matchStr) {
            if (delimLen <= 16 && strLen >= 16 && delimLen > 0) {
                __m128i set16 = _mm_loadu_si128((const __m128i *)delim.value);
                do {
                    __m128i b16 = _mm_loadu_si128((const __m128i *)pCur);
                    uint32_t r = _mm_cmpestri(set16, delimLen, b16, 16, _SIDD_CMP_EQUAL_ORDERED);
                    if (r != 16 && r + delimLen <= 16) {
                        pCur += r;
                        first = SIZED_STRING(pStart, pCur - pStart);
                        second = SIZED_STRING(pCur + delimLen, pEnd - pCur - delimLen);
                        return true;
                    }
                    pCur = pCur + (16 - delimLen + 1);
                } while (pCur + 16 <= pEnd);
            }
            for (; pCur + delimLen <= pEnd; ++pCur) {
                if (*pCur == delim.value[0] && memcmp(pCur, delim.value, delimLen) == 0) {
                    first = SIZED_STRING(pStart, pCur - pStart);
                    second = SIZED_STRING(pCur + delimLen, pEnd - pCur - delimLen);
                    return true;
                }
            }
        } else {
            if (delimLen <= 16 && strLen >= 16 && delimLen > 0) {
                __m128i set16 = _mm_loadu_si128((const __m128i *)delim.value);
                do {
                    __m128i b16 = _mm_loadu_si128((const __m128i *)pCur);
                    uint32_t r = _mm_cmpestri(set16, delimLen, b16, 16, _SIDD_CMP_EQUAL_ANY);
                    pCur += r;
                    if (r != 16) {
                        first = SIZED_STRING(pStart, pCur - pStart);
                        second = SIZED_STRING(pCur + 1, pEnd - pCur - 1);
                        return true;
                    }
                } while (pCur + 16 <= pEnd);
            } 
            for (; pCur < pEnd; ++pCur) {
                for (uint32_t i = 0; i < delimLen; ++i) {
                    if (*pCur == delim.value[i]) {
                        first = SIZED_STRING(pStart, pCur - pStart);
                        second = SIZED_STRING(pCur + 1, pEnd - pCur - 1);
                        return true;
                    }
                }
            }
        }

        first = str;
        second = SIZED_STRING();
        return false;
    }

    // 相当于strcmp . 比较的结果是bitmask, bitmask==0xffff 表示两个字符串完全相等.
    int str_cmpi_cmp(const char *a, const char *b){
        int r;
        __m128i a16 = _mm_loadu_si128((const __m128i *)a);
        __m128i b16 = _mm_loadu_si128((const __m128i *)b);

        r = _mm_cmpistri(
            a16, b16,
            _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_LEAST_SIGNIFICANT);

        printf("result: %d\n", r);

        return r;
    }

    // 相当于strspn . 字符串中第1个落在字符集中的字符的位置. 结果为16表示不包含字符集中的字符.
    int str_cmpi_set(const char *set, const char *str){
        int r;
        __m128i set16 = _mm_loadu_si128((const __m128i *)set);
        __m128i b16 = _mm_loadu_si128((const __m128i *)str);

        r = _mm_cmpistri(set16, b16, _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY | _SIDD_LEAST_SIGNIFICANT);

        if (r != 16)
            printf("found at: %d\n", r);
        else
            printf("not found: %d\n", r);

        return r;
    }

    // 检查是否包含字符范围内的字符
    int str_cmpe_range(const char *ranges, const char *str){
        int r = -1;

        __m128i ranges16 = _mm_loadu_si128((const __m128i *)ranges);
        __m128i b16 = _mm_loadu_si128((const __m128i *)str);
        r = _mm_cmpestri(ranges16, strlen(ranges), b16, strlen(str),
                        _SIDD_LEAST_SIGNIFICANT | _SIDD_CMP_RANGES | _SIDD_UBYTE_OPS);

        if (r != 16)
            printf("found at: %d\n", r);
        else
            printf("not found: %d\n", r);

        return r;
    }

    // 相当于 strstr . 在字符串中查找子串, 并返回匹配的起始位置, 如果没找到则结果为16.
    int str_cmpi_substr(const char *ptn, const char *str){
        int r;
        __m128i ptn16 = _mm_loadu_si128((const __m128i *)ptn);
        __m128i b16 = _mm_loadu_si128((const __m128i *)str);

        r = _mm_cmpistri(ptn16, b16,
                        _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ORDERED | _SIDD_LEAST_SIGNIFICANT);

        if (r != 16)
            printf("found at: %d\n", r);
        else
            printf("not found: %d\n", r);

        return r;
    }


#else
    #define SPLIT    split
    // split str to two str(first and second) by delim
    bool split(SIZED_STRING str, SIZED_STRING delim, SIZED_STRING &first, 
            SIZED_STRING &second, bool matchStr){
        uint32_t delimLen = delim.len, strLen = str.len;
        char *pStart = const_cast<char *>(str.value);
        char *pCur = pStart, *pEnd = pStart + strLen;

        if (matchStr) {
            for (; pCur + delimLen <= pEnd; ++pCur) {
                if (*pCur == delim.value[0] && memcmp(pCur, delim.value, delimLen) == 0) {
                    first = SIZED_STRING(pStart, pCur - pStart);
                    second = SIZED_STRING(pCur + delimLen, pEnd - pCur - delimLen);
                    return true;
                }
            }
        } else {
            for (; pCur < pEnd; ++pCur) {
                for (uint32_t i = 0; i < delimLen; ++i) {
                    if (*pCur == delim.value[i]) {
                        first = SIZED_STRING(pStart, pCur - pStart);
                        second = SIZED_STRING(pCur + 1, pEnd - pCur - 1);
                        return true;
                    }
                }
            }
        }

        first = str;
        second = SIZED_STRING();
        return false;
    }

#endif

const uint32_t buf_size = 63;
int main(int argc, char** argv) {
    if(argc < 2) return -1;

    FILE* fp = fopen(argv[1], "r");
    if(NULL == fp) return -1;
    fseek(fp, 0, SEEK_END);
    size_t buff_len = ftell(fp);

    rewind(fp);
    char* buffA = (char*) malloc(buff_len);
    if(NULL == buffA) return -1;
    size_t n = fread(buffA, 1, buff_len, fp);
    rewind(fp);

    int32_t res = 0;
    SIZED_STRING delim("cas", 3), first, second;
    struct timespec t1 = {0,0}, t2 = {0,0};
    clock_gettime(CLOCK_REALTIME, &t1); 
    for(size_t loop = 0; loop<10000; ++loop) {
        for(int i = 0; i < n; i += buf_size){
            SIZED_STRING str(buffA + i, buf_size);
            if(SPLIT(str, delim, first, second, false))
                ++res;
        }
    }
    clock_gettime(CLOCK_REALTIME, &t2); 
    double time = (t2.tv_sec - t1.tv_sec) + (t2.tv_nsec - t1.tv_nsec)/1000000000.0;
    printf("Execution time = %2.3lf seconds\n", time);
    printf("res is : %d\n", res);

    free(buffA);
    fclose(fp);

    return 0;
}