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

#ifndef __ELASTOS_DROID_SERVICE_TEXTSERVICE_SPELLCHECKERSERVICE_H__
#define __ELASTOS_DROID_SERVICE_TEXTSERVICE_SPELLCHECKERSERVICE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Text.h"
#include "Elastos.Droid.View.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/app/Service.h"

using Elastos::Droid::App::Service;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Text::Method::IWordIterator;
using Elastos::Droid::Internal::TextService::IISpellCheckerSessionListener;
using Elastos::Droid::Internal::TextService::IISpellCheckerService;
using Elastos::Droid::Internal::TextService::IISpellCheckerSession;
using Elastos::Droid::View::TextService::ISentenceSuggestionsInfo;
using Elastos::Droid::View::TextService::ISuggestionsInfo;
using Elastos::Droid::View::TextService::ITextInfo;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Service {
namespace TextService {

/**
 * SpellCheckerService provides an abstract base class for a spell checker.
 * This class combines a service to the system with the spell checker service interface that
 * spell checker must implement.
 *
 * <p>In addition to the normal Service lifecycle methods, this class
 * introduces a new specific callback that subclasses should override
 * {@link #createSession()} to provide a spell checker session that is corresponding
 * to requested language and so on. The spell checker session returned by this method
 * should extend {@link SpellCheckerService.Session}.
 * </p>
 *
 * <h3>Returning spell check results</h3>
 *
 * <p>{@link SpellCheckerService.Session#onGetSuggestions(TextInfo, int)}
 * should return spell check results.
 * It receives {@link android.view.textservice.TextInfo} and returns
 * {@link android.view.textservice.SuggestionsInfo} for the input.
 * You may want to override
 * {@link SpellCheckerService.Session#onGetSuggestionsMultiple(TextInfo[], int, boolean)} for
 * better performance and quality.
 * </p>
 *
 * <p>Please note that {@link SpellCheckerService.Session#getLocale()} does not return a valid
 * locale before {@link SpellCheckerService.Session#onCreate()} </p>
 *
 */
class SpellCheckerService
    : public Elastos::Droid::App::Service
    , public ISpellCheckerService
{
public:
    class SentenceLevelAdapter
        : public Object
    {
        friend class SpellCheckerService;
    public:
        class SentenceWordItem
            : public Object
        {
            friend class SentenceLevelAdapter;
        public:
            SentenceWordItem(
                /* [in] */ ITextInfo* ti,
                /* [in] */ Int32 start,
                /* [in] */ Int32 end);

        public:
            const AutoPtr<ITextInfo> mTextInfo;
            const Int32 mStart;
            const Int32 mLength;
        };

        class SentenceTextInfoParams
            : public Object
        {
            friend class SentenceLevelAdapter;
        public:
            SentenceTextInfoParams(
                /* [in] */ ITextInfo* ti,
                /* [in] */ IArrayList* items);

        public:
            const AutoPtr<ITextInfo> mOriginalTextInfo;
            const AutoPtr<IArrayList> mItems;
            Int32 mSize;
        };

    public:
        SentenceLevelAdapter(
            /* [in] */ ILocale* locale,
            /* [in] */ SpellCheckerService* host);

        static AutoPtr<ISentenceSuggestionsInfo> ReconstructSuggestions(
            /* [in] */ SentenceTextInfoParams* originalTextInfoParams,
            /* [in] */ ArrayOf<ISuggestionsInfo*>* results);

    private:
        CARAPI_(AutoPtr<SentenceTextInfoParams>) GetSplitWords(
            /* [in] */ ITextInfo* originalTextInfo);

    public:
        const static AutoPtr<ArrayOf<ISentenceSuggestionsInfo*> > EMPTY_SENTENCE_SUGGESTIONS_INFOS;

    private:
        static AutoPtr<ISuggestionsInfo> EMPTY_SUGGESTIONS_INFO;
        AutoPtr<IWordIterator> mWordIterator;
        SpellCheckerService* mHost;
    };

    class Session
        : public Object
        , public ISpellCheckerServiceSession
    {
        friend class SpellCheckerService;
        friend class SentenceLevelAdapter;
        friend class SentenceLevelAdapter::SentenceTextInfoParams;
        friend class SentenceLevelAdapter::SentenceWordItem;
    public:
        CAR_INTERFACE_DECL()

        Session(
            /* [in] */ SpellCheckerService* host);

        /**
         * @hide
         */
        CARAPI SetInternalISpellCheckerSession(
            /* [in] */ IInternalISpellCheckerSession* session);

        /**
         * This is called after the class is initialized, at which point it knows it can call
         * getLocale() etc...
         */
        virtual CARAPI OnCreate() = 0;

        /**
         * Get suggestions for specified text in TextInfo.
         * This function will run on the incoming IPC thread.
         * So, this is not called on the main thread,
         * but will be called in series on another thread.
         * @param textInfo the text metadata
         * @param suggestionsLimit the maximum number of suggestions to be returned
         * @return SuggestionsInfo which contains suggestions for textInfo
         */
        virtual CARAPI OnGetSuggestions(
            /* [in] */ ITextInfo* textInfo,
            /* [in] */ Int32 suggestionsLimit,
            /* [out] */ ISuggestionsInfo** si) = 0;

        /**
         * A batch process of onGetSuggestions.
         * This function will run on the incoming IPC thread.
         * So, this is not called on the main thread,
         * but will be called in series on another thread.
         * @param textInfos an array of the text metadata
         * @param suggestionsLimit the maximum number of suggestions to be returned
         * @param sequentialWords true if textInfos can be treated as sequential words.
         * @return an array of {@link SentenceSuggestionsInfo} returned by
         * {@link SpellCheckerService.Session#onGetSuggestions(TextInfo, int)}
         */
        CARAPI OnGetSuggestionsMultiple(
            /* [in] */ ArrayOf<ITextInfo*>* textInfos,
            /* [in] */ Int32 suggestionsLimit,
            /* [in] */ Boolean sequentialWords,
            /* [out, callee] */ ArrayOf<ISuggestionsInfo*>** sis);

        /**
         * Get sentence suggestions for specified texts in an array of TextInfo.
         * The default implementation splits the input text to words and returns
         * {@link SentenceSuggestionsInfo} which contains suggestions for each word.
         * This function will run on the incoming IPC thread.
         * So, this is not called on the main thread,
         * but will be called in series on another thread.
         * When you override this method, make sure that suggestionsLimit is applied to suggestions
         * that share the same start position and length.
         * @param textInfos an array of the text metadata
         * @param suggestionsLimit the maximum number of suggestions to be returned
         * @return an array of {@link SentenceSuggestionsInfo} returned by
         * {@link SpellCheckerService.Session#onGetSuggestions(TextInfo, int)}
         */
        CARAPI OnGetSentenceSuggestionsMultiple(
            /* [in] */ ArrayOf<ITextInfo*>* textInfos,
            /* [in] */ Int32 suggestionsLimit,
            /* [out, callee] */ ArrayOf<ISentenceSuggestionsInfo*>** ssis);

        /**
         * Request to abort all tasks executed in SpellChecker.
         * This function will run on the incoming IPC thread.
         * So, this is not called on the main thread,
         * but will be called in series on another thread.
         */
        CARAPI OnCancel();

        /**
         * Request to close this session.
         * This function will run on the incoming IPC thread.
         * So, this is not called on the main thread,
         * but will be called in series on another thread.
         */
        CARAPI OnClose();

        /**
         * @return Locale for this session
         */
        CARAPI GetLocale(
            /* [out] */ String* locale);

        /**
         * @return Bundle for this session
         */
        CARAPI GetBundle(
            /* [out] */ IBundle** bundle);

    private:
        AutoPtr<IInternalISpellCheckerSession> mInternalSession;
        AutoPtr<SentenceLevelAdapter> mSentenceLevelAdapter;
        SpellCheckerService* mHost;
    };

    class InternalISpellCheckerSession
        : public Object
        , public IInternalISpellCheckerSession
        , public IISpellCheckerSession
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        InternalISpellCheckerSession();

        CARAPI constructor(
            /* [in] */ const String& locale,
            /* [in] */ IISpellCheckerSessionListener* listener,
            /* [in] */ IBundle* bundle,
            /* [in] */ ISpellCheckerServiceSession* session);

        //@Override
        CARAPI OnGetSuggestionsMultiple(
            /* [in] */ ArrayOf<ITextInfo*>* textInfos,
            /* [in] */ Int32 suggestionsLimit,
            /* [in] */ Boolean sequentialWords);

        //@Override
        CARAPI OnGetSentenceSuggestionsMultiple(
            /* [in] */ ArrayOf<ITextInfo*>* textInfos,
            /* [in] */ Int32 suggestionsLimit);

        //@Override
        CARAPI OnCancel();

        //@Override
        CARAPI OnClose();

        CARAPI GetLocale(
            /* [out] */ String* locale);

        CARAPI GetBundle(
            /* [out] */ IBundle** bundle);

        ///pay for someone's error,he write a ToString in Binder.car
        CARAPI ToString(
            /* [out] */ String* str)
        {
            return NOERROR;
        }
        ///pay for someone's error

    private:
        AutoPtr<IISpellCheckerSessionListener> mListener;
        AutoPtr<ISpellCheckerServiceSession> mSession;
        String mLocale;
        AutoPtr<IBundle> mBundle;
    };

    class SpellCheckerServiceBinder
        : public Object
        , public ISpellCheckerServiceBinder
        , public IISpellCheckerService
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        SpellCheckerServiceBinder();

        CARAPI constructor(
            /* [in] */ ISpellCheckerService* service);

        //@Override
        CARAPI GetISpellCheckerSession(
            /* [in] */ const String& locale,
            /* [in] */ IISpellCheckerSessionListener* listener,
            /* [in] */ IBundle* bundle,
            /* [out] */ IISpellCheckerSession** session);

        ///pay for someone's error,he write a ToString in Binder.car
        CARAPI ToString(
            /* [out] */ String* str)
        {
            return NOERROR;
        }
        ///pay for someone's error

    private:
        AutoPtr<IWeakReference> mInternalServiceRef;
    };

public:
    CAR_INTERFACE_DECL()

    SpellCheckerService();

    /**
     * Implement to return the implementation of the internal spell checker
     * service interface. Subclasses should not override.
     */
    // @Override
    CARAPI OnBind(
        /* [in] */ IIntent* intent,
        /* [out] */ IBinder** b);

    /**
     * Factory method to create a spell checker session impl
     * @return SpellCheckerSessionImpl which should be overridden by a concrete implementation.
     */
    virtual CARAPI CreateSession(
        /* [out] */ ISpellCheckerServiceSession** session) = 0;

private:
    static const String TAG;
    static const Boolean DBG;
    AutoPtr<ISpellCheckerServiceBinder> mBinder;
};

} // namespace TextService
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_TEXTSERVICE_SPELLCHECKERSERVICE_H__
