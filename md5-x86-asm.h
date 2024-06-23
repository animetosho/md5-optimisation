
#ifndef STR
# define STR_HELPER(x) #x
# define STR(x) STR_HELPER(x)
#endif
#if defined(__x86_64__) || \
    defined(__amd64__ ) || \
    defined(__LP64    ) || \
    defined(_M_X64    ) || \
    defined(_M_AMD64  ) || \
    defined(_WIN64    )
	#define PLATFORM_AMD64 1
#endif
#if defined(PLATFORM_AMD64) || \
    defined(__i386__  ) || \
    defined(__i486__  ) || \
    defined(__i586__  ) || \
    defined(__i686__  ) || \
    defined(_M_I86    ) || \
    defined(_M_IX86   ) || \
    defined(_WIN32    )
	#define PLATFORM_X86 1
#endif

#ifdef PLATFORM_X86

template<typename HT>
struct MD5_STATE {
	HT A, B, C, D;
};

// GCC insanity [https://gcc.gnu.org/legacy-ml/gcc-help/2011-04/msg00566.html]
#define ASM_INPUTS [input0]"m"(_in[0]), [input1]"m"(_in[1]), [input2]"m"(_in[2]), [input3]"m"(_in[3]), [input4]"m"(_in[4]), [input5]"m"(_in[5]), [input6]"m"(_in[6]), [input7]"m"(_in[7]), [input8]"m"(_in[8]), [input9]"m"(_in[9]), [input10]"m"(_in[10]), [input11]"m"(_in[11]), [input12]"m"(_in[12]), [input13]"m"(_in[13]), [input14]"m"(_in[14]), [input15]"m"(_in[15])

