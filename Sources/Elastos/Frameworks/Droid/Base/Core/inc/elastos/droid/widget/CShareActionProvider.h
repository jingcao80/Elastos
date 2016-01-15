#ifndef __ELASTOS_DROID_WIDGET_CSHAREACTIONPROVIDER_H__
#define __ELASTOS_DROID_WIDGET_CSHAREACTIONPROVIDER_H__

#include "_Elastos_Droid_Widget_CShareActionProvider.h"
#include "elastos/droid/widget/ShareActionProvider.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CShareActionProvider)
    , public ShareActionProvider
{
public:
    CAR_OBJECT_DECL()
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_CSHAREACTIONPROVIDER_H__
