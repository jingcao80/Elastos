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

#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CSYSTEMUIDIALOG_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CSYSTEMUIDIALOG_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Phone_CSystemUIDialog.h"
#include "elastos/droid/systemui/statusbar/phone/SystemUIDialog.h"

using Elastos::Droid::App::AlertDialog;
using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

/**
 * Base class for dialogs that should appear over panels and keyguard.
 */
CarClass(CSystemUIDialog)
    , public SystemUIDialog
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CSYSTEMUIDIALOG_H__
