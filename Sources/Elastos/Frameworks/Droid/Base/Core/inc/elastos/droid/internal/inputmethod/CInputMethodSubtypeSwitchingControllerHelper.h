#ifndef __ELASTOS_DROID_INTERNAL_INPUTMETHOD_CINPUTMETHODSUBTYPESWITCHINGCONTROLLERHELPER_H__
#define __ELASTOS_DROID_INTERNAL_INPUTMETHOD_CINPUTMETHODSUBTYPESWITCHINGCONTROLLERHELPER_H__

#include "_Elastos_Droid_Internal_InputMethod_CInputMethodSubtypeSwitchingControllerHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace InputMethod {

CarClass(CInputMethodSubtypeSwitchingControllerHelper)
    , public Singleton
    , public IInputMethodSubtypeSwitchingControllerHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI CreateInstanceLocked(
        /* [in] */ IInputMethodSettings* settings,
        /* [in] */ IContext* context,
        /* [out] */ IInputMethodSubtypeSwitchingController** instance);
};

} // namespace InputMethod
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_INPUTMETHOD_CINPUTMETHODSUBTYPESWITCHINGCONTROLLERHELPER_H__
