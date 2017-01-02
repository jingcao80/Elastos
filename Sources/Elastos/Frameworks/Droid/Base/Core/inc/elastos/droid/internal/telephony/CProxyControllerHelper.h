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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CPROXYCONTROLLERHELPER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CPROXYCONTROLLERHELPER_H__

#include "_Elastos_Droid_Internal_Telephony_CProxyControllerHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccController;

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CProxyControllerHelper)
    , public Singleton
    , public IProxyControllerHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetInstance(
        /* [in] */ IContext* context,
        /* [in] */ ArrayOf<IPhone*>* phoneProxy,
        /* [in] */ IUiccController* uiccController,
        /* [in] */ ArrayOf<ICommandsInterface*>* ci,
        /* [out] */ IProxyController** result);

    CARAPI GetInstance(
        /* [out] */ IProxyController** result);
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CPROXYCONTROLLERHELPER_H__
