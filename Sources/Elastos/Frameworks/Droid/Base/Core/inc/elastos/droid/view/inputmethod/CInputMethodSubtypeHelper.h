
#ifndef __ELASTOS_DROID_VIEW_INPUTMETHOD_CINPUTMETHODSUBTYPEHELPER_H__
#define  __ELASTOS_DROID_VIEW_INPUTMETHOD_CINPUTMETHODSUBTYPEHELPER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_View_InputMethod_CInputMethodSubtypeHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;

using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace View {
namespace InputMethod {

CarClass(CInputMethodSubtypeHelper)
    , public Singleton
    , public IInputMethodSubtypeHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI Sort(
        /* [in] */ IContext* context,
        /* [in] */ Int32 flags,
        /* [in] */ IInputMethodInfo* imi,
        /* [in] */ IList* subtypeList,
        /* [out, callee] */ IList** sortedSubtypeList);
};


} // namespace InputMethod
} // namespace View
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_VIEW_INPUTMETHOD_CINPUTMETHODSUBTYPEHELPER_H__
