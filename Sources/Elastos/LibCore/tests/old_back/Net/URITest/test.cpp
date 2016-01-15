#include "test.h"

CTest::CTest()
{}

CTest::~CTest()
{}

int CTest::testBug19064(int argc, char* argv[])
{
    PEL("CTest::testBug19064")
    AutoPtr<IURI> uri = NULL;
    AutoPtr<IURI> relative = NULL;
    AutoPtr<IURI> result = NULL;
    AutoPtr<IURI> uri_result = NULL;
    Int32 myresult = 0;
    ECode ec;

    ec = CURI::New(String("http://android.com/"), (IURI**)&uri);
        assert(ec == NOERROR);

    ec = CURI::New(String("http://android.com/robots.txt"), (IURI**)&relative);
        assert(ec == NOERROR);

    ec = CURI::New(String("robots.txt"), (IURI**)&result);
        assert(ec == NOERROR);

    ec = uri->Relativize(relative, (IURI**)&uri_result);
        assert(ec == NOERROR);

    uri_result->CompareTo(result, &myresult);
        assert(myresult == 0);

    return 0;
}

int CTest::testBug19062(int argc, char* argv[])
{
    PEL("CTest::testBug19062")
    AutoPtr<IURI> uri = NULL;
    AutoPtr<IURI> uri_result = NULL;
    AutoPtr<IURI> result = NULL;
    Int32 myresult = 0;
    ECode ec;

    ec = CURI::New(String("x/y/../z/./q"), (IURI**)&uri);
        assert(ec == NOERROR);

    ec = CURI::New(String("x/z/q"), (IURI**)&result);
        assert(ec == NOERROR);

    ec = uri->Normalize((IURI**)&uri_result);
        assert(ec == NOERROR);

    uri_result->CompareTo(result, &myresult);
        assert(myresult == 0);
    return 0;
}

//Num: Test1
int CTest::testUriParts(int argc, char* argv[])
{
    AutoPtr<IURI> uri;
    CURI::New(String("http://username:password@host:8080/directory/file?query#ref"), (IURI**)&uri);
    String outStr;
    uri->GetScheme(&outStr);
    assert(outStr.Equals(String("http")));
    uri->GetAuthority(&outStr);
    assert(outStr.Equals(String("username:password@host:8080")));
    uri->GetRawAuthority(&outStr);
    assert(outStr.Equals(String("username:password@host:8080")));
    uri->GetUserInfo(&outStr);
    assert(outStr.Equals(String("username:password")));
    uri->GetRawUserInfo(&outStr);
    assert(outStr.Equals(String("username:password")));
    uri->GetHost(&outStr);
    assert(outStr.Equals(String("host")));
    Int32 port;
    uri->GetPort(&port);
    assert(port == 8080);
    uri->GetPath(&outStr);
    assert(outStr.Equals(String("/directory/file")));
    uri->GetRawPath(&outStr);
    assert(outStr.Equals(String("/directory/file")));
    uri->GetQuery(&outStr);
    assert(outStr.Equals(String("query")));
    uri->GetRawQuery(&outStr);
    assert(outStr.Equals(String("query")));
    uri->GetFragment(&outStr);
    assert(outStr.Equals(String("ref")));
    uri->GetRawFragment(&outStr);
    assert(outStr.Equals(String("ref")));
    uri->GetSchemeSpecificPart(&outStr);
    assert(outStr.Equals(String("//username:password@host:8080/directory/file?query")));
    uri->GetRawSchemeSpecificPart(&outStr);
    assert(outStr.Equals(String("//username:password@host:8080/directory/file?query")));
}

//Num: Test2
int CTest::testEqualsCaseMapping(int argc, char* argv[])
{
    String strURIlowcase0("HTTP://localhost/foo?bar=baz#quux");
    AutoPtr<IURI> uri1;
    AutoPtr<IURI> uri2;
    CURI::New(strURIlowcase0, (IURI**)&uri1);
    CURI::New(strURIlowcase0, (IURI**)&uri2);
    Boolean flag = FALSE;
    uri1->Equals(uri2, &flag);
    assert(flag == TRUE);

    String strURIlowcase1("http://localhost/foo?bar=baz#quux");
    CURI::New(strURIlowcase1, (IURI**)&uri1);
    String strURIlowcase2("http://LOCALHOST/foo?bar=baz#quux");
    CURI::New(strURIlowcase2, (IURI**)&uri2);
    uri1->Equals(uri2, &flag);
    assert(flag == TRUE);

    String strURIlowcase3("http://localhost/FOO?bar=baz#quux");
    CURI::New(strURIlowcase3, (IURI**)&uri2);
    uri1->Equals(uri2, &flag);
    assert(flag == FALSE);

    String strURIlowcase4("http://localhost/foo?BAR=BAZ#quux");
    CURI::New(strURIlowcase4, (IURI**)&uri2);
    uri1->Equals(uri2, &flag);
    assert(flag == FALSE);

    String strURIlowcase5("http://localhost/foo?bar=baz#QUUX");
    CURI::New(strURIlowcase5, (IURI**)&uri2);
    uri1->Equals(uri2, &flag);
    assert(flag == FALSE);
}

//Num: Test3
int CTest::testFileEqualsWithEmptyHost(int argc, char* argv[])
{
    String str0("file");
    String str1("");
    String str2("/a/");
    String str3 = String(NULL);
    String str4("file:/a/");
    AutoPtr<IURI> uri1;
    CURI::New(str0, str1, str2, str3, (IURI**)&uri1);
    AutoPtr<IURI> uri2;
    CURI::New(str4, (IURI**)&uri2);
    Boolean flag = FALSE;
    uri1->Equals(uri2, &flag);
    assert(flag == TRUE);

    CURI::New(str0, str3, str2, str3, (IURI**)&uri1);
    uri1->Equals(uri2, &flag);
    assert(flag == TRUE);
}

//Num: Test4
int CTest::testEmptyHost(int argc, char* argv[])
{
    String str0("http:///path");
    AutoPtr<IURI> uri;
    CURI::New(str0, (IURI**)&uri);
    String ret;
    uri->GetHost(&ret);
    assert(ret.Equals(String(NULL)));

    uri->GetPath(&ret);
    assert(ret.Equals(String("/path")));
}

