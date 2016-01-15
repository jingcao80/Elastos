
#ifndef __ELASTOS_DROID_VIEW_CDRAGEVENT_H__
#define __ELASTOS_DROID_VIEW_CDRAGEVENT_H__

#include "_Elastos_Droid_View_CDragEvent.h"
#include "elastos/droid/view/DragEvent.h"

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CDragEvent)
    , public DragEvent
{
public:
    CAR_OBJECT_DECL()
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_VIEW_CDRAGEVENT_H__
