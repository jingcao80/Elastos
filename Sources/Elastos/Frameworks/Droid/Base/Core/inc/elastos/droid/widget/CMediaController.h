#ifndef __ELASTOS_DROID_WIDGET_CMEDIACONTROLLER_H__
#define __ELASTOS_DROID_WIDGET_CMEDIACONTROLLER_H__

#include "_Elastos_Droid_Widget_CMediaController.h"
#include "elastos/droid/widget/MediaController.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CMediaController)
    , public MediaController
{
public:
    CAR_OBJECT_DECL()
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_CMEDIACONTROLLER_H__
