
#ifndef __ORG_ALLJOYN_BUS_CVARIANT_H__
#define __ORG_ALLJOYN_BUS_CVARIANT_H__

#include "_Org_Alljoyn_Bus_CVariant.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Alljoyn {
namespace Bus {

CarClass(CVariant)
    , public Object
    , public IVariant
{
public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    CVariant();

    /** Releases resources. */
    ~CVariant();

    /**
     * Creates a variant that can represent any AllJoyn data type.
     * This constructor is typically used by message receivers (method and
     * signal handlers) when the actual data type represented by the variant is
     * not known.
     */
    CARAPI constructor();

    /**
     * Creates a Variant that represents a specific AllJoyn data type and value.
     * This constructor is typically used by message senders (method calls and
     * signal emitters) when a specific Variant type and value are known at the
     * time the Variant is created.
     *
     * @param <T> the Java type the Variant contains
     * @param value object wrapped inside this Variant
     */
    CARAPI constructor(
        /* [in] */ IInterface* value);

    /**
     * Creates a Variant with an explicit signature that represents a specific
     * AllJoyn data type and instance value.
     * This constructor is typically used by message senders (method calls and
     * signal emitters) when a specific Variant type and value are known at the
     * time the Variant is created.
     *
     * @param <T> the Java type the Variant contains
     * @param value object wrapped inside the Variant
     * @param signature the DBus signature of value
     */
    CARAPI constructor(
        /* [in] */ IInterface* value,
        /* [in] */ const String& signature);

    /**
     * Gets the DBus type signature of the wrapped object.
     *
     * @throws AnnotationBusException This exception will occur if
     * <ul>
     * <li>a field of a user-defined type is not annotated with its position,
     * <li>a Java data type that is not supported is used,
     * <li>an Enum data type is not annotated with a valid AllJoyn type
     * </ul>
     * @return the signature of the wrapped object.
     */
    CARAPI GetSignature(
        /* [out] */ String* signature);

    /**
     * Gets the object wrapped by this Variant when object is specified by Type.
     *
     * @param <T> type returned from the getObject call.
     * @param type the type of the return value.
     * @return the value of the Variant.
     * @throws BusException if Variant data cannot be unmarshalled
     * @throws ClassCastException if Variant data cannot be converted to requested type
     */
    CARAPI GetObject(
        /* [in] */ CarDataType type,
        /* [out] */ IInterface** obj);

    /**
     * Gets the object wrapped by this Variant when the wrapped object is a
     * generic type.
     *
     * @param <T> type returned from the getObject call.
     * @param type the VariantTypeReference of the return value
     * @return the value of the Variant
     * @throws BusException if Variant data cannot be unmarshalled
     * @throws ClassCastException if Variant data cannot be converted to requested type
     * @see VariantTypeReference
     */
    CARAPI GetObject(
        /* [in] */ IVariantTypeReference* type,
        /* [out] */ IInterface** obj);

    /**
     * Indicates whether some other Variant is "equal to" this one.
     *
     * @param obj the other Variant
     * @return {@code true} when variants values are equal to one another
     */
    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* res);

public:
    /** Gets the wrapped native object. */
    CARAPI_(Int64) GetMsgArg();

    /** Sets the wrapped native object to the MsgArg. */
    CARAPI_(void) SetMsgArg(
        /* [in] */ Int64 msgArg);

    /** Gets the wrapped object. */
    CARAPI_(AutoPtr<IInterface>) GetValue();

private:
    /** Releases native resources */
    CARAPI_(void) Destroy();

private:
    /** Wrapped object */
    AutoPtr<IInterface> mValue;

    /** Wrapped object signature */
    String mSignature;

    /** Wrapped native object */
    Int64 mHandle;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_CVARIANT_H__
