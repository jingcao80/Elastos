
#include "elastos/droid/internal/widget/CLockPatternUtilsCache.h"
#include "elastos/droid/utility/CArrayMap.h"
#include <elastos/core/AutoLock.h>

using Elastos::Droid::Utility::CArrayMap;
using Elastos::Core::AutoLock;
using Elastos::Core::CBoolean;
using Elastos::Core::CInteger64;
using Elastos::Core::CString;
using Elastos::Core::IBoolean;
using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger64;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

CAR_INTERFACE_IMPL(CLockPatternUtilsCache::CacheKey, Object, ILockPatternUtilsCacheCacheKey);
CLockPatternUtilsCache::CacheKey::CacheKey()
    : mUserId(0)
{}

AutoPtr<ILockPatternUtilsCacheCacheKey> CLockPatternUtilsCache::CacheKey::Set(
    /* [in] */ const String& key,
    /* [in] */ Int32 userId)
{
    mKey = key;
    mUserId = userId;
    return THIS_PROBE(ILockPatternUtilsCacheCacheKey);
}

AutoPtr<CLockPatternUtilsCache::CacheKey> CLockPatternUtilsCache::CacheKey::Copy()
{
    AutoPtr<CacheKey> key = new CacheKey();
    key->Set(mKey, mUserId);
    return key;
}

ECode CLockPatternUtilsCache::CacheKey::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* e)
{
    VALIDATE_NOT_NULL(e);
    if (!ILockPatternUtilsCacheCacheKey::Probe(obj)) {
        *e = FALSE;
        return NOERROR;
    }
    CacheKey* o = (CacheKey*) ILockPatternUtilsCacheCacheKey::Probe(obj);
    *e = mUserId == o->mUserId && mKey.Equals(o->mKey);
    return NOERROR;
}

ECode CLockPatternUtilsCache::CacheKey::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash);
    *hash = mKey.GetHashCode() ^ mUserId;
    return NOERROR;
}

const String CLockPatternUtilsCache::HAS_LOCK_PATTERN_CACHE_KEY("LockPatternUtils.Cache.HasLockPatternCacheKey");
const String CLockPatternUtilsCache::HAS_LOCK_PASSWORD_CACHE_KEY("LockPatternUtils.Cache.HasLockPasswordCacheKey");
AutoPtr<ILockPatternUtilsCache> CLockPatternUtilsCache::sInstance;

CAR_OBJECT_IMPL(CLockPatternUtilsCache)
CAR_INTERFACE_IMPL_2(CLockPatternUtilsCache, Object, ILockPatternUtilsCache, IILockSettings);
CLockPatternUtilsCache::CLockPatternUtilsCache()
{
    CArrayMap::New((IArrayMap**)&mCache);
    mCacheKey = new CacheKey();
    CLockPatternUtilsCacheObserver::New((IILockSettingsObserver**)&mObserver);
}

AutoPtr<ILockPatternUtilsCache> CLockPatternUtilsCache::GetInstance(
    /* [in] */ IILockSettings* service)
{
    if (sInstance == NULL) {
        CLockPatternUtilsCache::New(service, (ILockPatternUtilsCache**)&sInstance);
    }
    return sInstance;
}

