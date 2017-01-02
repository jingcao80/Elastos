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

#include <Elastos.Droid.Os.h>
#include "elastos/droid/webkit/webview/chromium/native/base/CppHandlerThread.h"
#include "elastos/droid/webkit/webview/chromium/native/base/api/CppHandlerThread_dec.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::IThread;
using Elastos::Core::EIID_IThread;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Base {

//===============================================================
//               CppHandlerThread::InnerRunnable
//===============================================================
CAR_INTERFACE_IMPL(CppHandlerThread::InnerRunnable, Object, IRunnable);

CppHandlerThread::InnerRunnable::InnerRunnable(
    /* [in] */ CppHandlerThread* owner,
    /* [in] */ const Handle64 nativeThread,
    /* [in] */ const Handle64 nativeEvent)
    : mOwner(owner)
    , mNativeThread(nativeThread)
    , mNativeEvent(nativeEvent)
{
}

//@Override
ECode CppHandlerThread::InnerRunnable::Run()
{
    mOwner->NativeInitializeThread(mNativeThread, mNativeEvent);
    return NOERROR;
}

//===============================================================
//                   CppHandlerThread
//===============================================================

CppHandlerThread::CppHandlerThread(
    /* [in] */ const String& name)
{
    CHandlerThread::New(name, (IHandlerThread**)&mThread);
}

//@CalledByNative
AutoPtr<IInterface> CppHandlerThread::Create(
    /* [in] */ const String& name)
{
    AutoPtr<CppHandlerThread> thread = new CppHandlerThread(name);
    return TO_IINTERFACE(thread);
}

//@CalledByNative
void CppHandlerThread::Start(
    /* [in] */ const Handle64 nativeThread,
    /* [in] */ const Handle64 nativeEvent)
{
    AutoPtr<IThread> thread = IThread::Probe(mThread);
    thread->Start();
    AutoPtr<ILooper> looper;
    mThread->GetLooper((ILooper**)&looper);
    AutoPtr<IHandler> handler;
    CHandler::New(looper, (IHandler**)&handler);
    AutoPtr<IRunnable> runnable = new InnerRunnable(this, nativeThread, nativeEvent);
    Boolean result = FALSE;
    handler->Post(runnable, &result);
}

void CppHandlerThread::NativeInitializeThread(
    /* [in] */ Handle64 nativeCppHandlerThread,
    /* [in] */ Handle64 nativeEvent)
{
    Elastos_JavaHandlerThread_nativeInitializeThread(TO_IINTERFACE(this), nativeCppHandlerThread, nativeEvent);
}

void CppHandlerThread::Start(
    /* [in] */ IInterface* obj,
    /* [in] */ Handle64 nativeThread,
    /* [in] */ Handle64 nativeEvent)
{
    AutoPtr<CppHandlerThread> mObj = (CppHandlerThread*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("CppHandlerThread", "CppHandlerThread::Start, mObj is NULL");
        return;
    }
    mObj->Start(nativeThread, nativeEvent);
}

} // namespace Base
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
