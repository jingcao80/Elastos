#include <Elastos.CoreLibrary.h>
#include <elastos/core/StringBuilder.h>

#include <elastos/utility/etl/Pair.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/HashSet.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/etl/Vector.h>
#include <elastos/utility/etl/Stack.h>

using namespace Elastos;
using namespace Elastos::Core;
using namespace Elastos::Math;
using Elastos::Utility::Etl::MakePair;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::HashSet;
using Elastos::Utility::Etl::Pair;
using Elastos::Utility::Etl::Vector;
using Elastos::Utility::Etl::Stack;


#ifndef DEFINE_HASH_FUNC_FOR
#define DEFINE_HASH_FUNC_FOR(TypeName)                                                  \
_ETL_NAMESPACE_BEGIN                                 \
template<> struct Hash<TypeName *>                                                        \
{                                                                                       \
    size_t operator()(TypeName * s) const                                               \
    {                                                                                   \
        return (size_t)Object::GetHashCode(s);                                          \
    }                                                                                   \
};                                                                                      \
                                                                                        \
template<> struct Hash<AutoPtr<TypeName> >                                              \
{                                                                                       \
    size_t operator()(const AutoPtr<TypeName> & s) const                                \
    {                                                                                   \
        return (size_t)Object::GetHashCode(s.Get());                                    \
    }                                                                                   \
};                                                                                      \
                                                                                        \
template<> struct EqualTo<TypeName *>                                                   \
{                                                                                       \
    size_t operator()(TypeName * x, TypeName * y) const                                 \
    {                                                                                   \
        return (size_t)Object::Equals(x, y);                                            \
    }                                                                                   \
};                                                                                      \
                                                                                        \
template<> struct EqualTo<AutoPtr<TypeName> >                                           \
{                                                                                       \
    size_t operator()(const AutoPtr<TypeName> & x, const AutoPtr<TypeName> & y) const   \
    {                                                                                   \
        return (size_t)Object::Equals(x, y);                                            \
    }                                                                                   \
};                                                                                      \
                                                                                        \
_ETL_NAMESPACE_END
#endif // DEFINE_HASH_FUNC_FOR

DEFINE_HASH_FUNC_FOR(IBigInteger)

_ETL_NAMESPACE_BEGIN

// template<> struct Hash<IBigInteger*>
// {
//     size_t operator()(const IBigInteger* s) const { return (size_t)s; }
// };

// template<> struct Hash<AutoPtr<IBigInteger> >
// {
//     size_t operator()(const AutoPtr<IBigInteger> s) const { return (size_t)s.Get(); }
// };

// Hash functor for Pair<Int32, String>
template<> struct Hash< Pair<Int32, String> >
{
    size_t operator()(const Pair<Int32, String>& p) const
    {
        Int32 firstHashCode = p.mFirst;
        Hash<String> strHashFuctor;
        Int32 secondHashCode = strHashFuctor(p.mSecond);

        const Int32 prime = 31;
        Int32 result = 1;
        result = prime * result + firstHashCode;
        result = prime * result + secondHashCode;
        return result;
    }
};

// EqulaTo functor for Pair<Int32, String>
template<> struct EqualTo< Pair<Int32, String> >
{
    size_t operator()(const Pair<Int32, String>& x, const Pair<Int32, String>& y) const
    {
        if (x.mFirst != y.mFirst) {
            return FALSE;
        }

        if (!x.mSecond.Equals(y.mSecond)) {
            return FALSE;
        }

        return TRUE;
    }
};

_ETL_NAMESPACE_END

