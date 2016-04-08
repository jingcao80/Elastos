#ifndef __OLDABSTRACTMAPTEST_H__
#define __OLDABSTRACTMAPTEST_H__

#include "Elastos.CoreLibrary.h"
#include <elastos.h>
#include <elautoptr.h>
#include <elquintet.h>

using namespace Elastos;
using namespace Elastos::Core;
using namespace Elastos::Utility;
using Elastos::Utility::AbstractMap;

#define TEST(a, x) a.test_##x

class CTest
{
public:
    CTest();
    ~CTest();

public:
    int test_Constructor(int argc, char* argv[]);
    int test_equalsLjava_lang_Object(int argc, char* argv[]);
    int test_hashCode(int argc, char* argv[]);
    int test_isEmpty(int argc, char* argv[]);
    int test_put(int argc, char* argv[]);
    int test_size(int argc, char* argv[]);
    int test_toString(int argc, char* argv[]);

    // class AMT : public AbstractMap
    // {
    // public:
    //     CARAPI_(PInterface) Probe(
    //         /* [in] */ REIID riid);

    //     CARAPI Put(
    //         /* [in] */ PInterface key,
    //         /* [in] */ PInterface value,
    //         /* [out] */ PInterface* oldValue);

    //     CARAPI EntrySet(
    //         /* [out] */ ISet** entries);

    // private:
    //     AutoPtr< ArrayOf<IEntry*> > mEntries; // = new ArrayList<Entry<String, String>>();

    // };
};

#endif // __OLDABSTRACTMAPTEST_H__