//Num: Test5
int CTest::testNoHost(int argc, char* argv[])
{
    String str0("http:/path");
    AutoPtr<IURI> uri;
    CURI::New(str0, (IURI**)&uri);
    String ret;
    uri->GetHost(&ret);
    assert(ret.Equals(String(NULL)));

    uri->GetPath(&ret);
    assert(ret.Equals(String("/path")));
}

//Num: Test6
int CTest::testNoPath(int argc, char* argv[])
{
    AutoPtr<IURI> uri;
    CURI::New(String("http://host"), (IURI**)&uri);
    String ret;
    uri->GetHost(&ret);
    assert(ret.Equals(String("host")));

    uri->GetPath(&ret);
    assert(ret.Equals(String("")));
}

//Num: Test7
int CTest::testEmptyHostAndNoPath(int argc, char* argv[])
{
    AutoPtr<IURI> uri;
    String str0("http://");
    ECode ec = CURI::New(str0, (IURI**)&uri);
    assert(ec == E_URI_SYNTAX_EXCEPTION);
}

//Num: Test8
int CTest::testNoHostAndNoPath(int argc, char* argv[])
{
    String str0("http:");
    AutoPtr<IURI> uri;
    ECode ec = CURI::New(str0, (IURI**)&uri);
    assert(ec == E_URI_SYNTAX_EXCEPTION);
}

//Num: Test9
int CTest::testAtSignInUserInfo(int argc, char* argv[])
{
    String str0("http://user@userhost.com:password@host");
    AutoPtr<IURI> uri;
    CURI::New(str0, (IURI**)&uri);
    String outStr;
    uri->GetAuthority(&outStr);
    assert(outStr.Equals(String("user@userhost.com:password@host")));

    uri->GetUserInfo(&outStr);
    assert(outStr.Equals(String(NULL)));

    uri->GetHost(&outStr);
    assert(outStr.Equals(String(NULL)));
}

//Num: Test10
int CTest::testUserNoPassword(int argc, char* argv[])
{
    String str0("http://user@host");
    AutoPtr<IURI> uri;
    CURI::New(str0, (IURI**)&uri);
    String ret;
    uri->GetAuthority(&ret);
    assert(ret.Equals(String("user@host")));

    uri->GetUserInfo(&ret);
    assert(ret.Equals(String("user")));

    uri->GetHost(&ret);
    assert(ret.Equals(String("host")));
}

//Num: Test11
int CTest::testUserNoPasswordExplicitPort(int argc, char* argv[])
{
    String str0("http://user@host:8080");
    AutoPtr<IURI> uri;
    CURI::New(str0, (IURI**)&uri);
    String ret;
    uri->GetAuthority(&ret);
    assert(ret.Equals(String("user@host:8080")));

    uri->GetUserInfo(&ret);
    assert(ret.Equals(String("user")));

    uri->GetHost(&ret);
    assert(ret.Equals(String("host")));

    Int32 port;
    uri->GetPort(&port);
    assert(port == 8080);
}

//Num: Test12
int CTest::testUserPasswordHostPort(int argc, char* argv[])
{
    String str0("http://user:password@host:8080");
    AutoPtr<IURI> uri;
    CURI::New(str0, (IURI**)&uri);
    String ret;
    uri->GetAuthority(&ret);
    assert(ret.Equals(String("user:password@host:8080")));

    uri->GetUserInfo(&ret);
    assert(ret.Equals(String("user:password")));

    uri->GetHost(&ret);
    assert(ret.Equals(String("host")));

    Int32 port;
    uri->GetPort(&port);
    assert(port == 8080);
}

//Num: Test13
int CTest::testUserPasswordEmptyHostPort(int argc, char* argv[])
{
    String str0("http://user:password@:8080");
    AutoPtr<IURI> uri;
    CURI::New(str0, (IURI**)&uri);
    String ret;
    uri->GetAuthority(&ret);
    assert(ret.Equals(String("user:password@:8080")));

    uri->GetUserInfo(&ret);
    assert(ret.Equals(String(NULL)));

    uri->GetHost(&ret);
    assert(ret.Equals(String(NULL)));

    Int32 port;
    uri->GetPort(&port);
    assert(port == -1);
}

//Num: Test14
int CTest::testUserPasswordEmptyHostEmptyPort(int argc, char* argv[])
{
    String str0("http://user:password@:");
    AutoPtr<IURI> uri;
    CURI::New(str0, (IURI**)&uri);
    String ret;
    uri->GetAuthority(&ret);
    assert(ret.Equals(String("user:password@:")));
    // from RI. this is curious
    uri->GetUserInfo(&ret);
    assert(ret.Equals(String(NULL)));

    uri->GetHost(&ret);
    assert(ret.Equals(String(NULL)));

    Int32 port;
    uri->GetPort(&port);
    assert(port == -1);
}

//Num: Test15
int CTest::testPathOnly(int argc, char* argv[])
{
    String str0("http://host/path");
    AutoPtr<IURI> uri;
    CURI::New(str0, (IURI**)&uri);
    String ret;
    uri->GetHost(&ret);
    assert(ret.Equals(String("host")));

    uri->GetPath(&ret);
    assert(ret.Equals(String("/path")));
}

//Num: Test16
int CTest::testQueryOnly(int argc, char* argv[])
{
   String str0("http://host?query");
   AutoPtr<IURI> uri;
   CURI::New(str0, (IURI**)&uri);
   String ret;
   uri->GetHost(&ret);
   assert(ret.Equals(String("host")));

   uri->GetPath(&ret);
   assert(ret.Equals(String("")));

   uri->GetQuery(&ret);
   assert(ret.Equals(String("query")));
}

//Num: Test17
int CTest::testFragmentOnly(int argc, char* argv[])
{
    String str0("http://host#fragment");
    AutoPtr<IURI> uri;
    CURI::New(str0, (IURI**)&uri);
    String ret;
    uri->GetHost(&ret);
    assert(ret.Equals(String("host")));

    uri->GetPath(&ret);
    assert(ret.Equals(String("")));

    uri->GetQuery(&ret);
    assert(ret.Equals(String(NULL)));

    uri->GetFragment(&ret);
    assert(ret.Equals(String("fragment")));
}

