
#ifndef __ELASTOS_DROID_VIEW_INPUTMETHOD_CIINPUTMETHODCLIENT_H__
#define  __ELASTOS_DROID_VIEW_INPUTMETHOD_CIINPUTMETHODCLIENT_H__

#include "_Elastos_Droid_View_InputMethod_CIInputMethodClient.h"
#include "elastos/droid/view/inputmethod/CInputMethodManager.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace View {
namespace InputMethod {

CarClass(CIInputMethodClient)
    , public Object
    , public IInputMethodClient
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IInputMethodManager* host);

    CARAPI SetUsingInputMethod(
        /* [in] */ Boolean state);

    CARAPI OnBindMethod(
        /* [in] */ IInputBindResult* res);

    CARAPI OnUnbindMethod(
        /* [in] */ Int32 sequence);

    CARAPI SetActive(
        /* [in] */ Boolean active);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI SetUserActionNotificationSequenceNumber(
        /* [in] */ Int32 sequenceNumber);

private:
    AutoPtr<CInputMethodManager> mHost;
};

} // namespace InputMethod
} // namespace View
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_VIEW_INPUTMETHOD_CIINPUTMETHODCLIENT_H__
