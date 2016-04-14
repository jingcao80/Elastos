
#include "elastos/droid/hardware/camera2/marshal/MarshalRegistry.h"
#include "elastos/droid/hardware/camera2/marshal/MarshalHelpers.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Hardware::Camera2::Marshal::MarshalHelpers;
using Elastos::Utility::Logging::Slogger;
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
    Int32 code;
    IObject::Probe(typeReference)->GetHashCode(&code);
    mHash =  code ^ nativeType;
}


ECode MarshalRegistry::MarshalToken::Equals(
    /* [in] */ IInterface *obj,
    /* [out] */ Boolean *equal)
{
    VALIDATE_NOT_NULL(equal)
    *equal = FALSE;

    if (IMarshalRegistryMarshalToken::Probe(obj) != NULL) {
        MarshalToken* otherToken =(MarshalToken*)IMarshalRegistryMarshalToken::Probe(obj);

        Boolean tmp;
        IObject::Probe(mTypeReference)->Equals(otherToken->mTypeReference, &tmp);
        *equal = tmp && (mNativeType == otherToken->mNativeType);
        return NOERROR;
    }
    return NOERROR;
}


ECode MarshalRegistry::MarshalToken::GetHashCode(
    /* [out] */ Int32 *hashCode)
{
    VALIDATE_NOT_NULL(hashCode)
    *hashCode = 0;

    *hashCode = mHash;
    return NOERROR;
}

Boolean MarshalRegistry::InitStaticBlock()
{
    CArrayList::New((IList**)&sRegisteredMarshalQueryables);
    CHashMap::New((IHashMap**)&sMarshalerMap);
    return TRUE;
}

AutoPtr<IList> MarshalRegistry::sRegisteredMarshalQueryables;
AutoPtr<IHashMap> MarshalRegistry::sMarshalerMap;
Boolean MarshalRegistry::initStaticBlock = InitStaticBlock();

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
        //throw new AssertionError("No available query marshalers registered");
        Slogger::E("MarshalRegistry", "No available query marshalers registered");
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
            // throw new UnsupportedOperationException(
            //         "Could not find marshaler that matches the requested " +
            //                 "combination of type reference " +
            //                 typeToken + " and native type " +
            //                 MarshalHelpers.toStringNativeType(nativeType));
            String str;
            IObject::Probe(typeToken)->ToString(&str);
            String type;
            MarshalHelpers::ToStringNativeType(nativeType, &type);
            Slogger::E("MarshalRegistry", "Could not find marshaler that matches the requested "
                            "combination of type reference %d and "
                            "native type %d",str.string(), type.string());
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
        }

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
