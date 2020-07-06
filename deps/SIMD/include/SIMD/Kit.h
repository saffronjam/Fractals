// A header file to get you set going with Intel SIMD instrinsic programming.
// All necessary header files are inlucded for SSE2, SSE41, and AVX2
// Macros make the intrinsics easier to read and  generic so you can compile to
// SSE2 or AVX2 with the flip of a #define

#define SSE2  //indicates we want SSE2
#define SSE41 //indicates we want SSE4.1 instructions (floor and blend is available)
#define AVX2  //indicates we want AVX2 instructions (double speed!)

#ifndef AVX2
#include <xmmintrin.h> //SSE
#include <emmintrin.h> //SSE 2
#endif

#ifdef SSE41
#include <smmintrin.h> // SSE4.1
#endif

#ifdef AVX2
#include <immintrin.h> //avx2
#endif

// #include <zmmintrin.h> //avx512 the world is not yet ready...SOON

// create types we can use in either the 128 or 256 case
#ifndef AVX2
// m128 will be our base type
typedef __m128 SIMD;   //for floats
typedef __m128i SIMDi; //for integers

// we process 4 at a time
#define VECTOR_SIZE 4
#define MEMORY_ALIGNMENT 16

// intrinsic functions
#define Store(x, y) _mm_store_ps(x, y)
#define Load(x) _mm_load_ps(x)
#define SetOne(x) _mm_set1_ps(x)
#define SetZero() _mm_setzero_ps()
#define SetOnei(x) _mm_set1_epi32(x)
#define SetZeroi(x) _mm_setzero_epi32(x)
#define Add(x, y) _mm_add_ps(x, y)
#define Sub(x, y) _mm_sub_ps(x, y)
#define Addi(x, y) _mm_add_epi32(x, y)
#define Subi(x, y) _mm_sub_epi32(x, y)
#define Mul(x, y) _mm_mul_ps(x, y)
#define Muli(x, y) _mm_mul_epi32(x, y)
#define And(x, y) _mm_and_ps(x, y)
#define Andi(x, y) _mm_and_si128(x, y)
#define AndNot(x, y) _mm_andnot_ps(x, y)
#define Or(x, y) _mm_or_ps(x, y)
#define Ori(x, y) _mm_or_si128(x, y)
#define CastToFloat(x) _mm_castsi128_ps(x)
#define CastToInt(x) _mm_castps_si128(x)
#define ConvertToInt(x) _mm_cvtps_epi32(x)
#define ConvertToFloat(x) _mm_cvtepi32_ps(x)
#define Equal(x, y) _mm_cmpeq_ps(x, y)
#define Equali(x, y) _mm_cmpeq_epi32(x, y)
#define GreaterThan(x, y) _mm_cmpgt_ps(x, y)
#define GreaterThani(x, y) _mm_cmpgt_epi32(x, y)
#define GreaterThanOrEq(x, y) _mm_cmpge_ps(x, y)
#define LessThan(x, y) _mm_cmplt_ps(x, y)
#define LessThani(x, y) _mm_cmpgt_epi32(y, x)
#define LessThanOrEq(x, y) _mm_cmple_ps(x, y)
#define NotEqual(x, y) _mm_cmpneq_ps(x, y)
#ifdef SSE41
#define Floor(x) _mm_floor_ps(x)
#endif
#define Max(x, y) _mm_max_ps(x, y)
#define Maxi(x, y) _mm_max_epi32(x, y)
#define Min(x, y) _mm_min_ps(x, y)
#ifndef SSE41
#define BlendV(x, y, z) Or(AndNot(z, x), And(z, y)) //if we don't have sse4
#else
#define BlendV(x, y, z) _mm_blendv_ps(x, y, z)
#endif

#endif
#ifdef AVX2

// m256 will be our base type
typedef __m256 SIMD;   //for floats
typedef __m256i SIMDi; //for integers
typedef __m256d SIMDd; //for doubles

//process 8 at t time
#define VECTOR_SIZE 8
#define MEMORY_ALIGNMENT 32

//intrinsic functions
#define Store(x, y) _mm256_store_ps(x, y)
#define Load(x) _mm256_load_ps(x)
#define Set(x, y, z, w, a, b, c, d) _mm256_set_ps(x, y, z, w, a, b, c, d);
#define SetOne(x) _mm256_set1_ps(x)
#define SetZero() _mm256_setzero_ps()
#define SetOnei(x) _mm256_set1_epi32(x)
#define SetZeroi(x) _mm256_setzero_epi32(x)
#define Add(x, y) _mm256_add_ps(x, y)
#define Sub(x, y) _mm256_sub_ps(x, y)
#define Addi(x, y) _mm256_add_epi32(x, y)
#define Subi(x, y) _mm256_sub_epi32(x, y)
#define Mul(x, y) _mm256_mul_ps(x, y)
#define Muli(x, y) _mm256_mul_epi32(x, y)
#define And(x, y) _mm256_and_ps(x, y)
#define Andi(x, y) _mm256_and_si256(x, y)
#define AndNot(x, y) _mm256_andnot_ps(x, y)
#define Or(x, y) _mm256_or_ps(x, y)
#define Ori(x, y) _mm256_or_si256(x, y)
#define CastToFloat(x) _mm256_castsi256_ps(x)
#define CastToInt(x) _mm256_castps_si256(x)
#define ConvertToInt(x) _mm256_cvtps_epi32(x)
#define ConvertToFloat(x) _mm256_cvtepi32_ps(x)
#define Equal(x, y) _mm256_cmp_ps(x, y, _CMP_EQ_OQ)
#define Equali(x, y) _mm256_cmpeq_epi32(x, y)
#define GreaterThan(x, y) _mm256_cmp_ps(x, y, _CMP_GT_OQ)
#define GreaterThani(x, y) _mm256_cmpgt_epi32(x, y)
#define LessThan(x, y) _mm256_cmp_ps(x, y, _CMP_LT_OQ)
#define LessThani(x, y) _mm256_cmpgt_epi32(y, x)
#define LessThanOrEq(x, y) _mm256_cmp_ps(x, y, _CMP_LE_OQ)
#define GreaterThanOrEq(x, y) _mm256_cmp_ps(x, y, _CMP_GE_OQ)
#define NotEqual(x, y) _mm256_cmp_ps(x, y, _CMP_NEQ_OQ)
#define Floor(x) _mm256_floor_ps(x)
#define Max(x, y) _mm256_max_ps(x, y)
#define Maxi(x, y) _mm256_max_epi32(x, y)
#define Min(x, y) _mm256_min_ps(x, y)
#define Gather(x, y, z) _mm256_i32gather_epi32(x, y, z)
#define Gatherf(x, y, z) _mm256_i32gather_ps(x, y, z)
#define BlendV(x, y, z) _mm256_blendv_ps(x, y, z)
#endif