//Num: Test18
int CTest::testAtSignInPath(int argc, char* argv[])
{
    String str0("http://host/file@foo");
    AutoPtr<IURI> uri;
    CURI::New(str0, (IURI**)&uri);
    String ret;
    uri->GetPath(&ret);
    assert(ret.Equals(String("/file@foo")));

    uri->GetUserInfo(&ret);
    assert(ret.Equals(String(NULL)));
}

//Num: Test19
int CTest::testColonInPath(int argc, char* argv[])
{
    String str0("http://host/file:colon");
    AutoPtr<IURI> uri;
    CURI::New(str0, (IURI**)&uri);
    String ret;
    uri->GetPath(&ret);
    assert(ret.Equals(String("/file:colon")));
}

//Num: Test20
int CTest::testSlashInQuery(int argc, char* argv[])
{
    String str0("http://host/file?query/path");
    AutoPtr<IURI> uri;
    CURI::New(str0, (IURI**)&uri);
    String ret;
    uri->GetPath(&ret);
    assert(ret.Equals(String("/file")));

    uri->GetQuery(&ret);
    assert(ret.Equals(String("query/path")));
}

//Num: Test21
int CTest::testQuestionMarkInQuery(int argc, char* argv[])
{
    String str0("http://host/file?query?another");
    AutoPtr<IURI> uri;
    CURI::New(str0, (IURI**)&uri);
    String ret;
    uri->GetPath(&ret);
    assert(ret.Equals(String("/file")));

    uri->GetQuery(&ret);
    assert(ret.Equals(String("query?another")));
}

//Num: Test22
int CTest::testAtSignInQuery(int argc, char* argv[])
{
    String str0("http://host/file?query@at");
    AutoPtr<IURI> uri;
    CURI::New(str0, (IURI**)&uri);
    String ret;
    uri->GetPath(&ret);
    assert(ret.Equals(String("/file")));

    uri->GetQuery(&ret);
    assert(ret.Equals(String("query@at")));
}

//Num: Test23
int CTest::testColonInQuery(int argc, char* argv[])
{
    String str0("http://host/file?query:colon");
    AutoPtr<IURI> uri;
    CURI::New(str0, (IURI**)&uri);
    String ret;
    uri->GetPath(&ret);
    assert(ret.Equals(String("/file")));
    uri->GetQuery(&ret);
    assert(ret.Equals(String("query:colon")));
}

//Num: Test24
int CTest::testQuestionMarkInFragment(int argc, char* argv[])
{
    String str0("http://host/file#fragment?query");
    AutoPtr<IURI> uri;
    CURI::New(str0, (IURI**)&uri);
    String ret;
    uri->GetPath(&ret);
    assert(ret.Equals(String("/file")));

    uri->GetQuery(&ret);
    assert(ret.Equals(String(NULL)));

    uri->GetFragment(&ret);
    assert(ret.Equals(String("fragment?query")));
}

//Num: Test25
int CTest::testColonInFragment(int argc, char* argv[])
{
    String str0("http://host/file#fragment:80");
    AutoPtr<IURI> uri;
    CURI::New(str0, (IURI**)&uri);
    String ret;
    uri->GetPath(&ret);
    assert(ret.Equals(String("/file")));

    Int32 port;
    uri->GetPort(&port);
    assert(port == -1);

    uri->GetFragment(&ret);
    assert(ret.Equals(String("fragment:80")));
}

//Num: Test26
int CTest::testSlashInFragment(int argc, char* argv[])
{
    String str0("http://host/file#fragment/path");
    AutoPtr<IURI> uri;
    CURI::New(str0, (IURI**)&uri);
    String ret;
    uri->GetPath(&ret);
    assert(ret.Equals(String("/file")));

    uri->GetFragment(&ret);
    assert(ret.Equals(String("fragment/path")));
}

//Num: Test27
int CTest::testHashInFragment(int argc, char* argv[])
{
    String str0("http://host/file#fragment#another");
    AutoPtr<IURI> uri;
    ECode ec = CURI::New(str0, (IURI**)&uri);
    assert(ec == E_URI_SYNTAX_EXCEPTION);
}

//Num: Test28
int CTest::testEmptyPort(int argc, char* argv[])
{
    String str0("http://host:/");
    AutoPtr<IURI> uri;
    CURI::New(str0, (IURI**)&uri);
    Int32 port;
    uri->GetPort(&port);
    assert(port == -1);
}

//Num: Test29
int CTest::testNonNumericPort(int argc, char* argv[])
{
    String str0("http://host:x/");
    AutoPtr<IURI> uri;
    CURI::New(str0, (IURI**)&uri);
    String ret;
    uri->GetHost(&ret);
    assert(ret.Equals(String(NULL)));

    Int32 port;
    uri->GetPort(&port);
    assert(-1 == port);
}

//Num: Test30
int CTest::testNegativePort(int argc, char* argv[])
{
    String str0("http://host:-2/");
    AutoPtr<IURI> uri;
    CURI::New(str0, (IURI**)&uri);
    String ret;
    uri->GetHost(&ret);
    assert(ret.Equals(String(NULL)));

    Int32 port;
    uri->GetPort(&port);
    assert(-1 == port);
}

//Num: Test31
int CTest::testNegativePortEqualsPlaceholder(int argc, char* argv[])
{
    String str0("http://host:-1/");
    AutoPtr<IURI> uri;
    CURI::New(str0, (IURI**)&uri);
    // From the RI. This is curious
    String ret;
    uri->GetHost(&ret);
    assert(ret.Equals(String(NULL)));

    Int32 port;
    uri->GetPort(&port);
    assert(-1 == port);
}

//Num: Test32
int CTest::testRelativePathOnQuery(int argc, char* argv[])
{
    String str0("http://host/file?query/x");
    AutoPtr<IURI> base;
    CURI::New(str0, (IURI**)&base);
    AutoPtr<IURI> uri;
    base->ResolveEx(String("another"), (IURI**)&uri);
    String ret;
    uri->ToString(&ret);
    assert(ret.Equals(String("http://host/another")));

    uri->GetPath(&ret);
    assert(ret.Equals(String("/another")));

    uri->GetQuery(&ret);
    assert(ret.Equals(String(NULL)));

    uri->GetFragment(&ret);
    assert(ret.Equals(String(NULL)));
}

