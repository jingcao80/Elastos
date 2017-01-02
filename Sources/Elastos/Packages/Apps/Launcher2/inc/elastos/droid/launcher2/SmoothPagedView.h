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

#ifndef  __ELASTOS_DROID_LAUNCHER2_SMOOTHPAGEDVIEW_H__
#define  __ELASTOS_DROID_LAUNCHER2_SMOOTHPAGEDVIEW_H__

#include "_Elastos.Droid.Launcher2.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/launcher2/PagedView.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::Animation::IInterpolator;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class SmoothPagedView
    : public PagedView
{
public:
    class OvershootInterpolator
        : public Object
        , public IInterpolator
        , public ITimeInterpolator
    {
    public:
        CAR_INTERFACE_DECL();

        OvershootInterpolator();

        CARAPI_(void) SetDistance(
            /* [in] */ Int32 distance);

        CARAPI_(void) DisableSettle();

        CARAPI GetInterpolation(
            /* [in] */ Float t,
            /* [out] */ Float* result);

        // @Override
        CARAPI HasNativeInterpolator(
            /* [out] */ Boolean* res);

    private:
        static const Float DEFAULT_TENSION;
        Float mTension;
    };

public:
    TO_STRING_IMPL("SmoothPagedView")

    SmoothPagedView();

    /**
     * Used to inflate the Workspace from XML.
     *
     * @param context The application's context.
     * @param attrs The attributes set containing the Workspace's customization values.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    /**
     * Used to inflate the Workspace from XML.
     *
     * @param context The application's context.
     * @param attrs The attributes set containing the Workspace's customization values.
     * @param defStyle Unused.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    // @Override
    CARAPI ComputeScroll();

protected:
    CARAPI_(Int32) GetScrollMode();

    /**
     * Initializes various states for this workspace.
     */
    // @Override
    CARAPI Init();

    // @Override
    CARAPI SnapToDestination();

    // @Override
    CARAPI SnapToPageWithVelocity(
        /* [in] */ Int32 whichPage,
        /* [in] */ Int32 velocity);

    // @Override
    CARAPI SnapToPage(
        /* [in] */ Int32 whichPage);

private:
    CARAPI_(void) SnapToPageWithVelocity(
        /* [in] */ Int32 whichPage,
        /* [in] */ Int32 velocity,
        /* [in] */ Boolean settle);

public:
    static const Int32 DEFAULT_MODE;
    static const Int32 X_LARGE_MODE;

    Int32 mScrollMode;

private:
    static const Float SMOOTHING_SPEED;
    static const Float SMOOTHING_CONSTANT;

    Float mBaseLineFlingVelocity;
    Float mFlingVelocityInfluence;

    AutoPtr<IInterpolator> mScrollInterpolator;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_SMOOTHPAGEDVIEW_H__