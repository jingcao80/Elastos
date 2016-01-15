#include "elastos/droid/view/CDispListDispFormat.h"
#include <elastos/core/StringBuffer.h>

using Elastos::Core::StringBuffer;

namespace Elastos {
namespace Droid {
namespace View {

CDispListDispFormat::CDispListDispFormat()
{}

ECode CDispListDispFormat::constructor(
    /* [in] */ Int32 type,
    /* [in] */ Int32 format)
{
    mOutputType = type;
    mFormat = format;
    return NOERROR;
}

ECode CDispListDispFormat::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* rst)
{
    VALIDATE_NOT_NULL(rst);
    *rst = FALSE;
    if (NULL == obj) {
        return NOERROR;
    }

    IDispListDispFormat* temp = IDispListDispFormat::Probe(obj);
    if (temp != NULL) {
        CDispListDispFormat* cTemp = (CDispListDispFormat*)temp;
        *rst = (mOutputType == cTemp->mOutputType) && (mFormat == cTemp->mFormat);
    }
    return NOERROR;
}

ECode CDispListDispFormat::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash);
    *hash = mOutputType;
    return NOERROR;
}

ECode CDispListDispFormat::ToString(
    /* [out] */ String* str)
{
    StringBuffer sb;
    sb += "type = ";
    sb += mOutputType;
    sb += " format ";
    sb += mFormat;
    return sb.ToString(str);
}

ECode CDispListDispFormat::SetOutputType(
    /* [in] */ Int32 type)
{
    mOutputType = type;
    return NOERROR;
}

ECode CDispListDispFormat::GetOutputType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);
    *type = mOutputType;
    return NOERROR;
}

ECode CDispListDispFormat::SetFormat(
    /* [in] */ Int32 item)
{
    mFormat = item;
    return NOERROR;
}

ECode CDispListDispFormat::GetFormat(
    /* [out] */ Int32* item)
{
    VALIDATE_NOT_NULL(item);
    *item = mFormat;
    return NOERROR;
}

}// namespace View
}// namespace Droid
}// namespace View
