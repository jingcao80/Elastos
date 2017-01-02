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

#include "elastos/droid/server/notification/PackagePriorityExtractor.h"
#include "elastos/droid/server/notification/NotificationRecord.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Notification {

const String PackagePriorityExtractor::TAG("ImportantPackageExtractor");
const Boolean PackagePriorityExtractor::DBG = FALSE;


CAR_INTERFACE_IMPL_2(PackagePriorityExtractor, Object, IPackagePriorityExtractor, INotificationSignalExtractor);

PackagePriorityExtractor::PackagePriorityExtractor()
{
}

PackagePriorityExtractor::~PackagePriorityExtractor()
{}

ECode PackagePriorityExtractor::Initialize(
    /* [in] */ IContext* context)
{
    if (DBG) Slogger::D(TAG, "Initializing PackagePriorityExtractor.");
    return NOERROR;
}

ECode PackagePriorityExtractor::Process(
    /* [in] */ INotificationRecord* record,
    /* [out] */ IRankingReconsideration** consideration)
{
    VALIDATE_NOT_NULL(consideration);
    *consideration = NULL;

    AutoPtr<NotificationRecord> r = (NotificationRecord*)record;

    if (r == NULL || r->GetNotification() == NULL) {
        if (DBG) Slogger::D(TAG, "skipping empty notification");
        return NOERROR;
    }

    if (mConfig == NULL) {
        if (DBG) Slogger::D(TAG, "missing config");
        return NOERROR;
    }

    String pkg;
    r->mSbn->GetPackageName(&pkg);
    Int32 uid;
    r->mSbn->GetUid(&uid);

    Int32 packagePriority;
    mConfig->GetPackagePriority(pkg, uid, &packagePriority);
    r->SetPackagePriority(packagePriority);

    return NOERROR;
}

ECode PackagePriorityExtractor::SetConfig(
    /* [in] */ IRankingConfig* config)
{
    mConfig = config;
    return NOERROR;
}

} // Notification
} // Server
} // Droid
} // Elastos
