
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Telephony.h"
#include "elastos/providers/downloadprovider/downloads/RealSystemFacade.h"
#include "elastos/providers/downloadprovider/downloads/Constants.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IDownloadManagerHelper;
using Elastos::Droid::App::CDownloadManagerHelper;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::IConnectivityManagerHelper;
using Elastos::Droid::Net::CConnectivityManagerHelper;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::ITelephonyManagerHelper;
//using Elastos::Droid::Telephony::CTelephonyManagerHelper;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Providers {
namespace DownloadProvider {
namespace Downloads {

CAR_INTERFACE_IMPL(RealSystemFacade, Object, ISystemFacade)

RealSystemFacade::RealSystemFacade(
    /* [in] */ IContext* context)
{
    mContext = context;
}

ECode RealSystemFacade::GetCurrentTimeMillis(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    return sys->GetCurrentTimeMillis(result);
}

ECode RealSystemFacade::GetActiveNetworkInfo(
    /* [in] */ Int32 uid,
    /* [out] */ INetworkInfo** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IInterface> sr;
    mContext->GetSystemService(IContext::CONNECTIVITY_SERVICE, (IInterface**)&sr);
    AutoPtr<IConnectivityManager> connectivity = IConnectivityManager::Probe(sr);
    if (connectivity == NULL) {
        Logger::W(Constants::TAG, "couldn't get connectivity manager");
        *result = NULL;
        return NOERROR;
    }

    AutoPtr<INetworkInfo> activeInfo;
    connectivity->GetActiveNetworkInfoForUid(uid, (INetworkInfo**)&activeInfo);
    if (activeInfo == NULL && Constants::LOGVV) {
        Logger::V(Constants::TAG, "network is not available");
    }
    *result = activeInfo;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode RealSystemFacade::IsActiveNetworkMetered(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IConnectivityManagerHelper> hlp;
    CConnectivityManagerHelper::AcquireSingleton((IConnectivityManagerHelper**)&hlp);
    AutoPtr<IConnectivityManager> conn;
    hlp->From(mContext, (IConnectivityManager**)&conn);
    return conn->IsActiveNetworkMetered(result);
}

ECode RealSystemFacade::IsNetworkRoaming(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IInterface> sr;
    mContext->GetSystemService(IContext::CONNECTIVITY_SERVICE, (IInterface**)&sr);
    AutoPtr<IConnectivityManager> connectivity = IConnectivityManager::Probe(sr);
    if (connectivity == NULL) {
        Logger::W(Constants::TAG, "couldn't get connectivity manager");
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<INetworkInfo> info;
    connectivity->GetActiveNetworkInfo((INetworkInfo**)&info);
    Int32 type = 0;
    info->GetType(&type);
    Boolean isMobile = (info != NULL && type == IConnectivityManager::TYPE_MOBILE);

    AutoPtr<ITelephonyManagerHelper> hlp;
    assert(0 && "TODO");
    // CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&hlp);
    AutoPtr<ITelephonyManager> tm;
    hlp->GetDefault((ITelephonyManager**)&tm);
    Boolean bRoaming = FALSE;
    tm->IsNetworkRoaming(&bRoaming);
    Boolean isRoaming = isMobile && bRoaming;
    if (Constants::LOGVV && isRoaming) {
        Logger::V(Constants::TAG, "network is roaming");
    }
    *result = isRoaming;
    return NOERROR;
}

ECode RealSystemFacade::GetMaxBytesOverMobile(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IDownloadManagerHelper> hlp;
    CDownloadManagerHelper::AcquireSingleton((IDownloadManagerHelper**)&hlp);
    return hlp->GetMaxBytesOverMobile(mContext, result);
}

ECode RealSystemFacade::GetRecommendedMaxBytesOverMobile(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IDownloadManagerHelper> hlp;
    CDownloadManagerHelper::AcquireSingleton((IDownloadManagerHelper**)&hlp);
    return hlp->GetRecommendedMaxBytesOverMobile(mContext, result);
}

ECode RealSystemFacade::SendBroadcast(
    /* [in] */ IIntent* intent)
{
    mContext->SendBroadcast(intent);
    return NOERROR;
}

ECode RealSystemFacade::UserOwnsPackage(
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IApplicationInfo> info;
    pm->GetApplicationInfo(packageName, 0, (IApplicationInfo**)&info);
    Int32 muid = 0;
    info->GetUid(&muid);
    *result = muid == uid;
    return NOERROR;
}

} // namespace Downloads
} // namespace DownloadProvider
} // namespace Providers
} // namespace Elastos