//Num: Test33
int CTest::testRelativeFragmentOnQuery(int argc, char* argv[])
{
    String str0("http://host/file?query/x#fragment");
    AutoPtr<IURI> base;
    CURI::New(str0, (IURI**)&base);
    AutoPtr<IURI> uri;
    base->ResolveEx(String("#another"), (IURI**)&uri);
    String ret;
    uri->ToString(&ret);
    assert(ret.Equals(String("http://host/file?query/x#another")));

    uri->GetPath(&ret);
    assert(ret.Equals(String("/file")));

    uri->GetQuery(&ret);
    assert(ret.Equals(String("query/x")));

    uri->GetFragment(&ret);
    assert(ret.Equals(String("another")));
}

//Num: Test34
int CTest::testRelativePathAndFragment(int argc, char* argv[])
{
    String str0("http://host/file");
    AutoPtr<IURI> base;
    CURI::New(str0, (IURI**)&base);
    AutoPtr<IURI> uri;
    base->ResolveEx(String("another#fragment"), (IURI**)&uri);
    String ret;
    uri->ToString(&ret);
    assert(ret.Equals(String("http://host/another#fragment")));
}

//Num: Test35
int CTest::testRelativeParentDirectory(int argc, char* argv[])
{
    String str0("http://host/a/b/c");
    AutoPtr<IURI> base;
    CURI::New(str0, (IURI**)&base);
    AutoPtr<IURI> uri;
    base->ResolveEx(String("../d"), (IURI**)&uri);
    String ret;
    uri->ToString(&ret);
    assert(ret.Equals(String("http://host/a/d")));
}

//Num: Test36
int CTest::testRelativeChildDirectory(int argc, char* argv[])
{
    String str0("http://host/a/b/c");
    AutoPtr<IURI> base;
    CURI::New(str0, (IURI**)&base);
    AutoPtr<IURI> uri;
    base->ResolveEx(String("d/e"), (IURI**)&uri);
    String ret;
    uri->ToString(&ret);
    assert(ret.Equals(String("http://host/a/b/d/e")));
}

//Num: Test37
int CTest::testRelativeRootDirectory(int argc, char* argv[])
{
    String str0("http://host/a/b/c");
    AutoPtr<IURI> base;
    CURI::New(str0, (IURI**)&base);
    AutoPtr<IURI> uri;
    base->ResolveEx(String("/d"), (IURI**)&uri);
    String ret;
    uri->ToString(&ret);
    assert(ret.Equals(String("http://host/d")));
}

//Num: Test38
int CTest::testRelativeFullUrl(int argc, char* argv[])
{
    String str0("http://host/a/b/c");
    AutoPtr<IURI> base;
    CURI::New(str0, (IURI**)&base);
    AutoPtr<IURI> uri;
    base->ResolveEx(String("http://host2/d/e"), (IURI**)&uri);
    String ret;
    uri->ToString(&ret);
    assert(ret.Equals(String("http://host2/d/e")));

    base->ResolveEx(String("https://host2/d/e"), (IURI**)&uri);
    uri->ToString(&ret);
    assert(ret.Equals(String("https://host2/d/e")));
}

//Num: Test39
int CTest::testRelativeDifferentScheme(int argc, char* argv[])
{
    String str0("http://host/a/b/c");
    AutoPtr<IURI> base;
    CURI::New(str0, (IURI**)&base);
    AutoPtr<IURI> uri;
    base->ResolveEx(String("https://host2/d/e"), (IURI**)&uri);
    String ret;
    uri->ToString(&ret);
    assert(ret.Equals(String("https://host2/d/e")));
}

//Num: Test40
int CTest::testRelativeDifferentAuthority(int argc, char* argv[])
{
    String str0("http://host/a/b/c");
    AutoPtr<IURI> base;
    CURI::New(str0, (IURI**)&base);
    AutoPtr<IURI> uri;
    base->ResolveEx(String("//another/d/e"), (IURI**)&uri);
    String ret;
    uri->ToString(&ret);
    assert(ret.Equals(String("http://another/d/e")));
}

//Num: Test41
int CTest::testRelativeWithScheme(int argc, char* argv[])
{
    String str0("http://host/a/b/c");
    AutoPtr<IURI> base;
    CURI::New(str0, (IURI**)&base);
    AutoPtr<IURI> uri;
    ECode ec = base->ResolveEx(String("http:"), (IURI**)&uri);
    assert(ec == E_ILLEGAL_ARGUMENT_EXCEPTION);

    base->ResolveEx(String("http:/"), (IURI**)&uri);
    String ret;
    uri->ToString(&ret);
    Boolean flag = FALSE;
    assert(ret.Equals(String("http:/")));
}

//Num: Test42
int CTest::testMalformedUrlsRefusedByFirefoxAndChrome(int argc, char* argv[])
{
    String str0("http://host/a/b/c");
    AutoPtr<IURI> base;
    CURI::New(str0, (IURI**)&base);
    AutoPtr<IURI> uri;
    ECode ec = base->ResolveEx(String("http://"), (IURI**)&uri);
    assert(ec == E_ILLEGAL_ARGUMENT_EXCEPTION);

    ec = base->ResolveEx(String("//"), (IURI**)&uri);
    assert(ec == E_ILLEGAL_ARGUMENT_EXCEPTION);

    ec = base->ResolveEx(String("https:"), (IURI**)&uri);
    assert(ec == E_ILLEGAL_ARGUMENT_EXCEPTION);

    base->ResolveEx(String("https:/"), (IURI**)&uri);
    String ret;
    uri->ToString(&ret);
    assert(ret.Equals(String("https:/")));

    ec = base->ResolveEx(String("https://"), (IURI**)&uri);
    assert(ec == E_ILLEGAL_ARGUMENT_EXCEPTION);
}

