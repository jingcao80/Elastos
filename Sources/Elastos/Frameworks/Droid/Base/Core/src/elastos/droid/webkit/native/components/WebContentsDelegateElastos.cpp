
#include "elastos/droid/webkit/native/components/WebContentsDelegateElastos.h"
#include "elastos/droid/webkit/native/components/api/WebContentsDelegateElastos_dec.h"
#include "elastos/droid/webkit/native/content/browser/ContentViewCore.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Components {

//=====================================================================
//                      WebContentsDelegateElastos
//=====================================================================
const Int32 WebContentsDelegateElastos::LOG_LEVEL_TIP;
const Int32 WebContentsDelegateElastos::LOG_LEVEL_LOG;
const Int32 WebContentsDelegateElastos::LOG_LEVEL_WARNING;
const Int32 WebContentsDelegateElastos::LOG_LEVEL_ERROR;
const Int32 WebContentsDelegateElastos::INVALIDATE_TYPE_URL;
const Int32 WebContentsDelegateElastos::INVALIDATE_TYPE_TAB;
const Int32 WebContentsDelegateElastos::INVALIDATE_TYPE_LOAD;
const Int32 WebContentsDelegateElastos::INVALIDATE_TYPE_PAGE_ACTIONS;
const Int32 WebContentsDelegateElastos::INVALIDATE_TYPE_TITLE;

Int32 WebContentsDelegateElastos::GetMostRecentProgress()
{
    // ==================before translated======================
    // return mMostRecentProgress;

    return mMostRecentProgress;
}

ECode WebContentsDelegateElastos::OpenNewTab(
    /* [in] */ const String& url,
    /* [in] */ const String& extraHeaders,
    /* [in] */ ArrayOf<Byte>* postData,
    /* [in] */ Int32 disposition,
    /* [in] */ Boolean isRendererInitiated)
{
    VALIDATE_NOT_NULL(postData);
    return NOERROR;
}

ECode WebContentsDelegateElastos::ActivateContents()
{
    return NOERROR;
}

ECode WebContentsDelegateElastos::CloseContents()
{
    return NOERROR;
}

ECode WebContentsDelegateElastos::OnLoadStarted()
{
    return NOERROR;
}

ECode WebContentsDelegateElastos::OnLoadStopped()
{
    return NOERROR;
}

ECode WebContentsDelegateElastos::NavigationStateChanged(
    /* [in] */ Int32 flags)
{
    return NOERROR;
}

ECode WebContentsDelegateElastos::VisibleSSLStateChanged()
{
    return NOERROR;
}

ECode WebContentsDelegateElastos::OnLoadProgressChanged(
    /* [in] */ Int32 progress)
{
    return NOERROR;
}

ECode WebContentsDelegateElastos::RendererUnresponsive()
{
    return NOERROR;
}

ECode WebContentsDelegateElastos::RendererResponsive()
{
    return NOERROR;
}

ECode WebContentsDelegateElastos::OnUpdateUrl(
    /* [in] */ const String& url)
{
    return NOERROR;
}

Boolean WebContentsDelegateElastos::TakeFocus(
    /* [in] */ Boolean reverse)
{
    // ==================before translated======================
    // return false;

    return FALSE;
}

ECode WebContentsDelegateElastos::HandleKeyboardEvent(
    /* [in] */ IKeyEvent* event)
{
    VALIDATE_NOT_NULL(event);
    // ==================before translated======================
    // // TODO(bulach): we probably want to re-inject the KeyEvent back into
    // // the system. Investigate if this is at all possible.

    return NOERROR;
}

Boolean WebContentsDelegateElastos::AddMessageToConsole(
    /* [in] */ Int32 level,
    /* [in] */ const String& message,
    /* [in] */ Int32 lineNumber,
    /* [in] */ const String& sourceId)
{
    // ==================before translated======================
    // return false;

    return FALSE;
}

ECode WebContentsDelegateElastos::ShowRepostFormWarningDialog(
    /* [in] */ ContentViewCore* contentViewCore)
{
    VALIDATE_NOT_NULL(contentViewCore);
    return NOERROR;
}

