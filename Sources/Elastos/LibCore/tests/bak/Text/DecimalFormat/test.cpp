#include "test.h"

#include "Elastos.CoreLibrary.h"
#include <elautoptr.h>
#include <elastos/StringBuffer.h>

using namespace Elastos;
using namespace Elastos::Text;
using namespace Libcore::ICU;
using namespace Elastos::Math;
using namespace Elastos::Core;

int CTest::test_exponentSeparator(int argc, char* argv[])
{
    PFL
    AutoPtr<IDecimalFormat> df;
    CDecimalFormat::New(String("0E0"), (IDecimalFormat**)&df);
    String str;
    df->FormatDouble(12345., &str);
    PFL_EX("str: %s " , str.string())
    assert(str.Equals("1E4"));
    AutoPtr<IDecimalFormatSymbols> dfs;
    df->GetDecimalFormatSymbols((IDecimalFormatSymbols**)&dfs);
    dfs->SetExponentSeparator(String("-useless-api-"));
    df->SetDecimalFormatSymbols(dfs);
    df->FormatDouble(12345., &str);
    PFL_EX("str: %s " , str.string())
    assert(str.Equals("1-useless-api-4"));
    return 0;
}

int CTest::test_setMaximumFractionDigitsAffectsRoundingMode(int argc, char* argv[])
{

    AutoPtr<IDecimalFormat> df;
    AutoPtr<INumberFormatHelper> numhelper;
    CNumberFormatHelper::AcquireSingleton((INumberFormatHelper**)&numhelper);
    AutoPtr<ILocaleHelper> lochelper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&lochelper);
    AutoPtr<ILocale> loc;
    lochelper->GetUS((ILocale**)&loc);
    numhelper->GetInstanceEx(loc,(INumberFormat**)&df);
    PFL_EX("df : %p , loc : %p" , df.Get() ,loc.Get())
    df->SetMaximumFractionDigits(0);
    df->SetRoundingMode(RoundingMode_HALF_UP);
    String outstr(NULL);
    df->FormatDouble(-0.2,&outstr);
    PFL_EX("outstr: %s" , outstr.string())
    assert(outstr.Equals("-0"));
    df->SetMaximumFractionDigits(1);
    df->FormatDouble(-0.2,&outstr);
    PFL_EX("outstr: %s " ,outstr.string())
    assert(outstr.Equals("-0.2"));

    return 0;
}
// Android fails this test, truncating to 127 digits.
int CTest::test_setMaximumIntegerDigits(int argc, char* argv[])
{
    AutoPtr<INumberFormatHelper> nfh;
    CNumberFormatHelper::AcquireSingleton((INumberFormatHelper**)&nfh);
    AutoPtr<INumberFormat> numberFormat;
    AutoPtr<ILocale> Locale_US;
    CLocale::New(String("en"), String("US"), (ILocale**)&Locale_US);
    nfh->GetNumberInstanceEx(Locale_US, (INumberFormat**)&numberFormat);
    numberFormat->SetGroupingUsed(FALSE);
    numberFormat->SetMinimumIntegerDigits(400);
    String str;
    numberFormat->FormatInt64(123L, &str);
    PFL_EX("strlen: %d " ,str.GetLength())
    // shuld failed
    if (str.GetLength() != 309) {
        printf("should failed~~~~~~~~\n");
    }

    AutoPtr<IBigIntegerHelper> bighelper;
    CBigIntegerHelper::AcquireSingleton((IBigIntegerHelper**)&bighelper);
    AutoPtr<IBigInteger> bigint;
    bighelper->ValueOf(123L,(IBigInteger**)&bigint);
    str = String("");
    AutoPtr<IFieldPosition> position;
    CFieldPosition::New(0, (IFieldPosition**)&position);
    AutoPtr<IStringBuffer> sbbuf;
    sbbuf = new StringBuffer();
    AutoPtr<IStringBuffer> outbuf;
    numberFormat->FormatObjectEx((IInterface*)bigint->Probe(EIID_IInterface),sbbuf,position,(IStringBuffer**)&outbuf);
    outbuf->ToString(&str);
    PFL_EX("strlen2: %d " ,str.GetLength())
    // shuld failed
    if (str.GetLength() != 309) {
        printf("should failed~~~~~~~~\n");
    }

    return 0;
}

