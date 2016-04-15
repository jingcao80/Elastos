#include "test.h"
#include "cmdef.h"
#include "Elastos.CoreLibrary.h"
#include "elastos/StringBuffer.h"

using namespace Elastos::Core;
using namespace Elastos::Utility;

#include "Elastos.CoreLibrary.h"
#include <elautoptr.h>
#include <stdio.h>

CTest::CTest(){
    AutoPtr<ILocaleHelper> lhep;
    AutoPtr<ILocale> locus;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&lhep);
    lhep->GetUS((ILocale**)&locus);
    lhep->SetDefault(locus);

    // test with repeating formats and max argument index < max offset
    String pattern("A {3, number, currency} B {2, time} C {0, number, percent} D {4}  E {1,choice,0#off|1#on} F {0, date}");
    CMessageFormat::New(pattern, (IMessageFormat**)&format1);
}

int CTest::test_applyPattern(int argc, char* argv[])
{
    AutoPtr<IMessageFormat> format;
    CMessageFormat::New(String("test"), (IMessageFormat**)&format);
    AutoPtr<ILocale> Locale_FRENCH;
    AutoPtr<ILocaleHelper> lochelp;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&lochelp);
    lochelp->GetFRENCH((ILocale**)&Locale_FRENCH);
    format->SetLocale(Locale_FRENCH);
    // same LONG and FULL time patterns
    format->ApplyPattern(String("{0,time, Full}"));
    String pattern;
    format->ToPattern(&pattern);
    PFL_EX("pattern : %s " ,pattern.string())
    if (!pattern.Equals(String("{0,time, Full}"))) {
        printf("Wrong full time pattern\n");
        return 0;
    }
    return 0;
}

int CTest::test_formatToCharacterIterator(int argc, char* argv[])
{
    // Test for method formatToCharacterIterator(java.lang.Object)
    AutoPtr<IMessageFormat> format;
    CMessageFormat::New(String("{1, number}"), (IMessageFormat**)&format);
    AutoPtr<IAttributedCharacterIterator> aci;
    format->FormatToCharacterIterator(NULL, (IAttributedCharacterIterator**)&aci);
    if (aci == NULL)
    {
        printf("fail 1 is expected ~~\n");
    }

    AutoPtr<IMessageFormat> format2;
    CMessageFormat::New(String("{0, time}"), (IMessageFormat**)&format2);
    AutoPtr<IAttributedCharacterIterator> aci2;
    AutoPtr<IInterface> intface;
    format->FormatToCharacterIterator(intface, (IAttributedCharacterIterator**)&aci2);
    if (aci2 == NULL)
    {
        printf("fail 2 is expected ~~\n");
    }
    return 0;
}

int CTest::test_getLocale(int argc, char* argv[])
{

    ArrayOf<ILocale*> * l = ArrayOf<ILocale*>::Alloc(7);
    AutoPtr<ILocale> Locale_FRANCE, Locale_KOREA, l1, l2, l3, l4, l5;
    CLocale::New(String("fr"), String("FR"), (ILocale**)&Locale_FRANCE);
    CLocale::New(String("ko"), String("KR"), (ILocale**)&Locale_KOREA);
    String country, language;
    Locale_FRANCE->GetCountry(&country);
    Locale_FRANCE->GetLanguage(&language);
    CLocale::New(country, language, (ILocale**)&l1);
    CLocale::New(String("mk"), (ILocale**)&l2);
    CLocale::New(String("mk"), String("MK"), (ILocale**)&l3);
    CLocale::New(String("en"), String("US"), (ILocale**)&l4);
    CLocale::New(String("#ru"), String("@31230"), (ILocale**)&l5);
    (*l)[0] = Locale_FRANCE;
    (*l)[1] = Locale_KOREA;
    (*l)[2] = l1;
    (*l)[3] = l2;
    (*l)[4] = l3;
    (*l)[5] = l4;
    (*l)[6] = l5;
    String pattern("getLocale test {0,number,#,####}");
    AutoPtr<IMessageFormat> mf;
    for (Int32 i = 0; i < l->GetLength(); i++) {
        CMessageFormat::New(pattern, (*l)[i], (IMessageFormat**)&mf);
        AutoPtr<ILocale> result;
        mf->GetLocale((ILocale**)&result);
        assert((*l)[i] == result);

        String language1, language2;
        (*l)[i]->GetLanguage(&language1);
        result->GetLanguage(&language2);
        assert(language1.Equals(language2));

        String country1, country2;
        (*l)[i]->GetCountry(&country1);
        result->GetCountry(&country2);
        PFL_EX("country1 : %s , country2 :%s " , country1.string() , country2.string())
        assert(country1.Equals(country2));
    }
    CMessageFormat::New(pattern, (IMessageFormat**)&mf);
    mf->SetLocale(NULL);
    AutoPtr<ILocale> result;
    mf->GetLocale((ILocale**)&result);
    PFL_EX("result: %p" , result.Get())
    assert(result == NULL);
    return 0;
}

