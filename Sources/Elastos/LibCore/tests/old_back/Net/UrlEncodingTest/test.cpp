#include "test.h"

CTest::CTest()
{
    CURLEncoder::AcquireSingleton((IURLEncoder**)&mUecode);
    CURLDecoder::AcquireSingleton((IURLDecoder**)&mDecode);
}

CTest::~CTest()
{
}

int CTest::testUriRetainsOriginalEncoding(int argc, char* argv[])
{
    PEL("CTest::testUriRetainsOriginalEncoding")
    AutoPtr<IURI> res;
    CURI::New(String("http://foo#%61"), (IURI**)&res);
    String str;
    res->GetRawFragment(&str);
    assert(String("%61") == str);
    return 0;
}

int CTest::testDecodingPlus(int argc, char* argv[])
{
    PEL("CTest::testDecodingPlus")
    String outstr;
    mDecode->Decode(String("a+b"), &outstr);
    assert(String("a b") == outstr);
    mDecode->DecodeEx(String("a+b"), String("UTF-8"), &outstr);
    assert(String("a b") == outstr);
    AutoPtr<IURI> res;
    CURI::New(String("http://foo#a+b"), (IURI**)&res);
    String str;
    res->GetFragment(&str);
    assert(String("a+b") == str);
    return 0;
}

int CTest::testEncodingPlus(int argc, char* argv[])
{
    PEL("CTest::testEncodingPlus")
    String outstr;
    mUecode->Encode(String("a+b"), &outstr);
    assert(String("a%2Bb") == outstr);
    mUecode->EncodeEx(String("a+b"), String("UTF-8"), &outstr);
    assert(String("a%2Bb") == outstr);
    AutoPtr<IURI> res;
    CURI::New(String("http"), String("foo"), String("/"), String("a+b"), (IURI**)&res);
    String str;
    res->GetRawFragment(&str);
    assert(String("a+b") == str);
    return 0;
}

int CTest::testDecodingSpace(int argc, char* argv[])
{
    PEL("CTest::testDecodingSpace")
    String outstr;
    mDecode->Decode(String("a b"), &outstr);
    assert(String("a b") == outstr);
    mDecode->DecodeEx(String("a b"), String("UTF-8"), &outstr);
    assert(String("a b") == outstr);
    // try {
    AutoPtr<IURI> res;
    ECode ec = CURI::New(String("http://foo#a b"), (IURI**)&res);
    if (ec != NOERROR) {
        printf("fail expected~~~~~~~~~~~~~\n");
    }
    // } catch (URISyntaxException expected) {
    // }
    return 0;
}

int CTest::testEncodingSpace(int argc, char* argv[])
{
    PEL("CTest::testEncodingSpace")
    String outstr;
    mUecode->Encode(String("a b"), &outstr);
    assert(String("a+b") == outstr);
    mUecode->EncodeEx(String("a b"), String("UTF-8"), &outstr);
    assert(String("a+b") == outstr);
    AutoPtr<IURI> res;
    CURI::New(String("http"), String("foo"), String("/"), String("a b"), (IURI**)&res);
    String str;
    res->GetRawFragment(&str);
    assert(String("a%20b") == str);
    return 0;
}

int CTest::testUriDecodingPartial(int argc, char* argv[])
{
    PEL("CTest::testUriDecodingPartial")
    // try {
    AutoPtr<IURI> res;
    ECode ec = CURI::New(String("http://foo#%"), (IURI**)&res);
    if (ec != NOERROR) {
        printf("fail expected~~~~~~~~~~~~~\n");
    }
    // } catch (URISyntaxException expected) {
    // }
    // try {
    ec = CURI::New(String("http://foo#%0"), (IURI**)&res);
    if (ec != NOERROR) {
        printf("fail expected~~~~~~~~~~~~~\n");
    }
    // } catch (URISyntaxException expected) {
    // }
    return 0;
}

int CTest::testUrlDecoderDecodingPartial(int argc, char* argv[])
{
    PEL("CTest::testUrlDecoderDecodingPartial")
    // try {
    String outstr;
    ECode ec = mDecode->Decode(String("%"), &outstr);
    if (ec != NOERROR) {
        /* code */
        printf("fail expected~~~~~~~~~~~~\n");
    }
    // } catch (IllegalArgumentException expected) {
    // }
    // try {
    ec = mDecode->Decode(String("%0"), &outstr);
    if (ec != NOERROR) {
        printf("fail expected~~~~~~~~~~~~\n");
    }
    // } catch (IllegalArgumentException expected) {
    // }
    return 0;
}

