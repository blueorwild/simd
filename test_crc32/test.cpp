#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <x86intrin.h>

// 0X104C11DB7 (low 32bit CRC32)
// x^32 + x^26 + x^23 + x^22 + x^16 + x^12 + x^11 + x^10 + x^8 + x^7
// +x^5 + x^4 + x^2 + x + 1
static uint32_t crc32_tab_common[] = {
    0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
    0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
    0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
    0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
    0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
    0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
    0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
    0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
    0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
    0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
    0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
    0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
    0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
    0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
    0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
    0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
    0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
    0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
    0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
    0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
    0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
    0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
    0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
    0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
    0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
    0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
    0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
    0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
    0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
    0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
    0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
    0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
    0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
    0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
    0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
    0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
    0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
    0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
    0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
    0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
    0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
    0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
    0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
    0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
    0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
    0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
    0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
    0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
    0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
    0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
    0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
    0x2d02ef8dL
};

// 0X11EDC6F41 (low 32bit CRC32-C)
static uint32_t crc32_tab_sse[] = {
    0x00000000L, 0xf26b8303L, 0xe13b70f7L, 0x1350f3f4L, 0xc79a971fL,
    0x35f1141cL, 0x26a1e7e8L, 0xd4ca64ebL, 0x8ad958cfL, 0x78b2dbccL,
    0x6be22838L, 0x9989ab3bL, 0x4d43cfd0L, 0xbf284cd3L, 0xac78bf27L,
    0x5e133c24L, 0x105ec76fL, 0xe235446cL, 0xf165b798L, 0x030e349bL,
    0xd7c45070L, 0x25afd373L, 0x36ff2087L, 0xc494a384L, 0x9a879fa0L,
    0x68ec1ca3L, 0x7bbcef57L, 0x89d76c54L, 0x5d1d08bfL, 0xaf768bbcL,
    0xbc267848L, 0x4e4dfb4bL, 0x20bd8edeL, 0xd2d60dddL, 0xc186fe29L,
    0x33ed7d2aL, 0xe72719c1L, 0x154c9ac2L, 0x061c6936L, 0xf477ea35L,
    0xaa64d611L, 0x580f5512L, 0x4b5fa6e6L, 0xb93425e5L, 0x6dfe410eL,
    0x9f95c20dL, 0x8cc531f9L, 0x7eaeb2faL, 0x30e349b1L, 0xc288cab2L,
    0xd1d83946L, 0x23b3ba45L, 0xf779deaeL, 0x05125dadL, 0x1642ae59L,
    0xe4292d5aL, 0xba3a117eL, 0x4851927dL, 0x5b016189L, 0xa96ae28aL,
    0x7da08661L, 0x8fcb0562L, 0x9c9bf696L, 0x6ef07595L, 0x417b1dbcL,
    0xb3109ebfL, 0xa0406d4bL, 0x522bee48L, 0x86e18aa3L, 0x748a09a0L,
    0x67dafa54L, 0x95b17957L, 0xcba24573L, 0x39c9c670L, 0x2a993584L,
    0xd8f2b687L, 0x0c38d26cL, 0xfe53516fL, 0xed03a29bL, 0x1f682198L,
    0x5125dad3L, 0xa34e59d0L, 0xb01eaa24L, 0x42752927L, 0x96bf4dccL,
    0x64d4cecfL, 0x77843d3bL, 0x85efbe38L, 0xdbfc821cL, 0x2997011fL,
    0x3ac7f2ebL, 0xc8ac71e8L, 0x1c661503L, 0xee0d9600L, 0xfd5d65f4L,
    0x0f36e6f7L, 0x61c69362L, 0x93ad1061L, 0x80fde395L, 0x72966096L,
    0xa65c047dL, 0x5437877eL, 0x4767748aL, 0xb50cf789L, 0xeb1fcbadL,
    0x197448aeL, 0x0a24bb5aL, 0xf84f3859L, 0x2c855cb2L, 0xdeeedfb1L,
    0xcdbe2c45L, 0x3fd5af46L, 0x7198540dL, 0x83f3d70eL, 0x90a324faL,
    0x62c8a7f9L, 0xb602c312L, 0x44694011L, 0x5739b3e5L, 0xa55230e6L,
    0xfb410cc2L, 0x092a8fc1L, 0x1a7a7c35L, 0xe811ff36L, 0x3cdb9bddL,
    0xceb018deL, 0xdde0eb2aL, 0x2f8b6829L, 0x82f63b78L, 0x709db87bL,
    0x63cd4b8fL, 0x91a6c88cL, 0x456cac67L, 0xb7072f64L, 0xa457dc90L,
    0x563c5f93L, 0x082f63b7L, 0xfa44e0b4L, 0xe9141340L, 0x1b7f9043L,
    0xcfb5f4a8L, 0x3dde77abL, 0x2e8e845fL, 0xdce5075cL, 0x92a8fc17L,
    0x60c37f14L, 0x73938ce0L, 0x81f80fe3L, 0x55326b08L, 0xa759e80bL,
    0xb4091bffL, 0x466298fcL, 0x1871a4d8L, 0xea1a27dbL, 0xf94ad42fL,
    0x0b21572cL, 0xdfeb33c7L, 0x2d80b0c4L, 0x3ed04330L, 0xccbbc033L,
    0xa24bb5a6L, 0x502036a5L, 0x4370c551L, 0xb11b4652L, 0x65d122b9L,
    0x97baa1baL, 0x84ea524eL, 0x7681d14dL, 0x2892ed69L, 0xdaf96e6aL,
    0xc9a99d9eL, 0x3bc21e9dL, 0xef087a76L, 0x1d63f975L, 0x0e330a81L,
    0xfc588982L, 0xb21572c9L, 0x407ef1caL, 0x532e023eL, 0xa145813dL,
    0x758fe5d6L, 0x87e466d5L, 0x94b49521L, 0x66df1622L, 0x38cc2a06L,
    0xcaa7a905L, 0xd9f75af1L, 0x2b9cd9f2L, 0xff56bd19L, 0x0d3d3e1aL,
    0x1e6dcdeeL, 0xec064eedL, 0xc38d26c4L, 0x31e6a5c7L, 0x22b65633L,
    0xd0ddd530L, 0x0417b1dbL, 0xf67c32d8L, 0xe52cc12cL, 0x1747422fL,
    0x49547e0bL, 0xbb3ffd08L, 0xa86f0efcL, 0x5a048dffL, 0x8ecee914L,
    0x7ca56a17L, 0x6ff599e3L, 0x9d9e1ae0L, 0xd3d3e1abL, 0x21b862a8L,
    0x32e8915cL, 0xc083125fL, 0x144976b4L, 0xe622f5b7L, 0xf5720643L,
    0x07198540L, 0x590ab964L, 0xab613a67L, 0xb831c993L, 0x4a5a4a90L,
    0x9e902e7bL, 0x6cfbad78L, 0x7fab5e8cL, 0x8dc0dd8fL, 0xe330a81aL,
    0x115b2b19L, 0x020bd8edL, 0xf0605beeL, 0x24aa3f05L, 0xd6c1bc06L,
    0xc5914ff2L, 0x37faccf1L, 0x69e9f0d5L, 0x9b8273d6L, 0x88d28022L,
    0x7ab90321L, 0xae7367caL, 0x5c18e4c9L, 0x4f48173dL, 0xbd23943eL,
    0xf36e6f75L, 0x0105ec76L, 0x12551f82L, 0xe03e9c81L, 0x34f4f86aL,
    0xc69f7b69L, 0xd5cf889dL, 0x27a40b9eL, 0x79b737baL, 0x8bdcb4b9L,
    0x988c474dL, 0x6ae7c44eL, 0xbe2da0a5L, 0x4c4623a6L, 0x5f16d052L,
    0xad7d5351L
};
#ifdef CRC_SSE
    #define CRC32    crc32_sse
    // 0X11EDC6F41 (low 32bit CRC32-C)
    uint32_t crc32_sse(const unsigned char* s, uint32_t len) {
        uint32_t crc32val = 0;

        while (len >= 4){
            crc32val = _mm_crc32_u32(crc32val, *(unsigned int*) s);
            s += 4;
            len -= 4;
        }
        if (len >= 2){
            crc32val = _mm_crc32_u16(crc32val, *(unsigned short*) s);
            s += 2;
            len -= 2;
        }
        if (len){
            crc32val = _mm_crc32_u8(crc32val, *s);
        }

        return crc32val;
    }
