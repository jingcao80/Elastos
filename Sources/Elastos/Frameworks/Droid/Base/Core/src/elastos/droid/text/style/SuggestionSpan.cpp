//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "Elastos.Droid.View.h"
#include "elastos/droid/text/style/SuggestionSpan.h"
#include "elastos/droid/text/CTextPaint.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/res/CConfiguration.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/R.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/Arrays.h>

using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Text::CTextPaint;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Res::CConfiguration;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::View::InputMethod::IInputMethodManager;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Arrays;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

const String SuggestionSpan::TAG("SuggestionSpan");

CAR_INTERFACE_IMPL_3(SuggestionSpan, CharacterStyle, ISuggestionSpan, IParcelableSpan, IParcelable)

SuggestionSpan::SuggestionSpan()
{}

SuggestionSpan::~SuggestionSpan()
{}

ECode SuggestionSpan::constructor()
{
    return NOERROR;
}

ECode SuggestionSpan::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ArrayOf<String>* suggestions,
    /* [in] */ Int32 flags)
{
    String nullStr;
    return constructor(context, NULL, suggestions, flags, nullStr);
}

ECode SuggestionSpan::constructor(
    /* [in] */ ILocale* locale,
    /* [in] */ ArrayOf<String>* suggestions,
    /* [in] */ Int32 flags)
{
    String nullStr;
    return constructor(NULL, locale, suggestions, flags, nullStr);
}

ECode SuggestionSpan::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ILocale* locale,
    /* [in] */ ArrayOf<String>* suggestions,
    /* [in] */ Int32 flags,
    /* [in] */ const String& notificationTargetClassName)
{
    Int32 N = Elastos::Core::Math::Min(ISuggestionSpan::SUGGESTIONS_MAX_SIZE, suggestions->GetLength());
    mSuggestions = ArrayOf<String>::Alloc(N);
    for(Int32 i=0; i<N; i++){
        (*mSuggestions)[i] = (*suggestions)[i];
    }
    mFlags = flags;
    if (locale != NULL) {
        locale->ToString(&mLocaleString);
        locale->ToLanguageTag(&mLanguageTag);
    }
    else if (context != NULL) {
        AutoPtr<IResources> rs;
        context->GetResources((IResources**)&rs);
        AutoPtr<IConfiguration> cfg;
        rs->GetConfiguration((IConfiguration**)&cfg);
        AutoPtr<ILocale> localeCfg;
        cfg->GetLocale((ILocale**)&localeCfg);
        localeCfg->ToString(&mLocaleString);
        localeCfg->ToLanguageTag(&mLanguageTag);
    }
    else {
        Logger::E(String("SuggestionSpan"), String("No locale or context specified in SuggestionSpan constructor\n") );
        mLocaleString = "";
        mLanguageTag = "";
    }

    if (context != NULL) {
        context->GetPackageName(&mNotificationTargetPackageName);
    }

    if (!notificationTargetClassName.IsNull()) {
        mNotificationTargetClassName = notificationTargetClassName;
    }
    else {
        mNotificationTargetClassName = String("");
    }
    mHashCode = HashCodeInternal(mSuggestions, mLocaleString, mNotificationTargetClassName);

    InitStyle(context);

    return NOERROR;
}

void SuggestionSpan::InitStyle(
    /* [in] */ IContext* context)
{
    if (context == NULL) {
        mMisspelledUnderlineThickness = 0;
        mEasyCorrectUnderlineThickness = 0;
        mAutoCorrectionUnderlineThickness = 0;
        mMisspelledUnderlineColor = IColor::BLACK;
        mEasyCorrectUnderlineColor = IColor::BLACK;
        mAutoCorrectionUnderlineColor = IColor::BLACK;
        return;
    }

    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::SuggestionSpan);

    Int32 defStyle = R::attr::textAppearanceMisspelledSuggestion;
    AutoPtr<ITypedArray> typedArray;
    context->ObtainStyledAttributes( NULL, attrIds, defStyle, 0, (ITypedArray**)&typedArray);
    typedArray->GetDimension( R::styleable::SuggestionSpan_textUnderlineThickness, 0, &mMisspelledUnderlineThickness);
    typedArray->GetColor( R::styleable::SuggestionSpan_textUnderlineColor, IColor::BLACK, &mMisspelledUnderlineColor);

    defStyle = R::attr::textAppearanceEasyCorrectSuggestion;
    typedArray = NULL;
    context->ObtainStyledAttributes( NULL, attrIds, defStyle, 0, (ITypedArray**)&typedArray);
    typedArray->GetDimension( R::styleable::SuggestionSpan_textUnderlineThickness, 0, &mEasyCorrectUnderlineThickness);
    typedArray->GetColor( R::styleable::SuggestionSpan_textUnderlineColor, IColor::BLACK, &mEasyCorrectUnderlineColor);

    defStyle = R::attr::textAppearanceAutoCorrectionSuggestion;
    typedArray = NULL;
    context->ObtainStyledAttributes( NULL, attrIds, defStyle, 0, (ITypedArray**)&typedArray);
    typedArray->GetDimension( R::styleable::SuggestionSpan_textUnderlineThickness, 0, &mAutoCorrectionUnderlineThickness);
    typedArray->GetColor( R::styleable::SuggestionSpan_textUnderlineColor, IColor::BLACK, &mAutoCorrectionUnderlineColor);
}

