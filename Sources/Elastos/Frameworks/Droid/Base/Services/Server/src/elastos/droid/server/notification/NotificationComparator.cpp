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

#include <Elastos.Droid.Service.h>
#include "elastos/droid/server/notification/NotificationComparator.h"
#include "elastos/droid/server/notification/NotificationRecord.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Service::Notification::IStatusBarNotification;
using Elastos::Core::EIID_IComparator;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Notification {

CAR_INTERFACE_IMPL(NotificationComparator, Object, IComparator);

NotificationComparator::NotificationComparator()
{
}

NotificationComparator::~NotificationComparator()
{}

ECode NotificationComparator::Compare(
    /* [in] */ IInterface* lhs,
    /* [in] */ IInterface* rhs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    using Elastos::Core::Math;

    AutoPtr<NotificationRecord> left = (NotificationRecord*)INotificationRecord::Probe(lhs);
    AutoPtr<NotificationRecord> right = (NotificationRecord*)INotificationRecord::Probe(rhs);

    const Int32 leftPackagePriority = left->GetPackagePriority();
    const Int32 rightPackagePriority = right->GetPackagePriority();
    if (leftPackagePriority != rightPackagePriority) {
        // by priority, high to low
        *result = -1 * Math::Compare(leftPackagePriority, rightPackagePriority);
        return NOERROR;
    }

    Int32 leftScore;
    left->mSbn->GetScore(&leftScore);
    Int32 rightScore;
    right->mSbn->GetScore(&rightScore);
    if (leftScore != rightScore) {
        // by priority, high to low
        *result = -1 * Math::Compare(leftScore, rightScore);
        return NOERROR;
    }

    const Float leftPeople = left->GetContactAffinity();
    const Float rightPeople = right->GetContactAffinity();
    if (leftPeople != rightPeople) {
        // by contact proximity, close to far
        *result = -1 * Math::Compare(leftPeople, rightPeople);
        return NOERROR;
    }

    // then break ties by time, most recent first
    *result = -1 * Math::Compare(left->GetRankingTimeMs(), right->GetRankingTimeMs());
    return NOERROR;
}

} // Notification
} // Server
} // Droid
} // Elastos
