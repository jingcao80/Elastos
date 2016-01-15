
#ifndef __ELASTOS_DROID_WIDGET_INTERNAL_DIALOGTITLE_H__
#define __ELASTOS_DROID_WIDGET_INTERNAL_DIALOGTITLE_H__

#include "elastos/droid/widget/TextView.h"

using Elastos::Droid::Widget::TextView;

namespace Elastos {
namespace Droid {
namespace Widget {
namespace Internal {

/**
 * Used by dialogs to change the font size and number of lines to try to fit
 * the text to the available space.
 */
class DialogTitle : public TextView
{
public:
    DialogTitle(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    DialogTitle(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    DialogTitle(
        /* [in] */ IContext* context);

protected:
    DialogTitle();

    CARAPI Init(
        /* [in] */ IContext* context);

    CARAPI Init(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI Init(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    //@Override
    virtual CARAPI_(void) OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);
};

}// namespace Internal
}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_INTERNAL_DIALOGTITLE_H__