ECode SuggestionSpan::GetSuggestions(
    /* [out, callee] */ ArrayOf<String>** array)
{
    VALIDATE_NOT_NULL(array)
    *array = mSuggestions;
    REFCOUNT_ADD(*array)
    return NOERROR;
}

ECode SuggestionSpan::GetLocale(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = mLocaleString;
    return NOERROR;
}

ECode SuggestionSpan::GetNotificationTargetClassName(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = mNotificationTargetClassName;
    return NOERROR;
}

ECode SuggestionSpan::GetFlags(
    /* [out] */ Int32* flags)
{
    VALIDATE_NOT_NULL(flags)
    *flags = mFlags;
    return NOERROR;
}

ECode SuggestionSpan::SetFlags(
    /* [in] */ Int32 flags)
{
    mFlags = flags;
    return NOERROR;
}

ECode SuggestionSpan::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    mSuggestions = NULL;
    FAIL_RETURN(source->ReadArrayOfString((ArrayOf<String>**)&mSuggestions));
    FAIL_RETURN(source->ReadInt32(&mFlags));
    FAIL_RETURN(source->ReadString(&mLocaleString));
    FAIL_RETURN(source->ReadString(&mLanguageTag));
    FAIL_RETURN(source->ReadString(&mNotificationTargetClassName));
    FAIL_RETURN(source->ReadString(&mNotificationTargetPackageName));
    FAIL_RETURN(source->ReadInt32(&mHashCode));
    FAIL_RETURN(source->ReadInt32(&mEasyCorrectUnderlineColor));
    FAIL_RETURN(source->ReadFloat(&mEasyCorrectUnderlineThickness));
    FAIL_RETURN(source->ReadInt32(&mMisspelledUnderlineColor));
    FAIL_RETURN(source->ReadFloat(&mMisspelledUnderlineThickness));
    FAIL_RETURN(source->ReadInt32(&mAutoCorrectionUnderlineColor));
    FAIL_RETURN(source->ReadFloat(&mAutoCorrectionUnderlineThickness));
    return NOERROR;
}

ECode SuggestionSpan::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    FAIL_RETURN(dest->WriteArrayOfString(mSuggestions));
    FAIL_RETURN(dest->WriteInt32(mFlags));
    FAIL_RETURN(dest->WriteString(mLocaleString));
    FAIL_RETURN(dest->WriteString(mLanguageTag));
    FAIL_RETURN(dest->WriteString(mNotificationTargetClassName));
    FAIL_RETURN(dest->WriteString(mNotificationTargetPackageName));
    FAIL_RETURN(dest->WriteInt32(mHashCode));
    FAIL_RETURN(dest->WriteInt32(mEasyCorrectUnderlineColor));
    FAIL_RETURN(dest->WriteFloat(mEasyCorrectUnderlineThickness));
    FAIL_RETURN(dest->WriteInt32(mMisspelledUnderlineColor));
    FAIL_RETURN(dest->WriteFloat(mMisspelledUnderlineThickness));
    FAIL_RETURN(dest->WriteInt32(mAutoCorrectionUnderlineColor));
    FAIL_RETURN(dest->WriteFloat(mAutoCorrectionUnderlineThickness));
    return NOERROR;
}

ECode SuggestionSpan::GetSpanTypeId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = ITextUtils::SUGGESTION_SPAN;
    return NOERROR;
}

ECode SuggestionSpan::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    AutoPtr<ISuggestionSpan> ss = ISuggestionSpan::Probe(o);
    if (ss != NULL) {
        Int32 hashCode;
        ss->GetHashCode(&hashCode);
        *result = hashCode == mHashCode;
    }
    return NOERROR;
}

ECode SuggestionSpan::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash)
    *hash  = mHashCode;
    return NOERROR;
}

