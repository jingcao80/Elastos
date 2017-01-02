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

#ifndef __ELASTOS_DROID_INTERNAL_OS_CHANDLERCALLER_H__
#define __ELASTOS_DROID_INTERNAL_OS_CHANDLERCALLER_H__

#include "_Elastos_Droid_Internal_Os_CHandlerCaller.h"
#include "elastos/droid/os/Handler.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::Handler;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

CarClass(CHandlerCaller)
    , public Object
    , public IHandlerCaller
{
private:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CHandlerCaller::MyHandler")

        MyHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ Boolean async,
            /* [in] */ IWeakReference* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        AutoPtr<IWeakReference> mWeakHost;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    // if callback has this's reference,isStrong must be FLASE
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ ILooper* looper,
        /* [in] */ IHandlerCallerCallback* callback,
        /* [in] */ Boolean asyncHandler,
        /* [in] */ Boolean isStrong);

    CARAPI ExecuteOrSendMessage(
        /* [in] */ IMessage* msg);

    CARAPI SendMessageDelayed(
        /* [in] */ IMessage* msg,
        /* [in] */ Int64 delayMillis);

    CARAPI HasMessages(
        /* [in] */ Int32 what,
        /* [out] */ Boolean* result);

    CARAPI RemoveMessages(
        /* [in] */ Int32 what);

    CARAPI RemoveMessages(
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    CARAPI SendMessage(
        /* [in] */ IMessage* msg);

    CARAPI SendMessageAndWait(
        /* [in] */ IMessage* msg,
        /* [out] */ ISomeArgs** rargs);

    CARAPI ObtainMessage(
        /* [in] */ Int32 what,
        /* [out] */ IMessage** msg);

    CARAPI ObtainMessageBO(
        /* [in] */ Int32 what,
        /* [in] */ Boolean arg1,
        /* [in] */ IInterface* arg2,
        /* [out] */ IMessage** msg);

    CARAPI ObtainMessageBOO(
        /* [in] */ Int32 what,
        /* [in] */ Boolean arg1,
        /* [in] */ IInterface* arg2,
        /* [in] */ IInterface* arg3,
        /* [out] */ IMessage** msg);

    CARAPI ObtainMessageO(
        /* [in] */ Int32 what,
        /* [in] */ IInterface* arg1,
        /* [out] */ IMessage** msg);

    CARAPI ObtainMessageI(
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1,
        /* [out] */ IMessage** msg);

    CARAPI ObtainMessageII(
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2,
        /* [out] */ IMessage** msg);

    CARAPI ObtainMessageIO(
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1,
        /* [in] */ IInterface* arg2,
        /* [out] */ IMessage** msg);

    CARAPI ObtainMessageIIO(
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2,
        /* [in] */ IInterface* arg3,
        /* [out] */ IMessage** msg);

    CARAPI ObtainMessageIIOO(
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2,
        /* [in] */ IInterface* arg3,
        /* [in] */ IInterface* arg4,
        /* [out] */ IMessage** msg);

    CARAPI ObtainMessageIOO(
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1,
        /* [in] */ IInterface* arg2,
        /* [in] */ IInterface* arg3,
        /* [out] */ IMessage** msg);

    CARAPI ObtainMessageIOOO(
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1,
        /* [in] */ IInterface* arg2,
        /* [in] */ IInterface* arg3,
        /* [in] */ IInterface* arg4,
        /* [out] */ IMessage** msg);

    CARAPI ObtainMessageOO(
        /* [in] */ Int32 what,
        /* [in] */ IInterface* arg1,
        /* [in] */ IInterface* arg2,
        /* [out] */ IMessage** msg);

    CARAPI ObtainMessageOOO(
        /* [in] */ Int32 what,
        /* [in] */ IInterface* arg1,
        /* [in] */ IInterface* arg2,
        /* [in] */ IInterface* arg3,
        /* [out] */ IMessage** msg);

    CARAPI ObtainMessageOOOO(
        /* [in] */ Int32 what,
        /* [in] */ IInterface* arg1,
        /* [in] */ IInterface* arg2,
        /* [in] */ IInterface* arg3,
        /* [in] */ IInterface* arg4,
        /* [out] */ IMessage** msg);

    CARAPI ObtainMessageOOOOO(
        /* [in] */ Int32 what,
        /* [in] */ IInterface* arg1,
        /* [in] */ IInterface* arg2,
        /* [in] */ IInterface* arg3,
        /* [in] */ IInterface* arg4,
        /* [in] */ IInterface* arg5,
        /* [out] */ IMessage** msg);

    CARAPI ObtainMessageIIII(
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2,
        /* [in] */ Int32 arg3,
        /* [in] */ Int32 arg4,
        /* [out] */ IMessage** msg);

    CARAPI ObtainMessageIIIIII(
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2,
        /* [in] */ Int32 arg3,
        /* [in] */ Int32 arg4,
        /* [in] */ Int32 arg5,
        /* [in] */ Int32 arg6,
        /* [out] */ IMessage** msg);

    CARAPI ObtainMessageIIIIO(
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2,
        /* [in] */ Int32 arg3,
        /* [in] */ Int32 arg4,
        /* [in] */ IInterface* arg5,
        /* [out] */ IMessage** msg);

private:
    AutoPtr<IHandlerCallerCallback> GetCallback();

private:
    AutoPtr<ILooper> mMainLooper;
    AutoPtr<MyHandler> mH;

    AutoPtr<IHandlerCallerCallback> mCallback;
    AutoPtr<IWeakReference> mWeakCallback;
};

} // namespace Os
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_OS_CHANDLERCALLER_H__