//Num: Test43
int CTest::testRfc1808NormalExamples(int argc, char* argv[])
{
    String str0("http://a/b/c/d;p?q");
    AutoPtr<IURI> base;
    CURI::New(str0, (IURI**)&base);
    AutoPtr<IURI> uri;
    base->ResolveEx(String("https:h"), (IURI**)&uri);
    String ret;
    uri->ToString(&ret);
    assert(ret.Equals(String("https:h")));

    base->ResolveEx(String("g"), (IURI**)&uri);
    uri->ToString(&ret);
    assert(ret.Equals(String("http://a/b/c/g")));

    base->ResolveEx(String("./g"), (IURI**)&uri);
    uri->ToString(&ret);
    assert(ret.Equals(String("http://a/b/c/g")));

    base->ResolveEx(String("g/"), (IURI**)&uri);
    uri->ToString(&ret);
    assert(ret.Equals(String("http://a/b/c/g/")));

    base->ResolveEx(String("/g"), (IURI**)&uri);
    uri->ToString(&ret);
    assert(ret.Equals(String("http://a/g")));

    base->ResolveEx(String("//g"), (IURI**)&uri);
    uri->ToString(&ret);
    assert(ret.Equals(String("http://g")));

    base->ResolveEx(String("?y"), (IURI**)&uri);
    uri->ToString(&ret);
    assert(ret.Equals(String("http://a/b/c/d;p?y")));

    base->ResolveEx(String("g?y"), (IURI**)&uri);
    uri->ToString(&ret);
    assert(ret.Equals(String("http://a/b/c/g?y")));

    base->ResolveEx(String("#s"), (IURI**)&uri);
    uri->ToString(&ret);
    assert(ret.Equals(String("http://a/b/c/d;p?q#s")));

    base->ResolveEx(String("g#s"), (IURI**)&uri);
    uri->ToString(&ret);
    assert(ret.Equals(String("http://a/b/c/g#s")));

    base->ResolveEx(String("g?y#s"), (IURI**)&uri);
    uri->ToString(&ret);
    assert(ret.Equals(String("http://a/b/c/g?y#s")));

    base->ResolveEx(String(";x"), (IURI**)&uri);
    uri->ToString(&ret);
    assert(ret.Equals(String("http://a/b/c/;x")));

    base->ResolveEx(String("g;x"), (IURI**)&uri);
    uri->ToString(&ret);
    assert(ret.Equals(String("http://a/b/c/g;x")));

    base->ResolveEx(String("g;x?y#s"), (IURI**)&uri);
    uri->ToString(&ret);
    assert(ret.Equals(String("http://a/b/c/g;x?y#s")));

    base->ResolveEx(String(""), (IURI**)&uri);
    uri->ToString(&ret);
    assert(ret.Equals(String("http://a/b/c/d;p?q")));

    base->ResolveEx(String("."), (IURI**)&uri);
    uri->ToString(&ret);
    assert(ret.Equals(String("http://a/b/c/")));

    base->ResolveEx(String("./"), (IURI**)&uri);
    uri->ToString(&ret);
    assert(ret.Equals(String("http://a/b/c/")));

    base->ResolveEx(String(".."), (IURI**)&uri);
    uri->ToString(&ret);
    assert(ret.Equals(String("http://a/b/")));

    base->ResolveEx(String("../"), (IURI**)&uri);
    uri->ToString(&ret);
    assert(ret.Equals(String("http://a/b/")));

    base->ResolveEx(String("../g"), (IURI**)&uri);
    uri->ToString(&ret);
    assert(ret.Equals(String("http://a/b/g")));

    base->ResolveEx(String("../.."), (IURI**)&uri);
    uri->ToString(&ret);
    assert(ret.Equals(String("http://a/")));

    base->ResolveEx(String("../../"), (IURI**)&uri);
    uri->ToString(&ret);
    assert(ret.Equals(String("http://a/")));

    base->ResolveEx(String("../../g"), (IURI**)&uri);
    uri->ToString(&ret);
    assert(ret.Equals(String("http://a/g")));
}

//Num: Test44
int CTest::testRfc1808AbnormalExampleTooManyDotDotSequences(int argc, char* argv[])
{
    String str0("http://a/b/c/d;p?q");
    AutoPtr<IURI> base;
    CURI::New(str0, (IURI**)&base);
    AutoPtr<IURI> uri;
    base->ResolveEx(String("../../../g"), (IURI**)&uri);
    String ret;
    uri->ToString(&ret);
    assert(ret.Equals(String("http://a/g")));

    base->ResolveEx(String("../../../../g"), (IURI**)&uri);
    uri->ToString(&ret);
    assert(ret.Equals(String("http://a/g")));
}

//Num: Test45
int CTest::testRfc1808AbnormalExampleRemoveDotSegments(int argc, char* argv[])
{
    String str0("http://a/b/c/d;p?q");
    AutoPtr<IURI> base;
    CURI::New(String("http://a/b/c/d;p?q"), (IURI**)&base);
    AutoPtr<IURI> uri;
    base->ResolveEx(String("/./g"), (IURI**)&uri);
    String ret;
    uri->ToString(&ret);
    assert(ret.Equals(String("http://a/g")));

    base->ResolveEx(String("/../g"), (IURI**)&uri);
    uri->ToString(&ret);
    assert(ret.Equals(String("http://a/g")));

    base->ResolveEx(String("g."), (IURI**)&uri);
    uri->ToString(&ret);
    assert(ret.Equals(String("http://a/b/c/g.")));

    base->ResolveEx(String("g.."), (IURI**)&uri);
    uri->ToString(&ret);
    assert(ret.Equals(String("http://a/b/c/g..")));

    base->ResolveEx(String("..g"), (IURI**)&uri);
    uri->ToString(&ret);
    assert(ret.Equals(String("http://a/b/c/..g")));
}

//Num: Test46
int CTest::testRfc1808AbnormalExampleNonsensicalDots(int argc, char* argv[])
{
    String str0("http://a/b/c/d;p?q");
    AutoPtr<IURI> base;
    CURI::New(str0, (IURI**)&base);
    AutoPtr<IURI> uri;
    base->ResolveEx(String("./../g"), (IURI**)&uri);
    String ret;
    uri->ToString(&ret);
    assert(ret.Equals(String("http://a/b/g")));

    base->ResolveEx(String("./g/."), (IURI**)&uri);
    uri->ToString(&ret);
    assert(ret.Equals(String("http://a/b/c/g/")));

    base->ResolveEx(String("g/./h"), (IURI**)&uri);
    uri->ToString(&ret);
    assert(ret.Equals(String("http://a/b/c/g/h")));

    base->ResolveEx(String("g/../h"), (IURI**)&uri);
    uri->ToString(&ret);
    assert(ret.Equals(String("http://a/b/c/h")));

    base->ResolveEx(String("g;x=1/./y"), (IURI**)&uri);
    uri->ToString(&ret);
    assert(ret.Equals(String("http://a/b/c/g;x=1/y")));

    base->ResolveEx(String("g;x=1/../y"), (IURI**)&uri);
    uri->ToString(&ret);
    assert(ret.Equals(String("http://a/b/c/y")));
}

