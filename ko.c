/**
 * 한글 음절에서 초성, 중성, 종성을 추출하는 함수
 * 입력: 유니코드 한글 음절(AC00-D7A3)
 * 출력: 초성(0-18), 중성(0-20), 종성(0-27)
 */

#include <stdint.h>

// 한글 관련 상수
#define HANGUL_BASE 0xAC00 // '가'의 유니코드 값
#define JONGSEONG_COUNT 28 // 종성 개수 (없음 포함)
#define JUNGSEONG_COUNT 21 // 중성 개수
#define CHOSEONG_COUNT 19  // 초성 개수

/**************************************************************
 * Division and modulo functions for 6502 CPU
 * All functions avoid using multiplication and division
 * Only use bit shifting and addition/subtraction operations
 **************************************************************/

// Function to divide a 16-bit unsigned integer by 28 without using division
uint16_t __fastcall__ div28(uint16_t n) {
  // First, we'll use the fact that 28 = 4 * 7
  // So n/28 = n/(4*7) = (n/4)/7

  // Shift right by 2 to divide by 4
  register uint16_t n_div_4 = n >> 2;

  // Now we need to divide by 7
  // We'll use a fast method that avoids division

  // Initialize result
  register uint16_t result = 0;

  // If n/4 is less than 7, the result is 0
  if (n_div_4 < 7)
    return 0;

  // Handle large values efficiently
  // Using repeated subtraction would be too slow for large numbers
  // Instead, we'll use a more efficient approach

  /*
  // For values >= 7*16384 (114688)
  if (n_div_4 >= 114688) {
      n_div_4 -= 114688;
      result += 16384;
  }

  // For values >= 7*8192 (57344)
  if (n_div_4 >= 57344) {
      n_div_4 -= 57344;
      result += 8192;
  }*/

  // For values >= 7*4096 (28672)
  if (n_div_4 >= 28672) {
    n_div_4 -= 28672;
    result += 4096;
  }

  // For values >= 7*2048 (14336)
  if (n_div_4 >= 14336) {
    n_div_4 -= 14336;
    result += 2048;
  }

  // For values >= 7*1024 (7168)
  if (n_div_4 >= 7168) {
    n_div_4 -= 7168;
    result += 1024;
  }

  // For values >= 7*512 (3584)
  if (n_div_4 >= 3584) {
    n_div_4 -= 3584;
    result += 512;
  }

  // For values >= 7*256 (1792)
  if (n_div_4 >= 1792) {
    n_div_4 -= 1792;
    result += 256;
  }

  // For values >= 7*128 (896)
  if (n_div_4 >= 896) {
    n_div_4 -= 896;
    result += 128;
  }

  // For values >= 7*64 (448)
  if (n_div_4 >= 448) {
    n_div_4 -= 448;
    result += 64;
  }

  // For values >= 7*32 (224)
  if (n_div_4 >= 224) {
    n_div_4 -= 224;
    result += 32;
  }

  // For values >= 7*16 (112)
  if (n_div_4 >= 112) {
    n_div_4 -= 112;
    result += 16;
  }

  // For values >= 7*8 (56)
  if (n_div_4 >= 56) {
    n_div_4 -= 56;
    result += 8;
  }

  // For values >= 7*4 (28)
  if (n_div_4 >= 28) {
    n_div_4 -= 28;
    result += 4;
  }

  // For values >= 7*2 (14)
  if (n_div_4 >= 14) {
    n_div_4 -= 14;
    result += 2;
  }

  // For values >= 7*1 (7)
  if (n_div_4 >= 7) {
    n_div_4 -= 7;
    result += 1;
  }

  return result;
}

// Function to calculate modulo 28 of a 16-bit unsigned integer without using
// modulo
uint16_t __fastcall__ mod28(uint16_t n) {
  // Calculate modulo 28 using the formula: n % 28 = n - (n / 28) * 28
  // However, we can't use multiplication, so we need a different approach

  // First get the quotient n / 28
  register uint16_t quotient = div28(n);

  // Now subtract multiples of 28 from the original number
  // We'll do this by breaking down 28 = 16 + 8 + 4
  register uint16_t result = n;

  // Subtract quotient * 16
  register uint16_t temp = quotient << 4; // multiply by 16 using left shift
  result -= temp;

  // Subtract quotient * 8
  temp = quotient << 3; // multiply by 8 using left shift
  result -= temp;

  // Subtract quotient * 4
  temp = quotient << 2; // multiply by 4 using left shift
  result -= temp;

  // If the result is still >= 28, we need to subtract 28 more
  // This handles potential rounding errors in the div28 function
  while (result >= 28) {
    result -= 28;
  }

  return result;
}

