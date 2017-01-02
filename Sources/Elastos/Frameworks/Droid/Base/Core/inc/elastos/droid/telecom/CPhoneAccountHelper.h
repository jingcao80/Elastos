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

#ifndef __ELASTOS_DROID_TELECOM_CPHONEACCOUNTHELPER_H__
#define __ELASTOS_DROID_TELECOM_CPHONEACCOUNTHELPER_H__

#include "_Elastos_Droid_Telecom_CPhoneAccountHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Telecom {

CarClass(CPhoneAccountHelper)
    , public Singleton
    , public IPhoneAccountHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI Builder(
        /* [in] */ IPhoneAccountHandle* accountHandle,
        /* [in] */ ICharSequence* label,
        /* [out] */ IPhoneAccountBuilder** res);
};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_CPHONEACCOUNTHELPER_H__