ECode CLockPatternUtilsCache::constructor(
    /* [in] */ IILockSettings* service)
{
    mService = service;
    ECode ec = service->RegisterObserver(mObserver);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        // Not safe to do caching without the observer. System process has probably died
        // anyway, so crashing here is fine.
        // throw new RuntimeException(e);
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

ECode CLockPatternUtilsCache::SetBoolean(
    /* [in] */ const String& key,
    /* [in] */ Boolean value,
    /* [in] */ Int32 userId) /*throws RemoteException*/
{
    InvalidateCache(key, userId);
    FAIL_RETURN(mService->SetBoolean(key, value, userId));
    AutoPtr<IBoolean> v;
    CBoolean::New(value, (IBoolean**)&v);
    PutCache(key, userId, v);
    return NOERROR;
}

ECode CLockPatternUtilsCache::SetInt64(
    /* [in] */ const String& key,
    /* [in] */ Int64 value,
    /* [in] */ Int32 userId) /*throws RemoteException*/
{
    InvalidateCache(key, userId);
    FAIL_RETURN(mService->SetInt64(key, value, userId));
    AutoPtr<IInteger64> v;
    CInteger64::New(value, (IInteger64**)&v);
    PutCache(key, userId, v);
    return NOERROR;
}

ECode CLockPatternUtilsCache::SetString(
    /* [in] */ const String& key,
    /* [in] */ const String& value,
    /* [in] */ Int32 userId) /*throws RemoteException*/
{
    InvalidateCache(key, userId);
    FAIL_RETURN(mService->SetString(key, value, userId));
    AutoPtr<ICharSequence> v;
    CString::New(value, (ICharSequence**)&v);
    PutCache(key, userId, v);
    return NOERROR;
}

ECode CLockPatternUtilsCache::GetInt64(
    /* [in] */ const String& key,
    /* [in] */ Int64 defaultValue,
    /* [in] */ Int32 userId,
    /* [out] */ Int64* retValue) /*throws RemoteException*/
{
    VALIDATE_NOT_NULL(retValue);
    AutoPtr<IInterface> value = PeekCache(key, userId);
    if (IInteger64::Probe(value)) {
        return IInteger64::Probe(value)->GetValue(retValue);
    }
    Int64 result = 0;
    mService->GetInt64(key, defaultValue, userId, &result);
    AutoPtr<IInteger64> v;
    CInteger64::New(result, (IInteger64**)&v);
    PutCache(key, userId, v);
    *retValue = result;
    return NOERROR;
}

ECode CLockPatternUtilsCache::GetString(
    /* [in] */ const String& key,
    /* [in] */ const String& defaultValue,
    /* [in] */ Int32 userId,
    /* [out] */ String* retValue) /*throws RemoteException*/
{
    VALIDATE_NOT_NULL(retValue);
    AutoPtr<IInterface> value = PeekCache(key, userId);
    // if (value instanceof String) {
    //     return (String) value;
    // }
    if (ICharSequence::Probe(value)) {
        return ICharSequence::Probe(value)->ToString(retValue);
    }
    String result;
    mService->GetString(key, defaultValue, userId, &result);
    AutoPtr<ICharSequence> v;
    CString::New(result, (ICharSequence**)&v);
    PutCache(key, userId, v);
    *retValue = result;
    return NOERROR;
}

ECode CLockPatternUtilsCache::GetBoolean(
    /* [in] */ const String& key,
    /* [in] */ Boolean defaultValue,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* retValue) /*throws RemoteException*/
{
    VALIDATE_NOT_NULL(retValue);
    AutoPtr<IInterface> value = PeekCache(key, userId);
    if (IBoolean::Probe(value)) {
        return IBoolean::Probe(value)->GetValue(retValue);
    }
    Boolean result = FALSE;
    mService->GetBoolean(key, defaultValue, userId, &result);
    AutoPtr<IBoolean> v;
    CBoolean::New(result, (IBoolean**)&v);
    PutCache(key, userId, v);
    *retValue = result;
    return NOERROR;
}

ECode CLockPatternUtilsCache::GetLockPatternSize(
    /* [in] */ Int32 userId,
    /* [out] */ Byte* result) /*throws RemoteException*/
{
    return mService->GetLockPatternSize(userId, result);
}

ECode CLockPatternUtilsCache::SetLockPattern(
    /* [in] */ const String& pattern,
    /* [in] */ Int32 userId) /*throws RemoteException*/
{
    InvalidateCache(HAS_LOCK_PATTERN_CACHE_KEY, userId);
    FAIL_RETURN(mService->SetLockPattern(pattern, userId));
    AutoPtr<IBoolean> v;
    CBoolean::New(pattern != NULL, (IBoolean**)&v);
    PutCache(HAS_LOCK_PATTERN_CACHE_KEY, userId, v);
    return NOERROR;
}

ECode CLockPatternUtilsCache::CheckPattern(
    /* [in] */ const String& pattern,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* check) /*throws RemoteException*/
{
    VALIDATE_NOT_NULL(check);
    return mService->CheckPattern(pattern, userId, check);
}

ECode CLockPatternUtilsCache::SetLockPassword(
    /* [in] */ const String& password,
    /* [in] */ Int32 userId) /*throws RemoteException*/
{
    InvalidateCache(HAS_LOCK_PASSWORD_CACHE_KEY, userId);
    FAIL_RETURN(mService->SetLockPassword(password, userId));
    AutoPtr<IBoolean> v;
    CBoolean::New(password != NULL, (IBoolean**)&v);
    PutCache(HAS_LOCK_PASSWORD_CACHE_KEY, userId, v);
    return NOERROR;
}

ECode CLockPatternUtilsCache::CheckPassword(
    /* [in] */ const String& password,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* retValue) /*throws RemoteException*/
{
    VALIDATE_NOT_NULL(retValue);
    return mService->CheckPassword(password, userId, retValue);
}

ECode CLockPatternUtilsCache::CheckVoldPassword(
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* retValue) /*throws RemoteException*/
{
    VALIDATE_NOT_NULL(retValue);
    return mService->CheckVoldPassword(userId, retValue);
}

ECode CLockPatternUtilsCache::HavePattern(
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* retValue) /*throws RemoteException*/
{
    VALIDATE_NOT_NULL(retValue);
    AutoPtr<IInterface> value = PeekCache(HAS_LOCK_PATTERN_CACHE_KEY, userId);
    if (IBoolean::Probe(value)) {
        return IBoolean::Probe(value)->GetValue(retValue);
    }
    Boolean result = FALSE;
    mService->HavePattern(userId, &result);
    AutoPtr<IBoolean> v;
    CBoolean::New(result, (IBoolean**)&v);
    PutCache(HAS_LOCK_PATTERN_CACHE_KEY, userId, v);
    *retValue = result;
    return NOERROR;
}

ECode CLockPatternUtilsCache::HavePassword(
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* retValue) /*throws RemoteException*/
{
    VALIDATE_NOT_NULL(retValue);
    AutoPtr<IInterface> value = PeekCache(HAS_LOCK_PASSWORD_CACHE_KEY, userId);
    if (IBoolean::Probe(value)) {
        return IBoolean::Probe(value)->GetValue(retValue);
    }
    Boolean result = FALSE;
    mService->HavePassword(userId, &result);
    AutoPtr<IBoolean> v;
    CBoolean::New(result, (IBoolean**)&v);
    PutCache(HAS_LOCK_PASSWORD_CACHE_KEY, userId, v);
    *retValue = result;
    return NOERROR;
}

ECode CLockPatternUtilsCache::RemoveUser(
    /* [in] */ Int32 userId) /*throws RemoteException*/
{
    return mService->RemoveUser(userId);
}

ECode CLockPatternUtilsCache::RegisterObserver(
    /* [in] */ IILockSettingsObserver* observer) /*throws RemoteException*/
{
    return mService->RegisterObserver(observer);
}

ECode CLockPatternUtilsCache::UnregisterObserver(
    /* [in] */ IILockSettingsObserver* observer) /*throws RemoteException*/
{
    return mService->UnregisterObserver(observer);
}

AutoPtr<IInterface> CLockPatternUtilsCache::PeekCache(
    /* [in] */ const String& key,
    /* [in] */ Int32 userId)
{
    AutoLock lock(mCache);
    // Safe to reuse mCacheKey, because it is not stored in the map.
    AutoPtr<ILockPatternUtilsCacheCacheKey> k = mCacheKey->Set(key, userId);
    AutoPtr<IInterface> obj;
    mCache->Get(k, (IInterface**)&obj);
    return obj;
}

void CLockPatternUtilsCache::PutCache(
    /* [in] */ const String& key,
    /* [in] */ Int32 userId,
    /* [in] */ IInterface* value)
{
    synchronized (mCache) {
        // Create a new key, because this will be stored in the map.
        AutoPtr<CacheKey> cacheKey = new CacheKey();
        AutoPtr<ILockPatternUtilsCacheCacheKey> k = cacheKey->Set(key, userId);
        mCache->Put(k, value);
    }
}

void CLockPatternUtilsCache::InvalidateCache(
    /* [in] */ const String& key,
    /* [in] */ Int32 userId)
{
    synchronized (mCache) {
        // Safe to reuse mCacheKey, because it is not stored in the map.
        AutoPtr<ILockPatternUtilsCacheCacheKey> k = mCacheKey->Set(key, userId);
        mCache->Remove(k);
    }
}

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos
