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

#ifndef __ELASTOS_DROID_APP_CSEARCHABLEINFO_H__
#define __ELASTOS_DROID_APP_CSEARCHABLEINFO_H__

#include "Elastos.Droid.Content.h"
#include "_Elastos_Droid_App_CSearchableInfo.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Utility::Etl::HashMap;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Utility::IAttributeSet;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CSearchableInfo)
    , public Object
    , public ISearchableInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CSearchableInfo();

    CARAPI constructor();

    /**
     * Constructor
     *
     * Given a ComponentName, get the searchability info
     * and build a local copy of it.  Use the factory, not this.
     *
     * @param activityContext runtime context for the activity that the searchable info is about.
     * @param attr The attribute set we found in the XML file, contains the values that are used to
     * construct the object.
     * @param cName The component name of the searchable activity
     * @throws IllegalArgumentException if the searchability info is invalid or insufficient
     */
    CARAPI constructor(
        /* [in] */ IContext* activityContext,
        /* [in] */ IAttributeSet* attr,
        /* [in] */ IComponentName* cName);
    /**
     * Gets the search suggestion content provider authority.
     *
     * @return The search suggestions authority, or {@code null} if not set.
     * @see android.R.styleable#Searchable_searchSuggestAuthority
     */
    CARAPI GetSuggestAuthority(
        /* [out] */ String* auth);

    /**
     * Gets the name of the package where the suggestion provider lives,
     * or {@code null}.
     */
    CARAPI GetSuggestPackage(
        /* [out] */ String* pkg);

    /**
     * Gets the component name of the searchable activity.
     *
     * @return A component name, never {@code null}.
     */
    CARAPI GetSearchActivity(
        /* [out] */ IComponentName** nameRst);

    /**
     * Checks whether the badge should be a text label.
     *
     * @see android.R.styleable#Searchable_searchMode
     *
     * @hide This feature is deprecated, no need to add it to the API.
     */
    CARAPI UseBadgeLabel(
        /* [out] */ Boolean* isUsing);

    /**
     * Checks whether the badge should be an icon.
     *
     * @see android.R.styleable#Searchable_searchMode
     *
     * @hide This feature is deprecated, no need to add it to the API.
     */
    CARAPI UseBadgeIcon(
        /* [out] */ Boolean* isUsing);

    /**
     * Checks whether the text in the query field should come from the suggestion intent data.
     *
     * @see android.R.styleable#Searchable_searchMode
     */
    CARAPI ShouldRewriteQueryFromData(
        /* [out] */ Boolean* isShould);

    /**
     * Checks whether the text in the query field should come from the suggestion title.
     *
     * @see android.R.styleable#Searchable_searchMode
     */
    CARAPI ShouldRewriteQueryFromText(
        /* [out] */ Boolean* isShould);

    /**
     * Gets the resource id of the description string to use for this source in system search
     * settings, or {@code 0} if none has been specified.
     *
     * @see android.R.styleable#Searchable_searchSettingsDescription
     */
    CARAPI GetSettingsDescriptionId(
        /* [out] */ Boolean* id);

    /**
     * Gets the content provider path for obtaining search suggestions.
     *
     * @return The suggestion path, or {@code null} if not set.
     * @see android.R.styleable#Searchable_searchSuggestPath
     */
    CARAPI GetSuggestPath(
        /* [out] */ String* path);

    /**
     * Gets the selection for obtaining search suggestions.
     *
     * @see android.R.styleable#Searchable_searchSuggestSelection
     */
    CARAPI GetSuggestSelection(
        /* [out] */ String* selection);

    /**
     * Gets the optional intent action for use with these suggestions. This is
     * useful if all intents will have the same action
     * (e.g. {@link android.content.Intent#ACTION_VIEW})
     *
     * This can be overriden in any given suggestion using the column
     * {@link SearchManager#SUGGEST_COLUMN_INTENT_ACTION}.
     *
     * @return The default intent action, or {@code null} if not set.
     * @see android.R.styleable#Searchable_searchSuggestIntentAction
     */
    CARAPI GetSuggestIntentAction(
        /* [out] */ String* action);

    /**
     * Gets the optional intent data for use with these suggestions.  This is
     * useful if all intents will have similar data URIs,
     * but you'll likely need to provide a specific ID as well via the column
     * {@link SearchManager#SUGGEST_COLUMN_INTENT_DATA_ID}, which will be appended to the
     * intent data URI.
     *
     * This can be overriden in any given suggestion using the column
     * {@link SearchManager#SUGGEST_COLUMN_INTENT_DATA}.
     *
     * @return The default intent data, or {@code null} if not set.
     * @see android.R.styleable#Searchable_searchSuggestIntentData
     */
    CARAPI GetSuggestIntentData(
        /* [out] */ String* data);

    /**
     * Gets the suggestion threshold.
     *
     * @return The suggestion threshold, or {@code 0} if not set.
     * @see android.R.styleable#Searchable_searchSuggestThreshold
     */
    CARAPI GetSuggestThreshold(
        /* [out] */ Int32* threshold);

    /**
     * Get the context for the searchable activity.
     *
     * @param context You need to supply a context to start with
     * @return Returns a context related to the searchable activity
     * @hide
     */
    CARAPI GetActivityContext(
        /* [in] */ IContext* inCtx,
        /* [out] */ IContext** outCtx);

    /**
     * Get the context for the suggestions provider.
     *
     * @param context You need to supply a context to start with
     * @param activityContext If we can determine that the provider and the activity are the
     *        same, we'll just return this one.
     * @return Returns a context related to the suggestion provider
     * @hide
     */
    CARAPI GetProviderContext(
        /* [in] */ IContext* ctx,
        /* [in] */ IContext* activityCtx,
        /* [out] */ IContext** rstCtx);

    /**
     * If any action keys were defined for this searchable activity, look up and return.
     *
     * @param keyCode The key that was pressed
     * @return Returns the action key info, or {@code null} if none defined.
     *
     * @hide ActionKeyInfo is hidden
     */
    CARAPI FindActionKey(
        /* [in] */ Int32 keyCode,
        /* [out] */ IActionKeyInfo** info);

    /**
     * Gets the "label" (user-visible name) of this searchable context. This must be
     * read using the searchable Activity's resources.
     *
     * @return A resource id, or {@code 0} if no label was specified.
     * @see android.R.styleable#Searchable_label
     *
     * @hide deprecated functionality
     */
    CARAPI GetLabelId(
        /* [out] */ Int32* id);

    /**
     * Gets the resource id of the hint text. This must be
     * read using the searchable Activity's resources.
     *
     * @return A resource id, or {@code 0} if no hint was specified.
     * @see android.R.styleable#Searchable_hint
     */
    CARAPI GetHintId(
        /* [out] */ Int32* id);

    /**
     * Gets the icon id specified by the Searchable_icon meta-data entry. This must be
     * read using the searchable Activity's resources.
     *
     * @return A resource id, or {@code 0} if no icon was specified.
     * @see android.R.styleable#Searchable_icon
     *
     * @hide deprecated functionality
     */
    CARAPI GetIconId(
        /* [out] */ Int32* id);

    /**
     * Checks if the searchable activity wants the voice search button to be shown.
     *
     * @see android.R.styleable#Searchable_voiceSearchMode
     */
    CARAPI GetVoiceSearchEnabled(
        /* [out] */ Boolean* enabled);

    /**
     * Checks if voice search should start web search.
     *
     * @see android.R.styleable#Searchable_voiceSearchMode
     */
    CARAPI GetVoiceSearchLaunchWebSearch(
        /* [out] */ Boolean* mode);

    /**
     * Checks if voice search should start in-app search.
     *
     * @see android.R.styleable#Searchable_voiceSearchMode
     */
    CARAPI GetVoiceSearchLaunchRecognizer(
        /* [out] */ Boolean* mode);

    /**
     * Gets the resource id of the voice search language model string.
     *
     * @return A resource id, or {@code 0} if no language model was specified.
     * @see android.R.styleable#Searchable_voiceLanguageModel
     */
    CARAPI GetVoiceLanguageModeId(
        /* [out] */ Int32* modeId);

    /**
     * Gets the resource id of the voice prompt text string.
     *
     * @return A resource id, or {@code 0} if no voice prompt text was specified.
     * @see android.R.styleable#Searchable_voicePromptText
     */
    CARAPI GetVoicePromptTextId(
        /* [out] */ Int32* textId);

    /**
     * Gets the resource id of the spoken language to recognize in voice search.
     *
     * @return A resource id, or {@code 0} if no language was specified.
     * @see android.R.styleable#Searchable_voiceLanguage
     */
    CARAPI GetVoiceLanguageId(
        /* [out] */ Int32* id);

    /**
     * The maximum number of voice recognition results to return.
     *
     * @return the max results count, if specified in the searchable
     *         activity's metadata, or {@code 0} if not specified.
     * @see android.R.styleable#Searchable_voiceMaxResults
     */
    CARAPI GetVoiceMaxResults(
        /* [out] */ Int32* results);

    /**
     * Gets the resource id of replacement text for the "Search" button.
     *
     * @return A resource id, or {@code 0} if no replacement text was specified.
     * @see android.R.styleable#Searchable_searchButtonText
     * @hide This feature is deprecated, no need to add it to the API.
     */
    CARAPI GetSearchButtonText(
        /* [out] */ Int32* text);

    /**
     * Gets the input type as specified in the searchable attributes. This will default to
     * {@link InputType#TYPE_CLASS_TEXT} if not specified (which is appropriate
     * for free text input).
     *
     * @return the input type
     * @see android.R.styleable#Searchable_inputType
     */
    CARAPI GetInputType(
        /* [out] */ Int32* type);

    /**
     * Gets the input method options specified in the searchable attributes.
     * This will default to {@link EditorInfo#IME_ACTION_GO} if not specified (which is
     * appropriate for a search box).
     *
     * @return the input type
     * @see android.R.styleable#Searchable_imeOptions
     */
    CARAPI GetImeOptions(
        /* [out] */ Int32* options);

    /**
     * Checks whether the searchable should be included in global search.
     *
     * @return The value of the {@link android.R.styleable#Searchable_includeInGlobalSearch}
     *         attribute, or {@code false} if the attribute is not set.
     * @see android.R.styleable#Searchable_includeInGlobalSearch
     */
    CARAPI ShouldIncludeInGlobalSearch(
        /* [out] */ Boolean* isShould);

    /**
     * Checks whether this searchable activity should be queried for suggestions if a prefix
     * of the query has returned no results.
     *
     * @see android.R.styleable#Searchable_queryAfterZeroResults
     */
    CARAPI QueryAfterZeroResults(
        /* [out] */ Boolean* results);

    /**
     * Checks whether this searchable activity has auto URL detection turned on.
     *
     * @see android.R.styleable#Searchable_autoUrlDetect
     */
    CARAPI AutoUrlDetect(
        /* [out] */ Boolean* autoDetect);

    /**
     * Instantiates a new SearchableInfo from the data in a Parcel that was
     * previously written with {@link #writeToParcel(Parcel, int)}.
     *
     * @param in The Parcel containing the previously written SearchableInfo,
     * positioned at the location in the buffer where it was written.
     */
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    static CARAPI GetActivityMetaData(
        /* [in] */ IContext* ctx,
        /* [in] */ IActivityInfo* activityInfo,
        /* [in] */ Int32 userId,
        /* [out] */ ISearchableInfo** info);

