
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/inputmethod/CCorrectionInfo.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/ext/frameworkext.h"

#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Text::TextUtils;

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace View {
namespace InputMethod {

//========================================================================================
//              CCorrectionInfo::
//========================================================================================
CAR_INTERFACE_IMPL_2(CCorrectionInfo, Object, ICorrectionInfo, IParcelable)

CAR_OBJECT_IMPL(CCorrectionInfo)

CCorrectionInfo::CCorrectionInfo()
    : mOffset(0)
{
}

ECode CCorrectionInfo::constructor()
{
    return NOERROR;
}

ECode CCorrectionInfo::constructor(
    /* [in] */ Int32 offset,
    /* [in] */ ICharSequence* oldText,
    /* [in] */ ICharSequence* newText)
{
    mOffset = offset;
    mOldText = oldText;
    mNewText = newText;
    return NOERROR;
}

ECode CCorrectionInfo::GetOffset(
    /* [out] */ Int32* offset)
{
    VALIDATE_NOT_NULL(offset)

    *offset = mOffset;
    return NOERROR;
}

ECode CCorrectionInfo::GetOldText(
    /* [out] */ ICharSequence** text)
{
    VALIDATE_NOT_NULL(text)

    *text = mOldText;
    REFCOUNT_ADD(*text);

    return NOERROR;
}

ECode CCorrectionInfo::GetNewText(
    /* [out] */ ICharSequence** text)
{
    VALIDATE_NOT_NULL(text)

    *text = mNewText;
    REFCOUNT_ADD(*text);

    return NOERROR;
}

ECode CCorrectionInfo::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    StringBuilder builder("CorrectionInfo{#");
    builder += mOffset;
    builder += " \"";

    String strOldTxt;
    mOldText->ToString(&strOldTxt);
    builder += strOldTxt;

    builder += "\" -> \"";

    String strNewTxt;
    mOldText->ToString(&strNewTxt);
    builder += strNewTxt;

    builder += "\"}";
    *result = builder.ToString();
    return NOERROR;
}

ECode CCorrectionInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest)

    dest->WriteInt32(mOffset);
    TextUtils::WriteToParcel(mOldText, dest);
    TextUtils::WriteToParcel(mNewText, dest);
    return NOERROR;
}

ECode CCorrectionInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mOffset);
    TextUtils::CHAR_SEQUENCE_CREATOR::CreateFromParcel(source, (ICharSequence**)&mOldText);
    TextUtils::CHAR_SEQUENCE_CREATOR::CreateFromParcel(source, (ICharSequence**)&mNewText);
    return NOERROR;
}

} // namespace InputMethod
} // namespace View
} // namespace Droid
} // namespace Elastos
