#ifndef __ELASTOS_DROID_VIEW_CSCALEGESTUREDETECTOR_H__
#define __ELASTOS_DROID_VIEW_CSCALEGESTUREDETECTOR_H__

#include "_Elastos_Droid_View_CScaleGestureDetector.h"
#include "elastos/droid/view/ScaleGestureDetector.h"

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CScaleGestureDetector)
    , public ScaleGestureDetector
{
public:
    CAR_OBJECT_DECL();
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_CSCALEGESTUREDETECTOR_H__
