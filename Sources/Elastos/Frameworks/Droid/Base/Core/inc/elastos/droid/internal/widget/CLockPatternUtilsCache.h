
#ifndef __ELASTOS_DROID_WIDGET_INTERNAL_CLOCKPATTERNUTILSCACHE_H__
#define __ELASTOS_DROID_WIDGET_INTERNAL_CLOCKPATTERNUTILSCACHE_H__

#include "_Elastos_Droid_Internal_Widget_CLockPatternUtilsCache.h"
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/internal/widget/CLockPatternUtilsCacheObserver.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Utility::IArrayMap;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

/**
 * A decorator for {@link ILockSettings} that caches the key-value responses in memory.
 *
 * Specifically, the return values of {@link #getString(String, String, Int32)},
 * {@link #getLong(String, Int64, Int32)} and {@link #getBoolean(String, Boolean, Int32)} are cached.
 */
CarClass(CLockPatternUtilsCache)
    , public Object
    , public ILockPatternUtilsCache
    , public IILockSettings
{
private:
    class CacheKey
        : public Object
        , public ILockPatternUtilsCacheCacheKey
    {
    public:
        CAR_INTERFACE_DECL()

        CacheKey();

        CARAPI_(AutoPtr<ILockPatternUtilsCacheCacheKey>) Set(
            /* [in] */ const String& key,
            /* [in] */ Int32 userId);

        CARAPI_(AutoPtr<CacheKey>) Copy();

        // @Override
        CARAPI Equals(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* e);

        // @Override
        CARAPI GetHashCode(
            /* [out] */ Int32* hash);

    public:
        String mKey;
        Int32 mUserId;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CLockPatternUtilsCache();

    static CARAPI_(AutoPtr<ILockPatternUtilsCache>) GetInstance(
        /* [in] */ IILockSettings* service);

    // ILockSettings
    CARAPI constructor(
        /* [in] */ IILockSettings* service);

    CARAPI SetBoolean(
        /* [in] */ const String& key,
        /* [in] */ Boolean value,
        /* [in] */ Int32 userId) /*throws RemoteException*/;

    CARAPI SetInt64(
        /* [in] */ const String& key,
        /* [in] */ Int64 value,
        /* [in] */ Int32 userId) /*throws RemoteException*/;

    CARAPI SetString(
        /* [in] */ const String& key,
        /* [in] */ const String& value,
        /* [in] */ Int32 userId) /*throws RemoteException*/;

    CARAPI GetInt64(
        /* [in] */ const String& key,
        /* [in] */ Int64 defaultValue,
        /* [in] */ Int32 userId,
        /* [out] */ Int64* retValue) /*throws RemoteException*/;

    CARAPI GetString(
        /* [in] */ const String& key,
        /* [in] */ const String& defaultValue,
        /* [in] */ Int32 userId,
        /* [out] */ String* retValue) /*throws RemoteException*/;

    CARAPI GetBoolean(
        /* [in] */ const String& key,
        /* [in] */ Boolean defaultValue,
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* retValue) /*throws RemoteException*/;

    CARAPI GetLockPatternSize(
        /* [in] */ Int32 userId,
        /* [out] */ Byte* result) /*throws RemoteException*/;

    // @Override
    CARAPI SetLockPattern(
        /* [in] */ const String& pattern,
        /* [in] */ Int32 userId) /*throws RemoteException*/;

    // @Override
    CARAPI CheckPattern(
        /* [in] */ const String& pattern,
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* check) /*throws RemoteException*/;

    // @Override
    CARAPI SetLockPassword(
        /* [in] */ const String& password,
        /* [in] */ Int32 userId) /*throws RemoteException*/;

    // @Override
    CARAPI CheckPassword(
        /* [in] */ const String& password,
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* retValue) /*throws RemoteException*/;

    // @Override
    CARAPI CheckVoldPassword(
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* retValue) /*throws RemoteException*/;

    // @Override
    CARAPI HavePattern(
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* retValue) /*throws RemoteException*/;

    // @Override
    CARAPI HavePassword(
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* retValue) /*throws RemoteException*/;

    // @Override
    CARAPI RemoveUser(
        /* [in] */ Int32 userId) /*throws RemoteException*/;

    // @Override
    CARAPI RegisterObserver(
        /* [in] */ IILockSettingsObserver* observer) /*throws RemoteException*/;

    // @Override
    CARAPI UnregisterObserver(
        /* [in] */ IILockSettingsObserver* observer) /*throws RemoteException*/;

    // @Override
    // public IBinder AsBinder() {
    //     return mService.asBinder();
    // }

private:
    // Caching
    CARAPI_(AutoPtr<IInterface>) PeekCache(
        /* [in] */ const String& key,
        /* [in] */ Int32 userId);

    CARAPI_(void) PutCache(
        /* [in] */ const String& key,
        /* [in] */ Int32 userId,
        /* [in] */ IInterface* value);

    CARAPI_(void) InvalidateCache(
        /* [in] */ const String& key,
        /* [in] */ Int32 userId);

private:
    friend class CLockPatternUtilsCacheObserver;

    static const String HAS_LOCK_PATTERN_CACHE_KEY;
    static const String HAS_LOCK_PASSWORD_CACHE_KEY;

    static AutoPtr<ILockPatternUtilsCache> sInstance;

    AutoPtr<IILockSettings> mService;

    /** Only access when holding {@code mCache} lock. */
    AutoPtr<IArrayMap> mCache;/*<CacheKey, Object*/

    /** Only access when holding {@link #mCache} lock. */
    AutoPtr<CacheKey> mCacheKey;

    AutoPtr<IILockSettingsObserver> mObserver;
};

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_INTERNAL_CLOCKPATTERNUTILSCACHE_H__
