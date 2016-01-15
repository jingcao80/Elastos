#ifndef __ELASTOS_DROID_VIEW_CSURFACEVIEWHELPER_H__
#define __ELASTOS_DROID_VIEW_CSURFACEVIEWHELPER_H__

#include "_Elastos_Droid_View_CSurfaceViewHelper.h"

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CSurfaceViewHelper)
{
public:

    CARAPI AdjustSurfaceViewMotion(
        /* [in]*/ IMotionEvent* evt);

};

}// namespace View
}// namespace Droid
}// namespace Elastos

#endif
