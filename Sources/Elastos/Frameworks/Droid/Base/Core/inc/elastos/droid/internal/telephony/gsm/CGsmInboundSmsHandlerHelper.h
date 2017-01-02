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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CGSMINBOUNDSMSHANDLERHELPER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CGSMINBOUNDSMSHANDLERHELPER_H__

#include "Elastos.Droid.Content.h"
#include "_Elastos_Droid_Internal_Telephony_Gsm_CGsmInboundSmsHandlerHelper.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Telephony::IPhoneBase;
using Elastos::Droid::Internal::Telephony::ISmsStorageMonitor;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

CarClass(CGsmInboundSmsHandlerHelper)
    , public Singleton
    , public IGsmInboundSmsHandlerHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI MakeInboundSmsHandler(
        /* [in] */ IContext* context,
        /* [in] */ ISmsStorageMonitor* storageMonitor,
        /* [in] */ IPhoneBase* phone,
        /* [out] */ IGsmInboundSmsHandler** result);
};

} // namespace Gem
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CGSMINBOUNDSMSHANDLERHELPER_H__
