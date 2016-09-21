
#ifndef __ELASTOS_DROID_PHONE_COMMON_DIALPAD_DIGITSEDITTEXT_H__
#define __ELASTOS_DROID_PHONE_COMMON_DIALPAD_DIGITSEDITTEXT_H__

#include "elastos/droid/phone/common/widget/ResizingTextEditText.h"

using Elastos::Droid::Phone::Common::Widget::ResizingTextEditText;

namespace Elastos {
namespace Droid {
namespace Phone {
namespace Common {
namespace Dialpad {

/**
 * EditText which suppresses IME show up.
 */
class DigitsEditText
    : public ResizingTextEditText
{
public:
    CARAPI constructor(
        /* [in] */ IContext* ctx,
        /* [in] */ IAttributeSet* attrs);

    // @Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* res);

protected:
    // @Override
    CARAPI_(void) OnFocusChanged(
        /* [in] */ Boolean focused,
        /* [in] */ Int32 direction,
        /* [in] */ IRect* previouslyFocusedRect);
};

} // namespace Dialpad
} // namespace Common
} // namespace Phone
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_PHONE_COMMON_DIALPAD_DIGITSEDITTEXT_H__
