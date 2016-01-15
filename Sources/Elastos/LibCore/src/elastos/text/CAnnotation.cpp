
#include "CAnnotation.h"
#include "StringBuilder.h"
#include "StringUtils.h"

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Text {

CAR_INTERFACE_IMPL(CAnnotation, Object, IAnnotation)

CAR_OBJECT_IMPL(CAnnotation)

ECode CAnnotation::constructor(
    /* [in] */ IInterface * attribute)
{
    mValue = attribute;
    return NOERROR;
}

ECode CAnnotation::GetValue(
    /* [out] */ IInterface ** value)
{
    VALIDATE_NOT_NULL(value);
    *value = mValue;
    REFCOUNT_ADD(*value);
    return NOERROR;
}

ECode CAnnotation::ToString(
    /* [out] */ String * str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("CAnnotation[value=");
    IObject* obj = IObject::Probe(mValue);
    String objInfo;
    if (obj != NULL) {
        obj->ToString(&objInfo);
    }
    else {
        objInfo = StringUtils::ToHexString((Int32)mValue.Get());
    }
    sb.Append(objInfo);

    *str = sb.ToString();
    return NOERROR;
}

} // namespace Text
} // namespace Elastos

