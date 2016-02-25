
#ifndef __ELASTOS_DROID_INPUTMETHODSERVICE_CLATINKEYBOARDVIEW_H__
#define __ELASTOS_DROID_INPUTMETHODSERVICE_CLATINKEYBOARDVIEW_H__

#include "_Elastos_Droid_InputMethodService_CLatinKeyboardView.h"

#include "elastos/droid/inputmethodservice/KeyboardView.h"

namespace Elastos {
namespace Droid {
namespace InputMethodService {

CarClass(CLatinKeyboardView), public KeyboardView
{
public:
    CAR_OBJECT_DECL();

    CARAPI constructor(
        /* [in] */ IContext* ctx,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* ctx,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI OnLongPress(
        /* [in] */ IKeyboardKey* popupKey,
        /* [out] */ Boolean* res);
};

} // namespace InputMethodService
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_INPUTMETHODSERVICE_CLATINKEYBOARDVIEW_H__
