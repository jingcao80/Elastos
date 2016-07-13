#include "test.h"
#include <stdio.h>

#include "Elastos.CoreLibrary.h"
#include <elautoptr.h>
#include <elastos/Number.h>
#include <elastos/StringBuffer.h>
#include <elastos/Math.h>

using namespace Elastos;
using namespace Elastos::Text;
using namespace Elastos::Core;
using namespace Libcore::ICU;

int CTest::test_custom_Number_gets_longValue(int argc, char* argv[])
{
    class MyNumber : public Number
    {
    public:
        CARAPI_(PInterface) Probe(
            /* [in]  */ REIID riid)
        {
            if (riid == EIID_INumber)
            {
                return (INumber*)this;
            }
            return NULL;
        }

        CARAPI_(Byte) ByteValue()
        { return 0; }

        CARAPI_(Double) DoubleValue()
        { return 123; }

        CARAPI_(Float) FloatValue()
        { return 0; }

        CARAPI_(Int32) Int32Value()
        { return 0; }

        CARAPI_(Int64) Int64Value()
        { return 0L; }

        CARAPI_(String) ToString()
        { return String(NULL); }
    };

    AutoPtr<INumberFormatHelper> nfh;
    CNumberFormatHelper::AcquireSingleton((INumberFormatHelper**)&nfh);
    AutoPtr<ILocale> locale;
    CLocale::New(String("en"), String("US"), (ILocale**)&locale);
    AutoPtr<INumberFormat> nf;
    nfh->GetNumberInstanceEx(locale, (INumberFormat**)&nf);
    String result("");
PFL
    MyNumber * mynumber = new MyNumber();
    IInterface * object = reinterpret_cast<IInterface *>(mynumber->Probe(EIID_INumber));
PFL_EX("object : %p" , object)
    nf->FormatObject(object, &result);
PFL_EX("result : %s" , result.string())
    assert(result.Equals("123"));
    return 0;
}

int CTest::test_small_BigInteger_gets_longValue(int argc, char* argv[])
{
   class MyNumberFormat : public INumberFormat {
   public:
        CARAPI_(PInterface) Probe(
           /* [in]  */ REIID riid)
        {
           return NULL;
        }

        CARAPI FormatDoubleEx(
           /* [in] */ Double value,
           /* [in] */ const String& buffer,
           /* [in] */ IFieldPosition* field,
           /* [out] */ String* result)
        {
           StringBuffer b(buffer);
           b += "double";
           b.SubstringEx(0, b.GetLength() , result);
           return NOERROR;
        }

        CARAPI FormatInt64Ex(
           /* [in] */ Int64 value,
           /* [in] */ const String& buffer,
           /* [in] */ IFieldPosition* field,
           /* [out] */ String* result)
        {
           StringBuffer b(buffer);
           b += "long";
           b.SubstringEx(0, b.GetLength() , result);
           return NOERROR;
        }

        CARAPI ParseEx(
           /* [in] */ const String& string,
           /* [in] */ IParsePosition* position,
           /* [out] */ INumber** value)
        {
           return E_NOT_IMPLEMENTED;
        }
    };
    /*
        NumberFormat nf = new MyNumberFormat();
        assertEquals("long", nf.format(BigInteger.valueOf(Long.MAX_VALUE)));
        assertEquals("double", nf.format(BigInteger.valueOf(Long.MAX_VALUE).add(BigInteger.ONE)));
        assertEquals("long", nf.format(BigInteger.valueOf(Long.MIN_VALUE)));
        assertEquals("double", nf.format(BigInteger.valueOf(Long.MIN_VALUE).subtract(BigInteger.ONE)));
    */
    // AutoPtr<INumberFormat> nf = (INumberFormat *) new MyNumberFormat();

    return 0;
}

int CTest::test_getIntegerInstance_ar(int argc, char* argv[])
{
    AutoPtr<INumberFormatHelper> nfh;
    CNumberFormatHelper::AcquireSingleton((INumberFormatHelper**)&nfh);
    AutoPtr<ILocale> locale;
    CLocale::New(String("ar"), (ILocale**)&locale);
    AutoPtr<INumberFormat> numberFormat;
    nfh->GetNumberInstanceEx(locale, (INumberFormat**)&numberFormat);

    AutoPtr<INumberFormat> integerFormat;
    nfh->GetIntegerInstanceEx(locale, (INumberFormat**)&integerFormat);

    AutoPtr<IDecimalFormat> df = reinterpret_cast<IDecimalFormat *>(numberFormat->Probe(EIID_INumberFormat));
    String str("");
    df->ToPattern(&str);
    PFL_EX("#,##0.###;#,##0.###-  ==  %s " ,str.string())
    assert(str.Equals("#,##0.###;#,##0.###-"));

    AutoPtr<IDecimalFormat> df2 = reinterpret_cast<IDecimalFormat *>(integerFormat->Probe(EIID_INumberFormat));
    df2->ToPattern(&str);
    PFL_EX("#,##0;#,##0- == %s" , str.string())
    assert(str.Equals("#,##0;#,##0-"));

    return 0;
}

