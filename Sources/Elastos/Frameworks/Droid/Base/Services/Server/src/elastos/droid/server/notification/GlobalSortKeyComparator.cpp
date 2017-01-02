//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