ECode WebContentsDelegateElastos::ToggleFullscreenModeForTab(
    /* [in] */ Boolean enterFullscreen)
{
    return NOERROR;
}

Boolean WebContentsDelegateElastos::IsFullscreenForTabOrPending()
{
    // ==================before translated======================
    // return false;

    return FALSE;
}

const ECode WebContentsDelegateElastos::NotifyLoadProgressChanged(
    /* [in] */ Double progress)
{
    // ==================before translated======================
    // mMostRecentProgress = (int) (100.0 * progress);
    // onLoadProgressChanged(mMostRecentProgress);

    mMostRecentProgress = (Int32)(100.0 * progress);
    OnLoadProgressChanged(mMostRecentProgress);
    return NOERROR;
}

void WebContentsDelegateElastos::OpenNewTab(
    /* [in] */ IInterface* obj,
    /* [in] */ const String& url,
    /* [in] */ const String& extraHeaders,
    /* [in] */ ArrayOf<Byte>* postData,
    /* [in] */ Int32 disposition,
    /* [in] */ Boolean isRendererInitiated)
{
    WebContentsDelegateElastos* mObj = (WebContentsDelegateElastos*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("WebContentsDelegateElastos", "WebContentsDelegateElastos::OpenNewTab, mObj is NULL");
        return;
    }
    mObj->OpenNewTab(url, extraHeaders, postData, disposition, isRendererInitiated);
}

void WebContentsDelegateElastos::ActivateContents(
    /* [in] */ IInterface* obj)
{
    WebContentsDelegateElastos* mObj = (WebContentsDelegateElastos*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("WebContentsDelegateElastos", "WebContentsDelegateElastos::ActivateContents, mObj is NULL");
        return;
    }
    mObj->ActivateContents();
}

void WebContentsDelegateElastos::CloseContents(
    /* [in] */ IInterface* obj)
{
    WebContentsDelegateElastos* mObj = (WebContentsDelegateElastos*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("WebContentsDelegateElastos", "WebContentsDelegateElastos::CloseContents, mObj is NULL");
        return;
    }
    mObj->CloseContents();
}

void WebContentsDelegateElastos::OnLoadStarted(
    /* [in] */ IInterface* obj)
{
    WebContentsDelegateElastos* mObj = (WebContentsDelegateElastos*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("WebContentsDelegateElastos", "WebContentsDelegateElastos::OnLoadStarted, mObj is NULL");
        return;
    }
    mObj->OnLoadStarted();
}

void WebContentsDelegateElastos::OnLoadStopped(
    /* [in] */ IInterface* obj)
{
    WebContentsDelegateElastos* mObj = (WebContentsDelegateElastos*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("WebContentsDelegateElastos", "WebContentsDelegateElastos::OnLoadStopped, mObj is NULL");
        return;
    }
    mObj->OnLoadStopped();
}

void WebContentsDelegateElastos::NavigationStateChanged(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 flags)
{
    WebContentsDelegateElastos* mObj = (WebContentsDelegateElastos*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("WebContentsDelegateElastos", "WebContentsDelegateElastos::NavigationStateChanged, mObj is NULL");
        return;
    }
    mObj->NavigationStateChanged(flags);
}

void WebContentsDelegateElastos::VisibleSSLStateChanged(
    /* [in] */ IInterface* obj)
{
    WebContentsDelegateElastos* mObj = (WebContentsDelegateElastos*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("WebContentsDelegateElastos", "WebContentsDelegateElastos::VisibleSSLStateChanged, mObj is NULL");
        return;
    }
    mObj->VisibleSSLStateChanged();
}

void WebContentsDelegateElastos::NotifyLoadProgressChanged(
    /* [in] */ IInterface* obj,
    /* [in] */ Double progress)
{
    WebContentsDelegateElastos* mObj = (WebContentsDelegateElastos*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("WebContentsDelegateElastos", "WebContentsDelegateElastos::NotifyLoadProgressChanged, mObj is NULL");
        return;
    }
    mObj->NotifyLoadProgressChanged(progress);
}

