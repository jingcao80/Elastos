
#include "CParsePosition.h"
#include "StringBuilder.h"

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Text {

CAR_OBJECT_IMPL(CParsePosition)

CAR_INTERFACE_IMPL(CParsePosition, Object, IParsePosition)

CParsePosition::CParsePosition()
    : mCurrentPosition(0)
    , mErrorIndex(-1)
{}

ECode CParsePosition::constructor(
    /* [in] */ Int32 index)
{
    mCurrentPosition = index;
    return NOERROR;
}

ECode CParsePosition::GetErrorIndex(
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);

    *index = mErrorIndex;
    return NOERROR;
}

ECode CParsePosition::GetIndex(
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);

    *index = mCurrentPosition;
    return NOERROR;
}

ECode CParsePosition::SetErrorIndex(
    /* [in] */ Int32 index)
{
    mErrorIndex = index;
    return NOERROR;
}

ECode CParsePosition::SetIndex(
    /* [in] */ Int32 index)
{
    mCurrentPosition = index;
    return NOERROR;
}

ECode CParsePosition::Equals(
    /* [in] */ IInterface * object,
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    IParsePosition* pp = IParsePosition::Probe(object);
    if (pp == NULL) {
        return NOERROR;
    }
    CParsePosition* pos = (CParsePosition*) pp;
    *result = mCurrentPosition == pos->mCurrentPosition
            && mErrorIndex == pos->mErrorIndex;
    return NOERROR;
}

ECode CParsePosition::GetHashCode(
    /* [out] */ Int32 * hash)
{
    VALIDATE_NOT_NULL(hash)
    *hash = mCurrentPosition + mErrorIndex;
    return NOERROR;
}

ECode CParsePosition::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("CParsePosition[index=");
    sb += mCurrentPosition;
    sb += ", errorIndex=";
    sb += mErrorIndex;
    sb += "]";
    *str = sb.ToString();
    return NOERROR;
}

} // namespace Text
} // namespace Elastos
