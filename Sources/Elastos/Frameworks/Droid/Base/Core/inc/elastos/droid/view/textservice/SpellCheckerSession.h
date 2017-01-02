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

#ifndef __ELASTOS_DROID_VIEW_TEXTSERVICE_SPELLCHECKERSESSION_H__
#define __ELASTOS_DROID_VIEW_TEXTSERVICE_SPELLCHECKERSESSION_H__

#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/os/Handler.h"

#include <elastos/core/Object.h>

using Elastos::Droid::Os::Handler;
using Elastos::Droid::Internal::TextService::IITextServicesSessionListener;
using Elastos::Droid::Internal::TextService::IISpellCheckerSessionListener;
using Elastos::Droid::Internal::TextService::IITextServicesManager;

namespace Elastos {
namespace Droid {
namespace View {
namespace TextService {

/**
 * The SpellCheckerSession interface provides the per client functionality of SpellCheckerService.
 *
 *
 * <a name="Applications"></a>
 * <h3>Applications</h3>
 *
 * <p>In most cases, applications that are using the standard
 * {@link android.widget.TextView} or its subclasses will have little they need
 * to do to work well with spell checker services.  The main things you need to
 * be aware of are:</p>
 *
 * <ul>
 * <li> Properly set the {@link android.R.attr#inputType} in your editable
 * text views, so that the spell checker will have enough context to help the
 * user in editing text in them.
 * </ul>
 *
 * <p>For the rare people amongst us writing client applications that use the spell checker service
 * directly, you will need to use {@link #getSuggestions(TextInfo, int)} or
 * {@link #getSuggestions(TextInfo[], int, boolean)} for obtaining results from the spell checker
 * service by yourself.</p>
 *
 * <h3>Security</h3>
 *
 * <p>There are a lot of security issues associated with spell checkers,
 * since they could monitor all the text being sent to them
 * through, for instance, {@link android.widget.TextView}.
 * The Android spell checker framework also allows
 * arbitrary third party spell checkers, so care must be taken to restrict their
 * selection and interactions.</p>
 *
 * <p>Here are some key points about the security architecture behind the
 * spell checker framework:</p>
 *
 * <ul>
 * <li>Only the system is allowed to directly access a spell checker framework's
 * {@link android.service.textservice.SpellCheckerService} interface, via the
 * {@link android.Manifest.permission#BIND_TEXT_SERVICE} permission.  This is
 * enforced in the system by not binding to a spell checker service that does
 * not require this permission.
 *
 * <li>The user must explicitly enable a new spell checker in settings before
 * they can be enabled, to confirm with the system that they know about it
 * and want to make it available for use.
 * </ul>
 *
 */
class SpellCheckerSession
    : public Object
    , public ISpellCheckerSession
{
private:
    /** Handler that will execute the main tasks */
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("SpellCheckerSession::MyHandler")

        MyHandler(
            /* [in] */ SpellCheckerSession* host)
            : mHost(host)
        {}

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        SpellCheckerSession* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    SpellCheckerSession();

    /**
     * Constructor
     * @hide
     */
    CARAPI constructor(
        /* [in] */ ISpellCheckerInfo* info,
        /* [in] */ IITextServicesManager* tsm,
        /* [in] */ ISpellCheckerSessionListener* listener,
        /* [in] */ ISpellCheckerSubtype* subtype);

    /**
     * @return true if the connection to a text service of this session is disconnected and not
     * alive.
     */
    CARAPI IsSessionDisconnected(
        /* [out] */ Boolean* result);

    /**
     * Get the spell checker service info this spell checker session has.
     * @return SpellCheckerInfo for the specified locale.
     */
    CARAPI GetSpellChecker(
        /* [out] */ ISpellCheckerInfo** info);

    /**
     * Cancel pending and running spell check tasks
     */
    CARAPI Cancel();

    /**
     * Finish this session and allow TextServicesManagerService to disconnect the bound spell
     * checker.
     */
    CARAPI Close();

    /**
     * Get suggestions from the specified sentences
     * @param textInfos an array of text metadata for a spell checker
     * @param suggestionsLimit the maximum number of suggestions that will be returned
     */
    CARAPI GetSentenceSuggestions(
        /* [in] */ ArrayOf<ITextInfo*>* textInfos,
        /* [in] */ Int32 suggestionsLimit);

    /**
     * Get candidate strings for a substring of the specified text.
     * @param textInfo text metadata for a spell checker
     * @param suggestionsLimit the maximum number of suggestions that will be returned
     * @deprecated use {@link SpellCheckerSession#getSentenceSuggestions(TextInfo[], int)} instead
     */
    //@Deprecated
    CARAPI GetSuggestions(
        /* [in] */ ITextInfo* textInfo,
        /* [in] */ Int32 suggestionsLimit);

    /**
     * A batch process of getSuggestions
     * @param textInfos an array of text metadata for a spell checker
     * @param suggestionsLimit the maximum number of suggestions that will be returned
     * @param sequentialWords true if textInfos can be treated as sequential words.
     * @deprecated use {@link SpellCheckerSession#getSentenceSuggestions(TextInfo[], int)} instead
     */
    //@Deprecated
    CARAPI GetSuggestions(
        /* [in] */ ArrayOf<ITextInfo*>* textInfos,
        /* [in] */ Int32 suggestionsLimit,
        /* [in] */ Boolean sequentialWords);

    /**
     * @hide
     */
    CARAPI GetTextServicesSessionListener(
        /* [out] */ IITextServicesSessionListener** listener);

    /**
     * @hide
     */
    CARAPI GetSpellCheckerSessionListener(
        /* [out]*/ IISpellCheckerSessionListener** listener);

protected:
    CARAPI_(void) Finalize();

private:
    CARAPI_(void) HandleOnGetSuggestionsMultiple(
        /* [in] */ ArrayOf<ISuggestionsInfo*>* suggestionInfos);

    CARAPI_(void) HandleOnGetSentenceSuggestionsMultiple(
        /* [in] */ ArrayOf<ISentenceSuggestionsInfo*>* suggestionInfos);
public:
    /**
     * Name under which a SpellChecker service component publishes information about itself.
     * This meta-data must reference an XML resource.
     **/
    static const String SERVICE_META_DATA;

private:
    static const String TAG;
    static const Boolean DBG;

    AutoPtr<IInternalListener> mInternalListener;
    AutoPtr<IITextServicesManager> mTextServicesManager;
    AutoPtr<ISpellCheckerInfo> mSpellCheckerInfo;
    AutoPtr<ISpellCheckerSessionListenerImpl> mSpellCheckerSessionListenerImpl;
    AutoPtr<ISpellCheckerSubtype> mSubtype;

    Boolean mIsUsed;
    AutoPtr<ISpellCheckerSessionListener> mSpellCheckerSessionListener;

    /** Handler that will execute the main tasks */
    AutoPtr<IHandler> mHandler;
};

}   //namespace TextService
}   //namespace View
}   //namespace Droid
}   //namespace Elastos

#endif //__ELASTOS_DROID_VIEW_TEXTSERVICE_SPELLCHECKERSESSION_H__
