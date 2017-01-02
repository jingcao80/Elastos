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

#ifndef __ELASTOS_DROID_SERVER_AM_LOCKTOAPPREQUESTDIALOG_H__
#define __ELASTOS_DROID_SERVER_AM_LOCKTOAPPREQUESTDIALOG_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/server/am/TaskRecord.h"

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Internal::Widget::IILockSettings;
using Elastos::Droid::View::IWindowManagerPolicy;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::Widget::ICheckBox;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class CActivityManagerService;

class LockToAppRequestDialog
    : public Object
    , public IDialogInterfaceOnClickListener
{
public:
    CAR_INTERFACE_DECL()

    LockToAppRequestDialog(
        /* [in] */ IContext* context,
        /* [in] */ CActivityManagerService* activityManagerService);

    virtual CARAPI ClearPrompt();

    virtual CARAPI ShowLockTaskPrompt(
        /* [in] */ TaskRecord* task);

    // @Override
    CARAPI OnClick(
        /* [in] */ IDialogInterface* dialog,
        /* [in] */ Int32 which);

private:
    CARAPI_(AutoPtr<IILockSettings>) GetLockSettings();

    CARAPI_(Int32) GetLockString(
        /* [in] */ Int32 userId);

private:
    static const String TAG;
    AutoPtr<IContext> mContext;
    CActivityManagerService* mService;
    AutoPtr<IWindowManagerPolicy> mPolicy;
    AutoPtr<IAlertDialog> mDialog;
    AutoPtr<TaskRecord> mRequestedTask;
    AutoPtr<ICheckBox> mCheckbox;
    AutoPtr<IILockSettings> mLockSettingsService;
    AutoPtr<IAccessibilityManager> mAccessibilityService;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_AM_LOCKTOAPPREQUESTDIALOG_H__

