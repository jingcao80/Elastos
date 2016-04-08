
#define __USE_MALLOC

#include <elastos.h>
#include <elastos/HashMap.h>
#include <stdio.h>
#include <elautoptr.h>
#include <elrefbase.h>

using namespace Elastos;

struct Eqstr
{
    Boolean operator()(const char* s1, const char* s2) const
    {
        return strcmp(s1, s2) == 0;
    }
};

void testHashMap1()
{
    HashMap<const char*, Int32, Hash<const char*>, Eqstr> days;

    days["january"] = 31;
    days["february"] = 28;
    days["march"] = 31;
    days["april"] = 30;
    days["may"] = 31;
    days["june"] = 30;
    days["july"] = 31;
    days["august"] = 31;
    days["september"] = 30;
    days["october"] = 31;
    days["november"] = 30;
    days["december"] = 31;

    printf("september -> %d\n", days["september"]);
    printf("june -> %d\n", days["june"]);
    printf("february -> %d\n", days["february"]);
    printf("december -> %d\n", days["december"]);

    HashMap<const char*, Int32, Hash<const char*>, Eqstr>::Iterator it1 = days.Begin();
    HashMap<const char*, Int32, Hash<const char*>, Eqstr>::Iterator it2 = days.End();

    for(; it1 != it2; ++it1) printf("%s ", it1->mFirst);
    printf("\n");
}

struct Eqstring
{
    Boolean operator()(String s1, String s2) const
    {
        return s1.Compare(s2) == 0;
    }
};

void testHashMap2()
{
    HashMap<const char*, Int32, Hash<const char*>, Eqstr> days;

    days["january"] = 31;
    days["february"] = 28;
    days["march"] = 31;
    days["april"] = 30;
    days["may"] = 31;
    days["june"] = 30;
    days["july"] = 31;
    days["august"] = 31;
    days["september"] = 30;
    days["october"] = 31;
    days["november"] = 30;
    days["december"] = 31;

    printf("september -> %d\n", days["september"]);
    printf("june -> %d\n", days["june"]);
    printf("february -> %d\n", days["february"]);
    printf("december -> %d\n", days["december"]);

    HashMap<const char*, Int32, Hash<const char*>, Eqstr>::Iterator it1 = days.Begin();
    HashMap<const char*, Int32, Hash<const char*>, Eqstr>::Iterator it2 = days.End();

    for(; it1 != it2; ++it1) printf("%s ", it1->mFirst);
    printf("\n");
}

class A
{
public:
    A() { printf("==== Call A() ====\n"); };

    ~A() { printf("==== Call ~A() ====\n"); }
};

class B : public ElRefBase
{
public:
    B() { printf("==== Call B() ====\n"); };

    ~B() { printf("==== Call ~B() ====\n"); }

    CARAPI_(PInterface) Probe(
        /* [in]  */ REIID riid)
    {
        return NULL;
    }

    CARAPI_(UInt32) AddRef()
    {
        return ElRefBase::AddRef();
    }

    CARAPI_(UInt32) Release()
    {
        return ElRefBase::Release();
    }

    CARAPI GetInterfaceID(
        /* [in] */ IInterface *pObject,
        /* [out] */ InterfaceID *pIID)
    {
        return E_NOT_IMPLEMENTED;
    }
};

void testHashMap3()
{
    printf("======= Call testHashMap3 Begin =======\n");
    printf("======= test A Begin =======\n");
    HashMap<const char*, A*, Hash<const char*>, Eqstr> as;

    as["A1"] = new A;
    as["A2"] = new A;

    HashMap<const char*, A*, Hash<const char*>, Eqstr>::Iterator it = as.Begin();
    for (; it != as.End();) {
        delete it->mSecond;
        as.Erase(it++);
    }

    printf("======= test A End =======\n");
    printf("======= test B Begin =======\n");
    HashMap<const char*, AutoPtr<B>, Hash<const char*>, Eqstr> bs;

    bs["B1"] = new B;
    bs["B2"] = new B;
    printf("======= test B End =======\n");
    printf("======= Call testHashMap3 End =======\n");
}

int main(int argc, char *argv[])
{
//    testHashMap1();
//    testHashMap2();
    testHashMap3();

    return 0;
}
