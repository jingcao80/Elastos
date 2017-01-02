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

#ifndef __ELASTOS_DROID_SERVICE_DREAMS_CSANDMAN_H__
#define __ELASTOS_DROID_SERVICE_DREAMS_CSANDMAN_H__

#include "_Elastos_Droid_Service_Dreams_CSandman.h"
#include "elastos/droid/service/dreams/Sandman.h"

namespace Elastos {
namespace Droid {
namespace Service {
namespace Dreams {

CarClass(CSandman)
    , public Singleton
    , public ISandman
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Returns true if the specified dock app intent should be started.
     * False if we should dream instead, if appropriate.
     */
    CARAPI ShouldStartDockApp(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent,
        /* [out] */ Boolean* result);

    /**
     * Starts a dream manually.
     */
    CARAPI StartDreamByUserRequest(
        /* [in] */ IContext* context);

    /**
     * Starts a dream when docked if the system has been configured to do so,
     * otherwise does nothing.
     */
    CARAPI StartDreamWhenDockedIfAppropriate(
        /* [in] */ IContext* context);
};

} // namespace Dreams
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_DREAMS_CSANDMAN_H__