private:

    static CARAPI CreateActivityContext(
        /* [in] */ IContext* ctx,
        /* [in] */ IComponentName* activity,
        /* [out] */ IContext** outCtx);

    static CARAPI GetActivityMetaData(
        /* [in] */ IContext* ctx,
        /* [in] */ IXmlPullParser* xml,
        /* [in] */ IComponentName* cName,
        /* [out] */ ISearchableInfo** info);

    CARAPI_(void) AddActionKey(
        /* [in] */ IActionKeyInfo* keyInfo);

private:
    // general debugging support
    static const Boolean DBG = FALSE;
    static const String TAG;

    // static strings used for XML lookups.
    // TODO how should these be documented for the developer, in a more structured way than
    // the current long wordy javadoc in SearchManager.java ?
    static const String MD_LABEL_SEARCHABLE;
    static const String MD_XML_ELEMENT_SEARCHABLE;
    static const String MD_XML_ELEMENT_SEARCHABLE_ACTION_KEY;

    // flags in the searchMode attribute
    static const Int32 SEARCH_MODE_BADGE_LABEL = 0x04;
    static const Int32 SEARCH_MODE_BADGE_ICON = 0x08;
    static const Int32 SEARCH_MODE_QUERY_REWRITE_FROM_DATA = 0x10;
    static const Int32 SEARCH_MODE_QUERY_REWRITE_FROM_TEXT = 0x20;

    // true member variables - what we know about the searchability
    Int32 mLabelId;
    AutoPtr<IComponentName> mSearchActivity;
    Int32 mHintId;
    Int32 mSearchMode;
    Int32 mIconId;
    Int32 mSearchButtonText;
    Int32 mSearchInputType;
    Int32 mSearchImeOptions;
    Boolean mIncludeInGlobalSearch;
    Boolean mQueryAfterZeroResults;
    Boolean mAutoUrlDetect;
    Int32 mSettingsDescriptionId;
    String mSuggestAuthority;
    String mSuggestPath;
    String mSuggestSelection;
    String mSuggestIntentAction;
    String mSuggestIntentData;
    Int32 mSuggestThreshold;
    // Maps key codes to action key information. auto-boxing is not so bad here,
    // since keycodes for the hard keys are < 127. For such values, Integer.valueOf()
    // uses shared Integer objects.
    // This is not final, to allow lazy initialization.
    HashMap<Int32, AutoPtr<IActionKeyInfo> > mActionKeys;
    String mSuggestProviderPackage;

    // Flag values for Searchable_voiceSearchMode
    static const Int32 VOICE_SEARCH_SHOW_BUTTON = 1;
    static const Int32 VOICE_SEARCH_LAUNCH_WEB_SEARCH = 2;
    static const Int32 VOICE_SEARCH_LAUNCH_RECOGNIZER = 4;
    Int32 mVoiceSearchMode;
    Int32 mVoiceLanguageModeId;       // voiceLanguageModel
    Int32 mVoicePromptTextId;         // voicePromptText
    Int32 mVoiceLanguageId;           // voiceLanguage
    Int32 mVoiceMaxResults;           // voiceMaxResults
};

}// namespace App
}// namespace Droid
}// namespace Elastos
#endif
