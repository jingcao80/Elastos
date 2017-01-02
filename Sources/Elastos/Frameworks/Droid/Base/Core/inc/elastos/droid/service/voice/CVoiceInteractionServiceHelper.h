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

#ifndef __ELASTOS_DROID_SERVICE_VOICE_CVOICEINTERACTIONSERVICEHELPER_H__
#define __ELASTOS_DROID_SERVICE_VOICE_CVOICEINTERACTIONSERVICEHELPER_H__

#include "_Elastos_Droid_Service_Voice_CVoiceInteractionServiceHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Service::Voice::IVoiceInteractionServiceHelper;
using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Voice {

CarClass(CVoiceInteractionServiceHelper)
    , public Singleton
    , public IVoiceInteractionServiceHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Check whether the given service component is the currently active
     * VoiceInteractionService.
     */
    CARAPI IsActiveService(
        /* [in] */ IContext* context,
        /* [in] */ IComponentName* service,
        /* [out] */ Boolean* isActiveService);
};

} // namespace Voice
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_VOICE_CVOICEINTERACTIONSERVICEHELPER_H__
