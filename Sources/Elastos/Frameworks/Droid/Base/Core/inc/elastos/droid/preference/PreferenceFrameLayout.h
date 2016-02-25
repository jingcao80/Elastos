
#ifndef __ELASTOS_DROID_PREFERENCE_PREFERENCEFRAMELAYOUT_H__
#define __ELASTOS_DROID_PREFERENCE_PREFERENCEFRAMELAYOUT_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/FrameLayout.h"

using Elastos::Droid::Graphics::Drawable::IDrawableCallback;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::IViewManager;
using Elastos::Droid::View::IKeyEventCallback;
using Elastos::Droid::View::Accessibility::IAccessibilityEventSource;
using Elastos::Droid::Widget::IFrameLayout;
using Elastos::Droid::Widget::FrameLayout;
using Elastos::Droid::Widget::IFrameLayoutLayoutParams;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Content::Res::ITypedArray;

namespace Elastos {
namespace Droid {
namespace Preference {

class PreferenceFrameLayout
    : public FrameLayout
    , public IPreferenceFrameLayout
{
public:
    class LayoutParams
        : public FrameLayout::LayoutParams
        , public IPreferenceFrameLayoutParams
    {
    public:
        CAR_INTERFACE_DECL()

        LayoutParams();

        CARAPI constructor(
            /* [in] */ IContext* c,
            /* [in] */ IAttributeSet* attrs);

        CARAPI constructor(
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        CARAPI GetGravity(
            /* [out] */ Int32* gravity);

        CARAPI SetGravity(
            /* [in] */ Int32 gravity);

    public:
        Boolean mRemoveBorders;
    };

public:
    CAR_INTERFACE_DECL()

    PreferenceFrameLayout();

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

    CARAPI GenerateLayoutParams(
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IViewGroupLayoutParams** params);

    CARAPI AddView(
        /* [in] */ IView* child);

private:
    static const Int32 DEFAULT_BORDER_TOP = 0;
    static const Int32 DEFAULT_BORDER_BOTTOM = 0;
    static const Int32 DEFAULT_BORDER_LEFT = 0;
    static const Int32 DEFAULT_BORDER_RIGHT = 0;

    Int32 mBorderTop;
    Int32 mBorderBottom;
    Int32 mBorderLeft;
    Int32 mBorderRight;
    Boolean mPaddingApplied;
};

} // Preference
} // Droid
} // Elastos

#endif  // __ELASTOS_DROID_PREFERENCE_PREFERENCEFRAMELAYOUT_H__