//Num: Test47
int CTest::testRfc1808AbnormalExampleRelativeScheme(int argc, char* argv[])
{
    String str0("http://a/b/c/d;p?q");
    AutoPtr<IURI> base;
    CURI::New(str0, (IURI**)&base);
    AutoPtr<IURI> uri;
    base->ResolveEx(String("http:g"), (IURI**)&uri);
    String ret;
    uri->ToString(&ret);
    assert(ret.Equals(String("http:g")));

    Boolean flag = FALSE;
    uri->IsOpaque(&flag);
    assert(flag == TRUE);

    uri->IsAbsolute(&flag);
    assert(flag == TRUE);
}

//Num: Test48
int CTest::testRfc1808AbnormalExampleQueryOrFragmentDots(int argc, char* argv[])
{
    String str0("http://a/b/c/d;p?q");
    AutoPtr<IURI> base;
    CURI::New(str0, (IURI**)&base);
    AutoPtr<IURI> uri;
    base->ResolveEx(String("g?y/./x"), (IURI**)&uri);
    String ret;
    uri->ToString(&ret);
    Boolean flag = FALSE;
    assert(ret.Equals(String("http://a/b/c/g?y/./x")));

    base->ResolveEx(String("g?y/../x"), (IURI**)&uri);
    uri->ToString(&ret);
    assert(ret.Equals(String("http://a/b/c/g?y/../x")));

    base->ResolveEx(String("g#s/./x"), (IURI**)&uri);
    uri->ToString(&ret);
    assert(ret.Equals(String("http://a/b/c/g#s/./x")));

    base->ResolveEx(String("g#s/../x"), (IURI**)&uri);
    uri->ToString(&ret);
    assert(ret.Equals(String("http://a/b/c/g#s/../x")));
}

//Num: Test49
int CTest::testSquareBracketsInUserInfo(int argc, char* argv[])
{
    String str0("http://user:[::1]@host");
    AutoPtr<IURI> uri;
    ECode ec = CURI::New(str0, (IURI**)&uri);
    assert(ec == E_URI_SYNTAX_EXCEPTION);
}

//Num: Test50
int CTest::testFileUriExtraLeadingSlashes(int argc, char* argv[])
{
    String str0("file:////foo");
    AutoPtr<IURI> uri;
    CURI::New(str0, (IURI**)&uri);
    String ret;
    uri->GetAuthority(&ret);
    Boolean flag = FALSE;
    assert(ret.Equals(String(NULL)));

    uri->GetPath(&ret);
    assert(ret.Equals(String("//foo")));

    uri->ToString(&ret);
    assert(ret.Equals(String("file:////foo")));
}

//Num: Test51
int CTest::testFileUrlWithAuthority(int argc, char* argv[])
{
    String str0("file://x/foo");
    AutoPtr<IURI> uri;
    CURI::New(str0, (IURI**)&uri);
    String ret;
    uri->GetAuthority(&ret);
    assert(ret.Equals(String("x")));

    uri->GetPath(&ret);
    assert(ret.Equals(String("/foo")));

    uri->ToString(&ret);
    assert(ret.Equals(String("file://x/foo")));
}

//Num: Test52
int CTest::testEmptyAuthority(int argc, char* argv[])
{
    String str0("http:///foo");
    AutoPtr<IURI> uri;
    CURI::New(str0, (IURI**)&uri);
    String ret;
    uri->GetAuthority(&ret);
    assert(ret.Equals(String(NULL)));

    uri->GetPath(&ret);
    assert(ret.Equals(String("/foo")));

    uri->ToString(&ret);
    assert(ret.Equals(String("http:///foo")));
}

//Num: Test53
int CTest::testHttpUrlExtraLeadingSlashes(int argc, char* argv[])
{
    String str0("http:////foo");
    AutoPtr<IURI> uri;
    CURI::New(str0, (IURI**)&uri);
    String ret;
    uri->GetAuthority(&ret);
    assert(ret.Equals(String(NULL)));

    uri->GetPath(&ret);
    assert(ret.Equals(String("//foo")));

    uri->ToString(&ret);
    assert(ret.Equals(String("http:////foo")));
}

//Num: Test54
int CTest::testFileUrlRelativePath(int argc, char* argv[])
{
    String str0("file:/a/b/c");
    AutoPtr<IURI> base;
    CURI::New(str0, (IURI**)&base);
    AutoPtr<IURI> uri;
    base->ResolveEx(String("d"), (IURI**)&uri);
    String ret;
    uri->ToString(&ret);
    assert(ret.Equals(String("file:/a/b/d")));
}

//Num: Test55
int CTest::testFileUrlDottedPath(int argc, char* argv[])
{
    String str0("file:../a/b");
    AutoPtr<IURI> url;
    CURI::New(str0, (IURI**)&url);
    Boolean flag = FALSE;
    url->IsOpaque(&flag);
    assert(flag == TRUE);

    String ret;
    url->GetPath(&ret);
    assert(ret.Equals(String(NULL)));
}

//Num: Test56
int CTest::testParsingDotAsHostname(int argc, char* argv[])
{
    String str0("http://./");
    AutoPtr<IURI> uri;
    CURI::New(str0, (IURI**)&uri);
    String ret;
    uri->GetHost(&ret);
    assert(ret.Equals(String(NULL)));
}

//Num: Test57
int CTest::testSquareBracketsWithIPv4(int argc, char* argv[])
{
    String str0("http://[192.168.0.1]/");
    AutoPtr<IURI> uri;
    ECode ec = CURI::New(str0, (IURI**)&uri);
    assert(ec == E_URI_SYNTAX_EXCEPTION);
}

