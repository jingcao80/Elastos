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

#include "elastos/droid/systemui/statusbar/phone/SystemUIDialog.h"
#include "../R.h"

using Elastos::Droid::SystemUI::R;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

CAR_INTERFACE_IMPL(SystemUIDialog, AlertDialog, ISystemUIDialog)
ECode SystemUIDialog::constructor(
    /* [in] */ IContext* context)
{
    AlertDialog::constructor(context, R::style::Theme_SystemUI_Dialog);
    mContext = context;

    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    window->SetType(IWindowManagerLayoutParams::TYPE_STATUS_BAR_PANEL);
    window->AddFlags(IWindowManagerLayoutParams::FLAG_ALT_FOCUSABLE_IM
            | IWindowManagerLayoutParams::FLAG_SHOW_WHEN_LOCKED);
    AutoPtr<IWindowManagerLayoutParams> attrs;
    window->GetAttributes((IWindowManagerLayoutParams**)&attrs);
    attrs->SetTitle(/*getClass().*/GetSimpleName());
    window->SetAttributes(attrs);
    return NOERROR;
}

ECode SystemUIDialog::SetShowForAllUsers(
    /* [in] */ Boolean show)
{
    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    AutoPtr<IWindowManagerLayoutParams> attrs;
    window->GetAttributes((IWindowManagerLayoutParams**)&attrs);
    Int32 privateFlags = 0;
    attrs->GetPrivateFlags(&privateFlags);
    if (show) {
        privateFlags |= IWindowManagerLayoutParams::PRIVATE_FLAG_SHOW_FOR_ALL_USERS;
    }
    else {
        privateFlags &= ~IWindowManagerLayoutParams::PRIVATE_FLAG_SHOW_FOR_ALL_USERS;
    }
    attrs->SetPrivateFlags(privateFlags);
    return NOERROR;
}

ECode SystemUIDialog::SetMessage(
    /* [in] */ Int32 resId)
{
    String str;
    mContext->GetString(resId, &str);
    AutoPtr<ICharSequence> seq;
    CString::New(str, (ICharSequence**)&seq);
    return SetMessage(seq);
}

ECode SystemUIDialog::SetPositiveButton(
    /* [in] */ Int32 resId,
    /* [in] */ IDialogInterfaceOnClickListener* onClick)
{
    String str;
    mContext->GetString(resId, &str);
    AutoPtr<ICharSequence> seq;
    CString::New(str, (ICharSequence**)&seq);
    return SetButton(IDialogInterface::BUTTON_POSITIVE, seq, onClick);
}

ECode SystemUIDialog::SetNegativeButton(
    /* [in] */ Int32 resId,
    /* [in] */ IDialogInterfaceOnClickListener* onClick)
{
    String str;
    mContext->GetString(resId, &str);
    AutoPtr<ICharSequence> seq;
    CString::New(str, (ICharSequence**)&seq);
    return SetButton(IDialogInterface::BUTTON_NEGATIVE, seq, onClick);
}

AutoPtr<ICharSequence> SystemUIDialog::GetSimpleName()
{
    AutoPtr<ICharSequence> name;
    CString::New(String("SystemUIDialog"), (ICharSequence**)&name);
    return name;
}

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