int CTest::test_setFormat(int argc, char* argv[])
{
    AutoPtr<IMessageFormat> format1;
    AutoPtr<ILocale> Locale_US;
    CLocale::New(String("en"), String("US"), (ILocale**)&Locale_US);
    AutoPtr<ILocaleHelper> lh;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&lh);
    lh->SetDefault(Locale_US);

    String pattern("A {3, number, currency} B {2, time} C {0, number, percent} D {4}  E {1,choice,0#off|1#on} F {0, date}");
    CMessageFormat::New(pattern, (IMessageFormat**)&format1);

    AutoPtr<IMessageFormat> f1 = format1;
    AutoPtr<IDateFormat> df;
    AutoPtr<IDateFormatHelper> dfh;
    CDateFormatHelper::AcquireSingleton((IDateFormatHelper**)&dfh);
    dfh->GetTimeInstance((IDateFormat**)&df);
    AutoPtr<INumberFormat> nf;
    AutoPtr<INumberFormatHelper> nfh;
    CNumberFormatHelper::AcquireSingleton((INumberFormatHelper**)&nfh);
    nfh->GetInstance((INumberFormat**)&nf);
    // case 1: Compare getFormats() results after calls to setFormat()
    f1->SetFormat(0, df);
    f1->SetFormat(1, df);
    f1->SetFormat(2, nf);
    AutoPtr<IChoiceFormat> cf1, cf2;
    CChoiceFormat::New(String("0#off|1#on") , (IChoiceFormat**)&cf1);
    CChoiceFormat::New(String("1#few|2#ok|3#a lot") , (IChoiceFormat**)&cf2);
    f1->SetFormat(3, cf1);
    f1->SetFormat(4, cf2);
    f1->SetFormat(5, df);

    AutoPtr<ArrayOf<IFormat*> > formats;
    f1->GetFormats((ArrayOf<IFormat*> **)&formats);
    AutoPtr<ArrayOf<AutoPtr<IFormat> > > correctFormats = ArrayOf<AutoPtr<IFormat> >::Alloc(6);
    (*correctFormats)[0] = df;
    (*correctFormats)[1] = df;
    (*correctFormats)[2] = nf;
    (*correctFormats)[3] = cf1;
    (*correctFormats)[4] = cf2;
    (*correctFormats)[5] = df;

    PFL_EX("correctFormats : %d == formats : %d " , correctFormats->GetLength() ,  formats->GetLength())
    assert(correctFormats->GetLength() == formats->GetLength());
    for (int i = 0; i < correctFormats->GetLength(); i++) {
        PFL_EX("index : %d , correctFormats : %p ,formats : %p " ,i ,(*correctFormats)[i].Get(), (*formats)[i] )
        assert( (*correctFormats)[i] == (*formats)[i]);
    }

    // case 2: Try to setFormat using incorrect index
    f1->SetFormat(-1, df);
    ECode ec = f1->GetFormats((ArrayOf<IFormat*> **)&formats);
    if (ec != NOERROR)
    {
        PFL_EX("Expected ArrayIndexOutOfBoundsException was not thrown");
    }
    AutoPtr<IDateFormat> df2;
    dfh->GetDateTimeInstance((IDateFormat**)&df2);
    ec = f1->SetFormat(formats->GetLength(), df2);
    if (ec != NOERROR)
    {
        PFL_EX("Expected ArrayIndexOutOfBoundsException was not thrown")
    }

}

