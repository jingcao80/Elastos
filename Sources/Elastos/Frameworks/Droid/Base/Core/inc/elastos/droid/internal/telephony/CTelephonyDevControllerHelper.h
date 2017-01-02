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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CTELEPHONYDEVCONTROLLERHELPER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CTELEPHONYDEVCONTROLLERHELPER_H__

#include "_Elastos_Droid_Internal_Telephony_CTelephonyDevControllerHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CTelephonyDevControllerHelper)
    , public Singleton
    , public ITelephonyDevControllerHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI Create(
        /* [out] */ ITelephonyDevController** result);

    CARAPI GetInstance(
        /* [out] */ ITelephonyDevController** result);

    /**
     * each RIL call this interface to register/unregister the unsolicited hardware
     * configuration callback data it can provide.
     */
    CARAPI RegisterRIL(
        /* [in] */ ICommandsInterface* cmdsIf);

    CARAPI UnregisterRIL(
        /* [in] */ ICommandsInterface* cmdsIf);

    /**
     * get total number of registered modem.
     */
    CARAPI GetModemCount(
        /* [out] */ Int32* result);
};

} //namespace Elastos
} //namespace Droid
} //namespace Internal
} //namespace Telephony

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CTELEPHONYDEVCONTROLLERHELPER_H__
