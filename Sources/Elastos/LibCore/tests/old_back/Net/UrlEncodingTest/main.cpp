#include "test.h"
#include <stdio.h>
#include <stdlib.h>

typedef int (CTest::*PTestEntry)(int argc, char *argv[]);

PTestEntry TestEntry[] =
{
    &CTest::testUriRetainsOriginalEncoding,
    &CTest::testDecodingPlus,
    &CTest::testEncodingPlus,
    &CTest::testDecodingSpace,
    &CTest::testEncodingSpace,
    &CTest::testUriDecodingPartial,
    &CTest::testUrlDecoderDecodingPartial,
    &CTest::testUriDecodingInvalid,
    &CTest::testUrlDecoderDecodingInvalid,
    &CTest::testUrlDecoderFailsOnNullCharset,
    &CTest::testUrlDecoderFailsOnEmptyCharset,
    &CTest::testUrlEncoderFailsOnNullCharset,
    &CTest::testUrlEncoderFailsOnEmptyCharset,
    &CTest::testUrlDecoderIgnoresUnnecessaryCharset,
    &CTest::testUrlEncoderFailsOnInvalidCharset,
    &CTest::testDecoding,
    &CTest::testEncoding,
    &CTest::testDecodingLiterals,
    &CTest::testDecodingBrokenUtf8SequenceYieldsReplacementCharacter,
    &CTest::testDecodingUtf8Octets,
    &CTest::testDecodingNonUsDigits,
    &CTest::testUrlEncoderEncodesNonPrintableNonAsciiCharacters,
    &CTest::testUriDoesNotEncodeNonPrintableNonAsciiCharacters,
    &CTest::testUriEncodesControlCharacters,
    &CTest::testEncodeAndDecode,
};

int main(int argc, char *argv[])
{
    if (argc == 1) {
        printf("*ERROR* Require test case number\n");
        return -1;
    }

    int nIndex = atoi(argv[1]) - 1;
    int nLength = sizeof(TestEntry) / sizeof(TestEntry[0]);

    if (nIndex < 0 || nIndex >= nLength) {
        printf("*ERROR* Invalid testing case number\n");
        return -1;
    }

//    TPINIT;

    CTest TestObject;

    int nQuitCode = (TestObject.*TestEntry[nIndex])(argc, argv);

//    TPEND;

    return nQuitCode;
}
