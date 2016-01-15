
#include "org/apache/http/message/BasicHeaderElement.h"
#include "org/apache/http/utility/CCharArrayBuffer.h"
#include "elastos/core/CArrayOf.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::EIID_ICloneable;
using Elastos::Core::CArrayOf;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::Utility::ICharArrayBuffer;
using Org::Apache::Http::Utility::CCharArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

CAR_INTERFACE_IMPL_2(BasicHeaderElement, Object, IHeaderElement, ICloneable)

ECode BasicHeaderElement::constructor(
    /* [in] */ const String& name,
    /* [in] */ const String& value,
    /* [in] */ IArrayOf* parameters)/*INameValuePair*/
{
    if (name.IsNull()) {
        Logger::E("BasicHeaderElement", "Name may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mName = name;
    mValue = value;
    if (parameters != NULL) {
        mParameters = parameters;
    }
    else {
        CArrayOf::New(EIID_INameValuePair, 0, (IArrayOf**)&mParameters);
    }
    return NOERROR;
}

ECode BasicHeaderElement::constructor(
    /* [in] */ const String& name,
    /* [in] */ const String& value)
{
    return constructor(name, value, NULL);
}

ECode BasicHeaderElement::constructor()
{
    return NOERROR;
}

ECode BasicHeaderElement::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = mName;
    return NOERROR;
}

ECode BasicHeaderElement::GetValue(
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mValue;
    return NOERROR;
}

ECode BasicHeaderElement::GetParameters(
    /* [out, callee] */ ArrayOf<INameValuePair*>** _parameters)
{
    VALIDATE_NOT_NULL(_parameters)
    Int32 len;
    mParameters->GetLength(&len);
    AutoPtr< ArrayOf<INameValuePair*> > parameters = ArrayOf<INameValuePair*>::Alloc(len);
    for (Int32 i = 0; i < len; i++) {
        AutoPtr<IInterface> value;
        mParameters->Get(i, (IInterface**)&value);
        parameters->Set(i, INameValuePair::Probe(value));
    }
    *_parameters = parameters;
    REFCOUNT_ADD(*_parameters)
    return NOERROR;
}

ECode BasicHeaderElement::GetParameterCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    return mParameters->GetLength(count);
}

ECode BasicHeaderElement::GetParameter(
    /* [in] */ Int32 index,
    /* [out] */ INameValuePair** parameter)
{
    VALIDATE_NOT_NULL(parameter)
    AutoPtr<IInterface> value;
    mParameters->Get(index, (IInterface**)&value);
    *parameter = INameValuePair::Probe(value);
    REFCOUNT_ADD(*parameter)
    return NOERROR;
}

ECode BasicHeaderElement::GetParameterByName(
    /* [in] */ const String& name,
    /* [out] */ INameValuePair** parameter)
{
    VALIDATE_NOT_NULL(parameter)
    *parameter = NULL;

    if (name.IsNull()) {
        Logger::E("BasicHeaderElement", "Name may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<INameValuePair> found;
    Int32 len;
    mParameters->GetLength(&len);
    for (Int32 i = 0; i < len; i++) {
        AutoPtr<IInterface> value;
        mParameters->Get(i, (IInterface**)&value);
        AutoPtr<INameValuePair> current = INameValuePair::Probe(value);
        String n;
        current->GetName(&n);
        if (n.EqualsIgnoreCase(name)) {
            found = current;
            break;
        }
    }
    *parameter = found;
    REFCOUNT_ADD(*parameter)
    return NOERROR;
}

ECode BasicHeaderElement::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* equals)
{
    VALIDATE_NOT_NULL(equals)
    if (obj == NULL) {
        *equals = FALSE;
        return NOERROR;
    }
    if (this->Probe(EIID_IInterface) == obj) {
        *equals = TRUE;
        return NOERROR;
    }
    AutoPtr<IHeaderElement> elem = IHeaderElement::Probe(obj);
    if (elem != NULL) {
        AutoPtr<BasicHeaderElement> that = (BasicHeaderElement*)(IObject*)obj;
        Boolean result;
        *equals = mName.Equals(that->mName)
                && mValue.Equals(that->mValue)
                && (mParameters->DeepEquals(that->mParameters, &result), result);
        return NOERROR;
    }
    else {
        *equals = FALSE;
        return NOERROR;
    }
}

ECode BasicHeaderElement::ToString(
    /* [out] */ String* string)
{
    VALIDATE_NOT_NULL(string)
    AutoPtr<ICharArrayBuffer> buffer;
    CCharArrayBuffer::New(64, (ICharArrayBuffer**)&buffer);
    buffer->Append(mName);
    if (!mValue.IsNull()) {
        buffer->Append(String("="));
        buffer->Append(mValue);
    }
    Int32 len;
    mParameters->GetLength(&len);
    for (Int32 i = 0; i < len; i++) {
        buffer->Append(String("; "));
        AutoPtr<IInterface> value;
        mParameters->Get(i, (IInterface**)&value);
        buffer->Append(value);
    }
    return IObject::Probe(buffer)->ToString(string);
}

ECode BasicHeaderElement::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)
    // int hash = LangUtils.HASH_SEED;
    // hash = LangUtils.hashCode(hash, this.name);
    // hash = LangUtils.hashCode(hash, this.value);
    // for (int i = 0; i < this.parameters.length; i++) {
    //     hash = LangUtils.hashCode(hash, this.parameters[i]);
    // }
    // return hash;
    return E_NOT_IMPLEMENTED;
}

void BasicHeaderElement::CloneImpl(
    /* [in] */ BasicHeaderElement* obj)
{
    obj->mName = mName;
    obj->mValue = mValue;
    obj->mParameters = mParameters;
}

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org