int CTest::test_formatLjava_lang_ObjectLjava_lang_StringBufferLjava_text_FieldPosition(int argc, char* argv[])
{
    // Test for method java.lang.StringBuffer
    // java.text.MessageFormat.format(java.lang.Object [],
    // java.lang.StringBuffer, java.text.FieldPosition)
    AutoPtr<IMessageFormat> format;
    ASSERT_SUCCEEDED(CMessageFormat::New(String("{1,number,integer}"), (IMessageFormat**)&format));
    AutoPtr<IStringBuffer> buffer = new StringBuffer();
    AutoPtr< ArrayOf<IInterface*> > objlst = ArrayOf<IInterface*>::Alloc(2);
    String zerostr("0");
    AutoPtr<ICharSequence> charSeq;
    CString::New(zerostr, (ICharSequence**)&charSeq);
    AutoPtr<IDouble> dinter;
    CDouble::New(53.863, (IDouble**)&dinter);
    objlst->Set(0, charSeq);
    objlst->Set(1, dinter);
    AutoPtr<IFieldPosition> sfp;
    AutoPtr<IMessageFormatField> ARGUMENT;
    CMessageFormatField::New(String("message argument field"), (IMessageFormatField**)&ARGUMENT);
    CFieldPosition::New(ARGUMENT, (IFieldPosition**)&sfp);
    AutoPtr<IStringBuffer> outbuf;
    format->FormatObjects(objlst, buffer, sfp, (IStringBuffer**)&outbuf);
    String outstr;
    outbuf->ToString(&outstr);
    // assertEquals("Wrong result", "54", buffer.toString());
    assert(String("54").Equals(outstr));
    format->FormatObjects(objlst, buffer, sfp, (IStringBuffer**)&outbuf);
    outbuf->ToString(&outstr);
    // assertEquals("Wrong result", "5454", buffer.toString());
    assert(String("5454").Equals(outstr));

    buffer = new StringBuffer();
    format->ApplyPattern(String("{0,choice,0#zero|1#one '{1,choice,2#two {2,time}}'}"));
    AutoPtr<IDate> date;
    CDate::New((IDate**)&date);
    AutoPtr<IDateFormatHelper> dfh;
    CDateFormatHelper::AcquireSingleton((IDateFormatHelper**)&dfh);
    AutoPtr<IDateFormat> df;
    dfh->GetTimeInstance((IDateFormat**)&df);
    String datestr;
    df->FormatDate(date,&datestr);
    String expected = String("one two ") + datestr;

    CDouble::New(1.6, (IDouble**)&dinter);
    AutoPtr<IInteger32> iinter;
    CInteger32::New(3, (IInteger32**)&iinter);
    objlst = ArrayOf<IInterface*>::Alloc(3);
    objlst->Set(0, dinter);
    objlst->Set(1, iinter);
    objlst->Set(2, date);
    format->FormatObjects(objlst, buffer, sfp, (IStringBuffer**)&outbuf);
    outbuf->ToString(&outstr);
    // assertEquals("Choice not recursive:\n" + expected + "\n" + buffer,
    //         expected, buffer.toString());
    assert(outstr.Equals(expected));

    CDouble::New(0.6, (IDouble**)&dinter);
    objlst = ArrayOf<IInterface*>::Alloc(2);
    objlst->Set(0, dinter);
    objlst->Set(1, iinter);
    format->FormatObjects(objlst, buffer, NULL, (IStringBuffer**)&outbuf);
    // assertEquals(expected + "zero", str.toString());
    outbuf->ToString(&outstr);
    assert(outstr.Equals(expected + String("zero")));
    // assertEquals(expected + "zero", buffer.toString());
    buffer->ToString(&outstr);
    assert(outstr.Equals(expected + String("zero")));

    // try {
    objlst = ArrayOf<IInterface*>::Alloc(3);
    String nullstr("");
    CString::New(nullstr, (ICharSequence**)&charSeq);
    CDouble::New(1, (IDouble**)&dinter);
    objlst->Set(0, (IInterface*)&zerostr);
    objlst->Set(1, dinter);
    objlst->Set(2, (IInterface*)&charSeq);
    ECode ec = format->FormatObjects(objlst, buffer, sfp, (IStringBuffer**)&outbuf);
    if (ec != NOERROR)
    {
        printf("IllegalArgumentException was not thrown.\n");
    }
    //     fail("IllegalArgumentException was not thrown.");
    // } catch(IllegalArgumentException iae) {
    //     //expected
    // }

    // try {
    objlst = ArrayOf<IInterface*>::Alloc(2);
    objlst->Set(0, (IInterface*)&charSeq);
    objlst->Set(1, iinter);
    ec = format->FormatObjects(objlst, buffer, sfp, (IStringBuffer**)&outbuf);
    if (ec != NOERROR)
    {
        printf("IllegalArgumentException was not thrown.\n");
    }
    //     fail("IllegalArgumentException was not thrown.");
    // } catch(IllegalArgumentException iae) {
    //     //expected
    // }
    return 0;
}