void testHashMap()
{
    printf("\n==== enter TestHashMap ====\n");
    {
        AutoPtr<IBigInteger> i1, i2;
        CBigInteger::New(1, 1, (IBigInteger**)&i1);
        CBigInteger::New(1, 2, (IBigInteger**)&i2);

        printf(" >> Base Map 1\n");

        typedef HashMap<String, IBigInteger* > HashMapType;
        typedef typename HashMapType::ValueType ValueType;
        HashMapType* baseMap = new HashMapType();
        baseMap->Insert(ValueType(String("2"), i2.Get()));
        baseMap->Clear();
        delete baseMap;
    }

    {
        AutoPtr<IBigInteger> i1, i2;
        CBigInteger::New(1, 1, (IBigInteger**)&i1);
        CBigInteger::New(1, 2, (IBigInteger**)&i2);

        printf(" >> Base Map 2\n");
        typedef HashMap<IBigInteger*, IBigInteger*> HashMapType;
        typedef typename HashMapType::ValueType ValueType;

        HashMapType* baseMap = new HashMapType();
        baseMap->Insert(ValueType(i1.Get(), i2.Get()));
        baseMap->Clear();
        delete baseMap;
    }

    {
        printf(" >> Interface Map\n");

        AutoPtr<IBigInteger> i1, i2;
        CBigInteger::New(1, 1, (IBigInteger**)&i1);
        CBigInteger::New(1, 2, (IBigInteger**)&i2);

        typedef HashMap<AutoPtr<IBigInteger>, AutoPtr<IBigInteger> > HashMapType;
        typedef typename HashMapType::ValueType ValueType;

        HashMapType* baseMap = new HashMapType();
        baseMap->Insert(ValueType(i1, i2));
        baseMap->Clear();
        delete baseMap;
    }

    {
        printf(" >> Int32/String Map\n");
        HashMap<Int32, String> map;
        typedef typename HashMap<Int32, String>::ValueType ValueType;
        for (Int32 i = 0; i < 10; ++i) {
            StringBuilder sb("value_");
            sb += i;
            map.Insert(ValueType(i, sb.ToString()));
        }

        HashMap<Int32, String>::Iterator it = map.Begin();
        for (; it != map.End();) {
            if (it->mFirst % 2 == 0) {
                map.Erase(it++);
                printf(" Erase ... %d %s\n", it->mFirst, (it->mSecond).string());
            }
            else {
                ++it;
            }
        }

        printf(" ===== Erase end()\n");
        for (it = map.Begin(); it != map.End(); ++it) {
            printf(" After Erase: %s\n", (it->mSecond).string());
        }
    }

    {
        printf(" >> String/Int32 Map\n");
        HashMap<String, Int32> map;
        typedef typename HashMap<String, Int32>::ValueType ValueType;
        for (Int32 i = 0; i < 10; ++i) {
            StringBuilder sb("value_");
            sb += i;
            printf("%d, %s\n", i, sb.ToString().string());
            map.Insert(ValueType(sb.ToString(), i));
        }

        String key("value_1");
        HashMap<String, Int32>::Iterator it = map.Begin();
        for (it = map.Begin(); it != map.End(); ++it) {
            if (it->mFirst == key) {
                printf("Found key: %s, %d\n", key.string(), it->mSecond);
                assert(it->mFirst.Equals(key));
                assert(it->mFirst.Equals("value_1"));
                break;
            }
        }

        it = map.Find(key);
        assert(it != map.End());
        printf("Found value_1, value: %d\n", it->mSecond);

        for (it = map.Begin(); it != map.End();) {
            if (it->mSecond % 2 == 0) {
                map.Erase(it++);
                printf(" Erase ... %d %s\n", it->mSecond, (it->mFirst).string());
            }
            else {
                ++it;
            }
        }

        printf(" ===== Erase end()\n");
        for (it = map.Begin(); it != map.End(); ++it) {
            printf(" After Erase: %d\n", it->mSecond);
        }
    }

    printf("\n==== leave TestHashMap ====\n");
}

void testHashMapWithPair()
{
    typedef Pair<Int32, String> Int32StringPair;

    HashMap<Int32StringPair, String> pairMap;
    typedef typename HashMap<Int32StringPair, String>::ValueType PairValueType;
    typedef typename HashMap<Int32StringPair, String>::Iterator PairIterator;

    for (Int32 i = 0; i < 10; ++i) {
        StringBuilder sbKey("Key_");
        sbKey += i;
        Int32StringPair pair = MakePair(i, sbKey.ToString());

        StringBuilder sbValue("Value_");
        sbValue += i;
        String value = sbValue.ToString();

        pairMap.Insert(PairValueType(pair, value));
    }

    PairIterator it;
    for (it = pairMap.Begin(); it != pairMap.End(); ++it) {
        Int32StringPair pair = it->mFirst;
        String value = it->mSecond;

        printf(" >> <%d, %s> : %s\n", pair.mFirst, pair.mSecond.string(), value.string());
    }
}

void testFind()
{
    AutoPtr<IBigInteger> i1, i2, i3, i4;
    CBigInteger::New(1, 1, (IBigInteger**)&i1);
    CBigInteger::New(1, 2, (IBigInteger**)&i2);
    CBigInteger::New(1, 3, (IBigInteger**)&i3);

    List<AutoPtr<IBigInteger> > list;
    list.PushBack(i1);
    list.PushBack(i2);
    list.PushBack(i3);

    List<AutoPtr<IBigInteger> >::Iterator it;
    it = Find(list.Begin(), list.End(), i1);
    if (it != list.End()) {
        printf(" Found i1 ...\n");
    }
    else {
        printf(" Not Found i1...\n");
    }

    i4 = i1;
    it = Find(list.Begin(), list.End(), i4);
    if (it != list.End()) {
        printf(" Found i4 ...\n");
    }
    else {
        printf(" Not Found i4...\n");
    }
}

void printList(List<Int32>& list, const char* info)
{
    printf("%s list values: ", info);
    List<Int32>::Iterator it;
    for (it = list.Begin(); it != list.End(); ++it)
    {
        printf("%d, ", *it);
    }
    printf("\n");
}

void printVector(Vector<Int32>& vector, const char* info)
{
    printf("%s Vector values: ", info);
    Vector<Int32>::Iterator it;
    for (it = vector.Begin(); it != vector.End(); ++it)
    {
        printf("%d, ", *it);
    }
    printf("\n");
}

void printHashMap(HashMap<Int32, Int32>& map, const char* info)
{
    printf("%s HashMap values: ", info);
    HashMap<Int32, Int32>::Iterator it;
    for (it = map.Begin(); it != map.End(); ++it)
    {
        printf("(%d, %d), ", it->mFirst, it->mSecond);
    }
    printf("\n");
}

