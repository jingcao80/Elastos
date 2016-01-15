
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/textservice/SpellCheckerSession.h"
#include "elastos/droid/view/textservice/CSpellCheckerSessionListenerImpl.h"
#include "elastos/droid/view/textservice/CInternalListener.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace View {
namespace TextService {

//========================================================================================
//              SpellCheckerSession::MyHandler::
//========================================================================================
ECode SpellCheckerSession::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);

    switch (what) {
        case ISpellCheckerSession::MSG_ON_GET_SUGGESTION_MULTIPLE: {
            assert(0 && "TODO");
//            mHost->HandleOnGetSuggestionsMultiple((SuggestionsInfo[]) obj);
            break;
        }
        case ISpellCheckerSession::MSG_ON_GET_SUGGESTION_MULTIPLE_FOR_SENTENCE: {
            assert(0 && "TODO");
//            mHost->HandleOnGetSentenceSuggestionsMultiple((SentenceSuggestionsInfo[]) obj);
            break;
        }
    }
    return NOERROR;
}

//========================================================================================
//              SpellCheckerSession::
//========================================================================================
CAR_INTERFACE_IMPL(SpellCheckerSession, Object, ISpellCheckerSession)

const String SpellCheckerSession::TAG("SpellCheckerSession");
const Boolean SpellCheckerSession::DBG = FALSE;
const String SpellCheckerSession::SERVICE_META_DATA("android.view.textservice.scs");

SpellCheckerSession::SpellCheckerSession()
{
    mHandler = new MyHandler(this);
}

ECode SpellCheckerSession::constructor(
    /* [in] */ ISpellCheckerInfo* info,
    /* [in] */ IITextServicesManager* tsm,
    /* [in] */ ISpellCheckerSessionListener* listener,
    /* [in] */ ISpellCheckerSubtype* subtype)
{
    if (info == NULL || listener == NULL || tsm == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }

    mSpellCheckerInfo = info;
    CSpellCheckerSessionListenerImpl::New(mHandler, (ISpellCheckerSessionListenerImpl**)&mSpellCheckerSessionListenerImpl);
    CInternalListener::New(mSpellCheckerSessionListenerImpl, (IInternalListener**)&mInternalListener);
    mTextServicesManager = tsm;
    mIsUsed = TRUE;
    mSpellCheckerSessionListener = listener;
    mSubtype = subtype;
    return NOERROR;
}

ECode SpellCheckerSession::IsSessionDisconnected(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mSpellCheckerSessionListenerImpl->IsDisconnected(result);
}

ECode SpellCheckerSession::GetSpellChecker(
    /* [out] */ ISpellCheckerInfo** info)
{
    VALIDATE_NOT_NULL(info)
    *info = ISpellCheckerInfo::Probe(mSpellCheckerSessionListenerImpl);
    REFCOUNT_ADD(*info)
    return NOERROR;
}

ECode SpellCheckerSession::Cancel()
{
    return mSpellCheckerSessionListenerImpl->Cancel();
}

ECode SpellCheckerSession::Close()
{
    mIsUsed = FALSE;
    mSpellCheckerSessionListenerImpl->Close();
    mTextServicesManager->FinishSpellCheckerService(IISpellCheckerSessionListener::Probe(mSpellCheckerSessionListenerImpl));
    return NOERROR;
}

ECode SpellCheckerSession::GetSentenceSuggestions(
    /* [in] */ ArrayOf<ITextInfo*>* textInfos,
    /* [in] */ Int32 suggestionsLimit)
{
    return mSpellCheckerSessionListenerImpl->GetSentenceSuggestionsMultiple(
                textInfos, suggestionsLimit);
}

ECode SpellCheckerSession::GetSuggestions(
    /* [in] */ ITextInfo* textInfo,
    /* [in] */ Int32 suggestionsLimit)
{
    AutoPtr<ArrayOf<ITextInfo*> > textInfoArray = ArrayOf<ITextInfo*>::Alloc(1);
    (*textInfoArray)[0] = textInfo;
    GetSuggestions(textInfoArray, suggestionsLimit, FALSE);
    return NOERROR;
}

ECode SpellCheckerSession::GetSuggestions(
    /* [in] */ ArrayOf<ITextInfo*>* textInfos,
    /* [in] */ Int32 suggestionsLimit,
    /* [in] */ Boolean sequentialWords)
{
    if (DBG) {
        String strID;
        mSpellCheckerInfo->GetId(&strID);
        Logger::W(TAG, "getSuggestions from %s", (const char*)strID);
    }
    mSpellCheckerSessionListenerImpl->GetSuggestionsMultiple(
            textInfos, suggestionsLimit, sequentialWords);
    return NOERROR;
}

ECode SpellCheckerSession::GetTextServicesSessionListener(
    /* [out] */ IITextServicesSessionListener** listener)
{
    VALIDATE_NOT_NULL(listener)
    *listener = IITextServicesSessionListener::Probe(mInternalListener);
    REFCOUNT_ADD(*listener)
    return NOERROR;
}

ECode SpellCheckerSession::GetSpellCheckerSessionListener(
    /* [out]*/ IISpellCheckerSessionListener** listener)
{
    VALIDATE_NOT_NULL(listener)
    *listener = IISpellCheckerSessionListener::Probe(mSpellCheckerSessionListenerImpl);
    REFCOUNT_ADD(*listener)
    return NOERROR;
}

void SpellCheckerSession::HandleOnGetSuggestionsMultiple(
    /* [in] */ ArrayOf<ISuggestionsInfo*>* suggestionInfos)
{
    mSpellCheckerSessionListener->OnGetSuggestions(suggestionInfos);
}

void SpellCheckerSession::HandleOnGetSentenceSuggestionsMultiple(
    /* [in] */ ArrayOf<ISentenceSuggestionsInfo*>* suggestionInfos)
{
    mSpellCheckerSessionListener->OnGetSentenceSuggestions(suggestionInfos);
}

}   //namespace TextService
}   //namespace View
}   //namespace Droid
}   //namespace Elastos
