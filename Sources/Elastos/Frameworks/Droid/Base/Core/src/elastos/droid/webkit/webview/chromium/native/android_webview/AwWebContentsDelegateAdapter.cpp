
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Webkit.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwWebContentsDelegateAdapter.h"
#include "elastos/droid/webkit/webview/chromium/native/base/ContentUriUtils.h"
#include "elastos/droid/webkit/webview/chromium/native/base/ThreadUtils.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/ContentVideoView.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Webkit::Webview::Chromium::Base::ContentUriUtils;
using Elastos::Droid::Webkit::Webview::Chromium::Base::ThreadUtils;
using Elastos::Droid::Webkit::Webview::Chromium::Content::Browser::ContentVideoView;
using Elastos::Droid::Webkit::Webview::Chromium::Content::Browser::ContentViewCore;
using Elastos::Droid::Webkit::EIID_IValueCallback;

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Provider::IMediaStore;
using Elastos::Droid::Provider::IMediaStoreMediaColumns;
using Elastos::Droid::Webkit::IConsoleMessage;
using Elastos::Droid::Webkit::CConsoleMessage;
using Elastos::Droid::Os::EIID_IHandler;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;


namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

const String AwWebContentsDelegateAdapter::TAG("AwWebContentsDelegateAdapter");
const Int32 AwWebContentsDelegateAdapter::MSG_CONTINUE_PENDING_RELOAD = 1;
const Int32 AwWebContentsDelegateAdapter::MSG_CANCEL_PENDING_RELOAD = 2;

//===============================================================
//       AwWebContentsDelegateAdapter::GetDisplayNameTask
//===============================================================

AwWebContentsDelegateAdapter::GetDisplayNameTask::GetDisplayNameTask(
    /* [in] */ IContentResolver* contentResolver,
    /* [in] */ Int32 processId,
    /* [in] */ Int32 renderId,
    /* [in] */ Int32 modeFlags,
    /* [in] */ ArrayOf<String>* filePaths)
    : mProcessId(processId)
    , mRenderId(renderId)
    , mModeFlags(modeFlags)
    , mFilePaths(filePaths)
    , mContentResolver(contentResolver)
{
}

//@Override
ECode AwWebContentsDelegateAdapter::GetDisplayNameTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [in] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IArrayList> displayNamesList;
    CArrayList::New((IArrayList**)&displayNamesList);
    //AutoPtr<ArrayOf<String> > displayNames = ArrayOf<String>::Alloc(mFilePaths->GetLength());
    for (Int32 i = 0; i < mFilePaths->GetLength(); ++i) {
        //(*displayNames)[i] = ResolveFileName(mFilePaths[i]);
        String fileName = ResolveFileName((*mFilePaths)[i]);
        AutoPtr<ICharSequence> ics;
        CString::New(fileName, (ICharSequence**)&ics);
        displayNamesList->Add(ics);
    }

    *result = displayNamesList.Get();
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//@Override
ECode AwWebContentsDelegateAdapter::GetDisplayNameTask::OnPostExecute(
    /* [in] */ IInterface* result)
{
    //TODO check the correction of the below code
    AutoPtr<ArrayOf<IInterface*> > displayNames;
    AutoPtr<IArrayList> al = IArrayList::Probe(result);
    al->ToArray((ArrayOf<IInterface*>**)&displayNames);
    Int32 length = displayNames->GetLength();
    AutoPtr<ArrayOf<String> > sDN = ArrayOf<String>::Alloc(length);
    for(Int32 i = 0; i < length; ++i)
    {
        String element;
        AutoPtr<ICharSequence> ics = ICharSequence::Probe((*displayNames)[i]);
        ics->ToString(&element);
        (*sDN)[i] = element;
    }
    NativeFilesSelectedInChooser(mProcessId, mRenderId, mModeFlags, mFilePaths, sDN);
    return NOERROR;
}

/**
 * @return the display name of a path if it is a content URI and is present in the database
 * or an empty string otherwise.
 */
