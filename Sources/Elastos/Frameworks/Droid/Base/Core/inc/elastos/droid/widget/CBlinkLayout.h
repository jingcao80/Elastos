#ifndef __ELASTOS_DROID_WIDGET_CBLINKLAYOUT_H__
#define __ELASTOS_DROID_WIDGET_CBLINKLAYOUT_H__

#include "_Elastos_Droid_Widget_CBlinkLayout.h"

#include "elastos/droid/view/LayoutInflater.h"

using Elastos::Droid::View::LayoutInflater;

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CBlinkLayout) , public LayoutInflater::BlinkLayout
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Widget
} // namepsace Droid
} // namespace Elastos

#endif