void printHashSet(HashSet<Int32>& set, const char* info)
{
    printf("%s HashSet values: ", info);
    HashSet<Int32>::Iterator it;
    for (it = set.Begin(); it != set.End(); ++it)
    {
        printf("%d, ", *it);
    }
    printf("\n");
}

void testErase()
{
    //==================================================
    //    List
    //==================================================
    List<Int32> list;

    // init
    for (int i = 0; i < 5; ++i) {
        list.PushBack(i);
    }

    // erase with Iterator
    printList(list, "Before erase with Iterator");
    List<Int32>::Iterator it;
    for (it = list.Begin(); it != list.End();)
    {
        if (0 == (*it) % 2) {
            it = list.Erase(it);
        }
        else {
            ++it;
        }
    }
    printList(list, "After erase with Iterator");

    // reset
    list.Clear();
    for (int i = 0; i < 5; ++i) {
        list.PushBack(i);
    }

    // erase with ReverseIterator
    printList(list, "Before erase with ReverseIterator");
    List<Int32>::ReverseIterator rit;
    for (rit = list.RBegin(); rit != list.REnd();)
    {
        if (0 == (*rit) % 2) {
            rit = List<Int32>::ReverseIterator(list.Erase(--(rit.GetBase())));
            // rit = List<Int32>::ReverseIterator(list.Erase((++rit).GetBase()));
        }
        else {
            ++rit;
        }
    }
    printList(list, "After erase with ReverseIterator");

    //==================================================
    //    Vector
    //==================================================
    Vector<Int32> vector;

    // init
    for (int i = 0; i < 5; ++i) {
        vector.PushBack(i);
    }

    // erase with Iterator
    printVector(vector, "Before erase with Iterator");
    Vector<Int32>::Iterator vit;
    for (vit = vector.Begin(); vit != vector.End();)
    {
        if (0 == (*vit) % 2) {
            vit = vector.Erase(vit);
        }
        else {
            ++vit;
        }
    }
    printVector(vector, "After erase with Iterator");

    // reset
    vector.Clear();
    for (int i = 0; i < 5; ++i) {
        vector.PushBack(i);
    }

    // erase with ReverseIterator
    printVector(vector, "Before erase with ReverseIterator");
    Vector<Int32>::ReverseIterator vrit;
    for (vrit = vector.RBegin(); vrit != vector.REnd();)
    {
        if (0 == (*vrit) % 2) {
            vrit = Vector<Int32>::ReverseIterator(vector.Erase(--(vrit.GetBase())));
        }
        else {
            ++vrit;
        }
    }
    printVector(vector, "After erase with ReverseIterator");

    //==================================================
    //    HashMap
    //==================================================
    HashMap<Int32, Int32> map;

    // init
    for (int i = 0; i < 5; ++i) {
        map[i] = i;
    }

    // erase with Iterator
    printHashMap(map, "Before erase with Iterator");
    HashMap<Int32, Int32>::Iterator mit;
    for (mit = map.Begin(); mit != map.End();)
    {
        if (0 == mit->mFirst % 2) {
            map.Erase(mit++);
        }
        else {
            ++mit;
        }
    }
    printHashMap(map, "After erase with Iterator");

    //==================================================
    //    HashSet
    //==================================================
    HashSet<Int32> set;

    // init
    for (int i = 0; i < 5; ++i) {
        set.Insert(i);
    }

    // erase with Iterator
    printHashSet(set, "Before erase with Iterator");
    HashSet<Int32>::Iterator sit;
    for (sit = set.Begin(); sit != set.End();)
    {
        if (0 == (*sit) % 2) {
            set.Erase(sit++);
        }
        else {
            ++sit;
        }
    }
    printHashSet(set, "After erase with Iterator");
}

void testReverseFind()
{
    List<Int32> list;
    list.PushBack(0);
    list.PushBack(1);
    list.PushBack(2);
    list.PushBack(0);
    list.PushBack(3);
    list.PushBack(4);
    list.PushBack(5);

    List<Int32>::ReverseIterator rit = Find(list.RBegin(), list.REnd(), 0);
    List<Int32>::Iterator it = rit.GetBase();
    printf(" it: %d, ++it: %d\n", *it, *(++it));
}

void testStack()
{
    AutoPtr<Stack<Int32> > intStack = NULL;
    {
        intStack = new Stack<Int32>();
        for (Int32 i = 0; i < 10; ++i) {
            intStack->Push(i);
        }
        printf(" >> intStack 1 refcount : %d\n", intStack->GetRefCount());

        AutoPtr<Stack<Int32> > intStack2 = intStack;
        printf(" >> intStack 2 refcount : %d\n", intStack->GetRefCount());
    }

    printf(" >> intStack 3 refcount : %d\n", intStack->GetRefCount());
}

int main(int argc, char *argv[])
{
    printf("==== call testETL ====\n");

//    testHashMapWithPair();

    // testHashMap();

    // testFind();

    // testErase();

    // testReverseFind();

    testStack();

    printf("\n==== call testETL end ====\n");

    return 0;
}
