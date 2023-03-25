
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <stdio.h>
#include "md5-x86-asm.h"
#include "md5-arm-asm.h"

template<typename HT>
void md5_init(MD5_STATE<HT>* state) {
	state->A = 0x67452301;
	state->B = 0xefcdab89;
	state->C = 0x98badcfe;
	state->D = 0x10325476;
}
#ifdef __AVX512VL__
#include <immintrin.h>
template<>
void md5_init<__m128i>(MD5_STATE<__m128i>* state) {
	state->A = _mm_cvtsi32_si128(0x67452301);
	state->B = _mm_cvtsi32_si128(0xefcdab89);
	state->C = _mm_cvtsi32_si128(0x98badcfe);
	state->D = _mm_cvtsi32_si128(0x10325476);
}
#endif

template<typename HT, void(&fn)(MD5_STATE<HT>*, const void*)>
void md5(MD5_STATE<HT>* state, const void* __restrict__ src, size_t len) {
	md5_init<HT>(state);
	char* __restrict__ _src = (char* __restrict__)src;
	uint64_t totalLen = len << 3; // length in bits
	
	for(; len >= 64; len -= 64) {
		fn(state, _src);
		_src += 64;
	}
	len &= 63;
	
	
	// finalize
	char block[64];
	memcpy(block, _src, len);
	block[len++] = 0x80;
	
	// write this in a loop to avoid duplicating the force-inlined process_block function twice
	for(int iter = (len <= 64-8); iter < 2; iter++) {
		if(iter == 0) {
			memset(block + len, 0, 64-len);
			len = 0;
		} else {
			memset(block + len, 0, 64-8 - len);
			memcpy(block + 64-8, &totalLen, 8);
		}
		
		fn(state, block);
	}
}


bool do_tests(const char* expected, const void* __restrict__ src, size_t len) {
	MD5_STATE<uint32_t> hash;
	md5<uint32_t, md5_block_std>(&hash, src, len);
	if(memcmp(&hash, expected, 16)) return true;
#ifdef PLATFORM_X86
	md5<uint32_t, md5_block_gopt>(&hash, src, len);
	if(memcmp(&hash, expected, 16)) return true;
	md5<uint32_t, md5_block_ghopt>(&hash, src, len);
	if(memcmp(&hash, expected, 16)) return true;
#ifdef __BMI__
	md5<uint32_t, md5_block_ghbmi>(&hash, src, len);
	if(memcmp(&hash, expected, 16)) return true;
#endif
	md5<uint32_t, md5_block_nolea>(&hash, src, len);
	if(memcmp(&hash, expected, 16)) return true;
	md5<uint32_t, md5_block_noleag>(&hash, src, len);
	if(memcmp(&hash, expected, 16)) return true;
	md5<uint32_t, md5_block_noleagh>(&hash, src, len);
	if(memcmp(&hash, expected, 16)) return true;
#ifdef PLATFORM_AMD64
	md5<uint32_t, md5_block_cache4>(&hash, src, len);
	if(memcmp(&hash, expected, 16)) return true;
	md5<uint32_t, md5_block_cache8>(&hash, src, len);
	if(memcmp(&hash, expected, 16)) return true;
	md5<uint32_t, md5_block_cache_gopt>(&hash, src, len);
	if(memcmp(&hash, expected, 16)) return true;
#ifdef __AVX512VL__
	MD5_STATE<__m128i> hashV;
	md5<__m128i, md5_block_avx512>(&hashV, src, len);
	hash.A = _mm_cvtsi128_si32(hashV.A);
	hash.B = _mm_cvtsi128_si32(hashV.B);
	hash.C = _mm_cvtsi128_si32(hashV.C);
	hash.D = _mm_cvtsi128_si32(hashV.D);
	if(memcmp(&hash, expected, 16)) return true;
#endif
#endif
#endif
	
	return false;
}


#ifdef _WIN32
// because high-resolution timer in MinGW is questionable: https://github.com/msys2/MINGW-packages/issues/5086
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
static uint64_t queryPerfFreq() {
	LARGE_INTEGER f;
	QueryPerformanceFrequency(&f);
	return f.QuadPart;
}
static uint64_t perfFreq = queryPerfFreq();
class Timer
{
	uint64_t beg_;
	static uint64_t getTime() {
		LARGE_INTEGER t;
		QueryPerformanceCounter(&t);
		return t.QuadPart;
	}
public:
	Timer() {
		reset();
	}
	void reset() { beg_ = getTime(); }
	double elapsed() const { 
		return (double)(getTime() - beg_) / perfFreq;
	}
};
#else
// from https://stackoverflow.com/a/19471595/459150
#include <chrono>
class Timer
{
public:
    Timer() : beg_(clock_::now()) {}
    void reset() { beg_ = clock_::now(); }
    double elapsed() const { 
        return std::chrono::duration_cast<second_>
            (clock_::now() - beg_).count(); }

private:
    typedef std::chrono::high_resolution_clock clock_;
    typedef std::chrono::duration<double, std::ratio<1> > second_;
    std::chrono::time_point<clock_> beg_;
};
#endif

