
#include "elastos/droid/server/notification/GlobalSortKeyComparator.h"
#include "elastos/droid/server/notification/NotificationRecord.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::EIID_IComparator;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Notification {

CAR_INTERFACE_IMPL(GlobalSortKeyComparator, Object, IComparator);

GlobalSortKeyComparator::GlobalSortKeyComparator()
{
}

GlobalSortKeyComparator::~GlobalSortKeyComparator()
{}

ECode GlobalSortKeyComparator::Compare(
    /* [in] */ IInterface* lhs,
    /* [in] */ IInterface* rhs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -1;

    AutoPtr<NotificationRecord> left = (NotificationRecord*)INotificationRecord::Probe(lhs);
    AutoPtr<NotificationRecord> right = (NotificationRecord*)INotificationRecord::Probe(rhs);

    if (left->GetGlobalSortKey().IsNull()) {
        Slogger::E("GlobalSortKeyComparator", "Missing left global sort key: %p", left.Get());
        return E_ILLEGAL_STATE_EXCEPTION;
        // throw new IllegalStateException("Missing left global sort key: " + left);
    }
    if (right->GetGlobalSortKey().IsNull()) {
        Slogger::E("GlobalSortKeyComparator", "Missing right global sort key: %p", right.Get());
        return E_ILLEGAL_STATE_EXCEPTION;
        // throw new IllegalStateException("Missing right global sort key: " + right);
    }

    *result = left->GetGlobalSortKey().Compare(right->GetGlobalSortKey());
    return NOERROR;
}

} // Notification
} // Server
} // Droid
} // Elastos
