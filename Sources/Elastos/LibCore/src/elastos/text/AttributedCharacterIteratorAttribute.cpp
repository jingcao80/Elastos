
#include "StringBuilder.h"
#include "AttributedCharacterIteratorAttribute.h"
#include "CAttributedCharacterIteratorAttribute.h"
#include <utils/Log.h>

using Elastos::Core::StringBuilder;
using Elastos::IO::EIID_ISerializable;
using Elastos::Text::CAttributedCharacterIteratorAttribute;

namespace Elastos {
namespace Text {

static AutoPtr<IAttributedCharacterIteratorAttribute> InitAttribute(const String& name)
{
    AutoPtr<CAttributedCharacterIteratorAttribute> attribute;
    ASSERT_SUCCEEDED(CAttributedCharacterIteratorAttribute::NewByFriend(
            name, (CAttributedCharacterIteratorAttribute**)&attribute));
    return (IAttributedCharacterIteratorAttribute*)attribute.Get();
}

const AutoPtr<IAttributedCharacterIteratorAttribute> AttributedCharacterIteratorAttribute::INPUT_METHOD_SEGMENT =
        InitAttribute(String("input_method_segment"));
const AutoPtr<IAttributedCharacterIteratorAttribute> AttributedCharacterIteratorAttribute::LANGUAGE =
        InitAttribute(String("language"));
const AutoPtr<IAttributedCharacterIteratorAttribute> AttributedCharacterIteratorAttribute::READING =
        InitAttribute(String("reading"));

CAR_INTERFACE_IMPL_2(AttributedCharacterIteratorAttribute, Object, IAttributedCharacterIteratorAttribute, ISerializable)

ECode AttributedCharacterIteratorAttribute::GetClassID(
    /* [out] */ ClassID* clsid)
{
    assert(0 && "Not a CAR class!");
    return E_NOT_IMPLEMENTED;
}

ECode AttributedCharacterIteratorAttribute::constructor(
    /* [in] */ const String& name)
{
    mName = name;
    return NOERROR;
}

ECode AttributedCharacterIteratorAttribute::Equals(
    /* [in] */ IInterface * obj,
    /* [out] */ Boolean * value)
{
    VALIDATE_NOT_NULL(value)
    *value = FALSE;
    VALIDATE_NOT_NULL(obj)

    IAttributedCharacterIteratorAttribute* o = IAttributedCharacterIteratorAttribute::Probe(obj);
    if (o != NULL) {
        *value = (o == this);
    }
    return NOERROR;
}

ECode AttributedCharacterIteratorAttribute::GetHashCode(
    /* [out] */ Int32 * value)
{
    VALIDATE_NOT_NULL(value)
    return Object::GetHashCode(value);
}

ECode AttributedCharacterIteratorAttribute::ToString(
    /* [out] */ String * str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("AttributedCharacterIteratorAttribute(");
    sb += mName;
    sb += ")";
    *str = sb.ToString();
    return NOERROR;
}

ECode AttributedCharacterIteratorAttribute::GetName(
    /* [out] */ String * str)
{
    VALIDATE_NOT_NULL(str)
    *str = mName;
    return NOERROR;
}

ECode AttributedCharacterIteratorAttribute::ReadResolve(
    /* [out] */ IInterface ** obj)
{
    VALIDATE_NOT_NULL(obj);
    *obj = NULL;

    ClassID clsid;
    GetClassID(&clsid);
    if (clsid != ECLSID_CAttributedCharacterIteratorAttribute) {
        ALOGE("AttributedCharacterIteratorAttribute::ReadResolve: cannot resolve subclasses");
        return E_INVALID_OBJECT_EXCEPTION;
    }

    String name;
    INPUT_METHOD_SEGMENT->GetName(&name);
    if (mName.Equals(name)) {
        *obj = INPUT_METHOD_SEGMENT;
        REFCOUNT_ADD(*obj);
        return NOERROR;
    }
    LANGUAGE->GetName(&name);
    if (mName.Equals(name)) {
        *obj = LANGUAGE;
        REFCOUNT_ADD(*obj);
        return NOERROR;
    }
    READING->GetName(&name);
    if (mName.Equals(name)) {
        *obj = READING;
        REFCOUNT_ADD(*obj);
        return NOERROR;
    }

    ALOGE("AttributedCharacterIteratorAttribute::ReadResolve: Unknown attribute");
    return E_INVALID_OBJECT_EXCEPTION;
}

} // namespace Text
} // namespace Elastos
