#ifndef __ELASTOS_DROID_VIEW_CSCALEGESTUREDETECTORSIMPLEONSCALEGESTURELISTENER_H__
#define __ELASTOS_DROID_VIEW_CSCALEGESTUREDETECTORSIMPLEONSCALEGESTURELISTENER_H__

#include "_Elastos_Droid_View_CScaleGestureDetectorSimpleOnScaleGestureListener.h"
#include "elastos/droid/view/ScaleGestureDetector.h"

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CScaleGestureDetectorSimpleOnScaleGestureListener)
    , public ScaleGestureDetector::SimpleOnScaleGestureListener
{
public:
    CAR_OBJECT_DECL();
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_CSCALEGESTUREDETECTORSIMPLEONSCALEGESTURELISTENER_H__
