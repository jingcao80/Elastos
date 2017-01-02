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

#ifndef __ELASTOS_DROID_WIDGET_SPELLCHECKER_H__
#define __ELASTOS_DROID_WIDGET_SPELLCHECKER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Text.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.Droid.Utility.h>
#include <elastos/core/Runnable.h>

using Elastos::Core::Runnable;
using Elastos::Droid::Text::IEditable;
using Elastos::Droid::Text::Method::IWordIterator;
using Elastos::Droid::Text::Style::ISpellCheckSpan;
using Elastos::Droid::Text::Style::ISuggestionSpan;
using Elastos::Droid::Utility::ILruCache;
using Elastos::Droid::View::TextService::ISuggestionsInfo;
using Elastos::Droid::View::TextService::ISentenceSuggestionsInfo;
using Elastos::Droid::View::TextService::ISpellCheckerSession;
using Elastos::Droid::View::TextService::ITextServicesManager;
using Elastos::Droid::View::TextService::ISpellCheckerSessionListener;

using Elastos::Core::Object;
using Elastos::Utility::ILocale;

namespace Elastos {
namespace Droid {
namespace Widget {

class SpellChecker;

class SpellParser
    : public Object
{
public:
    SpellParser(
        /* [in] */ SpellChecker* host);

    CARAPI_(void) Parse(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    CARAPI_(Boolean) IsFinished();

    CARAPI_(void) Stop();

    CARAPI_(void) Parse();

private:
    CARAPI_(void) SetRangeSpan(
        /* [in] */ IEditable* editable,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    CARAPI_(void) RemoveRangeSpan (
        /* [in] */ IEditable* editable);

    CARAPI_(void) RemoveSpansAt(
        /* [in] */ IEditable* editable,
        /* [in] */ Int32 offset,
        /* [in] */ ArrayOf<IInterface*>* spans);
    //private <T> void removeSpansAt(Editable editable, int offset, T[] spans)
private:
    AutoPtr<IObject> mRange;
    SpellChecker* mHost;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::Widget::SpellParser, IInterface)

namespace Elastos {
namespace Droid {
namespace Widget {

class SpellChecker
    : public Object
    , public ISpellChecker
    , public ISpellCheckerSessionListener
{
    friend class SpellParser;
private:
    class SpellCheckerRunnable
        : public Runnable
    {
    public:
        SpellCheckerRunnable(
            /* [in] */ SpellChecker* host);

        CARAPI Run();

    private:
        SpellChecker* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    SpellChecker();

    CARAPI constructor (
        /* [in] */ ITextView* textView);

    CARAPI CloseSession();

    CARAPI OnSpellCheckSpanRemoved(
        /* [in] */ ISpellCheckSpan* spellCheckSpan);

    CARAPI OnSelectionChanged();

    CARAPI SpellCheck(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    // @Override
    CARAPI OnGetSuggestions(
        /* [in] */ ArrayOf<ISuggestionsInfo*>* results);

    // @Override
    CARAPI OnGetSentenceSuggestions(
        /* [in] */ ArrayOf<ISentenceSuggestionsInfo*>* results);

    static CARAPI_(Boolean) HaveWordBoundariesChanged(
        /* [in] */ IEditable* editable,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ Int32 spanStart,
        /* [in] */ Int32 spanEnd);

private:
    CARAPI_(void) ResetSession();

    CARAPI_(void) SetLocale(
        /* [in] */ ILocale* locale);

    CARAPI_(Boolean) IsSessionActive();

    CARAPI_(Int32) NextSpellCheckSpanIndex();

    CARAPI_(void) AddSpellCheckSpan(
        /* [in] */ IEditable* editable,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    CARAPI_(void) SpellCheck();

    CARAPI_(AutoPtr<ISpellCheckSpan>) OnGetSuggestionsInternal(
        /* [in] */ ISuggestionsInfo* suggestionsInfo,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length);

    CARAPI_(void) ScheduleNewSpellCheck();

    CARAPI_(void) CreateMisspelledSuggestionSpan(
        /* [in] */ IEditable* editable,
        /* [in] */ ISuggestionsInfo* suggestionsInfo,
        /* [in] */ ISpellCheckSpan* spellCheckSpan,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length);

private:
    static const String TAG;
    static const Boolean DBG;
    static const Int32 MAX_NUMBER_OF_WORDS = 50;
    static const Int32 AVERAGE_WORD_LENGTH = 7;
    static const Int32 WORD_ITERATOR_INTERVAL = MAX_NUMBER_OF_WORDS * AVERAGE_WORD_LENGTH;
    static const Int32 SPELL_PAUSE_DURATION = 400;
    static const Int32 MIN_SENTENCE_LENGTH = 50;
    static const Int32 USE_SPAN_RANGE = -1;
    static const Int32 SUGGESTION_SPAN_CACHE_SIZE = 10;

    AutoPtr<ITextView> mTextView;
    AutoPtr<ISpellCheckerSession> mSpellCheckerSession;

    Boolean mIsSentenceSpellCheckSupported;
    Int32 mCookie;
    AutoPtr<ArrayOf<Int32> > mIds;
    AutoPtr<ArrayOf<ISpellCheckSpan*> > mSpellCheckSpans;
    Int32 mLength;
    AutoPtr<ArrayOf<SpellParser*> > mSpellParsers;
    Int32 mSpanSequenceCounter;
    AutoPtr<ILocale> mCurrentLocale;
    AutoPtr<IWordIterator> mWordIterator;
    AutoPtr<ITextServicesManager> mTextServicesManager;
    AutoPtr<SpellCheckerRunnable> mSpellRunnable;

    AutoPtr<ILruCache> mSuggestionSpanCache;//LruCache<Int64, AutoPtr<ISuggestionSpan> >
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_SPELLCHECKER_H__
