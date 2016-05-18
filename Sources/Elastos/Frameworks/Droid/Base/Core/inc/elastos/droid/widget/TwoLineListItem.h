#ifndef __ELASTOS_DROID_WIDGET_TWOLINELISTITEM_H__
#define __ELASTOS_DROID_WIDGET_TWOLINELISTITEM_H__

#include "elastos/droid/widget/RelativeLayout.h"

namespace Elastos {
namespace Droid {
namespace Widget {

class TwoLineListItem
    : public RelativeLayout
    , public ITwoLineListItem
{
public:
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

    virtual CARAPI GetText1(
        /* [out] */ ITextView** textView);

    virtual CARAPI GetText2(
        /* [out] */ ITextView** textView);

    virtual CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    virtual CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

protected:
    CARAPI OnFinishInflate();

private:
    AutoPtr<ITextView> mText1;
    AutoPtr<ITextView> mText2;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_TWOLINELISTITEM_H__
