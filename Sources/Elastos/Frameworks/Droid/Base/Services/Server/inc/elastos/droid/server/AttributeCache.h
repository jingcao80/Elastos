//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_SERVER_ATTRIBUTECACHE_H__
#define __ELASTOS_DROID_SERVER_ATTRIBUTECACHE_H__

#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.Droid.Content.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/Arrays.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Utility::Arrays;
using Elastos::Utility::Etl::HashMap;

#ifndef HASH_EQUALTO_FUNC_FOR_AUTOPTR_INT32ARRAY
#define HASH_EQUALTO_FUNC_FOR_AUTOPTR_INT32ARRAY
_ETL_NAMESPACE_BEGIN
template<> struct Hash<AutoPtr<ArrayOf<Int32> > >
{
    size_t operator()(const AutoPtr<ArrayOf<Int32> > s) const {
        Int32 len = s != NULL ? s->GetLength() : 0;
        size_t hash = 0;
        for (Int32 i = 0; i < len; i++)
            hash += 31 * hash + (*s)[i];
        return hash;
    }
};

template<> struct EqualTo<AutoPtr<ArrayOf<Int32> > >
{
    size_t operator()(const AutoPtr<ArrayOf<Int32> > x, const AutoPtr<ArrayOf<Int32> > y) const
    {
        assert(x != NULL);
        return Arrays::Equals(x, y);
    }
};
_ETL_NAMESPACE_END
#endif

namespace Elastos {
namespace Droid {
namespace Server {

class Entry
    : public Object
{
public:
    Entry(
        /* [in] */ IContext* c,
        /* [in] */ ITypedArray* ta);

    IContext* mContext;
    AutoPtr<ITypedArray> mArray;
};

typedef HashMap<AutoPtr<ArrayOf<Int32> >, AutoPtr<Entry> > EntryMap;
typedef EntryMap::Iterator EntryMapIterator;

typedef HashMap<Int32, AutoPtr<EntryMap> > PackageMap;
typedef PackageMap::Iterator PackageMapIterator;

class AttributeCache
    : public Object
{
public:
    class Package
        : public Object
    {
    public:
        Package(
            /* [in] */ IContext* c);

        ~Package();

        AutoPtr<IContext> mContext;

    private:
        PackageMap mMap;

        friend class AttributeCache;
    };


public:
    static CARAPI_(void) Init(
        /* [in] */ IContext* context);

    static CARAPI_(AutoPtr<AttributeCache>) GetInstance();

    AttributeCache(
        /* [in] */ IContext* context);

    ~AttributeCache();

    CARAPI_(void) UpdateConfiguration(
        /* [in] */ IConfiguration* config);

    // CARAPI RemoveUser(
    //     /* [in] */ Int32 userId);

    AutoPtr<Entry> Get(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 resId,
        /* [in] */ ArrayOf<Int32>* styleable,
        /* [in] */ Int32 userId);

    CARAPI_(void) RemovePackage(
        /* [in] */ const String& packageName);

private:
    static AutoPtr<AttributeCache> sInstance;
    AutoPtr<IContext> mContext;
    // WeakHashMap<String, Package>
    HashMap<String, AutoPtr<Package> > mPackages;
    AutoPtr<IConfiguration> mConfiguration;
};


}// Server
}// Droid
}// Elastos

#endif // __ELASTOS_DROID_SERVER_ATTRIBUTECACHE_H__
