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

#ifndef __ELASTOS_CORE_CBOOLEAN_H__
#define __ELASTOS_CORE_CBOOLEAN_H__

#include "_Elastos_Core_CBoolean.h"
#include "Object.h"

using Elastos::IO::ISerializable;

namespace Elastos {
namespace Core {

CarClass(CBoolean)
    , public Object
    , public IBoolean
    , public ISerializable
    , public IComparable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Boolean value);

    CARAPI GetValue(
        /* [out] */ Boolean* result);

    CARAPI CompareTo(
        /* [in] */ IInterface* other,
        /* [out] */ Int32* result);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI ToString(
        /* [out] */ String* result);

private:
    Boolean mValue;
};

} // namespace Core
} // namespace Elastos

#endif //__ELASTOS_CORE_CBOOLEAN_H__
