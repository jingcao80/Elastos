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

#ifndef __ELASTOS_DROID_SERVER_NOTIFICATION_PACKAGEVISIBILITYEXTRACTOR_H__
#define __ELASTOS_DROID_SERVER_NOTIFICATION_PACKAGEVISIBILITYEXTRACTOR_H__

#include "elastos/droid/ext/frameworkext.h"
#include <_Elastos.Droid.Server.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Notification {

class PackageVisibilityExtractor
    : public Object
    , public IPackageVisibilityExtractor
    , public INotificationSignalExtractor
{
public:
    CAR_INTERFACE_DECL()

    PackageVisibilityExtractor();

    ~PackageVisibilityExtractor();

    CARAPI Initialize(
        /* [in] */ IContext* context);

    CARAPI Process(
        /* [in] */ INotificationRecord* record,
        /* [out] */ IRankingReconsideration** consideration);

    CARAPI SetConfig(
        /* [in] */ IRankingConfig* config);

private:
    static const String TAG;
    static const Boolean DBG;

    AutoPtr<IRankingConfig> mConfig;
};

} // Notification
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_NOTIFICATION_PACKAGEVISIBILITYEXTRACTOR_H__
