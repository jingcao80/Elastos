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

#ifndef __ELASTOS_DROID_GOOGLE_MMS_UTILITY_ABSTRACTCACHE_H__
#define __ELASTOS_DROID_GOOGLE_MMS_UTILITY_ABSTRACTCACHE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Utility {

class AbstractCache
    : public Object
    , public IAbstractCache
{
private:
    class CacheEntry
        : public Object
    {
    public:
        Int32 mHit;
        AutoPtr<IInterface> mValue;
    };

public:
    CAR_INTERFACE_DECL();

    virtual CARAPI Put(
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value,
        /* [out] */ Boolean* result);

    virtual CARAPI Get(
        /* [in] */ IInterface* key,
        /* [out] */ IInterface** result);

    virtual CARAPI Purge(
        /* [in] */ IInterface* key,
        /* [out] */ IInterface** result);

    virtual CARAPI PurgeAll();

    virtual CARAPI Size(
        /* [out] */ Int32* result);

protected:
    AbstractCache();

private:
    static const String TAG;
    static const Boolean DEBUG;
    static const Boolean LOCAL_LOGV;
    static const Int32 MAX_CACHED_ITEMS = 500;
    /*const*/  AutoPtr<IHashMap> mCacheMap;// K, CacheEntry<V>
};

} // namespace Utility
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GOOGLE_MMS_UTILITY_ABSTRACTCACHE_H__

