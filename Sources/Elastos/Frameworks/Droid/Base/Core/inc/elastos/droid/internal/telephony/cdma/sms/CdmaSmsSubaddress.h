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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SMS_CDMASMSSUBADDRESS_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SMS_CDMASMSSUBADDRESS_H__

#include "Elastos.Droid.Internal.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

// package com.android.internal.telephony.cdma.sms;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {
namespace Sms {

class CdmaSmsSubaddress
    : public Object
    , public ICdmaSmsSubaddress
{
public:
    CAR_INTERFACE_DECL();

    CARAPI SetType(
        /* [in] */ Int32 type);

    CARAPI GetType(
        /* [out] */ Int32* result);

    CARAPI SetOdd(
        /* [in] */ Byte odd);

    CARAPI GetOdd(
        /* [out] */ Byte* result);

    CARAPI SetOrigBytes(
        /* [in] */ ArrayOf<Byte>* origBytes);

    CARAPI GetOrigBytes(
        /* [out, callee] */ ArrayOf<Byte>** result);

    Int32 type;
    Byte odd;
    AutoPtr<ArrayOf<Byte> > origBytes;
};

} // namespace Sms
} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SMS_CDMASMSSUBADDRESS_H__