void assertBigDecimalWithFraction(AutoPtr<IBigDecimal> bd, String expectedResult, Int32 fraction);

void assertDecFmtWithMultiplierAndFraction(String value, int multiplier, int fraction, String expectedResult);

void assertDecFmtWithMultiplierAndFractionByLocale(String value, int multiplier, int fraction,
                                                    AutoPtr<ILocale> locale, String expectedResult);

// Regression test for http://b/1897917: BigDecimal does not take into account multiplier.
int CTest::test_BigDecimalBug1897917(int argc, char* argv[]) {
    // For example. the BigDecimal 0.17 formatted in PercentInstance is 0% instead of 17%:
    AutoPtr<INumberFormatHelper> pfhelper;
    CNumberFormatHelper::AcquireSingleton((INumberFormatHelper **) &pfhelper);
    AutoPtr<INumberFormat> pf;
    pfhelper->GetPercentInstance((INumberFormat**)&pf);
    AutoPtr<IBigDecimalHelper> bdhelper;
    CBigDecimalHelper::AcquireSingleton((IBigDecimalHelper **) &bdhelper);
    AutoPtr<IBigDecimal> bd;
    bdhelper->ValueOfEx2(0.17,(IBigDecimal**)&bd);
    AutoPtr<IFieldPosition> position;
    CFieldPosition::New(0, (IFieldPosition**)&position);
    String outstr;

    AutoPtr<IStringBuffer> sbbuf;
    sbbuf = new StringBuffer();
    AutoPtr<IStringBuffer> outbuf;
    pf->FormatObjectEx((IInterface*)bd->Probe(EIID_IInterface),sbbuf,position,(IStringBuffer**)&outbuf);
    outbuf->ToString(&outstr);

    PFL_EX("outstr: %s " ,outstr.string())
    assert(outstr.Equals("17%"));

    // Test long decimal formatted in PercentInstance with various fractions.
    String longDec("11.2345678901234567890123456789012345678901234567890");
    CBigDecimal::New(longDec,(IBigDecimal**)&bd);

    assertBigDecimalWithFraction(bd, String("1,123.46%"), 2);
    assertBigDecimalWithFraction(bd, String("1,123.45678901%"), 8);
    assertBigDecimalWithFraction(bd, String("1,123.4567890123%"), 10);
    assertBigDecimalWithFraction(bd, String("1,123.45678901234567890123%"), 20);
    assertBigDecimalWithFraction(bd, String("1,123.456789012345678901234567890123%"), 30);

    // Test trailing zeros.
    assertDecFmtWithMultiplierAndFraction(String("3333.33333333"), 3, 4, String("10,000"));
    assertDecFmtWithMultiplierAndFraction(String("3333.33333333"), -3, 4, String("-10,000"));
    assertDecFmtWithMultiplierAndFraction(String("0.00333333"), 3, 4, String("0.01"));

    AutoPtr<ILocaleHelper> lh;
    CLocaleHelper::AcquireSingleton((ILocaleHelper **) &lh);
    AutoPtr<ILocale> locale;
    lh->GetUS((ILocale**)&locale);
    assertDecFmtWithMultiplierAndFractionByLocale(String("3330000000000000000000000000000000"), 3, 4,
                                            locale, String("9,990,000,000,000,000,000,000,000,000,000,000"));
}

