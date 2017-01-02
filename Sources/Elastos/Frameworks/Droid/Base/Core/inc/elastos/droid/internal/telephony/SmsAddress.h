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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_SMSADDRESS_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_SMSADDRESS_H__

#include "_Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

class SmsAddress
    : public Object
    , public ISmsAddress
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Returns the address of the SMS message in String form or NULL if unavailable
     */
    CARAPI GetAddressString(
        /* [out] */ String* result);

    /**
     * Returns TRUE if this is an alphanumeric address
     */
    CARAPI IsAlphanumeric(
        /* [out] */  Boolean* result);

    /**
     * Returns TRUE if this is a network address
     */
    CARAPI IsNetworkSpecific(
        /* [out] */ Boolean* result);

    CARAPI CouldBeEmailGateway(
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* result);
public:
    //TODO
    Int32 ton;
    String address;
    AutoPtr<ArrayOf<Byte> > origBytes;

};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_SMSADDRESS_H__
