
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
    Elastos_JavaHandlerThread_nativeInitializeThread(THIS_PROBE(IInterface), nativeCppHandlerThread, nativeEvent);
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