int CTest::testUriDecodingInvalid(int argc, char* argv[])
{
    PEL("CTest::testUriDecodingInvalid")
    // try {
    AutoPtr<IURI> res;
    ECode ec = CURI::New(String("http://foo#%0g"), (IURI**)&res);
    if (ec != NOERROR) {
        printf("fail expected~~~~~~~~~~~\n");
    }

    // } catch (URISyntaxException expected) {
    // }
    return 0;
}

int CTest::testUrlDecoderDecodingInvalid(int argc, char* argv[])
{
    PEL("CTest::testUrlDecoderDecodingInvalid")
    // try {
    String outstr;
    ECode ec = mDecode->Decode(String("%0g"), &outstr);
    if (ec != NOERROR) {
        printf("fail expected~~~~~~~~~~~\n");
    }
    //     fail();
    // } catch (IllegalArgumentException expected) {
    // }
    return 0;
}

int CTest::testUrlDecoderFailsOnNullCharset(int argc, char* argv[])
{
    PEL("CTest::testUrlDecoderFailsOnNullCharset")
    // try {
    String outstr;
    ECode ec = mDecode->Decode(String("ab"), &outstr);
    if (ec != NOERROR) {
        printf("fail expected~~~~~~~~~~~\n");
    }
    //     fail();
    // } catch (IllegalCharsetNameException expected) {
    // } catch (NullPointerException expected) {
    // }
    return 0;
}

int CTest::testUrlDecoderFailsOnEmptyCharset(int argc, char* argv[])
{
    PEL("CTest::testUrlDecoderFailsOnEmptyCharset")
    // try {
    String outstr;
    ECode ec = mDecode->DecodeEx(String("ab"), String(""), &outstr);
    if (ec != NOERROR) {
        printf("fail expected~~~~~~~~~~~\n");
    }
    //     fail();
    // } catch (IllegalCharsetNameException expected) {
    // } catch (UnsupportedEncodingException expected) {
    // }
    return 0;
}

int CTest::testUrlEncoderFailsOnNullCharset(int argc, char* argv[])
{
    PEL("CTest::testUrlEncoderFailsOnNullCharset")
    // try {
    String outstr;
    ECode ec = mUecode->EncodeEx(String("ab"), String(NULL), &outstr);
    if (ec != NOERROR) {
        printf("fail expected~~~~~~~~~~~\n");
    }
    //     fail();
    // } catch (IllegalCharsetNameException expected) {
    // } catch (NullPointerException expected) {
    // }
    return 0;
}

int CTest::testUrlEncoderFailsOnEmptyCharset(int argc, char* argv[])
{
    PEL("CTest::testUrlEncoderFailsOnEmptyCharset")
    // try {
    //     URLEncoder.encode("ab", "");
    String outstr;
    ECode ec = mUecode->EncodeEx(String("ab"), String(""), &outstr);
    if (ec != NOERROR) {
        printf("fail expected~~~~~~~~~~~\n");
    }
    //     fail();
    // } catch (IllegalCharsetNameException expected) {
    // } catch (UnsupportedEncodingException expected) {
    // }
    return 0;
}

int CTest::testUrlDecoderIgnoresUnnecessaryCharset(int argc, char* argv[])
{
    PEL("CTest::testUrlDecoderIgnoresUnnecessaryCharset")
    // try {
    String outstr;
    mDecode->DecodeEx(String("ab"), String("no-such-charset"), &outstr);
    assert(String("ab") == outstr);
    //     // no fail()
    // } catch (UnsupportedCharsetException expected) {
    // }
    return 0;
}

int CTest::testUrlEncoderFailsOnInvalidCharset(int argc, char* argv[])
{
    PEL("CTest::testUrlEncoderFailsOnInvalidCharset")
    // try {
    String outstr;
    ECode ec = mUecode->EncodeEx(String("ab"), String("no-such-charset"), &outstr);
    if (ec != NOERROR) {
        printf("fail expected~~~~~~~~~~~\n");
    }
    //     fail();
    // } catch (UnsupportedCharsetException expected) {
    // } catch (UnsupportedEncodingException expected) {
    // }
    return 0;
}

