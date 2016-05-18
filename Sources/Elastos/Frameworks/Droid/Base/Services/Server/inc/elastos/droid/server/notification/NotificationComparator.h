
#ifndef __ELASTOS_DROID_SERVER_NOTIFICATION_NOTIFICATIONCOMPARATOR_H__
#define __ELASTOS_DROID_SERVER_NOTIFICATION_NOTIFICATIONCOMPARATOR_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::IComparator;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Notification {

/**
 * Sorts notifications individually into attention-relelvant order.
 */
class NotificationComparator
    : public Object
    , public IComparator
{
public:
    CAR_INTERFACE_DECL()

    NotificationComparator();

    ~NotificationComparator();

    // @Override
    CARAPI Compare(
        /* [in] */ IInterface* lhs,
        /* [in] */ IInterface* rhs,
        /* [out] */ Int32* result);
};

} // Notification
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_NOTIFICATION_NOTIFICATIONCOMPARATOR_H__
