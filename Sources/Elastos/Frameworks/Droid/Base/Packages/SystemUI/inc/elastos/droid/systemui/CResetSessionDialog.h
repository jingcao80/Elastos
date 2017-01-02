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

#ifndef  __ELASTOS_DROID_SYSTEMUI_CRESETSESSIONDIALOG_H__
#define  __ELASTOS_DROID_SYSTEMUI_CRESETSESSIONDIALOG_H__

#include "_Elastos_Droid_SystemUI_CResetSessionDialog.h"
#include "elastos/droid/systemui/statusbar/phone/SystemUIDialog.h"

using Elastos::Droid::SystemUI::StatusBar::Phone::SystemUIDialog;

namespace Elastos {
namespace Droid {
namespace SystemUI {

CarClass(CResetSessionDialog)
    , public SystemUIDialog
    , public IDialogInterfaceOnClickListener
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CResetSessionDialog();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 userId);

    // @Override
    CARAPI OnClick(
        /* [in] */ IDialogInterface* dialog,
        /* [in] */ Int32 which);

private:
    static const Int32 BUTTON_WIPE;
    static const Int32 BUTTON_DONTWIPE;

    Int32 mUserId;
};

} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_CRESETSESSIONDIALOG_H__
