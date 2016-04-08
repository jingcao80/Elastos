#ifndef __PATTTERN_TEST_H
#define __PATTTERN_TEST_H


#include <elastos.h>
using namespace Elastos;

#define TEST(a, x) a.test_##x

class CTest {
public:
    CTest(){};
    ~CTest(){};

public:
    int test_2DigitYearStartIsCloned(int argc, char* argv[]);
    int test_StandAloneNames(int argc, char* argv[]);
    int test2038(int argc, char* argv[]);
    int testParsingUncommonTimeZoneAbbreviations(int argc, char* argv[]);
    int testFormattingUncommonTimeZoneAbbreviations(int argc, char* argv[]);
    int testTimeZoneFormatting(int argc, char* argv[]);
    int testObsoleteDstZoneName(int argc, char* argv[]);
    int testDstZoneNameWithNonDstTimestamp(int argc, char* argv[]);
    int testNonDstZoneNameWithDstTimestamp(int argc, char* argv[]);
    int testDstZoneWithNonDstTimestampForNonHourDstZone(int argc, char* argv[]);
    int testNonDstZoneWithDstTimestampForNonHourDstZone(int argc, char* argv[]);
    int testLocales(int argc, char* argv[]);
    int testParseTimezoneOnly(int argc, char* argv[]);
    int testParseArabic(int argc, char* argv[]);


private:
};


#endif //__PATTTERN_TEST_H
