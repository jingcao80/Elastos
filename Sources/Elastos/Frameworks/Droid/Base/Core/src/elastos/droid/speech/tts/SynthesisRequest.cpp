#include "elastos/droid/speech/tts/SynthesisRequest.h"
#include "elastos/droid/os/CBundle.h"

using Elastos::Droid::Os::CBundle;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Tts {


/******************
 * SynthesisRequest
 *******************************************************************************************************/

CAR_INTERFACE_IMPL(SynthesisRequest, Object, ISynthesisRequest)

SynthesisRequest::SynthesisRequest()
{}

SynthesisRequest::~SynthesisRequest()
{}

ECode SynthesisRequest::constructor()
{
    return NOERROR;
}

ECode SynthesisRequest::constructor(
    /* [in] */ const String& text,
    /* [in] */ IBundle* params)
{
    CString::New(text, (ICharSequence**)&mText);

    // Makes a copy of params.
    return CBundle::New(params, (IBundle**)&mParams);
}

ECode SynthesisRequest::constructor(
    /* [in] */ ICharSequence* text,
    /* [in] */ IBundle* params)
{
    mText = text;

    // Makes a copy of params.
    return CBundle::New(params, (IBundle**)&mParams);
}

ECode SynthesisRequest::GetText(
    /* [out] */ String* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = ToString(mText);
    return NOERROR;
}

ECode SynthesisRequest::GetCharSequenceText(
    /* [out] */ ICharSequence** ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = mText;
    return NOERROR;
}

ECode SynthesisRequest::GetVoiceName(
    /* [out] */ String* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = mVoiceName;
    return NOERROR;
}

ECode SynthesisRequest::GetLanguage(
    /* [out] */ String* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = mLanguage;
    return NOERROR;
}

ECode SynthesisRequest::GetCountry(
    /* [out] */ String* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = mCountry;
    return NOERROR;
}

ECode SynthesisRequest::GetVariant(
    /* [out] */ String* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = mVariant;
    return NOERROR;
}

ECode SynthesisRequest::GetSpeechRate(
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = mSpeechRate;
    return NOERROR;
}

ECode SynthesisRequest::GetPitch(
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = mPitch;
    return NOERROR;
}

ECode SynthesisRequest::GetParams(
    /* [out] */ IBundle** ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = mParams;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode SynthesisRequest::GetCallerUid(
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = mCallerUid;
    return NOERROR;
}

ECode SynthesisRequest::SetLanguage(
    /* [in] */ const String& language,
    /* [in] */ const String& country,
    /* [in] */ const String& variant)
{
    mLanguage = language;
    mCountry = country;
    mVariant = variant;
    return NOERROR;
}

ECode SynthesisRequest::SetVoiceName(
    /* [in] */ const String& voiceName)
{
    mVoiceName = voiceName;
    return NOERROR;
}

ECode SynthesisRequest::SetSpeechRate(
    /* [in] */ Int32 speechRate)
{
    mSpeechRate = speechRate;
    return NOERROR;
}

ECode SynthesisRequest::SetPitch(
    /* [in] */ Int32 pitch)
{
    mPitch = pitch;
    return NOERROR;
}

ECode SynthesisRequest::SetCallerUid(
    /* [in] */ Int32 uid)
{
    mCallerUid = uid;
    return NOERROR;
}

} // namespace Tts
} // namespace Speech
} // namepsace Droid
} // namespace Elastos
