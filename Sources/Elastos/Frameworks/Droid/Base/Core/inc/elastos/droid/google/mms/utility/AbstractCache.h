#ifndef __ELASTOS_DROID_GOOGLE_MMS_UTILITY_ABSTRACTCACHE_H__
#define __ELASTOS_DROID_GOOGLE_MMS_UTILITY_ABSTRACTCACHE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

// import android.util.Log;
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
        Int32 hit;
        AutoPtr<IInterface> value;
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

