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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CSMSHEADERHELPER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CSMSHEADERHELPER_H__

#include "_Elastos_Droid_Internal_Telephony_CSmsHeaderHelper.h"
#include "_Elastos.Droid.Internal.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CSmsHeaderHelper)
    , public Singleton
    , public ISmsHeaderHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Create structured SmsHeader object from serialized byte array representation.
     * (see TS 23.040 9.2.3.24)
     * @param data is user data header bytes
     * @return SmsHeader object
     */
    CARAPI FromByteArray(
        /* [in] */ ArrayOf<Byte>* data,
        /* [out] */ ISmsHeader** result);

    /**
     * Create serialized Byte array representation from structured SmsHeader object.
     * (see TS 23.040 9.2.3.24)
     * @return Byte array representing the SmsHeader
     */
    CARAPI ToByteArray(
        /* [in] */ ISmsHeader* smsHeader,
        /* [out, callee] */ ArrayOf<Byte>** result);

};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CSMSHEADERHELPER_H__
