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

#include "elastos/droid/hardware/camera2/marshal/MarshalRegistry.h"
#include "elastos/droid/hardware/camera2/marshal/MarshalHelpers.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Hardware::Camera2::Marshal::MarshalHelpers;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Marshal {

CAR_INTERFACE_IMPL(MarshalRegistry::MarshalToken, Object, IMarshalRegistryMarshalToken)

MarshalRegistry::MarshalToken::MarshalToken(
    /* [in] */ ITypeReference* typeReference,
    /* [in] */ Int32 nativeType)
    : mTypeReference(typeReference)
    , mNativeType(nativeType)
{
    mHash =  Object::GetHashCode(typeReference) ^ nativeType;
}

ECode MarshalRegistry::MarshalToken::Equals(
    /* [in] */ IInterface *obj,
    /* [out] */ Boolean *equal)
{
    VALIDATE_NOT_NULL(equal)
    *equal = FALSE;

    MarshalToken* otherToken =(MarshalToken*)IMarshalRegistryMarshalToken::Probe(obj);
    if (otherToken != NULL) {
        Boolean tmp = Object::Equals(mTypeReference, otherToken->mTypeReference);
        *equal = tmp && (mNativeType == otherToken->mNativeType);
        return NOERROR;
    }
    return NOERROR;
}

ECode MarshalRegistry::MarshalToken::GetHashCode(
    /* [out] */ Int32 *hashCode)
{
    VALIDATE_NOT_NULL(hashCode)
    *hashCode = mHash;
    return NOERROR;
}

static AutoPtr<IList> InitRegisteredMarshalQueryables()
{
    AutoPtr<IList> list;
    CArrayList::New((IList**)&list);
    return list;
}

static AutoPtr<IHashMap> InitMarshalerMap()
{
    AutoPtr<IHashMap> map;
    CHashMap::New((IHashMap**)&map);
    return map;
}

INIT_PROI_3 AutoPtr<IList> MarshalRegistry::sRegisteredMarshalQueryables = InitRegisteredMarshalQueryables();
INIT_PROI_3 AutoPtr<IHashMap> MarshalRegistry::sMarshalerMap = InitMarshalerMap();


ECode MarshalRegistry::RegisterMarshalQueryable(
    /* [in] */ IMarshalQueryable* queryable)
{
    return sRegisteredMarshalQueryables->Add(TO_IINTERFACE(queryable));
}

ECode MarshalRegistry::GetMarshaler(
    /* [in] */ ITypeReference* typeToken,
    /* [in] */ Int32 nativeType,
    /* [out] */ IMarshaler** outmar)
{
    VALIDATE_NOT_NULL(outmar);
    *outmar = NULL;

    // TODO: can avoid making a new token each time by code-genning
    // the list of type tokens and native types from the keys (at the call sites)
    AutoPtr<IMarshalRegistryMarshalToken> marshalToken = new MarshalToken(typeToken, nativeType);

    /*
     * Marshalers are instantiated lazily once they are looked up; successive lookups
     * will not instantiate new marshalers.
     */
    AutoPtr<IInterface> value;
    sMarshalerMap->Get(TO_IINTERFACE(marshalToken), (IInterface**)&value);
    AutoPtr<IMarshaler> marshaler = IMarshaler::Probe(value);

    Int32 size;
    sRegisteredMarshalQueryables->GetSize(&size);
    if (size == 0) {
        Logger::E("MarshalRegistry", "No available query marshalers registered");
        return E_ASSERTION_ERROR;
    }

    if (marshaler == NULL) {
        // Query each marshaler to see if they support the native/managed type combination
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            sRegisteredMarshalQueryables->Get(i, (IInterface**)&obj);
            AutoPtr<IMarshalQueryable> potentialMarshaler = IMarshalQueryable::Probe(obj);

            Boolean result;
            potentialMarshaler->IsTypeMappingSupported(typeToken, nativeType, &result);
            if (result) {
                potentialMarshaler->CreateMarshaler(typeToken, nativeType, (IMarshaler**)&marshaler);
                break;
            }
        }

        if (marshaler == NULL) {
            String type;
            MarshalHelpers::ToStringNativeType(nativeType, &type);
            Logger::E("MarshalRegistry", "Could not find marshaler that matches the requested "
                "combination of type reference %s and native type %d, %s",
                TO_CSTR(typeToken), nativeType, type.string());
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
        }

        // String type;
        // MarshalHelpers::ToStringNativeType(nativeType, &type);
        // Logger::D("MarshalRegistry", ">> GetMarshaler %s for type:%d(%s), class:%s",
        //     TO_CSTR(marshaler), nativeType, type.string(), TO_CSTR(typeToken));

        // Only put when no cached version exists to avoid +0.5ms lookup per call.
        sMarshalerMap->Put(TO_IINTERFACE(marshalToken), TO_IINTERFACE(marshaler));
    }

    *outmar = marshaler;
    REFCOUNT_ADD(*outmar)
    return NOERROR;
}

} // namespace Marshal
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
