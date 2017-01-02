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

#ifndef __ELASTOS_DROID_CONTENT_CSYNCRESULTHELPER_H__
#define __ELASTOS_DROID_CONTENT_CSYNCRESULTHELPER_H__

#include "_Elastos_Droid_Content_CSyncResultHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CSyncResultHelper)
    , public Singleton
    , public ISyncResultHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * This instance of a SyncResult is returned by the SyncAdapter in response to a
     * sync request when a sync is already underway. The SyncManager will reschedule the
     * sync request to try again later.
     */
    CARAPI GetALREADY_IN_PROGRESS(
        /* [out] */ ISyncResult** syncResult);

};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CSYNCRESULTHELPER_H__
