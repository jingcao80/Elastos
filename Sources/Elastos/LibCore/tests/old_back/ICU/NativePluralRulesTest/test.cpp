#include "test.h"
#include <stdio.h>
#include <Elastos.CoreLibrary.h>
#include <elautoptr.h>

using namespace Elastos;
using namespace Elastos::Core;

CTest::CTest()
{
    CNativePluralRulesHelper::AcquireSingleton((INativePluralRulesHelper **)&nprhep);
}

int CTest::testEnglish(int argc, char* argv[]) {
    AutoPtr<ILocale> usloc;
    CLocale::New(String("en") , String("US") , (ILocale **)&usloc);
    nprhep->ForLocale(usloc , (INativePluralRules **) &npr);
    Int32 value;
    npr->QuantityForInt(-1,&value);
    assert(INativePluralRules::OTHER == value);
    npr->QuantityForInt(0,&value);
    assert(INativePluralRules::OTHER == value);
    npr->QuantityForInt(1,&value);
    assert(INativePluralRules::ONE == value);
    npr->QuantityForInt(2,&value);
    assert(INativePluralRules::OTHER == value);
}

int CTest::testCzech(int argc, char* argv[]) {
    AutoPtr<ILocale> usloc;
    CLocale::New(String("cs") , String("CZ") , (ILocale **)&usloc);
    nprhep->ForLocale(usloc , (INativePluralRules **) &npr);
    Int32 value;
    npr->QuantityForInt(-1,&value);
    assert(INativePluralRules::OTHER == value);
    npr->QuantityForInt(0,&value);
    assert(INativePluralRules::OTHER == value);
    npr->QuantityForInt(1,&value);
    assert(INativePluralRules::ONE == value);
    npr->QuantityForInt(2,&value);
    assert(INativePluralRules::FEW == value);
    npr->QuantityForInt(3,&value);
    assert(INativePluralRules::FEW == value);
    npr->QuantityForInt(4,&value);
    assert(INativePluralRules::FEW == value);
    npr->QuantityForInt(5,&value);
    assert(INativePluralRules::OTHER == value);
}

int CTest::testArabic(int argc, char* argv[]) {
    AutoPtr<ILocale> usloc;
    CLocale::New(String("ar") , (ILocale **)&usloc);
    nprhep->ForLocale(usloc , (INativePluralRules **) &npr);
    Int32 value;
    npr->QuantityForInt(-1,&value);
    assert(INativePluralRules::OTHER == value);
    npr->QuantityForInt(0,&value);
    assert(INativePluralRules::ZERO == value);
    npr->QuantityForInt(1,&value);
    assert(INativePluralRules::ONE == value);
    npr->QuantityForInt(2,&value);
    assert(INativePluralRules::TWO == value);

    for (int i = 3; i <= 10; ++i) {
        npr->QuantityForInt(i,&value);
        assert(INativePluralRules::FEW == value);
    }
    npr->QuantityForInt(11,&value);
    assert(INativePluralRules::MANY == value);
    npr->QuantityForInt(99,&value);
    assert(INativePluralRules::MANY == value);
    npr->QuantityForInt(100,&value);
    assert(INativePluralRules::OTHER == value);
    npr->QuantityForInt(101,&value);
    assert(INativePluralRules::OTHER == value);
    npr->QuantityForInt(102,&value);
    assert(INativePluralRules::OTHER == value);
    npr->QuantityForInt(103,&value);
    assert(INativePluralRules::FEW == value);
    npr->QuantityForInt(111,&value);
    assert(INativePluralRules::MANY == value);
}
