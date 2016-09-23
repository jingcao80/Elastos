
#ifndef __ELASTOS_DROID_PHONE_COMMON_DIALPAD_DIALPADTEXTVIEW_H__
#define __ELASTOS_DROID_PHONE_COMMON_DIALPAD_DIALPADTEXTVIEW_H__

#include "Elastos.Droid.Graphics.h"
#include <elastos/droid/widget/TextView.h>

using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Widget::TextView;

namespace Elastos {
namespace Droid {
namespace Phone {
namespace Common {
namespace Dialpad {

/**
 * EditText which suppresses IME show up.
 */
class DialpadTextView
    : public TextView
{
public:
    DialpadTextView();

    CARAPI constructor(
        /* [in] */ IContext* ctx,
        /* [in] */ IAttributeSet* attrs);

    /**
     * Draw the text to fit within the height/width which have been specified during measurement.
     */
    // @Override
    CARAPI Draw(
        /* [in] */ ICanvas* canvas);

protected:
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

private:
    AutoPtr<IRect> mTextBounds;
    String mTextStr;
};

} // namespace Dialpad
} // namespace Common
} // namespace Phone
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_PHONE_COMMON_DIALPAD_DIALPADTEXTVIEW_H__