int CTest::test_formatLjava_lang_StringLjava_lang_Object(int argc, char* argv[])
{
    // AutoPtr<ITimeZoneHelper> tzh;
    // CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&tzh);
    // AutoPtr<ITimeZone> tzutc;
    // tzh->GetTimeZone(String("UTC"), (ITimeZone**)&tzutc);
    // tzh->SetDefault(tzutc);
    // int iCurrency = 123;
    // int iInteger  = 0x80000000; // Integer.MIN_VALUE;
    // AutoPtr<IDate> date;
    // CDate::New(12345678, (IDate**)&date);
    // AutoPtr< ArrayOf<IInterface*> > objlst = ArrayOf<IInterface*>::Alloc(3);
    // objlst->Set(0, date);
    // objlst->Set(1, (IInterface*)&iCurrency);
    // objlst->Set(2, (IInterface*)&iInteger);
    // String resStr   = String("Date: Jan 1, 1970 Currency: $") + /* iCurrency */ String("123") + String(".00 Integer: -2,147,483,648");
    // String pattern  = String("Date: {0,date} Currency: {1, number, currency} Integer: {2, number, integer}");

    // String   sFormat  = MessageFormat.format(pattern, (Object[]) args);
    // assertEquals(
    //         "format(String, Object[]) with valid parameters returns incorrect string: case 1",
    //         sFormat, resStr);

    // pattern = "abc {4, number, integer} def {3,date} ghi {2,number} jkl {1,choice,0#low|1#high} mnop {0}";
    // resStr  = "abc -2,147,483,648 def Jan 1, 1970 ghi -2,147,483,648 jkl high mnop -2,147,483,648";
    // Object[] args_ = { iInteger, 1, iInteger, date, iInteger };
    // sFormat = MessageFormat.format(pattern, args_);
    // assertEquals(
    //         "format(String, Object[]) with valid parameters returns incorrect string: case 1",
    //         sFormat, resStr);

    // try {
    //     args = null;
    //     MessageFormat.format(null, args);
    //     fail("Doesn't throw IllegalArgumentException: null, null");
    // } catch (Exception e) {
    //     // expected
    // }

    // try {
    //     MessageFormat.format("Invalid {1,foobar} format descriptor!",
    //             new Object[] {iInteger} );
    //     fail("Doesn't throw IllegalArgumentException with invalid pattern as a parameter: case 1");
    // } catch (IllegalArgumentException ex) {
    //     // expected
    // }

    // try {
    //     MessageFormat.format(
    //             "Invalid {1,date,invalid-spec} format descriptor!", new Object[]{""});
    //     fail("Doesn't throw IllegalArgumentException with invalid pattern as a parameter: case 2");
    // } catch (IllegalArgumentException ex) {
    //     // expected
    // }

    // try {
    //     MessageFormat.format("{0,number,integer", new Object[] {iInteger});
    //     fail("Doesn't throw IllegalArgumentException, doesn't detect unmatched brackets");
    // } catch (IllegalArgumentException ex) {
    //     // expected
    // }

    // try {
    //     MessageFormat.format(
    //             "Valid {1, date} format {0, number} descriptor!", new Object[]{ "" } );
    //     fail("Doesn't throw IllegalArgumentException with invalid Object array");
    // } catch (IllegalArgumentException ex) {
    //     // expected
    // }
    return 0;
}