String AwWebContentsDelegateAdapter::GetDisplayNameTask::ResolveFileName(
    /* [in] */ const String& filePath)
{
    if (mContentResolver == NULL || filePath.IsNullOrEmpty())
    {
        return String("");
    }

    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    AutoPtr<IUri> uri;
    helper->Parse(filePath, (IUri**)&helper);
    return ContentUriUtils::GetDisplayName(
            uri, mContentResolver, IMediaStoreMediaColumns::DISPLAY_NAME);
}

//===============================================================
//         AwWebContentsDelegateAdapter::InnerHandler
//===============================================================

AwWebContentsDelegateAdapter::InnerHandler::InnerHandler(
    /* [in] */ AwWebContentsDelegateAdapter* owner,
    /* [in] */ ILooper* looper,
    /* [in] */ ContentViewCore* contentViewCore)
    : Handler(looper)
    , mOwner(owner)
    , mContentViewCore(contentViewCore)
{
}

ECode AwWebContentsDelegateAdapter::InnerHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    VALIDATE_NOT_NULL(msg);
    Int32 what;
    msg->GetWhat(&what);
    switch(what) {
        case MSG_CONTINUE_PENDING_RELOAD: {
            mContentViewCore->ContinuePendingReload();
            break;
        }
        case MSG_CANCEL_PENDING_RELOAD: {
            mContentViewCore->CancelPendingReload();
            break;
        }
        default:
            //throw new IllegalStateException("WebContentsDelegateAdapter: unhandled message " + msg.what);
            Logger::E(TAG, "WebContentsDelegateAdapter: unhandled message, msg:0x%x", what);
            assert(0);
    }
    return NOERROR;
}

//===============================================================
//       AwWebContentsDelegateAdapter::InnerValueCallback
//===============================================================
CAR_INTERFACE_IMPL(AwWebContentsDelegateAdapter::InnerValueCallback, Object, IValueCallback);

AwWebContentsDelegateAdapter::InnerValueCallback::InnerValueCallback(
    /* [in] */ AwWebContentsDelegateAdapter* owner,
    /* [in] */ Int32 processId,
    /* [in] */ Int32 renderId,
    /* [in] */ Int32 modeFlags,
    /* [in] */ Boolean completed)
    : mOwner(owner)
    , mProcessId(processId)
    , mRenderId(renderId)
    , mModeFlags(modeFlags)
    , mCompleted(completed)
{
}

ECode AwWebContentsDelegateAdapter::InnerValueCallback::OnReceiveValue(
    /* [in] */ IInterface* results)
{
    if (mCompleted) {
        //throw new IllegalStateException("Duplicate showFileChooser result");
        Logger::E(TAG, "InnerValueCallback::OnReceiveValue: Duplicate showFileChooser result");
        assert(0);
    }

    mCompleted = TRUE;

    if (results == NULL) {
        NativeFilesSelectedInChooser(
                mProcessId, mRenderId, mModeFlags, NULL, NULL);
        return NOERROR;
    }
    AutoPtr<IArrayList> al = IArrayList::Probe(results);
    if (al == NULL)
    {
        Logger::E(TAG, "InnerValueCallback::OnReceiveValue: can not get the arraylist");
        NativeFilesSelectedInChooser(
                mProcessId, mRenderId, mModeFlags, NULL, NULL);
        return NOERROR;
    }

    AutoPtr<IContentResolver> contentResolver;
    mOwner->mContext->GetContentResolver((IContentResolver**)&contentResolver);

    AutoPtr<ArrayOf<IInterface*> > getFromArrayList;
    al->ToArray((ArrayOf<IInterface*>**)&getFromArrayList);
    Int32 length = getFromArrayList->GetLength();
    AutoPtr<ArrayOf<String> > filePaths = ArrayOf<String>::Alloc(length);
    for(Int32 i = 0; i < length; ++i)
    {
        String element;
        AutoPtr<ICharSequence> ics = ICharSequence::Probe((*getFromArrayList)[i]);
        ics->ToString(&element);
        (*filePaths)[i] = element;
    }

    AutoPtr<GetDisplayNameTask> task = new GetDisplayNameTask(
            contentResolver, mProcessId, mRenderId, mModeFlags, filePaths);
    task->Execute(getFromArrayList);

    return NOERROR;
}

