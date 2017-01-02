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

#ifndef __ELASTOS_DROID_APP_ACTIVITYMANAGERNATIVE_H__
#define __ELASTOS_DROID_APP_ACTIVITYMANAGERNATIVE_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentSender;

namespace Elastos {
namespace Droid {
namespace App {

class ECO_PUBLIC ActivityManagerNative
{
public:
    static CARAPI_(AutoPtr<IIActivityManager>) GetDefault();

    /**
     * Convenience for checking whether the system is ready.  For internal use only.
     */
    static CARAPI_(Boolean) IsSystemReady();

    /**
     * Convenience for sending a sticky broadcast.  For internal use only.
     * If you don't care about permission, use null.
     */
    static CARAPI_(void) BroadcastStickyIntent(
        /* [in] */ IIntent* intent,
        /* [in] */ const String& permission,
        /* [in] */ Int32 userId);

    static CARAPI_(void) NoteWakeupAlarm(
        /* [in] */ IPendingIntent* ps,
        /* [in] */ Int32 sourceUid,
        /* [in] */ const String& sourcePkg);

protected:
    static Boolean sSystemReady;

private:
    ActivityManagerNative();
    ActivityManagerNative(const ActivityManagerNative&);
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_ACTIVITYMANAGERNATIVE_H__
