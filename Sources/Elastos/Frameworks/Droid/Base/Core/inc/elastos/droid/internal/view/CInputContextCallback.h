
#ifndef __ELASTOS_DROID_INTERNAL_VIEW_CINPUTCONTEXTCALLBACK_H__
#define __ELASTOS_DROID_INTERNAL_VIEW_CINPUTCONTEXTCALLBACK_H__

#include "_Elastos_Droid_Internal_View_CInputContextCallback.h"
#include "elastos/droid/internal/view/InputConnectionWrapper.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {

CarClass(CInputContextCallback)
    , public InputConnectionWrapper::InputContextCallback
{
public:
    CAR_OBJECT_DECL()
};

} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_VIEW_CINPUTCONTEXTCALLBACK_H__
