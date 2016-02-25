
#ifndef __ELASTOS_DROID_WIDGET_TOGGLEBUTTON_H__
#define __ELASTOS_DROID_WIDGET_TOGGLEBUTTON_H__

#include "elastos/droid/widget/CompoundButton.h"

using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfo;

namespace Elastos {
namespace Droid {
namespace Widget {

class ToggleButton
    : public CompoundButton
    , public IToggleButton
{
public:
    CAR_INTERFACE_DECL();

    ToggleButton();

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

    //@Override
    virtual CARAPI SetChecked(
        /* [in] */ Boolean checked);

    virtual CARAPI GetTextOn(
        /* [out] */ ICharSequence** on);

    virtual CARAPI SetTextOn(
        /* [in] */ ICharSequence* textOn);

    virtual CARAPI GetTextOff(
        /* [out] */ ICharSequence** off);

    virtual CARAPI SetTextOff(
        /* [in] */ ICharSequence* textOff);

    //@Override
    virtual CARAPI SetBackgroundDrawable(
        /* [in] */ IDrawable* d);

    virtual CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    virtual CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

protected:
    //@Override
    virtual CARAPI OnFinishInflate();

    //@Override
    virtual CARAPI DrawableStateChanged();

private:
    CARAPI_(void) UpdateReferenceToIndicatorDrawable(
        /* [in] */ IDrawable* backgroundDrawable);

    CARAPI_(void) SyncTextState();

private:
    AutoPtr<ICharSequence> mTextOn;
    AutoPtr<ICharSequence> mTextOff;

    AutoPtr<IDrawable> mIndicatorDrawable;

    static const Int32 NO_ALPHA;
    Float mDisabledAlpha;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_TOGGLEBUTTON_H__