// Function to divide a 16-bit unsigned integer by 21 without using division
uint16_t __fastcall__ div21(uint16_t n) {
  // Initialize result
  register uint16_t result = 0;

  // If n is less than 21, the result is 0
  if (n < 21)
    return 0;

  /*
  // For values >= 21*16384 (344064)
  if (n >= 344064) {
      n -= 344064;
      result += 16384;
  }

  // For values >= 21*8192 (172032)
  if (n >= 172032) {
      n -= 172032;
      result += 8192;
  }

  // For values >= 21*4096 (86016)
  if (n >= 86016) {
      n -= 86016;
      result += 4096;
  }
  */

  // For values >= 21*2048 (43008)
  if (n >= 43008U) {
    n -= 43008U;
    result += 2048;
  }

  // For values >= 21*1024 (21504)
  if (n >= 21504) {
    n -= 21504;
    result += 1024;
  }

  // For values >= 21*512 (10752)
  if (n >= 10752) {
    n -= 10752;
    result += 512;
  }

  // For values >= 21*256 (5376)
  if (n >= 5376) {
    n -= 5376;
    result += 256;
  }

  // For values >= 21*128 (2688)
  if (n >= 2688) {
    n -= 2688;
    result += 128;
  }

  // For values >= 21*64 (1344)
  if (n >= 1344) {
    n -= 1344;
    result += 64;
  }

  // For values >= 21*32 (672)
  if (n >= 672) {
    n -= 672;
    result += 32;
  }

  // For values >= 21*16 (336)
  if (n >= 336) {
    n -= 336;
    result += 16;
  }

  // For values >= 21*8 (168)
  if (n >= 168) {
    n -= 168;
    result += 8;
  }

  // For values >= 21*4 (84)
  if (n >= 84) {
    n -= 84;
    result += 4;
  }

  // For values >= 21*2 (42)
  if (n >= 42) {
    n -= 42;
    result += 2;
  }

  // For values >= 21*1 (21)
  if (n >= 21) {
    n -= 21;
    result += 1;
  }

  return result;
}

// Function to calculate modulo 21 of a 16-bit unsigned integer without using
// modulo
uint16_t __fastcall__ mod21(uint16_t n) {
  // Calculate modulo 21 using a similar approach to mod28

  // First get the quotient n / 21
  register uint16_t quotient = div21(n);

  // Now subtract multiples of 21 from the original number
  // We'll do this by breaking down 21 = 16 + 4 + 1
  register uint16_t result = n;

  // Subtract quotient * 16
  register uint16_t temp = quotient << 4; // multiply by 16 using left shift
  result -= temp;

  // Subtract quotient * 4
  temp = quotient << 2; // multiply by 4 using left shift
  result -= temp;

  // Subtract quotient * 1
  result -= quotient;

  // If the result is still >= 21, we need to subtract 21 more
  // This handles potential rounding errors in the div21 function
  while (result >= 21) {
    result -= 21;
  }

  return result;
}

/**
 * Decomposes a Hangul syllable into its Jamo components
 *
 * @param syllable Unicode code point of the Hangul syllable (AC00-D7A3)
 * @param choseong Initial consonant index (0-18)
 * @param jungseong Vowel index (0-20)
 * @param jongseong Final consonant index (0-27, 0 means no final consonant)
 */
void __fastcall__ decomposeHangul(uint16_t syllable, uint8_t *choseong,
                                  uint8_t *jungseong, uint8_t *jongseong) {
  register uint16_t syllable_offset, syllable_without_jong;

  // Check if the character is in the Hangul syllable range
  if (syllable < 0xAC00 || syllable > 0xD7A3) {
    return;
  }

  // Calculate the offset from the beginning of Hangul syllables block
  syllable_offset = syllable - 0xAC00;

  // 종성 = syllableIndex % JONGSEONG_COUNT
  // 중성 = (syllableIndex / JONGSEONG_COUNT) % JUNGSEONG_COUNT
  // 초성 = (syllableIndex / JUNGSEONG_COUNT) / JONGSEONG_COUNT

  // Extract jongseong (final consonant)
  // jongseong = syllable_offset % 28
  *jongseong = mod28(syllable_offset);

  // Remove jongseong from offset to get syllable without final consonant
  // syllable_without_jong = syllable_offset / 28
  syllable_without_jong = div28(syllable_offset);

  // Extract jungseong (vowel)
  // jungseong = syllable_without_jong % 21
  *jungseong = mod21(syllable_without_jong);

  // Extract choseong (initial consonant)
  // choseong = syllable_without_jong / 21
  *choseong = div21(syllable_without_jong);
}

