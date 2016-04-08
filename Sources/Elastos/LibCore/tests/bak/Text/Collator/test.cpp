#include "test.h"
#include <stdio.h>

#include "Elastos.CoreLibrary.h"
#include <elautoptr.h>
#include <elastos/StringBuffer.h>

using namespace Elastos;
using namespace Elastos::Core;
using namespace Elastos::Text;
using namespace Libcore::ICU;

int CTest::test_setStrength(int argc, char* argv[])
{
    printf("%s %d\n", __FILE__, __LINE__);
    AutoPtr<ICollatorHelper> collatorHelper;
    PRINT_FILE_LINE
    CCollatorHelper::AcquireSingleton((ICollatorHelper**)&collatorHelper);
    PRINT_FILE_LINE
    AutoPtr<ICollator> collator;
    collatorHelper->GetInstance((ICollator**)&collator);
    collator->SetStrength(ICollator::PRIMARY);
    PRINT_FILE_LINE
    Int32 strength = 0;
    collator->GetStrength(&strength);
    assert(ICollator::PRIMARY == strength);
    collator->SetStrength(ICollator::SECONDARY);
    collator->GetStrength(&strength);
    assert(ICollator::SECONDARY == strength);
    collator->SetStrength(ICollator::TERTIARY);
    collator->GetStrength(&strength);
    assert(ICollator::TERTIARY == strength);
    collator->SetStrength(ICollator::IDENTICAL);
    collator->GetStrength(&strength);
    assert(ICollator::IDENTICAL == strength);
    collator->SetStrength(-1);
    collator->GetStrength(&strength);
    printf("strength=%d,IllegalArgumentException was not thrown.\n", strength);
    return 0;
}

int CTest::test_stackCorruption(int argc, char* argv[])
{
    AutoPtr<ICollatorHelper> collatorHelper;
    CCollatorHelper::AcquireSingleton((ICollatorHelper**)&collatorHelper);
    AutoPtr<ICollator> mColl;
    collatorHelper->GetInstance((ICollator**)&mColl);
    mColl->SetStrength(ICollator::PRIMARY);
    AutoPtr<ICollationKey> icu;
    mColl->GetCollationKey(
        String("2d294f2d3739433565147655394f3762f3147312d3731641452f310"),
        (ICollationKey**)&icu);
    return 0;
}

int CTest::test_collationKeySize(int argc, char* argv[])
{
    StringBuffer b("");
    for (Int32 i = 0; i < 1024; i++) {
        b += "0123456789ABCDEF";
    }
    String sixteen ;
    b.SubstringEx(0, b.GetLength(), &sixteen);
    b += "_THE_END";
    String sixteenplus;
    b.SubstringEx(0, b.GetLength(), &sixteenplus);
    AutoPtr<ICollatorHelper> collatorHelper;
    CCollatorHelper::AcquireSingleton((ICollatorHelper**)&collatorHelper);
    AutoPtr<ICollator> mColl;
    collatorHelper->GetInstance((ICollator**)&mColl);

    mColl->SetStrength(ICollator::PRIMARY);
    ArrayOf<Byte> * arr;
    AutoPtr<ICollationKey> icu;
    mColl->GetCollationKey(sixteen, (ICollationKey**)&icu);
    icu->ToByteArray(&arr);
    Int32 len = arr->GetLength();
    assert((*arr)[len - 1] == 0);// Collation key not 0 terminated
    len--;
    StringBuffer sb("");
    for (Int32 i = 0; i < len; i++) {
        sb += (*arr)[i];
    }
    String foo ;

    sb.SubstringEx(0, len, &foo);
    mColl->GetCollationKey(sixteen, (ICollationKey**)&icu);
    icu->ToByteArray(&arr);
    len = arr->GetLength();
    assert((*arr)[len - 1] == 0);// Collation key not 0 terminated
    len--;
    StringBuffer sb2("");
    for (Int32 i = 0; i < len; i++) {
        sb2 += (*arr)[i];
    }

    String bar ;
    sb2.SubstringEx(0, len, &bar);
    Int32 result = foo.Equals(bar);
    if (result  != 0) {
        printf("Collation keys should differ\n");
    }
    return 0;
}

int CTest::test_decompositionCompatibility(int argc, char* argv[])
{
    AutoPtr<ICollatorHelper> collatorHelper;
    CCollatorHelper::AcquireSingleton((ICollatorHelper**)&collatorHelper);
    AutoPtr<ICollator> myCollator;
    collatorHelper->GetInstance((ICollator**)&myCollator);
    myCollator->SetDecomposition(ICollator::NO_DECOMPOSITION);
    Int32 value(0);
    myCollator->CompareEx(String("\u00e0\u0325"), String("a\u0325\u0300"), &value);
    if (value == 0) {
        printf("Error: \u00e0\u0325 should not equal to a\u0325\u0300 without decomposition");
        assert(0);
    }
    myCollator->SetDecomposition(ICollator::CANONICAL_DECOMPOSITION);
    myCollator->CompareEx(String("\u00e0\u0325"), String("a\u0325\u0300"), &value);
    if (value != 0) {
        printf("Error: \u00e0\u0325 should equal to a\u0325\u0300 with decomposition");
        assert(0);
    }
    return 0;
}

int CTest::test_EqualsObject(int argc, char* argv[])
{
    String rule("< a < b < c < d < e");
    AutoPtr<IRuleBasedCollator> coll;
    CRuleBasedCollator::New(rule, (IRuleBasedCollator**)&coll);

    Int32 value(0);
    coll->GetStrength(&value);
    assert(ICollator::TERTIARY == value);

    AutoPtr<IRuleBasedCollator> other;
    CRuleBasedCollator::New(rule, (IRuleBasedCollator**)&other);

    coll->SetStrength(ICollator::PRIMARY);
    assert(coll != other);

    coll->SetStrength(ICollator::TERTIARY);
    coll->SetDecomposition(ICollator::CANONICAL_DECOMPOSITION);
    other->SetDecomposition(ICollator::NO_DECOMPOSITION);
    assert(coll != other);
    return 0;
}

int CTest::test_Harmony(int argc, char* argv[])
{
    String rule("< a< b< c< d");
    AutoPtr<IRuleBasedCollator> coll;
    CRuleBasedCollator::New(rule, (IRuleBasedCollator**)&coll);
    AutoPtr<ICollationElementIterator> cei;
    coll->GetCollationElementIteratorEx((ICharacterIterator*)NULL,
        (ICollationElementIterator**)&cei);
    printf("NullPointerException expected\n");

    return 0;
}
