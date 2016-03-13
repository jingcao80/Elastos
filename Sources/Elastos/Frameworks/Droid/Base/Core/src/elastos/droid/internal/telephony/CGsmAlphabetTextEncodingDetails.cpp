#include "elastos/droid/internal/telephony/CGsmAlphabetTextEncodingDetails.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CAR_INTERFACE_IMPL(CGsmAlphabetTextEncodingDetails, Object, IGsmAlphabetTextEncodingDetails)

CAR_OBJECT_IMPL(CGsmAlphabetTextEncodingDetails)

CGsmAlphabetTextEncodingDetails::CGsmAlphabetTextEncodingDetails()
    : mMsgCount(0)
    , mCodeUnitCount(0)
    , mCodeUnitsRemaining(0)
    , mCodeUnitSize(0)
    , mLanguageTable(0)
    , mLanguageShiftTable(0)
{
}

CGsmAlphabetTextEncodingDetails::~CGsmAlphabetTextEncodingDetails()
{
}

ECode CGsmAlphabetTextEncodingDetails::constructor()
{
    return NOERROR;
}

ECode CGsmAlphabetTextEncodingDetails::SetMsgCount(
    /* [in] */ Int32 msgCount)
{
    mMsgCount = msgCount;
    return NOERROR;
}

ECode CGsmAlphabetTextEncodingDetails::GetMsgCount(
    /* [out] */ Int32* msgCount)
{
    VALIDATE_NOT_NULL(msgCount);
    *msgCount = mMsgCount;
    return NOERROR;
}

ECode CGsmAlphabetTextEncodingDetails::SetCodeUnitCount(
    /* [in] */ Int32 codeUnitCount)
{
    mCodeUnitCount = codeUnitCount;
    return NOERROR;
}

ECode CGsmAlphabetTextEncodingDetails::GetCodeUnitCount(
    /* [out] */ Int32* codeUnitCount)
{
    VALIDATE_NOT_NULL(codeUnitCount);
    *codeUnitCount = mCodeUnitCount;
    return NOERROR;
}

ECode CGsmAlphabetTextEncodingDetails::SetCodeUnitsRemaining(
    /* [in] */ Int32 codeUnitsRemaining)
{
    mCodeUnitsRemaining = codeUnitsRemaining;
    return NOERROR;
}

ECode CGsmAlphabetTextEncodingDetails::GetCodeUnitsRemaining(
    /* [out] */ Int32* codeUnitsRemaining)
{
    VALIDATE_NOT_NULL(codeUnitsRemaining);
    *codeUnitsRemaining = mCodeUnitsRemaining;
    return NOERROR;
}

ECode CGsmAlphabetTextEncodingDetails::SetCodeUnitSize(
    /* [in] */ Int32 codeUnitSize)
{
    mCodeUnitSize = codeUnitSize;
    return NOERROR;
}

ECode CGsmAlphabetTextEncodingDetails::GetCodeUnitSize(
    /* [out] */ Int32* codeUnitSize)
{
    VALIDATE_NOT_NULL(codeUnitSize);
    *codeUnitSize = mCodeUnitSize;
    return NOERROR;
}


ECode CGsmAlphabetTextEncodingDetails::SetLanguageTable(
    /* [in] */ Int32 languageTable)
{
    mLanguageTable = languageTable;
    return NOERROR;
}

ECode CGsmAlphabetTextEncodingDetails::GetLanguageTable(
    /* [out] */ Int32* languageTable)
{
    VALIDATE_NOT_NULL(languageTable);
    *languageTable = mLanguageTable;
    return NOERROR;
}

ECode CGsmAlphabetTextEncodingDetails::SetLanguageShiftTable(
    /* [in] */ Int32 languageShiftTable)
{
    mLanguageShiftTable = languageShiftTable;
    return NOERROR;
}

ECode CGsmAlphabetTextEncodingDetails::GetLanguageShiftTable(
    /* [out] */ Int32* languageShiftTable)
{
    VALIDATE_NOT_NULL(languageShiftTable);
    *languageShiftTable = mLanguageShiftTable;
    return NOERROR;
}

ECode CGsmAlphabetTextEncodingDetails::ToString(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);
    StringBuilder sb;

    sb.Append("TextEncodingDetails");
    sb.Append("{ msgCount=");
    sb.Append(mMsgCount);
    sb.Append(", codeUnitCount=");
    sb.Append(mCodeUnitCount);
    sb.Append(", codeUnitsRemaining=");
    sb.Append(mCodeUnitsRemaining);
    sb.Append(", codeUnitSize=");
    sb.Append(mCodeUnitSize);
    sb.Append(", languageTable=");
    sb.Append(mLanguageTable);
    sb.Append(", languageShiftTable=");
    sb.Append(mLanguageShiftTable);
    sb.Append(" }");
    *res = sb.ToString();
    return NOERROR;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos