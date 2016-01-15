
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/view/inputmethod/CExtractedTextRequest.h"

namespace Elastos {
namespace Droid {
namespace View {
namespace InputMethod {

//========================================================================================
//              CExtractedTextRequest::
//========================================================================================
CAR_INTERFACE_IMPL_2(CExtractedTextRequest, Object, IExtractedTextRequest, IParcelable)

CAR_OBJECT_IMPL(CExtractedTextRequest)

CExtractedTextRequest::CExtractedTextRequest()
    : mToken(0)
    , mFlags(0)
    , mHintMaxLines(0)
    , mHintMaxChars(0)
{
}

ECode CExtractedTextRequest::constructor()
{
    return NOERROR;
}

ECode CExtractedTextRequest::GetToken(
    /* [out] */ Int32* token)
{
    VALIDATE_NOT_NULL(token);
    *token = mToken;
    return NOERROR;
}

ECode CExtractedTextRequest::SetToken(
    /* [in] */ Int32 token)
{
    mToken = token;
    return NOERROR;
}

ECode CExtractedTextRequest::GetFlags(
    /* [out] */ Int32* flags)
{
    VALIDATE_NOT_NULL(flags);
    *flags = mFlags;
    return NOERROR;
}

ECode CExtractedTextRequest::SetFlags(
    /* [in] */ Int32 flags)
{
    mFlags = flags;
    return NOERROR;
}

ECode CExtractedTextRequest::GetHintMaxLines(
    /* [out] */ Int32* hintMaxLines)
{
    VALIDATE_NOT_NULL(hintMaxLines);
    *hintMaxLines = mHintMaxLines;
    return NOERROR;
}

ECode CExtractedTextRequest::SetHintMaxLines(
    /* [in] */ Int32 hintMaxLines)
{
    mHintMaxLines = hintMaxLines;
    return NOERROR;
}

ECode CExtractedTextRequest::GetHintMaxChars(
    /* [out] */ Int32* hintMaxChars)
{
    VALIDATE_NOT_NULL(hintMaxChars);
    *hintMaxChars = mHintMaxChars;
    return NOERROR;
}

ECode CExtractedTextRequest::SetHintMaxChars(
    /* [in] */ Int32 hintMaxChars)
{
    mHintMaxChars = hintMaxChars;
    return NOERROR;
}

ECode CExtractedTextRequest::ReadFromParcel(
    /* [in] */ IParcel *source)
{
    source->ReadInt32(&mToken);
    source->ReadInt32(&mFlags);
    source->ReadInt32(&mHintMaxLines);
    source->ReadInt32(&mHintMaxChars);

    return NOERROR;
}

ECode CExtractedTextRequest::WriteToParcel(
    /* [in] */ IParcel *dest)
{
    dest->WriteInt32(mToken);
    dest->WriteInt32(mFlags);
    dest->WriteInt32(mHintMaxLines);
    dest->WriteInt32(mHintMaxChars);

    return NOERROR;
}

} // namespace InputMethod
} // namespace View
} // namespace Droid
} // namespace Elastos
