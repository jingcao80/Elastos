#include "test.h"
#include <stdio.h>
#include <stdlib.h>
#include <elautoptr.h>

typedef int (CTest::*PTestEntry)(int argc, char *argv[]);

PTestEntry TestEntry[] =
{
    &CTest::testBug19064, //1. ok
    &CTest::testBug19062, //1. ok
    &CTest::testUriParts, //1. ok
    &CTest::testEqualsCaseMapping, //2. ok
    &CTest::testFileEqualsWithEmptyHost, //3. ok
    &CTest::testEmptyHost, //4. ok
    &CTest::testNoHost, //5. ok
    &CTest::testNoPath, //6. ok
    &CTest::testEmptyHostAndNoPath, //7. ok
    &CTest::testNoHostAndNoPath, //8. ok
    &CTest::testAtSignInUserInfo, //9. ok
    &CTest::testUserNoPassword, //10. ok
    &CTest::testUserNoPasswordExplicitPort, //11. ok
    &CTest::testUserPasswordHostPort, //12. ok
    &CTest::testUserPasswordEmptyHostPort, //13. ok
    &CTest::testUserPasswordEmptyHostEmptyPort, //14. ok
    &CTest::testPathOnly, //15. ok
    &CTest::testQueryOnly, //16. ok
    &CTest::testFragmentOnly, //17. ok
    &CTest::testAtSignInPath, //18. ok
    &CTest::testColonInPath, //19. ok
    &CTest::testSlashInQuery, //20. ok
    &CTest::testQuestionMarkInQuery, //21. ok
    &CTest::testAtSignInQuery, //22. ok
    &CTest::testColonInQuery, //23. ok
    &CTest::testQuestionMarkInFragment, //24. ok
    &CTest::testColonInFragment, //25. ok
    &CTest::testSlashInFragment, //26. ok
    &CTest::testHashInFragment, //27. ok
    &CTest::testEmptyPort, //28. ok
    &CTest::testNonNumericPort, //29. ok
    &CTest::testNegativePort, //30. ok
    &CTest::testNegativePortEqualsPlaceholder, //31. ok
    &CTest::testRelativePathOnQuery, //32. ok
    &CTest::testRelativeFragmentOnQuery, //33. ok
    &CTest::testRelativePathAndFragment, //34. ok
    &CTest::testRelativeParentDirectory, //35. ok
    &CTest::testRelativeChildDirectory, //36. ok
    &CTest::testRelativeRootDirectory, //37. ok
    &CTest::testRelativeFullUrl, //38. ok
    &CTest::testRelativeDifferentScheme, //39. ok
    &CTest::testRelativeDifferentAuthority, //40. ok
    &CTest::testRelativeWithScheme, //41. ok
    &CTest::testMalformedUrlsRefusedByFirefoxAndChrome, //42. ok
    &CTest::testRfc1808NormalExamples, //43. ok
    &CTest::testRfc1808AbnormalExampleTooManyDotDotSequences, //44. not ok
    &CTest::testRfc1808AbnormalExampleRemoveDotSegments, //45. ok
    &CTest::testRfc1808AbnormalExampleNonsensicalDots, //46. ok
    &CTest::testRfc1808AbnormalExampleRelativeScheme, //47. ok
    &CTest::testRfc1808AbnormalExampleQueryOrFragmentDots, //48. ok
    &CTest::testSquareBracketsInUserInfo, //49. ok
    &CTest::testFileUriExtraLeadingSlashes, //50. ok
    &CTest::testFileUrlWithAuthority, //51. ok
    &CTest::testEmptyAuthority, //52. ok
    &CTest::testHttpUrlExtraLeadingSlashes, //53. ok
    &CTest::testFileUrlRelativePath, //54. ok
    &CTest::testFileUrlDottedPath, //55. ok
    &CTest::testParsingDotAsHostname, //56. ok
    &CTest::testSquareBracketsWithIPv4, //57. ok
    &CTest::testSquareBracketsWithHostname, //58. ok
    &CTest::testIPv6WithoutSquareBrackets, //59. ok
    &CTest::testEqualityWithNoPath, //60. ok
    &CTest::testRelativize, //61. ok
    &CTest::testParseServerAuthorityInvalidAuthority, //62. ok
    &CTest::testParseServerAuthorityOmittedAuthority, //63. ok
    &CTest::testEncodingParts, //64. not ok
    &CTest::testSchemeCaseIsNotCanonicalized, //65. ok
    &CTest::testEmptyAuthorityWithPath, //66. ok
    &CTest::testEmptyAuthorityWithQuery, //67. ok
    &CTest::testEmptyAuthorityWithFragment, //68. ok
    &CTest::testEncodingConstructorsRefuseRelativePath, //69. ok
    &CTest::testEncodingConstructorsAcceptEmptyPath, //70. ok
    &CTest::testResolveRelativeAndAbsolute, //71. ok
    &CTest::testRelativizeRelativeAndAbsolute, //72. ok
    &CTest::testPartContainsSpace, //73. ok
    &CTest::testUnderscore, //74. ok
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
