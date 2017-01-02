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

#include "elastos/droid/systemui/CResetSessionDialog.h"
#include "elastos/droid/systemui/CGuestResumeSessionReceiver.h"
#include "R.h"

using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace SystemUI {

const Int32 CResetSessionDialog::BUTTON_WIPE = IDialogInterface::BUTTON_NEGATIVE;
const Int32 CResetSessionDialog::BUTTON_DONTWIPE = IDialogInterface::BUTTON_POSITIVE;

CAR_OBJECT_IMPL(CResetSessionDialog)
CAR_INTERFACE_IMPL(CResetSessionDialog, SystemUIDialog, IDialogInterfaceOnClickListener)
CResetSessionDialog::CResetSessionDialog()
    : mUserId(0)
{}

ECode CResetSessionDialog::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 userId)
{
    SystemUIDialog::constructor(context);

    String str;
    context->GetString(R::string::guest_wipe_session_title, &str);
    AutoPtr<ICharSequence> seq;
    CString::New(str, (ICharSequence**)&seq);
    SetTitle(seq);

    context->GetString(R::string::guest_wipe_session_message, &str);
    seq = NULL;
    CString::New(str, (ICharSequence**)&seq);
    SetMessage(seq);
    SetCanceledOnTouchOutside(FALSE);

    context->GetString(R::string::guest_wipe_session_wipe, &str);
    seq = NULL;
    CString::New(str, (ICharSequence**)&seq);
    SetButton(BUTTON_WIPE, seq, this);

    context->GetString(R::string::guest_wipe_session_dontwipe, &str);
    seq = NULL;
    CString::New(str, (ICharSequence**)&seq);
    SetButton(BUTTON_DONTWIPE, seq, this);

    mUserId = userId;
    return NOERROR;
}

ECode CResetSessionDialog::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    if (which == BUTTON_WIPE) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        CGuestResumeSessionReceiver::WipeGuestSession(context, mUserId);
        Dismiss();
    }
    else if (which == BUTTON_DONTWIPE) {
        Cancel();
    }
    return NOERROR;
}

} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