int CTest::test_formatLjava_lang_ObjectLjava_lang_StringBufferLjava_text_FieldPosition2(int argc, char* argv[])
{
    // // Test for method java.lang.StringBuffer
    // // java.text.MessageFormat.format(java.lang.Object,
    // // java.lang.StringBuffer, java.text.FieldPosition)
    // new Support_MessageFormat(
    //         "test_formatLjava_lang_ObjectLjava_lang_StringBufferLjava_text_FieldPosition")
    //         .t_format_with_FieldPosition();

    // String pattern = "On {4,date} at {3,time}, he ate {2,number, integer} " +
    //         "hamburger{2,choice,1#|1<s}.";
    // MessageFormat format = new MessageFormat(pattern, Locale.US);

    // Object[] objects = new Object[] { "", new Integer(3), 8, ""};

    // try {
    //     format.format(objects, new StringBuffer(),
    //             new FieldPosition(DateFormat.Field.AM_PM));
    //     fail("IllegalArgumentException was not thrown.");
    // } catch(IllegalArgumentException iae) {
    //     //expected
    // }
    return 0;
}

int CTest::test_setFormatsLjava_text_Format(int argc, char* argv[])
{
    // try {
    //     MessageFormat f1 = (MessageFormat) format1.clone();

    //     // case 1: Test with repeating formats and max argument index < max
    //     // offset
    //     // compare getFormats() results after calls to setFormats(Format[])
    //     Format[] correctFormats = new Format[] {
    //             DateFormat.getTimeInstance(),
    //             new ChoiceFormat("0#off|1#on"),
    //             DateFormat.getTimeInstance(),
    //             NumberFormat.getCurrencyInstance(),
    //             new ChoiceFormat("1#few|2#ok|3#a lot") };

    //     f1.setFormats(correctFormats);
    //     Format[] formats = f1.getFormats();

    //     assertTrue("Test1A:Returned wrong number of formats:",
    //             correctFormats.length <= formats.length);
    //     for (int i = 0; i < correctFormats.length; i++) {
    //         assertEquals("Test1B:wrong format for argument index " + i
    //                 + ":", correctFormats[i], formats[i]);
    //     }

    //     // case 2: Try to pass null argument to setFormats().
    //     try {
    //         f1.setFormats(null);
    //         fail("Expected exception NullPointerException was not thrown");
    //     } catch (NullPointerException e) {
    //         // expected
    //     }
    // } catch (Exception e) {
    //     fail("Unexpected exception " + e.toString());
    // }
    return 0;
}

int CTest::test_parseLjava_lang_String(int argc, char* argv[])
{
    // String pattern = "A {3, number, currency} B {2, time} C {0, number, percent} D {4}  E {1,choice,0#off|1#on} F {0, date}";
    // MessageFormat mf = new MessageFormat(pattern);
    // String sToParse = "A $12,345.00 B 9:56:07 AM C 3,200% D 1/15/70 9:56 AM  E on F Jan 1, 1970";
    // Object[] result;
    // try {
    //     result = mf.parse(sToParse);

    //     assertTrue("No result: " + result.length, result.length == 5);
    //     assertTrue("Object 0 is not date", result[0] instanceof Date);
    //     assertEquals("Object 1 is not stringr", result[1].toString(), "1.0");
    //     assertTrue("Object 2 is not date", result[2] instanceof Date);
    //     assertEquals("Object 3 is not number", result[3].toString(),
    //             "12345");
    //     assertEquals("Object 4 is not string", result[4].toString(),
    //             "1/15/70 9:56 AM");

    // } catch (java.text.ParseException pe) {
    //     fail("ParseException is thrown for incorrect string " + sToParse);
    // }

    // sToParse = "xxdate is Feb 28, 1999";
    // try {
    //     result = format1.parse(sToParse);
    //     fail("ParseException is thrown for incorrect string " + sToParse);
    // } catch (java.text.ParseException pe) {
    //     // expected
    // }

    // sToParse = "vm=Test, @3 4 6, 3   ";
    // mf = new MessageFormat("vm={0},{1},{2}");
    // try {
    //     result = mf.parse(sToParse);
    //     assertTrue("No result: " + result.length, result.length == 3);
    //     assertEquals("Object 0 is not string", result[0].toString(), "Test");
    //     assertEquals("Object 1 is not string", result[1].toString(),
    //             " @3 4 6");
    //     assertEquals("Object 2 is not string", result[2].toString(),
    //             " 3   ");
    // } catch (java.text.ParseException pe) {
    //     fail("ParseException is thrown for correct string " + sToParse);
    // }

    // try {
    //     result = mf.parse(null);
    //     fail("ParseException is not thrown for null " + sToParse);
    // } catch (java.text.ParseException pe) {
    //     // expected
    // }
    return 0;
}

