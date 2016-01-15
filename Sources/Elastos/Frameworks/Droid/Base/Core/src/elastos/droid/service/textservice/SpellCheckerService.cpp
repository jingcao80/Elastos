
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.Text.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/service/textservice/CInternalISpellCheckerSession.h"
#include "elastos/droid/service/textservice/CSpellCheckerServiceBinder.h"
#include "elastos/droid/service/textservice/SpellCheckerService.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/text/method/CWordIterator.h"
#include "elastos/droid/view/textservice/CSentenceSuggestionsInfo.h"
#include "elastos/droid/view/textservice/CSuggestionsInfo.h"
#include "elastos/droid/view/textservice/CTextInfo.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Service::Textservice::CInternalISpellCheckerSession;
using Elastos::Droid::Service::Textservice::CSpellCheckerServiceBinder;
using Elastos::Droid::Text::ISelectionPositionIterator;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::Method::CWordIterator;
using Elastos::Droid::View::Internal::TextService::EIID_IISpellCheckerService;
using Elastos::Droid::View::Internal::TextService::EIID_IISpellCheckerSession;
using Elastos::Droid::View::TextService::CSentenceSuggestionsInfo;
using Elastos::Droid::View::TextService::CSuggestionsInfo;
using Elastos::Droid::View::TextService::CTextInfo;
using Elastos::Core::AutoLock;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Text::IBreakIterator;
using Elastos::Utility::CLocale;
using Elastos::Utility::IArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Textservice {

//=============
// SpellCheckerService
//=============

const String SpellCheckerService::TAG("SpellCheckerService");
const Boolean SpellCheckerService::DBG = FALSE;

CAR_INTERFACE_IMPL(SpellCheckerService, Service, ISpellCheckerService)

SpellCheckerService::SpellCheckerService()
{
    CSpellCheckerServiceBinder::New((ISpellCheckerService*)this, (ISpellCheckerServiceBinder**)&mBinder);
}

ECode SpellCheckerService::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** b)
{
    VALIDATE_NOT_NULL(b)
    if (DBG) {
        Logger::W(TAG, "onBind");
    }
    *b = IBinder::Probe(mBinder.Get());
    REFCOUNT_ADD(*b)
    return NOERROR;
}

//===================
// SpellCheckerService::Session
//===================

CAR_INTERFACE_IMPL(SpellCheckerService::Session, Object, ISpellCheckerServiceSession)

SpellCheckerService::Session::Session(
    /* [in] */ SpellCheckerService* host)
    : mHost(host)
{
}

ECode SpellCheckerService::Session::SetInternalISpellCheckerSession(
    /* [in] */ IInternalISpellCheckerSession* session)
{
    mInternalSession = session;
    return NOERROR;
}

ECode SpellCheckerService::Session::OnGetSuggestionsMultiple(
    /* [in] */ ArrayOf<ITextInfo*>* textInfos,
    /* [in] */ Int32 suggestionsLimit,
    /* [in] */ Boolean sequentialWords,
    /* [out, callee] */ ArrayOf<ISuggestionsInfo*>** sis)
{
    VALIDATE_NOT_NULL(sis)
    const Int32 length = textInfos->GetLength();
    AutoPtr<ArrayOf<ISuggestionsInfo*> > retval = ArrayOf<ISuggestionsInfo*>::Alloc(length);
    for (Int32 i = 0; i < length; ++i) {
        OnGetSuggestions((*textInfos)[i], suggestionsLimit, (ISuggestionsInfo**)&((*retval)[i]));
        Int32 cookie, sequence;
        (*textInfos)[i]->GetCookie(&cookie);
        (*textInfos)[i]->GetSequence(&sequence);
        (*retval)[i]->SetCookieAndSequence(cookie, sequence);
    }
    *sis = retval;
    REFCOUNT_ADD(*sis)
    return NOERROR;
}