//Num: Test58
int CTest::testSquareBracketsWithHostname(int argc, char* argv[])
{
    String str0("http://[google.com]/");
    AutoPtr<IURI> uri;
    ECode ec = CURI::New(str0, (IURI**)&uri);
    assert(ec == E_URI_SYNTAX_EXCEPTION);
}

//Num: Test59
int CTest::testIPv6WithoutSquareBrackets(int argc, char* argv[])
{
    String str0("http://fe80::1234/");
    AutoPtr<IURI> uri;
    CURI::New(str0, (IURI**)&uri);
    String ret;
    uri->GetHost(&ret);
    assert(ret.Equals(String(NULL)));
}

//Num: Test60
int CTest::testEqualityWithNoPath(int argc, char* argv[])
{
    String str0("http://android.com/");
    AutoPtr<IURI> uri;
    CURI::New(str0, (IURI**)&uri);
    AutoPtr<IURI> anotherUri;
    String str1("http://android.com");
    CURI::New(str1, (IURI**)&anotherUri);
    Boolean flag = FALSE;
    uri->Equals(anotherUri, &flag);
    assert(flag == FALSE);
}

//Num: Test61
int CTest::testRelativize(int argc, char* argv[])
{
    String str0("http://host/a/b");
    AutoPtr<IURI> a;
    CURI::New(str0, (IURI**)&a);

    AutoPtr<IURI> b;
    String str1("http://host/a/b/c");
    CURI::New(str1, (IURI**)&b);
    AutoPtr<IURI> uri;
    a->Relativize(b, (IURI**)&uri);
    String ret;
    uri->ToString(&ret);
    assert(ret.Equals(String("b/c")));
}

//Num: Test62
int CTest::testParseServerAuthorityInvalidAuthority(int argc, char* argv[])
{
    String str0("http://host:-2/");
    AutoPtr<IURI> uri;
    CURI::New(str0, (IURI**)&uri);
    String ret;
    uri->GetAuthority(&ret);
    assert(ret.Equals(String("host:-2")));

    uri->GetHost(&ret);
    assert(ret.Equals(String(NULL)));

    Int32 port;
    uri->GetPort(&port);
    assert(-1 == port);
}

//Num: Test63
int CTest::testParseServerAuthorityOmittedAuthority(int argc, char* argv[])
{
    String str0("http:file");
    AutoPtr<IURI> uri;
    CURI::New(str0, (IURI**)&uri);
    AutoPtr<IURI> anotherUri;
    uri->ParseServerAuthority((IURI**)&anotherUri);

    String ret;
    uri->GetAuthority(&ret);
    assert(ret.Equals(String(NULL)));

    uri->GetHost(&ret);
    assert(ret.Equals(String(NULL)));

    Int32 port;
    uri->GetPort(&port);
    assert(-1 == port);
}

//Num: Test64
int CTest::testEncodingParts(int argc, char* argv[])
{
    String str0("http");
    String str1("user:pa55w?rd");
    String str2("host");
    String str3("/doc|search");
    String str4("q=green robots");
    String str5("over 6\"");
    AutoPtr<IURI> uri;
    CURI::New(str0, str1, str2, 80, str3, str4, str5, (IURI**)&uri);
    String ret;
    uri->GetScheme(&ret);
    assert(ret.Equals(str0));

    uri->GetAuthority(&ret);
    assert(ret.Equals(String("user:pa55w?rd@host:80")));

    uri->GetRawAuthority(&ret);
    assert(ret.Equals(String("user:pa55w%3Frd@host:80")));

    uri->GetUserInfo(&ret);
    assert(ret.Equals(str1));

    uri->GetRawUserInfo(&ret);
    assert(ret.Equals(String("user:pa55w%3Frd")));

    uri->GetPath(&ret);
    assert(ret.Equals(str3));

    uri->GetRawPath(&ret);
    assert(ret.Equals(String("/doc%7Csearch")));

    uri->GetQuery(&ret);
    assert(ret.Equals(str4));

    uri->GetRawQuery(&ret);
    assert(ret.Equals(String("q=green%20robots")));

    uri->GetFragment(&ret);
    assert(ret.Equals(str5));

    uri->GetRawFragment(&ret);
    assert(ret.Equals("over%206%22"));

    uri->GetSchemeSpecificPart(&ret);
    assert(ret.Equals(String("//user:pa55w?rd@host:80/doc|search?q=green robots")));

    uri->GetRawSchemeSpecificPart(&ret);
    assert(ret.Equals(String("//user:pa55w%3Frd@host:80/doc%7Csearch?q=green%20robots")));

    uri->ToString(&ret);
    assert(ret.Equals(String("http://user:pa55w%3Frd@host:80/doc%7Csearch?q=green%20robots#over%206%22")));
}

//Num: Test65
int CTest::testSchemeCaseIsNotCanonicalized(int argc, char* argv[])
{
    String str0("HTTP://host/path");
    AutoPtr<IURI> uri;
    CURI::New(str0, (IURI**)&uri);
    String ret;
    uri->GetScheme(&ret);
    assert(ret.Equals(String("HTTP")));
}

//Num: Test66
int CTest::testEmptyAuthorityWithPath(int argc, char* argv[])
{
    String str0("http:///path");
    AutoPtr<IURI> uri;
    CURI::New(str0, (IURI**)&uri);
    String ret;
    uri->GetAuthority(&ret);
    assert(ret.Equals(String(NULL)));

    uri->GetPath(&ret);
    assert(ret.Equals(String("/path")));
}

//Num: Test67
int CTest::testEmptyAuthorityWithQuery(int argc, char* argv[])
{
    String str0("http://?query");
    AutoPtr<IURI> uri;
    CURI::New(str0, (IURI**)&uri);
    String ret;
    uri->GetAuthority(&ret);
    Boolean flag = FALSE;
    assert(ret.Equals(String(NULL)));

    uri->GetPath(&ret);
    assert(ret.Equals(String("")));

    uri->GetQuery(&ret);
    assert(ret.Equals(String("query")));
}

