
#include "CGeckoConnectivityReceiver.h"
#include "GeckoApp.h"
#include "GeckoAppShell.h"

using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Net::IConnectivityManager;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace Snowfox {

ECode CGeckoConnectivityReceiver::OnReceive(
    /* [in] */ IContext *pContext,
    /* [in] */ IIntent *pIntent)
{
    String status;
    ECode ec;
    AutoPtr<INetworkInfo> pInfo;
    AutoPtr<IConnectivityManager> pCm;
    ec = pContext->GetSystemService(
        IContext::CONNECTIVITY_SERVICE, (IInterface**)&pCm);
    if (FAILED(ec)) return ec;

    ec = pCm->GetActiveNetworkInfo((INetworkInfo**)&pInfo);
    if (FAILED(ec)) return ec;

    if (!pInfo) {
        status = "unknown";
    }
    else {
        Boolean isConnected;
        ec = pInfo->IsConnected(&isConnected);
        if (FAILED(ec)) return ec;

        if (!isConnected) {
            status = "down";
        }
        else {
            status = "up";
        }
    }

    if (GeckoApp::CheckLaunchState(GeckoApp::LaunchState_GeckoRunning)) {
        GeckoAppShell::OnChangeNetworkLinkStatus(status);
    }
    return NOERROR;
}

} // namespace Snowfox
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
