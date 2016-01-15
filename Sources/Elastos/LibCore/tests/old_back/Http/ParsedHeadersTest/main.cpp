#include "test.h"
#include <stdio.h>
#include <stdlib.h>
#include <elautoptr.h>

typedef int (CTest::*PTestEntry)(int argc, char *argv[]);

PTestEntry TestEntry[] =
{
    &CTest::test_upperCaseHttpHeaders,//1
    &CTest::test_commaSeparatedCacheControlHeaders;//2
    &CTest::test_quotedFieldName,//3
    &CTest::test_unquotedValue,//4
    &CTest::test_quotedValue,//5
    &CTest::test_danglingQuote,//6
    &CTest::test_trailingComma,//7
    &CTest::test_trailingEquals,//8
    &CTest::test_spaceBeforeEquals,//9
    &CTest::test_spaceAfterEqualsWithQuotes,//10
    &CTest::test_spaceAfterEqualsWithoutQuotes,//11
    &CTest::test_pragmaDirectivesAreCaseInsensitive,//12
    &CTest::test_missingInteger,//13
    &CTest::test_invalidInteger,//14
    &CTest::test_veryLargeInteger,//15
    &CTest::test_negativeInteger,//16
    &CTest::test_parseChallengesWithCommaInRealm,//17
    &CTest::test_parseChallengesWithMultipleHeaders,//18
    &CTest::test_parseChallengesWithExtraWhitespace,//19
    &CTest::test_parseChallengesWithMissingRealm,//20
    &CTest::test_parseChallengesWithEmptyRealm,//21
    &CTest::test_parseChallengesWithMissingScheme,//22
    &CTest::test_parseChallengesWithManyParameters,//23
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

    CTest TestObject;

    int nQuitCode = (TestObject.*TestEntry[nIndex])(argc, argv);

    return nQuitCode;
}
