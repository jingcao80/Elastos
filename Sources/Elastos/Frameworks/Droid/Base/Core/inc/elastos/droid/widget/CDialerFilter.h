#ifndef __ELASTOS_DROID_WIDGET_CDIALERFILTER_H__
#define __ELASTOS_DROID_WIDGET_CDIALERFILTER_H__

#include "_Elastos_Droid_Widget_CDialerFilter.h"
#include "elastos/droid/widget/DialerFilter.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CDialerFilter)
    , public DialerFilter
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_CDIALERFILTER_H__