const size_t TEST_SIZE = 16384; // same as OpenSSL max
const unsigned NUM_TRIALS = 5;
const unsigned NUM_ROUNDS = 16384;
void* benchData;

#include <float.h>
template<typename HT, void(&fn)(MD5_STATE<HT>*, const void*)>
static void run_bench(const char* label) {
	std::cout << label;
	
	MD5_STATE<HT> hash;
	double result = DBL_MAX;
	int trial = NUM_TRIALS;
	while(trial--) {
		int rounds = NUM_ROUNDS;
		Timer t;
		while(rounds--) {
			md5<HT, fn>(&hash, benchData, TEST_SIZE);
		}
		asm(""::"m"(hash):); // force hash to be written
		double secs = t.elapsed();
		if(secs < result) result = secs;
	}
	printf("%8.1f MB/s\n", (double)(TEST_SIZE*NUM_ROUNDS/1048576) / result);
}


int main(void) {
	#define TEST_ERROR(s) std::cout << "TEST ERROR: " s << std::endl
	
	if(do_tests("\xd4\x1d\x8c\xd9\x8f\0\xb2\x04\xe9\x80\x09\x98\xec\xf8\x42\x7e", "", 0))
		TEST_ERROR("Failed blank");
	if(do_tests("\x0c\xc1\x75\xb9\xc0\xf1\xb6\xa8\x31\xc3\x99\xe2\x69\x77\x26\x61", "a", 1))
		TEST_ERROR("Failed partial block");
	if(do_tests("\x3d\x37\x3b\x8c\xd6\xfd\x06\x9d\x31\x3c\xdc\x3f\x38\xa1\x89\x63", "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ012", 55)) // everything fits in an MD5 block
		TEST_ERROR("Failed nearly full block");
	if(do_tests("\xd4\x3e\x61\xe9\xb5\xf8\xc9\xd2\x2c\x4d\xc5\xdb\x6e\x6d\xf7\x75", "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123", 56)) // just crosses one byte into second block
		TEST_ERROR("Failed nearly full block (2)");
	if(do_tests("\xce\x3a\x13\xcb\x6c\x59\xe1\xda\xd8\xa1\x70\xec\xd5\x0f\x0c\xe8", "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-", 63))
		TEST_ERROR("Failed nearly full block (3)");
	if(do_tests("\x2a\x37\x87\xf9\x92\x07\xe3\x6b\x2c\xb2\xc3\x40\x68\x92\xde\xf0", "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_", 64))
		TEST_ERROR("Failed full block");
	if(do_tests("\x77\xf8\x6b\xd2\x20\x76\xca\x4e\x99\x0f\xc7\xba\x77\x78\x11\x13", "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_=", 65))
		TEST_ERROR("Failed full+partial block");
	if(do_tests("\x9b\x27\x94\x27\xd4\x81\xc9\xc9\xc7\x1d\x9a\xcb\x4f\xc9\xe9\x9a", "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_", 128))
		TEST_ERROR("Failed two full blocks");
	
	
	// run benchmark
	benchData = calloc(1, TEST_SIZE);
	run_bench<uint32_t, md5_block_std>( "Std   : ");
#ifdef PLATFORM_X86
	run_bench<uint32_t, md5_block_gopt>("GOpt  : ");
	run_bench<uint32_t, md5_block_ghopt>("GHOpt : ");
#ifdef __BMI__
	run_bench<uint32_t, md5_block_ghbmi>("GHBMI : ");
#endif
	run_bench<uint32_t, md5_block_nolea>("NoLEA : ");
	run_bench<uint32_t, md5_block_noleag>("NoL-G : ");
	run_bench<uint32_t, md5_block_noleagh>("NoL-GH: ");
#ifdef PLATFORM_AMD64
	run_bench<uint32_t, md5_block_cache4>("Cache4: ");
	run_bench<uint32_t, md5_block_cache8>("Cache8: ");
	run_bench<uint32_t, md5_block_cache_gopt>("Cach8G: ");
#ifdef __AVX512VL__
	run_bench<__m128i, md5_block_avx512>("AVX512: ");
#endif
#endif
#endif
	
	return 0;
}