static inline __attribute__((always_inline)) void md5_block_std(MD5_STATE<uint32_t>* __restrict__ state, const void* __restrict__ data) {
	uint32_t A;
	uint32_t B;
	uint32_t C;
	uint32_t D;
	const uint32_t* _in = (const uint32_t*)data;
	void *tmp1, *tmp2;
	
#ifndef PLATFORM_AMD64
	A = state->A;
	B = state->B;
	C = state->C;
	D = state->D;
#endif
	
	
#define ROUND_F(I, A, B, C, D, NEXT_IN, K, R) \
	"xorl %k[" STR(C) "], %k[TMP1]\n" \
	"leal " STR(K) "(%k[" STR(I) STR(A) "], %k[TMP2]), %k[" STR(A) "]\n" \
	"andl %k[" STR(B) "], %k[TMP1]\n" \
	"movl " NEXT_IN ", %k[TMP2]\n" \
	"xorl %k[" STR(D) "], %k[TMP1]\n" \
	"addl %k[TMP1], %k[" STR(A) "]\n" \
	"roll $" STR(R) ", %k[" STR(A) "]\n" \
	"movl %k[" STR(C) "], %k[TMP1]\n" \
	"addl %k[" STR(B) "], %k[" STR(A) "]\n"
#define ROUND_H(A, B, C, D, NEXT_IN, K, R) \
	"xorl %k[" STR(C) "], %k[TMP1]\n" \
	"leal " STR(K) "(%k[" STR(A) "], %k[TMP2]), %k[" STR(A) "]\n" \
	"xorl %k[" STR(B) "], %k[TMP1]\n" \
	"movl " NEXT_IN ", %k[TMP2]\n" \
	"addl %k[TMP1], %k[" STR(A) "]\n" \
	"roll $" STR(R) ", %k[" STR(A) "]\n" \
	"movl %k[" STR(C) "], %k[TMP1]\n" \
	"addl %k[" STR(B) "], %k[" STR(A) "]\n"
#define ROUND_I(A, B, C, D, NEXT_IN, K, R) \
	"notl %k[TMP1]\n" \
	"leal " STR(K) "(%k[" STR(A) "], %k[TMP2]), %k[" STR(A) "]\n" \
	"orl %k[" STR(B) "], %k[TMP1]\n" \
	"movl " NEXT_IN ", %k[TMP2]\n" \
	"xorl %k[" STR(C) "], %k[TMP1]\n" \
	"addl %k[TMP1], %k[" STR(A) "]\n" \
	"roll $" STR(R) ", %k[" STR(A) "]\n" \
	"movl %k[" STR(C) "], %k[TMP1]\n" \
	"addl %k[" STR(B) "], %k[" STR(A) "]\n"
#define ROUND_I_LAST(A, B, C, D, K, R) \
	"notl %k[TMP1]\n" \
	"leal " STR(K) "(%k[" STR(A) "], %k[TMP2]), %k[" STR(A) "]\n" \
	"orl %k[" STR(B) "], %k[TMP1]\n" \
	"xorl %k[" STR(C) "], %k[TMP1]\n" \
	"addl %k[TMP1], %k[" STR(A) "]\n" \
	"roll $" STR(R) ", %k[" STR(A) "]\n" \
	"addl %k[" STR(B) "], %k[" STR(A) "]\n"

#ifdef PLATFORM_AMD64
	void *tmp3;
#define ROUND_G(A, B, C, D, NEXT_IN, K, R) \
	"notl %k[TMP1]\n" \
	"movl %k[" STR(D) "], %k[TMP3]\n" \
	"leal " STR(K) "(%k[" STR(A) "], %k[TMP2]), %k[" STR(A) "]\n" \
	"andl %k[" STR(C) "], %k[TMP1]\n" \
	"movl " NEXT_IN ", %k[TMP2]\n" \
	"andl %k[" STR(B) "], %k[TMP3]\n" \
	"orl %k[TMP3], %k[TMP1]\n" \
	"addl %k[TMP1], %k[" STR(A) "]\n" \
	"roll $" STR(R) ", %k[" STR(A) "]\n" \
	"movl %k[" STR(C) "], %k[TMP1]\n" \
	"addl %k[" STR(B) "], %k[" STR(A) "]\n"
#else
#define ROUND_G(A, B, C, D, NEXT_IN, K, R) \
	"notl %k[TMP1]\n" \
	"leal " STR(K) "(%k[" STR(A) "], %k[TMP2]), %k[" STR(A) "]\n" \
	"movl %k[" STR(D) "], %k[TMP2]\n" \
	"andl %k[" STR(C) "], %k[TMP1]\n" \
	"andl %k[" STR(B) "], %k[TMP2]\n" \
	"orl %k[TMP2], %k[TMP1]\n" \
	"movl " NEXT_IN ", %k[TMP2]\n" \
	"addl %k[TMP1], %k[" STR(A) "]\n" \
	"roll $" STR(R) ", %k[" STR(A) "]\n" \
	"movl %k[" STR(C) "], %k[TMP1]\n" \
	"addl %k[" STR(B) "], %k[" STR(A) "]\n"
#endif

#define RF4(I, i0, i1, i2, i3, k0, k1, k2, k3) \
	ROUND_F(I, A, I##B, I##C, I##D, "%[input" STR(i0) "]", k0, 7) \
	ROUND_F(I, D, A, I##B, I##C, "%[input" STR(i1) "]", k1, 12) \
	ROUND_F(I, C, D, A, I##B, "%[input" STR(i2) "]", k2, 17) \
	ROUND_F(I, B, C, D, A, "%[input" STR(i3) "]", k3, 22)
	
#define RG4(i0, i1, i2, i3, k0, k1, k2, k3) \
	ROUND_G(A, B, C, D, "%[input" STR(i0) "]", k0, 5) \
	ROUND_G(D, A, B, C, "%[input" STR(i1) "]", k1, 9) \
	ROUND_G(C, D, A, B, "%[input" STR(i2) "]", k2, 14) \
	ROUND_G(B, C, D, A, "%[input" STR(i3) "]", k3, 20)
	
#define RH4(i0, i1, i2, i3, k0, k1, k2, k3) \
	ROUND_H(A, B, C, D, "%[input" STR(i0) "]", k0, 4) \
	ROUND_H(D, A, B, C, "%[input" STR(i1) "]", k1, 11) \
	ROUND_H(C, D, A, B, "%[input" STR(i2) "]", k2, 16) \
	ROUND_H(B, C, D, A, "%[input" STR(i3) "]", k3, 23)
	
#define RI4(i0, i1, i2, i3, k0, k1, k2, k3) \
	ROUND_I(A, B, C, D, "%[input" STR(i0) "]", k0, 6) \
	ROUND_I(D, A, B, C, "%[input" STR(i1) "]", k1, 10) \
	ROUND_I(C, D, A, B, "%[input" STR(i2) "]", k2, 15) \
	ROUND_I(B, C, D, A, "%[input" STR(i3) "]", k3, 21)
	
	__asm__(
		"movl %[input0], %k[TMP2]\n"
#ifdef PLATFORM_AMD64
		"movl %k[ID], %k[TMP1]\n"
		RF4(I, 1,  2,  3,  4,  -0x28955b88, -0x173848aa, 0x242070db, -0x3e423112)
#else
		"movl %k[D], %k[TMP1]\n"
		RF4(, 1,  2,  3,  4,  -0x28955b88, -0x173848aa, 0x242070db, -0x3e423112)
#endif
		RF4(, 5,  6,  7,  8,  -0x0a83f051, 0x4787c62a, -0x57cfb9ed, -0x02b96aff)
		RF4(, 9, 10, 11, 12,  0x698098d8, -0x74bb0851, -0x0000a44f, -0x76a32842)
		RF4(,13, 14, 15,  1,  0x6b901122, -0x02678e6d, -0x5986bc72, 0x49b40821)
	: [TMP1]"=&R"(tmp1), [TMP2]"=&r"(tmp2),
#ifdef PLATFORM_AMD64
	  [A]"=&R"(A), [B]"=&R"(B), [C]"=&R"(C), [D]"=&R"(D)
	: [IA]"r"(state->A), [IB]"r"(state->B), [IC]"r"(state->C), [ID]"r"(state->D),
#else
	  [A]"+&R"(A), [B]"+&R"(B), [C]"+&R"(C), [D]"+&R"(D)
 	:
#endif
	ASM_INPUTS
	:);
	
	__asm__(
		RG4( 6, 11,  0,  5,  -0x09e1da9e, -0x3fbf4cc0, 0x265e5a51, -0x16493856)
		RG4(10, 15,  4,  9,  -0x29d0efa3, 0x02441453, -0x275e197f, -0x182c0438)
		RG4(14,  3,  8, 13,  0x21e1cde6, -0x3cc8f82a, -0x0b2af279, 0x455a14ed)
		RG4( 2,  7, 12,  5,  -0x561c16fb, -0x03105c08, 0x676f02d9, -0x72d5b376)
		
		RH4( 8, 11, 14,  1,  -0x0005c6be, -0x788e097f, 0x6d9d6122, -0x021ac7f4)
		RH4( 4,  7, 10, 13,  -0x5b4115bc, 0x4bdecfa9, -0x0944b4a0, -0x41404390)
		RH4( 0,  3,  6,  9,  0x289b7ec6, -0x155ed806, -0x2b10cf7b, 0x04881d05)
		RH4(12, 15,  2,  0,  -0x262b2fc7, -0x1924661b, 0x1fa27cf8, -0x3b53a99b)
		
		RI4( 7, 14,  5, 12,  -0x0bd6ddbc, 0x432aff97, -0x546bdc59, -0x036c5fc7)
		RI4( 3, 10,  1,  8,  0x655b59c3, -0x70f3336e, -0x00100b83, -0x7a7ba22f)
		RI4(15,  6, 13,  4,  0x6fa87e4f, -0x01d31920, -0x5cfebcec, 0x4e0811a1)
		
		ROUND_I(A, B, C, D, "%[input11]", -0x08ac817e, 6)
		ROUND_I(D, A, B, C, "%[input2]" , -0x42c50dcb, 10)
		ROUND_I(C, D, A, B, "%[input9]" , 0x2ad7d2bb, 15)
		ROUND_I_LAST(B, C, D, A, -0x14792c6f, 21)
	: [TMP1]"+&R"(tmp1), [TMP2]"+&r"(tmp2),
#ifdef PLATFORM_AMD64
	  [A]"+&R"(A), [B]"+&R"(B), [C]"+&R"(C), [D]"+&R"(D)
	, [TMP3]"=&r"(tmp3)
#else
	  [A]"+&R"(A), [B]"+&R"(B), [C]"+&R"(C), [D]"+&R"(D)
#endif
	: ASM_INPUTS
	:);
	
	state->A += A;
	state->B += B;
	state->C += C;
	state->D += D;
}
#undef ROUND_G

static inline __attribute__((always_inline)) void md5_block_gopt(MD5_STATE<uint32_t>* __restrict__ state, const void* __restrict__ data) {
	uint32_t A;
	uint32_t B;
	uint32_t C;
	uint32_t D;
	const uint32_t* _in = (const uint32_t*)data;
	void *tmp1, *tmp2;
	
#ifndef PLATFORM_AMD64
	A = state->A;
	B = state->B;
	C = state->C;
	D = state->D;
#endif
	
#ifdef PLATFORM_AMD64
	void* tmp3;
#define ROUND_G(A, B, C, D, NEXT_IN, K, R) \
	"notl %k[TMP1]\n" \
	"leal " STR(K) "(%k[" STR(A) "], %k[TMP2]), %k[" STR(A) "]\n" \
	"andl %k[" STR(C) "], %k[TMP1]\n" \
	"movl " NEXT_IN ", %k[TMP2]\n" \
	"movl %k[" STR(D) "], %k[TMP3]\n" \
	"addl %k[TMP1], %k[" STR(A) "]\n" \
	"andl %k[" STR(B) "], %k[TMP3]\n" \
	"addl %k[TMP3], %k[" STR(A) "]\n" \
	"roll $" STR(R) ", %k[" STR(A) "]\n" \
	"movl %k[" STR(C) "], %k[TMP1]\n" \
	"addl %k[" STR(B) "], %k[" STR(A) "]\n"
#else
#define ROUND_G(A, B, C, D, NEXT_IN, K, R) \
	"notl %k[TMP1]\n" \
	"leal " STR(K) "(%k[" STR(A) "], %k[TMP2]), %k[" STR(A) "]\n" \
	"andl %k[" STR(C) "], %k[TMP1]\n" \
	"movl " NEXT_IN ", %k[TMP2]\n" \
	"addl %k[TMP1], %k[" STR(A) "]\n" \
	"movl %k[" STR(D) "], %k[TMP1]\n" \
	"andl %k[" STR(B) "], %k[TMP1]\n" \
	"addl %k[TMP1], %k[" STR(A) "]\n" \
	"roll $" STR(R) ", %k[" STR(A) "]\n" \
	"movl %k[" STR(C) "], %k[TMP1]\n" \
	"addl %k[" STR(B) "], %k[" STR(A) "]\n"
	
#endif

	__asm__(
		"movl %[input0], %k[TMP2]\n"
#ifdef PLATFORM_AMD64
		"movl %k[ID], %k[TMP1]\n"
		RF4(I, 1,  2,  3,  4,  -0x28955b88, -0x173848aa, 0x242070db, -0x3e423112)
#else
		"movl %k[D], %k[TMP1]\n"
		RF4(, 1,  2,  3,  4,  -0x28955b88, -0x173848aa, 0x242070db, -0x3e423112)
#endif
		RF4(, 5,  6,  7,  8,  -0x0a83f051, 0x4787c62a, -0x57cfb9ed, -0x02b96aff)
		RF4(, 9, 10, 11, 12,  0x698098d8, -0x74bb0851, -0x0000a44f, -0x76a32842)
		RF4(,13, 14, 15,  1,  0x6b901122, -0x02678e6d, -0x5986bc72, 0x49b40821)
	: [TMP1]"=&R"(tmp1), [TMP2]"=&r"(tmp2),
#ifdef PLATFORM_AMD64
	  [A]"=&R"(A), [B]"=&R"(B), [C]"=&R"(C), [D]"=&R"(D)
	: [IA]"r"(state->A), [IB]"r"(state->B), [IC]"r"(state->C), [ID]"r"(state->D),
#else
	  [A]"+&R"(A), [B]"+&R"(B), [C]"+&R"(C), [D]"+&R"(D)
	:
#endif
	ASM_INPUTS
	:);
	
	__asm__(
		RG4( 6, 11,  0,  5,  -0x09e1da9e, -0x3fbf4cc0, 0x265e5a51, -0x16493856)
		RG4(10, 15,  4,  9,  -0x29d0efa3, 0x02441453, -0x275e197f, -0x182c0438)
		RG4(14,  3,  8, 13,  0x21e1cde6, -0x3cc8f82a, -0x0b2af279, 0x455a14ed)
		RG4( 2,  7, 12,  5,  -0x561c16fb, -0x03105c08, 0x676f02d9, -0x72d5b376)
		
		RH4( 8, 11, 14,  1,  -0x0005c6be, -0x788e097f, 0x6d9d6122, -0x021ac7f4)
		RH4( 4,  7, 10, 13,  -0x5b4115bc, 0x4bdecfa9, -0x0944b4a0, -0x41404390)
		RH4( 0,  3,  6,  9,  0x289b7ec6, -0x155ed806, -0x2b10cf7b, 0x04881d05)
		RH4(12, 15,  2,  0,  -0x262b2fc7, -0x1924661b, 0x1fa27cf8, -0x3b53a99b)
		
		RI4( 7, 14,  5, 12,  -0x0bd6ddbc, 0x432aff97, -0x546bdc59, -0x036c5fc7)
		RI4( 3, 10,  1,  8,  0x655b59c3, -0x70f3336e, -0x00100b83, -0x7a7ba22f)
		RI4(15,  6, 13,  4,  0x6fa87e4f, -0x01d31920, -0x5cfebcec, 0x4e0811a1)
		
		ROUND_I(A, B, C, D, "%[input11]", -0x08ac817e, 6)
		ROUND_I(D, A, B, C, "%[input2]" , -0x42c50dcb, 10)
		ROUND_I(C, D, A, B, "%[input9]" , 0x2ad7d2bb, 15)
		ROUND_I_LAST(B, C, D, A, -0x14792c6f, 21)
	: [TMP1]"+&R"(tmp1), [TMP2]"+&r"(tmp2),
#ifdef PLATFORM_AMD64
	  [A]"+&R"(A), [B]"+&R"(B), [C]"+&R"(C), [D]"+&R"(D)
	, [TMP3]"=&r"(tmp3)
#else
	  [A]"+&R"(A), [B]"+&R"(B), [C]"+&R"(C), [D]"+&R"(D)
#endif
	: ASM_INPUTS
	:);
	
	state->A += A;
	state->B += B;
	state->C += C;
	state->D += D;
}

#undef ROUND_H

static inline __attribute__((always_inline)) void md5_block_ghopt(MD5_STATE<uint32_t>* __restrict__ state, const void* __restrict__ data) {
	uint32_t A;
	uint32_t B;
	uint32_t C;
	uint32_t D;
	const uint32_t* _in = (const uint32_t*)data;
	void *tmp1, *tmp2;
	
#ifndef PLATFORM_AMD64
	A = state->A;
	B = state->B;
	C = state->C;
	D = state->D;
#else
	void* tmp3;
#endif
	
#define ROUND_H(A, B, C, D, NEXT_IN, K, R) \
	"xorl %k[" STR(A) "], %k[TMP1]\n" \
	"leal " STR(K) "(%k[" STR(A) "], %k[TMP2]), %k[" STR(A) "]\n" \
	"xorl %k[" STR(B) "], %k[TMP1]\n" \
	"movl " NEXT_IN ", %k[TMP2]\n" \
	"addl %k[TMP1], %k[" STR(A) "]\n" \
	"roll $" STR(R) ", %k[" STR(A) "]\n" \
	"addl %k[" STR(B) "], %k[" STR(A) "]\n"
	
	__asm__(
		"movl %[input0], %k[TMP2]\n"
#ifdef PLATFORM_AMD64
		"movl %k[ID], %k[TMP1]\n"
		RF4(I, 1,  2,  3,  4,  -0x28955b88, -0x173848aa, 0x242070db, -0x3e423112)
#else
		"movl %k[D], %k[TMP1]\n"
		RF4(, 1,  2,  3,  4,  -0x28955b88, -0x173848aa, 0x242070db, -0x3e423112)
#endif
		RF4(, 5,  6,  7,  8,  -0x0a83f051, 0x4787c62a, -0x57cfb9ed, -0x02b96aff)
		RF4(, 9, 10, 11, 12,  0x698098d8, -0x74bb0851, -0x0000a44f, -0x76a32842)
		RF4(,13, 14, 15,  1,  0x6b901122, -0x02678e6d, -0x5986bc72, 0x49b40821)
	: [TMP1]"=&R"(tmp1), [TMP2]"=&r"(tmp2),
#ifdef PLATFORM_AMD64
	  [A]"=&R"(A), [B]"=&R"(B), [C]"=&R"(C), [D]"=&R"(D)
	: [IA]"r"(state->A), [IB]"r"(state->B), [IC]"r"(state->C), [ID]"r"(state->D),
#else
	  [A]"+&R"(A), [B]"+&R"(B), [C]"+&R"(C), [D]"+&R"(D)
	:
#endif
	ASM_INPUTS
	:);
	
	__asm__(
		RG4( 6, 11,  0,  5,  -0x09e1da9e, -0x3fbf4cc0, 0x265e5a51, -0x16493856)
		RG4(10, 15,  4,  9,  -0x29d0efa3, 0x02441453, -0x275e197f, -0x182c0438)
		RG4(14,  3,  8, 13,  0x21e1cde6, -0x3cc8f82a, -0x0b2af279, 0x455a14ed)
		RG4( 2,  7, 12,  5,  -0x561c16fb, -0x03105c08, 0x676f02d9, -0x72d5b376)
		
		"xorl %k[C], %k[TMP1]\n"
		"leal -0x0005c6be(%k[A], %k[TMP2]), %k[A]\n"
		"xorl %k[B], %k[TMP1]\n"
		"movl %[input8], %k[TMP2]\n"
		"addl %k[TMP1], %k[A]\n"
		"roll $4, %k[A]\n"
		"addl %k[B], %k[A]\n"
		ROUND_H(D, A, B, C, "%[input11]", -0x788e097f, 11)
		ROUND_H(C, D, A, B, "%[input14]", 0x6d9d6122, 16)
		ROUND_H(B, C, D, A, "%[input1]", -0x021ac7f4, 23)
		RH4( 4,  7, 10, 13,  -0x5b4115bc, 0x4bdecfa9, -0x0944b4a0, -0x41404390)
		RH4( 0,  3,  6,  9,  0x289b7ec6, -0x155ed806, -0x2b10cf7b, 0x04881d05)
		RH4(12, 15,  2,  0,  -0x262b2fc7, -0x1924661b, 0x1fa27cf8, -0x3b53a99b)
		"movl %k[D], %k[TMP1]\n"
		
		RI4( 7, 14,  5, 12,  -0x0bd6ddbc, 0x432aff97, -0x546bdc59, -0x036c5fc7)
		RI4( 3, 10,  1,  8,  0x655b59c3, -0x70f3336e, -0x00100b83, -0x7a7ba22f)
		RI4(15,  6, 13,  4,  0x6fa87e4f, -0x01d31920, -0x5cfebcec, 0x4e0811a1)
		
		ROUND_I(A, B, C, D, "%[input11]", -0x08ac817e, 6)
		ROUND_I(D, A, B, C, "%[input2]" , -0x42c50dcb, 10)
		ROUND_I(C, D, A, B, "%[input9]" , 0x2ad7d2bb, 15)
		ROUND_I_LAST(B, C, D, A, -0x14792c6f, 21)
	: [TMP1]"+&R"(tmp1), [TMP2]"+&r"(tmp2),
#ifdef PLATFORM_AMD64
	  [A]"+&R"(A), [B]"+&R"(B), [C]"+&R"(C), [D]"+&R"(D)
	, [TMP3]"=&r"(tmp3)
#else
	  [A]"+&R"(A), [B]"+&R"(B), [C]"+&R"(C), [D]"+&R"(D)
#endif
	: ASM_INPUTS
	:);
	
	state->A += A;
	state->B += B;
	state->C += C;
	state->D += D;
}


#ifdef __BMI__
#undef ROUND_G

static inline __attribute__((always_inline)) void md5_block_ghbmi(MD5_STATE<uint32_t>* __restrict__ state, const void* __restrict__ data) {
	uint32_t A;
	uint32_t B;
	uint32_t C;
	uint32_t D;
	const uint32_t* _in = (const uint32_t*)data;
	void *tmp1, *tmp2;
	
#ifndef PLATFORM_AMD64
	A = state->A;
	B = state->B;
	C = state->C;
	D = state->D;
#endif
	
#ifdef PLATFORM_AMD64
	void* tmp3;
#define ROUND_G(A, B, C, D, NEXT_IN, K, R) \
	"leal " STR(K) "(%k[" STR(A) "], %k[TMP2]), %k[" STR(A) "]\n" \
	"movl " NEXT_IN ", %k[TMP2]\n" \
	"movl %k[" STR(D) "], %k[TMP3]\n" \
	"addl %k[TMP1], %k[" STR(A) "]\n" \
	"andl %k[" STR(B) "], %k[TMP3]\n" \
	"addl %k[TMP3], %k[" STR(A) "]\n" \
	"roll $" STR(R) ", %k[" STR(A) "]\n" \
	"andnl %k[" STR(B) "], %k[" STR(C) "], %k[TMP1]\n" \
	"addl %k[" STR(B) "], %k[" STR(A) "]\n"
#define ROUND_G_LAST(A, B, C, D, NEXT_IN, K, R) \
	"leal " STR(K) "(%k[" STR(A) "], %k[TMP2]), %k[" STR(A) "]\n" \
	"movl " NEXT_IN ", %k[TMP2]\n" \
	"movl %k[" STR(D) "], %k[TMP3]\n" \
	"addl %k[TMP1], %k[" STR(A) "]\n" \
	"andl %k[" STR(B) "], %k[TMP3]\n" \
	"addl %k[TMP3], %k[" STR(A) "]\n" \
	"roll $" STR(R) ", %k[" STR(A) "]\n" \
	"movl %k[" STR(C) "], %k[TMP1]\n" \
	"addl %k[" STR(B) "], %k[" STR(A) "]\n"
#else
#define ROUND_G(A, B, C, D, NEXT_IN, K, R) \
	"leal " STR(K) "(%k[" STR(A) "], %k[TMP2]), %k[" STR(A) "]\n" \
	"addl %k[TMP1], %k[" STR(A) "]\n" \
	"movl " NEXT_IN ", %k[TMP2]\n" \
	"movl %k[" STR(D) "], %k[TMP1]\n" \
	"andl %k[" STR(B) "], %k[TMP1]\n" \
	"addl %k[TMP1], %k[" STR(A) "]\n" \
	"roll $" STR(R) ", %k[" STR(A) "]\n" \
	"andnl %k[" STR(B) "], %k[" STR(C) "], %k[TMP1]\n" \
	"addl %k[" STR(B) "], %k[" STR(A) "]\n"
#define ROUND_G_LAST(A, B, C, D, NEXT_IN, K, R) \
	"leal " STR(K) "(%k[" STR(A) "], %k[TMP2]), %k[" STR(A) "]\n" \
	"addl %k[TMP1], %k[" STR(A) "]\n" \
	"movl " NEXT_IN ", %k[TMP2]\n" \
	"movl %k[" STR(D) "], %k[TMP1]\n" \
	"andl %k[" STR(B) "], %k[TMP1]\n" \
	"addl %k[TMP1], %k[" STR(A) "]\n" \
	"roll $" STR(R) ", %k[" STR(A) "]\n" \
	"movl %k[" STR(C) "], %k[TMP1]\n" \
	"addl %k[" STR(B) "], %k[" STR(A) "]\n"
#endif
#define ROUND_F_LAST(A, B, C, D, NEXT_IN, K, R) \
	"xorl %k[" STR(C) "], %k[TMP1]\n" \
	"leal " STR(K) "(%k[" STR(A) "], %k[TMP2]), %k[" STR(A) "]\n" \
	"andl %k[" STR(B) "], %k[TMP1]\n" \
	"movl " NEXT_IN ", %k[TMP2]\n" \
	"xorl %k[" STR(D) "], %k[TMP1]\n" \
	"addl %k[TMP1], %k[" STR(A) "]\n" \
	"roll $" STR(R) ", %k[" STR(A) "]\n" \
	"andnl %k[" STR(B) "], %k[" STR(C) "], %k[TMP1]\n" \
	"addl %k[" STR(B) "], %k[" STR(A) "]\n"

#ifdef PLATFORM_AMD64
#undef ROUND_I
#undef ROUND_I_LAST
#define ROUND_I(A, B, C, D, NEXT_IN, K, R) \
	"leal " STR(K) "-1(%k[" STR(A) "], %k[TMP2]), %k[" STR(A) "]\n" \
	"andnl %k[" STR(D) "], %k[" STR(B) "], %k[TMP1]\n" \
	"movl " NEXT_IN ", %k[TMP2]\n" \
	"xorl %k[" STR(C) "], %k[TMP1]\n" \
	"subl %k[TMP1], %k[" STR(A) "]\n" \
	"roll $" STR(R) ", %k[" STR(A) "]\n" \
	"addl %k[" STR(B) "], %k[" STR(A) "]\n"
#define ROUND_I_LAST(A, B, C, D, K, R) \
	"leal " STR(K) "-1(%k[" STR(A) "], %k[TMP2]), %k[" STR(A) "]\n" \
	"andnl %k[" STR(D) "], %k[" STR(B) "], %k[TMP1]\n" \
	"xorl %k[" STR(C) "], %k[TMP1]\n" \
	"subl %k[TMP1], %k[" STR(A) "]\n" \
	"roll $" STR(R) ", %k[" STR(A) "]\n" \
	"addl %k[" STR(B) "], %k[" STR(A) "]\n"
#endif
	
	__asm__(
		"movl %[input0], %k[TMP2]\n"
#ifdef PLATFORM_AMD64
		"movl %k[ID], %k[TMP1]\n"
		RF4(I, 1,  2,  3,  4,  -0x28955b88, -0x173848aa, 0x242070db, -0x3e423112)
#else
		"movl %k[D], %k[TMP1]\n"
		RF4(, 1,  2,  3,  4,  -0x28955b88, -0x173848aa, 0x242070db, -0x3e423112)
#endif
		RF4(, 5,  6,  7,  8,  -0x0a83f051, 0x4787c62a, -0x57cfb9ed, -0x02b96aff)
		RF4(, 9, 10, 11, 12,  0x698098d8, -0x74bb0851, -0x0000a44f, -0x76a32842)
		
		ROUND_F(, A, B, C, D, "%[input13]", 0x6b901122, 7)
		ROUND_F(, D, A, B, C, "%[input14]", -0x02678e6d, 12)
		ROUND_F(, C, D, A, B, "%[input15]", -0x5986bc72, 17)
		ROUND_F_LAST(B, C, D, A, "%[input1]", 0x49b40821, 22)
	: [TMP1]"=&R"(tmp1), [TMP2]"=&r"(tmp2),
#ifdef PLATFORM_AMD64
	  [A]"=&R"(A), [B]"=&R"(B), [C]"=&R"(C), [D]"=&R"(D)
	: [IA]"r"(state->A), [IB]"r"(state->B), [IC]"r"(state->C), [ID]"r"(state->D),
#else
	  [A]"+&R"(A), [B]"+&R"(B), [C]"+&R"(C), [D]"+&R"(D)
	:
#endif
	ASM_INPUTS
	:);
	
	__asm__(
		RG4( 6, 11,  0,  5,  -0x09e1da9e, -0x3fbf4cc0, 0x265e5a51, -0x16493856)
		RG4(10, 15,  4,  9,  -0x29d0efa3, 0x02441453, -0x275e197f, -0x182c0438)
		RG4(14,  3,  8, 13,  0x21e1cde6, -0x3cc8f82a, -0x0b2af279, 0x455a14ed)
		ROUND_G(A, B, C, D, "%[input2]", -0x561c16fb, 5)
		ROUND_G(D, A, B, C, "%[input7]", -0x03105c08, 9)
		ROUND_G(C, D, A, B, "%[input12]", 0x676f02d9, 14)
		ROUND_G_LAST(B, C, D, A, "%[input5]", -0x72d5b376, 20)
		
		"xorl %k[C], %k[TMP1]\n"
		"leal -0x0005c6be(%k[A], %k[TMP2]), %k[A]\n"
		"xorl %k[B], %k[TMP1]\n"
		"movl %[input8], %k[TMP2]\n"
		"addl %k[TMP1], %k[A]\n"
		"roll $4, %k[A]\n"
		"addl %k[B], %k[A]\n"
		ROUND_H(D, A, B, C, "%[input11]", -0x788e097f, 11)
		ROUND_H(C, D, A, B, "%[input14]", 0x6d9d6122, 16)
		ROUND_H(B, C, D, A, "%[input1]", -0x021ac7f4, 23)
		RH4( 4,  7, 10, 13,  -0x5b4115bc, 0x4bdecfa9, -0x0944b4a0, -0x41404390)
		RH4( 0,  3,  6,  9,  0x289b7ec6, -0x155ed806, -0x2b10cf7b, 0x04881d05)
		RH4(12, 15,  2,  0,  -0x262b2fc7, -0x1924661b, 0x1fa27cf8, -0x3b53a99b)
		
		RI4( 7, 14,  5, 12,  -0x0bd6ddbc, 0x432aff97, -0x546bdc59, -0x036c5fc7)
		RI4( 3, 10,  1,  8,  0x655b59c3, -0x70f3336e, -0x00100b83, -0x7a7ba22f)
		RI4(15,  6, 13,  4,  0x6fa87e4f, -0x01d31920, -0x5cfebcec, 0x4e0811a1)
		
		ROUND_I(A, B, C, D, "%[input11]", -0x08ac817e, 6)
		ROUND_I(D, A, B, C, "%[input2]" , -0x42c50dcb, 10)
		ROUND_I(C, D, A, B, "%[input9]" , 0x2ad7d2bb, 15)
		ROUND_I_LAST(B, C, D, A, -0x14792c6f, 21)
	: [TMP1]"+&R"(tmp1), [TMP2]"+&r"(tmp2),
#ifdef PLATFORM_AMD64
	  [A]"+&R"(A), [B]"+&R"(B), [C]"+&R"(C), [D]"+&R"(D)
	, [TMP3]"=&r"(tmp3)
#else
	  [A]"+&R"(A), [B]"+&R"(B), [C]"+&R"(C), [D]"+&R"(D)
#endif
	: ASM_INPUTS
	:);
	
	state->A += A;
	state->B += B;
	state->C += C;
	state->D += D;
}
#undef ROUND_F_LAST
#undef ROUND_G_LAST
#endif

#undef ROUND_F
#undef ROUND_G
#undef ROUND_H
#undef ROUND_I
#undef ROUND_I_LAST
#undef RF4
#undef RG4
#undef RH4
#undef RI4



static inline __attribute__((always_inline)) void md5_block_nolea(MD5_STATE<uint32_t>* __restrict__ state, const void* __restrict__ data) {
	uint32_t A;
	uint32_t B;
	uint32_t C;
	uint32_t D;
	const uint32_t* _in = (const uint32_t*)data;
	void *tmp1, *tmp2;
	
	A = state->A;
	B = state->B;
	C = state->C;
	D = state->D;
	
#define ROUND_F(A, B, C, D, NEXT_IN, K, R) \
	"xorl %k[" STR(C) "], %k[TMP1]\n" \
	"addl $" STR(K) ", %k[" STR(A) "]\n" \
	"andl %k[" STR(B) "], %k[TMP1]\n" \
	"xorl %k[" STR(D) "], %k[TMP1]\n" \
	"addl " NEXT_IN ", %k[" STR(D) "]\n" \
	"addl %k[TMP1], %k[" STR(A) "]\n" \
	"roll $" STR(R) ", %k[" STR(A) "]\n" \
	"movl %k[" STR(C) "], %k[TMP1]\n" \
	"addl %k[" STR(B) "], %k[" STR(A) "]\n"
#define ROUND_H(A, B, C, D, NEXT_IN, K, R) \
	"xorl %k[" STR(C) "], %k[TMP1]\n" \
	"addl $" STR(K) ", %k[" STR(A) "]\n" \
	"addl " NEXT_IN ", %k[" STR(D) "]\n" \
	"xorl %k[" STR(B) "], %k[TMP1]\n" \
	"addl %k[TMP1], %k[" STR(A) "]\n" \
	"roll $" STR(R) ", %k[" STR(A) "]\n" \
	"movl %k[" STR(C) "], %k[TMP1]\n" \
	"addl %k[" STR(B) "], %k[" STR(A) "]\n"
#define ROUND_I(A, B, C, D, NEXT_IN, K, R) \
	"notl %k[TMP1]\n" \
	"addl $" STR(K) ", %k[" STR(A) "]\n" \
	"addl " NEXT_IN ", %k[" STR(D) "]\n" \
	"orl %k[" STR(B) "], %k[TMP1]\n" \
	"xorl %k[" STR(C) "], %k[TMP1]\n" \
	"addl %k[TMP1], %k[" STR(A) "]\n" \
	"roll $" STR(R) ", %k[" STR(A) "]\n" \
	"movl %k[" STR(C) "], %k[TMP1]\n" \
	"addl %k[" STR(B) "], %k[" STR(A) "]\n"
#define ROUND_I_LAST(A, B, C, D, K, R) \
	"notl %k[TMP1]\n" \
	"addl $" STR(K) ", %k[" STR(A) "]\n" \
	"orl %k[" STR(B) "], %k[TMP1]\n" \
	"xorl %k[" STR(C) "], %k[TMP1]\n" \
	"addl %k[TMP1], %k[" STR(A) "]\n" \
	"roll $" STR(R) ", %k[" STR(A) "]\n" \
	"addl %k[" STR(B) "], %k[" STR(A) "]\n"

#define ROUND_G(A, B, C, D, NEXT_IN, K, R) \
	"notl %k[TMP1]\n" \
	"movl %k[" STR(D) "], %k[TMP2]\n" \
	"addl $" STR(K) ", %k[" STR(A) "]\n" \
	"addl " NEXT_IN ", %k[" STR(D) "]\n" \
	"andl %k[" STR(C) "], %k[TMP1]\n" \
	"andl %k[" STR(B) "], %k[TMP2]\n" \
	"orl %k[TMP2], %k[TMP1]\n" \
	"addl %k[TMP1], %k[" STR(A) "]\n" \
	"roll $" STR(R) ", %k[" STR(A) "]\n" \
	"movl %k[" STR(C) "], %k[TMP1]\n" \
	"addl %k[" STR(B) "], %k[" STR(A) "]\n"

#define RF4(I, i0, i1, i2, i3, k0, k1, k2, k3) \
	ROUND_F(A, I##B, I##C, I##D, "%[input" STR(i0) "]", k0, 7) \
	ROUND_F(D, A, I##B, I##C, "%[input" STR(i1) "]", k1, 12) \
	ROUND_F(C, D, A, I##B, "%[input" STR(i2) "]", k2, 17) \
	ROUND_F(B, C, D, A, "%[input" STR(i3) "]", k3, 22)
	
#define RG4(i0, i1, i2, i3, k0, k1, k2, k3) \
	ROUND_G(A, B, C, D, "%[input" STR(i0) "]", k0, 5) \
	ROUND_G(D, A, B, C, "%[input" STR(i1) "]", k1, 9) \
	ROUND_G(C, D, A, B, "%[input" STR(i2) "]", k2, 14) \
	ROUND_G(B, C, D, A, "%[input" STR(i3) "]", k3, 20)
	
#define RH4(i0, i1, i2, i3, k0, k1, k2, k3) \
	ROUND_H(A, B, C, D, "%[input" STR(i0) "]", k0, 4) \
	ROUND_H(D, A, B, C, "%[input" STR(i1) "]", k1, 11) \
	ROUND_H(C, D, A, B, "%[input" STR(i2) "]", k2, 16) \
	ROUND_H(B, C, D, A, "%[input" STR(i3) "]", k3, 23)
	
#define RI4(i0, i1, i2, i3, k0, k1, k2, k3) \
	ROUND_I(A, B, C, D, "%[input" STR(i0) "]", k0, 6) \
	ROUND_I(D, A, B, C, "%[input" STR(i1) "]", k1, 10) \
	ROUND_I(C, D, A, B, "%[input" STR(i2) "]", k2, 15) \
	ROUND_I(B, C, D, A, "%[input" STR(i3) "]", k3, 21)
	
	__asm__(
		"addl %[input0], %k[A]\n"
		"movl %k[D], %k[TMP1]\n"
		RF4(,  1,  2,  3,  4,  -0x28955b88, -0x173848aa, 0x242070db, -0x3e423112)
		RF4(,  5,  6,  7,  8,  -0x0a83f051, 0x4787c62a, -0x57cfb9ed, -0x02b96aff)
		RF4(,  9, 10, 11, 12,  0x698098d8, -0x74bb0851, -0x0000a44f, -0x76a32842)
		RF4(, 13, 14, 15,  1,  0x6b901122, -0x02678e6d, -0x5986bc72, 0x49b40821)
	: [TMP1]"=&R"(tmp1), [TMP2]"=&r"(tmp2),
	  [A]"+&R"(A), [B]"+&R"(B), [C]"+&R"(C), [D]"+&R"(D)
	: ASM_INPUTS
	:);
	
	__asm__(
		RG4( 6, 11,  0,  5,  -0x09e1da9e, -0x3fbf4cc0, 0x265e5a51, -0x16493856)
		RG4(10, 15,  4,  9,  -0x29d0efa3, 0x02441453, -0x275e197f, -0x182c0438)
		RG4(14,  3,  8, 13,  0x21e1cde6, -0x3cc8f82a, -0x0b2af279, 0x455a14ed)
		RG4( 2,  7, 12,  5,  -0x561c16fb, -0x03105c08, 0x676f02d9, -0x72d5b376)
		
		RH4( 8, 11, 14,  1,  -0x0005c6be, -0x788e097f, 0x6d9d6122, -0x021ac7f4)
		RH4( 4,  7, 10, 13,  -0x5b4115bc, 0x4bdecfa9, -0x0944b4a0, -0x41404390)
		RH4( 0,  3,  6,  9,  0x289b7ec6, -0x155ed806, -0x2b10cf7b, 0x04881d05)
		RH4(12, 15,  2,  0,  -0x262b2fc7, -0x1924661b, 0x1fa27cf8, -0x3b53a99b)
		
		RI4( 7, 14,  5, 12,  -0x0bd6ddbc, 0x432aff97, -0x546bdc59, -0x036c5fc7)
		RI4( 3, 10,  1,  8,  0x655b59c3, -0x70f3336e, -0x00100b83, -0x7a7ba22f)
		RI4(15,  6, 13,  4,  0x6fa87e4f, -0x01d31920, -0x5cfebcec, 0x4e0811a1)
		
		ROUND_I(A, B, C, D, "%[input11]", -0x08ac817e, 6)
		ROUND_I(D, A, B, C, "%[input2]" , -0x42c50dcb, 10)
		ROUND_I(C, D, A, B, "%[input9]" , 0x2ad7d2bb, 15)
		ROUND_I_LAST(B, C, D, A, -0x14792c6f, 21)
	: [TMP1]"+&R"(tmp1), [TMP2]"+&r"(tmp2),
	  [A]"+&R"(A), [B]"+&R"(B), [C]"+&R"(C), [D]"+&R"(D)
	: ASM_INPUTS
	:);
	
	state->A += A;
	state->B += B;
	state->C += C;
	state->D += D;
}
#undef ROUND_G

static inline __attribute__((always_inline)) void md5_block_noleag(MD5_STATE<uint32_t>* __restrict__ state, const void* __restrict__ data) {
	uint32_t A;
	uint32_t B;
	uint32_t C;
	uint32_t D;
	const uint32_t* _in = (const uint32_t*)data;
	void *tmp1, *tmp2;
	
	A = state->A;
	B = state->B;
	C = state->C;
	D = state->D;
	
#define ROUND_G(A, B, C, D, NEXT_IN, K, R) \
	"notl %k[TMP1]\n" \
	"addl $" STR(K) ", %k[" STR(A) "]\n" \
	"andl %k[" STR(C) "], %k[TMP1]\n" \
	"movl %k[" STR(D) "], %k[TMP2]\n" \
	"addl " NEXT_IN ", %k[" STR(D) "]\n" \
	"addl %k[TMP1], %k[" STR(A) "]\n" \
	"andl %k[" STR(B) "], %k[TMP2]\n" \
	"addl %k[TMP2], %k[" STR(A) "]\n" \
	"roll $" STR(R) ", %k[" STR(A) "]\n" \
	"movl %k[" STR(C) "], %k[TMP1]\n" \
	"addl %k[" STR(B) "], %k[" STR(A) "]\n"

	__asm__(
		"addl %[input0], %k[A]\n"
		"movl %k[D], %k[TMP1]\n"
		RF4(,  1,  2,  3,  4,  -0x28955b88, -0x173848aa, 0x242070db, -0x3e423112)
		RF4(,  5,  6,  7,  8,  -0x0a83f051, 0x4787c62a, -0x57cfb9ed, -0x02b96aff)
		RF4(,  9, 10, 11, 12,  0x698098d8, -0x74bb0851, -0x0000a44f, -0x76a32842)
		RF4(, 13, 14, 15,  1,  0x6b901122, -0x02678e6d, -0x5986bc72, 0x49b40821)
	: [TMP1]"=&R"(tmp1), [TMP2]"=&r"(tmp2),
	  [A]"+&R"(A), [B]"+&R"(B), [C]"+&R"(C), [D]"+&R"(D)
	: ASM_INPUTS
	:);
	
	__asm__(
		RG4( 6, 11,  0,  5,  -0x09e1da9e, -0x3fbf4cc0, 0x265e5a51, -0x16493856)
		RG4(10, 15,  4,  9,  -0x29d0efa3, 0x02441453, -0x275e197f, -0x182c0438)
		RG4(14,  3,  8, 13,  0x21e1cde6, -0x3cc8f82a, -0x0b2af279, 0x455a14ed)
		RG4( 2,  7, 12,  5,  -0x561c16fb, -0x03105c08, 0x676f02d9, -0x72d5b376)
		
		RH4( 8, 11, 14,  1,  -0x0005c6be, -0x788e097f, 0x6d9d6122, -0x021ac7f4)
		RH4( 4,  7, 10, 13,  -0x5b4115bc, 0x4bdecfa9, -0x0944b4a0, -0x41404390)
		RH4( 0,  3,  6,  9,  0x289b7ec6, -0x155ed806, -0x2b10cf7b, 0x04881d05)
		RH4(12, 15,  2,  0,  -0x262b2fc7, -0x1924661b, 0x1fa27cf8, -0x3b53a99b)
		
		RI4( 7, 14,  5, 12,  -0x0bd6ddbc, 0x432aff97, -0x546bdc59, -0x036c5fc7)
		RI4( 3, 10,  1,  8,  0x655b59c3, -0x70f3336e, -0x00100b83, -0x7a7ba22f)
		RI4(15,  6, 13,  4,  0x6fa87e4f, -0x01d31920, -0x5cfebcec, 0x4e0811a1)
		
		ROUND_I(A, B, C, D, "%[input11]", -0x08ac817e, 6)
		ROUND_I(D, A, B, C, "%[input2]" , -0x42c50dcb, 10)
		ROUND_I(C, D, A, B, "%[input9]" , 0x2ad7d2bb, 15)
		ROUND_I_LAST(B, C, D, A, -0x14792c6f, 21)
	: [TMP1]"+&R"(tmp1), [TMP2]"+&r"(tmp2),
	  [A]"+&R"(A), [B]"+&R"(B), [C]"+&R"(C), [D]"+&R"(D)
	: ASM_INPUTS
	:);
	
	state->A += A;
	state->B += B;
	state->C += C;
	state->D += D;
}

#undef ROUND_H

static inline __attribute__((always_inline)) void md5_block_noleagh(MD5_STATE<uint32_t>* __restrict__ state, const void* __restrict__ data) {
	uint32_t A;
	uint32_t B;
	uint32_t C;
	uint32_t D;
	const uint32_t* _in = (const uint32_t*)data;
	void *tmp1, *tmp2;
	
	A = state->A;
	B = state->B;
	C = state->C;
	D = state->D;
	
#define ROUND_H(A, B, C, D, NEXT_IN, K, R) \
	"addl $" STR(K) ", %k[" STR(A) "]\n" \
	"xorl %k[" STR(B) "], %k[TMP1]\n" \
	"addl %k[TMP1], %k[" STR(A) "]\n" \
	"xorl %k[" STR(D) "], %k[TMP1]\n" \
	"addl " NEXT_IN ", %k[" STR(D) "]\n" \
	"roll $" STR(R) ", %k[" STR(A) "]\n" \
	"addl %k[" STR(B) "], %k[" STR(A) "]\n"
	
	__asm__(
		"addl %[input0], %k[A]\n"
		"movl %k[D], %k[TMP1]\n"
		RF4(,  1,  2,  3,  4,  -0x28955b88, -0x173848aa, 0x242070db, -0x3e423112)
		RF4(,  5,  6,  7,  8,  -0x0a83f051, 0x4787c62a, -0x57cfb9ed, -0x02b96aff)
		RF4(,  9, 10, 11, 12,  0x698098d8, -0x74bb0851, -0x0000a44f, -0x76a32842)
		RF4(, 13, 14, 15,  1,  0x6b901122, -0x02678e6d, -0x5986bc72, 0x49b40821)
	: [TMP1]"=&R"(tmp1), [TMP2]"=&r"(tmp2),
	  [A]"+&R"(A), [B]"+&R"(B), [C]"+&R"(C), [D]"+&R"(D)
	: ASM_INPUTS
	:);
	
	__asm__(
		RG4( 6, 11,  0,  5,  -0x09e1da9e, -0x3fbf4cc0, 0x265e5a51, -0x16493856)
		RG4(10, 15,  4,  9,  -0x29d0efa3, 0x02441453, -0x275e197f, -0x182c0438)
		RG4(14,  3,  8, 13,  0x21e1cde6, -0x3cc8f82a, -0x0b2af279, 0x455a14ed)
		RG4( 2,  7, 12,  5,  -0x561c16fb, -0x03105c08, 0x676f02d9, -0x72d5b376)
		
		"xorl %k[C], %k[TMP1]\n"
		"addl $-0x0005c6be, %k[A]\n"
		"xorl %k[B], %k[TMP1]\n"
		"addl %k[TMP1], %k[A]\n"
		"xorl %k[D], %k[TMP1]\n"
		"addl %[input8], %k[D]\n"
		"roll $4, %k[A]\n"
		"addl %k[B], %k[A]\n"
		ROUND_H(D, A, B, C, "%[input11]", -0x788e097f, 11)
		ROUND_H(C, D, A, B, "%[input14]", 0x6d9d6122, 16)
		ROUND_H(B, C, D, A, "%[input1]", -0x021ac7f4, 23)
		RH4( 4,  7, 10, 13,  -0x5b4115bc, 0x4bdecfa9, -0x0944b4a0, -0x41404390)
		RH4( 0,  3,  6,  9,  0x289b7ec6, -0x155ed806, -0x2b10cf7b, 0x04881d05)
		RH4(12, 15,  2,  0,  -0x262b2fc7, -0x1924661b, 0x1fa27cf8, -0x3b53a99b)
		// above contains a redundant XOR - TODO: consider eliminating
		"movl %k[D], %k[TMP1]\n"
		
		RI4( 7, 14,  5, 12,  -0x0bd6ddbc, 0x432aff97, -0x546bdc59, -0x036c5fc7)
		RI4( 3, 10,  1,  8,  0x655b59c3, -0x70f3336e, -0x00100b83, -0x7a7ba22f)
		RI4(15,  6, 13,  4,  0x6fa87e4f, -0x01d31920, -0x5cfebcec, 0x4e0811a1)
		
		ROUND_I(A, B, C, D, "%[input11]", -0x08ac817e, 6)
		ROUND_I(D, A, B, C, "%[input2]" , -0x42c50dcb, 10)
		ROUND_I(C, D, A, B, "%[input9]" , 0x2ad7d2bb, 15)
		ROUND_I_LAST(B, C, D, A, -0x14792c6f, 21)
	: [TMP1]"+&R"(tmp1), [TMP2]"+&r"(tmp2),
	  [A]"+&R"(A), [B]"+&R"(B), [C]"+&R"(C), [D]"+&R"(D)
	: ASM_INPUTS
	:);
	
	state->A += A;
	state->B += B;
	state->C += C;
	state->D += D;
}

#undef ROUND_F_IA
#undef ROUND_F_A

#ifdef PLATFORM_AMD64
#undef ROUND_F
#undef ROUND_G
#undef ROUND_H
#undef ROUND_I
#undef ROUND_I_LAST
#undef RF4
#undef RG4
#undef RH4
#undef RI4


static inline __attribute__((always_inline)) void md5_block_cache4(MD5_STATE<uint32_t>* __restrict__ state, const void* __restrict__ data) {
	uint32_t A = state->A;
	uint32_t B = state->B;
	uint32_t C = state->C;
	uint32_t D = state->D;
	const uint32_t* _in = (const uint32_t*)data;
	uintptr_t tmp1, tmp2, tmp3;
	uintptr_t cache0, cache4, cache8, cache12;
	
#define ROUND_F(A, B, C, D, INPUT, NEXT_IN, K, R) \
	"xorl %k[" STR(C) "], %k[TMP1]\n" \
	"leal " STR(K) "(%k[" STR(A) "], %k[" INPUT "]), %k[" STR(A) "]\n" \
	"andl %k[" STR(B) "], %k[TMP1]\n" \
	NEXT_IN \
	"xorl %k[" STR(D) "], %k[TMP1]\n" \
	"addl %k[TMP1], %k[" STR(A) "]\n" \
	"roll $" STR(R) ", %k[" STR(A) "]\n" \
	"movl %k[" STR(C) "], %k[TMP1]\n" \
	"addl %k[" STR(B) "], %k[" STR(A) "]\n"
#define ROUND_H(A, B, C, D, INPUT, NEXT_IN, K, R) \
	"xorl %k[" STR(C) "], %k[TMP1]\n" \
	"leal " STR(K) "(%k[" STR(A) "], %k[" INPUT "]), %k[" STR(A) "]\n" \
	"xorl %k[" STR(B) "], %k[TMP1]\n" \
	NEXT_IN \
	"addl %k[TMP1], %k[" STR(A) "]\n" \
	"roll $" STR(R) ", %k[" STR(A) "]\n" \
	"movl %k[" STR(C) "], %k[TMP1]\n" \
	"addl %k[" STR(B) "], %k[" STR(A) "]\n"
#define ROUND_I(A, B, C, D, INPUT, NEXT_IN, K, R) \
	"notl %k[TMP1]\n" \
	"leal " STR(K) "(%k[" STR(A) "], %k[" INPUT "]), %k[" STR(A) "]\n" \
	"orl %k[" STR(B) "], %k[TMP1]\n" \
	NEXT_IN \
	"xorl %k[" STR(C) "], %k[TMP1]\n" \
	"addl %k[TMP1], %k[" STR(A) "]\n" \
	"roll $" STR(R) ", %k[" STR(A) "]\n" \
	"movl %k[" STR(C) "], %k[TMP1]\n" \
	"addl %k[" STR(B) "], %k[" STR(A) "]\n"
#define ROUND_I_LAST(A, B, C, D, INPUT, K, R) \
	"notl %k[TMP1]\n" \
	"leal " STR(K) "(%k[" STR(A) "], %k[" INPUT "]), %k[" STR(A) "]\n" \
	"orl %k[" STR(B) "], %k[TMP1]\n" \
	"xorl %k[" STR(C) "], %k[TMP1]\n" \
	"addl %k[TMP1], %k[" STR(A) "]\n" \
	"roll $" STR(R) ", %k[" STR(A) "]\n" \
	"addl %k[" STR(B) "], %k[" STR(A) "]\n"

#define ROUND_G(A, B, C, D, INPUT, NEXT_IN, K, R) \
	"notl %k[TMP1]\n" \
	"movl %k[" STR(D) "], %k[TMP3]\n" \
	"leal " STR(K) "(%k[" STR(A) "], %k[" INPUT "]), %k[" STR(A) "]\n" \
	"andl %k[" STR(C) "], %k[TMP1]\n" \
	NEXT_IN \
	"andl %k[" STR(B) "], %k[TMP3]\n" \
	"orl %k[TMP3], %k[TMP1]\n" \
	"addl %k[TMP1], %k[" STR(A) "]\n" \
	"roll $" STR(R) ", %k[" STR(A) "]\n" \
	"movl %k[" STR(C) "], %k[TMP1]\n" \
	"addl %k[" STR(B) "], %k[" STR(A) "]\n"


#define ASM_PARAMS(i0, i1, i2, i3, cn) \
	[A]"+&R"(A), [B]"+&R"(B), [C]"+&R"(C), [D]"+&R"(D), [TMP1]"+&R"(tmp1), [TMP2]"+&r"(tmp2), [TMP3]"+&r"(tmp3), \
	  [cache]"+&r"(cache##cn) \
	: [input0]"m"(_in[i0]), [input1]"m"(_in[i1]), [input2]"m"(_in[i2]), [input3]"m"(_in[i3]) \
	:
	
	
#define ASM_PARAMS2(i0, i1, i2, i3, i4, r1, r2) \
	[A]"+&R"(A), [B]"+&R"(B), [C]"+&R"(C), [D]"+&R"(D), [TMP1]r1(tmp1), [TMP2]r2(tmp2), [TMP3]"=&r"(tmp3), \
	  [cache]"+&r"(cache##i0), [cacheN]"=&r"(cache##i4) \
	: [input0]"m"(_in[i0]), [input1]"m"(_in[i1]), [input2]"m"(_in[i2]), [input3]"m"(_in[i3]), [input4]"m"(_in[i4]) \
	:
#define RF4(k0, k1, k2, k3, lastStmt) \
	ROUND_F(A, B, C, D, "cache", "movl %[input1], %k[TMP2]\n", k0, 7) \
	ROUND_F(D, A, B, C, "TMP2", "movl %[input2], %k[TMP2]\n", k1, 12) \
	ROUND_F(C, D, A, B, "TMP2", "movl %[input3], %k[TMP2]\n", k2, 17) \
	ROUND_F(B, C, D, A, "TMP2", lastStmt, k3, 22)
	
#define RG4(i0, i1, i2, i3, k0, k1, k2, k3, lastStmt) __asm__( \
	ROUND_G(A, B, C, D, "TMP2", "movl %[input0], %k[TMP2]\n", k0, 5) \
	ROUND_G(D, A, B, C, "TMP2", "movl %[input1], %k[TMP2]\n", k1, 9) \
	ROUND_G(C, D, A, B, "TMP2", "", k2, 14) \
	ROUND_G(B, C, D, A, "cache", lastStmt, k3, 20) \
: ASM_PARAMS(i0, i1, i2, i3, i2));
	
#define RH4(i0, i1, i2, i3, k0, k1, k2, k3, lastStmt) __asm__( \
	ROUND_H(A, B, C, D, "TMP2", "movl %[input1], %k[TMP2]\n", k0, 4) \
	ROUND_H(D, A, B, C, "cache", "", k1, 11) \
	ROUND_H(C, D, A, B, "TMP2", "movl %[input2], %k[TMP2]\n", k2, 16) \
	ROUND_H(B, C, D, A, "TMP2", lastStmt, k3, 23) \
: ASM_PARAMS(i0, i1, i2, i3, i0));
	
#define RI4(i0, i1, i2, i3, k0, k1, k2, k3) __asm__( \
	ROUND_I(A, B, C, D, "cache", "movl %[input1], %k[TMP2]\n", k0, 6) \
	ROUND_I(D, A, B, C, "TMP2", "movl %[input2], %k[TMP2]\n", k1, 10) \
	ROUND_I(C, D, A, B, "TMP2", "movl %[input3], %k[TMP2]\n", k2, 15) \
	ROUND_I(B, C, D, A, "TMP2", "", k3, 21) \
: ASM_PARAMS(i0, i1, i2, i3, i0));
	
	memcpy(&cache0, _in, 4);
	__asm__(
		"movl %k[D], %k[TMP1]\n"
		RF4(-0x28955b88, -0x173848aa, 0x242070db, -0x3e423112, "movl %[input4], %k[cacheN]\n")
	: ASM_PARAMS2(0, 1, 2, 3, 4, "=&R", "=&r"));
	__asm__(
		RF4(-0x0a83f051, 0x4787c62a, -0x57cfb9ed, -0x02b96aff, "movl %[input4], %k[cacheN]\n")
	: ASM_PARAMS2(4, 5, 6, 7, 8, "+&R", "+&r"));
	__asm__(
		RF4(0x698098d8, -0x74bb0851, -0x0000a44f, -0x76a32842, "movl %[input4], %k[cacheN]\n")
	: ASM_PARAMS2(8, 9, 10, 11, 12, "+&R", "+&r"));
	__asm__(
		RF4(0x6b901122, -0x02678e6d, -0x5986bc72, 0x49b40821, "movl %[input4], %k[TMP2]\n")
	: [A]"+&R"(A), [B]"+&R"(B), [C]"+&R"(C), [D]"+&R"(D), [TMP1]"+&R"(tmp1), [TMP2]"+&r"(tmp2),
	  [cache]"+&r"(cache12)
	: [input0]"m"(_in[12]), [input1]"m"(_in[13]), [input2]"m"(_in[14]), [input3]"m"(_in[15]), [input4]"m"(_in[1])
	:);
	
	RG4( 6, 11,  0,  5,  -0x09e1da9e, -0x3fbf4cc0, 0x265e5a51, -0x16493856, "movl %[input3], %k[TMP2]\n")
	RG4(10, 15,  4,  9,  -0x29d0efa3, 0x02441453, -0x275e197f, -0x182c0438, "movl %[input3], %k[TMP2]\n")
	RG4(14,  3,  8, 13,  0x21e1cde6, -0x3cc8f82a, -0x0b2af279, 0x455a14ed, "movl %[input3], %k[TMP2]\n")
	RG4( 2,  7, 12,  5,  -0x561c16fb, -0x03105c08, 0x676f02d9, -0x72d5b376, "movl %[input3], %k[TMP2]\n")
	
	RH4( 8, 11, 14,  1,  -0x0005c6be, -0x788e097f, 0x6d9d6122, -0x021ac7f4, "movl %[input3], %k[TMP2]\n")
	RH4( 4,  7, 10, 13,  -0x5b4115bc, 0x4bdecfa9, -0x0944b4a0, -0x41404390, "movl %[input3], %k[TMP2]\n")
	RH4( 0,  3,  6,  9,  0x289b7ec6, -0x155ed806, -0x2b10cf7b, 0x04881d05, "movl %[input3], %k[TMP2]\n")
	RH4(12, 15,  2,  7,  -0x262b2fc7, -0x1924661b, 0x1fa27cf8, -0x3b53a99b, "")
	
	RI4( 0,  7, 14,  5,  -0x0bd6ddbc, 0x432aff97, -0x546bdc59, -0x036c5fc7)
	RI4(12,  3, 10,  1, 0x655b59c3, -0x70f3336e, -0x00100b83, -0x7a7ba22f)
	RI4( 8, 15,  6, 13, 0x6fa87e4f, -0x01d31920, -0x5cfebcec, 0x4e0811a1)
	
	__asm__(
		ROUND_I(A, B, C, D, "cache", "movl %[input1], %k[TMP2]\n", -0x08ac817e, 6)
		ROUND_I(D, A, B, C, "TMP2", "movl %[input2], %k[TMP2]\n", -0x42c50dcb, 10)
		ROUND_I(C, D, A, B, "TMP2", "movl %[input3], %k[TMP2]\n", 0x2ad7d2bb, 15)
		ROUND_I_LAST(B, C, D, A, "TMP2", -0x14792c6f, 21)
	: ASM_PARAMS(4, 11, 2, 9, 4));
	state->A += A;
	state->B += B;
	state->C += C;
	state->D += D;
}
#undef ROUND_F
#undef ROUND_G
#undef ROUND_H
#undef ROUND_I
#undef ROUND_I_LAST
#undef RF4
#undef RG4
#undef RH4
#undef RI4
#undef ASM_PARAMS
#undef ASM_PARAMS2


// avoiding loads by doing a 64-bit load, then shifting, doesn't seem to be worth the effort
//#define MD5_CACHE_LOADSHIFT 1



static inline __attribute__((always_inline)) void md5_block_cache8(MD5_STATE<uint32_t>* __restrict__ state, const void* __restrict__ data) {
	uint32_t A = state->A;
	uint32_t B = state->B;
	uint32_t C = state->C;
	uint32_t D = state->D;
	const uint32_t* _in = (const uint32_t*)data;
	uintptr_t tmp1, tmp2;
	uintptr_t cache1, cache3, cache5, cache7, cache9, cache11, cache13, cache15;
	
#define ROUND_F(A, B, C, D, INPUT, NEXT_IN, K, R) \
	"xorl %k[" STR(C) "], %k[TMP1]\n" \
	"leal " STR(K) "(%k[" STR(A) "], %k[" INPUT "]), %k[" STR(A) "]\n" \
	"andl %k[" STR(B) "], %k[TMP1]\n" \
	NEXT_IN \
	"xorl %k[" STR(D) "], %k[TMP1]\n" \
	"addl %k[TMP1], %k[" STR(A) "]\n" \
	"roll $" STR(R) ", %k[" STR(A) "]\n" \
	"movl %k[" STR(C) "], %k[TMP1]\n" \
	"addl %k[" STR(B) "], %k[" STR(A) "]\n"
#define ROUND_H(A, B, C, D, INPUT, NEXT_IN, K, R) \
	"xorl %k[" STR(C) "], %k[TMP1]\n" \
	"leal " STR(K) "(%k[" STR(A) "], %k[" INPUT "]), %k[" STR(A) "]\n" \
	"xorl %k[" STR(B) "], %k[TMP1]\n" \
	NEXT_IN \
	"addl %k[TMP1], %k[" STR(A) "]\n" \
	"roll $" STR(R) ", %k[" STR(A) "]\n" \
	"movl %k[" STR(C) "], %k[TMP1]\n" \
	"addl %k[" STR(B) "], %k[" STR(A) "]\n"
#define ROUND_I(A, B, C, D, INPUT, NEXT_IN, K, R) \
	"notl %k[TMP1]\n" \
	"leal " STR(K) "(%k[" STR(A) "], %k[" INPUT "]), %k[" STR(A) "]\n" \
	"orl %k[" STR(B) "], %k[TMP1]\n" \
	NEXT_IN \
	"xorl %k[" STR(C) "], %k[TMP1]\n" \
	"addl %k[TMP1], %k[" STR(A) "]\n" \
	"roll $" STR(R) ", %k[" STR(A) "]\n" \
	"movl %k[" STR(C) "], %k[TMP1]\n" \
	"addl %k[" STR(B) "], %k[" STR(A) "]\n"
#define ROUND_I_LAST(A, B, C, D, INPUT, K, R) \
	"notl %k[TMP1]\n" \
	"leal " STR(K) "(%k[" STR(A) "], %k[" INPUT "]), %k[" STR(A) "]\n" \
	"orl %k[" STR(B) "], %k[TMP1]\n" \
	"xorl %k[" STR(C) "], %k[TMP1]\n" \
	"addl %k[TMP1], %k[" STR(A) "]\n" \
	"roll $" STR(R) ", %k[" STR(A) "]\n" \
	"addl %k[" STR(B) "], %k[" STR(A) "]\n"

#define ROUND_G(A, B, C, D, INPUT, NEXT_IN, K, R) \
	"notl %k[TMP1]\n" \
	"leal " STR(K) "(%k[" STR(A) "], %k[" INPUT "]), %k[" STR(A) "]\n" \
	"movl %k[" STR(D) "], %k[TMP2]\n" \
	"andl %k[" STR(C) "], %k[TMP1]\n" \
	"andl %k[" STR(B) "], %k[TMP2]\n" \
	"orl %k[TMP2], %k[TMP1]\n" \
	NEXT_IN \
	"addl %k[TMP1], %k[" STR(A) "]\n" \
	"roll $" STR(R) ", %k[" STR(A) "]\n" \
	"movl %k[" STR(C) "], %k[TMP1]\n" \
	"addl %k[" STR(B) "], %k[" STR(A) "]\n"

#define ASM_PARAMS(i0, i1, i2, i3, i4) \
	[A]"+&R"(A), [B]"+&R"(B), [C]"+&R"(C), [D]"+&R"(D), [TMP1]"+&R"(tmp1), [TMP2]"+&r"(tmp2), \
	  [cache0]"+&r"(cache##i0), [cache2]"+&r"(cache##i2) \
	: [input0]"m"(_in[i0]), [input1]"m"(_in[i1]), [input2]"m"(_in[i2]), [input3]"m"(_in[i3]), [input4]"m"(_in[i4]) \
	:
	
	
#ifdef MD5_CACHE_LOADSHIFT
#define ASM_PARAMS2(i0, i1, i2, i3, i4, r1, r2) \
	[A]"+&R"(A), [B]"+&R"(B), [C]"+&R"(C), [D]"+&R"(D), [TMP1]r1(tmp1), [TMP2]r2(tmp2), \
	  [cache0]"+&r"(cache##i0), [cache2]"=&r"(cache##i2), [cache4]"=&r"(cache##i4) \
	: [input0]"m"(_in[i0]), [input1]"m"(_in[i1]), [input2]"m"(_in[i2]), [input3]"m"(_in[i3]), [input4]"m"(_in[i4]) \
	:
#define RF4(k0, k1, k2, k3, lastStmt) \
	ROUND_F(A, B, C, D, "cache0", "shr $32, %[cache0]\n", k0, 7) \
	ROUND_F(D, A, B, C, "cache0", "movq %[input1], %[cache2]\n", k1, 12) \
	ROUND_F(C, D, A, B, "cache2", "shr $32, %[cache2]\n", k2, 17) \
	ROUND_F(B, C, D, A, "cache2", lastStmt, k3, 22)
#else
#define ASM_PARAMS2(i0, i1, i2, i3, i4, r1, r2) \
	[A]"+&R"(A), [B]"+&R"(B), [C]"+&R"(C), [D]"+&R"(D), [TMP1]r1(tmp1), [TMP2]"+&r"(tmp2), \
	  [cache0]"=&r"(cache##i0), [cache2]"=&r"(cache##i2), [cache4]"=&r"(cache##i4) \
	: [input0]"m"(_in[i0]), [input1]"m"(_in[i1]), [input2]"m"(_in[i2]), [input3]"m"(_in[i3]), [input4]"m"(_in[i4]) \
	:
#define RF4(k0, k1, k2, k3, lastStmt) \
	ROUND_F(A, B, C, D, "TMP2", "movl %[input0], %k[cache0]\n", k0, 7) \
	ROUND_F(D, A, B, C, "cache0", "movl %[input1], %k[TMP2]\n", k1, 12) \
	ROUND_F(C, D, A, B, "TMP2", "movl %[input2], %k[cache2]\n", k2, 17) \
	ROUND_F(B, C, D, A, "cache2", lastStmt, k3, 22)
#endif
	
#define RG4(i0, i1, i2, i3, i4, k0, k1, k2, k3, lastStmt) __asm__( \
	ROUND_G(A, B, C, D, "cache0", "movl %[input1], %k[TMP2]\n", k0, 5) \
	ROUND_G(D, A, B, C, "TMP2", "", k1, 9) \
	ROUND_G(C, D, A, B, "cache2", "movl %[input3], %k[TMP2]\n", k2, 14) \
	ROUND_G(B, C, D, A, "TMP2", lastStmt, k3, 20) \
: ASM_PARAMS(i0, i1, i2, i3, i4));
	
#define RH4(i0, i1, i2, i3, i4, k0, k1, k2, k3, lastStmt) __asm__( \
	ROUND_H(A, B, C, D, "cache0", "", k0, 4) \
	ROUND_H(D, A, B, C, "TMP2", "movl %[input3], %k[TMP2]\n", k1, 11) \
	ROUND_H(C, D, A, B, "cache2", "", k2, 16) \
	ROUND_H(B, C, D, A, "TMP2", lastStmt, k3, 23) \
: ASM_PARAMS(i0, i1, i2, i3, i4));
	
#define RI4(i0, i1, i2, i3, k0, k1, k2, k3) __asm__( \
	ROUND_I(A, B, C, D, "TMP2", "movl %[input1], %k[TMP2]\n", k0, 6) \
	ROUND_I(D, A, B, C, "cache0", "", k1, 10) \
	ROUND_I(C, D, A, B, "TMP2", "movl %[input3], %k[TMP2]\n", k2, 15) \
	ROUND_I(B, C, D, A, "cache2", "", k3, 21) \
: ASM_PARAMS(i0, i1, i2, i3, 0));
	
#ifdef MD5_CACHE_LOADSHIFT
	memcpy(&cache1, _in, 8);
# define MD5_F_LOAD "movq %[input3], %[cache4]\n"
#else
	memcpy(&tmp2, _in, 4);
# define MD5_F_LOAD "movl %[input3], %k[TMP2]\n"
#endif
	__asm__(
		"movl %k[D], %k[TMP1]\n"
		RF4(-0x28955b88, -0x173848aa, 0x242070db, -0x3e423112, MD5_F_LOAD)
	: ASM_PARAMS2(1, 2, 3, 4, 5, "=&R", "=&r"));
	__asm__(
		RF4(-0x0a83f051, 0x4787c62a, -0x57cfb9ed, -0x02b96aff, MD5_F_LOAD)
	: ASM_PARAMS2(5, 6, 7, 8, 9, "+&R", "+&r"));
	__asm__(
		RF4(0x698098d8, -0x74bb0851, -0x0000a44f, -0x76a32842, MD5_F_LOAD)
	: ASM_PARAMS2(9, 10, 11, 12, 13, "+&R", "+&r"));
	__asm__(
		RF4(0x6b901122, -0x02678e6d, -0x5986bc72, 0x49b40821, "")
	: [A]"+&R"(A), [B]"+&R"(B), [C]"+&R"(C), [D]"+&R"(D), [TMP1]"+&R"(tmp1), [TMP2]"+&r"(tmp2),
	  [cache0]"+&r"(cache13), [cache2]"=&r"(cache15)
	: [input0]"m"(_in[13]), [input1]"m"(_in[14]), [input2]"m"(_in[15])
	:);
	
	RG4( 1,  6, 11,  0,  0,  -0x09e1da9e, -0x3fbf4cc0, 0x265e5a51, -0x16493856, "")
	RG4( 5, 10, 15,  4,  0,  -0x29d0efa3, 0x02441453, -0x275e197f, -0x182c0438, "")
	RG4( 9, 14,  3,  8,  0,  0x21e1cde6, -0x3cc8f82a, -0x0b2af279, 0x455a14ed, "")
	RG4(13,  2,  7, 12,  8,  -0x561c16fb, -0x03105c08, 0x676f02d9, -0x72d5b376, "movl %[input4], %k[TMP2]\n")
	
	RH4( 5,  8, 11, 14,  4,  -0x0005c6be, -0x788e097f, 0x6d9d6122, -0x021ac7f4, "movl %[input4], %k[TMP2]\n")
	RH4( 1,  4,  7, 10,  0,  -0x5b4115bc, 0x4bdecfa9, -0x0944b4a0, -0x41404390, "movl %[input4], %k[TMP2]\n")
	RH4(13,  0,  3,  6, 12,  0x289b7ec6, -0x155ed806, -0x2b10cf7b, 0x04881d05, "movl %[input4], %k[TMP2]\n")
	RH4( 9, 12, 15,  2,  0,  -0x262b2fc7, -0x1924661b, 0x1fa27cf8, -0x3b53a99b, "movl %[input4], %k[TMP2]\n")
	
	RI4( 7, 14,  5, 12,  -0x0bd6ddbc, 0x432aff97, -0x546bdc59, -0x036c5fc7)
	RI4( 3, 10,  1,  8,  0x655b59c3, -0x70f3336e, -0x00100b83, -0x7a7ba22f)
	RI4(15,  6, 13,  4,  0x6fa87e4f, -0x01d31920, -0x5cfebcec, 0x4e0811a1)
	
	__asm__(
		ROUND_I(A, B, C, D, "TMP2", "movl %[input1], %k[TMP2]\n", -0x08ac817e, 6)
		ROUND_I(D, A, B, C, "cache0", "", -0x42c50dcb, 10)
		ROUND_I(C, D, A, B, "TMP2", "", 0x2ad7d2bb, 15)
		ROUND_I_LAST(B, C, D, A, "cache2", -0x14792c6f, 21)
	: ASM_PARAMS(11, 2, 9, 0, 0));
	state->A += A;
	state->B += B;
	state->C += C;
	state->D += D;
}

#undef ROUND_G

static inline __attribute__((always_inline)) void md5_block_cache_gopt(MD5_STATE<uint32_t>* __restrict__ state, const void* __restrict__ data) {
	uint32_t A = state->A;
	uint32_t B = state->B;
	uint32_t C = state->C;
	uint32_t D = state->D;
	const uint32_t* _in = (const uint32_t*)data;
	uintptr_t tmp1, tmp2;
	uintptr_t cache1, cache3, cache5, cache7, cache9, cache11, cache13, cache15;
	
#define ROUND_G(A, B, C, D, INPUT, NEXT_IN, K, R) \
	"notl %k[TMP1]\n" \
	"leal " STR(K) "(%k[" STR(A) "], %k[" INPUT "]), %k[" STR(A) "]\n" \
	"movl %k[" STR(D) "], %k[TMP2]\n" \
	"andl %k[" STR(C) "], %k[TMP1]\n" \
	"addl %k[TMP1], %k[" STR(A) "]\n" \
	"andl %k[" STR(B) "], %k[TMP2]\n" \
	"addl %k[TMP2], %k[" STR(A) "]\n" \
	NEXT_IN \
	"roll $" STR(R) ", %k[" STR(A) "]\n" \
	"movl %k[" STR(C) "], %k[TMP1]\n" \
	"addl %k[" STR(B) "], %k[" STR(A) "]\n"
	
	
#ifdef MD5_CACHE_LOADSHIFT
	memcpy(&cache1, _in, 8);
	__asm__("":"=r"(tmp2)::); // prevent compiler warning
#else
	memcpy(&tmp2, _in, 4);
#endif
	__asm__(
		"movl %k[D], %k[TMP1]\n"
		RF4(-0x28955b88, -0x173848aa, 0x242070db, -0x3e423112, MD5_F_LOAD)
	: ASM_PARAMS2(1, 2, 3, 4, 5, "=&R", "=&r"));
	__asm__(
		RF4(-0x0a83f051, 0x4787c62a, -0x57cfb9ed, -0x02b96aff, MD5_F_LOAD)
	: ASM_PARAMS2(5, 6, 7, 8, 9, "+&R", "+&r"));
	__asm__(
		RF4(0x698098d8, -0x74bb0851, -0x0000a44f, -0x76a32842, MD5_F_LOAD)
	: ASM_PARAMS2(9, 10, 11, 12, 13, "+&R", "+&r"));
	__asm__(
		RF4(0x6b901122, -0x02678e6d, -0x5986bc72, 0x49b40821, "")
	: [A]"+&R"(A), [B]"+&R"(B), [C]"+&R"(C), [D]"+&R"(D), [TMP1]"+&R"(tmp1), [TMP2]"+&r"(tmp2),
	  [cache0]"+&r"(cache13), [cache2]"=&r"(cache15)
	: [input0]"m"(_in[13]), [input1]"m"(_in[14]), [input2]"m"(_in[15])
	:);
#undef MD5_F_LOAD
	
	RG4( 1,  6, 11,  0,  0,  -0x09e1da9e, -0x3fbf4cc0, 0x265e5a51, -0x16493856, "")
	RG4( 5, 10, 15,  4,  0,  -0x29d0efa3, 0x02441453, -0x275e197f, -0x182c0438, "")
	RG4( 9, 14,  3,  8,  0,  0x21e1cde6, -0x3cc8f82a, -0x0b2af279, 0x455a14ed, "")
	RG4(13,  2,  7, 12,  8,  -0x561c16fb, -0x03105c08, 0x676f02d9, -0x72d5b376, "movl %[input4], %k[TMP2]\n")
	
	RH4( 5,  8, 11, 14,  4,  -0x0005c6be, -0x788e097f, 0x6d9d6122, -0x021ac7f4, "movl %[input4], %k[TMP2]\n")
	RH4( 1,  4,  7, 10,  0,  -0x5b4115bc, 0x4bdecfa9, -0x0944b4a0, -0x41404390, "movl %[input4], %k[TMP2]\n")
	RH4(13,  0,  3,  6, 12,  0x289b7ec6, -0x155ed806, -0x2b10cf7b, 0x04881d05, "movl %[input4], %k[TMP2]\n")
	RH4( 9, 12, 15,  2,  0,  -0x262b2fc7, -0x1924661b, 0x1fa27cf8, -0x3b53a99b, "movl %[input4], %k[TMP2]\n")
	
	RI4( 7, 14,  5, 12,  -0x0bd6ddbc, 0x432aff97, -0x546bdc59, -0x036c5fc7)
	RI4( 3, 10,  1,  8,  0x655b59c3, -0x70f3336e, -0x00100b83, -0x7a7ba22f)
	RI4(15,  6, 13,  4,  0x6fa87e4f, -0x01d31920, -0x5cfebcec, 0x4e0811a1)
	
	__asm__(
		ROUND_I(A, B, C, D, "TMP2", "movl %[input1], %k[TMP2]\n", -0x08ac817e, 6)
		ROUND_I(D, A, B, C, "cache0", "", -0x42c50dcb, 10)
		ROUND_I(C, D, A, B, "TMP2", "", 0x2ad7d2bb, 15)
		ROUND_I_LAST(B, C, D, A, "cache2", -0x14792c6f, 21)
	: ASM_PARAMS(11, 2, 9, 0, 0));
	state->A += A;
	state->B += B;
	state->C += C;
	state->D += D;
}

#undef RF4
#undef ASM_PARAMS
#undef ASM_PARAMS2
#endif

#undef ROUND_F
#undef ROUND_G
#undef ROUND_H
#undef ROUND_I
#undef RG4
#undef RH4
#undef RI4



#if defined(__AVX512VL__) && defined(PLATFORM_AMD64)
#include <immintrin.h>
static const uint32_t md5_constants[64] __attribute__((aligned(16))) = {
	// F
	0xd76aa478L, 0xe8c7b756L, 0x242070dbL, 0xc1bdceeeL,
	0xf57c0fafL, 0x4787c62aL, 0xa8304613L, 0xfd469501L,
	0x698098d8L, 0x8b44f7afL, 0xffff5bb1L, 0x895cd7beL,
	0x6b901122L, 0xfd987193L, 0xa679438eL, 0x49b40821L,
	
	// G-F
	0x124c2332L, 0x0d566e0cL, 0xd8cf331dL, 0x33173e99L,
	0xf257ec19L, 0x8ea74a33L, 0x18106d2dL, 0x6a286dd8L,
	0xdbd97c15L, 0x969cd637L, 0x0244b8a2L, 0x9d018293L,
	0x219a3b68L, 0xac4b7772L, 0x1cbdc448L, 0x8eedde60L,
	
	// H-G
	0x00ea6050L, 0xaea0c4e2L, 0xc7bcb26dL, 0xe01a22feL,
	0x640ad3e1L, 0x29cb28e5L, 0x444769c5L, 0x8f4c4887L,
	0x4217e194L, 0xb7f30253L, 0xbc7ba81dL, 0x473f06d1L,
	0x59b14d5bL, 0x7eb795c1L, 0x3aae3036L, 0x47009677L,
	
	// I-H
	0x0987fa4aL, 0xe0c5738dL, 0x662b7c56L, 0xba1d9c0dL,
	0xab74aed9L, 0xfc9966f7L, 0x9e79260fL, 0x4c6fb437L,
	0xe83687ceL, 0x11b20358L, 0x4130380dL, 0x4f9d9113L,
	0x7e7fbfdeL, 0x256c92dbL, 0xadaeeb9bL, 0xde8a69e8L
};

static inline __attribute__((always_inline)) void md5_block_avx512(MD5_STATE<__m128i>* __restrict__ state, const void* __restrict__ data) {
	__m128i A;
	__m128i B;
	__m128i C;
	__m128i D;
	const __m128i* _in = (const __m128i*)data;
	__m128i tmp1, tmp2;
	__m128i in0, in4, in8, in12;
	
#define ASM_OUTPUTS [A]"+&x"(A), [B]"+&x"(B), [C]"+&x"(C), [D]"+&x"(D), [TMP1]"=&x"(tmp1), [TMP2]"+&x"(tmp2)

#define RF4(i) \
	"vmovdqu %[input" STR(i) "], %[in" STR(i) "]\n" \
	"vpaddd %[k" STR(i) "], %[in" STR(i) "], %[in" STR(i) "]\n" \
	ROUND_X(0xd8, A, A, B, C, D, "%[in" STR(i) "]", 25) \
	"vpsrlq $32, %[in" STR(i) "], %[TMP1]\n" \
	ROUND_X(0xd8, D, D, A, B, C, "%[TMP1]", 20) \
	"vpunpckhqdq %[in" STR(i) "], %[in" STR(i) "], %[TMP1]\n" \
	ROUND_X(0xd8, C, C, D, A, B, "%[TMP1]", 15) \
	"vpsrlq $32, %[TMP1], %[TMP1]\n" \
	ROUND_X(0xd8, B, B, C, D, A, "%[TMP1]", 10)
#define RF4_FIRST(i) \
	"vmovdqu %[input" STR(i) "], %[in" STR(i) "]\n" \
	"vpaddd %[k" STR(i) "], %[in" STR(i) "], %[in" STR(i) "]\n" \
	ROUND_X(0xd8, IA, A, IB, IC, ID, "%[in" STR(i) "]", 25) \
	"vpsrlq $32, %[in" STR(i) "], %[TMP1]\n" \
	ROUND_X(0xd8, ID, D, A, IB, IC, "%[TMP1]", 20) \
	"vpunpckhqdq %[in" STR(i) "], %[in" STR(i) "], %[TMP1]\n" \
	ROUND_X(0xd8, IC, C, D, A, IB, "%[TMP1]", 15) \
	"vpsrlq $32, %[TMP1], %[TMP1]\n" \
	ROUND_X(0xd8, IB, B, C, D, A, "%[TMP1]", 10)

	
#define RG4(rs, r1, r2) \
	"vpsrlq $32, " rs ", %[TMP1]\n" \
	ROUND_X(0xac, A, A, B, C, D, "%[TMP1]", 27) \
	"vpunpckhqdq " r1 ", " r1 ", %[TMP1]\n" \
	ROUND_X(0xac, D, D, A, B, C, "%[TMP1]", 23) \
	"vpsrldq $12, " r2 ", %[TMP1]\n" \
	ROUND_X(0xac, C, C, D, A, B, "%[TMP1]", 18) \
	ROUND_X(0xac, B, B, C, D, A, rs, 12)
	
#define RH4(r1, rs, r2) \
	"vpsrlq $32, " r1 ", %[TMP1]\n" \
	ROUND_H(A, B, C, D, "%[TMP1]", 28) \
	ROUND_H(D, A, B, C, rs, 21) \
	"vpsrldq $12, " rs ", %[TMP1]\n" \
	ROUND_H(C, D, A, B, "%[TMP1]", 16) \
	"vpunpckhqdq " r2 ", " r2 ", %[TMP1]\n" \
	ROUND_H(B, C, D, A, "%[TMP1]",  9)
	
#define RI4(r1, rs, r2) \
	ROUND_X(0x63, A, A, B, C, D, r1, 26) \
	"vpsrldq $12, " rs ", %[TMP1]\n" \
	ROUND_X(0x63, D, D, A, B, C, "%[TMP1]", 22) \
	"vpunpckhqdq " r2 ", " r2 ", %[TMP1]\n" \
	ROUND_X(0x63, C, C, D, A, B, "%[TMP1]", 17) \
	"vpsrlq $32, " rs ", %[TMP1]\n" \
	ROUND_X(0x63, B, B, C, D, A, "%[TMP1]", 11)
	
#define ROUND_X(T, IA, A, B, C, D, I, R) \
	"vpaddd " I ", %[" STR(IA) "], %[" STR(A) "]\n" \
	"vpternlogd $" STR(T) ", %[" STR(B) "], %[" STR(C) "], %[TMP2]\n" \
	"vpaddd %[TMP2], %[" STR(A) "], %[" STR(A) "]\n" \
	"vprord $" STR(R) ", %[" STR(A) "], %[" STR(A) "]\n" \
	"vmovdqa %[" STR(C) "], %[TMP2]\n" \
	"vpaddd %[" STR(B) "], %[" STR(A) "], %[" STR(A) "]\n"
	
#define ROUND_H(A, B, C, D, I, R) \
	"vpaddd " I ", %[" STR(A) "], %[TMP1]\n" \
	"vpternlogd $0x96, %[" STR(B) "], %[" STR(A) "], %[TMP2]\n" \
	"vpaddd %[TMP2], %[TMP1], %[" STR(A) "]\n" \
	"vprord $" STR(R) ", %[" STR(A) "], %[" STR(A) "]\n" \
	"vpaddd %[" STR(B) "], %[" STR(A) "], %[" STR(A) "]\n"
	
	__asm__(
		"vmovdqa %[ID], %[TMP2]\n"
		RF4_FIRST(0)
		RF4(4)
		RF4(8)
		RF4(12)
	: [A]"=&x"(A), [B]"=&x"(B), [C]"=&x"(C), [D]"=&x"(D), [TMP1]"=&x"(tmp1), [TMP2]"=&x"(tmp2),
	  [in0]"=&x"(in0), [in4]"=&x"(in4), [in8]"=&x"(in8), [in12]"=&x"(in12),
	  // marked as output to prevent bad clobbering by compilers
	  [IA]"+&x"(state->A), [IB]"+&x"(state->B), [IC]"+&x"(state->C), [ID]"+&x"(state->D)
	: [input0]"m"(_in[0]), [input4]"m"(_in[1]), [input8]"m"(_in[2]), [input12]"m"(_in[3]),
	  [k0]"m"(md5_constants[0]), [k4]"m"(md5_constants[4]), [k8]"m"(md5_constants[8]), [k12]"m"(md5_constants[12])
	:);
	
#define ASM_PARAMS(n) \
	ASM_OUTPUTS, [in0]"+&x"(in0), [in4]"+&x"(in4), [in8]"+&x"(in8), [in12]"+&x"(in12) \
	: [k0]"m"(md5_constants[n]), [k1]"m"(md5_constants[n+4]), [k2]"m"(md5_constants[n+8]), [k3]"m"(md5_constants[n+12]) \
	:
	
	__asm__(
		"vpaddd %[k0], %[in0], %[in0]\n"
		"vpaddd %[k1], %[in4], %[in4]\n"
		"vpaddd %[k2], %[in8], %[in8]\n"
		RG4("%[in0]", "%[in4]", "%[in8]")
		"vpaddd %[k3], %[in12], %[in12]\n"
		RG4("%[in4]", "%[in8]", "%[in12]")
		RG4("%[in8]", "%[in12]", "%[in0]")
		RG4("%[in12]", "%[in0]", "%[in4]")
	: ASM_PARAMS(16));
	
	__asm__(
		"vpaddd %[k1], %[in4], %[in4]\n"
		"vpsrlq $32, %[in4], %[TMP1]\n"
		
		"vpaddd %[TMP1], %[A], %[A]\n"
		"vpternlogd $0x96, %[B], %[C], %[TMP2]\n"
		"vpaddd %[TMP2], %[A], %[A]\n"
		"vprord $28, %[A], %[A]\n"
		"vpaddd %[B], %[A], %[A]\n"
		
		"vpaddd %[k2], %[in8], %[in8]\n"
		ROUND_H(D, A, B, C, "%[in8]", 21)
		"vpsrldq $12, %[in8], %[TMP1]\n"
		"vpaddd %[k3], %[in12], %[in12]\n"
		ROUND_H(C, D, A, B, "%[TMP1]", 16)
		"vpunpckhqdq %[in12], %[in12], %[TMP1]\n"
		ROUND_H(B, C, D, A, "%[TMP1]",  9)
		
		"vpaddd %[k0], %[in0], %[in0]\n"
		RH4("%[in0]", "%[in4]", "%[in8]")
		RH4("%[in12]", "%[in0]", "%[in4]")
		RH4("%[in8]", "%[in12]", "%[in0]")
		"vmovdqa %[D], %[TMP2]\n"
	: ASM_PARAMS(32));
	
	__asm__(
		"vpaddd %[k0], %[in0], %[in0]\n"
		"vpaddd %[k1], %[in4], %[in4]\n"
		"vpaddd %[k3], %[in12], %[in12]\n"
		RI4("%[in0]", "%[in4]", "%[in12]")
		"vpaddd %[k2], %[in8], %[in8]\n"
		RI4("%[in12]", "%[in0]", "%[in8]")
		RI4("%[in8]", "%[in12]", "%[in4]")
		RI4("%[in4]", "%[in8]", "%[in0]") // contains an unnecessary move on final ROUND_X... oh well
	: ASM_PARAMS(48));
	
	state->A = _mm_add_epi32(A, state->A);
	state->B = _mm_add_epi32(B, state->B);
	state->C = _mm_add_epi32(C, state->C);
	state->D = _mm_add_epi32(D, state->D);
#undef ROUND_X
#undef RF4
#undef RG4
#undef RH4
#undef RI4
#undef ASM_OUTPUTS
#undef ASM_PARAMS
}
#endif


#endif