void WebContentsDelegateElastos::RendererUnresponsive(
    /* [in] */ IInterface* obj)
{
    WebContentsDelegateElastos* mObj = (WebContentsDelegateElastos*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("WebContentsDelegateElastos", "WebContentsDelegateElastos::RendererUnresponsive, mObj is NULL");
        return;
    }
    mObj->RendererUnresponsive();
}

void WebContentsDelegateElastos::RendererResponsive(
    /* [in] */ IInterface* obj)
{
    WebContentsDelegateElastos* mObj = (WebContentsDelegateElastos*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("WebContentsDelegateElastos", "WebContentsDelegateElastos::RendererResponsive, mObj is NULL");
        return;
    }
    mObj->RendererResponsive();
}

void WebContentsDelegateElastos::OnUpdateUrl(
    /* [in] */ IInterface* obj,
    /* [in] */ const String& url)
{
    WebContentsDelegateElastos* mObj = (WebContentsDelegateElastos*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("WebContentsDelegateElastos", "WebContentsDelegateElastos::OnUpdateUrl, mObj is NULL");
        return;
    }
    mObj->OnUpdateUrl(url);
}

Boolean WebContentsDelegateElastos::TakeFocus(
    /* [in] */ IInterface* obj,
    /* [in] */ Boolean reverse)
{
    WebContentsDelegateElastos* mObj = (WebContentsDelegateElastos*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("WebContentsDelegateElastos", "WebContentsDelegateElastos::TakeFocus, mObj is NULL");
        return FALSE;
    }
    return mObj->TakeFocus(reverse);
}

void WebContentsDelegateElastos::HandleKeyboardEvent(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* event)
{
    WebContentsDelegateElastos* mObj = (WebContentsDelegateElastos*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("WebContentsDelegateElastos", "WebContentsDelegateElastos::HandleKeyboardEvent, mObj is NULL");
        return;
    }
    IKeyEvent* ke = IKeyEvent::Probe(event);
    mObj->HandleKeyboardEvent(ke);
}

Boolean WebContentsDelegateElastos::AddMessageToConsole(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 level,
    /* [in] */ const String& message,
    /* [in] */ Int32 lineNumber,
    /* [in] */ const String& sourceId)
{
    WebContentsDelegateElastos* mObj = (WebContentsDelegateElastos*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("WebContentsDelegateElastos", "WebContentsDelegateElastos::AddMessageToConsole, mObj is NULL");
        return FALSE;
    }
    return mObj->AddMessageToConsole(level, message, lineNumber, sourceId);
}

void WebContentsDelegateElastos::ShowRepostFormWarningDialog(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* contentViewCore)
{
    WebContentsDelegateElastos* mObj = (WebContentsDelegateElastos*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("WebContentsDelegateElastos", "WebContentsDelegateElastos::ShowRepostFormWarningDialog, mObj is NULL");
        return;
    }
    ContentViewCore* cvc = (ContentViewCore*)(IObject::Probe(contentViewCore));
    mObj->ShowRepostFormWarningDialog(cvc);
}

void WebContentsDelegateElastos::ToggleFullscreenModeForTab(
    /* [in] */ IInterface* obj,
    /* [in] */ Boolean enterFullscreen)
{
    WebContentsDelegateElastos* mObj = (WebContentsDelegateElastos*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("WebContentsDelegateElastos", "WebContentsDelegateElastos::ToggleFullscreenModeForTab, mObj is NULL");
        return;
    }
    mObj->ToggleFullscreenModeForTab(enterFullscreen);
}

Boolean WebContentsDelegateElastos::IsFullscreenForTabOrPending(
    /* [in] */ IInterface* obj)
{
    WebContentsDelegateElastos* mObj = (WebContentsDelegateElastos*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("WebContentsDelegateElastos", "WebContentsDelegateElastos::IsFullscreenForTabOrPending, mObj is NULL");
        return FALSE;
    }
    return mObj->IsFullscreenForTabOrPending();
}

} // namespace Components
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

