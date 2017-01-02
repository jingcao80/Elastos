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

#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_TASKVIEWFOOTER_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_TASKVIEWFOOTER_H__

#include "elastos/droid/systemui/recents/RecentsConfiguration.h"
#include <elastos/droid/widget/FrameLayout.h>

using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Widget::FrameLayout;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Views {

/** The task footer view */
class TaskViewFooter
    : public FrameLayout
    , public ITaskViewFooter
{
public:
    TaskViewFooter();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    /** Sets the callbacks for when the footer height changes. */
    CARAPI SetCallbacks(
        /* [in] */ ITaskFooterViewCallbacks* cb);

    /** Sets the footer height. */
    CARAPI SetFooterHeight(
        /* [in] */ Int32 footerHeight);

    /** Gets the footer height. */
    CARAPI GetFooterHeight(
        /* [out] */ Int32* height);

    /** Animates the footer into and out of view. */
    CARAPI AnimateFooterVisibility(
        /* [in] */ Boolean visible,
        /* [in] */ Int32 duration);

public:
    AutoPtr<RecentsConfiguration> mConfig;

    AutoPtr<ITaskFooterViewCallbacks> mCb;
    Int32 mFooterHeight;
    Int32 mMaxFooterHeight;
    AutoPtr<IAnimator> mFooterAnimator; //IObjectAnimator
};

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_TASKVIEWFOOTER_H__