//===============================================================
//                AwWebContentsDelegateAdapter
//===============================================================

AwWebContentsDelegateAdapter::AwWebContentsDelegateAdapter(
    /* [in] */ AwContentsClient* contentsClient,
    /* [in] */ IView* containerView,
    /* [in] */ IContext* context)
    : mContentsClient(contentsClient)
    , mContext(context)
{
    SetContainerView(containerView);
}

void AwWebContentsDelegateAdapter::SetContainerView(
    /* [in] */ IView* containerView)
{
    mContainerView = containerView;
}

//@Override
ECode AwWebContentsDelegateAdapter::OnLoadProgressChanged(
    /* [in] */ Int32 progress)
{
    mContentsClient->OnProgressChanged(progress);
    return NOERROR;
}

//@Override
ECode AwWebContentsDelegateAdapter::HandleKeyboardEvent(
    /* [in] */ IKeyEvent* event)
{
    Int32 action;
    event->GetAction(&action);
    if (action == IKeyEvent::ACTION_DOWN) {
        Int32 direction;
        Int32 keyCode;
        event->GetKeyCode(&keyCode);
        switch (keyCode) {
            case IKeyEvent::KEYCODE_DPAD_DOWN:
                direction = IView::FOCUS_DOWN;
                break;
            case IKeyEvent::KEYCODE_DPAD_UP:
                direction = IView::FOCUS_UP;
                break;
            case IKeyEvent::KEYCODE_DPAD_LEFT:
                direction = IView::FOCUS_LEFT;
                break;
            case IKeyEvent::KEYCODE_DPAD_RIGHT:
                direction = IView::FOCUS_RIGHT;
                break;
            default:
                direction = 0;
                break;
        }
        if (direction != 0 && TryToMoveFocus(direction)) return NOERROR;
    }
    mContentsClient->OnUnhandledKeyEvent(event);
    return NOERROR;
}

//@Override
Boolean AwWebContentsDelegateAdapter::TakeFocus(
    /* [in] */ Boolean reverse)
{
    Int32 layoutDirection;
    mContainerView->GetLayoutDirection(&layoutDirection);
    Int32 direction =
        (reverse == (layoutDirection == IView::LAYOUT_DIRECTION_RTL)) ?
        IView::FOCUS_RIGHT : IView::FOCUS_LEFT;
    if (TryToMoveFocus(direction)) return TRUE;
    direction = reverse ? IView::FOCUS_UP : IView::FOCUS_DOWN;
    return TryToMoveFocus(direction);
}

Boolean AwWebContentsDelegateAdapter::TryToMoveFocus(
    /* [in] */ Int32 direction)
{
    AutoPtr<IView> focus;
    mContainerView->FocusSearch(direction, (IView**)&focus);
    Boolean bFocus = FALSE;
    focus->RequestFocus(&bFocus);
    return focus != NULL && focus != mContainerView && bFocus;
}

//@Override
Boolean AwWebContentsDelegateAdapter::AddMessageToConsole(
    /* [in] */ Int32 level,
    /* [in] */ const String& message,
    /* [in] */ Int32 lineNumber,
    /* [in] */ const String& sourceId)
{
    Elastos::Droid::Webkit::MessageLevel messageLevel = Elastos::Droid::Webkit::DEBUG;
    switch(level) {
        case LOG_LEVEL_TIP:
            messageLevel = Elastos::Droid::Webkit::TIP;
            break;
        case LOG_LEVEL_LOG:
            messageLevel = Elastos::Droid::Webkit::LOG;
            break;
        case LOG_LEVEL_WARNING:
            messageLevel = Elastos::Droid::Webkit::WARNING;
            break;
        case LOG_LEVEL_ERROR:
            messageLevel = Elastos::Droid::Webkit::ERROR;
            break;
        default:
            Logger::W(TAG, "Unknown message level, defaulting to DEBUG");
            break;
    }

    AutoPtr<IConsoleMessage> cm;
    CConsoleMessage::New(message, sourceId, lineNumber, messageLevel, (IConsoleMessage**)&cm);
    return mContentsClient->OnConsoleMessage(cm);
}

