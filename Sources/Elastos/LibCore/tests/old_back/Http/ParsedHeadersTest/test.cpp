#include "test.h"

#define TEST(a, x) a.test_##x

CTest::CTest()
{
      CUri::New(String("http"),String("localhost"),String("/"),(IUri**)&uri);
}

CTest::~CTest(){}

int CTest::test_upperCaseHttpHeaders(int argc, char* argv[])
{
    AutoPtr<IRawHeaders> headers;
    CRawHeaders::New((IRawHeaders**)&headers);
    headers->Add(String("CACHE-CONTROL"), String("no-store"));
    headers->Add(String("DATE"), String("Thu, 01 Jan 1970 00:00:01 UTC"));
    headers->Add(String("EXPIRES", String("Thu, 01 Jan 1970 00:00:02 UTC"));
    headers->Add(String("LAST-MODIFIED", String("Thu, 01 Jan 1970 00:00:03 UTC"));
    headers->Add(String("ETAG", String("v1"));
    headers->Add(String("PRAGMA", String("no-cache"));
    AutoPtr<IResponseHeaders> parsedHeaders;
    CResponseHeaders::New(uri, headers, (IResponseHeaders**)&parsedHeaders);
    Boolean value = FALSE;
    parsedHeaders->IsNoStore(&value);
    assert(value);

    AutoPtr<IDate> d1,d2;
    CDate::New(1000L, (IDate**)&d1);
    parsedHeaders->GetServedDate((IDate**)&d2);
    value = FALSE;
    d1->Equals(d2, &value);
    assert(value);

    AutoPtr<IDate> d3,d4;
    CDate::New(2000L, (IDate**)&d3);
    parsedHeaders->GetExpires((IDate**)&d4);
    value = FALSE;
    d3->Equals(d4, &value);
    assert(value);

    AutoPtr<IDate> d5,d6;
    CDate::New(2000L, (IDate**)&d5);
    parsedHeaders->GetLastModified((IDate**)&d6);
    value = FALSE;
    d5->Equals(d6, &value);
    assert(value);

    String s;
    parsedHeaders->GetEtag(&s);
    assert(String("v1") == s);

    value = FALSE;
    parsedHeaders->IsNoCache(&value);
    assert(value);
    return 0;
}

int test_commaSeparatedCacheControlHeaders(int argc, char* argv[])
{
    AutoPtr<IRawHeaders> headers;
    CRawHeaders::New((IRawHeaders**)&headers);
    headers->Add(String("Cache-Control"), String("no-store, max-age=60, public"));
    AutoPtr<IResponseHeaders> parsedHeaders;
    CResponseHeaders::New(uri, headers, (IResponseHeaders**)&parsedHeaders);
    Boolean value = FALSE;
    parsedHeaders->IsNoStore(&value);
    assert(value);

    Int32 value;
    parsedHeaders->GetMaxAgeSeconds(&value);
    assert(value == 60);

    Boolean b = FALSE;
    parsedHeaders->IsPublic(&b);
    assert(b);
    return 0;
}

int test_quotedFieldName(int argc, char* argv[])
{
    AutoPtr<IRawHeaders> headers;
    CRawHeaders::New((IRawHeaders**)&headers);
    headers->Add(String("Cache-Control"), String("private=\"Set-Cookie\", no-store"));
    AutoPtr<IResponseHeaders> parsedHeaders;
    CResponseHeaders::New(uri, headers, (IResponseHeaders**)&parsedHeaders);
    Boolean value = FALSE;
    parsedHeaders->IsNoStore(&value);
    assert(value);
    return 0;
}

int test_unquotedValue(int argc, char* argv[])
{
    AutoPtr<IRawHeaders> headers;
    CRawHeaders::New((IRawHeaders**)&headers);
    headers->Add(String("Cache-Control"), String("private=Set-Cookie, no-store"));
    AutoPtr<IResponseHeaders> parsedHeaders;
    CResponseHeaders::New(uri, headers, (IResponseHeaders**)&parsedHeaders);
    Boolean value = FALSE;
    parsedHeaders->IsNoStore(&value);
    assert(value);
    return 0;
}

int test_quotedValue(int argc, char* argv[])
{
    AutoPtr<IRawHeaders> headers;
    CRawHeaders::New((IRawHeaders**)&headers);
    headers->Add(String("Cache-Control"), String("private=\" a, no-cache, c \", no-store"));
    AutoPtr<IResponseHeaders> parsedHeaders;
    CResponseHeaders::New(uri, headers, (IResponseHeaders**)&parsedHeaders);
    Boolean value = FALSE;
    parsedHeaders->IsNoStore(&value);
    assert(value);

    value = FALSE;
    parsedHeaders->IsNoCache(&value);
    assert(value);
    return 0;
}

int test_danglingQuote(int argc, char* argv[])
{
    AutoPtr<IRawHeaders> headers;
    CRawHeaders::New((IRawHeaders**)&headers);
    headers->Add(String("Cache-Control"), String("Cache-Control", "private=\"a, no-cache, c"));
    AutoPtr<IResponseHeaders> parsedHeaders;
    CResponseHeaders::New(uri, headers, (IResponseHeaders**)&parsedHeaders);
    Boolean value = FALSE;
    parsedHeaders->IsNoCache(&value);
    assert(value);
    return 0;
}

int test_trailingComma(int argc, char* argv[])
{
    AutoPtr<IRawHeaders> headers;
    CRawHeaders::New((IRawHeaders**)&headers);
    headers->Add(String("Cache-Control"), String("public,"));
    AutoPtr<IResponseHeaders> parsedHeaders;
    CResponseHeaders::New(uri, headers, (IResponseHeaders**)&parsedHeaders);
    Boolean value = FALSE;
    parsedHeaders->IsPublic(&value);
    assert(value);
    return 0;
}

int test_trailingEquals(int argc, char* argv[])
{
    AutoPtr<IRawHeaders> headers;
    CRawHeaders::New((IRawHeaders**)&headers);
    headers->Add(String("Cache-Control"), String("private="));
    AutoPtr<IResponseHeaders> parsedHeaders;
    CResponseHeaders::New(uri, headers, (IResponseHeaders**)&parsedHeaders);
    return 0;
}

int test_spaceBeforeEquals(int argc, char* argv[])
{
    AutoPtr<IRawHeaders> headers;
    CRawHeaders::New((IRawHeaders**)&headers);
    headers->Add(String("Cache-Control"), String("max-age =60"));
    AutoPtr<IResquestHeaders> parsedHeaders;
    CRequestHeaders::New(uri, headers, (IRequestHeaders**)&parsedHeaders);
    Int32 value;
    parsedHeaders->GetMaxAgeSeconds(&value);
    assert(60 == value);
    return 0;
}

int test_spaceAfterEqualsWithQuotes(int argc, char* argv[])
{
    AutoPtr<IRawHeaders> headers;
    CRawHeaders::New((IRawHeaders**)&headers);
    headers->Add(String("Cache-Control"), String("max-age= \"60\""));
    AutoPtr<IRequestHeaders> parsedHeaders;
    CRequestHeaders::New(uri, headers, (IRequestHeaders**)&parsedHeaders);
    Int32 value;
    parsedHeaders->GetMaxAgeSeconds(&value);
    assert(60 == value);
    return 0;
}

int test_spaceAfterEqualsWithoutQuotes(int argc, char* argv[])
{
    AutoPtr<IRawHeaders> headers;
    CRawHeaders::New((IRawHeaders**)&headers);
    headers->Add(String("Cache-Control"), String("max-age= 60"));
    AutoPtr<IResquestHeaders> parsedHeaders;
    CRequestHeaders::New(uri, headers, (IResquestHeaders**)&parsedHeaders);
    Int32 value;
    parsedHeaders->GetMaxAgeSeconds(&value);
    assert(60 == value);
    return 0;
}

int test_cacheControlRequestDirectivesAreCaseInsensitive(int argc, char* argv[])
{
    AutoPtr<IRawHeaders> headers;
    CRawHeaders::New((IRawHeaders**)&headers);
    headers->Add(String("Cache-Control"), String("NO-CACHE"));
    headers->Add(String("Cache-Control"), String("MAX-AGE=60"));
    headers->Add(String("Cache-Control"), String("MAX-STALE=70"));
    headers->Add(String("Cache-Control"), String("MIN-FRESH=80"));
    headers->Add(String("Cache-Control"), String("ONLY-IF-CACHED"));
    AutoPtr<IRequestHeaders> parsedHeaders;
    CRequestHeaders::New(uri, headers, (IRequestHeaders**)&parsedHeaders);
    Boolean value = FALSE;
    parsedHeaders->IsNoCache(&value);
    assert(value);

    Int32 i;
    parsedHeaders->GetMaxAgeSeconds(&i);
    assert(60==i);
    parsedHeaders->GetMaxStaleSeconds(&i);
    assert(70==i);
    parsedHeaders->GetMinFreshSeconds(&i);
    assert(80==i);
    value = FALSE;
    parsedHeaders->IsOnlyIfCached(&value);
    assert(value);
    return 0;
}

int test_cacheControlResponseDirectivesAreCaseInsensitive(int argc, char* argv[])
{
    AutoPtr<IRawHeaders> headers;
    CRawHeaders::New((IRawHeaders**)&headers);
    headers->Add(String("Cache-Control"), String("NO-CACHE"));
    headers->Add(String("Cache-Control"), String("NO-STORE"));
    headers->Add(String("Cache-Control"), String("MAX-AGE=60"));
    headers->Add(String("Cache-Control"), String("S-MAXAGE=70"));
    headers->Add(String("Cache-Control"), String("PUBLIC"));
    headers->Add(String("Cache-Control"), String("MUST-REVALIDATE"));
    AutoPtr<IResponseHeaders> parsedHeaders;
    CResponseHeaders::New(uri, headers, (IResponseHeaders**)&parsedHeaders);
    Boolean value = FALSE;
    parsedHeaders->IsNoCache(&value);
    assert(value);
    value = FALSE;
    parsedHeaders->IsNoStore(&value);
    assert(value);

    Int32 i;
    parsedHeaders->GetMaxAgeSeconds(&i);
    assert(60==i);
    parsedHeaders->GetMaxStaleSeconds(&i);
    assert(70==i);
    value = FALSE;
    parsedHeaders->IsPublic(&i);
    assert(value);
    value = FALSE;
    parsedHeaders->IsMustRevalidate(&value);
    assert(value);
    return 0;
}

int test_pragmaDirectivesAreCaseInsensitive(int argc, char* argv[])
{
    AutoPtr<IRawHeaders> headers;
    CRawHeaders::New((IRawHeaders**)&headers);
    headers->Add(String("Pragma"), String("NO-CACHE"));
    AutoPtr<IResquestHeaders> parsedHeaders;
    CRequestHeaders::New(uri, headers, (IResquestHeaders**)&parsedHeaders);
    Boolean value = FALSE;
    parsedHeaders->IsNoCache(&value);
    assert(value);
    return 0;
}

int test_missingInteger(int argc, char* argv[])
{
    AutoPtr<IRawHeaders> headers;
    CRawHeaders::New((IRawHeaders**)&headers);
    headers->Add(String("Cache-Control"), String("max-age"));
    AutoPtr<IRequestHeaders> parsedHeaders;
    CRequestHeaders::New(uri, headers, (IRequestHeaders**)&parsedHeaders);
    Int32 value;
    parsedHeaders->GetMaxAgeSeconds(&value);
    assert(-1 == value);
    return 0;
}

int test_invalidInteger(int argc, char* argv[])
{
    AutoPtr<IRawHeaders> headers;
    CRawHeaders::New((IRawHeaders**)&headers);
    headers->Add(String("Cache-Control"), String("MAX-AGE=pi"));
    AutoPtr<IRequestHeaders> parsedHeaders;
    CRequestHeaders::New(uri, headers, (IRequestHeaders**)&parsedHeaders);
    Int32 value;
    parsedHeaders->GetMaxAgeSeconds(&value);
    assert(-1 == value);
    return 0;
}

int test_veryLargeInteger(int argc, char* argv[])
{
    AutoPtr<IRawHeaders> headers;
    CRawHeaders::New((IRawHeaders**)&headers);
    StringBuilder s("MAX-AGE=");
    s += 0x7FFFFFFF + 1L;
    String str;
    s.ToString(&str);
    headers->Add(String("Cache-Control"), str);
    AutoPtr<IRequestHeaders> parsedHeaders;
    CRequestHeaders::New(uri, headers, (IRequestHeaders**)&parsedHeaders);
    Int32 i;
    parsedHeaders->GetMaxAgeSeconds(&i);
    assert(0x7FFFFFFF == i);
    return 0;
}

int test_negativeInteger(int argc, char* argv[])
{
    AutoPtr<IRawHeaders> headers;
    CRawHeaders::New((IRawHeaders**)&headers);
    headers->Add(String("Cache-Control"), String("MAX-AGE=-2"));
    AutoPtr<IRequestHeaders> parsedHeaders;
    CRequestHeaders::New(uri, headers, (IRequestHeaders**)&parsedHeaders);
    Int32 i;
    parsedHeaders->GetMaxAgeSeconds(&i);
    assert(0 == i);
    return 0;
}

int test_parseChallengesWithCommaInRealm(int argc, char* argv[])
{
    AutoPtr<IRawHeaders> headers;
    CRawHeaders::New((IRawHeaders**)&headers);
    headers->Add(String("WWW-Authenticate"), String("s1 realm=\"ab,cd\", s2 realm=\"ef,gh\""));
    // assertEquals(Arrays.asList(new Challenge("s1", "ab,cd"), new Challenge("s2", "ef,gh")),
    //         HeaderParser.parseChallenges(headers, "WWW-Authenticate"));
    return 0;
}

int test_parseChallengesWithMultipleHeaders(int argc, char* argv[])
{
    AutoPtr<IRawHeaders> headers;
    CRawHeaders::New((IRawHeaders**)&headers);
    headers->Add(String("WWW-Authenticate"), String("s1 realm=\"abc\""));
    headers->Add(String("WWW-Authenticate"), String("s2 realm=\"def\""));
    // assertEquals(Arrays.asList(new Challenge("s1", "abc"), new Challenge("s2", "def")),
    //         HeaderParser.parseChallenges(headers, "WWW-Authenticate"));
    return 0;
}

int test_parseChallengesWithExtraWhitespace(int argc, char* argv[])
{
    AutoPtr<IRawHeaders> headers;
    CRawHeaders::New((IRawHeaders**)&headers);
    headers->Add(String("WWW-Authenticate"), String("  s1  realm=\"a\"  ,  s2  realm=\"b\",  "));
    // assertEquals(Arrays.asList(new Challenge("s1", "a"), new Challenge("s2", "b")),
    //         HeaderParser.parseChallenges(headers, "WWW-Authenticate"));
    return 0;
}

int test_parseChallengesWithMissingRealm(int argc, char* argv[])
{
    AutoPtr<IRawHeaders> headers;
    CRawHeaders::New((IRawHeaders**)&headers);
    headers->Add(String("WWW-Authenticate"), String("Basic"));
    // assertEquals(Collections.<Challenge>emptyList(),
    //         HeaderParser.parseChallenges(headers, "WWW-Authenticate"));
    return 0;
}

int test_parseChallengesWithEmptyRealm(int argc, char* argv[])
{
    AutoPtr<IRawHeaders> headers;
    CRawHeaders::New((IRawHeaders**)&headers);
    headers->Add(String("WWW-Authenticate"), String("Basic realm=\"\""));
    // assertEquals(Arrays.asList(new Challenge("Basic", "")),
    //         HeaderParser.parseChallenges(headers, "WWW-Authenticate"));
    return 0;
}

int test_parseChallengesWithMissingScheme(int argc, char* argv[])
{
    AutoPtr<IRawHeaders> headers;
    CRawHeaders::New((IRawHeaders**)&headers);
    headers->Add(String("WWW-Authenticate"), String("realm=\"a\""));
    // assertEquals(Collections.<Challenge>emptyList(),
    //         HeaderParser.parseChallenges(headers, "WWW-Authenticate"));
    return 0;
}

// http://code.google.com/p/android/issues/detail?id=11140
int test_parseChallengesWithManyParameters(int argc, char* argv[])
{
    AutoPtr<IRawHeaders> headers;
    CRawHeaders::New((IRawHeaders**)&headers);
    StringBuilder s("Digest realm=\"abc\",");
    s += " qop=\"auth,auth-int\",";
    s += " nonce=\"dcd98b7102dd2f0e8b11d0f600bfb0c093\",";
    s += " opaque=\"5ccc069c403ebaf9f0171e9517f40e41\",";
    s += " Basic realm=\"def\"";
    String str;
    s.ToString(&str);
    headers->Add(String("WWW-Authenticate"), str);
    // assertEquals(Arrays.asList(new Challenge("Digest", "abc"), new Challenge("Basic", "def")),
    //         HeaderParser.parseChallenges(headers, "WWW-Authenticate"));
    return 0;
}