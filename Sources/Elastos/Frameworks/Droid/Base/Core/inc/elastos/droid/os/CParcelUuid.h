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

#ifndef __ELASTOS_DROID_OS_CPARCELUUID_H__
#define __ELASTOS_DROID_OS_CPARCELUUID_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "_Elastos_Droid_Os_CParcelUuid.h"
#include <elastos/core/Object.h>

using Elastos::Utility::IUUID;

namespace Elastos {
namespace Droid {
namespace Os {

/**
 * This class is a Parcelable wrapper around {@link UUID} which is an
 * immutable representation of a 128-bit universally unique
 * identifier.
 */
CarClass(CParcelUuid)
    , public Object
    , public IParcelUuid
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CParcelUuid();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IUUID* pfd);

    /**
     * Creates a new ParcelUuid from a string representation of {@link UUID}.
     *
     * @param uuid
     *            the UUID string to parse.
     * @return a ParcelUuid instance.
     * @throws NullPointerException
     *             if {@code uuid} is {@code null}.
     * @throws IllegalArgumentException
     *             if {@code uuid} is not formatted correctly.
     */
    static CARAPI FromString(
        /* [in] */ const String& uuid,
        /* [out] */ IParcelUuid** id);

    CARAPI GetUuid(
        /* [out] */ IUUID** uuid);

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hash);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    AutoPtr<IUUID> mUuid;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OS_CPARCELUUID_H__
