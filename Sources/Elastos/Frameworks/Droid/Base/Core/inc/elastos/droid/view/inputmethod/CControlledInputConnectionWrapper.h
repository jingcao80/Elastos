
#ifndef __ELASTOS_DROID_VIEW_INPUTMETHOD_CCONTROLLEDINPUTCONNECTIONWRAPPER_H__
#define __ELASTOS_DROID_VIEW_INPUTMETHOD_CCONTROLLEDINPUTCONNECTIONWRAPPER_H__

#include "_Elastos_Droid_View_InputMethod_CControlledInputConnectionWrapper.h"
#include "elastos/droid/internal/view/IInputConnectionWrapper.h"
#include "elastos/droid/view/inputmethod/CInputMethodManager.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Internal::View::IIInputContext;
using Elastos::Droid::Internal::View::IIInputContextCallback;
using Elastos::Droid::Internal::View::IInputConnectionWrapper;

using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace View {
namespace InputMethod {

CarClass(CControlledInputConnectionWrapper)
    , public IInputConnectionWrapper
{
public:
    CAR_OBJECT_DECL()

    CControlledInputConnectionWrapper();

    CARAPI constructor(
        /* [in] */ ILooper* mainLooper,
        /* [in] */ IInputConnection* conn,
        /* [in] */ IInputMethodManager* agmer);

    CARAPI IsActive(
        /* [out] */ Boolean* result);

    CARAPI Deactivate();

private:
    AutoPtr<CInputMethodManager> mParentInputMethodManager;
    Boolean mActive;
};

} // namespace InputMethod
} // namespace View
} // namespace Droid
} // namespace Elastos

#endif   //__ELASTOS_DROID_VIEW_INPUTMETHOD_CCONTROLLEDINPUTCONNECTIONWRAPPER_H__
