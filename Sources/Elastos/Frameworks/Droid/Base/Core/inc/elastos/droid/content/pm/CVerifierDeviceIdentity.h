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

#ifndef __ELASTOS_DROID_CONTENT_PM_CVERIFIERDEVICEIDENTITY_H__
#define __ELASTOS_DROID_CONTENT_PM_CVERIFIERDEVICEIDENTITY_H__

#include "_Elastos_Droid_Content_Pm_CVerifierDeviceIdentity.h"
#include "CVerifierDeviceIdentityHelper.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CarClass(CVerifierDeviceIdentity)
    , public Object
    , public IVerifierDeviceIdentity
    , public IParcelable
{
    friend class CVerifierDeviceIdentityHelper;

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CVerifierDeviceIdentity();

    ~CVerifierDeviceIdentity();

    CARAPI constructor();

    /**
     * Create a verifier device identity from a long.
     *
     * @param identity device identity in a 64-bit integer.
     * @throws
     */
    CARAPI constructor(
        /* [in] */ Int64 identity);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* isEquals);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    CARAPI constructor(
        /* [in] */ IParcel* source);

    static String EncodeBase32(
        /* [in] */ Int64 input);

    static CARAPI DecodeBase32(
        /* [in] */ ArrayOf<Byte>* input,
        /* [out] */ Int64* value);

private:
    Int64 mIdentity;

    String mIdentityString;

    /**
     * Encoded size of a long (64-bit) into Base32. This format will end up
     * looking like XXXX-XXXX-XXXX-X (length ignores hyphens) when applied with
     * the GROUP_SIZE below.
     */
    static const Int32 LONG_SIZE;

    /**
     * Size of groupings when outputting as strings. This helps people read it
     * out and keep track of where they are.
     */
    static const Int32 GROUP_SIZE;

    static const AutoPtr<ArrayOf<Char32> > ENCODE;

    static const Char32 SEPARATOR;

};

}
}
}
}

#endif // __ELASTOS_DROID_CONTENT_PM_CVERIFIERDEVICEIDENTITY_H__