ECode SpellCheckerService::Session::OnGetSentenceSuggestionsMultiple(
    /* [in] */ ArrayOf<ITextInfo*>* textInfos,
    /* [in] */ Int32 suggestionsLimit,
    /* [out, callee] */ ArrayOf<ISentenceSuggestionsInfo*>** ssis)
{
    VALIDATE_NOT_NULL(ssis)
    if (textInfos == NULL || textInfos->GetLength() == 0) {
        *ssis = SentenceLevelAdapter::EMPTY_SENTENCE_SUGGESTIONS_INFOS;
        REFCOUNT_ADD(*ssis)
        return NOERROR;
    }
    if (mHost->DBG) {
        Logger::D(TAG, "onGetSentenceSuggestionsMultiple: + %d, %d", textInfos->GetLength(), suggestionsLimit);
    }
    if (mSentenceLevelAdapter == NULL) {
        synchronized(this){
            if (mSentenceLevelAdapter == NULL) {
                String localeStr;
                GetLocale(&localeStr);
                if (!TextUtils::IsEmpty(localeStr)) {
                    AutoPtr<ILocale> locale;
                    CLocale::New(localeStr, (ILocale**)&locale);
                    mSentenceLevelAdapter = new SentenceLevelAdapter(locale.Get(), mHost);
                }
            }
        }
    }
    if (mSentenceLevelAdapter == NULL) {
        *ssis = SentenceLevelAdapter::EMPTY_SENTENCE_SUGGESTIONS_INFOS;
        REFCOUNT_ADD(*ssis)
        return NOERROR;
    }
    Int32 infosSize = textInfos->GetLength();
    AutoPtr<ArrayOf<ISentenceSuggestionsInfo*> > retval = ArrayOf<ISentenceSuggestionsInfo*>::Alloc(infosSize);
    for (Int32 i = 0; i < infosSize; ++i) {
        AutoPtr<SentenceLevelAdapter::SentenceTextInfoParams> textInfoParams =
                mSentenceLevelAdapter->GetSplitWords((*textInfos)[i]);
        AutoPtr<IArrayList> mItems = textInfoParams->mItems;
        Int32 itemsSize;
        mItems->GetSize(&itemsSize);
        AutoPtr<ArrayOf<ITextInfo*> > splitTextInfos = ArrayOf<ITextInfo*>::Alloc(itemsSize);
        for (Int32 j = 0; j < itemsSize; ++j) {
            AutoPtr<IInterface> obj;
            mItems->Get(j, (IInterface**)&obj);
            AutoPtr<IObject> _obj = IObject::Probe(obj);
            AutoPtr<SentenceLevelAdapter::SentenceWordItem> item = (SentenceLevelAdapter::SentenceWordItem*)(_obj.Get());
            (*splitTextInfos)[j] = item->mTextInfo;
        }
        AutoPtr<ArrayOf<ISuggestionsInfo*> > results;
        OnGetSuggestionsMultiple(splitTextInfos, suggestionsLimit, TRUE, (ArrayOf<ISuggestionsInfo*>**)&results);
        (*retval)[i] = SentenceLevelAdapter::ReconstructSuggestions(textInfoParams, results.Get());
    }
    *ssis = retval.Get();
    REFCOUNT_ADD(*ssis)
    return NOERROR;
}

ECode SpellCheckerService::Session::OnCancel()
{
    return NOERROR;
}

ECode SpellCheckerService::Session::OnClose()
{
    return NOERROR;
}

ECode SpellCheckerService::Session::GetLocale(
    /* [out] */ String* locale)
{
    VALIDATE_NOT_NULL(locale)
    return mInternalSession->GetLocale(locale);
}

ECode SpellCheckerService::Session::GetBundle(
    /* [out] */ IBundle** bundle)
{
    VALIDATE_NOT_NULL(bundle)
    return mInternalSession->GetBundle(bundle);
}

//===============================
// SpellCheckerService::InternalISpellCheckerSession
//===============================

CAR_INTERFACE_IMPL_3(SpellCheckerService::InternalISpellCheckerSession, Object, IInternalISpellCheckerSession, IISpellCheckerSession, IBinder)

SpellCheckerService::InternalISpellCheckerSession::InternalISpellCheckerSession()
{
}

ECode SpellCheckerService::InternalISpellCheckerSession::constructor(
    /* [in] */ const String& locale,
    /* [in] */ IISpellCheckerSessionListener* listener,
    /* [in] */ IBundle* bundle,
    /* [in] */ ISpellCheckerServiceSession* session)
{
    mListener = listener;
    mSession = session;
    mLocale = locale;
    mBundle = bundle;
    session->SetInternalISpellCheckerSession((IInternalISpellCheckerSession*)this);
    return NOERROR;
}

