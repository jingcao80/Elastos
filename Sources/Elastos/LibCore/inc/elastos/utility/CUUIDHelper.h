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

#ifndef __ELASTOS_UTILITY_CUUIDHELPER_H__
#define __ELASTOS_UTILITY_CUUIDHELPER_H__

#include "_Elastos_Utility_CUUIDHelper.h"
#include "Singleton.h"


namespace Elastos {
namespace Utility {

CarClass(CUUIDHelper)
    , public Singleton
    , public IUUIDHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * <p>
     * Generates a variant 2, version 4 (randomly generated number) UUID as per
     * <a href="http://www.ietf.org/rfc/rfc4122.txt">RFC 4122</a>.
     *
     * @return an UUID instance.
     */
    CARAPI RandomUUID(
        /* [out] */ IUUID** uuid);

    /**
     * <p>
     * Generates a variant 2, version 3 (name-based, MD5-hashed) UUID as per <a
     * href="http://www.ietf.org/rfc/rfc4122.txt">RFC 4122</a>.
     *
     * @param name
     *            the name used as byte array to create an UUID.
     * @return an UUID instance.
     */
    CARAPI NameUUIDFromBytes(
        /* [in] */ ArrayOf<Byte>* name,
        /* [out] */ IUUID** uuid);

    /**
     * <p>
     * Parses a UUID string with the format defined by {@link #toString()}.
     *
     * @param uuid
     *            the UUID string to parse.
     * @return an UUID instance.
     * @throws NullPointerException
     *             if {@code uuid} is {@code null}.
     * @throws IllegalArgumentException
     *             if {@code uuid} is not formatted correctly.
     */
    CARAPI FromString(
        /* [in] */ const String& name,
        /* [out] */ IUUID** uuid);
};

} // namespace Utility
} // namespace Elastos

#endif //__CUUIDHELPER_H__
