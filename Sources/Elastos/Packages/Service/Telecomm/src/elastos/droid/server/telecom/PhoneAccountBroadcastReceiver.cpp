
#include "elastos/droid/server/telecom/PhoneAccountBroadcastReceiver.h"
#include "elastos/droid/server/telecom/CallsManager.h"
#include <Elastos.Droid.Net.h>

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

CAR_INTERFACE_IMPL(PhoneAccountBroadcastReceiver, BroadcastReceiver, IPhoneAccountBroadcastReceiver)

ECode PhoneAccountBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_PACKAGE_FULLY_REMOVED.Equals(action)) {
        AutoPtr<IUri> uri;
        intent->GetData((IUri**)&uri);
        if (uri == NULL) {
            return NOERROR;
        }
        String packageName;
        uri->GetSchemeSpecificPart(&packageName);
        HandlePackageRemoved(context, packageName);
    }
    return NOERROR;
}

ECode PhoneAccountBroadcastReceiver::HandlePackageRemoved(
    /* [in] */ IContext* context,
    /* [in] */ const String& packageName)
{
    AutoPtr <CallsManager> callsManager = CallsManager::GetInstance();
    if (callsManager != NULL) {
        AutoPtr<PhoneAccountRegistrar> phoneAccountRegistrar;
        callsManager->GetPhoneAccountRegistrar((PhoneAccountRegistrar**)&phoneAccountRegistrar);
        phoneAccountRegistrar->ClearAccounts(packageName);
    }
    return NOERROR;
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
