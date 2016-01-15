
#include "elastos/droid/widget/TextViewSavedState.h"
#include "elastos/droid/text/TextUtils.h"

using Elastos::Droid::Text::TextUtils;

namespace Elastos {
namespace Droid {
namespace Widget {

TextViewSavedState::TextViewSavedState()
    : mSelStart(-1)
    , mSelEnd(-1)
    , mFrozenWithFocus(FALSE)
{
}

TextViewSavedState::TextViewSavedState(
    /* [in] */ IParcelable* superState)
    : ViewBaseSavedState(superState)
    , mSelStart(-1)
    , mSelEnd(-1)
    , mFrozenWithFocus(FALSE)
{
}

ECode TextViewSavedState::WriteToParcel(
    /* [in] */ IParcel* out)
{
    ViewBaseSavedState::WriteToParcel(out);

    out->WriteInt32(mSelStart);
    out->WriteInt32(mSelEnd);
    out->WriteInt32(mFrozenWithFocus ? 1 : 0);
    TextUtils::WriteToParcel(mText, out);

     if (mError == NULL) {
        out->WriteInt32(0);
     } else {
        out->WriteInt32(1);
        TextUtils::WriteToParcel(mError, out);
     }

    return NOERROR;
}

ECode TextViewSavedState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    VALIDATE_NOT_NULL(source);

    ViewBaseSavedState::ReadFromParcel(source);

    source->ReadInt32(&mSelStart);
    source->ReadInt32(&mSelEnd);
    Int32 data;
    source->ReadInt32(&data);
    mFrozenWithFocus = data != 0;
    mText = NULL;
    FAIL_RETURN(TextUtils::CHAR_SEQUENCE_CREATOR::CreateFromParcel(source, (ICharSequence**)&mText));

    source->ReadInt32(&data);
    if (data != 0) {
        mError = NULL;
        FAIL_RETURN(TextUtils::CHAR_SEQUENCE_CREATOR::CreateFromParcel(source, (ICharSequence**)&mError));
    }

    return NOERROR;
}

ECode TextViewSavedState::Init()
{
    return ViewBaseSavedState::Init();
}

ECode TextViewSavedState::Init(
    /* [in] */ IParcelable* superState)
{
    return ViewBaseSavedState::Init(superState);
}



} // namespace Widget
} // namespace Droid
} // namespace Elastos
