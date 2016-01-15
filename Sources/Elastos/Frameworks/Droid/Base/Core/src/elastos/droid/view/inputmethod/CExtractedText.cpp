
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/view/inputmethod/CExtractedText.h"
#include "elastos/droid/text/TextUtils.h"

using Elastos::Droid::Text::TextUtils;

namespace Elastos {
namespace Droid {
namespace View {
namespace InputMethod {

//========================================================================================
//              CExtractedText::
//========================================================================================
const Int32 CExtractedText::FLAG_SINGLE_LINE = 0x0001;
const Int32 CExtractedText::FLAG_SELECTING = 0x0002;

CAR_INTERFACE_IMPL_2(CExtractedText, Object, IExtractedText, IParcelable)

CAR_OBJECT_IMPL(CExtractedText)

CExtractedText::CExtractedText()
    : mStartOffset(0)
    , mPartialStartOffset(0)
    , mPartialEndOffset(0)
    , mSelectionStart(0)
    , mSelectionEnd(0)
    , mFlags(0)
{}

ECode CExtractedText::constructor()
{
    return NOERROR;
}

ECode CExtractedText::SetText(
    /* [in] */ ICharSequence* text)
{
    mText = text;
    return NOERROR;
}

ECode CExtractedText::GetText(
    /* [out] */ ICharSequence** text)
{
    VALIDATE_NOT_NULL(text);
    *text = mText;
    REFCOUNT_ADD(*text);
    return NOERROR;
}

ECode CExtractedText::SetPartialStartOffset(
    /* [in] */ Int32 offset)
{
    mPartialStartOffset = offset;
    return NOERROR;
}

ECode CExtractedText::GetPartialStartOffset(
    /* [out] */ Int32* offset)
{
    VALIDATE_NOT_NULL(offset);
    *offset = mPartialStartOffset;
    return NOERROR;
}

ECode CExtractedText::SetPartialEndOffset(
    /* [in] */ Int32 offset)
{
    mPartialEndOffset = offset;
    return NOERROR;
}

ECode CExtractedText::GetPartialEndOffset(
    /* [out] */ Int32* offset)
{
    VALIDATE_NOT_NULL(offset);
    *offset = mPartialEndOffset;
    return NOERROR;
}

ECode CExtractedText::SetSelectionStart(
    /* [in] */ Int32 value)
{
    mSelectionStart = value;
    return NOERROR;
}

ECode CExtractedText::GetSelectionStart(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mSelectionStart;
    return NOERROR;
}

ECode CExtractedText::SetSelectionEnd(
    /* [in] */ Int32 value)
{
    mSelectionEnd = value;
    return NOERROR;
}

ECode CExtractedText::GetSelectionEnd(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mSelectionEnd;
    return NOERROR;
}

ECode CExtractedText::SetFlags(
    /* [in] */ Int32 value)
{
    mFlags = value;
    return NOERROR;
}

ECode CExtractedText::GetFlags(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mFlags;
    return NOERROR;
}

ECode CExtractedText::GetStartOffset(
    /* [out] */ Int32* startOffset)
{
    VALIDATE_NOT_NULL(startOffset);
    *startOffset = mStartOffset;
    return NOERROR;
}

ECode CExtractedText::SetStartOffset(
    /* [in] */ Int32 startOffset)
{
    mStartOffset = startOffset;
    return NOERROR;
}

ECode CExtractedText::ReadFromParcel(
    /* [in] */ IParcel *source)
{
    TextUtils::CHAR_SEQUENCE_CREATOR::CreateFromParcel(source, (ICharSequence**)&mText);
    source->ReadInt32(&mStartOffset);
    source->ReadInt32(&mPartialStartOffset);
    source->ReadInt32(&mPartialEndOffset);
    source->ReadInt32(&mSelectionStart);
    source->ReadInt32(&mSelectionEnd);
    source->ReadInt32(&mFlags);
    return NOERROR;
}

ECode CExtractedText::WriteToParcel(
    /* [in] */ IParcel *dest)
{
    TextUtils::WriteToParcel(mText, dest);
    dest->WriteInt32(mStartOffset);
    dest->WriteInt32(mPartialStartOffset);
    dest->WriteInt32(mPartialEndOffset);
    dest->WriteInt32(mSelectionStart);
    dest->WriteInt32(mSelectionEnd);
    dest->WriteInt32(mFlags);
    return NOERROR;
}

} // namespace InputMethod
} // namespace View
} // namespace Droid
} // namespace Elastos