int CTest::test_parseObjectLjava_lang_StringLjavajava_text_ParsePosition(int argc, char* argv[])
{
    // MessageFormat mf = new MessageFormat("{0,number,#.##}, {0,number,#.#}");
    // try {
    //     // case 1: Try to parse correct data string.
    //     Object[] objs = { new Double(3.1415) };
    //     String result = mf.format(objs);
    //     // result now equals "3.14, 3.1"
    //     Object[] res = null;
    //     ParsePosition pp = new ParsePosition(0);
    //     int parseIndex = pp.getIndex();
    //     res = (Object[]) mf.parseObject(result, pp);
    //     assertTrue("Parse operation return null", res != null);
    //     assertTrue("parse operation return array with incorrect length",
    //             1 == res.length);
    //     assertTrue("ParseIndex is incorrect", pp.getIndex() != parseIndex);
    //     assertTrue("Result object is incorrect", new Double(3.1)
    //             .equals(res[0]));

    //     // case 2: Try to parse partially correct data string.
    //     pp.setIndex(0);
    //     char[] cur = result.toCharArray();
    //     cur[cur.length / 2] = 'Z';
    //     String partialCorrect = new String(cur);
    //     res = (Object[]) mf.parseObject(partialCorrect, pp);
    //     assertTrue("Parse operation return null", res == null);
    //     assertTrue("ParseIndex is incorrect", pp.getIndex() == 0);
    //     assertTrue("ParseErrorIndex is incorrect",
    //             pp.getErrorIndex() == cur.length / 2);

    //     // case 3: Try to use argument ParsePosition as null.
    //     try {
    //         mf.parseObject(result, null);
    //         fail("Expected NullPointerException was not thrown");
    //     } catch (NullPointerException e) {
    //         // expected
    //     }
    // } catch (Exception e) {
    //     fail("Unexpected exception " + e.toString());
    // }
    return 0;
}

int CTest::test_format_Object(int argc, char* argv[])
{
    // // Regression for HARMONY-1875
    // Locale.setDefault(Locale.CANADA);
    // TimeZone.setDefault(TimeZone.getTimeZone("UTC"));
    // String pat = "text here {0,date,yyyyyyyyy} and here";
    // Calendar c = Calendar.getInstance();
    // String etalon = "text here 00000" + c.get(Calendar.YEAR) + " and here";
    // MessageFormat obj = new MessageFormat(pat);
    // assertEquals(etalon, obj.format(new Object[] { new Date() }));
    return 0;
}

int CTest::test_parseLjava_lang_StringLjava_text_ParsePosition(int argc, char* argv[])
{
    // ParsePosition pos = new ParsePosition(2);

    // MessageFormat mf = new MessageFormat("{0}; {0}; {0}");
    // String parse = "a; b; c";
    // try {
    //     mf.parse(parse, null);
    //     fail("NullPointerException was not thrown.");
    // } catch(NullPointerException npe) {
    //     //expected
    // }

    // try {
    //     mf.parse(null, pos);
    // } catch(NullPointerException npe) {
    //     fail("NullPointerException was thrown.");
    // }
    return 0;
}
