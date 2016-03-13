#ifndef __ELASTOS_DROID_WIDGET_CREMOTEVIEWSBITMAPCACHE_H__
#define __ELASTOS_DROID_WIDGET_CREMOTEVIEWSBITMAPCACHE_H__

#include "_Elastos_Droid_Widget_CRemoteViewsBitmapCache.h"
#include "elastos/droid/widget/RemoteViews.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CRemoteViewsBitmapCache)
    , public RemoteViews::BitmapCache
{
public:
    CAR_OBJECT_DECL()
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_CREMOTEVIEWSBITMAPCACHE_H__
