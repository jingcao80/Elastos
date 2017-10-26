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

#ifndef __ELASTOS_DROID_PROVIDERS_DOWNLOADS_CSIZELIMITACTIVITY_H__
#define __ELASTOS_DROID_PROVIDERS_DOWNLOADS_CSIZELIMITACTIVITY_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Providers_Downloads_CSizeLimitActivity.h"
#include "elastos/droid/app/Activity.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnCancelListener;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Utility::IQueue;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Downloads {

/**
 * Activity to show dialogs to the user when a download exceeds a limit on download sizes for
 * mobile networks.  This activity gets started by the background download service when a download's
 * size is discovered to be exceeded one of these thresholds.
 */
CarClass(CSizeLimitActivity)
    , public Activity
    , public ISizeLimitActivity
{
private:
    class InnerListener
        : public Object
        , public IDialogInterfaceOnCancelListener
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerListener(
            /* [in] */ CSizeLimitActivity* host);

        CARAPI OnCancel(
            /* [in] */ IDialogInterface* dialog);

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);
    private:
        CSizeLimitActivity* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CSizeLimitActivity();

    CARAPI OnCancel(
        /* [in] */ IDialogInterface* dialog);

    CARAPI OnClick(
        /* [in] */ IDialogInterface* dialog,
        /* [in] */ Int32 which);

    CARAPI OnNewIntent(
        /* [in] */ IIntent* intent);

    CARAPI OnResume();

private:
    CARAPI_(void) ShowNextDialog();

    using Activity::ShowDialog;

    CARAPI_(void) ShowDialog(
        /* [in] */ ICursor* cursor);

    CARAPI_(void) DialogClosed();

private:
    AutoPtr<IDialog> mDialog;
    AutoPtr<IQueue> mDownloadsToShow;
    AutoPtr<IUri> mCurrentUri;
    AutoPtr<IIntent> mCurrentIntent;
    AutoPtr<InnerListener> mListener;
};

} // namespace Downloads
} // namespace Providers
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_PROVIDERS_DOWNLOADS_CSIZELIMITACTIVITY_H__