//Num: Test68
int CTest::testEmptyAuthorityWithFragment(int argc, char* argv[])
{
    String str0("http://#fragment");
    AutoPtr<IURI> uri;
    CURI::New(str0, (IURI**)&uri);
    String ret;
    uri->GetAuthority(&ret);
    Boolean flag = FALSE;
    assert(ret.Equals(String(NULL)));

    uri->GetPath(&ret);
    assert(ret.Equals(String("")));

    uri->GetFragment(&ret);
    assert(ret.Equals(String("fragment")));
}

//Num: Test69
int CTest::testEncodingConstructorsRefuseRelativePath(int argc, char* argv[])
{
    String str0("http");
    String str1("host");
    String str2("relative");
    AutoPtr<IURI> uri;
    ECode ec;
    ec = CURI::New(str0, str1, str2, String(NULL), (IURI**)&uri);
    assert(ec == E_URI_SYNTAX_EXCEPTION);

    ec = CURI::New(str0, str1, str2, String(NULL), String(NULL), (IURI**)&uri);
    assert(ec == E_URI_SYNTAX_EXCEPTION);

    ec = CURI::New(str0, String(NULL), str1, -1, str2, String(NULL), String(NULL), (IURI**)&uri);
    assert(ec == E_URI_SYNTAX_EXCEPTION);
}

//Num: Test70
int CTest::testEncodingConstructorsAcceptEmptyPath(int argc, char* argv[])
{
    String str0("http");
    String str1("host");
    String str2("");
    AutoPtr<IURI> uri;
    CURI::New(str0, str1, str2, String(NULL), (IURI**)&uri);
    String ret;
    uri->GetPath(&ret);
    assert(ret.Equals(str2));

    CURI::New(str0, str1, str2, String(NULL), String(NULL), (IURI**)&uri);
    uri->GetPath(&ret);
    assert(ret.Equals(str2));

    CURI::New(str0, String(NULL), str1, -1, str2, String(NULL), String(NULL), (IURI**)&uri);
    uri->GetPath(&ret);
    assert(ret.Equals(str2));
}

//Num: Test71
int CTest::testResolveRelativeAndAbsolute(int argc, char* argv[])
{
    String str0("http://android.com/");
    AutoPtr<IURI> absolute;
    CURI::New(str0, (IURI**)&absolute);
    String str1("robots.txt");
    AutoPtr<IURI> relative;
    CURI::New(str1, (IURI**)&relative);

    AutoPtr<IURI> outURI;
    absolute->Resolve(absolute, (IURI**)&outURI);
    Boolean flag = FALSE;
    outURI->Equals(absolute, &flag);
    assert(flag == TRUE);

    String str2("http://android.com/robots.txt");
    AutoPtr<IURI> uri;
    CURI::New(str2, (IURI**)&uri);
    absolute->Resolve(relative, (IURI**)&outURI);
    uri->Equals(outURI, &flag);
    assert(flag == TRUE);

    relative->Resolve(absolute, (IURI**)&outURI);
    absolute->Equals(outURI, &flag);
    assert(flag == TRUE);

    relative->Resolve(relative, (IURI**)&outURI);
    relative->Equals(outURI, &flag);
    assert(flag == TRUE);
}

//Num: Test72
int CTest::testRelativizeRelativeAndAbsolute(int argc, char* argv[])
{
    String str0("http://android.com/");
    AutoPtr<IURI> absolute;
    CURI::New(str0, (IURI**)&absolute);
    String str1("robots.txt");
    AutoPtr<IURI> relative;
    CURI::New(str1, (IURI**)&relative);

    String str2("http://android.com/robots.txt");
    AutoPtr<IURI> uri;
    CURI::New(str2, (IURI**)&uri);
    AutoPtr<IURI> outURI;
    absolute->Relativize(uri, (IURI**)&outURI);
    Boolean flag = FALSE;
    relative->Equals(outURI, &flag);
    assert(flag == TRUE);

    absolute->Relativize(absolute, (IURI**)&outURI);
    String str3("");
    AutoPtr<IURI> uri1;
    CURI::New(str3, (IURI**)&uri1);
    uri1->Equals(outURI, &flag);
    assert(flag == TRUE);

    absolute->Relativize(relative, (IURI**)&outURI);
    relative->Equals(outURI, &flag);
    assert(flag == TRUE);

    relative->Relativize(absolute, (IURI**)&outURI);
    absolute->Equals(outURI, &flag);
    assert(flag == TRUE);

    relative->Relativize(relative, (IURI**)&outURI);
    uri1->Equals(outURI, &flag);
    assert(flag == TRUE);
}

//Num: Test73
int CTest::testPartContainsSpace(int argc, char* argv[])
{
    String str0("ht tp://host/");
    AutoPtr<IURI> uri;
    ECode ec = CURI::New(str0, (IURI**)&uri);
    assert(ec == E_URI_SYNTAX_EXCEPTION);

    String str1("http://user name@host/");
    ec = CURI::New(str1, (IURI**)&uri);
    assert(ec == E_URI_SYNTAX_EXCEPTION);

    String str2("http://ho st/");
    ec = CURI::New(str2, (IURI**)&uri);
    assert(ec == E_URI_SYNTAX_EXCEPTION);

    String str3("http://host:80 80/");
    ec = CURI::New(str3, (IURI**)&uri);
    assert(ec == E_URI_SYNTAX_EXCEPTION);

    String str4("http://host/fi le");
    ec = CURI::New(str4, (IURI**)&uri);
    assert(ec == E_URI_SYNTAX_EXCEPTION);

    String str5("http://host/file?que ry");
    ec = CURI::New(str5, (IURI**)&uri);
    assert(ec == E_URI_SYNTAX_EXCEPTION);

    String str6("http://host/file?query#re f");
    ec = CURI::New(str6, (IURI**)&uri);
    assert(ec == E_URI_SYNTAX_EXCEPTION);
}

//Num: Test74
int CTest::testUnderscore(int argc, char* argv[])
{
    String str0("http://a_b.c.d.net/");
    AutoPtr<IURI> uri;
    CURI::New(str0, (IURI**)&uri);
    String ret;
    uri->GetAuthority(&ret);
    Boolean flag = FALSE;
    assert(ret.Equals(String("a_b.c.d.net")));

    uri->GetHost(&ret);
    assert(ret.Equals(String(NULL)));
}