
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
    if (mValue == NULL && mHandle != 0) {
        String signature;
        GetSignature(&signature);
        ECode ec = MsgArg::UnmarshalInterface(mHandle, signature, (IInterface**)&mValue);
        if (FAILED(ec)) {
            Logger::E("CVariant", "failed to UnmarshalInterface from %08x with signature: %s",
                mHandle, signature.string());
        }
    }
    return mValue;
}

ECode CVariant::GetSignature(
    /* [out] */ String* signature)
{
    VALIDATE_NOT_NULL(signature)
    *signature = NULL;

    if (mHandle != 0) {
        AutoPtr<ArrayOf<Int64> > array = ArrayOf<Int64>::Alloc(1);
        (*array)[0] = mHandle;
        *signature = MsgArg::GetSignature(array);
    }
    else if (mValue != NULL) {
        *signature = Signature::TypeSig(mValue, mSignature);
    }

    return NOERROR;
}

ECode CVariant::GetObject(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)

    *obj = GetValue();
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode CVariant::GetArrayObject(
    /* [out] */ IArrayOf** obj)
{
    VALIDATE_NOT_NULL(obj)
    GetValue();
    *obj = IArrayOf::Probe(mValue);
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

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
    sb += Object::ToString(GetValue());
    sb += ", handle:";
    sb += StringUtils::ToHexString(mHandle);
    sb += ", signature:";
    if (mSignature.IsNull()) {
        String sig;
        GetSignature(&sig);
        sb += sig;
    }
    else {
        sb += mSignature;
    }
    sb += "}";
    *str = sb.ToString();
    return NOERROR;
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
