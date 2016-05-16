
#ifndef __ELASTOS_DROID_WIDGET_TEXTVIEWWITHCIRCULARINDICATOR_H__
#define __ELASTOS_DROID_WIDGET_TEXTVIEWWITHCIRCULARINDICATOR_H__

#include "elastos/droid/widget/TextView.h"

using Elastos::Droid::Widget::TextView;

namespace Elastos {
namespace Droid {
namespace Widget {

class TextViewWithCircularIndicator
    : public TextView
    , public ITextViewWithCircularIndicator
{
public:
    CAR_INTERFACE_DECL()

    TextViewWithCircularIndicator();

    ~TextViewWithCircularIndicator();

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

    CARAPI SetCircleColor(
        /* [in] */ Int32 color);

    CARAPI SetDrawIndicator(
        /* [in] */ Boolean drawIndicator);

    //@Override
    virtual CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

    //@Override
    virtual CARAPI GetContentDescription(
        /* [out] */ ICharSequence** text);

private:
    CARAPI_(void) Init();

private:
    static const Int32 SELECTED_CIRCLE_ALPHA;

    AutoPtr<IPaint> mCirclePaint;
    String mItemIsSelectedText;
    Int32 mCircleColor;
    Boolean mDrawIndicator;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif
