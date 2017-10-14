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

#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_SIZEADAPTIVELAYOUT_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_SIZEADAPTIVELAYOUT_H__

#include "elastos/droid/view/ViewGroup.h"

using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Animation::IAnimatorSet;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::View::ViewGroup;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroupLayoutParams;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

/**
 * A layout that switches between its children based on the requested layout height.
 * Each child specifies its minimum and maximum valid height.  Results are undefined
 * if children specify overlapping ranges.  A child may specify the maximum height
 * as 'unbounded' to indicate that it is willing to be displayed arbitrarily tall.
 *
 * <p>
 * See {@link SizeAdaptiveLayout.LayoutParams} for a full description of the
 * layout parameters used by SizeAdaptiveLayout.
 */
class SizeAdaptiveLayout
    : public ViewGroup
    , public ISizeAdaptiveLayout
{
public:
    class BringToFrontOnEnd
        : public Object
        , public IAnimatorListener
    {
    public:
        CAR_INTERFACE_DECL()

        BringToFrontOnEnd(
            /* [in] */ SizeAdaptiveLayout* host);

        virtual CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

        virtual CARAPI OnAnimationCancel(
            /* [in] */ IAnimator* animation);

        virtual CARAPI OnAnimationRepeat(
            /* [in] */ IAnimator* animation);

        virtual CARAPI OnAnimationStart(
            /* [in] */ IAnimator* animation);

    private:
        SizeAdaptiveLayout* mHost;
    };

    /**
     * Per-child layout information associated with ViewSizeAdaptiveLayout.
     *
     * TODO extend to width and height
     *
     * @attr ref android.R.styleable#SizeAdaptiveLayout_Layout_layout_minHeight
     * @attr ref android.R.styleable#SizeAdaptiveLayout_Layout_layout_maxHeight
     */
    class SizeAdaptiveLayoutLayoutParams
        : public ViewGroup::LayoutParams
        , public ISizeAdaptiveLayoutLayoutParams
    {
    public:
        CAR_INTERFACE_DECL()

        SizeAdaptiveLayoutLayoutParams();

        /**
         * {@inheritDoc}
         */
        CARAPI constructor(
            /* [in] */ IContext* c,
            /* [in] */ IAttributeSet* attrs);

        /**
         * Creates a new set of layout parameters with the specified width, height
         * and valid height bounds.
         *
         * @param width the width, either {@link #MATCH_PARENT},
         *        {@link #WRAP_CONTENT} or a fixed size in pixels
         * @param height the height, either {@link #MATCH_PARENT},
         *        {@link #WRAP_CONTENT} or a fixed size in pixels
         * @param minHeight the minimum height of this child
         * @param maxHeight the maximum height of this child
         *        or {@link #UNBOUNDED} if the child can grow forever
         */
        CARAPI constructor(
            /* [in] */ Int32 width,
            /* [in] */ Int32 height,
            /* [in] */ Int32 minHeight,
            /* [in] */ Int32 maxHeight);

        /**
         * {@inheritDoc}
         */
        CARAPI constructor(
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        /**
         * Constructs a new LayoutParams with default values as defined in {@link LayoutParams}.
         */
        CARAPI constructor();

        /**
         * {@inheritDoc}
         */
        CARAPI constructor(
            /* [in] */ IViewGroupLayoutParams* p);

        CARAPI Debug(
            /* [in] */ const String& output,
            /* [out] */ String* str);

        CARAPI SetMinHeight(
            /* [in] */ Int32 mh);

        CARAPI GetMinHeight(
            /* [out] */ Int32* mh);

        CARAPI SetMaxHeight(
            /* [in] */ Int32 mh);

        CARAPI GetMaxHeight(
            /* [out] */ Int32* mh);

    public:
        /**
         * Indicates the minimum valid height for the child.
         */
        // @ViewDebug.ExportedProperty(category = "layout")
        Int32 mMinHeight;

        /**
         * Indicates the maximum valid height for the child.
         */
        // @ViewDebug.ExportedProperty(category = "layout")
        Int32 mMaxHeight;
    };

public:
    CAR_INTERFACE_DECL()

    SizeAdaptiveLayout();

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

    /**
     * Visible for testing
     * @hide
     */
    virtual CARAPI GetTransitionAnimation(
        /* [out] */ IAnimator** animator);

    /**
     * Visible for testing
     * @hide
     */
    virtual CARAPI GetModestyPanel(
        /* [out] */ IView** panel);

    virtual CARAPI OnAttachedToWindow();


    virtual CARAPI GenerateLayoutParams(
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IViewGroupLayoutParams** params);

protected:
    virtual CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    virtual CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    virtual CARAPI GenerateDefaultLayoutParams(
        /* [out] */ IViewGroupLayoutParams** params);

    virtual CARAPI_(Boolean) CheckLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p);

private:
    CARAPI Initialize();

    CARAPI_(Int32) ClampSizeToBounds(
        /* [in] */ Int32 measuredHeight,
        /* [in] */ IView* child);

    //TODO extend to width and height
    CARAPI_(AutoPtr<IView>) SelectActiveChild(
        /* [in] */ Int32 heightMeasureSpec);

private:
    // TypedArray indices
    static const Int32 MIN_VALID_HEIGHT;
    static const Int32 MAX_VALID_HEIGHT;
    static const String TAG;
    static const Boolean DEBUG;
    static const Boolean REPORT_BAD_BOUNDS;
    static const Int64 CROSSFADE_TIME;

    // view state
    AutoPtr<IView> mActiveChild;
    AutoPtr<IView> mLastActive;

    // animation state
    AutoPtr<IAnimatorSet> mTransitionAnimation;
    AutoPtr<IAnimatorListener> mAnimatorListener;
    AutoPtr<IObjectAnimator> mFadePanel;
    AutoPtr<IObjectAnimator> mFadeView;
    Int32 mCanceledAnimationCount;
    AutoPtr<IView> mEnteringView;
    AutoPtr<IView> mLeavingView;

    // View used to hide larger views under smaller ones to create a uniform crossfade
    AutoPtr<IView> mModestyPanel;
    Int32 mModestyPanelTop;
};

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_SIZEADAPTIVELAYOUT_H__
