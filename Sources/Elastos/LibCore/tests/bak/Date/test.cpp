#include "test.h"

CTest::CTest()
{}

CTest::~CTest()
{}

int CTest::main(int argc, char* argv[])
{
    printf("========== main ==========\n");
    /*Int64 val = 1110;
    AutoPtr<IGregorianCalendar> cal;
    ECode ec = CGregorianCalendar::New((IGregorianCalendar**)&cal);
    printf("test::Step 2 %08x\n", ec);
    cal = NULL;
    ec = CGregorianCalendar::New(val, (IGregorianCalendar**)&cal);
    printf("test::Step 3 %08x\n", ec);

    printf("test::main is running \n");*/

    AutoPtr<IDate> date;
    // ECode ec = CDate::New((IDate**)&date);
    ECode ec = CDate::New(String("Jan 1, 1970"), (IDate**)&date);
    if (FAILED(ec)){
        printf("Failed to create IDate. %08x\n", ec);
        return 0;
    }

    String str;
    date->ToString(&str);
    printf("Date : %s\n", str.string());

    Int32 year;
    ec = date->GetYear(&year);
    assert(FAILED(ec) == FALSE);
    printf(" Get year: %d\n", year);
    assert(year == 1970);

    year = 2013;
    date->SetYear(year);
    date->ToString(&str);
    printf("Date : %s\n", str.string());

    printf("===============================================\n");
    String format("HH:mm");
    AutoPtr<ISimpleDateFormat> sdf;
    CSimpleDateFormat::New(format, (ISimpleDateFormat**)&sdf);
    printf("===");
    String result;
    sdf->FormatDate(date, &result);
    printf("Format Date: [%s]\n", result.string());
    return 0;
}

int CTest::test_11155(int argc, char* argv[])
{
    AutoPtr<IDate> date = NULL;
    Int32 timezoneOffset = 0;
PFL
    ECode ec = CDate::New((IDate**)&date);
PFL
    assert( ec == NOERROR);
    ec = date->GetTimezoneOffset(&timezoneOffset);
    assert(ec == NOERROR);
    PFL_EX(" timezoneOffset: %d", timezoneOffset)
    assert(timezoneOffset == -480);
    return 0;
}
