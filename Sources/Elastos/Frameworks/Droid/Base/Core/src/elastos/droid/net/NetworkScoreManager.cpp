
#include "elastos/droid/net/NetworkScoreManager.h"
#include "elastos/droid/Manifest.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/net/Network.h"
#include "elastos/droid/net/NetworkScorerAppManager.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/UserHandle.h"

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::ServiceManager;

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(NetworkScoreManager, Object, INetworkScoreManager)

ECode NetworkScoreManager::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    AutoPtr<IBinder> iBinder = IBinder::Probe(ServiceManager::GetService(IContext::NETWORK_SCORE_SERVICE));
    mService = IINetworkScoreService::Probe(iBinder);
    return NOERROR;
}

ECode NetworkScoreManager::GetActiveScorerPackage(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<INetworkScorerAppData> app;
    NetworkScorerAppManager::GetActiveScorer(mContext, (INetworkScorerAppData**)&app);
    if (app == NULL) {
        *result = String(NULL);
        return NOERROR;
    }
    return app->GetPackageName(result);
}

ECode NetworkScoreManager::UpdateScores(
    /* [in] */ ArrayOf<IScoredNetwork*>* networks,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

        // try {
    ECode ec = mService->UpdateScores(networks, result);
        // } catch (RemoteException e) {
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        *result = FALSE;
        return NOERROR;
    }
        // }
    return ec;
}

ECode NetworkScoreManager::ClearScores(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

        // try {
    ECode ec = mService->ClearScores(result);
        // } catch (RemoteException e) {
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        *result = FALSE;
        return NOERROR;
    }
        // }
    return ec;
}

ECode NetworkScoreManager::SetActiveScorer(
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

        // try {
    ECode ec = mService->SetActiveScorer(packageName, result);
        // } catch (RemoteException e) {
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        *result = FALSE;
        return NOERROR;
    }
        // }
    return ec;
}

ECode NetworkScoreManager::DisableScoring()
{
        // try {
    ECode ec = mService->DisableScoring();
        // } catch (RemoteException e) {
    if (ec == (ECode)E_REMOTE_EXCEPTION) return NOERROR;
        // }
    return ec;
}

ECode NetworkScoreManager::RequestScores(
    /* [in] */ ArrayOf<INetworkKey*>* networks,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    String activeScorer;
    GetActiveScorerPackage(&activeScorer);
    if (activeScorer == String(NULL)) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<IIntent> intent;
    CIntent::New(ACTION_SCORE_NETWORKS, (IIntent**)&intent);
    intent->SetPackage(activeScorer);
    intent->SetFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    Int32 size = networks->GetLength();
    AutoPtr<ArrayOf<IParcelable*> > parcelables = ArrayOf<IParcelable*>::Alloc(size);
    for (Int32 i = 0; i < size; ++i) {
        parcelables->Set(i, IParcelable::Probe((*networks)[i]));
    }
    intent->PutExtra(EXTRA_NETWORKS_TO_SCORE, parcelables);
    // A scorer should never become active if its package doesn't hold SCORE_NETWORKS, but
    // ensure the package still holds it to be extra safe.
    mContext->SendBroadcastAsUser(intent, UserHandle::OWNER, Manifest::permission::SCORE_NETWORKS);
    *result = TRUE;
    return NOERROR;
}

ECode NetworkScoreManager::RegisterNetworkScoreCache(
    /* [in] */ Int32 networkType,
    /* [in] */ IINetworkScoreCache* scoreCache)
{
        // try {
    ECode ec = mService->RegisterNetworkScoreCache(networkType, scoreCache);
        // } catch (RemoteException e) {
    if (ec == (ECode)E_REMOTE_EXCEPTION) return NOERROR;
        // }
    return ec;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