int CTest::testDecoding(int argc, char* argv[])
{
    PEL("CTest::testDecoding")
    AutoPtr< ArrayOf<Char32> > char1 = ArrayOf<Char32>::Alloc(1);
    (*char1)[0] = 0x0000;
    assertDecoded(String("a") + String(*char1) + String("b"), String("a%00b"));
    assertDecoded(String("a b"), String("a%20b"));
    assertDecoded(String("a+b"), String("a%2bb"));
    assertDecoded(String("a%b"), String("a%25b"));
    (*char1)[0] = 0x007f;
    assertDecoded(String("a") + String(*char1) + String("b"), String("a%7fb"));
    return 0;
}

int CTest::testEncoding(int argc, char* argv[])
{
    PEL("CTest::testEncoding")
    assertEncoded(String("a%25b"), String("a%b"));
    assertEncoded(String("a%7Fb"), String("a\u007fb"));
    return 0;
}

int CTest::testDecodingLiterals(int argc, char* argv[])
{
    PEL("CTest::testDecodingLiterals")
    AutoPtr< ArrayOf<Char32> > char2 = ArrayOf<Char32>::Alloc(2);
    (*char2)[0] = 0xd842;
    (*char2)[1] = 0xdf9f;
    assertDecoded(String(*char2), String(*char2));
    return 0;
}

int CTest::testDecodingBrokenUtf8SequenceYieldsReplacementCharacter(int argc, char* argv[])
{
    PEL("CTest::testDecodingBrokenUtf8SequenceYieldsReplacementCharacter")
    assertDecoded(String("a\ufffdb"), String("a%ffb"));
    return 0;
}

int CTest::testDecodingUtf8Octets(int argc, char* argv[])
{
    PEL("CTest::testDecodingUtf8Octets")
    assertDecoded(String("\u20AC"), String("%e2%82%ac"));
    AutoPtr< ArrayOf<Char32> > char2 = ArrayOf<Char32>::Alloc(2);
    (*char2)[0] = 0xd842;
    (*char2)[1] = 0xdf9f;
    assertDecoded(String(*char2), String("%f0%a0%ae%9f"));
    return 0;
}

int CTest::testDecodingNonUsDigits(int argc, char* argv[])
{
    PEL("CTest::testDecodingNonUsDigits")
    // try {
    AutoPtr<IURI> res;
    AutoPtr< ArrayOf<Char32> > char2 = ArrayOf<Char32>::Alloc(2);
    (*char2)[0] = 0x0664;
    (*char2)[1] = 0x0661;

    ECode ec = CURI::New(String("http://foo#") + String("%") + String(*char2), (IURI**)&res);
    if (ec != NOERROR) {
        printf("fail~~~~~~\n");
    }
    //     fail();
    // } catch (URISyntaxException expected) {
    // }
    // try {
    String outstr;
    ec = mDecode->Decode(String("%\u0664\u0661"), &outstr);
    if (ec != NOERROR) {
        printf("fails expected~~~~~~~~~\n");
    }
    //     fail(); // RI fails this test returning "A"
    // } catch (IllegalArgumentException expected) {
    // }
    return 0;
}

int CTest::testUrlEncoderEncodesNonPrintableNonAsciiCharacters(int argc, char* argv[])
{
    PEL("CTest::testUrlEncoderEncodesNonPrintableNonAsciiCharacters")
    String outstr;
    mUecode->EncodeEx(String("\u0000"), String("UTF-8"), &outstr);
    assert(String("%00") == outstr);
    mUecode->Encode(String("\u0000"), &outstr);
    assert(String("%00") == outstr);
    mUecode->EncodeEx(String("\u20AC"), String("UTF-8"), &outstr);
    assert(String("%E2%82%AC") == outstr);
    mUecode->Encode(String("\u20AC"), &outstr);
    assert(String("%E2%82%AC") == outstr);

    AutoPtr< ArrayOf<Char32> > char2 = ArrayOf<Char32>::Alloc(2);
    (*char2)[0] = 0xd842;
    (*char2)[1] = 0xdf9f;
    mUecode->EncodeEx(String(*char2), String("UTF-8"), &outstr);
    assert(String("%F0%A0%AE%9F") == outstr);
    mUecode->Encode(String(*char2), &outstr);
    assert(String("%F0%A0%AE%9F") == outstr);
    return 0;
}

