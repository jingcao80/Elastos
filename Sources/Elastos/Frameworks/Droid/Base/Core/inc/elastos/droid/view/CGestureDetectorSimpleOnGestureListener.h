
#ifndef __ELASTOS_DROID_VIEW_CGESTUREDETECTORSIMPLEONGESTURELISTENER_H__
#define __ELASTOS_DROID_VIEW_CGESTUREDETECTORSIMPLEONGESTURELISTENER_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "_Elastos_Droid_View_CGestureDetectorSimpleOnGestureListener.h"
#include "elastos/droid/view/GestureDetector.h"

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CGestureDetectorSimpleOnGestureListener)
    , public GestureDetector::SimpleOnGestureListener
{
public:
    CAR_OBJECT_DECL()
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_CGESTUREDETECTORSIMPLEONGESTURELISTENER_H__
