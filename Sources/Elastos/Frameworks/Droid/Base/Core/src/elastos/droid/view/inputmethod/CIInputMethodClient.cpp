
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/inputmethod/CIInputMethodClient.h"

using Elastos::Droid::Internal::View::EIID_IInputMethodClient;
using Elastos::Droid::Os::EIID_IBinder;

namespace Elastos {
namespace Droid {
namespace View {
namespace InputMethod {

//========================================================================================
//              CIInputMethodClient::
//========================================================================================
CAR_INTERFACE_IMPL_2(CIInputMethodClient, Object, IInputMethodClient, IBinder)

CAR_OBJECT_IMPL(CIInputMethodClient)

ECode CIInputMethodClient::constructor(
    /* [in] */ IInputMethodManager* host)
{
    mHost = (CInputMethodManager*)host;
    return NOERROR;
}

ECode CIInputMethodClient::SetUsingInputMethod(
    /* [in] */ Boolean state)
{
    return NOERROR;
}

ECode CIInputMethodClient::OnBindMethod(
    /* [in] */ IInputBindResult* res)
{
    AutoPtr<IMessage> msg;
    mHost->mH->ObtainMessage(CInputMethodManager::MSG_BIND,
        res, (IMessage**)&msg);
    Boolean result;
    return mHost->mH->SendMessage(msg, &result);
}

ECode CIInputMethodClient::OnUnbindMethod(
    /* [in] */ Int32 sequence)
{
    AutoPtr<IMessage> msg;
    mHost->mH->ObtainMessage(CInputMethodManager::MSG_UNBIND,
        sequence, 0, (IMessage**)&msg);
    Boolean result;
    return mHost->mH->SendMessage(msg, &result);
}

ECode CIInputMethodClient::SetActive(
    /* [in] */ Boolean active)
{
    AutoPtr<IMessage> msg;
    mHost->mH->ObtainMessage(CInputMethodManager::MSG_SET_ACTIVE,
        active ? 1 : 0, 0, (IMessage**)&msg);
    Boolean result;
    return mHost->mH->SendMessage(msg, &result);
}

ECode CIInputMethodClient::ToString(
    /* [out] */ String* str)
{
    return E_NOT_IMPLEMENTED;
}

ECode CIInputMethodClient::SetUserActionNotificationSequenceNumber(
    /* [in] */ Int32 sequenceNumber)
{
    AutoPtr<IMessage> msg;
    mHost->mH->ObtainMessage(mHost->MSG_SET_USER_ACTION_NOTIFICATION_SEQUENCE_NUMBER,
            sequenceNumber, 0, (IMessage**)&msg);
    Boolean result;
    return mHost->mH->SendMessage(msg, &result);
}

} // namespace InputMethod
} // namespace View
} // namespace Droid
} // namespace Elastos
