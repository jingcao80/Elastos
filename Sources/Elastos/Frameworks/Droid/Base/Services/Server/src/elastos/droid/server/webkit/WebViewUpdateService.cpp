
#include "elastos/droid/server/webkit/WebViewUpdateService.h"
#include "elastos/droid/server/webkit/CWebViewUpdateBinderService.h"
#include <elastos/droid/os/Binder.h>
#include <elastos/droid/os/Process.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/AutoLock.h>
#include <Elastos.Droid.Content.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Webkit::EIID_IIWebViewUpdateService;
using Elastos::Droid::Webkit::IWebViewFactory;
using Elastos::Droid::Webkit::CWebViewFactory;
using Elastos::Core::ISynchronize;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Webkit {

const String WebViewUpdateService::TAG("WebViewUpdateService");
const Int32 WebViewUpdateService::WAIT_TIMEOUT_MS = 5000; // Same as KEY_DISPATCHING_TIMEOUT.

//=======================================================================
// WebViewUpdateService::BinderService
//=======================================================================

CAR_INTERFACE_IMPL_2(WebViewUpdateService::BinderService, Object, IIWebViewUpdateService, IBinder)

ECode WebViewUpdateService::BinderService::constructor(
    /* [in] */ ISystemService* webViewUpdateService)
{
    mHost = (WebViewUpdateService*)webViewUpdateService;
    return NOERROR;
}

ECode WebViewUpdateService::BinderService::NotifyRelroCreationCompleted(
    /* [in] */ Boolean is64Bit,
    /* [in] */ Boolean success)
{
    // Verify that the caller is either the shared relro process (nominal case) or the
    // system server (only in the case the relro process crashes and we get here via the
    // crashHandler).
    if (Binder::GetCallingUid() != IProcess::SHARED_RELRO_UID &&
            Binder::GetCallingUid() != IProcess::SYSTEM_UID) {
        return NOERROR;
    }

    {    AutoLock syncLock(mHost);
        if (is64Bit) {
            mHost->mRelroReady64Bit = TRUE;
        } else {
            mHost->mRelroReady32Bit = TRUE;
        }
        ((ISynchronize*)mHost)->NotifyAll();
    }
    return NOERROR;
}

ECode WebViewUpdateService::BinderService::WaitForRelroCreationCompleted(
    /* [in] */ Boolean is64Bit)
{
    // The WebViewUpdateService depends on the prepareWebViewInSystemServer call, which
    // happens later (during the PHASE_ACTIVITY_MANAGER_READY) in SystemServer.java. If
    // another service there tries to bring up a WebView in the between, the wait below
    // would deadlock without the check below.
    if (Binder::GetCallingPid() == Process::MyPid()) {
        Slogger::E(TAG, "Cannot create a WebView from the SystemServer");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int64 nt;
    system->GetNanoTime(&nt);
    Int64 NS_PER_MS = 1000000;
    Int64 timeoutTimeMs = nt / NS_PER_MS + WAIT_TIMEOUT_MS;
    Boolean relroReady = (is64Bit ? mHost->mRelroReady64Bit : mHost->mRelroReady32Bit);
    Slogger::E(TAG, "TODO:  WebViewFactory::PrepareWebViewInSystemServer is not implemented!");
    // {    AutoLock syncLock(this);
    //     while (!relroReady) {
    //         system->GetNanoTime(&nt);
    //         Int64 timeNowMs = nt / NS_PER_MS;
    //         if (timeNowMs >= timeoutTimeMs) break;
    //         // try {
    //         Wait(timeoutTimeMs - timeNowMs);
    //         // } catch (InterruptedException e) {}
    //         relroReady = (is64Bit ? mHost->mRelroReady64Bit : mHost->mRelroReady32Bit);
    //     }
    // }
    if (!relroReady) Slogger::W(TAG, "creating relro file timed out");
    return NOERROR;
}

//=======================================================================
// WebViewUpdateService::WebViewUpdatedReceiver
//=======================================================================
WebViewUpdateService::WebViewUpdatedReceiver::WebViewUpdatedReceiver(
    /* [in] */ WebViewUpdateService* host)
    : mHost(host)
{}

ECode WebViewUpdateService::WebViewUpdatedReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    AutoPtr<IWebViewFactory> fact;
    CWebViewFactory::AcquireSingleton((IWebViewFactory**)&fact);
    String pkgName;
    fact->GetWebViewPackageName(&pkgName);
    String webviewPackage("package:");
    webviewPackage += pkgName;

    String dataStr;
    intent->GetDataString(&dataStr);
    if (webviewPackage.Equals(dataStr)) {
        mHost->OnWebViewUpdateInstalled();
    }
    return NOERROR;
}

//=======================================================================
// WebViewUpdateService
//=======================================================================

WebViewUpdateService::WebViewUpdateService()
    : mRelroReady32Bit(FALSE)
    , mRelroReady64Bit(FALSE)
{}

ECode WebViewUpdateService::constructor(
    /* [in] */ IContext* context)
{
    return SystemService::constructor(context);
}

ECode WebViewUpdateService::OnStart()
{
    mWebViewUpdatedReceiver = new WebViewUpdatedReceiver(this);
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_PACKAGE_REPLACED);
    filter->AddDataScheme(String("package"));
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IIntent> intent;
    context->RegisterReceiver(mWebViewUpdatedReceiver, filter, (IIntent**)&intent);

    AutoPtr<IBinder> service;
    CWebViewUpdateBinderService::New(this, (IBinder**)&service);
    PublishBinderService(String("webviewupdate"), service);
    return NOERROR;
}

void WebViewUpdateService::OnWebViewUpdateInstalled()
{
    Slogger::D(TAG, "WebView Package updated!");

    {    AutoLock syncLock(this);
        mRelroReady32Bit = FALSE;
        mRelroReady64Bit = FALSE;
    }

    AutoPtr<IWebViewFactory> fact;
    CWebViewFactory::AcquireSingleton((IWebViewFactory**)&fact);
    fact->OnWebViewUpdateInstalled();
}


}// Webkit
}// Server
}// Droid
}// Elastos
