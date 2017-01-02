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

#ifndef __ELASTOS_DROID_TEXT_STYLE_SuggestionSpan_H__
#define __ELASTOS_DROID_TEXT_STYLE_SuggestionSpan_H__

#include "elastos/droid/text/style/CharacterStyle.h"

using Elastos::Utility::ILocale;
using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

/**
 * Holds suggestion candidates for the text enclosed in this span.
 *
 * When such a span is edited in an EditText, double tapping on the text enclosed in this span will
 * display a popup dialog listing suggestion replacement for that text. The user can then replace
 * the original text by one of the suggestions.
 *
 * These spans should typically be created by the input method to provide correction and alternates
 * for the text.
 *
 * @see TextView#isSuggestionsEnabled()
 */
class SuggestionSpan
    : public CharacterStyle
    , public ISuggestionSpan
    , public IParcelableSpan
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    SuggestionSpan();

    virtual ~SuggestionSpan();

    CARAPI constructor();

    /**
     * @param context Context for the application
     * @param suggestions Suggestions for the string under the span
     * @param flags Additional flags indicating how this span is handled in TextView
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ ArrayOf<String>* suggestions,
        /* [in] */ Int32 flags);

    /**
     * @param locale Locale of the suggestions
     * @param suggestions Suggestions for the string under the span
     * @param flags Additional flags indicating how this span is handled in TextView
     */
    CARAPI constructor(
        /* [in] */ ILocale* locale,
        /* [in] */ ArrayOf<String>* suggestions,
        /* [in] */ Int32 flags);

    /**
     * @param context Context for the application
     * @param locale locale Locale of the suggestions
     * @param suggestions Suggestions for the string under the span. Only the first up to
     * {@link SuggestionSpan#SUGGESTIONS_MAX_SIZE} will be considered. Null values not permitted.
     * @param flags Additional flags indicating how this span is handled in TextView
     * @param notificationTargetClass if not null, this class will get notified when the user
     * selects one of the suggestions.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ ILocale* locale,
        /* [in] */ ArrayOf<String>* suggestions,
        /* [in] */ Int32 flags,
        /* [in] */ const String& otificationTargetClassName);

    /**
     * @return an array of suggestion texts for this span
     */
    CARAPI GetSuggestions(
        /* [out, callee] */ ArrayOf<String>** suggestions);

    /**
     * @return the locale of the suggestions
     */
    CARAPI GetLocale(
        /* [out] */ String* locale);

    /**
     * @return The name of the class to notify. The class of the original IME package will receive
     * a notification when the user selects one of the suggestions. The notification will include
     * the original string, the suggested replacement string as well as the hashCode of this span.
     * The class will get notified by an intent that has those information.
     * This is an internal API because only the framework should know the class name.
     *
     * @hide
     */
    CARAPI GetNotificationTargetClassName(
        /* [out] */ String* str);

    CARAPI GetFlags(
        /* [out] */ Int32* flags);

    CARAPI SetFlags(
        /* [in] */ Int32 flags);

    //@Override
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    //@Override
    CARAPI GetSpanTypeId(
        /* [out] */ Int32* id);

    //@Override
    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI GetHashCode(
        /* [out] */ Int32* hash);

    //@Override
    CARAPI UpdateDrawState(
        /* [in] */ ITextPaint* tp);

    /**
     * @return The color of the underline for that span, or 0 if there is no underline
     *
     * @hide
     */
    CARAPI GetUnderlineColor(
        /* [out] */ Int32* color);

    CARAPI NotifySelection(
        /* [in] */ IContext* context,
        /* [in] */ const String& original,
        /* [in] */ Int32 index);

private:
    CARAPI_(void) InitStyle(
        /* [in] */ IContext* context);

    static CARAPI_(Int32) HashCodeInternal(
        /* [in] */ ArrayOf<String>* suggestions,
        /* [in] */ const String& locale,
        /* [in] */ const String& notificationTargetClassName);

private:
    static const String TAG;// = "SuggestionSpan";

    /*
     * TODO: Needs to check the validity and add a feature that TextView will change
     * the current IME to the other IME which is specified in SuggestionSpan.
     * An IME needs to set the span by specifying the target IME and Subtype of SuggestionSpan.
     * And the current IME might want to specify any IME as the target IME including other IMEs.
     */

    Int32 mFlags;
    AutoPtr< ArrayOf<String> > mSuggestions;
    String mLocaleString;
    String mNotificationTargetClassName;
    String mNotificationTargetPackageName;
    Int32 mHashCode;

    Float mEasyCorrectUnderlineThickness;
    Int32 mEasyCorrectUnderlineColor;

    Float mMisspelledUnderlineThickness;
    Int32 mMisspelledUnderlineColor;

    Float mAutoCorrectionUnderlineThickness;
    Int32 mAutoCorrectionUnderlineColor;

};

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_STYLE_SuggestionSpan_H__
