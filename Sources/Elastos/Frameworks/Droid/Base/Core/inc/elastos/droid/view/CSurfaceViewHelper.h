#ifndef __ELASTOS_DROID_VIEW_CSURFACEVIEWHELPER_H__
#define __ELASTOS_DROID_VIEW_CSURFACEVIEWHELPER_H__

#include "_Elastos_Droid_View_CSurfaceViewHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CSurfaceViewHelper)
    , public Singleton
    , public ISurfaceViewHelper
{
public:

    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI AdjustSurfaceViewMotion(
        /* [in]*/ IMotionEvent* evt);

};

}// namespace View
}// namespace Droid
}// namespace Elastos

#endif
