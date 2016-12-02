
#include "org/alljoyn/bus/CVariant.h"
#include "org/alljoyn/bus/MsgArg.h"
#include "org/alljoyn/bus/Signature.h"
#include <elastos/utility/logging/Logger.h>
#include <alljoyn/MsgArg.h>

using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Alljoyn {
namespace Bus {

CAR_INTERFACE_IMPL(CVariant, Object, IVariant)

CAR_OBJECT_IMPL(CVariant)

CVariant::CVariant()
    : mHandle(0)
{}

/** Releases resources. */
CVariant::~CVariant()
{
    Destroy();
}

ECode CVariant::constructor()
{
    return NOERROR;
}

ECode CVariant::constructor(
    /* [in] */ IInterface* value)
{
    mValue = value;
    return NOERROR;
}

ECode CVariant::constructor(
    /* [in] */ IInterface* value,
    /* [in] */ const String& signature)
{
    mValue = value;
    mSignature = signature;
    return NOERROR;
}

void CVariant::Destroy()
{
    ajn::MsgArg* arg = reinterpret_cast<ajn::MsgArg*>(mHandle);
    if (!arg) {
        return;
    }
    delete arg;
    mHandle = 0;
}

Int64 CVariant::GetMsgArg()
{
    return mHandle;
}

void CVariant::SetMsgArg(
    /* [in] */ Int64 msgArg)
{
    ajn::MsgArg* arg = (ajn::MsgArg*)msgArg;
    assert(ajn::ALLJOYN_VARIANT == arg->typeId);
    ajn::MsgArg* argCopy = new ajn::MsgArg(*arg->v_variant.val);
    if (!argCopy) {
        Logger::E("CVariant", "OutOfMemoryError");
        return;
    }
    mHandle = reinterpret_cast<Int64>(argCopy);
}

AutoPtr<IInterface> CVariant::GetValue()
{
    return mValue;
}

ECode CVariant::GetSignature(
    /* [out] */ String* signature)
{
    VALIDATE_NOT_NULL(signature)
    if (mValue != NULL) {
        // *signature = Signature::TypeSig(mValue.getClass(), mSignature);
    }
    else if(mHandle != 0) {
        AutoPtr<ArrayOf<Int64> > array = ArrayOf<Int64>::Alloc(1);
        (*array)[0] = mHandle;
        *signature = MsgArg::GetSignature(array);
    }
    else {
        *signature = NULL;
    }
    return NOERROR;
}

ECode CVariant::GetObject(
    /* [in] */ CarDataType type,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)
    if (mValue == NULL) {
        MsgArg::Unmarshal(mHandle, type, (PVoid)&mValue);
    }
    // @SuppressWarnings(value = "unchecked")
    *obj = mValue;
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

/**
 * Gets the object wrapped by this CVariant when object is specified by Class.
 *
 * @param <T> type returned from the getObject call.
 * @param type the type of the return value.
 * @return the value of the CVariant.
 * @throws BusException if CVariant data cannot be unmarshalled
 * @throws ClassCastException if CVariant data cannot be converted to requested type
 */
// public <T> T getObject(
//     /* [in] */ Class<T> type) throws BusException {
//     if (value == null) {
//         value = MsgArg.unmarshal(handle, type);
//     }
//     @SuppressWarnings(value = "unchecked")
//     T o = (T) value;
//     return o;
// }

ECode CVariant::GetObject(
    /* [in] */ IVariantTypeReference* type,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)
    if (mValue == NULL) {
        // Type sc = type.getClass().getGenericSuperclass();
        // value = MsgArg.unmarshal(handle, ((ParameterizedType) sc).getActualTypeArguments()[0]);
    }
    // @SuppressWarnings(value = "unchecked")
    *obj = mValue;
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode CVariant::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = FALSE;
    AutoPtr<IInterface> obj1;
    if (FAILED(GetObject(CarDataType_Interface, (IInterface**)&obj1)))
        return NOERROR;
    AutoPtr<IInterface> obj2;
    if (FAILED(IVariant::Probe(obj)->GetObject(CarDataType_Interface, (IInterface**)&obj2)))
        return NOERROR;
    *res = Object::Equals(obj1, obj2);
    return NOERROR;
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
