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

#ifndef  __ELASTOS_DROID_LAUNCHER2_CPAGEDVIEWICON_H__
#define  __ELASTOS_DROID_LAUNCHER2_CPAGEDVIEWICON_H__

#include "_Elastos_Droid_Launcher2_CPagedViewIcon.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Runnable.h>
#include "elastos/droid/widget/TextView.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Utility.h"

using Elastos::Droid::Content::IContext;
using Elastos::Core::Runnable;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Widget::TextView;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

/**
 * An icon on a PagedView, specifically for items in the launcher's paged view (with compound
 * drawables on the top).
 */
CarClass(CPagedViewIcon)
    , public TextView
    , public IPagedViewIcon
{
private:
    class MyRunnable
        : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ CPagedViewIcon* host);

        CARAPI Run();

    private:
        AutoPtr<CPagedViewIcon> mHost;
    };

public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    CPagedViewIcon();

    CARAPI ApplyFromApplicationInfo(
        /* [in] */ IApplicationInfo* info,
        /* [in] */ Boolean scaleUp,
        /* [in] */ IPagedViewIconPressedCallback* cb);

    CARAPI LockDrawableState();

    CARAPI ResetDrawableState();

protected:
    CARAPI DrawableStateChanged();

private:
    static const String TAG;
    static const Float PRESS_ALPHA;

    AutoPtr<IPagedViewIconPressedCallback> mPressedCallback;
    Boolean mLockDrawableState;

    AutoPtr<IBitmap> mIcon;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_CPAGEDVIEWICON_H__