/**
 * Composes a Hangul syllable from its Jamo components
 *
 * @param choseong Initial consonant index (0-18)
 * @param jungseong Vowel index (0-20)
 * @param jongseong Final consonant index (0-27, 0 means no final consonant)
 * @return Unicode code point of the composed Hangul syllable
 */
uint16_t __fastcall__ composeHangul(uint8_t choseong, uint8_t jungseong,
                                    uint8_t jongseong) {
  register uint16_t offset;

  // Check if indices are within valid ranges
  if (choseong > 18 || jungseong > 20 || jongseong > 27) {
    // Return a default value if indices are out of range
    return 0;
  }

  // Calculate the offset from the beginning of Hangul syllables block
  offset = 0;

  // Add choseong contribution: choseong_index * 588
  // Since we can't use multiplication, we use shifts and addition
  // 588 = 512 + 64 + 8 + 4
  offset += choseong << 9; // choseong * 512
  offset += choseong << 6; // choseong * 64
  offset += choseong << 3; // choseong * 8
  offset += choseong << 2; // choseong * 4

  // Add jungseong contribution: jungseong_index * 28
  // 28 = 16 + 8 + 4
  offset += jungseong << 4; // jungseong * 16
  offset += jungseong << 3; // jungseong * 8
  offset += jungseong << 2; // jungseong * 4

  // Add jongseong contribution: jongseong_index
  offset += jongseong;

  // Return the calculated syllable
  return 0xAC00 + offset;
}

// 한글 자모 이름 출력용 배열
const char *CHOSEONG_NAMES[] = {"ㄱ", "ㄲ", "ㄴ", "ㄷ", "ㄸ", "ㄹ", "ㅁ",
                                "ㅂ", "ㅃ", "ㅅ", "ㅆ", "ㅇ", "ㅈ", "ㅉ",
                                "ㅊ", "ㅋ", "ㅌ", "ㅍ", "ㅎ"};

const char *JUNGSEONG_NAMES[] = {"ㅏ", "ㅐ", "ㅑ", "ㅒ", "ㅓ", "ㅔ", "ㅕ",
                                 "ㅖ", "ㅗ", "ㅘ", "ㅙ", "ㅚ", "ㅛ", "ㅜ",
                                 "ㅝ", "ㅞ", "ㅟ", "ㅠ", "ㅡ", "ㅢ", "ㅣ"};

const char *JONGSEONG_NAMES[] = {"없음", "ㄱ", "ㄲ", "ㄳ", "ㄴ", "ㄵ", "ㄶ",
                                 "ㄷ",   "ㄹ", "ㄺ", "ㄻ", "ㄼ", "ㄽ", "ㄾ",
                                 "ㄿ",   "ㅀ", "ㅁ", "ㅂ", "ㅄ", "ㅅ", "ㅆ",
                                 "ㅇ",   "ㅈ", "ㅊ", "ㅋ", "ㅌ", "ㅍ", "ㅎ"};

#if 0
#include <stdio.h>

// Test structure to hold test cases
typedef struct {
    uint16_t syllable;     // Input Hangul syllable
    uint16_t choseong;     // Expected choseong index
    uint16_t jungseong;    // Expected jungseong index
    uint16_t jongseong;    // Expected jongseong index
    const char* desc;      // Description of the test case
} TestCase;


