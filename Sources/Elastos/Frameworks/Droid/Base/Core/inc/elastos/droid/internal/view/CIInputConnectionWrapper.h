
#ifndef __ELASTOS_DROID_INTERNAL_VIEW_CIINPUTCONNECTIONWRAPPER_H__
#define __ELASTOS_DROID_INTERNAL_VIEW_CIINPUTCONNECTIONWRAPPER_H__

#include "_Elastos_Droid_Internal_View_CIInputConnectionWrapper.h"
#include "elastos/droid/internal/view/IInputConnectionWrapper.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {

CarClass(CIInputConnectionWrapper)
    , public IInputConnectionWrapper
{
public:
    CAR_OBJECT_DECL()
};

} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_VIEW_CIINPUTCONNECTIONWRAPPER_H__
