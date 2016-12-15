
#include "org/alljoyn/bus/CVariant.h"
#include "org/alljoyn/bus/MsgArg.h"
#include "org/alljoyn/bus/Signature.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>
#include <alljoyn/MsgArg.h>

using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
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
    *signature = NULL;

    if (mValue != NULL) {
        *signature = Signature::TypeSig(mValue, mSignature);
    }
    else if (mHandle != 0) {
        AutoPtr<ArrayOf<Int64> > array = ArrayOf<Int64>::Alloc(1);
        (*array)[0] = mHandle;
        *signature = MsgArg::GetSignature(array);
    }

    return NOERROR;
}

ECode CVariant::GetObject(
    /* [in] */ CarDataType type,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)
    if (type == CarDataType_ArrayOf)
        return E_ILLEGAL_ARGUMENT_EXCEPTION;

    if (mValue == NULL) {
        AutoPtr<MsgArg::CarValue> value = new MsgArg::CarValue(type);
        PVoid arg = value->ToValuePtr();
        if (SUCCEEDED(MsgArg::Unmarshal(mHandle, type, arg)))
            mValue = value->Convert();
    }
    // @SuppressWarnings(value = "unchecked")
    *obj = mValue;
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode CVariant::GetArrayObject(
    /* [in] */ CarDataType elementType,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)
    if (mValue == NULL) {
        AutoPtr<MsgArg::CarValue> value = new MsgArg::CarValue(CarDataType_ArrayOf);
        value->mElementType = elementType;
        PVoid arg = value->ToValuePtr();
        if (SUCCEEDED(MsgArg::Unmarshal(mHandle, CarDataType_ArrayOf, arg)))
            mValue = value->Convert();
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

// ECode CVariant::GetObject(
//     /* [in] */ IVariantTypeReference* type,
//     /* [out] */ IInterface** obj)
// {
//     VALIDATE_NOT_NULL(obj)
//     if (mValue == NULL) {
//         // Type sc = type.getClass().getGenericSuperclass();
//         // value = MsgArg.unmarshal(handle, ((ParameterizedType) sc).getActualTypeArguments()[0]);
//     }
//     // @SuppressWarnings(value = "unchecked")
//     *obj = mValue;
//     REFCOUNT_ADD(*obj)
//     return NOERROR;
// }

ECode CVariant::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = FALSE;
    if (IVariant::Probe(obj) == NULL)
        return NOERROR;

    CVariant* other = (CVariant*)IVariant::Probe(obj);
    if (mValue != NULL && other->mValue != NULL)
        *res = Object::Equals(mValue, other->mValue);
    else if (mHandle != 0 && other->mHandle != 0)
        *res = *reinterpret_cast<ajn::MsgArg*>(mHandle) == *reinterpret_cast<ajn::MsgArg*>(other->mHandle);
    else if (mValue == NULL && other->mValue == NULL && !mHandle && !other->mHandle)
        *res = TRUE;

    return NOERROR;
}

ECode CVariant::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("CVariant{");
    sb += StringUtils::ToHexString((Int32)this);
    sb += ", value:";
    sb += Object::ToString(mValue);
    sb += ", signature:";
    sb += mSignature;
    sb += "}";
    *str = sb.ToString();
    return NOERROR;
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
