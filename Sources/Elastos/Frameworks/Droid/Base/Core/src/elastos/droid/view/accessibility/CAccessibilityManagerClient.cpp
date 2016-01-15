
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/view/accessibility/CAccessibilityManagerClient.h"

using Elastos::Droid::Os::EIID_IBinder;

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

CAR_INTERFACE_IMPL_2(CAccessibilityManagerClient, Object, IIAccessibilityManagerClient, IBinder)

CAR_OBJECT_IMPL(CAccessibilityManagerClient)

CAccessibilityManagerClient::CAccessibilityManagerClient()
{}

CAccessibilityManagerClient::~CAccessibilityManagerClient()
{}

ECode CAccessibilityManagerClient::constructor(
    /* [in] */ IAccessibilityManager* host)
{
    mHost = (CAccessibilityManager*)host;
    return NOERROR;
}

ECode CAccessibilityManagerClient::SetState(
    /* [in] */ Int32 stateFlags)
{
    // We do not want to change this immediately as the applicatoin may
    // have already checked that accessibility is on and fired an event,
    // that is now propagating up the view tree, Hence, if accessibility
    // is now off an exception will be thrown. We want to have the exception
    // enforcement to guard against apps that fire unnecessary accessibility
    // events when accessibility is off.
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(
            CAccessibilityManager::MyHandler::MSG_SET_STATE, stateFlags, 0, (IMessage**)&msg);
    Boolean result;
    return mHost->mHandler->SendMessage(msg, &result);
}

ECode CAccessibilityManagerClient::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = "CAccessibilityManagerClient";
    return NOERROR;
}

} // Accessibility
} // View
} // Droid
} // Elastos
