
#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_DIALOGTITLE_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_DIALOGTITLE_H__

#include "elastos/droid/widget/TextView.h"

using Elastos::Droid::Widget::TextView;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

/**
 * Used by dialogs to change the font size and number of lines to try to fit
 * the text to the available space.
 */
class DialogTitle
    : public TextView
    , public IDialogTitle
{
public:
    CAR_INTERFACE_DECL()

    DialogTitle();

    ~DialogTitle();

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

protected:
    //@Override
    virtual CARAPI_(void) OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);
};

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_WIDGET_DIALOGTITLE_H__
