
#include "CFieldPosition.h"
#include "StringBuilder.h"

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Text {

CAR_OBJECT_IMPL(CFieldPosition)

CAR_INTERFACE_IMPL(CFieldPosition, Object, IFieldPosition)

CFieldPosition::CFieldPosition()
    : mMyField(0)
    , mBeginIndex(0)
    , mEndIndex(0)
{}

ECode CFieldPosition::constructor(
    /* [in] */ Int32 field)
{
    mMyField = field;
    return NOERROR;
}

ECode CFieldPosition::constructor(
    /* [in] */ IAttributedCharacterIteratorAttribute* attribute)
{
    mMyAttribute = attribute;
    mMyField = -1;
    return NOERROR;
}

ECode CFieldPosition::constructor(
    /* [in] */ IAttributedCharacterIteratorAttribute* attribute,
    /* [in] */ Int32 field)
{
    mMyAttribute = attribute;
    mMyField = field;
    return NOERROR;
}

ECode CFieldPosition::Equals(
    /* [in] */ IInterface * object,
    /* [out] */ Boolean * value)
{
    VALIDATE_NOT_NULL(value)

    IFieldPosition* pos = IFieldPosition::Probe(object);
    if (!pos) {
        *value = FALSE;
    }
    else {
        CFieldPosition* cp = (CFieldPosition*)pos;
        *value = mMyField == cp->mMyField &&
             mMyAttribute == cp->mMyAttribute &&
             mBeginIndex == cp->mBeginIndex &&
             mEndIndex == cp->mEndIndex;
    }
    return NOERROR;
}

ECode CFieldPosition::Clear()
{
    mBeginIndex = mEndIndex = 0;
    return NOERROR;
}

ECode CFieldPosition::GetBeginIndex(
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);

    *index = mBeginIndex;
    return NOERROR;
}

ECode CFieldPosition::GetEndIndex(
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);

    *index = mEndIndex;
    return NOERROR;
}

ECode CFieldPosition::GetField(
    /* [out] */ Int32* field)
{
    VALIDATE_NOT_NULL(field);

    *field = mMyField;
    return NOERROR;
}

ECode CFieldPosition::GetFieldAttribute(
    /* [out] */ IAttributedCharacterIteratorAttribute** attribute)
{
    VALIDATE_NOT_NULL(attribute);

    *attribute = mMyAttribute;
    REFCOUNT_ADD(*attribute);
    return NOERROR;
}

ECode CFieldPosition::GetHashCode(
    /* [out] */ Int32 * value)
{
    Int32 attributeHash = 0;
    if (mMyAttribute) {
        attributeHash = Object::GetHashCode(mMyAttribute);
    }
    *value = attributeHash + mMyField * 10 + mBeginIndex * 100 + mEndIndex;
    return NOERROR;
}

ECode CFieldPosition::SetBeginIndex(
    /* [in] */ Int32 index)
{
    mBeginIndex = index;
    return NOERROR;
}

ECode CFieldPosition::SetEndIndex(
    /* [in] */ Int32 index)
{
    mEndIndex = index;
    return NOERROR;
}

ECode CFieldPosition::ToString(
    /* [out] */ String * str)
{
    VALIDATE_NOT_NULL(str)

    StringBuilder sb("CFieldPosition[attribute=");
    sb.Append(Object::ToString(mMyAttribute));
    sb.Append(", field=");
    sb.Append(mMyField);
    sb.Append(", beginIndex=");
    sb.Append(mBeginIndex);
    sb.Append(", endIndex=");
    sb.Append(mEndIndex);
    sb.Append("]");
    *str = sb.ToString();
    return NOERROR;
}

} // namespace Text
} // namespace Elastos
