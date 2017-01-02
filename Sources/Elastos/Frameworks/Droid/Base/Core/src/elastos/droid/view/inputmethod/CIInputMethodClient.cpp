//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