// Function to run tests on the decomposeHangul function
void testDecomposeHangul() {
    // Define test cases
    TestCase tests[] = {
        // syllable, choseong, jungseong, jongseong, description
        {0xAC00, 0, 0, 0, "가 (U+AC00): Basic syllable with no final consonant"},
        {0xAC01, 0, 0, 1, "각 (U+AC01): Basic syllable with final consonant ㄱ"},
        {0xB098, 2, 0, 0, "나 (U+B098): Syllable with ㄴ initial consonant"},
        {0xB2E4, 3, 0, 0, "다 (U+B2E4): Syllable with ㄷ initial consonant"},
        {0xC544, 11, 0, 0, "아 (U+C544): Syllable with ㅇ initial consonant"},
        {0xC790, 12, 0, 0, "자 (U+C790): Syllable with ㅈ initial consonant"},
        {0xD55C, 18, 0, 4, "한 (U+D55C): Syllable with ㅎ initial and ㄴ final consonant"},
        {0xAE00, 0, 18, 8, "글 (U+AE00): Syllable with ㅡ vowel"},
        {0xADF8, 0, 18, 0, "그 (U+ADF8): Another syllable with ㅡ vowel"},
        {0xC774, 11, 20, 0, "이 (U+C774): Syllable with ㅣ vowel"},
        {0xD7A3, 18, 20, 27, "힣 (U+D7A3): Last syllable in the block"}
    };

    // Number of test cases
    uint16_t numTests = sizeof(tests) / sizeof(tests[0]);

    // Run the tests
    printf("Running %u Hangul decomposition tests...\n", numTests);
    printf("---------------------------------------------------------\n");
    printf("| %-6s | %-10s | %-8s | %-8s | %-8s |\n", "Char", "Description", "Choseong", "Jungseong", "Jongseong");
    printf("---------------------------------------------------------\n");

    for (uint16_t i = 0; i < numTests; i++) {
        // Decompose the syllable
        HangulJamo result = decomposeHangul(tests[i].syllable);

        // Check if the result matches the expected values
        uint8_t passed = (result.choseong == tests[i].choseong) &&
                          (result.jungseong == tests[i].jungseong) &&
                          (result.jongseong == tests[i].jongseong);

        // Print the test result
        printf("| U+%04X | %-10s | %2u (%-4s) | %2u (%-4s) | %2u (%-4s) | %s\n",
               tests[i].syllable,
               tests[i].desc,
               result.choseong, (result.choseong == tests[i].choseong) ? "OK" : "FAIL",
               result.jungseong, (result.jungseong == tests[i].jungseong) ? "OK" : "FAIL",
               result.jongseong, (result.jongseong == tests[i].jongseong) ? "OK" : "FAIL",
               passed ? "PASSED" : "FAILED");
    }

    printf("---------------------------------------------------------\n");
}

// Function to test the composeHangul function
void testComposeHangul() {
    // Define test cases
    TestCase tests[] = {
        // expected syllable, choseong, jungseong, jongseong, description
        {0xAC00, 0, 0, 0, "가 (U+AC00): Basic syllable with no final consonant"},
        {0xAC01, 0, 0, 1, "각 (U+AC01): Basic syllable with final consonant ㄱ"},
        {0xB098, 2, 0, 0, "나 (U+B098): Syllable with ㄴ initial consonant"},
        {0xD55C, 18, 0, 4, "한 (U+D55C): Syllable with ㅎ initial and ㄴ final consonant"},
        {0xAE00, 0, 18, 8, "글 (U+AE00): Syllable with ㅡ vowel"},
        {0xC774, 11, 20, 0, "이 (U+C774): Syllable with ㅣ vowel"},
        {0xD7A3, 18, 20, 27, "힣 (U+D7A3): Last syllable in the block"}
    };

    // Number of test cases
    uint16_t numTests = sizeof(tests) / sizeof(tests[0]);

    // Run the tests
    printf("\nRunning %u Hangul composition tests...\n", numTests);
    printf("----------------------------------------------------\n");
    printf("| %-6s | %-32s | %-8s |\n", "Char", "Description", "Result");
    printf("----------------------------------------------------\n");

    for (uint16_t i = 0; i < numTests; i++) {
        // Create a jamo structure with the test values
        HangulJamo jamo = {
            .choseong = tests[i].choseong,
            .jungseong = tests[i].jungseong,
            .jongseong = tests[i].jongseong
        };

        // Compose the syllable
        uint16_t result = composeHangul(jamo);

        // Check if the result matches the expected value
        uint8_t passed = (result == tests[i].syllable);

        // Print the test result
        printf("| U+%04X | %-32s | U+%04X %-4s |\n",
               tests[i].syllable,
               tests[i].desc,
               result,
               passed ? "OK" : "FAIL");
    }

    printf("----------------------------------------------------\n");
}

// Main test function
void testHangulFunctions() {
    printf("Testing Hangul syllable decomposition and composition functions\n");
    printf("=============================================================\n\n");

    // Test the decomposition function
    testDecomposeHangul();

    // Test the composition function
    testComposeHangul();

    printf("\nTests completed.\n");
}

// Sample main function to run the tests
int main() {
    testHangulFunctions();
    return 0;
}

#endif