Int32 SuggestionSpan::HashCodeInternal(
    /* [in] */ ArrayOf<String>* suggestions,
    /* [in] */ const String& locale,
    /* [in] */ const String& notificationTargetClassName)
{
    Int32 size = suggestions->GetLength();
    AutoPtr<ArrayOf<Int64> > arrays = ArrayOf<Int64>::Alloc(3 + size);
    Int32 i = 0;
    arrays->Set(i++, SystemClock::GetUptimeMillis());
    for (Int32 j = 0; j < suggestions->GetLength(); ++j) {
        arrays->Set(i++, (*suggestions)[j].GetHashCode());
    }

    arrays->Set(i++, locale.GetHashCode());
    arrays->Set(i++, notificationTargetClassName.GetHashCode());

    return Arrays::GetHashCode(arrays);
}

ECode SuggestionSpan::UpdateDrawState(
    /* [in] */ ITextPaint* tp)
{
    VALIDATE_NOT_NULL(tp);
    CTextPaint* p = (CTextPaint*)tp;
    /*const*/ Boolean misspelled = (mFlags & ISuggestionSpan::FLAG_MISSPELLED) != 0;
    /*const*/ Boolean easy = (mFlags & ISuggestionSpan::FLAG_EASY_CORRECT) != 0;
    /*const*/ Boolean autoCorrection = (mFlags & ISuggestionSpan::FLAG_AUTO_CORRECTION) != 0;
    if (easy) {
        Int32 underlineColor;
        if (!misspelled) {
            p->SetUnderlineText(mEasyCorrectUnderlineColor, mEasyCorrectUnderlineThickness);
        }
        else if ((tp->GetUnderlineColor(&underlineColor), underlineColor) == 0) {
            // Spans are rendered in an arbitrary order. Since misspelled is less prioritary
            // than just easy, do not apply misspelled if an easy (or a mispelled) has been set
            p->SetUnderlineText(mMisspelledUnderlineColor, mMisspelledUnderlineThickness);
        }
    }
    else if (autoCorrection) {
        p->SetUnderlineText(mAutoCorrectionUnderlineColor, mAutoCorrectionUnderlineThickness);
    }
    return NOERROR;
}

ECode SuggestionSpan::GetUnderlineColor(
    /* [out] */ Int32* color)
{
    VALIDATE_NOT_NULL(color)
    *color = 0;
    /*const*/ Boolean misspelled = (mFlags & ISuggestionSpan::FLAG_MISSPELLED) != 0;
    /*const*/ Boolean easy = (mFlags & ISuggestionSpan::FLAG_EASY_CORRECT) != 0;
    /*const*/ Boolean autoCorrection = (mFlags & ISuggestionSpan::FLAG_AUTO_CORRECTION) != 0;
    if (easy) {
        if (!misspelled) {
            *color = mEasyCorrectUnderlineColor;
        return NOERROR;
        }
        else {
            *color =  mMisspelledUnderlineColor;
        return NOERROR;
        }
    }
    else if (autoCorrection) {
        *color =  mAutoCorrectionUnderlineColor;
        return NOERROR;
    }
    return NOERROR;
}

ECode SuggestionSpan::NotifySelection(
    /* [in] */ IContext* context,
    /* [in] */ const String& original,
    /* [in] */ Int32 index)
{
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);

    if (context == NULL || mNotificationTargetClassName.IsNull()) {
        return NOERROR;
    }
    // Ensures that only a class in the original IME package will receive the
    // notification.
    if (mSuggestions == NULL || index < 0 || index >= mSuggestions->GetLength()) {
        // Log.w(TAG, "Unable to notify the suggestion as the index is out of range index=" + index
        //         + " length=" + mSuggestions.length);
        return NOERROR;
    }

    // The package name is not mandatory (legacy from JB), and if the package name
    // is missing, we try to notify the suggestion through the input method manager.
    if (!mNotificationTargetPackageName.IsNull()) {
        Int32 hash;
        GetHashCode(&hash);
        intent->SetClassName(mNotificationTargetPackageName, mNotificationTargetClassName);
        intent->SetAction(ISuggestionSpan::ACTION_SUGGESTION_PICKED);
        intent->PutExtra(ISuggestionSpan::SUGGESTION_SPAN_PICKED_BEFORE, original);
        intent->PutExtra(ISuggestionSpan::SUGGESTION_SPAN_PICKED_AFTER, (*mSuggestions)[index]);
        intent->PutExtra(ISuggestionSpan::SUGGESTION_SPAN_PICKED_HASHCODE, hash);
        context->SendBroadcast(intent);
    }
    else {
        AutoPtr<IInputMethodManager> imm;
        //imm = InputMethodManager::PeekInstance();
        if (imm != NULL) {
            imm->NotifySuggestionPicked(this, original, index);
        }
    }

    return NOERROR;
}

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos
