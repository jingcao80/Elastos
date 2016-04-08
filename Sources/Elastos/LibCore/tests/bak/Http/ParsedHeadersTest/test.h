#ifndef __DATAGRAMSOCKET_TEST_H_
#define __DATAGRAMSOCKET_TEST_H_

#include "cmdef.h"
#include "Elastos.CoreLibrary.h"
#include <elastos.h>
#include <elautoptr.h>

using namespace Elastos;
using namespace Elastos::Core;
using namespace Elastos::Net;
using namespace Elastos::Utility;
using Elastos::Net::Http::IRawHeaders;
using Elastos::Net::Http::IRawHeadersHelper;
using Elastos::Net::Http::CRawHeadersHelper;
using Elastos::Net::Http::CRawHeaders;
using Elastos::Net::Http::IResponseHeaders;
using Elastos::Net::Http::CResponseHeaders;
using Elastos::Net::Http::IRequestHeaders;
using Elastos::Net::Http::CRequestHeaders;


#define TEST(a, x) a.test_##x

class CTest {
public:
    CTest();
    ~CTest();

public:
    int test_upperCaseHttpHeaders(int argc, char* argv[]);

    int test_commaSeparatedCacheControlHeaders(int argc, char* argv[]);

    int test_quotedFieldName(int argc, char* argv[]);

    int test_unquotedValue(int argc, char* argv[]);

    int test_quotedValue(int argc, char* argv[]);

    int test_danglingQuote(int argc, char* argv[]);

    int test_trailingComma(int argc, char* argv[]);

    int test_trailingEquals(int argc, char* argv[]);

    int test_spaceBeforeEquals(int argc, char* argv[]);

    int test_spaceAfterEqualsWithQuotes(int argc, char* argv[]);

    int test_spaceAfterEqualsWithoutQuotes(int argc, char* argv[]);

    int test_pragmaDirectivesAreCaseInsensitive(int argc, char* argv[]);

    int test_missingInteger(int argc, char* argv[]);

    int test_invalidInteger(int argc, char* argv[]);

    int test_veryLargeInteger(int argc, char* argv[]);

    int test_negativeInteger(int argc, char* argv[]);

    int test_parseChallengesWithCommaInRealm(int argc, char* argv[]);

    int test_parseChallengesWithMultipleHeaders(int argc, char* argv[]);

    int test_parseChallengesWithExtraWhitespace(int argc, char* argv[]);

    int test_parseChallengesWithMissingRealm(int argc, char* argv[]);

    int test_parseChallengesWithEmptyRealm(int argc, char* argv[]);

    int test_parseChallengesWithMissingScheme(int argc, char* argv[]);

    // http://code.google.com/p/android/issues/detail?id=11140
    int test_parseChallengesWithManyParameters(int argc, char* argv[]);

private:
    AutoPtr<IUri> uri;

};

#endif // __DATAGRAMSOCKET_TEST_H_