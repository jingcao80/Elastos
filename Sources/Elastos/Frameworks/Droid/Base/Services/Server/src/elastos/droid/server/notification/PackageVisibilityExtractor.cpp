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

#include "elastos/droid/server/notification/PackageVisibilityExtractor.h"
#include "elastos/droid/server/notification/NotificationRecord.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Notification {

const String PackageVisibilityExtractor::TAG("PackageVisibilityExtractor");
const Boolean PackageVisibilityExtractor::DBG = FALSE;

CAR_INTERFACE_IMPL_2(PackageVisibilityExtractor, Object, IPackageVisibilityExtractor, INotificationSignalExtractor);

PackageVisibilityExtractor::PackageVisibilityExtractor()
{
}

PackageVisibilityExtractor::~PackageVisibilityExtractor()
{}

ECode PackageVisibilityExtractor::Initialize(
    /* [in] */ IContext* context)
{
    if (DBG) Slogger::D(TAG, "Initializing PackageVisibilityExtractor.");
    return NOERROR;
}

ECode PackageVisibilityExtractor::Process(
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

    Int32 packageVisibility;
    mConfig->GetPackageVisibilityOverride(pkg, uid, &packageVisibility);
    r->SetPackageVisibilityOverride(packageVisibility);

    return NOERROR;
}

ECode PackageVisibilityExtractor::SetConfig(
    /* [in] */ IRankingConfig* config)
{
    mConfig = config;
    return NOERROR;
}

} // Notification
} // Server
} // Droid
} // Elastos
