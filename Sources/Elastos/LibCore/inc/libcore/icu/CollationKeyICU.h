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

#ifndef __LIBCORE_ICU_COLLATIONKEYICU_H__
#define __LIBCORE_ICU_COLLATIONKEYICU_H__

#include "Elastos.CoreLibrary.Libcore.h"
#include "text/CollationKey.h"

using Elastos::Text::CollationKey;
using Elastos::Text::ICollationKey;

namespace Libcore {
namespace ICU {

class CollationKeyICU
    : public CollationKey
    , public ICollationKeyICU
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ const String& source,
        /* [in] */ ArrayOf<Byte>* bytes);

    CARAPI CompareTo(
        /* [in] */ IInterface* value,
        /* [out] */ Int32* result);

    CARAPI Equals(
        /* [in] */ IInterface * object,
        /* [out] */ Boolean * value);

    CARAPI GetHashCode(
        /* [out] */ Int32 * value);

    CARAPI ToByteArray(
        /* [out, callee] */ ArrayOf<Byte>** array);

private:
    /**
     * The key.
     */
    AutoPtr<ArrayOf<Byte> > mBytes;

    Int32 mHashCode;
};

} // namespace ICU
} // namespace Libcore

#endif //__LIBCORE_ICU_COLLATIONKEYICU_H__