int CTest::test_numberLocalization(int argc, char* argv[])
{
    AutoPtr<ILocale> arabic;
    CLocale::New(String("ar"), (ILocale**)&arabic);
    AutoPtr<INumberFormatHelper> nfh;
    CNumberFormatHelper::AcquireSingleton((INumberFormatHelper**)&nfh);
    AutoPtr<INumberFormat> nf;
    nfh->GetNumberInstanceEx(arabic, (INumberFormat**)&nf);
    AutoPtr<IDecimalFormatSymbols> dfs;
    CDecimalFormatSymbols::New(arabic, (IDecimalFormatSymbols**)&dfs);
    Char32 result;
    dfs->GetZeroDigit(&result);
    Char32 expected = 0x0660;
    PFL_EX(" %c == %c ", expected, result)
    assert(expected == result);
    String str("");
    nf->FormatInt64(1234567890L, &str);
    PFL_EX("%s == str : %s" ,String("\u0661\u066c\u0662\u0663\u0664\u066c\u0665\u0666\u0667\u066c\u0668\u0669\u0660").string(), str.string())
    assert(String("\u0661\u066c\u0662\u0663\u0664\u066c\u0665\u0666\u0667\u066c\u0668\u0669\u0660")
        .Equals(str));
    return 0;
}


int CTest::test_10333(int argc, char* argv[])
{
    AutoPtr<INumberFormatHelper> nfh;
    CNumberFormatHelper::AcquireSingleton((INumberFormatHelper**)&nfh);
    AutoPtr<ILocale> Locale_US;
    CLocale::New(String("en"), String("US"), (ILocale**)&Locale_US);
    AutoPtr<INumberFormat> nf;
    nfh->GetPercentInstanceEx(Locale_US, (INumberFormat**)&nf);
    String str("");
    nf->FormatDouble(0.15, &str);
    assert(str.Equals("15%"));
    nf->FormatDouble(15, &str);
    assert(str.Equals("1,500%"));
/*
        try {
            nf.format("15");
            fail();
        } catch (IllegalArgumentException expected) {
        }
*/
    return 0;
}

int CTest::test_10514(int argc, char* argv[])
{
    AutoPtr<IChoiceFormat> pTCF;
    String fms;
    Double limits[5] = {1 ,2 ,3 , 4, 5};
    String formats[5] = { String("Tiny Apple") , String("Small Apple") , String("Medium Apple") , String("Large Apple") , String("Huge Apple")};
    ArrayOf<Double> appleLimits(limits , 5);
    ArrayOf<String> appleFormats(formats , 5);
    ECode ec = CChoiceFormat::New(appleLimits , appleFormats , (IChoiceFormat**)&pTCF);
    ASSERT_SUCCEEDED(ec);
    ec = pTCF->FormatDouble(Elastos::Core::Math::DOUBLE_NEGATIVE_INFINITY , &fms);
    ASSERT_SUCCEEDED(ec);
    assert(fms.Equals("Tiny Apple"));
}

int CTest::test_10522(int argc, char* argv[])
{
    AutoPtr<IChoiceFormat> pTCF;
    Int32 i = 0;
    String str;
    AutoPtr<ICharSequence> pCS;
    Double tmpl[2] = {0 ,0};
    Double tstLimits[2] = {0 ,1};
    Double limits[4] = {0 ,1 , 0 , 2};
    String tstFms[2] = { String("0") , String("1")};
    String formats[4] = { String("Less than one") , String("one") , String("Between one and two") , String("Greater than two")};
    ArrayOf<Double> appleLimits(limits , 4);
    ArrayOf<String> appleFormats(formats , 4);
    ArrayOf<Double> tmpLim(tmpl , 2);
    ArrayOf<Double> tstSetChoicesLim(tstLimits , 2);
    ArrayOf<Double> * getLim = &tmpLim;
    ArrayOf<String> tstSetChoicesFms(tstFms , 2);
    AutoPtr<ArrayOf<ICharSequence *> > pGetCS = ArrayOf<ICharSequence *>::Alloc(2);
    AutoPtr<ArrayOf<IInterface *> > pGetFms = ArrayOf<IInterface *>::Alloc(2);
    CString::New(String(""), (ICharSequence**)&(*pGetCS)[0]);
    CString::New(String(""), (ICharSequence**)&(*pGetCS)[1]);
    (*pGetFms)[0] = (IInterface *)(*pGetCS)[0]->Probe(EIID_IInterface);
    (*pGetFms)[1] = (IInterface *)(*pGetCS)[1]->Probe(EIID_IInterface);
    ECode ec = CChoiceFormat::New(appleLimits, appleFormats, (IChoiceFormat**)&pTCF);
    ASSERT_SUCCEEDED(ec);
    ec = pTCF->SetChoices(tstSetChoicesLim, tstSetChoicesFms);
    ASSERT_SUCCEEDED(ec);
    ec = pTCF->GetLimits((ArrayOf<Double> **)&getLim);
    ASSERT_SUCCEEDED(ec);
    for(i = 0; i < 2; i++){
        if(tstLimits[i] != (*getLim)[i]){
            printf("GetLimits failed.\n");
        }
    }
PFL
    ec = pTCF->GetFormats((ArrayOf<IInterface*> **)&pGetFms);
PFL_EX("ec : %p , pGetFms : %p , len : %d " ,ec ,pGetFms.Get() , pGetFms->GetLength())
    ASSERT_SUCCEEDED(ec);
    pCS = (ICharSequence*)(*pGetFms)[0]->Probe(EIID_ICharSequence);
PFL
    pCS->ToString(&str);
PFL_EX("str : %s " ,str.string())
    assert( str.Equals(String("0")));
    pCS = NULL;
    pCS = (ICharSequence*)(*pGetFms)[1]->Probe(EIID_ICharSequence);
    pCS->ToString(&str);
PFL_EX("str2 : %s " ,str.string())
    assert(str.Equals(String("1")));
}