#else
    #define CRC32    crc32
    // 0X104C11DB7 (low 32bit CRC32)
    uint32_t crc32(const unsigned char* s, uint32_t len) {
        uint32_t crc32val = 0x0;  // sse CRC32-C
        // uint32_t crc32val = 0xFFFFFFFF;  // common CRC32
        for (uint32_t i = 0;  i < len;  ++i) {
            crc32val = crc32_tab_sse[(crc32val ^ s[i]) & 0xFF] ^ ((crc32val >> 8) & 0x00FFFFFF);
        }
        return crc32val ^ 0x0;  // sse CRC32-C
        // return crc32val ^ 0xFFFFFFFF; // common CRC32
    }
#endif

uint32_t table[256] = {0};
void gen_crc_table_common(void){
	int i,j,k = 0;
	uint32_t temp = 0x0UL;
    // 1 0000 0100 1100 0001 0001 1101 1011 0111 (B)  the top 1 is hidden
	uint64_t poly_src = 0X104C11DB7;
    // 1110 1101 1011 1000 1000 0011 0010 0000 (B)  reverse
	uint32_t poly     = 0XEDB88320;
    
	for(i = 0;i < 256; ++i){
        table[i] = i;
		for(j = 7;j >= 0; --j){
			temp = table[i] & 0x01; // take the last bit
			if(temp){
				table[i] = table[i] >> 1;
				table[i] ^= poly;
			}
			else{
				table[i] = table[i] >> 1;
			}
		}
	}
}

