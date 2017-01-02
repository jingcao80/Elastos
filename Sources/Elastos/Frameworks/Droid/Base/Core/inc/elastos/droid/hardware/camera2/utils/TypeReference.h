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

#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_TYPEREFERENCE_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_TYPEREFERENCE_H__

#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Utils {

class TypeReference
    : public Object
    , public ITypeReference
{
public:
    CAR_INTERFACE_DECL()

    TypeReference();

    virtual ~TypeReference() {}

    /**
     * Create a new type reference for {@code T}.
     *
     * @throws IllegalArgumentException if {@code T}'s actual type contains a type variable
     *
     * @see TypeReference
     */
    CARAPI constructor(
        /* [in] */ const ClassID& classId,
        /* [in] */ const InterfaceID& interfaceId);

    CARAPI constructor(
        /* [in] */ const ClassID& classId,
        /* [in] */ const InterfaceID& interfaceId,
        /* [in] */ ITypeReference* component);

    /**
     * Return the dynamic {@link Type} corresponding to the captured type {@code T}.
     */
    CARAPI GetClassType(
        /* [out] */ ClassID* classId);

    CARAPI GetInterfaceType(
        /* [out] */ InterfaceID* interfaceId);

    /**
     * Get the component type, e.g. {@code T} from {@code T[]}.
     *
     * @return component type, or {@code null} if {@code T} is not an array
     */
    CARAPI GetComponent(
        /* [out] */ ITypeReference** reference);

    static AutoPtr<ITypeReference> CreateSpecializedTypeReference(
        /* [in] */ const ClassID& classId,
        /* [in] */ const InterfaceID& interfaceId);

    static AutoPtr<ITypeReference> CreateSpecializedTypeReference(
        /* [in] */ const ClassID& classId,
        /* [in] */ const InterfaceID& interfaceId,
        /* [in] */ const ClassID& componentClassId,
        /* [in] */ const InterfaceID& componentInterfaceId);

    static AutoPtr<ITypeReference> CreateSpecializedTypeReference(
        /* [in] */ const ClassID& classId,
        /* [in] */ const InterfaceID& interfaceId,
        /* [in] */ ITypeReference* component);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hash);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    Int32 mHash;
    ClassID mClassId;
    InterfaceID mInterfaceId;

    AutoPtr<ITypeReference> mComponent;
};

} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_TYPEREFERENCE_H__