int CTest::testBigDecimalTestBigIntWithMultiplier(int argc, char* argv[]) {
    // Big integer tests.
    assertDecFmtWithMultiplierAndFraction(String("123456789012345"), 10, 0,
            String("1,234,567,890,123,450"));
    assertDecFmtWithMultiplierAndFraction(String("12345678901234567890"), 10, 0,
            String("123,456,789,012,345,678,900"));
    assertDecFmtWithMultiplierAndFraction(String("98765432109876543210987654321"), 10, 0,
            String("987,654,321,098,765,432,109,876,543,210"));

    assertDecFmtWithMultiplierAndFraction(String("123456789012345"), -10, 0,
            String("-1,234,567,890,123,450"));
    assertDecFmtWithMultiplierAndFraction(String("12345678901234567890"), -10, 0,
            String("-123,456,789,012,345,678,900"));
    assertDecFmtWithMultiplierAndFraction(String("98765432109876543210987654321"), -10, 0,
            String("-987,654,321,098,765,432,109,876,543,210"));

    AutoPtr<ILocale> inLocale;
    CLocale::New(String("en"), String("IN"), (ILocale**)&inLocale);
    assertDecFmtWithMultiplierAndFractionByLocale(String("123456789012345"), 10, 0,
            inLocale, String("1,23,45,67,89,01,23,450"));
    assertDecFmtWithMultiplierAndFractionByLocale(String("12345678901234567890"), 10, 0,
            inLocale, String("12,34,56,78,90,12,34,56,78,900"));
    assertDecFmtWithMultiplierAndFractionByLocale(String("98765432109876543210987654321"), 10, 0,
            inLocale, String("9,87,65,43,21,09,87,65,43,21,09,87,65,43,210"));

    assertDecFmtWithMultiplierAndFractionByLocale(String("123456789012345"), -10, 0,
            inLocale, String("-1,23,45,67,89,01,23,450"));
    assertDecFmtWithMultiplierAndFractionByLocale(String("12345678901234567890"), -10, 0,
            inLocale, String("-12,34,56,78,90,12,34,56,78,900"));
    assertDecFmtWithMultiplierAndFractionByLocale(String("98765432109876543210987654321"), -10, 0,
            inLocale, String("-9,87,65,43,21,09,87,65,43,21,09,87,65,43,210"));
}

int CTest::testBigDecimalICUConsistency(int argc, char* argv[]) {

    // AutoPtr<IDecimalFormat> df;
    // AutoPtr<INumberFormatHelper> nfhelper;
    // CNumberFormatHelper::AcquireSingleton((INumberFormatHelper**)&nfhelper);
    // nfhelper->GetInstance((INumberFormat**)&df);
    // df->SetMaximumFractionDigits(2);
    // df->setMultiplier(2);

    // AutoPtr<IBigDecimalHelper> bdhelper;
    // CBigDecimalHelper::AcquireSingleton((IBigDecimalHelper **) &bdhelper);
    // AutoPtr<IBigDecimal> bd;
    // bdhelper->ValueOfEx2(0.17,(IBigDecimal**)&bd);
    // AutoPtr<IFieldPosition> position;
    // CFieldPosition::New(0, (IFieldPosition**)&position);
    // String outstr;
    // df->FormatObjectEx(bd,String(NULL),position,&outstr);


    // assertEquals(df.format(BigDecimal.valueOf(0.16)),
    // df.format(BigDecimal.valueOf(0.16).doubleValue()));
    // assertEquals(df.format(BigDecimal.valueOf(0.0293)),
    // df.format(BigDecimal.valueOf(0.0293).doubleValue()));
    // assertEquals(df.format(BigDecimal.valueOf(0.006)),
    // df.format(BigDecimal.valueOf(0.006).doubleValue()));
    // assertEquals(df.format(BigDecimal.valueOf(0.00283)),
    // df.format(BigDecimal.valueOf(0.00283).doubleValue()));
    // assertEquals(df.format(BigDecimal.valueOf(1.60)),
    // df.format(BigDecimal.valueOf(1.60).doubleValue()));
    // assertEquals(df.format(BigDecimal.valueOf(15)),
    // df.format(BigDecimal.valueOf(15).doubleValue()));
    // assertEquals(df.format(BigDecimal.valueOf(170)),
    // df.format(BigDecimal.valueOf(170).doubleValue()));
    // assertEquals(df.format(BigDecimal.valueOf(234.56)),
    // df.format(BigDecimal.valueOf(234.56).doubleValue()));
    // assertEquals(df.format(BigDecimal.valueOf(0)),
    // df.format(BigDecimal.valueOf(0).doubleValue()));
    // assertEquals(df.format(BigDecimal.valueOf(-1)),
    // df.format(BigDecimal.valueOf(-1).doubleValue()));
    // assertEquals(df.format(BigDecimal.valueOf(-10000)),
    // df.format(BigDecimal.valueOf(-10000).doubleValue()));
    // assertEquals(df.format(BigDecimal.valueOf(-0.001)),
    // df.format(BigDecimal.valueOf(-0.001).doubleValue()));
    // assertEquals(df.format(BigDecimal.valueOf(1234567890.1234567)),
    // df.format(BigDecimal.valueOf(1234567890.1234567).doubleValue()));
    // assertEquals(df.format(BigDecimal.valueOf(1.234567E100)),
    // df.format(BigDecimal.valueOf(1.234567E100).doubleValue()));
}

