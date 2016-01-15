
#ifndef __ELASTOS_DROID_VIEW_CTOUCHDELEGATE_H__
#define __ELASTOS_DROID_VIEW_CTOUCHDELEGATE_H__

#include "_Elastos_Droid_View_CTouchDelegate.h"
#include "elastos/droid/view/TouchDelegate.h"

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CTouchDelegate)
    , public TouchDelegate
{
public:
    CAR_OBJECT_DECL()
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_VIEW_CTOUCHDELEGATE_H__
