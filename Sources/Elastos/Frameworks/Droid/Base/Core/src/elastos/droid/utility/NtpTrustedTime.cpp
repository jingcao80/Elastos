
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/utility/NtpTrustedTime.h"

//#include "elastos/droid/provider/Settings.h"
//#include "elastos/droid/os/SystemClock.h"
//#include "elastos/droid/net/CSntpClient.h"

#include <elastos/core/AutoLock.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>
//#include "elastos/droid/R.h"

using Elastos::Utility::Logging::Logger;
//using Elastos::Droid::R;
// using Elastos::Droid::Content::Res::IResources;
// using Elastos::Droid::Content::IContentResolver;
// using Elastos::Droid::Provider::Settings;
// using Elastos::Droid::Provider::ISettingsGlobal;
// using Elastos::Droid::Net::ISntpClient;
// using Elastos::Droid::Net::CSntpClient;
//using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::Math;

namespace Elastos {
namespace Droid {
namespace Utility {

const String NtpTrustedTime::TAG("NtpTrustedTime");
const Boolean NtpTrustedTime::LOGD = FALSE;
AutoPtr<NtpTrustedTime> NtpTrustedTime::sSingleton;
Object NtpTrustedTime::mLock;

CAR_INTERFACE_IMPL_2(NtpTrustedTime, Object, INtpTrustedTime, ITrustedTime)

NtpTrustedTime::NtpTrustedTime(
    /* [in] */ const String& server,
    /* [in] */ Int64 timeout)
    : mServer(server)
    , mTimeout(timeout)
    , mHasCache(FALSE)
    , mCachedNtpTime(0L)
    , mCachedNtpElapsedRealtime(0L)
    , mCachedNtpCertainty(0L)
{
    if (LOGD) {
        Logger::D(TAG, "creating NtpTrustedTime using %s", server.string());
    }
}


ECode NtpTrustedTime::GetInstance(
    /* [in] */ IContext* context,
    /* [out] */ INtpTrustedTime** instance)
{
    VALIDATE_NOT_NULL(instance);

    AutoLock lock(mLock);
    if (sSingleton == NULL) {
        // AutoPtr<IResources> res;
        // context->GetResources((IResources**)&res);

        // AutoPtr<IContentResolver> resolver;
        // context->GetContentResolver((IContentResolver**)&resolver);

        // String defaultServer;
        // res->GetString(R::string::config_ntpServer, &defaultServer);
        // Int64 defaultTimeout;
        // res->GetInteger(R::integer::config_ntpTimeout, (Int32*)&defaultTimeout);

        // String secureServer;
        // FAIL_RETURN(Settings::Global::GetString(resolver, ISettingsGlobal::NTP_SERVER, &secureServer))
        // Int64 timeout;
        // Settings::Global::GetInt64(
        //         resolver, ISettingsGlobal::NTP_TIMEOUT, defaultTimeout, &timeout);

        // String server = secureServer != NULL ? secureServer : defaultServer;
        // sSingleton = new NtpTrustedTime(server, timeout);
    }
    *instance = (INtpTrustedTime*)sSingleton.Get();
    REFCOUNT_ADD(*instance);
    return NOERROR;
}

ECode NtpTrustedTime::ForceRefresh(
    /* [out] */  Boolean* isRefreshed)
{
    VALIDATE_NOT_NULL(isRefreshed);
    *isRefreshed = FALSE;

    if (TextUtils::IsEmpty(mServer)) {
        // missing server, so no trusted time available
        return NOERROR;
    }

    // AutoPtr<ISntpClient> client;
    // CSntpClient::New((ISntpClient**)&client);
    // Boolean hasTime;
    // client->RequestTime(mServer, (Int32) mTimeout, &hasTime);
    // if (hasTime) {
    //     mHasCache = TRUE;
    //     client->GetNtpTime(&mCachedNtpTime);
    //     client->GetNtpTimeReference(&mCachedNtpElapsedRealtime);
    //     Int64 tripTime;
    //     client->GetRoundTripTime(&tripTime);
    //     mCachedNtpCertainty = tripTime / 2;
    //     *isRefreshed = TRUE;
    // }

    if (LOGD) {
        if (*isRefreshed)
            Logger::D(TAG, "forceRefresh() has time CachedNtpCertainty %lld.", mCachedNtpCertainty);
        else
            Logger::D(TAG, "forceRefresh() has no time.");
    }

    return NOERROR;
}

ECode NtpTrustedTime::HasCache(
    /* [out] */  Boolean* hasCache)
{
    VALIDATE_NOT_NULL(hasCache);
    *hasCache = mHasCache;
    return NOERROR;
}

ECode NtpTrustedTime::GetCacheAge(
    /* [out] */  Int64* cacheAge)
{
    VALIDATE_NOT_NULL(cacheAge);
    // if (mHasCache) {
    //     *cacheAge = SystemClock::GetElapsedRealtime() - mCachedNtpElapsedRealtime;
    // }
    // else {
    //     *cacheAge = Elastos::Core::Math::INT64_MAX_VALUE;
    // }
    return NOERROR;
}

ECode NtpTrustedTime::GetCacheCertainty(
    /* [out] */  Int64* cacheCertainty)
{
    VALIDATE_NOT_NULL(cacheCertainty);
    if (mHasCache) {
        *cacheCertainty = mCachedNtpCertainty;
    }
    else {
        *cacheCertainty = Elastos::Core::Math::INT64_MAX_VALUE;
    }
    return NOERROR;
}

ECode NtpTrustedTime::GetCurrentTimeMillis(
    /* [out] */  Int64* currentTimeMillis)
{
    VALIDATE_NOT_NULL(currentTimeMillis);
    *currentTimeMillis = 0;

    if (!mHasCache) {
        Logger::E(TAG, "Missing authoritative time source");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (LOGD) {
        Logger::D(TAG, "currentTimeMillis() cache hit");
    }

    // current time is age after the last ntp cache; callers who
    // want fresh values will hit makeAuthoritative() first.
    GetCacheAge(currentTimeMillis);
    *currentTimeMillis += mCachedNtpTime;
    return NOERROR;
}

ECode NtpTrustedTime::GetCachedNtpTime(
    /* [out] */ Int64* cacheNtpTime)
{
    VALIDATE_NOT_NULL(cacheNtpTime);
    if (LOGD) {
        Logger::D(TAG, "getCachedNtpTime() cache hit");
    }
    *cacheNtpTime = mCachedNtpTime;
    return NOERROR;
}

ECode NtpTrustedTime::GetCachedNtpTimeReference(
    /* [out] */ Int64* cachedNtpTimeReference)
{
    VALIDATE_NOT_NULL(cachedNtpTimeReference);
    *cachedNtpTimeReference = mCachedNtpElapsedRealtime;
    return NOERROR;
}

} // namespace Utility
} // namespace Droid
} // namespace Elastos
