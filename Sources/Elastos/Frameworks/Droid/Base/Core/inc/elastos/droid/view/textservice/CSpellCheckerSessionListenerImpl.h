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

#ifndef __ELASTOS_DROID_VIEW_TEXTSERVICE_CSPELLCHECKERSESSIONLISTENERIMPL_H__
#define __ELASTOS_DROID_VIEW_TEXTSERVICE_CSPELLCHECKERSESSIONLISTENERIMPL_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "_Elastos_Droid_View_TextService_CSpellCheckerSessionListenerImpl.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Internal::TextService::IISpellCheckerSession;
using Elastos::Droid::Internal::TextService::IISpellCheckerSessionListener;

using Elastos::Core::Object;
using Elastos::Utility::IQueue;

namespace Elastos {
namespace Droid {
namespace View {
namespace TextService {

CarClass(CSpellCheckerSessionListenerImpl)
    , public Object
    , public ISpellCheckerSessionListenerImpl
    , public IISpellCheckerSessionListener
{
private:
    class SpellCheckerParams
        : public Object
    {
    public:
        SpellCheckerParams(
            /* [in] */ Int32 what,
            /* [in] */ ArrayOf<ITextInfo*>* textInfos,
            /* [in] */ Int32 suggestionsLimit,
            /* [in] */ Boolean sequentialWords)
            : mWhat(what)
            , mTextInfos(textInfos)
            , mSuggestionsLimit(suggestionsLimit)
            , mSequentialWords(sequentialWords)
        {
        }

    public:
        Int32 mWhat;
        AutoPtr<ArrayOf<ITextInfo*> > mTextInfos;
        Int32 mSuggestionsLimit;
        Boolean mSequentialWords;
        AutoPtr<ISpellCheckerSession> mSession;
    };

    class MyHandler
        : public Handler
    {
    public:
        MyHandler(
            /* [in] */ ILooper* lp,
            /* [in] */ CSpellCheckerSessionListenerImpl* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    public:
        CSpellCheckerSessionListenerImpl* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CSpellCheckerSessionListenerImpl();

    CARAPI constructor(
        /* [in] */ IHandler* handler);

    CARAPI OnServiceConnected(
        /* [in] */ IISpellCheckerSession* session);

    CARAPI Cancel();

    CARAPI GetSuggestionsMultiple(
        /* [in] */ ArrayOf<ITextInfo*>* textInfos,
        /* [in] */ Int32 suggestionsLimit,
        /* [in] */ Boolean sequentialWords);

    CARAPI GetSentenceSuggestionsMultiple(
        /* [in] */ ArrayOf<ITextInfo*>* textInfos,
        /* [in] */ Int32 suggestionsLimit);

    CARAPI Close();

    CARAPI IsDisconnected(
        /* [out] */ Boolean* result);

    CARAPI OnGetSuggestions(
        /* [in] */ ArrayOf<ISuggestionsInfo*>* results);

    CARAPI OnGetSentenceSuggestions(
        /* [in] */ ArrayOf<ISentenceSuggestionsInfo*>* results);

private:
    CARAPI_(void) ProcessTask(
        /* [in] */ IISpellCheckerSession* session,
        /* [in] */ SpellCheckerParams* scp,
        /* [in] */ Boolean async);

    CARAPI_(void) ProcessOrEnqueueTask(
        /* [in] */ SpellCheckerParams* scp);

private:
    static const String TAG;
    static const Boolean DBG;

    static const Int32 TASK_CANCEL;
    static const Int32 TASK_GET_SUGGESTIONS_MULTIPLE;
    static const Int32 TASK_CLOSE;
    static const Int32 TASK_GET_SUGGESTIONS_MULTIPLE_FOR_SENTENCE;
    AutoPtr<IQueue> mPendingTasks;
    AutoPtr<IHandler> mHandler;

    Boolean mOpened;
    AutoPtr<IISpellCheckerSession> mISpellCheckerSession;
    AutoPtr<IHandlerThread> mThread;
    AutoPtr<IHandler> mAsyncHandler;
};

}   //namespace TextService
}   //namespace View
}   //namespace Droid
}   //namespace Elastos

#endif //__ELASTOS_DROID_VIEW_TEXTSERVICE_CSPELLCHECKERSESSIONLISTENERIMPL_H__
