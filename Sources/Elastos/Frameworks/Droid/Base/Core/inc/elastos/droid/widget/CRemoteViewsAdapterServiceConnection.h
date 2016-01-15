
#ifndef __ELASTOS_DROID_WIDGET_CREMOTEVIEWSADAPTERSERVICECONNECTION_H__
#define __ELASTOS_DROID_WIDGET_CREMOTEVIEWSADAPTERSERVICECONNECTION_H__

#include "_Elastos_Droid_Widget_CRemoteViewsAdapterServiceConnection.h"
#include "elastos/droid/widget/RemoteViewsAdapter.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CRemoteViewsAdapterServiceConnection)
    , public RemoteViewsAdapter::RemoteViewsAdapterServiceConnection
{
public:
    CAR_OBJECT_DECL()
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_CREMOTEVIEWSADAPTERSERVICECONNECTION_H__
