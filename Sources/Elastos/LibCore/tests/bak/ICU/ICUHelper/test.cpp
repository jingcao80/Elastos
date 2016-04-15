#include "test.h"
#include <stdio.h>
#include <Elastos.CoreLibrary.h>
#include <elautoptr.h>
using namespace Elastos;
using namespace Elastos::Core;
//using namespace Elastos::Core;

static Int64 GetCurrentTimeMillis()
{
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int64 curTime;
    system->GetCurrentTimeMillis(&curTime);
    return curTime;
}

int CTest::test1(int argc, char* argv[])
{
//    ArrayOf<String> *languages;
//    AutoPtr<IICUUtil> pICUHelper;
//    ECode ec = NOERROR;
//    ec = CICUUtil::AcquireSingleton((IICUUtil**)&pICUHelper);
//
//    //Get ICUHelper
//    printf("========================================\n");
//    pICUHelper->GetISOLanguages(&languages);
//
//    for(Int32 count = 0; count < languages->GetLength(); count++) {
//        printf("the str is %s\n", (const char *)(*languages)[count]);
//    }
//
//    ArrayOf<String>::Free(languages);
//
//    String localName("jp_JPddd_TRANDITIONAL");
//    AutoPtr<ILocale> locale;
//    pICUHelper->LocaleFromString(localName, (ILocale**)&locale);
    return 0;
}

int CTest::test2(int argc, char* argv[])
{
//    ArrayOf<String> *str = ArrayOf<String>::Alloc(2);
//    (*str)[0] = String("jp_JP_TRADITION");
//    (*str)[1] = String("zh_ZH_FFHHFF");
//    printf("%s %d", __FILE__, __LINE__);
//    ArrayOf<ILocale*> *ppLocales;
//    AutoPtr<IICUUtil> pICUHelper;
//    ECode ec = NOERROR;
//    ec = CICUUtil::AcquireSingleton((IICUUtil**)&pICUHelper);
//
//    pICUHelper->LocalesFromStrings((*str), &ppLocales);
//    printf("***%s %d\n", __FILE__, __LINE__);
//    String strCountry;
//    (*ppLocales)[0]->GetLanguage(&strCountry);
//    printf("the languages is %s\n", (const char*)strCountry);
//    (*ppLocales)[1]->GetLanguage(&strCountry);
//    printf("the languages is %s\n", (const char*)strCountry);
//    ArrayOf<ILocale*>::Free(ppLocales);
//
//    pICUHelper->GetAvailableLocales(&ppLocales);
////    (*ppLocales)[0]->GetLanguage(&strCountry);
////    printf("the languages is %s\n", (const char*)strCountry);
////    (*ppLocales)[1]->GetLanguage(&strCountry);
////    printf("the languages is %s\n", (const char*)strCountry);
//    for(Int32 count = 0; count < ppLocales->GetLength(); count++) {
//        (*ppLocales)[count]->GetCountry(&strCountry);
//         printf("the str is %s\n", (const char *)strCountry);
//        (*ppLocales)[count]->GetVariant(&strCountry);
//        printf("the var is %s\n", (const char *)strCountry);
//    }
    return 0;
}



int CTest::test3(int argc, char* argv[])
{
    AutoPtr<IDate> date;
    ECode ec = NOERROR;
    ec = CDate::New((IDate**)&date);
    return 0;
}

int CTest::test4(int argc, char* argv[])
{
    String name;
    if (name.IsNull()) {
        printf("NULL **** NULL NULL **** NULL\n");
    }

    name = String("");

    if (name.IsEmpty()) {
        printf("IsEmpty **** IsEmpty IsEmpty **** IsEmpty\n");
    }
    return 0;
}


int CTest::test5(int argc, char* argv[])
{
    AutoPtr<IDate> pdate;
    printf("%s, %d\n", __FILE__, __LINE__);
    Int64 mli = GetCurrentTimeMillis();
    printf("mli is %lld\n", mli);

    CDate::New(GetCurrentTimeMillis(), (IDate **) &pdate);
    printf("%s, %d\n", __FILE__, __LINE__);
    Int32 da = 0;
    printf("%s, %d\n", __FILE__, __LINE__);
    pdate->GetDate(&da);
    printf("####################the date is %d\n", da);
    Int32 year = 0;
    pdate->GetYear(&year);
    printf("###############the year is %d\n", year);
    Int32 day = 0;
    pdate->GetDay(&day);
    printf("###############the day is %d\n", day);
    return 0;
}


int CTest::test6(int argc, char* argv[])
{
    AutoPtr<IDate> date1;
    AutoPtr<IDate> date2;

    CDate::New(GetCurrentTimeMillis(), (IDate **) &date1);
    Int32 count = 0;
    while(count < 10000) count++;

    CDate::New(GetCurrentTimeMillis(), (IDate **) &date2);

    Boolean isAfter, isBef;
    date1->IsAfter((IDate*)date2, &isAfter);
    printf("the date2 is after date1 is %d\n", isAfter);

    date1->IsBefore((IDate*)date2, &isBef);
    printf("the date2 is after date1 is %d\n", isBef);

    printf("OK!\n");
    return 0;
}

int CTest::test8(int argc, char* argv[])
{
    Int64 millis =  GetCurrentTimeMillis();
    printf("the init millis is  %lld\n", millis);
    AutoPtr<IGregorianCalendar> cal;
    CGregorianCalendar::New((IGregorianCalendar**)&cal);
    //cal->SetTimeInMillis(millis);
    AutoPtr<IDate> date;
    AutoPtr<IDate> pdate;
    Int32 year = 0, day = 0, month = 0, hour=0, minute = 0 ,second = 0;
    Int32 year1 = 0, day1 = 0, month1 = 0, hour1=0, minute1 = 0 ,second1 = 0;
    CDate::New(millis, (IDate**)&pdate);

    pdate->GetYear(&year);
    pdate->GetMonth(&month);
    pdate->GetDay(&day);
    pdate->GetHours(&hour);
    pdate->GetMinutes(&minute);
    pdate->GetSeconds(&second);
    printf("before cal%d %d %d %d %d %d\n\n",year,month,day,hour,minute,second);
    cal->SetEx3(year,month,day,hour,minute,second);
    cal->GetTime((IDate**)&date);

    date->GetYear(&year1);
    date->GetMonth(&month1);
    date->GetDay(&day1);
    date->GetHours(&hour1);
    date->GetMinutes(&minute1);
    date->GetSeconds(&second1);
    printf(" after cal%d %d %d %d %d %d\n\n",year1,month1,day1,hour1,minute1,second1);

    Int64 ti;
    cal->GetTimeInMillis(&ti);
    printf("the cal millis is  %lld\n", ti);
//    cal->SetEx3(year1,month,day,hour,minute,second);
    return 0;
}

class A
{
public:
    A() {i=0;}
public:
    void fun() {printf("A fun\n");
        printf("the is is %d\n", i);
        }
    virtual void fun1() {printf("A fun1\n" ); i++;}
private:
    virtual void fun2() {printf("A fun2\n"); i++;}

public:
    int i;
};

class B : public A
{
public:
    void fun() {
        printf("----------------------------------fun\n");
        A::fun();
        printf("B fun\n");
        }
    virtual void fun1() {
        printf("----------------------------------fun1\n");
        //A::fun1();
        printf("B fun1\n");
        i++;
        printf("the is is %d\n", i);
        }
};



int CTest::test7(int argc, char* argv[])
{
    A* a;
    B b;
    a = &b;
    a->fun();
    a->fun1();

    b.fun();
    b.fun1();
}
