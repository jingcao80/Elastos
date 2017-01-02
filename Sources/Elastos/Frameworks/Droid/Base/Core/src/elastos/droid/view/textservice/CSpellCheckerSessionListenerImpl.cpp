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

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/textservice/CSpellCheckerSessionListenerImpl.h"
#include "elastos/droid/os/CMessage.h"
#include "elastos/droid/os/CMessageHelper.h"

#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Os::CMessage;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Os::CMessageHelper;

using Elastos::Utility::Logging::Logger;
using Elastos::Utility::CLinkedList;

namespace Elastos {
namespace Droid {
namespace View {
namespace TextService {

//========================================================================================
//              CSpellCheckerSessionListenerImpl::MyHandler::
//========================================================================================
CSpellCheckerSessionListenerImpl::MyHandler::MyHandler(
    /* [in] */ ILooper* lp,
    /* [in] */ CSpellCheckerSessionListenerImpl* host)
    : Handler(lp)
    , mHost(host)
{}

ECode CSpellCheckerSessionListenerImpl::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    AutoPtr<SpellCheckerParams> scp = (SpellCheckerParams*)(IObject*)obj.Get();
    mHost->ProcessTask(IISpellCheckerSession::Probe(scp->mSession), scp, TRUE);
    return NOERROR;
}

//========================================================================================
//              CSpellCheckerSessionListenerImpl::
//========================================================================================
const String CSpellCheckerSessionListenerImpl::TAG("CSpellCheckerSessionListenerImpl");
const Boolean CSpellCheckerSessionListenerImpl::DBG = FALSE;

const Int32 CSpellCheckerSessionListenerImpl::TASK_CANCEL = 1;
const Int32 CSpellCheckerSessionListenerImpl::TASK_GET_SUGGESTIONS_MULTIPLE = 2;
const Int32 CSpellCheckerSessionListenerImpl::TASK_CLOSE = 3;
const Int32 CSpellCheckerSessionListenerImpl::TASK_GET_SUGGESTIONS_MULTIPLE_FOR_SENTENCE = 4;

CAR_INTERFACE_IMPL_2(CSpellCheckerSessionListenerImpl, Object, ISpellCheckerSessionListenerImpl, ISpellCheckerSessionListener)

CAR_OBJECT_IMPL(CSpellCheckerSessionListenerImpl)

CSpellCheckerSessionListenerImpl::CSpellCheckerSessionListenerImpl()
    : mOpened(FALSE)
{
    CLinkedList::New((IQueue**)&mPendingTasks);
}

ECode CSpellCheckerSessionListenerImpl::constructor(
    /* [in] */ IHandler* handler)
{
    mOpened = FALSE;
    mHandler = handler;
    return NOERROR;
}

void CSpellCheckerSessionListenerImpl::ProcessTask(
    /* [in] */ IISpellCheckerSession* session,
    /* [in] */ SpellCheckerParams* scp,
    /* [in] */ Boolean async)
{
    if (async || mAsyncHandler == NULL) {
        switch (scp->mWhat) {
            case TASK_CANCEL:
                if (DBG) {
                    Logger::W(TAG, "Cancel spell checker tasks.");
                }
                session->OnCancel();
                break;
            case TASK_GET_SUGGESTIONS_MULTIPLE:
                if (DBG) {
                    Logger::W(TAG, "Get suggestions from the spell checker.");
                }
                session->OnGetSuggestionsMultiple(scp->mTextInfos,
                        scp->mSuggestionsLimit, scp->mSequentialWords);
                break;
            case TASK_GET_SUGGESTIONS_MULTIPLE_FOR_SENTENCE:
                if (DBG) {
                    Logger::W(TAG, "Get sentence suggestions from the spell checker.");
                }
                session->OnGetSentenceSuggestionsMultiple(
                        scp->mTextInfos, scp->mSuggestionsLimit);
                break;
            case TASK_CLOSE:
                if (DBG) {
                    Logger::W(TAG, "Close spell checker tasks.");
                }
                session->OnClose();
                break;
        }
    }
    else {
        // The interface is to a local object, so need to execute it
        // asynchronously.
        scp->mSession = ISpellCheckerSession::Probe(session);
        AutoPtr<IMessage> pMsg = CMessage::Obtain(mAsyncHandler, 1, scp->Probe(EIID_IInterface));
        Boolean bSnd = FALSE;
        mAsyncHandler->SendMessage(pMsg, &bSnd);
    }

    if (scp->mWhat == TASK_CLOSE) {
        // If we are closing, we want to clean up our state now even
        // if it is pending as an async operation.
        {    AutoLock syncLock(this);
            mISpellCheckerSession = NULL;
            mHandler = NULL;
            if (mThread != NULL) {
                Boolean bQ = FALSE;
                mThread->Quit(&bQ);
            }
            mThread = NULL;
            mAsyncHandler = NULL;
        }
    }
}

ECode CSpellCheckerSessionListenerImpl::OnServiceConnected(
    /* [in] */ IISpellCheckerSession* session)
{
    {    AutoLock syncLock(this);
        mISpellCheckerSession = session;
        assert(0 && "TODO");
        // if (session->AsBinder() instanceof Binder && mThread == NULL) {
        //     // If this is a local object, we need to do our own threading
        //     // to make sure we handle it asynchronously.
        //     CHandlerThread::New("SpellCheckerSession",
        //             Process.THREAD_PRIORITY_BACKGROUND, (IHandlerThread**)&mThread);
        //     mThread->Start();
        //     mAsyncHandler = new MyHandler(mThread.getLooper());
        // }
        mOpened = TRUE;
    }
    if (DBG) {
        Logger::D(TAG, "onServiceConnected - Success");
    }
    Boolean bEmp = FALSE;
    while (!(mPendingTasks->IsEmpty(&bEmp), bEmp)) {
        AutoPtr<IInterface> p;
        mPendingTasks->Poll((IInterface**)&p);
        ProcessTask(session, (SpellCheckerParams*)(Object*)(IObject*)p.Get(), FALSE);
    }
    return NOERROR;
}

ECode CSpellCheckerSessionListenerImpl::Cancel()
{
    if (DBG) {
        Logger::W(TAG, "cancel");
    }
    AutoPtr<SpellCheckerParams> p = new SpellCheckerParams(TASK_CANCEL, NULL, 0, FALSE);
    ProcessOrEnqueueTask(p);
    return NOERROR;
}

ECode CSpellCheckerSessionListenerImpl::GetSuggestionsMultiple(
    /* [in] */ ArrayOf<ITextInfo*>* textInfos,
    /* [in] */ Int32 suggestionsLimit,
    /* [in] */ Boolean sequentialWords)
{
    if (DBG) {
        Logger::W(TAG, "getSuggestionsMultiple");
    }
    AutoPtr<SpellCheckerParams> p = new SpellCheckerParams(TASK_GET_SUGGESTIONS_MULTIPLE, textInfos,
                                            suggestionsLimit, sequentialWords);
    ProcessOrEnqueueTask(p);
    return NOERROR;
}

ECode CSpellCheckerSessionListenerImpl::GetSentenceSuggestionsMultiple(
    /* [in] */ ArrayOf<ITextInfo*>* textInfos,
    /* [in] */ Int32 suggestionsLimit)
{
    if (DBG) {
        Logger::W(TAG, "getSentenceSuggestionsMultiple");
    }
    AutoPtr<SpellCheckerParams> p = new SpellCheckerParams(TASK_GET_SUGGESTIONS_MULTIPLE_FOR_SENTENCE,
                                            textInfos, suggestionsLimit, FALSE);
    ProcessOrEnqueueTask(p);
    return NOERROR;
}

ECode CSpellCheckerSessionListenerImpl::Close()
{
    if (DBG) {
        Logger::W(TAG, "close");
    }
    AutoPtr<SpellCheckerParams> p = new SpellCheckerParams(TASK_CLOSE, NULL, 0, FALSE);
    ProcessOrEnqueueTask(p);
    return NOERROR;
}

ECode CSpellCheckerSessionListenerImpl::IsDisconnected(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mOpened && mISpellCheckerSession == NULL;
    return NOERROR;
}

void CSpellCheckerSessionListenerImpl::ProcessOrEnqueueTask(
    /* [in] */ SpellCheckerParams* scp)
{
    if (DBG) {
        Logger::D(TAG, "process or enqueue task: %p", mISpellCheckerSession.Get());
    }
    AutoPtr<IISpellCheckerSession> session;
    {    AutoLock syncLock(this);
        session = mISpellCheckerSession;
        if (session == NULL) {
            AutoPtr<SpellCheckerParams> closeTask;
            if (scp->mWhat == TASK_CANCEL) {
                Boolean bEmp = FALSE;
                while (!(mPendingTasks->IsEmpty(&bEmp), bEmp)) {
                    AutoPtr<IInterface> p;
                    mPendingTasks->Poll((IInterface**)&p);
                    AutoPtr<SpellCheckerParams> tmp = (SpellCheckerParams*)(Object*)(IObject*)p.Get();
                    if (tmp->mWhat == TASK_CLOSE) {
                        // Only one close task should be processed, while we need to remove
                        // all close tasks from the queue
                        closeTask = tmp;
                    }
                }
            }
            Boolean b = FALSE;
            mPendingTasks->Offer((IInterface*)(IObject*)scp, &b);
            if (closeTask != NULL) {
                mPendingTasks->Offer((IInterface*)(IObject*)closeTask.Get(), &b);
            }
            return;
        }
    }
    ProcessTask(session, scp, FALSE);
}

ECode CSpellCheckerSessionListenerImpl::OnGetSuggestions(
    /* [in] */ ArrayOf<ISuggestionsInfo*>* results)
{
    AutoLock lock(this);
    if (mHandler != NULL) {
        AutoPtr<IMessageHelper> hlp;
        CMessageHelper::AcquireSingleton((IMessageHelper**)&hlp);
        AutoPtr<IMessage> msg;
        assert(0 && "TODO"); // results to IInterface*
        // hlp->Obtain(mHandler, ISpellCheckerSession::MSG_ON_GET_SUGGESTION_MULTIPLE, results, (IMessage**)&msg);
        Boolean bSnd = FALSE;
        mHandler->SendMessage(msg, &bSnd);
    }
    return NOERROR;
}

ECode CSpellCheckerSessionListenerImpl::OnGetSentenceSuggestions(
    /* [in] */ ArrayOf<ISentenceSuggestionsInfo*>* results)
{
    AutoLock lock(this);
    if (mHandler != NULL) {
        AutoPtr<IMessageHelper> hlp;
        CMessageHelper::AcquireSingleton((IMessageHelper**)&hlp);
        AutoPtr<IMessage> msg;
        assert(0 && "TODO"); // results to IInterface*
        // hlp->Obtain(mHandler, ISpellCheckerSession::MSG_ON_GET_SUGGESTION_MULTIPLE_FOR_SENTENCE, results, (IMessage**)&msg);
        Boolean bSnd = FALSE;
        mHandler->SendMessage(msg, &bSnd);
    }
    return NOERROR;
}

} // namespace TextService
} // namespace View
} // namespace Droid
} // namespace Elastos
