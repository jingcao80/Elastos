
#ifndef  __ELASTOS_DROID_HARDWARE_CAMERA2_MARSHAL_MARSHALREGISTRY_H__
#define  __ELASTOS_DROID_HARDWARE_CAMERA2_MARSHAL_MARSHALREGISTRY_H__

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

    static CARAPI_(Boolean) InitStaticBlock();

private:
    static AutoPtr<IList> sRegisteredMarshalQueryables;
    static AutoPtr<IHashMap> sMarshalerMap;

    static Boolean initStaticBlock;
};

} // namespace Marshal
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_MARSHAL_MARSHALREGISTRY_H__
