#ifndef __INETADDRESSHELPER_TEST_H_
#define __INETADDRESSHELPER_TEST_H_

#include <elastos.h>
#include "Elastos.CoreLibrary.h"

using namespace Elastos;
using namespace Elastos::Net;

#define TEST(a, x) a.test_##x

class CTest {
public:
    CTest();
    ~CTest();

public:
    int testBug19064(int argc, char* argv[]);
    int testBug19062(int argc, char* argv[]);
    int testUriParts(int argc, char* argv[]);
    int testEqualsCaseMapping(int argc, char* argv[]);
    int testFileEqualsWithEmptyHost(int argc, char* argv[]);
    int testUriSerialization(int argc, char* argv[]);
    int testEmptyHost(int argc, char* argv[]);
    int testNoHost(int argc, char* argv[]);
    int testNoPath(int argc, char* argv[]);
    int testEmptyHostAndNoPath(int argc, char* argv[]);
    int testNoHostAndNoPath(int argc, char* argv[]);
    int testAtSignInUserInfo(int argc, char* argv[]);
    int testUserNoPassword(int argc, char* argv[]);
    int testUserNoPasswordExplicitPort(int argc, char* argv[]);
    int testUserPasswordHostPort(int argc, char* argv[]);
    int testUserPasswordEmptyHostPort(int argc, char* argv[]);
    int testUserPasswordEmptyHostEmptyPort(int argc, char* argv[]);
    int testPathOnly(int argc, char* argv[]);
    int testQueryOnly(int argc, char* argv[]);
    int testFragmentOnly(int argc, char* argv[]);
    int testAtSignInPath(int argc, char* argv[]);
    int testColonInPath(int argc, char* argv[]);
    int testSlashInQuery(int argc, char* argv[]);
    int testQuestionMarkInQuery(int argc, char* argv[]);
    int testAtSignInQuery(int argc, char* argv[]);
    int testColonInQuery(int argc, char* argv[]);
    int testQuestionMarkInFragment(int argc, char* argv[]);
    int testColonInFragment(int argc, char* argv[]);
    int testSlashInFragment(int argc, char* argv[]);
    int testHashInFragment(int argc, char* argv[]);
    int testEmptyPort(int argc, char* argv[]);
    int testNonNumericPort(int argc, char* argv[]);
    int testNegativePort(int argc, char* argv[]);
    int testNegativePortEqualsPlaceholder(int argc, char* argv[]);
    int testRelativePathOnQuery(int argc, char* argv[]);
    int testRelativeFragmentOnQuery(int argc, char* argv[]);
    int testRelativePathAndFragment(int argc, char* argv[]);
    int testRelativeParentDirectory(int argc, char* argv[]);
    int testRelativeChildDirectory(int argc, char* argv[]);
    int testRelativeRootDirectory(int argc, char* argv[]);
    int testRelativeFullUrl(int argc, char* argv[]);
    int testRelativeDifferentScheme(int argc, char* argv[]);
    int testRelativeDifferentAuthority(int argc, char* argv[]);
    int testRelativeWithScheme(int argc, char* argv[]);
    int testMalformedUrlsRefusedByFirefoxAndChrome(int argc, char* argv[]);
    int testRfc1808NormalExamples(int argc, char* argv[]);
    int testRfc1808AbnormalExampleTooManyDotDotSequences(int argc, char* argv[]);
    int testRfc1808AbnormalExampleRemoveDotSegments(int argc, char* argv[]);
    int testRfc1808AbnormalExampleNonsensicalDots(int argc, char* argv[]);
    int testRfc1808AbnormalExampleRelativeScheme(int argc, char* argv[]);
    int testRfc1808AbnormalExampleQueryOrFragmentDots(int argc, char* argv[]);
    int testSquareBracketsInUserInfo(int argc, char* argv[]);
    int testFileUriExtraLeadingSlashes(int argc, char* argv[]);
    int testFileUrlWithAuthority(int argc, char* argv[]);
    int testEmptyAuthority(int argc, char* argv[]);
    int testHttpUrlExtraLeadingSlashes(int argc, char* argv[]);
    int testFileUrlRelativePath(int argc, char* argv[]);
    int testFileUrlDottedPath(int argc, char* argv[]);
    int testParsingDotAsHostname(int argc, char* argv[]);
    int testSquareBracketsWithIPv4(int argc, char* argv[]);
    int testSquareBracketsWithHostname(int argc, char* argv[]);
    int testIPv6WithoutSquareBrackets(int argc, char* argv[]);
    int testEqualityWithNoPath(int argc, char* argv[]);
    int testRelativize(int argc, char* argv[]);
    int testParseServerAuthorityInvalidAuthority(int argc, char* argv[]);
    int testParseServerAuthorityOmittedAuthority(int argc, char* argv[]);
    int testEncodingParts(int argc, char* argv[]);
    int testSchemeCaseIsNotCanonicalized(int argc, char* argv[]);
    int testEmptyAuthorityWithPath(int argc, char* argv[]);
    int testEmptyAuthorityWithQuery(int argc, char* argv[]);
    int testEmptyAuthorityWithFragment(int argc, char* argv[]);
    int testEncodingConstructorsRefuseRelativePath(int argc, char* argv[]);
    int testEncodingConstructorsAcceptEmptyPath(int argc, char* argv[]);
    int testResolveRelativeAndAbsolute(int argc, char* argv[]);
    int testRelativizeRelativeAndAbsolute(int argc, char* argv[]);
    int testPartContainsSpace(int argc, char* argv[]);
    int testUnderscore(int argc, char* argv[]);
};

#endif // __INETADDRESSHELPER_TEST_H_