void assertBigDecimalWithFraction(AutoPtr<IBigDecimal> bd, String expectedResult, Int32 fraction) {
    AutoPtr<INumberFormatHelper> pfhelper;
    CNumberFormatHelper::AcquireSingleton((INumberFormatHelper **) &pfhelper);
    AutoPtr<INumberFormat> pf;
    pfhelper->GetPercentInstance((INumberFormat**)&pf);
    pf->SetMaximumFractionDigits(fraction);
    AutoPtr<IFieldPosition> position;
    CFieldPosition::New(0, (IFieldPosition**)&position);
    String outstr;

    AutoPtr<IStringBuffer> sbbuf;
    sbbuf = new StringBuffer();
    AutoPtr<IStringBuffer> outbuf;
    pf->FormatObjectEx((IInterface*)bd->Probe(EIID_IInterface),sbbuf,position,(IStringBuffer**)&outbuf);
    outbuf->ToString(&outstr);

    // PFL_EX("outstr: %s ,expectedResult: %s " ,outstr.string() ,expectedResult.string())

    assert(expectedResult.Equals(outstr));
}

void assertDecFmtWithMultiplierAndFraction(String value, int multiplier, int fraction, String expectedResult) {
    AutoPtr<IDecimalFormat> df;
    AutoPtr<INumberFormatHelper> nfhelper;
    CNumberFormatHelper::AcquireSingleton((INumberFormatHelper**)&nfhelper);
    nfhelper->GetInstance((INumberFormat**)&df);
    df->SetMultiplier(multiplier);
    df->SetMaximumFractionDigits(fraction);
    AutoPtr<IBigDecimal> d;
    CBigDecimal::New(value,(IBigDecimal**)&d);
    String str2;
    d->ToString(&str2);
    // PFL_EX("value:%s, str2:%s",value.string(),str2.string())


    AutoPtr<IFieldPosition> position;
    CFieldPosition::New(0, (IFieldPosition**)&position);
    String outstr;

    AutoPtr<IStringBuffer> sbbuf;
    sbbuf = new StringBuffer();
    AutoPtr<IStringBuffer> outbuf;
    df->FormatObjectEx((IInterface*)d->Probe(EIID_IInterface),sbbuf,position,(IStringBuffer**)&outbuf);
    outbuf->ToString(&outstr);

    PFL_EX("outstr: %s ,expectedResult: %s " ,outstr.string() ,expectedResult.string())
    assert(expectedResult.Equals(outstr));
}

void assertDecFmtWithMultiplierAndFractionByLocale(String value, int multiplier, int fraction,
                                                    AutoPtr<ILocale> locale, String expectedResult) {
    AutoPtr<IDecimalFormat> df;
    AutoPtr<INumberFormatHelper> nfh;
    CNumberFormatHelper::AcquireSingleton((INumberFormatHelper**)&nfh);
    ECode ec = nfh->GetIntegerInstanceEx(locale,(INumberFormat**)&df);
    df->SetMultiplier(multiplier);
    df->SetMaximumFractionDigits(fraction);

    AutoPtr<IBigDecimal> d;
    CBigDecimal::New(value,(IBigDecimal**)&d);
    AutoPtr<IFieldPosition> position;
    CFieldPosition::New(0, (IFieldPosition**)&position);
    String outstr ,outstr2;

    AutoPtr<IStringBuffer> sbbuf;
    sbbuf = new StringBuffer();
    AutoPtr<IStringBuffer> outbuf;
    df->FormatObjectEx((IInterface*)d->Probe(EIID_IInterface), sbbuf, position, (IStringBuffer**)&outbuf);
    outbuf->ToString(&outstr);

    d->ToString(&outstr2);
    PFL_EX("outstr : %s , expectedResult : %s " , outstr.string() , expectedResult.string())
    assert(expectedResult.Equals(outstr));
}
