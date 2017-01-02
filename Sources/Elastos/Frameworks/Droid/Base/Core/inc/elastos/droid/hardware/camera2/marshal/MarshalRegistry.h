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

#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_MARSHAL_MARSHALREGISTRY_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_MARSHAL_MARSHALREGISTRY_H__

#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Hardware::Camera2::Utils::ITypeReference;
using Elastos::Core::Object;
using Elastos::Utility::IList;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Marshal {

class MarshalRegistry
{
private:
    class MarshalToken
        : public Object
        , public IMarshalRegistryMarshalToken
    {
    public:
        CAR_INTERFACE_DECL()

        MarshalToken(
            /* [in] */ ITypeReference* typeReference,
            /* [in] */ Int32 nativeType);

        //@Override
        CARAPI Equals(
            /* [in] */ IInterface *obj,
            /* [out] */ Boolean *equal);

        //@Override
        CARAPI GetHashCode(
            /* [out] */ Int32 *hashCode);

    protected:
        AutoPtr<ITypeReference> mTypeReference;
        Int32 mNativeType;

    private:
        Int32 mHash;
    };

public:
    /**
     * Register a marshal queryable for the managed type {@code T}.
     *
     * <p>Multiple marshal queryables for the same managed type {@code T} may be registered;
     * this is desirable if they support different native types (e.g. marshaler 1 supports
     * {@code Integer <-> TYPE_INT32}, marshaler 2 supports {@code Integer <-> TYPE_BYTE}.</p>
     *
     * @param queryable a non-{@code null} marshal queryable that supports marshaling {@code T}
     */
    static CARAPI RegisterMarshalQueryable(
        /* [in] */ IMarshalQueryable* queryable);

    /**
     * Lookup a marshaler between {@code T} and {@code nativeType}.
     *
     * <p>Marshalers are looked up in the order they were registered; earlier registered
     * marshal queriers get priority.</p>
     *
     * @param typeToken The compile-time type reference for {@code T}
     * @param nativeType The native type, e.g. {@link CameraMetadataNative#TYPE_BYTE TYPE_BYTE}
     * @return marshaler a non-{@code null} marshaler that supports marshaling the type combo
     *
     * @throws UnsupportedOperationException If no marshaler matching the args could be found
     */
    // @SuppressWarnings("unchecked")
    static CARAPI GetMarshaler(
        /* [in] */ ITypeReference* typeToken,
        /* [in] */ Int32 nativeType,
        /* [out] */ IMarshaler** outmar);

private:
    MarshalRegistry() {}


private:
    static AutoPtr<IList> sRegisteredMarshalQueryables;
    static AutoPtr<IHashMap> sMarshalerMap;
};

} // namespace Marshal
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_MARSHAL_MARSHALREGISTRY_H__