ECode SpellCheckerService::InternalISpellCheckerSession::OnGetSuggestionsMultiple(
    /* [in] */ ArrayOf<ITextInfo*>* textInfos,
    /* [in] */ Int32 suggestionsLimit,
    /* [in] */ Boolean sequentialWords)
{
    Int32 pri;
    Process::GetThreadPriority(Process::MyTid(), &pri);
    Process::SetThreadPriority(IProcess::THREAD_PRIORITY_BACKGROUND);
#if 0 //TODO
    AutoPtr<ArrayOf<ISuggestionsInfo*> > results;
    ECode ec = mSession->OnGetSuggestionsMultiple(
        textInfos, suggestionsLimit, sequentialWords, (ArrayOf<ISuggestionsInfo*>**)&results);
    if (FAILED(ec)) {
        Process::SetThreadPriority(pri);
        return E_REMOTE_EXCEPTION;
    }
    mListener->OnGetSuggestions(results.Get());
#endif
    return NOERROR;
}

ECode SpellCheckerService::InternalISpellCheckerSession::OnGetSentenceSuggestionsMultiple(
    /* [in] */ ArrayOf<ITextInfo*>* textInfos,
    /* [in] */ Int32 suggestionsLimit)
{
#if 0 //TODO
    AutoPtr<ArrayOf<ISentenceSuggestionsInfo*> > results;
    ECode ec = mSession->OnGetSentenceSuggestionsMultiple(textInfos, suggestionsLimit, (ArrayOf<ISentenceSuggestionsInfo*>**)&results);
    if (FAILED(ec)) {
        return E_REMOTE_EXCEPTION;
    }
    mListener->OnGetSentenceSuggestions(results.Get());
#endif
    return NOERROR;
}

ECode SpellCheckerService::InternalISpellCheckerSession::OnCancel()
{
    Int32 pri;
    Process::GetThreadPriority(Process::MyTid(), &pri);
    Process::SetThreadPriority(IProcess::THREAD_PRIORITY_BACKGROUND);
    mSession->OnCancel();
    Process::SetThreadPriority(pri);
    return NOERROR;
}

ECode SpellCheckerService::InternalISpellCheckerSession::OnClose()
{
    Int32 pri;
    Process::GetThreadPriority(Process::MyTid(), &pri);
    Process::SetThreadPriority(IProcess::THREAD_PRIORITY_BACKGROUND);
    mSession->OnClose();
    Process::SetThreadPriority(pri);
    mListener = NULL;
    return NOERROR;
}

ECode SpellCheckerService::InternalISpellCheckerSession::GetLocale(
    /* [out] */ String* locale)
{
    VALIDATE_NOT_NULL(locale)
    *locale = mLocale;
    return NOERROR;
}

ECode SpellCheckerService::InternalISpellCheckerSession::GetBundle(
    /* [out] */ IBundle** bundle)
{
    VALIDATE_NOT_NULL(bundle)
    *bundle = mBundle;
    REFCOUNT_ADD(*bundle)
    return NOERROR;
}

//==============================
// SpellCheckerService::SpellCheckerServiceBinder
//==============================

CAR_INTERFACE_IMPL_3(SpellCheckerService::SpellCheckerServiceBinder, Object, ISpellCheckerServiceBinder, IISpellCheckerService, IBinder)

SpellCheckerService::SpellCheckerServiceBinder::SpellCheckerServiceBinder()
{
}

ECode SpellCheckerService::SpellCheckerServiceBinder::constructor(
    /* [in] */ ISpellCheckerService* service)
{
    AutoPtr<IWeakReferenceSource> wrs = IWeakReferenceSource::Probe(service);
    VALIDATE_NOT_NULL(wrs);
    mInternalServiceRef = NULL;
    wrs->GetWeakReference((IWeakReference**)&mInternalServiceRef);
    return NOERROR;
}