int CTest::testUriDoesNotEncodeNonPrintableNonAsciiCharacters(int argc, char* argv[])
{
    PEL("CTest::testUriDoesNotEncodeNonPrintableNonAsciiCharacters")
    AutoPtr<IURI> urlarr;
    CURI::New(String("http"), String("foo"), String("/"), String("\u20AC"), (IURI**)&urlarr);
    String outstr;
    urlarr->GetRawFragment(&outstr);
    assert(String("\u20AC") == outstr);
    AutoPtr< ArrayOf<Char32> > char2 = ArrayOf<Char32>::Alloc(2);
    (*char2)[0] = 0xd842;
    (*char2)[1] = 0xdf9f;
    urlarr = NULL;
    CURI::New(String("http"), String("foo"), String("/"), String(*char2), (IURI**)&urlarr);
    urlarr->GetRawFragment(&outstr);
    assert(String(*char2) == outstr);
    return 0;
}

int CTest::testUriEncodesControlCharacters(int argc, char* argv[])
{
    PEL("CTest::testUriEncodesControlCharacters")
    AutoPtr<IURI> urlarr;
    CURI::New(String("http"), String("foo"), String("/"), String("\u0001"), (IURI**)&urlarr);
    String outstr;
    urlarr->GetRawFragment(&outstr);
    assert(String("%01") == outstr);

    // The RI fails this, encoding \u0001 but not \u0000
    CURI::New(String("http"), String("foo"), String("/"), String("\u0000"), (IURI**)&urlarr);
    urlarr->GetRawFragment(&outstr);
    assert(String("%00") == outstr);
    return 0;
}

int CTest::testEncodeAndDecode(int argc, char* argv[])
{
    PEL("CTest::testEncodeAndDecode")
    assertRoundTrip(String("http://jcltest.apache.org/test?hl=en&q=te st"),
            String("http%3A%2F%2Fjcltest.apache.org%2Ftest%3Fhl%3Den%26q%3Dte+st"));
    assertRoundTrip(String("file://a b/c/d.e-f*g_ l"),
            String("file%3A%2F%2Fa+b%2Fc%2Fd.e-f*g_+l"));
    assertRoundTrip(String("jar:file://a.jar !/b.c/\u1052"),
            String("jar%3Afile%3A%2F%2Fa.jar+%21%2Fb.c%2F%E1%81%92"));
    assertRoundTrip(String("ftp://test:pwd@localhost:2121/%D0%9C"),
            String("ftp%3A%2F%2Ftest%3Apwd%40localhost%3A2121%2F%25D0%259C"));
    return 0;
}

void CTest::assertDecoded(String decoded, String original)
{
    PEL("CTest::assertDecoded")
    AutoPtr<IURI> urlarr;
    CURI::New(String("http://foo#") + original, (IURI**)&urlarr);
    String outstr;
    urlarr->GetFragment(&outstr);
PFL_EX("%s == %s", decoded.string(), outstr.string())
    assert(decoded == outstr);
    mDecode->Decode(original, &outstr);
PFL_EX("%s == %s", decoded.string(), outstr.string())
    assert(decoded == outstr);
    mDecode->DecodeEx(original, String("UTF-8"), &outstr);
PFL_EX("%s == %s", decoded.string(), outstr.string())
    assert(decoded == outstr);
}

void CTest::assertEncoded(String encoded, String original)
{
    PEL("CTest::assertEncoded")
    String outstr;
    mUecode->EncodeEx(original, String("UTF-8"), &outstr);
    assert(encoded == outstr);
    mUecode->Encode(original, &outstr);
    assert(encoded == outstr);
    AutoPtr<IURI> urlarr;
    CURI::New(String("http"), String("foo"), String("/"), original, (IURI**)&urlarr);
    urlarr->GetRawFragment(&outstr);
    assert(encoded == outstr);
}

void CTest::assertRoundTrip(String original, String encoded)
{
    PEL("CTest::assertRoundTrip")
    String outstr;
    mUecode->EncodeEx(original, String("UTF-8"), &outstr);
    assert(encoded == outstr);
    mDecode->DecodeEx(encoded, String("UTF-8"), &outstr);
    assert(original == outstr);
}
