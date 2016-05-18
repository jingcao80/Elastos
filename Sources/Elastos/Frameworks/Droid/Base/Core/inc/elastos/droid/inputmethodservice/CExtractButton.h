
#ifndef __ELASTOS_DROID_INPUTMETHODSERVICE_ELASTOS_DROID_INPUTMEHTODSERVICE_CEXTRACTBUTTON_H__
#define __ELASTOS_DROID_INPUTMETHODSERVICE_ELASTOS_DROID_INPUTMEHTODSERVICE_CEXTRACTBUTTON_H__

#include "_Elastos_Droid_InputMethodService_CExtractButton.h"
#include <elastos/core/Object.h>
#include "elastos/droid/widget/Button.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Widget::Button;

namespace Elastos {
namespace Droid {
namespace InputMethodService {

/**
 * Specialization of {@link Button} that ignores the window not being focused.
 */
CarClass(CExtractButton)
    , public Button
{
public:
    CAR_OBJECT_DECL()

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
     * Pretend like the window this view is in always has focus, so it will
     * highlight when selected.
     */
    CARAPI HasWindowFocus(
        /* [out] */ Boolean* res);
};

} // namespace InputMethodService
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_INPUTMETHODSERVICE_ELASTOS_DROID_INPUTMEHTODSERVICE_CEXTRACTBUTTON_H__