ECode SpellCheckerService::SpellCheckerServiceBinder::GetISpellCheckerSession(
    /* [in] */ const String& locale,
    /* [in] */ IISpellCheckerSessionListener* listener,
    /* [in] */ IBundle* bundle,
    /* [out] */ IISpellCheckerSession** s)
{
    VALIDATE_NOT_NULL(s)
    AutoPtr<ISpellCheckerService> service;
    mInternalServiceRef->Resolve(EIID_ISpellCheckerService, (IInterface**)&service);
    if (service == NULL) {
        *s = NULL;
        return NOERROR;
    }
    AutoPtr<ISpellCheckerServiceSession> session;
    service->CreateSession((ISpellCheckerServiceSession**)&session);
    AutoPtr<IInternalISpellCheckerSession> internalSession;
    CInternalISpellCheckerSession::New(locale, listener, bundle, session, (IInternalISpellCheckerSession**)&internalSession);
    session->OnCreate();
    *s = IISpellCheckerSession::Probe(internalSession.Get());
    REFCOUNT_ADD(*s)
    return NOERROR;
}

//========================================
// SpellCheckerService::SentenceLevelAdapter::SentenceWordItem
//========================================

SpellCheckerService::SentenceLevelAdapter::SentenceWordItem::SentenceWordItem(
    /* [in] */ ITextInfo* ti,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
    : mTextInfo(ti)
    , mStart(start)
    , mLength(end - start)
{
}

//============================================
// SpellCheckerService::SentenceLevelAdapter::SentenceTextInfoParams
//============================================

SpellCheckerService::SentenceLevelAdapter::SentenceTextInfoParams::SentenceTextInfoParams(
    /* [in] */ ITextInfo* ti,
    /* [in] */ IArrayList* items)
    : mOriginalTextInfo(ti)
    , mItems(items)
{
    items->GetSize(&mSize);
}

//============================
// SpellCheckerService::SentenceLevelAdapter
//============================

static AutoPtr<ISuggestionsInfo> Init()
{
    AutoPtr<CSuggestionsInfo> cs;
    CSuggestionsInfo::NewByFriend(0, NULL, (CSuggestionsInfo**)&cs);
    AutoPtr<ISuggestionsInfo> is = (ISuggestionsInfo*)(cs.Get());
    return is;
}

AutoPtr<ISuggestionsInfo> SpellCheckerService::SentenceLevelAdapter::EMPTY_SUGGESTIONS_INFO = Init();

SpellCheckerService::SentenceLevelAdapter::SentenceLevelAdapter(
    /* [in] */ ILocale* locale,
    /* [in] */ SpellCheckerService* host)
    : mHost(host)
{
    CWordIterator::New(locale, (IWordIterator**)&mWordIterator);
}

AutoPtr<SpellCheckerService::SentenceLevelAdapter::SentenceTextInfoParams> SpellCheckerService::SentenceLevelAdapter::GetSplitWords(
    /* [in] */ ITextInfo* originalTextInfo)
{
    AutoPtr<IWordIterator> wordIterator = mWordIterator;
    AutoPtr<ICharSequence> originalText;
    String text;
    originalTextInfo->GetText(&text);
    CString::New(text, (ICharSequence**)&originalText);
    Int32 cookie;
    originalTextInfo->GetCookie(&cookie);
    Int32 start = 0;
    Int32 end;
    originalText->GetLength(&end);
    AutoPtr<IArrayList> wordItems;
    wordIterator->SetCharSequence(originalText, 0, end);
    Int32 wordEnd;
    ISelectionPositionIterator::Probe(wordIterator)->Following(start, &wordEnd);
    Int32 wordStart;
     wordIterator->GetBeginning(wordEnd, &wordStart);
    if (SpellCheckerService::DBG) {
        Logger::D(TAG, "iterator: break: ---- 1st word start = %d, end = %d\n%p",wordStart , wordEnd, originalText.Get());
    }
    while (wordStart <= end && wordEnd != IBreakIterator::DONE
        && wordStart != IBreakIterator::DONE) {
        if (wordEnd >= start && wordEnd > wordStart) {
            AutoPtr<ICharSequence> query;
            originalText->SubSequence(wordStart, wordEnd, (ICharSequence**)&query);
            Int32 queryLength;
            query->GetLength(&queryLength);
            Int32 queryHashcode;
            IObject::Probe(query)->GetHashCode(&queryHashcode);
            AutoPtr<ITextInfo> ti;
            CTextInfo::New(query, 0, queryLength, cookie, queryHashcode, (ITextInfo**)&ti);
            AutoPtr<SentenceWordItem> swi = new SentenceWordItem(ti, wordStart, wordEnd);
            AutoPtr<IInterface> _swi = IInterface::Probe((IObject*)swi);
            wordItems->Add(_swi);
            if (SpellCheckerService::DBG) {
                Int32 size;
                wordItems->GetSize(&size);
                Logger::D(TAG, "Adapter: word (%d)%p", size- 1, query.Get());
            }
        }
        ISelectionPositionIterator::Probe(wordIterator)->Following(wordEnd, &wordEnd);
        if (wordEnd == IBreakIterator::DONE) {
            break;
        }
        wordIterator->GetBeginning(wordEnd, &wordStart);
    }
    AutoPtr<SentenceTextInfoParams> stip = new SentenceTextInfoParams(originalTextInfo, wordItems);
    return stip;
}

AutoPtr<ISentenceSuggestionsInfo> SpellCheckerService::SentenceLevelAdapter::ReconstructSuggestions(
    /* [in] */ SentenceTextInfoParams* originalTextInfoParams,
    /* [in] */ ArrayOf<ISuggestionsInfo*>* results)
{
    if (results == NULL || results->GetLength() == 0) {
        return NULL;
    }
    if (SpellCheckerService::DBG) {
        Logger::W(TAG, "Adapter: onGetSuggestions: got %d", results->GetLength());
    }
    if (originalTextInfoParams == NULL) {
        if (SpellCheckerService::DBG) {
            Logger::W(TAG, "Adapter: originalTextInfoParams is null.");
        }
        return NULL;
    }
    Int32 originalCookie;
    originalTextInfoParams->mOriginalTextInfo->GetCookie(&originalCookie);
    Int32 originalSequence;
    originalTextInfoParams->mOriginalTextInfo->GetSequence(&originalSequence);

    Int32 querySize = originalTextInfoParams->mSize;
    AutoPtr<ArrayOf<Int32> > offsets = ArrayOf<Int32>::Alloc(querySize);
    AutoPtr<ArrayOf<Int32> > lengths = ArrayOf<Int32>::Alloc(querySize);
    AutoPtr<ArrayOf<ISuggestionsInfo*> > reconstructedSuggestions = ArrayOf<ISuggestionsInfo*>::Alloc(querySize);

    for (Int32 i = 0; i < querySize; ++i) {
        AutoPtr<SentenceWordItem> item;
        originalTextInfoParams->mItems->Get(i, (IInterface**)&item);
        AutoPtr<ISuggestionsInfo> result = NULL;
        for (Int32 j = 0; j < results->GetLength(); ++j) {
            AutoPtr<ISuggestionsInfo> cur = (*results)[j];
            Int32 curSequence, tiSequence;
            cur->GetSequence(&curSequence);
            item->mTextInfo->GetSequence(&tiSequence);
            if (cur != NULL && curSequence == tiSequence) {
                result = cur;
                result->SetCookieAndSequence(originalCookie, originalSequence);
                break;
            }
        }
        (*offsets)[i] = item->mStart;
        (*lengths)[i] = item->mLength;
        (*reconstructedSuggestions)[i] = result != NULL ? result : EMPTY_SUGGESTIONS_INFO;
        if (SpellCheckerService::DBG) {
            Int32 size;
            (*reconstructedSuggestions)[i]->GetSuggestionsCount(&size);
            String sug;
            (*reconstructedSuggestions)[i]->GetSuggestionAt(0, &sug);
            String str = size > 0 ? sug : String("<none>");
            Logger::W(TAG, "reconstructedSuggestions(%d)%d, first = %s, offset = %d, length = %d",
                i ,size, str.string(), (*offsets)[i], (*lengths)[i]);
        }
    }
    AutoPtr<ISentenceSuggestionsInfo> ssinfo;
    CSentenceSuggestionsInfo::New(reconstructedSuggestions, offsets, lengths, (ISentenceSuggestionsInfo**)&ssinfo);
    return ssinfo;
}

} // namespace Service
} // namespace Textservice
} // namepsace Droid
} // namespace Elastos