void gen_crc_table_sse(void){
	int i,j,k = 0;
	uint32_t temp = 0x0UL;
    // 1 0001 1110 1101 1100 0110 1111 0100 0001 (B)  the top 1 is hidden
	uint64_t poly_src = 0X11EDC6F41;
    // 1000 0010 1111 0110 0011 1011 0111 1000 (B)  reverse
	uint32_t poly     = 0x82F63B78;
    
	for(i = 0;i < 256; ++i){
        table[i] = i;
		for(j = 7;j >= 0; --j){
			temp = table[i] & 0x01; // take the last bit
			if(temp){
				table[i] = table[i] >> 1;
				table[i] ^= poly;
			}
			else{
				table[i] = table[i] >> 1;
			}
		}
	}
}

int main(int argc, char** argv) {
    // gen_crc_table_sse();
    // for(uint32_t i = 0; i < 256; ++i){
    //     printf("0x%x", table[i]);
    //     printf("L, ");
    // }
    // return 0;

    if(argc < 2) return -1;

    FILE* fp = fopen(argv[1], "r");
    if(NULL == fp) return -1;
    fseek(fp, 0, SEEK_END);
    size_t buff_len = ftell(fp);

    rewind(fp);
    unsigned char* buffA = (unsigned char*) malloc(buff_len);
    if(NULL == buffA) return -1;
    size_t n = fread(buffA, 1, buff_len, fp);
    rewind(fp);

    uint64_t res = 0;
    struct timespec t1 = {0,0}, t2 = {0,0};
    clock_gettime(CLOCK_REALTIME, &t1); 
    for(size_t loop = 0; loop<1000; ++loop) {
        res += CRC32(buffA, n);
    }
    clock_gettime(CLOCK_REALTIME, &t2); 
    double time = (t2.tv_sec - t1.tv_sec) + (t2.tv_nsec - t1.tv_nsec)/1000000000.0;
    printf("Execution time = %2.3lf seconds\n", time);
    printf("res is : %lld\n", res);

    free(buffA);
    fclose(fp);

    return 0;
}