//@Override
ECode AwWebContentsDelegateAdapter::OnUpdateUrl(
    /* [in] */ const String& url)
{
    // TODO: implement
    Logger::W(TAG, "not implement");
    return NOERROR;
}

//@Override
ECode AwWebContentsDelegateAdapter::OpenNewTab(
    /* [in] */ const String& url,
    /* [in] */ const String& extraHeaders,
    /* [in] */ ArrayOf<Byte>* postData,
    /* [in] */ Int32 disposition,
    /* [in] */ Boolean isRendererInitiated)
{
    // This is only called in chrome layers.
    Logger::W(TAG, "This is only called in chrome layers");
    assert(FALSE);
    return NOERROR;
}

//@Override
ECode AwWebContentsDelegateAdapter::CloseContents()
{
    mContentsClient->OnCloseWindow();
    return NOERROR;
}

//@Override
ECode AwWebContentsDelegateAdapter::ShowRepostFormWarningDialog(
    /* [in] */ ContentViewCore* contentViewCore)
{
    // TODO(mkosiba) We should be using something akin to the JsResultReceiver as the
    // callback parameter (instead of ContentViewCore) and implement a way of converting
    // that to a pair of messages.
    //const Int32 MSG_CONTINUE_PENDING_RELOAD = 1;
    //const Int32 MSG_CANCEL_PENDING_RELOAD = 2;

    // TODO(sgurun) Remember the URL to cancel the reload behavior
    // if it is different than the most recent NavigationController entry.
    AutoPtr<ILooper> looper = ThreadUtils::GetUiThreadLooper();
    AutoPtr<IHandler> handler = new InnerHandler(this, looper, contentViewCore);
    AutoPtr<IMessage> resend;
    handler->ObtainMessage(MSG_CONTINUE_PENDING_RELOAD, (IMessage**)&resend);
    AutoPtr<IMessage> dontResend;
    handler->ObtainMessage(MSG_CANCEL_PENDING_RELOAD, (IMessage**)&dontResend);
    mContentsClient->OnFormResubmission(dontResend, resend);
    return NOERROR;
}

//@Override
void AwWebContentsDelegateAdapter::RunFileChooser(
    /* [in] */ Int32 processId,
    /* [in] */ Int32 renderId,
    /* [in] */ Int32 modeFlags,
    /* [in] */ const String& acceptTypes,
    /* [in] */ const String& title,
    /* [in] */ const String& defaultFilename,
    /* [in] */ Boolean capture)
{
    AutoPtr<AwContentsClient::FileChooserParams> params = new AwContentsClient::FileChooserParams();
    params->mode = modeFlags;
    params->acceptTypes = acceptTypes;
    params->title = title;
    params->defaultFilename = defaultFilename;
    params->capture = capture;

    AutoPtr<IValueCallback> callback = new InnerValueCallback(this, processId, renderId, modeFlags, FALSE);
    mContentsClient->ShowFileChooser(callback, params);
}

//@Override
Boolean AwWebContentsDelegateAdapter::AddNewContents(
    /* [in] */ Boolean isDialog,
    /* [in] */ Boolean isUserGesture)
{
    return mContentsClient->OnCreateWindow(isDialog, isUserGesture);
}

//@Override
ECode AwWebContentsDelegateAdapter::ActivateContents()
{
    mContentsClient->OnRequestFocus();
    return NOERROR;
}

//@Override
ECode AwWebContentsDelegateAdapter::ToggleFullscreenModeForTab(
    /* [in] */ Boolean enterFullscreen)
{
    if (!enterFullscreen) {
        AutoPtr<ContentVideoView> videoView = ContentVideoView::GetContentVideoView();
        if (videoView != NULL) videoView->ExitFullscreen(FALSE);
    }
    return NOERROR;
}

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
