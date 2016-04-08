#ifndef __INETSOCKETADDRESSTEST_H__
#define __INETSOCKETADDRESSTEST_H__

#include "Elastos.CoreLibrary.h"
#include <elastos.h>
#include <elautoptr.h>
#include <elquintet.h>
#include <elastos/StringUtils.h>

using namespace Elastos;
using namespace Elastos::Core;
using namespace Elastos::Utility;
using namespace Elastos::Net;

#define TEST(a, x) a.test_##x

class CTest
{
public:
    CTest();
    ~CTest();

public:
    int testUriRetainsOriginalEncoding(int argc, char* argv[]);
    int testDecodingPlus(int argc, char* argv[]);
    int testEncodingPlus(int argc, char* argv[]);
    int testDecodingSpace(int argc, char* argv[]);
    int testEncodingSpace(int argc, char* argv[]);
    int testUriDecodingPartial(int argc, char* argv[]);
    int testUrlDecoderDecodingPartial(int argc, char* argv[]);
    int testUriDecodingInvalid(int argc, char* argv[]);
    int testUrlDecoderDecodingInvalid(int argc, char* argv[]);
    int testUrlDecoderFailsOnNullCharset(int argc, char* argv[]);
    int testUrlDecoderFailsOnEmptyCharset(int argc, char* argv[]);
    int testUrlEncoderFailsOnNullCharset(int argc, char* argv[]);
    int testUrlEncoderFailsOnEmptyCharset(int argc, char* argv[]);
    int testUrlDecoderIgnoresUnnecessaryCharset(int argc, char* argv[]);
    int testUrlEncoderFailsOnInvalidCharset(int argc, char* argv[]);
    int testDecoding(int argc, char* argv[]);
    int testEncoding(int argc, char* argv[]);
    int testDecodingLiterals(int argc, char* argv[]);
    int testDecodingBrokenUtf8SequenceYieldsReplacementCharacter(int argc, char* argv[]);
    int testDecodingUtf8Octets(int argc, char* argv[]);
    int testDecodingNonUsDigits(int argc, char* argv[]);
    int testUrlEncoderEncodesNonPrintableNonAsciiCharacters(int argc, char* argv[]);
    int testUriDoesNotEncodeNonPrintableNonAsciiCharacters(int argc, char* argv[]);
    int testUriEncodesControlCharacters(int argc, char* argv[]);
    int testEncodeAndDecode(int argc, char* argv[]);

private:
    void assertDecoded(String decoded, String original);
    void assertEncoded(String encoded, String original);
    void assertRoundTrip(String original, String encoded);

private:
    AutoPtr<IURLEncoder> mUecode;
    AutoPtr<IURLDecoder> mDecode;
};

#endif // __INETSOCKETADDRESSTEST_H__