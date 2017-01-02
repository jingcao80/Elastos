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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CSYNTHETICSMSMESSAGEHELPER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CSYNTHETICSMSMESSAGEHELPER_H__

#include "_Elastos_Droid_Internal_Telephony_CSyntheticSmsMessageHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CSyntheticSmsMessageHelper)
    , public Singleton
    , public ISyntheticSmsMessageHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI IsSyntheticPdu(
        /* [in] */ ArrayOf<Byte>* pdu,
        /* [out] */ Boolean* result);

    CARAPI CreateFromPdu(
        /* [in] */ ArrayOf<Byte>* pdu,
        /* [out] */ ISyntheticSmsMessage** result);
};

} //namespace Elastos
} //namespace Droid
} //namespace Internal
} //namespace Telephony

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CSYNTHETICSMSMESSAGEHELPER_H__
