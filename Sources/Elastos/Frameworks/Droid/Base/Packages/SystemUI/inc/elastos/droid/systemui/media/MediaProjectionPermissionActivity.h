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


#ifndef __ELASTOS_DROID_SYSTEMUI_MEDIA_MEDIAPROJECTIONPERMISSIONACTIVITY_H__
#define __ELASTOS_DROID_SYSTEMUI_MEDIA_MEDIAPROJECTIONPERMISSIONACTIVITY_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/droid/internal/app/AlertActivity.h>

using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Internal::App::AlertActivity;
using Elastos::Droid::Media::Projection::IIMediaProjectionManager;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::ICompoundButtonOnCheckedChangeListener;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Media {

class MediaProjectionPermissionActivity
    : public AlertActivity
{
private:
    class InnerListener
        : public Object
        , public IDialogInterfaceOnClickListener
        , public ICompoundButtonOnCheckedChangeListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerListener(
            /* [in] */ MediaProjectionPermissionActivity* host);

        // @Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

        // @Override
        CARAPI OnCheckedChanged(
            /* [in] */ ICompoundButton* buttonView,
            /* [in] */ Boolean isChecked);

    private:
        MediaProjectionPermissionActivity* mHost;
    };

public:

    MediaProjectionPermissionActivity();

    CARAPI constructor();

    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    // @Override
    CARAPI OnClick(
        /* [in] */ IDialogInterface* dialog,
        /* [in] */ Int32 which);

    // @Override
    CARAPI OnCheckedChanged(
        /* [in] */ ICompoundButton* buttonView,
        /* [in] */ Boolean isChecked);

private:
    CARAPI_(AutoPtr<IIntent>) GetMediaProjectionIntent(
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName,
        /* [in] */ Boolean permanentGrant);

private:
    const static String TAG;

    Boolean mPermanentGrant;
    String mPackageName;
    Int32 mUid;
    AutoPtr<IIMediaProjectionManager> mService;
};

} // namespace Media
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_MEDIA_MEDIAPROJECTIONPERMISSIONACTIVITY_H__

