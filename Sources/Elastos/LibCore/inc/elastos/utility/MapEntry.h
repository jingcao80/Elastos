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

#ifndef __ELASTOS_UTILITY_MAPENTRY_H__
#define __ELASTOS_UTILITY_MAPENTRY_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/core/Object.h"

using Elastos::Core::ICloneable;
using Elastos::Core::Object;

namespace Elastos {
namespace Utility {

class MapEntry
    : public Object
    , public IMapEntry
    , public ICloneable
{
public:
    class Type
        : public Object
    {
    public:
        virtual CARAPI Get(
            /* [in] */ IMapEntry* entry,
            /* [out] */ IInterface** value) = 0;
    };

public:
    MapEntry(
        /* [in] */ IInterface* theKey)
        : mKey(theKey)
    {}

    MapEntry(
        /* [in] */ IInterface* theKey,
        /* [in] */ IInterface* theValue)
        : mKey(theKey)
        , mValue(theValue)
    {}

    CAR_INTERFACE_DECL()

    CARAPI Clone(
        /* [out] */ IInterface** outface);

    CARAPI Equals(
        /* [in] */ IInterface* entry,
        /* [out] */ Boolean* result);

    CARAPI GetKey(
        /* [out] */ IInterface** key);

    CARAPI GetValue(
        /* [out] */ IInterface** value);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI SetValue(
        /* [in] */ IInterface* object,
        /* [out] */ IInterface** oldObject);

    CARAPI ToString(
        /* [out] */ String* str);

public:
    AutoPtr<IInterface> mKey;
    AutoPtr<IInterface> mValue;
};

} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY__MAPENTRY_H__
