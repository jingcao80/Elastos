
namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Chromium {
namespace Chromium {

//=====================================================================
//       WebViewChromium::WebViewChromiumRunQueue::InnerRunnable
//=====================================================================

WebViewChromium::WebViewChromiumRunQueue::InnerRunnable::InnerRunnable(
    /* [in] */ WebViewChromiumRunQueue* owner)
    : mOwner(owner)
{
}

ECode WebViewChromium::WebViewChromiumRunQueue::InnerRunnable::Run()
{
    mOwner->DrainQueue();
    return NOERROR;
}

//===============================================================
//           WebViewChromium::WebViewChromiumRunQueue
//===============================================================

WebViewChromium::WebViewChromiumRunQueue::WebViewChromiumRunQueue(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
    mQueue = new ConcurrentLinkedQueue<Runnable>();
}

void WebViewChromium::WebViewChromiumRunQueue::AddTask(
    /* [in] */ IRunnable* task)
{
    mQueue->Add(task);
    if (mFactory->HasStarted()) {
        AutoPtr<IRunnable> runnable = new InnerRunnable(this);
        ThreadUtils::RunOnUiThread(runnable);
    }
}

void WebViewChromium::WebViewChromiumRunQueue::DrainQueue()
{
    if (mQueue == NULL || mQueue.isEmpty()) {
        return;
    }

    Runnable task = mQueue.poll();
    while(task != NULL) {
        task.run();
        task = mQueue.poll();
    }
}

//===============================================================
//           WebViewChromium::InternalAccessAdapter
//===============================================================

WebViewChromium::InternalAccessAdapter::InternalAccessAdapter(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
}

//@Override
Boolean WebViewChromium::InternalAccessAdapter::DrawChild(
    /* [in] */ ICanvas* arg0,
    /* [in] */ IView* arg1,
    /* [in] */ Int64 arg2)
{
    UnimplementedWebViewApi::Invoke();
    return FALSE;
}

//@Override
Boolean WebViewChromium::InternalAccessAdapter::Super_onKeyUp(
    /* [in] */ Int32 arg0,
    /* [in] */ IKeyEvent* arg1)
{
    // Intentional no-op
    return FALSE;
}

//@Override
Boolean WebViewChromium::InternalAccessAdapter::Super_dispatchKeyEventPreIme(
    /* [in] */ IKeyEvent* arg0)
{
    UnimplementedWebViewApi::Invoke();
    return FALSE;
}

//@Override
Boolean WebViewChromium::InternalAccessAdapter::Super_dispatchKeyEvent(
    /* [in] */ IKeyEvent* event)
{
    return mWebViewPrivate->Super_dispatchKeyEvent(event);
}

//@Override
Boolean WebViewChromium::InternalAccessAdapter::Super_onGenericMotionEvent(
    /* [in] */ IMotionEvent* arg0)
{
    return mWebViewPrivate->Super_onGenericMotionEvent(arg0);
}

//@Override
void WebViewChromium::InternalAccessAdapter::Super_onConfigurationChanged(
    /* [in] */ IConfiguration* arg0)
{
    // Intentional no-op
}

//@Override
Int32 WebViewChromium::InternalAccessAdapter::Super_getScrollBarStyle()
{
    return mWebViewPrivate->Super_getScrollBarStyle();
}

//@Override
Boolean WebViewChromium::InternalAccessAdapter::AwakenScrollBars()
{
    mWebViewPrivate->AwakenScrollBars(0);
    // TODO: modify the WebView.PrivateAccess to provide a return value.
    return TRUE;
}

//@Override
Boolean WebViewChromium::InternalAccessAdapter::Super_awakenScrollBars(
    /* [in] */ Int32 arg0,
    /* [in] */ Boolean arg1)
{
    // TODO: need method on WebView.PrivateAccess?
    UnimplementedWebViewApi::Invoke();
    return FALSE;
}

//@Override
void WebViewChromium::InternalAccessAdapter::OnScrollChanged(
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 oldl,
    /* [in] */ Int32 oldt)
{
    // Intentional no-op.
    // Chromium calls this directly to trigger accessibility events. That isn't needed
    // for WebView since super_scrollTo invokes onScrollChanged for us.
}

//@Override
void WebViewChromium::InternalAccessAdapter::OverScrollBy(
    /* [in] */ Int32 deltaX,
    /* [in] */ Int32 deltaY,
    /* [in] */ Int32 scrollX,
    /* [in] */ Int32 scrollY,
    /* [in] */ Int32 scrollRangeX,
    /* [in] */ Int32 scrollRangeY,
    /* [in] */ Int32 maxOverScrollX,
    /* [in] */ Int32 maxOverScrollY,
    /* [in] */ Boolean isTouchEvent)
{
    mWebViewPrivate->OverScrollBy(deltaX, deltaY, scrollX, scrollY,
            scrollRangeX, scrollRangeY, maxOverScrollX, maxOverScrollY, isTouchEvent);
}

//@Override
void WebViewChromium::InternalAccessAdapter::Super_scrollTo(
    /* [in] */ Int32 scrollX,
    /* [in] */ Int32 scrollY)
{
    mWebViewPrivate->Super_scrollTo(scrollX, scrollY);
}

//@Override
void WebViewChromium::InternalAccessAdapter::SetMeasuredDimension(
    /* [in] */ Int32 measuredWidth,
    /* [in] */ Int32 measuredHeight)
{
    mWebViewPrivate->SetMeasuredDimension(measuredWidth, measuredHeight);
}

// @Override
Boolean WebViewChromium::InternalAccessAdapter::Super_onHoverEvent(
    /* [in] */ IMotionEvent* event)
{
    return mWebViewPrivate->Super_onHoverEvent(event);
}


//===============================================================
//           WebViewChromium::WebViewNativeGLDelegate
//===============================================================

WebViewChromium::WebViewNativeGLDelegate::WebViewNativeGLDelegate(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
}

//@Override
Boolean WebViewChromium::WebViewNativeGLDelegate::RequestDrawGL(
    /* [in] */ ICanvas* canvas,
    /* [in] */ Boolean waitForCompletion,
    /* [in] */ IView* containerView)
{
    if (mGLfunctor == NULL) {
        mGLfunctor = new DrawGLFunctor(mAwContents->GetAwDrawGLViewContext());
    }

    return mGLfunctor->RequestDrawGL(
            (HardwareCanvas*) canvas, containerView->GetViewRootImpl(), waitForCompletion);
}

//@Override
void WebViewChromium::WebViewNativeGLDelegate::DetachGLFunctor()
{
    if (mGLfunctor != NULL) {
        mGLfunctor->Detach();
    }
}

//===============================================================
//                WebViewChromium::InitRunnable
//===============================================================

WebViewChromium::InitRunnable::InitRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const Boolean privateBrowsing)
    : mOwner(owner)
    , mPrivateBrowsing(privateBrowsing)
{
}

ECode WebViewChromium::InitRunnable::Run()
{
    mOwner->InitForReal();
    if (mPrivateBrowsing) {
        // Intentionally irreversibly disable the webview instance, so that private
        // user data cannot leak through misuse of a non-privateBrowing WebView
        // instance. Can't just null out mAwContents as we never null-check it
        // before use.
        mOwner->Destroy();
    }

    return NOERROR;
}

//===============================================================
//             WebViewChromium::CheckThreadRunnable
//===============================================================

WebViewChromium::CheckThreadRunnable::CheckThreadRunnable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
}

ECode WebViewChromium::CheckThreadRunnable::Run()
{
//    throw threadViolation;
    return NOERROR;
}

//===============================================================
//     WebViewChromium::SetHorizontalScrollbarOverlayRunnable
//===============================================================

WebViewChromium::SetHorizontalScrollbarOverlayRunnable::SetHorizontalScrollbarOverlayRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const Boolean overlay)
    : mOwner(owner)
    , mOverlay(overlay)
{
}

ECode WebViewChromium::SetHorizontalScrollbarOverlayRunnable::Run()
{
    mOwner->SetHorizontalScrollbarOverlay(mOverlay);
    return NOERROR;
}

//===============================================================
//      WebViewChromium::SetVerticalScrollbarOverlayRunnable
//===============================================================

WebViewChromium::SetVerticalScrollbarOverlayRunnable::SetVerticalScrollbarOverlayRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const Boolean overlay)
    : mOwner(owner)
    , mOverlay(overlay)
{
}

ECode WebViewChromium::SetVerticalScrollbarOverlayRunnable::Run()
{
    mOwner->SetVerticalScrollbarOverlay(mOverlay);
    return NOERROR;
}

//===============================================================
//      WebViewChromium::OverlayHorizontalScrollbarCallable
//===============================================================

WebViewChromium::OverlayHorizontalScrollbarCallable::OverlayHorizontalScrollbarCallable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
}

ECode WebViewChromium::OverlayHorizontalScrollbarRunnable::Run(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOwner->OverlayHorizontalScrollbar();
    return NOERROR;
}

//===============================================================
//      WebViewChromium::OverlayVerticalScrollbarCallable
//===============================================================

WebViewChromium::OverlayVerticalScrollbarCallable::OverlayVerticalScrollbarCallable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
}

ECode WebViewChromium::OverlayVerticalScrollbarCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOwner->OverlayVerticalScrollbar();
    return NOERROR;
}

//===============================================================
//            WebViewChromium::GetCertificateCallable
//===============================================================

WebViewChromium::GetCertificateCallable::GetCertificateCallable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
}

ECode WebViewChromium::GetCertificateCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOwner->GetCertificate();
    return NOERROR;
}

//===============================================================
//      WebViewChromium::SetHttpAuthUsernamePasswordRunnable
//===============================================================

WebViewChromium::SetHttpAuthUsernamePasswordRunnable::SetHttpAuthUsernamePasswordRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const String& host,
    /* [in] */ const String& realm,
    /* [in] */ const String& username,
    /* [in] */ const String& password)
    : mOwner(owner)
    , mHost(host)
    , mRealm(realm)
    , mUsername(username)
    , mPassword(password)
{
}

WebViewChromium::SetHttpAuthUsernamePasswordRunnable::SetHttpAuthUsernamePasswordRunnable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
}

ECode WebViewChromium::SetHttpAuthUsernamePasswordRunnable::Run()
{
    mOwner->SetHttpAuthUsernamePassword(mHost, mRealm, mUsername, mPassword);
    return NOERROR;
}

//===============================================================
//      WebViewChromium::GetHttpAuthUsernamePasswordCallable
//===============================================================

WebViewChromium::GetHttpAuthUsernamePasswordCallable::GetHttpAuthUsernamePasswordCallable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const String& host,
    /* [in] */ const String& realm)
    : mOwner(owner)
    , mHost(host)
    , mRealm(realm)
{
}

ECode WebViewChromium::GetHttpAuthUsernamePasswordCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = GetHttpAuthUsernamePassword(host, realm);
    return NOERROR;
}

//===============================================================
//               WebViewChromium::DestroyRunnable
//===============================================================

WebViewChromium::DestroyRunnable::DestroyRunnable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
}

ECode WebViewChromium::DestroyRunnable::Run()
{
    mOwner->Destroy();
    return NOERROR;
}

//===============================================================
//          WebViewChromium::SetNetworkAvailableRunnable
//===============================================================

WebViewChromium::SetNetworkAvailableRunnable::SetNetworkAvailableRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const Boolean networkUp)
    : mOwner(owner)
    , mNetworkUp(networkUp)
{
}

ECode WebViewChromium::SetNetworkAvailableRunnable::Run()
{
    mOwner->SetNetworkAvailable(mNetworkUp);
    return NOERROR;
}

//===============================================================
//            WebViewChromium::SaveStateCallable
//===============================================================

WebViewChromium::SaveStateCallable::SaveStateCallable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const IBundle* outState);

ECode WebViewChromium::SaveStateCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOwner->SaveState(mOutState);
    return NOERROR;
}

//===============================================================
//            WebViewChromium::RestoreStateCallable
//===============================================================

WebViewChromium::RestoreStateCallable::RestoreStateCallable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const IBundle* inState)
    : mOwner(owner)
    , mInState(inState)
{
}

ECode WebViewChromium::RestoreStateCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = RestoreState(inState);
    return NOERROR;
}

//===============================================================
//               WebViewChromium::LoadUrlRunnable
//===============================================================

WebViewChromium::LoadUrlRunnable::LoadUrlRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const String& url,
    /* [in] */ const String& JAVASCRIPT_SCHEME)
    : mOwner(owner)
    , mUrl(url)
    , mJAVASCRIPT_SCHEME(JAVASCRIPT_SCHEME)
{
}

ECode WebViewChromium::LoadUrlRunnable::Run()
{
    mAwContents->EvaluateJavaScriptEvenIfNotYetNavigated(
                            url.Substring(JAVASCRIPT_SCHEME.GetLength()));
    return NOERROR;
}

//===============================================================
//           WebViewChromium::LoadUrlOnUiThreadRunnable
//===============================================================

WebViewChromium::LoadUrlOnUiThreadRunnable::LoadUrlOnUiThreadRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const LoadUrlParams* loadUrlParams)
    : mOwner(owner)
    , mLoadUrlParams(loadUrlParams)
{
}

ECode WebViewChromium::LoadUrlOnUiThreadRunnable::Run()
{
    mOwner->mAwContents->LoadUrl(mLoadUrlParams);
    return NOERROR;
}

//===============================================================
//            WebViewChromium::SaveWebArchiveRunnable
//===============================================================

WebViewChromium::SaveWebArchiveRunnable::SaveWebArchiveRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const String& basename,
    /* [in] */ const Boolean autoname,
    /* [in] */ IValueCallback* callback)
    : mOwner(owner)
    , mBasename(basename)
    , mAutoname(autoname)
    , mCallback(callback)
{
}

ECode WebViewChromium::SaveWebArchiveRunnable::Run()
{
    mOwner->SaveWebArchive(mBasename, mAutoname, mCallback);
    return NOERROR;
}

//===============================================================
//            WebViewChromium::StopLoadingRunnable
//===============================================================

WebViewChromium::StopLoadingRunnable::StopLoadingRunnable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
}

ECode WebViewChromium::StopLoadingRunnable::Run()
{
    mOwner->StopLoading();
    return NOERROR;
}

//===============================================================
//              WebViewChromium::ReloadRunnable
//===============================================================

WebViewChromium::ReloadRunnable::ReloadRunnable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
}

ECode WebViewChromium::ReloadRunnable::Run()
{
    mOwner->Reload();
    return NOERROR;
}

//===============================================================
//              WebViewChromium::CanGoBackCallable
//===============================================================

WebViewChromium::CanGoBackCallable::CanGoBackCallable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
}

ECode WebViewChromium::CanGoBackCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOwner->CanGoBack();
    return NOERROR;
}

//===============================================================
//               WebViewChromium::GoBackRunnable
//===============================================================

WebViewChromium::GoBackRunnable::GoBackRunnable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
}

ECode WebViewChromium::GoBackRunnable::Run()
{
    mOwner->GoBack();
    return NOERROR;
}

//===============================================================
//             WebViewChromium::CanGoForwardCallable
//===============================================================

WebViewChromium::CanGoForwardCallable::CanGoForwardCallable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
}

ECode WebViewChromium::CanGoForwardCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    return NOERROR;
}

//===============================================================
//              WebViewChromium::GoForwardRunnable
//===============================================================

WebViewChromium::GoForwardRunnable::GoForwardRunnable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
}

ECode WebViewChromium::GoForwardRunnable::Run()
{
    return NOERROR;
}

//===============================================================
//           WebViewChromium::CanGoBackOrForwardCallable
//===============================================================

WebViewChromium::CanGoBackOrForwardCallable::CanGoBackOrForwardCallable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const Int32 steps)
    : mOwner(owner)
    , mSteps(steps)
{
}

ECode WebViewChromium::CanGoBackOrForwardCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOwner->CanGoBackOrForward(mSteps);
    return NOERROR;
}

//===============================================================
//            WebViewChromium::GoBackOrForwardRunnable
//===============================================================

WebViewChromium::GoBackOrForwardRunnable::GoBackOrForwardRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const Int32 steps)
    : mOwner(owner)
    , mSteps(steps)
{
}

ECode WebViewChromium::GoBackOrForwardRunnable::Run()
{
    mOwner->GoBackOrForward(mSteps);
    return NOERROR;
}

//===============================================================
//              WebViewChromium::PageUpCallable
//===============================================================

WebViewChromium::PageUpCallable::PageUpCallable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const Boolean top)
    : mOwner(owner)
    , mTop(top)
{
}

ECode WebViewChromium::PageUpCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOwner->PageUp(mTop);
    return NOERROR;
}

//===============================================================
//              WebViewChromium::PageDownCallable
//===============================================================

WebViewChromium::PageDownCallable::PageDownCallable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const Boolean bottom)
    : mOwner(owner)
    , mBottom(bottom)
{
}

ECode WebViewChromium::PageDownCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOwner->PageDown(mBottom);
    return NOERROR;
}

//===============================================================
//              WebViewChromium::ClearViewRunnable
//===============================================================

WebViewChromium::ClearViewRunnable::ClearViewRunnable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
}

ECode WebViewChromium::ClearViewRunnable::Run()
{
    mOwner->ClearView();
    return NOERROR;
}

//===============================================================
//              WebViewChromium::CapturePictureCallable
//===============================================================

WebViewChromium::CapturePictureCallable::CapturePictureCallable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
}

ECode WebViewChromium::CapturePictureCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOwner->CapturePicture();
    return NOERROR;
}

//===============================================================
//            WebViewChromium::InvokeZoomPickerRunnable
//===============================================================

WebViewChromium::InvokeZoomPickerRunnable::InvokeZoomPickerRunnable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
}

ECode WebViewChromium::InvokeZoomPickerRunnable::Run()
{
    mOwner->InvokeZoomPicker();
    return NOERROR;
}

//===============================================================
//            WebViewChromium::GetHitTestResultCallable
//===============================================================

WebViewChromium::GetHitTestResultCallable::GetHitTestResultCallable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
}

ECode WebViewChromium::GetHitTestResultCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = GetHitTestResult();
    return NOERROR;
}

//===============================================================
//          WebViewChromium::RequestFocusNodeHrefRunnable
//===============================================================

WebViewChromium::RequestFocusNodeHrefRunnable::RequestFocusNodeHrefRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const IMessage* hrefMsg)
    : mOwner(owner)
    , mHrefMsg(hrefMsg)
{
}

ECode WebViewChromium::RequestFocusNodeHrefRunnable::Run()
{
    mOwner->RequestFocusNodeHref(mHrefMsg);
    return NOERROR;
}

//===============================================================
//            WebViewChromium::RequestImageRefRunnable
//===============================================================

WebViewChromium::RequestImageRefRunnable::RequestImageRefRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const IMessage* msg)
    : mOwner(owner)
    , mMsg(msg)
{
}

ECode WebViewChromium::RequestImageRefRunnable::Run()
{
    mOwner->RequestImageRef(msg);
    return NOERROR;
}

//===============================================================
//              WebViewChromium::GetUrlCallable
//===============================================================

WebViewChromium::GetUrlCallable::GetUrlCallable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
}

ECode WebViewChromium::GetUrlCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOwner->GetUrl();
    return NOERROR;
}

//===============================================================
//            WebViewChromium::GetOriginalUrlCallable
//===============================================================

WebViewChromium::GetOriginalUrlCallable::GetOriginalUrlCallable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
}

ECode WebViewChromium::GetOriginalUrlCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOwner->GetOriginalUrl();
    return NOERROR;
}

//===============================================================
//              WebViewChromium::GetTitleCallable
//===============================================================

WebViewChromium::GetTitleCallable::GetTitleCallable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
}

ECode WebViewChromium::GetTitleCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOwner->GetTitle();
    return NOERROR;
}

//===============================================================
//              WebViewChromium::GetFaviconCallable
//===============================================================

WebViewChromium::GetFaviconCallable::GetFaviconCallable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
}

ECode WebViewChromium::GetFaviconCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOwner->GetFavicon();
    return NOERROR;
}

//===============================================================
//              WebViewChromium::PauseTimersRunnable
//===============================================================

WebViewChromium::PauseTimersRunnable::PauseTimersRunnable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
}

ECode WebViewChromium::PauseTimersRunnable::Run()
{
    mOwner->PauseTimers();
    return NOERROR;
}

//===============================================================
//              WebViewChromium::ResumeTimersRunnable
//===============================================================

WebViewChromium::ResumeTimersRunnable::ResumeTimersRunnable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
}

ECode WebViewChromium::ResumeTimersRunnable::Run()
{
    mOwner->ResumeTimers();
    return NOERROR;
}

//===============================================================
//               WebViewChromium::OnPauseRunnable
//===============================================================

WebViewChromium::OnPauseRunnable::OnPauseRunnable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
}

ECode WebViewChromium::OnPauseRunnable::Run()
{
    mOwner->OnPause();
    return NOERROR;
}

//===============================================================
//               WebViewChromium::OnResumeRunnable
//===============================================================

WebViewChromium::OnResumeRunnable::OnResumeRunnable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
}

ECode WebViewChromium::OnResumeRunnable::Run()
{
    mOwner->OnResume();
    return NOERROR;
}

//===============================================================
//               WebViewChromium::IsPausedCallable
//===============================================================

WebViewChromium::IsPausedCallable::IsPausedCallable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
}

ECode WebViewChromium::IsPausedCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOwner->IsPaused();
    return NOERROR;
}

//===============================================================
//               WebViewChromium::ClearCacheRunnable
//===============================================================

WebViewChromium::ClearCacheRunnable::ClearCacheRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const Boolean includeDiskFiles)
    : mOwner(owner)
    , mIncludeDiskFiles(includeDiskFiles)
{
}

ECode WebViewChromium::ClearCacheRunnable::Run()
{
    mOwner->ClearCache(includeDiskFiles);
    return NOERROR;
}

//===============================================================
//              WebViewChromium::ClearFormDataRunnable
//===============================================================

WebViewChromium::ClearFormDataRunnable::ClearFormDataRunnable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
}

ECode WebViewChromium::ClearFormDataRunnable::Run()
{
    mOwner->ClearFormData();
    return NOERROR;
}

//===============================================================
//              WebViewChromium::ClearHistoryRunnable
//===============================================================

WebViewChromium::ClearHistoryRunnable::ClearHistoryRunnable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
}

ECode WebViewChromium::ClearHistoryRunnable::Run()
{
    mOwner->ClearHistory();
    return NOERROR;
}

//===============================================================
//           WebViewChromium::ClearSslPreferencesRunnable
//===============================================================

WebViewChromium::ClearSslPreferencesRunnable::ClearSslPreferencesRunnable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
}

ECode WebViewChromium::ClearSslPreferencesRunnable::Run()
{
    mOwner->ClearSslPreferences();
    return NOERROR;
}

//===============================================================
//           WebViewChromium::CopyBackForwardListCallable
//===============================================================

WebViewChromium::CopyBackForwardListCallable::CopyBackForwardListCallable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
}

ECode WebViewChromium::CopyBackForwardListCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOwner->CopyBackForwardList();
    return NOERROR;
}

//===============================================================
//              WebViewChromium::FindNextRunnable
//===============================================================

WebViewChromium::FindNextRunnable::FindNextRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const Boolean forwards)
    : mOwner(owner)
    , mForwards(forwards)
{
}

ECode WebViewChromium::FindNextRunnable::Run()
{
    mOwner->FindNext(mForwards);
    return NOERROR;
}

//===============================================================
//              WebViewChromium::FindAllAsyncRunnable
//===============================================================

WebViewChromium::FindAllAsyncRunnable::FindAllAsyncRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const String& searchString)
    : mOwner(owner)
    , mSearchString(searchString)
{
}

ECode WebViewChromium::FindAllAsyncRunnable::Run()
{
    mOwner->FindAllAsync(searchString);
    return NOERROR;
}

//===============================================================
//       WebViewChromium::NotifyFindDialogDismissedRunnable
//===============================================================

WebViewChromium::NotifyFindDialogDismissedRunnable::NotifyFindDialogDismissedRunnable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
}

ECode WebViewChromium::NotifyFindDialogDismissedRunnable::Run()
{
    mOwner->NotifyFindDialogDismissed();
    return NOERROR;
}

//===============================================================
//           WebViewChromium::ClearMatchesRunnable
//===============================================================

WebViewChromium::ClearMatchesRunnable::ClearMatchesRunnable(
    /* [in] */ WebViewChromium* owner)
{
}

ECode WebViewChromium::ClearMatchesRunnable::Run()
{
    mOwner->ClearMatches();
    return NOERROR;
}

//===============================================================
//           WebViewChromium::DocumentHasImagesRunnable
//===============================================================

WebViewChromium::DocumentHasImagesRunnable::DocumentHasImagesRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const IMessage* response)
    : mOwner(owner)
    , mResponse(response)
{
}

ECode WebViewChromium::DocumentHasImagesRunnable::Run()
{
    mOwner->DocumentHasImages(mResponse);
    return NOERROR;
}

//===============================================================
//           WebViewChromium::SetPictureListenerRunnable
//===============================================================

WebViewChromium::SetPictureListenerRunnable::SetPictureListenerRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const IWebViewPictureListener* listener)
    : mOwner(owner)
    , mListener(listener)
{
}

ECode WebViewChromium::SetPictureListenerRunnable::Run()
{
    mOwner->SetPictureListener(mListener);
    return NOERROR;
}

//===============================================================
//         WebViewChromium::AddJavascriptInterfaceRunnable
//===============================================================

WebViewChromium::AddJavascriptInterfaceRunnable::AddJavascriptInterfaceRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ IInterface* obj,
    /* [in] */ const String& interfaceName)
    : mOwner(owner)
    , mObj(obj)
    , mInterfaceName(interfaceName)
{
}

ECode WebViewChromium::AddJavascriptInterfaceRunnable::Run()
{
    mOwner->AddJavascriptInterface(mObj, mInterfaceName);
    return NOERROR;
}

//===============================================================
//      WebViewChromium::RemoveJavascriptInterfaceRunnable
//===============================================================

WebViewChromium::RemoveJavascriptInterfaceRunnable::RemoveJavascriptInterfaceRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const String& interfaceName)
    : mOwner(owner)
    , mInterfaceName(interfaceName)
{
}

ECode WebViewChromium::RemoveJavascriptInterfaceRunnable::Run()
{
    mOwner->RemoveJavascriptInterface(mInterfaceName);
    return NOERROR;
}

//===============================================================
//           WebViewChromium::FlingScrollRunnable
//===============================================================

WebViewChromium::FlingScrollRunnable::FlingScrollRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const Int32 vx,
    /* [in] */ const Int32 vy)
    : mOwner(owner)
    , mVx(vx)
    , mVy(vy)
{
}

ECode WebViewChromium::FlingScrollRunnable::Run()
{
    mOwner->FlingScroll(mVx, mVy);
    return NOERROR;
}

//===============================================================
//              WebViewChromium::ZoomInCallable
//===============================================================

WebViewChromium::ZoomInCallable::ZoomInCallable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
}

ECode WebViewChromium::ZoomInCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOwner->ZoomIn();
    return NOERROR;
}

//===============================================================
//              WebViewChromium::ZoomOutCallable
//===============================================================

WebViewChromium::ZoomOutCallable::ZoomOutCallable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
}

ECode WebViewChromium::ZoomOutCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOwner->ZoomOut();
    return NOERROR;
}

//===============================================================
//     WebViewChromium::ShouldDelayChildPressedStateCallable
//===============================================================

WebViewChromium::ShouldDelayChildPressedStateCallable::ShouldDelayChildPressedStateCallable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
}

ECode WebViewChromium::ShouldDelayChildPressedStateCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOwner->ShouldDelayChildPressedState();
    return NOERROR;
}

//===============================================================
//     WebViewChromium::GetAccessibilityNodeProviderCallable
//===============================================================

WebViewChromium::GetAccessibilityNodeProviderCallable::GetAccessibilityNodeProviderCallable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
}

ECode WebViewChromium::GetAccessibilityNodeProviderCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOwner->GetAccessibilityNodeProvider();
    return NOERROR;
}

//===============================================================
//   WebViewChromium::OnInitializeAccessibilityNodeInfoRunnable
//===============================================================

WebViewChromium::OnInitializeAccessibilityNodeInfoRunnable::OnInitializeAccessibilityNodeInfoRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const IAccessibilityNodeInfo* info)
    : mOwner(owner)
    , mInfo(info)
{
}

ECode WebViewChromium::OnInitializeAccessibilityNodeInfoRunnable::Run()
{
    mOwner->OnInitializeAccessibilityNodeInfo(mInfo);
    return NOERROR;
}

//===============================================================
//    WebViewChromium::OnInitializeAccessibilityEventRunnable
//===============================================================

WebViewChromium::OnInitializeAccessibilityEventRunnable::OnInitializeAccessibilityEventRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const IAccessibilityEvent* event)
    : mOwner(owner)
    , mEvent(event)
{
}

ECode WebViewChromium::OnInitializeAccessibilityEventRunnable::Run()
{
    mOwner->OnInitializeAccessibilityEvent(mEvent);
    return NOERROR;
}

//===============================================================
//     WebViewChromium::PerformAccessibilityActionCallable
//===============================================================

WebViewChromium::PerformAccessibilityActionCallable::PerformAccessibilityActionCallable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const Int32 action,
    /* [in] */ const IBundle* arguments)
    : mOwner(owner)
    , mAction(action)
    , mArguments(arguments)
{
}

ECode WebViewChromium::PerformAccessibilityActionCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOwner->PerformAccessibilityAction(mAction, mArguments);
    return NOERROR;
}

//===============================================================
//         WebViewChromium::SetOverScrollModeRunnable
//===============================================================

WebViewChromium::SetOverScrollModeRunnable::SetOverScrollModeRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const Int32 mode)
    : mOwner(owner)
    , mMode(mode)
{
}

ECode WebViewChromium::SetOverScrollModeRunnable::Run()
{
    mOwner->SetOverScrollMode(mMode);
    return NOERROR;
}

//===============================================================
//         WebViewChromium::SetScrollBarStyleRunnable
//===============================================================

WebViewChromium::SetScrollBarStyleRunnable::SetScrollBarStyleRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const Int32 style)
    : mOwner(owner)
    , mStyle(style)
{
}

ECode WebViewChromium::SetScrollBarStyleRunnable::Run()
{
    mOwner->SetScrollBarStyle(mStyle);
    return NOERROR;
}

//===============================================================
//           WebViewChromium::OnOverScrolledRunnable
//===============================================================

WebViewChromium::OnOverScrolledRunnable::OnOverScrolledRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const Int32 scrollX,
    /* [in] */ const Int32 scrollY,
    /* [in] */ const Boolean clampedX,
    /* [in] */ const Boolean clampedY)
    : mOwner(owner)
    , mScrollX(scrollX)
    , mScrollY(scrollY)
    , mClampedX(clampedX)
    , mClampedY(clampedY)
{
}

ECode WebViewChromium::OnOverScrolledRunnable::Run()
{
    mOwner->OnOverScrolled(mScrollX, mScrollY, mClampedX, mClampedY);
    return NOERROR;
}

//===============================================================
//      WebViewChromium::OnWindowVisibilityChangedRunnable
//===============================================================

WebViewChromium::OnWindowVisibilityChangedRunnable::OnWindowVisibilityChangedRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const Int32 visibility)
    : mOwner(owner)
    , mVisibility(visibility)
{
}

ECode WebViewChromium::OnWindowVisibilityChangedRunnable::Run()
{
    mOwner->OnWindowVisibilityChanged(mVisibility);
    return NOERROR;
}

//===============================================================
//             WebViewChromium::OnDrawRunnable
//===============================================================

WebViewChromium::OnDrawRunnable::OnDrawRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const ICanvas* canvas)
    : mOwner(owner)
    , mCanvas(canvas)
{
}

ECode WebViewChromium::OnDrawRunnable::Run()
{
    mOwner->OnDraw(mCanvas);
    return NOERROR;
}

//===============================================================
//        WebViewChromium::OnConfigurationChangedRunnable
//===============================================================

WebViewChromium::OnConfigurationChangedRunnable::OnConfigurationChangedRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const IConfiguration* newConfig)
    : mOwner(owner)
    , mNewConfig(newConfig)
{
}

ECode WebViewChromium::OnConfigurationChangedRunnable::Run()
{
    mOwner->OnConfigurationChanged(mNewConfig);
    return NOERROR;
}

//===============================================================
//           WebViewChromium::OnKeyMultipleCallable
//===============================================================

WebViewChromium::OnKeyMultipleCallable::OnKeyMultipleCallable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const Int32 keyCode,
    /* [in] */ const Int32 repeatCount,
    /* [in] */ const IKeyEvent* event)
    : mOwner(owner)
    , mKeyCode(keyCode)
    , mRepeatCount(repeatCount)
    , mEvent(event)
{
}

ECode WebViewChromium::OnKeyMultipleCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOwner->OnKeyMultiple(mKeyCode, mRepeatCount, mEvent);
    return NOERROR;
}

//===============================================================
//            WebViewChromium::OnKeyDownCallable
//===============================================================

WebViewChromium::OnKeyDownCallable::OnKeyDownCallable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const Int32 keyCode,
    /* [in] */ const IKeyEvent* event)
    : mOwner(owner)
    , mKeyCode(keyCode)
    , mEvent(event)
{
}

ECode WebViewChromium::OnKeyDownCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOwner->OnKeyDown(mKeyCode, mEvent);
    return NOERROR;
}

//===============================================================
//              WebViewChromium::OnKeyUpCallable
//===============================================================

WebViewChromium::OnKeyUpCallable::OnKeyUpCallable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const Int32 keyCode,
    /* [in] */ const IKeyEvent* event)
    : mOwner(owner)
    , mKeyCode(keyCode)
    , mEvent(event)
{
}

ECode WebViewChromium::OnKeyUpCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOwner->OnKeyUp(mKeyCode, mEvent);
    return NOERROR;
}

//===============================================================
//         WebViewChromium::OnDetachedFromWindowRunnable
//===============================================================

WebViewChromium::OnDetachedFromWindowRunnable::OnDetachedFromWindowRunnable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
}

ECode WebViewChromium::OnDetachedFromWindowRunnable::Run()
{
    mOwner->OnDetachedFromWindow();
    return NOERROR;
}

//===============================================================
//         WebViewChromium::OnVisibilityChangedRunnable
//===============================================================

WebViewChromium::OnVisibilityChangedRunnable::OnVisibilityChangedRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const IView* changedView,
    /* [in] */ const Int32 visibility)
    : mOwner(owner)
    , mChangedView(changedView)
    , mVisibility(visibility)
{
}

ECode WebViewChromium::OnVisibilityChangedRunnable::Run()
{
    mOwner->OnVisibilityChanged(mChangedView, mVisibility);
    return NOERROR;
}

//===============================================================
//         WebViewChromium::OnWindowFocusChangedRunnable
//===============================================================

WebViewChromium::OnWindowFocusChangedRunnable::OnWindowFocusChangedRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const Boolean hasWindowFocus)
    : mOwner(owner)
    , mHasWindowFocus(hasWindowFocus)
{
}

ECode WebViewChromium::OnWindowFocusChangedRunnable::Run()
{
    mOwner->OnWindowFocusChanged(mHasWindowFocus);
    return NOERROR;
}

//===============================================================
//           WebViewChromium::OnFocusChangedRunnable
//===============================================================

WebViewChromium::OnFocusChangedRunnable::OnFocusChangedRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const Boolean focused,
    /* [in] */ const Int32 direction,
    /* [in] */ const IRect* previouslyFocusedRect)
    : mOwner(owner)
    , mFocused(focused)
    , mDirection(direction)
    , mPreviouslyFocusedRect(previouslyFocusedRect)
{
}

ECode WebViewChromium::OnFocusChangedRunnable::Run()
{
    mOwner->OnFocusChanged(mFocused, mDirection, mPreviouslyFocusedRect);
    return NOERROR;
}

//===============================================================
//            WebViewChromium::OnSizeChangedRunnable
//===============================================================

WebViewChromium::OnSizeChangedRunnable::OnSizeChangedRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const Int32 w,
    /* [in] */ const Int32 h,
    /* [in] */ const Int32 ow,
    /* [in] */ const Int32 oh)
    : mOwner(owner)
    , mW(w)
    , mH(h)
    , mOw(ow)
    , mOh(oh)
{
}

ECode WebViewChromium::OnSizeChangedRunnable::Run()
{
    mOwner->OnSizeChanged(mW, mH, mOw, mOh);
    return NOERROR;
}

//===============================================================
//           WebViewChromium::DispatchKeyEventCallable
//===============================================================

WebViewChromium::DispatchKeyEventCallable::DispatchKeyEventCallable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const IKeyEvent* event)
    : mOwner(owner)
    , mEvent(event)
{
}

ECode WebViewChromium::DispatchKeyEventCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOwner->DispatchKeyEvent(event);
    return NOERROR;
}

//===============================================================
//            WebViewChromium::OnTouchEventCallable
//===============================================================

WebViewChromium::OnTouchEventCallable::OnTouchEventCallable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const IMotionEvent* ev)
{
}

ECode WebViewChromium::OnTouchEventCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOwner->OnTouchEvent(mEv);
    return NOERROR;
}

//===============================================================
//            WebViewChromium::OnHoverEventCallable
//===============================================================

WebViewChromium::OnHoverEventCallable::OnHoverEventCallable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const IMotionEvent* event)
    : mOwner(owner)
    , mEvent(event)
{
}

ECode WebViewChromium::OnHoverEventCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOwner->OnHoverEvent(mEvent);
    return NOERROR;
}

//===============================================================
//         WebViewChromium::OnGenericMotionEventCallable
//===============================================================

WebViewChromium::OnGenericMotionEventCallable::OnGenericMotionEventCallable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const IMotionEvent* event)
    : mOwner(owner)
    , mEvent(event)
{
}

ECode WebViewChromium::OnGenericMotionEventCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOwner->OnGenericMotionEvent(mEvent);
    return NOERROR;
}

//===============================================================
//           WebViewChromium::RequestFocusCallable
//===============================================================

WebViewChromium::RequestFocusCallable::RequestFocusCallable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const Int32 direction,
    /* [in] */ const IRect* previouslyFocusedRect)
    : mOwner(owner)
    , mDirection(direction)
    , mPreviouslyFocusedRect(previouslyFocusedRect)
{
}

ECode WebViewChromium::RequestFocusCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOwner->RequestFocus(mDirection, mPreviouslyFocusedRect);
    return NOERROR;
}

//===============================================================
//             WebViewChromium::OnMeasureRunnable
//===============================================================

WebViewChromium::OnMeasureRunnable::OnMeasureRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const Int32 widthMeasureSpec,
    /* [in] */ const Int32 heightMeasureSpec)
    : mOwner(owner)
    , mWidthMeasureSpec(widthMeasureSpec)
    , mHeightMeasureSpec(heightMeasureSpec)
{
}

ECode WebViewChromium::OnMeasureRunnable::Run()
{
    mOwner->OnMeasure(mWidthMeasureSpec, mHeightMeasureSpec);
    return NOERROR;
}

//===============================================================
//    WebViewChromium::RequestChildRectangleOnScreenCallable
//===============================================================

WebViewChromium::RequestChildRectangleOnScreenCallable::RequestChildRectangleOnScreenCallable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const IView* child,
    /* [in] */ const IRect* rect,
    /* [in] */ const Boolean immediate)
    : mOwner(owner)
    , mChild(child)
    , mRect(rect)
    , mImmediate(immediate)
{
}

ECode WebViewChromium::RequestChildRectangleOnScreenCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOwner->RequestChildRectangleOnScreen(mChild, mRect, mImmediate);
    return NOERROR;
}

//===============================================================
//          WebViewChromium::SetBackgroundColorRunnable
//===============================================================

WebViewChromium::SetBackgroundColorRunnable::SetBackgroundColorRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const Int32 color)
    : mOwner(owner)
    , mColor(color)
{
}

ECode WebViewChromium::SetBackgroundColorRunnable::Run()
{
    mOwner->SetBackgroundColor(mColor);
    return NOERROR;
}

//===============================================================
//             WebViewChromium::SetLayerTypeRunnable
//===============================================================

WebViewChromium::SetLayerTypeRunnable::SetLayerTypeRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const Int32 layerType,
    /* [in] */ const IPaint* paint)
    : mOwner(owner)
    , mLayerType(layerType)
    , mPaint(paint)
{
}

ECode WebViewChromium::SetLayerTypeRunnable::Run()
{
    mOwner->SetLayerType(mLayerType, mPaint);
    return NOERROR;
}

//===============================================================
//     WebViewChromium::ComputeHorizontalScrollRangeCallable
//===============================================================

WebViewChromium::ComputeHorizontalScrollRangeCallable::ComputeHorizontalScrollRangeCallable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
}

ECode WebViewChromium::ComputeHorizontalScrollRangeCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOwner->ComputeHorizontalScrollRange();
    return NOERROR;
}

//===============================================================
//     WebViewChromium::ComputeHorizontalScrollOffsetCallable
//===============================================================

WebViewChromium::ComputeHorizontalScrollOffsetCallable::ComputeHorizontalScrollOffsetCallable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
}

ECode WebViewChromium::ComputeHorizontalScrollOffsetCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOwner->ComputeHorizontalScrollOffset();
    return NOERROR;
}

//===============================================================
//      WebViewChromium::ComputeVerticalScrollRangeCallable
//===============================================================

WebViewChromium::ComputeVerticalScrollRangeCallable::ComputeVerticalScrollRangeCallable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
}

ECode WebViewChromium::ComputeVerticalScrollRangeCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOwner->ComputeVerticalScrollRange();
    return NOERROR;
}

//===============================================================
//      WebViewChromium::ComputeVerticalScrollOffsetCallable
//===============================================================

WebViewChromium::ComputeVerticalScrollOffsetCallable::ComputeVerticalScrollOffsetCallable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
}

ECode WebViewChromium::ComputeVerticalScrollOffsetCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOwner->ComputeVerticalScrollOffset();
    return NOERROR;
}

//===============================================================
//      WebViewChromium::ComputeVerticalScrollExtentCallable
//===============================================================

WebViewChromium::ComputeVerticalScrollExtentCallable::ComputeVerticalScrollExtentCallable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
}

ECode WebViewChromium::ComputeVerticalScrollExtentCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOwner->ComputeVerticalScrollExtent();
    return NOERROR;
}

//===============================================================
//          WebViewChromium::ComputeScrollRunnable
//===============================================================

WebViewChromium::ComputeScrollRunnable::ComputeScrollRunnable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
}

ECode WebViewChromium::ComputeScrollRunnable::Run()
{
    mOwner->ComputeScroll();
    return NOERROR;
}




















//===============================================================
//                      WebViewChromium
//===============================================================

const String WebViewChromium::TAG = WebViewChromium.class.getSimpleName();

Boolean WebViewChromium::sRecordWholeDocumentEnabledByApi = FALSE;

// This does not touch any global / non-threadsafe state, but note that
// init is ofter called right after and is NOT threadsafe.
WebViewChromium::WebViewChromium(
    /* [in] */ WebViewChromiumFactoryProvider* factory,
    /* [in] */ IWebView* webView,
    /* [in] */ IWebViewPrivateAccess* webViewPrivate)
{
#if 0
    mWebView = webView;
    mWebViewPrivate = webViewPrivate;
    mHitTestResult = new WebView.HitTestResult();
    mAppTargetSdkVersion = mWebView.getContext().getApplicationInfo().targetSdkVersion;
    mFactory = factory;
    mRunQueue = new WebViewChromiumRunQueue();
    String webViewAssetPath = WebViewFactory.getLoadedPackageInfo().applicationInfo.sourceDir;
    mWebView.getContext().getAssets().addAssetPath(webViewAssetPath);
#endif
}

void WebViewChromium::EnableSlowWholeDocumentDraw()
{
    sRecordWholeDocumentEnabledByApi = TRUE;
}

void WebViewChromium::CompleteWindowCreation(
    /* [in] */ IWebView* parent,
    /* [in] */ IWebView* child)
{
#if 0
    AwContents parentContents = ((WebViewChromium) parent.getWebViewProvider()).mAwContents;
    AwContents childContents =
            child == null ? null : ((WebViewChromium) child.getWebViewProvider()).mAwContents;
    parentContents.supplyContentsForPopup(childContents);
#endif
}

AutoPtr<IInterface> WebViewChromium::RunBlockingFuture(
    /* [in] */ IFutureTask* task)
{
#if 0
    if (!mFactory.hasStarted()) throw new RuntimeException("Must be started before we block!");
    if (ThreadUtils.runningOnUiThread()) {
        throw new IllegalStateException("This method should only be called off the UI thread");
    }
    mRunQueue.addTask(task);
    try {
        return task.get(4, TimeUnit.SECONDS);
    } catch (java.util.concurrent.TimeoutException e) {
        throw new RuntimeException("Probable deadlock detected due to WebView API being called "
                + "on incorrect thread while the UI thread is blocked.", e);
    } catch (Exception e) {
        throw new RuntimeException(e);
    }
#endif
}

// We have a 4 second timeout to try to detect deadlocks to detect and aid in debuggin
// deadlocks.
// Do not call this method while on the UI thread!
void WebViewChromium::RunVoidTaskOnUiThreadBlocking(
    /* [in] */ IRunnable* r)
{
#if 0
    FutureTask<Void> task = new FutureTask<Void>(r, null);
    runBlockingFuture(task);
#endif
}

AutoPtr<IInterface> WebViewChromium::RunOnUiThreadBlocking(
    /* [in] */ ICallable* c)
{
#if 0
    return runBlockingFuture(new FutureTask<T>(c));
#endif
}

// WebViewProvider methods --------------------------------------------------------------------

//@Override
// BUG=6790250 |javaScriptInterfaces| was only ever used by the obsolete DumpRenderTree
// so is ignored. TODO: remove it from WebViewProvider.
ECode WebViewChromium::Init(
    /* [in] */ const IMap* javaScriptInterfaces,
    /* [in] */ const Boolean privateBrowsing)
{
#if 0
    if (privateBrowsing) {
        mFactory.startYourEngines(true);
        final String msg = "Private browsing is not supported in WebView.";
        if (mAppTargetSdkVersion >= Build.VERSION_CODES.KITKAT) {
            throw new IllegalArgumentException(msg);
        } else {
            Log.w(TAG, msg);
            TextView warningLabel = new TextView(mWebView.getContext());
            warningLabel.setText(mWebView.getContext().getString(
                    R.string.webviewchromium_private_browsing_warning));
            mWebView.addView(warningLabel);
        }
    }

    // We will defer real initialization until we know which thread to do it on, unless:
    // - we are on the main thread already (common case),
    // - the app is targeting >= JB MR2, in which case checkThread enforces that all usage
    //   comes from a single thread. (Note in JB MR2 this exception was in WebView.java).
    if (mAppTargetSdkVersion >= Build.VERSION_CODES.JELLY_BEAN_MR2) {
        mFactory.startYourEngines(false);
        checkThread();
    } else if (!mFactory.hasStarted()) {
        if (Looper.myLooper() == Looper.getMainLooper()) {
            mFactory.startYourEngines(true);
        }
    }

    final boolean isAccessFromFileURLsGrantedByDefault =
            mAppTargetSdkVersion < Build.VERSION_CODES.JELLY_BEAN;
    final boolean areLegacyQuirksEnabled =
            mAppTargetSdkVersion < Build.VERSION_CODES.KITKAT;

    mContentsClientAdapter = new WebViewContentsClientAdapter(mWebView);
    mWebSettings = new ContentSettingsAdapter(new AwSettings(
            mWebView.getContext(), isAccessFromFileURLsGrantedByDefault,
            areLegacyQuirksEnabled));

    if (mAppTargetSdkVersion <= Build.VERSION_CODES.KITKAT) {
        mWebSettings.setMixedContentMode(WebSettings.MIXED_CONTENT_ALWAYS_ALLOW);
        // On KK and older versions we always allowed third party cookies.
        mWebSettings.setAcceptThirdPartyCookies(true);
        mWebSettings.getAwSettings().setZeroLayoutHeightDisablesViewportQuirk(true);
    }

    AutoPtr<IRunnable> runnable = new InitRunnable(this, privateBrowsing);
    mRunQueue.addTask(runnable);
#endif
    return NOERROR;
}

// Wrap Context so that we can use resources from the webview resource apk.
AutoPtr<IContext> WebViewChromium::ResourcesContextWrapper(
    /* [in] */ const IContext* ctx)
{
#if 0
    return new ContextWrapper(ctx) {
        @Override
        public ClassLoader getClassLoader() {
            final ClassLoader appCl = getBaseContext().getClassLoader();
            final ClassLoader webViewCl = this.getClass().getClassLoader();
            return new ClassLoader() {
                @Override
                protected Class<?> findClass(String name) throws ClassNotFoundException {
                    // First look in the WebViewProvider class loader.
                    try {
                        return webViewCl.loadClass(name);
                    } catch (ClassNotFoundException e) {
                        // Look in the app class loader; allowing it to throw ClassNotFoundException.
                        return appCl.loadClass(name);
                    }
                }
            };
        }

        @Override
        public Object getSystemService(String name) {
            if (name.equals(Context.LAYOUT_INFLATER_SERVICE)) {
                LayoutInflater i = (LayoutInflater) getBaseContext().getSystemService(name);
                return i.cloneInContext(this);
            } else {
                return getBaseContext().getSystemService(name);
            }
        }

    };
#endif
}

void WebViewChromium::InitForReal()
{
#if 0
    Context ctx = resourcesContextWrapper(mWebView.getContext());
    mAwContents = new AwContents(mFactory.getBrowserContext(), mWebView, ctx,
            new InternalAccessAdapter(), new WebViewNativeGLDelegate(),
            mContentsClientAdapter, mWebSettings.getAwSettings());

    if (mAppTargetSdkVersion >= Build.VERSION_CODES.KITKAT) {
        // On KK and above, favicons are automatically downloaded as the method
        // old apps use to enable that behavior is deprecated.
        AwContents.setShouldDownloadFavicons();
    }

    AwContentsStatics.setRecordFullDocument(sRecordWholeDocumentEnabledByApi ||
            mAppTargetSdkVersion < Build.VERSION_CODES.L);

    if (mAppTargetSdkVersion <= Build.VERSION_CODES.KITKAT) {
        // On KK and older versions, JavaScript objects injected via addJavascriptInterface
        // were not inspectable.
        mAwContents.disableJavascriptInterfacesInspection();
    }

    // TODO: This assumes AwContents ignores second Paint param.
    mAwContents.setLayerType(mWebView.getLayerType(), null);
#endif
}

void WebViewChromium::StartYourEngine()
{
#if 0
    mRunQueue.drainQueue();
#endif
}

// private RuntimeException createThreadException() {
//     return new IllegalStateException(
//             "Calling View methods on another thread than the UI thread.");
// }

Boolean WebViewChromium::CheckNeedsPost()
{
#if 0
    boolean needsPost = !mFactory.hasStarted() || !ThreadUtils.runningOnUiThread();
    if (!needsPost && mAwContents == null) {
        throw new IllegalStateException(
                "AwContents must be created if we are not posting!");
    }
    return needsPost;
#endif
}

//  Intentionally not static, as no need to check thread on static methods
void WebViewChromium::CheckThread()
{
#if 0
    if (!ThreadUtils.runningOnUiThread()) {
        final RuntimeException threadViolation = createThreadException();
        ThreadUtils::PostOnUiThread(new Runnable() {
            @Override
            public void run() {
                throw threadViolation;
            }
        });
        throw createThreadException();
    }
#endif
}

//@Override
ECode WebViewChromium::SetHorizontalScrollbarOverlay(
    /* [in] */ const Boolean overlay)
{
#if 0
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new SetHorizontalScrollbarOverlayRunnable(this, overlay);
        mRunQueue.addTask(runnable);
        return;
    }
    mAwContents.setHorizontalScrollbarOverlay(overlay);
#endif
    return NOERROR;
}

//@Override
ECode WebViewChromium::SetVerticalScrollbarOverlay(
    /* [in] */ const Boolean overlay)
{
#if 0
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new SetVerticalScrollbarOverlayRunnable(this, overlay);
        mRunQueue.addTask(runnable);
        return;
    }
    mAwContents.setVerticalScrollbarOverlay(overlay);
#endif
    return NOERROR;
}

//@Override
ECode WebViewChromium::OverlayHorizontalScrollbar(
    /* [out] */ Boolean* result)
{
#if 0
    mFactory.startYourEngines(false);
    if (checkNeedsPost()) {
        AutoPtr<ICallable> callable = new OverlayHorizontalScrollbarRunnable(this);
        boolean ret = runOnUiThreadBlocking(callable);
        return ret;
    }
    return mAwContents.overlayHorizontalScrollbar();
#endif
}

//@Override
ECode WebViewChromium::OverlayVerticalScrollbar(
    /* [out] */ Boolean* result)
{
#if 0
    mFactory.startYourEngines(false);
    if (checkNeedsPost()) {
        AutoPtr<ICallable> callable = new OverlayVerticalScrollbarCallable(this);
        Boolean ret = runOnUiThreadBlocking(callable);
        return ret;
    }
    return mAwContents.overlayVerticalScrollbar();
#endif
}

//@Override
ECode WebViewChromium::GetVisibleTitleHeight(
    /* [out] */ Int32* height)
{
    // This is deprecated in WebView and should always return 0.
    VALIDATE_NOT_NULL(height);
    *height = 0;
    return NOERROR;
}

//@Override
ECode WebViewChromium::GetCertificate(
    /* [out] */ ISslCertificate** ssl)
{
#if 0
    mFactory.startYourEngines(true);
    if (checkNeedsPost()) {
        AutoPtr<ICallable> callable = new GetCertificateCallable(this);
        SslCertificate ret = runOnUiThreadBlocking(callable);
        return ret;
    }
    return mAwContents.getCertificate();
#endif
}

//@Override
ECode WebViewChromium::SetCertificate(
    /* [in] */ ISslCertificate* certificate)
{
    // intentional no-op
    return NOERROR;
}

//@Override
ECode WebViewChromium::SavePassword(
    /* [in] */ const String& host,
    /* [in] */ const String& username,
    /* [in] */ const String& password)
{
    // This is a deprecated API: intentional no-op.
    return NOERROR;
}

//@Override
ECode WebViewChromium::SetHttpAuthUsernamePassword(
    /* [in] */ const String& host,
    /* [in] */ const String& realm,
    /* [in] */ const String& username,
    /* [in] */ const String& password)
{
#if 0
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new SetHttpAuthUsernamePasswordRunnable(this, host, realm, username, password);
        mRunQueue.addTask(runnable);
        return;
    }
    mAwContents.setHttpAuthUsernamePassword(host, realm, username, password);

    return NOERROR;
#endif
}

//@Override
ECode WebViewChromium::GetHttpAuthUsernamePassword(
    /* [in] */ const String& host,
    /* [in] */ const String& realm,
    /* [out] */ ArrayOf<String>** up)
{
#if 0
    mFactory.startYourEngines(true);
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> callable = new GetHttpAuthUsernamePasswordCallable(this, host, realm);
        String[] ret = runOnUiThreadBlocking(callable);
        return ret;
    }
    return mAwContents.getHttpAuthUsernamePassword(host, realm);
#endif
}

//@Override
ECode WebViewChromium::Destroy()
{
#if 0
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new DestroyRunnable(this);
        mRunQueue.addTask(runnable);
        return;
    }

    mAwContents.destroy();
    if (mGLfunctor != null) {
        mGLfunctor.destroy();
        mGLfunctor = null;
    }

    return NOERROR;
#endif
}

//@Override
ECode WebViewChromium::SetNetworkAvailable(
    /* [in] */ const Boolean networkUp)
{
#if 0
    // Note that this purely toggles the JS navigator.online property.
    // It does not in affect chromium or network stack state in any way.
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new SetNetworkAvailableRunnable(this, networkUp);
        mRunQueue.addTask(runnable);
        return;
    }
    mAwContents.setNetworkAvailable(networkUp);

    return NOERROR;
#endif
}

//@Override
ECode WebViewChromium::SaveState(
    /* [in] */ const IBundle* outState,
    /* [out] */ IWebBackForwardList** wfl)
{
#if 0
    mFactory.startYourEngines(true);
    if (checkNeedsPost()) {
        AutoPtr<ICallable> callable = new SaveStateCallable(this, outState);
        WebBackForwardList ret = runOnUiThreadBlocking(callable);
        return ret;
    }
    if (outState == null) return null;
    if (!mAwContents.saveState(outState)) return null;
    return copyBackForwardList();
#endif
}

//@Override
ECode WebViewChromium::SavePicture(
    /* [in] */ IBundle* b,
    /* [in] */ IFile* dest,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // Intentional no-op: hidden method on WebView.
    *result = FALSE;
    return NOERROR;
}

//@Override
ECode WebViewChromium::RestorePicture(
    /* [in] */ IBundle* b,
    /* [in] */ IFile* src,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // Intentional no-op: hidden method on WebView.
    *result = FALSE;
    return NOERROR;
}

//@Override
ECode WebViewChromium::RestoreState(
    /* [in] */ const IBundle* inState,
    /* [out] */ IWebBackForwardList** wfl)
{
#if 0
    mFactory.startYourEngines(true);
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new RestoreStateCallable(this, inState);
        WebBackForwardList ret = runOnUiThreadBlocking(runnable);
        return ret;
    }
    if (inState == null) return null;
    if (!mAwContents.restoreState(inState)) return null;
    return copyBackForwardList();
#endif
}

//@Override
ECode WebViewChromium::LoadUrl(
    /* [in] */ const String& url,
    /* [in] */ IMap* additionalHttpHeaders)
{
#if 0
    // TODO: We may actually want to do some sanity checks here (like filter about://chrome).

    // For backwards compatibility, apps targeting less than K will have JS URLs evaluated
    // directly and any result of the evaluation will not replace the current page content.
    // Matching Chrome behavior more closely; apps targetting >= K that load a JS URL will
    // have the result of that URL replace the content of the current page.
    final String JAVASCRIPT_SCHEME = "javascript:";
    if (mAppTargetSdkVersion < Build.VERSION_CODES.KITKAT &&
            url != null && url.startsWith(JAVASCRIPT_SCHEME)) {
        mFactory.startYourEngines(true);
        if (checkNeedsPost()) {
            AutoPtr<IRunnable> runnable = new LoadUrlRunnable(this, url, JAVASCRIPT_SCHEME);
            mRunQueue.addTask(runnable);
        } else {
            mAwContents.evaluateJavaScriptEvenIfNotYetNavigated(
                    url.substring(JAVASCRIPT_SCHEME.length()));
        }
        return;
    }

    LoadUrlParams params = new LoadUrlParams(url);
    if (additionalHttpHeaders != null) params.setExtraHeaders(additionalHttpHeaders);
    loadUrlOnUiThread(params);
#endif
}

//@Override
ECode WebViewChromium::LoadUrl(
    /* [in] */ const String& url)
{
    // Early out to match old WebView implementation
    if (url == NULL) {
        return NOERROR;
    }

    LoadUrl(url, NULL);

    return NOERROR;
}

//@Override
ECode WebViewChromium::PostUrl(
    /* [in] */ const String& url,
    /* [in] */ ArrayOf<Byte>* postData)
{
#if 0
    LoadUrlParams params = LoadUrlParams.createLoadHttpPostParams(url, postData);
    Map<String,String> headers = new HashMap<String,String>();
    headers.put("Content-Type", "application/x-www-form-urlencoded");
    params.setExtraHeaders(headers);
    loadUrlOnUiThread(params);
#endif
}

String WebViewChromium::FixupMimeType(
    /* [in] */ const String& mimeType)
{
    return TextUtils::IsEmpty(mimeType) ? String("text/html") : mimeType;
}

String WebViewChromium::FixupData(
    /* [in] */ const String& data)
{
    return TextUtils::IsEmpty(data) ? String("") : data;
}

String WebViewChromium::FixupBase(
    /* [in] */ const String& url)
{
    return TextUtils::IsEmpty(url) ? String("about:blank") : url;
}

String WebViewChromium::FixupHistory(
    /* [in] */ const String& url)
{
    return TextUtils::IsEmpty(url) ? String("about:blank") : url;
}

Boolean WebViewChromium::IsBase64Encoded(
    /* [in] */ const String& encoding)
{
    return String("base64").Equals(encoding);
}

//@Override
ECode WebViewChromium::LoadData(
    /* [in] */ const String& data,
    /* [in] */ const String& mimeType,
    /* [in] */ const String& encoding)
{
#if 0
    loadUrlOnUiThread(LoadUrlParams.createLoadDataParams(
            fixupData(data), fixupMimeType(mimeType), isBase64Encoded(encoding)));
#endif
}

//@Override
ECode WebViewChromium::LoadDataWithBaseURL(
    /* [in] */ const String& baseUrl,
    /* [in] */ const String& data,
    /* [in] */ const String& mimeType,
    /* [in] */ const String& encoding,
    /* [in] */ const String& historyUrl)
{
#if 0
    data = fixupData(data);
    mimeType = fixupMimeType(mimeType);
    LoadUrlParams loadUrlParams;
    baseUrl = fixupBase(baseUrl);
    historyUrl = fixupHistory(historyUrl);

    if (baseUrl.startsWith("data:")) {
        // For backwards compatibility with WebViewClassic, we use the value of |encoding|
        // as the charset, as long as it's not "base64".
        boolean isBase64 = isBase64Encoded(encoding);
        loadUrlParams = LoadUrlParams.createLoadDataParamsWithBaseUrl(
                data, mimeType, isBase64, baseUrl, historyUrl, isBase64 ? null : encoding);
    } else {
        // When loading data with a non-data: base URL, the classic WebView would effectively
        // "dump" that string of data into the WebView without going through regular URL
        // loading steps such as decoding URL-encoded entities. We achieve this same behavior by
        // base64 encoding the data that is passed here and then loading that as a data: URL.
        try {
            loadUrlParams = LoadUrlParams.createLoadDataParamsWithBaseUrl(
                    Base64.encodeToString(data.getBytes("utf-8"), Base64.DEFAULT), mimeType,
                    true, baseUrl, historyUrl, "utf-8");
        } catch (java.io.UnsupportedEncodingException e) {
            Log.wtf(TAG, "Unable to load data string " + data, e);
            return;
        }
    }
    loadUrlOnUiThread(loadUrlParams);
#endif
}

void WebViewChromium::LoadUrlOnUiThread(
    /* [in] */ const LoadUrlParams* loadUrlParams)
{
#if 0
    // This is the last point that we can delay starting the Chromium backend up
    // and if the app has not caused us to bind the Chromium UI thread to a background thread
    // we now bind Chromium's notion of the UI thread to the app main thread.
    mFactory.startYourEngines(true);
    if (checkNeedsPost()) {
        // Disallowed in WebView API for apps targetting a new SDK
        assert mAppTargetSdkVersion < Build.VERSION_CODES.JELLY_BEAN_MR2;
        AutoPtr<IRunnable> runnable = new LoadUrlOnUiThreadRunnable(this, loadUrlParams);
        mRunQueue.addTask(runnable);
        return;
    }
    mAwContents.loadUrl(loadUrlParams);
#endif
}

void WebViewChromium::EvaluateJavaScript(
    /* [in] */ const String& script,
    /* [in] */ IValueCallback* resultCallback)
{
    CheckThread();
    mAwContents->EvaluateJavaScript(script, resultCallback);
}

//@Override
ECode WebViewChromium::SaveWebArchive(
    /* [in] */ const String& filename)
{
    return SaveWebArchive(filename, FALSE, NULL);
}

//@Override
ECode WebViewChromium::SaveWebArchive(
    /* [in] */ const String& basename,
    /* [in] */ const Boolean autoname,
    /* [in] */ IValueCallback* callback)
{
#if 0
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new SaveWebArchiveRunnable(this, basename, autoname, callback);
        mRunQueue.addTask(runnable);
        return;
    }
    mAwContents.saveWebArchive(basename, autoname, callback);
#endif
}

//@Override
ECode WebViewChromium::StopLoading()
{
#if 0
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new StopLoadingRunnable(this);
        mRunQueue.addTask(runnable);
        return;
    }

    mAwContents.stopLoading();
#endif
}

//@Override
ECode WebViewChromium::Reload()
{
#if 0
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new ReloadRunnable(this);
        mRunQueue.addTask(runnable);
        return;
    }
    mAwContents.reload();
#endif
}

//@Override
ECode WebViewChromium::CanGoBack(
    /* [out] */ Boolean* result)
{
#if 0
    mFactory.startYourEngines(true);
    if (checkNeedsPost()) {
        AutoPtr<ICallable> callable = new CanGoBackCallable(this);
        Boolean ret = runOnUiThreadBlocking(callable);
        return ret;
    }
    return mAwContents.canGoBack();
#endif
}

//@Override
ECode WebViewChromium::GoBack()
{
#if 0
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new GoBackRunnable(this);
        mRunQueue.addTask(runnable);
        return;
    }
    mAwContents.goBack();
#endif
}

//@Override
ECode WebViewChromium::CanGoForward(
    /* [out] */ Boolean* result)
{
    mFactory.startYourEngines(true);
    if (checkNeedsPost()) {
        AutoPtr<ICallable> callable = new CanGoForwardCallable(this);
        Boolean ret = runOnUiThreadBlocking(callable);
        return ret;
    }
    return mAwContents.canGoForward();
}

//@Override
ECode WebViewChromium::GoForward()
{
#if 0
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new GoForwardRunnable(this);
        mRunQueue.addTask(runnable);
        return;
    }
    mAwContents.goForward();
#endif
}

//@Override
ECode WebViewChromium::CanGoBackOrForward(
    /* [in] */ const Int32 steps,
    /* [out] */ Boolean* result)
{
#if 0
    mFactory.startYourEngines(true);
    if (checkNeedsPost()) {
        AutoPtr<ICallable> callable = new CanGoBackOrForwardCallable(this, steps);
        Boolean ret = runOnUiThreadBlocking(callable);
        return ret;
    }
    return mAwContents.canGoBackOrForward(steps);
#endif
}

//@Override
ECode WebViewChromium::GoBackOrForward(
    /* [in] */ const Int32 steps)
{
#if 0
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new GoBackOrForwardRunnable(this, steps);
        mRunQueue.addTask(runnable);
        return;
    }
    mAwContents.goBackOrForward(steps);
#endif
}

//@Override
ECode WebViewChromium::IsPrivateBrowsingEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // Not supported in this WebView implementation.
    *result = FALSE;
    return NOERROR;
}

//@Override
ECode WebViewChromium::PageUp(
    /* [in] */ const Boolean top,
    /* [out] */ Boolean* result)
{
#if 0
    mFactory.startYourEngines(true);
    if (checkNeedsPost()) {
        AutoPtr<ICallable> callable = new PageUpCallable(this, top);
        Boolean ret = runOnUiThreadBlocking(runnable);
        return ret;
    }
    return mAwContents.pageUp(top);
#endif
}

//@Override
ECode WebViewChromium::PageDown(
    /* [in] */ const Boolean bottom,
    /* [out] */ Boolean* result)
{
#if 0
    mFactory.startYourEngines(true);
    if (checkNeedsPost()) {
        AutoPtr<ICallable> callable = new PageDownCallable(this, bottom);
        Boolean ret = runOnUiThreadBlocking(callable);
        return ret;
    }
    return mAwContents.pageDown(bottom);
#endif
}

//@Override
ECode WebViewChromium::ClearView()
{
#if 0
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new ClearViewRunnable(this);
        mRunQueue.addTask(runnable);
        return;
    }
    mAwContents.clearView();
#endif
}

//@Override
ECode WebViewChromium::CapturePicture(
    /* [out] */ IPicture** pic)
{
#if 0
    mFactory.startYourEngines(true);
    if (checkNeedsPost()) {
        AutoPtr<ICallable> callable = new CapturePictureCallable(this);
        Picture ret = runOnUiThreadBlocking(callable);
        return ret;
    }
    return mAwContents.capturePicture();
#endif
}

//@Override
ECode WebViewChromium::GetScale(
    /* [out] */ Float* scale)
{
#if 0
    // No checkThread() as it is mostly thread safe (workaround for b/10652991).
    mFactory.startYourEngines(true);
    return mAwContents.getScale();
#endif
}

//@Override
ECode WebViewChromium::SetInitialScale(
    /* [in] */ const Int32 scaleInPercent)
{
#if 0
    // No checkThread() as it is thread safe
    mWebSettings.getAwSettings().setInitialPageScale(scaleInPercent);
#endif
}

//@Override
ECode WebViewChromium::InvokeZoomPicker()
{
#if 0
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new InvokeZoomPickerRunnable(this);
        mRunQueue.addTask(runnable);
        return;
    }
    mAwContents.invokeZoomPicker();
#endif
}

//@Override
ECode WebViewChromium::GetHitTestResult(
    /* [out] */ IWebViewHitTestResult** result)
{
#if 0
    mFactory.startYourEngines(true);
    if (checkNeedsPost()) {
        AutoPtr<ICallable> callable = new GetHitTestResultCallable(this);
        WebView.HitTestResult ret = runOnUiThreadBlocking(callable);
        return ret;
    }
    AwContents.HitTestData data = mAwContents.getLastHitTestResult();
    mHitTestResult.setType(data.hitTestResultType);
    mHitTestResult.setExtra(data.hitTestResultExtraData);
    return mHitTestResult;
#endif
}

//@Override
ECode WebViewChromium::RequestFocusNodeHref(
    /* [in] */ const IMessage* hrefMsg)
{
#if 0
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new RequestFocusNodeHrefRunnable(this, hrefMsg);
        mRunQueue.addTask(runnable);
        return;
    }
    mAwContents.requestFocusNodeHref(hrefMsg);
#endif
}

//@Override
ECode WebViewChromium::RequestImageRef(
    /* [in] */ const IMessage* msg)
{
#if 0
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new RequestImageRefRunnable(this, msg);
        mRunQueue.addTask(runnable);
        return;
    }
    mAwContents.requestImageRef(msg);
#endif
}

//@Override
ECode WebViewChromium::GetUrl(
    /* [out] */ String* url)
{
#if 0
    mFactory.startYourEngines(true);
    if (checkNeedsPost()) {
        AutoPtr<ICallable> callable = new GetUrlCallable(this);
        String ret = runOnUiThreadBlocking(callable);
        return ret;
    }
    String url =  mAwContents.getUrl();
    if (url == null || url.trim().isEmpty()) return null;
    return url;
#endif
}

//@Override
ECode WebViewChromium::GetOriginalUrl(
    /* [out] */ String* url)
{
#if 0
    mFactory.startYourEngines(true);
    if (checkNeedsPost()) {
        AutoPtr<ICallable> callable = new GetOriginalUrlCallable(this);
        String ret = runOnUiThreadBlocking(callable);
        return ret;
    }
    String url =  mAwContents.getOriginalUrl();
    if (url == null || url.trim().isEmpty()) return null;
    return url;
#endif
}

//@Override
ECode WebViewChromium::GetTitle(
    /* [out] */ String* title)
{
#if 0
    mFactory.startYourEngines(true);
    if (checkNeedsPost()) {
        AutoPtr<ICallable> callable = new GetTitleCallable(this);
        String ret = runOnUiThreadBlocking(callable);
        return ret;
    }
    return mAwContents.getTitle();
#endif
}

//@Override
ECode WebViewChromium::GetFavicon(
    /* [out] */ IBitmap** bitmap)
{
#if 0
    mFactory.startYourEngines(true);
    if (checkNeedsPost()) {
        AutoPtr<ICallable> callable = new GetFaviconCallable(this);
        Bitmap ret = runOnUiThreadBlocking(callable);
        return ret;
    }
    return mAwContents.getFavicon();
#endif
}

//@Override
ECode WebViewChromium::GetTouchIconUrl(
    /* [out] */ String* url)
{
    VALIDATE_NOT_NULL(url);
    // Intentional no-op: hidden method on WebView.
    *url = NULL;
    return NOERROR;
}

//@Override
ECode WebViewChromium::GetProgress(
    /* [out] */ Int32* progress)
{
    VALIDATE_NOT_NULL(progress);
    if (mAwContents == NULL) {
        *progress = 100;
        return NOERROR;
    }

    // No checkThread() because the value is cached java side (workaround for b/10533304).
    *result = mAwContents->GetMostRecentProgress();
    return NOERROR;
}

//@Override
ECode WebViewChromium::GetContentHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    if (mAwContents == NULL) {
        *height = 0;
        return NOERROR;
    }

    // No checkThread() as it is mostly thread safe (workaround for b/10594869).
    *height = mAwContents->GetContentHeightCss();

    return NOERROR;
}

//@Override
ECode WebViewChromium::GetContentWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);

    if (mAwContents == NULL) {
        *width = 0;
        return NOERROR;
    }

    // No checkThread() as it is mostly thread safe (workaround for b/10594869).
    *width = mAwContents->GetContentWidthCss();

    return NOERROR;
}

//@Override
ECode WebViewChromium::PauseTimers()
{
#if 0
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new PauseTimersRunnable(this);
        mRunQueue.addTask(runnable);
        return;
    }
    mAwContents.pauseTimers();
#endif
}

//@Override
void WebViewChromium::ResumeTimers()
{
#if 0
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new ResumeTimersRunnable(this);
        mRunQueue.addTask(runnable);
        return;
    }
    mAwContents.resumeTimers();
#endif
}

//@Override
void WebViewChromium::OnPause()
{
#if 0
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new OnPauseRunnable(this);
        mRunQueue.addTask(runnable);
        return;
    }
    mAwContents.onPause();
#endif
}

//@Override
ECode WebViewChromium::OnResume()
{
#if 0
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new OnResumeRunnable(this);
        mRunQueue.addTask(runnable);
        return;
    }
    mAwContents.onResume();
#endif
}

//@Override
ECode WebViewChromium::IsPaused(
    /* [out] */ Boolean* result)
{
#if 0
    mFactory.startYourEngines(true);
    if (checkNeedsPost()) {
        AutoPtr<ICallable> callable = new IsPausedCallable(this);
        Boolean ret = runOnUiThreadBlocking(callable);
        return ret;
    }
    return mAwContents.isPaused();
#endif
}

//@Override
ECode WebViewChromium::FreeMemory()
{
    // Intentional no-op. Memory is managed automatically by Chromium.
    return NOERROR;
}

//@Override
ECode WebViewChromium::ClearCache(
    /* [in] */ const Boolean includeDiskFiles)
{
#if 0
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new ClearCacheRunnable(this, includeDiskFiles);
        mRunQueue.addTask(runnable);
        return;
    }
    mAwContents.clearCache(includeDiskFiles);
#endif
}

/**
 * This is a poorly named method, but we keep it for historical reasons.
 */
//@Override
ECode WebViewChromium::ClearFormData()
{
#if 0
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new ClearFormDataRunnable(this);
        mRunQueue.addTask(runnable);
        return;
    }
    mAwContents.hideAutofillPopup();
#endif
}

//@Override
ECode WebViewChromium::ClearHistory()
{
#if 0
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new ClearHistoryRunnable(this);
        mRunQueue.addTask(runnable);
        return;
    }
    mAwContents.clearHistory();
#endif
}

//@Override
ECode WebViewChromium::ClearSslPreferences()
{
#if 0
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new ClearSslPreferencesRunnable(this);
        mRunQueue.addTask(runnable);
        return;
    }
    mAwContents.clearSslPreferences();
#endif
}

//@Override
ECode WebViewChromium::CopyBackForwardList(
    /* [out] */ IWebBackForwardList** wfl)
{
#if 0
    mFactory.startYourEngines(true);
    if (checkNeedsPost()) {
        AutoPtr<ICallable> callable = new CopyBackForwardListCallable(this);
        WebBackForwardList ret = runOnUiThreadBlocking(callable);
        return ret;
    }
    return new WebBackForwardListChromium(
            mAwContents.getNavigationHistory());
#endif
}

//@Override
ECode WebViewChromium::SetFindListener(
    /* [in] */ IWebViewFindListener* listener)
{
    mContentsClientAdapter->SetFindListener(listener);
    return NOERROR;
}

//@Override
ECode WebViewChromium::FindNext(
    /* [in] */ const Boolean forwards)
{
#if 0
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new FindNextRunnable(this, forwards);
        mRunQueue.addTask(runnable);
        return;
    }
    mAwContents.findNext(forwards);
#endif
}

//@Override
ECode WebViewChromium::FindAll(
    /* [in] */ const String& searchString,
    /* [out] */ Int32* all)
{
    VALIDATE_NOT_NULL(all);
    FindAllAsync(searchString);
    *all = 0;
    return NOERROR;
}

//@Override
ECode WebViewChromium::FindAllAsync(
    /* [in] */ const String searchString)
{
#if 0
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new FindAllAsyncRunnable(this, searchString);
        mRunQueue.addTask(runnable);
        return;
    }
    mAwContents.findAllAsync(searchString);
#endif
}

//@Override
ECode WebViewChromium::ShowFindDialog(
    /* [in] */ const String& text,
    /* [in] */ const Boolean showIme,
    /* [out] */ Boolean* result)
{
#if 0
    mFactory.startYourEngines(false);
    if (checkNeedsPost()) {
        return false;
    }
    if (mWebView.getParent() == null) {
        return false;
    }

    FindActionModeCallback findAction = new FindActionModeCallback(mWebView.getContext());
    if (findAction == null) {
        return false;
    }

    mWebView.startActionMode(findAction);
    findAction.setWebView(mWebView);
    if (showIme) {
        findAction.showSoftInput();
    }

    if (text != null) {
        findAction.setText(text);
        findAction.findAll();
    }

    return true;
#endif
}

//@Override
ECode WebViewChromium::NotifyFindDialogDismissed()
{
#if 0
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new NotifyFindDialogDismissedRunnable(this);
        mRunQueue.addTask(runnable);
        return;
    }
    clearMatches();
#endif
}

//@Override
ECode WebViewChromium::ClearMatches()
{
#if 0
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new ClearMatchesRunnable(this);
        mRunQueue.addTask(runnable);
        return;
    }
    mAwContents.clearMatches();
#endif
}

//@Override
ECode WebViewChromium::DocumentHasImages(
    /* [in] */ const IMessage* response)
{
#if 0
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new DocumentHasImagesRunnable(this, response);
        mRunQueue.addTask(runnable);
        return;
    }
    mAwContents.documentHasImages(response);
#endif
}

//@Override
ECode WebViewChromium::SetWebViewClient(
    /* [in] */ IWebViewClient* client)
{
    mContentsClientAdapter->SetWebViewClient(client);
    return NOERROR;
}

//@Override
ECode WebViewChromium::SetDownloadListener(
    /* [in] */ IDownloadListener* listener)
{
    mContentsClientAdapter->SetDownloadListener(listener);
    return NOERROR;
}

//@Override
ECode WebViewChromium::SetWebChromeClient(
    /* [in] */ IWebChromeClient* client)
{
#if 0
    mWebSettings.getAwSettings().setFullscreenSupported(doesSupportFullscreen(client));
    mContentsClientAdapter.setWebChromeClient(client);
#endif
}

/**
 * Returns true if the supplied {@link WebChromeClient} supports fullscreen.
 *
 * <p>For fullscreen support, implementations of {@link WebChromeClient#onShowCustomView}
 * and {@link WebChromeClient#onHideCustomView()} are required.
 */
Boolean WebViewChromium::DoesSupportFullscreen(
    /* [in] */ IWebChromeClient* client)
{
#if 0
    if (client == null) {
        return false;
    }
    // If client is not a subclass of WebChromeClient then the methods have not been
    // implemented because WebChromeClient has empty implementations.
    if (client.getClass().isAssignableFrom(WebChromeClient.class)) {
        return false;
    }
    try {
        client.getClass().getDeclaredMethod("onShowCustomView", View.class,
                CustomViewCallback.class);
        client.getClass().getDeclaredMethod("onHideCustomView");
        return true;
    } catch (NoSuchMethodException e) {
        return false;
    }
#endif
}

//@Override
ECode WebViewChromium::SetPictureListener(
    /* [in] */ const IWebViewPictureListener* listener)
{
#if 0
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new SetPictureListenerRunnable(this, listener);
        mRunQueue.addTask(runnable);
        return;
    }
    mContentsClientAdapter.setPictureListener(listener);
    mAwContents.enableOnNewPicture(listener != null,
            mAppTargetSdkVersion >= Build.VERSION_CODES.JELLY_BEAN_MR2);
#endif
}

//@Override
ECode WebViewChromium::AddJavascriptInterface(
    /* [in] */ IInterface* obj,
    /* [in] */ const String& interfaceName)
{
#if 0
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new AddJavascriptInterfaceRunnable(this, obj, interfaceName);
        mRunQueue.addTask(runnable);
        return;
    }
    Class<? extends Annotation> requiredAnnotation = null;
    if (mAppTargetSdkVersion >= Build.VERSION_CODES.JELLY_BEAN_MR1) {
       requiredAnnotation = JavascriptInterface.class;
    }
    mAwContents.addPossiblyUnsafeJavascriptInterface(obj, interfaceName, requiredAnnotation);
#endif
}

//@Override
ECode WebViewChromium::RemoveJavascriptInterface(
    /* [in] */ const String& interfaceName)
{
#if 0
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new RemoveJavascriptInterfaceRunnable(this, interfaceName);
        mRunQueue.addTask(runnable);
        return;
    }
    mAwContents.removeJavascriptInterface(interfaceName);
#endif
}

//@Override
ECode WebViewChromium::GetSettings(
    /* [out] */ IWebSettings** ws)
{
    VALIDATE_NOT_NULL(ws);
    *ws = mWebSettings;
    return NOERROR;
}

//@Override
ECode WebViewChromium::SetMapTrackballToArrowKeys(
    /* [in] */ Boolean setMap)
{
    // This is a deprecated API: intentional no-op.
    return NOERROR;
}

//@Override
ECode WebViewChromium::FlingScroll(
    /* [in] */ const Int32 vx,
    /* [in] */ const Int32 vy)
{
#if 0
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new FlingScrollRunnable(this, vx, vy);
        mRunQueue.addTask(runnable);
        return;
    }
    mAwContents.flingScroll(vx, vy);
#endif
}

//@Override
ECode WebViewChromium::GetZoomControls(
    /* [out] */ IView** view)
{
#if 0
    mFactory.startYourEngines(false);
    if (checkNeedsPost()) {
        return null;
    }

    // This was deprecated in 2009 and hidden in JB MR1, so just provide the minimum needed
    // to stop very out-dated applications from crashing.
    Log.w(TAG, "WebView doesn't support getZoomControls");
    return mAwContents.getSettings().supportZoom() ? new View(mWebView.getContext()) : null;
#endif
}

//@Override
ECode WebViewChromium::CanZoomIn(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (CheckNeedsPost()) {
        return FALSE;
    }

    *result = mAwContents->CanZoomIn();

    return NOERROR;
}

//@Override
ECode WebViewChromium::CanZoomOut(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (CheckNeedsPost()) {
        *result = FALSE;
        return NOERROR;
    }

    *result = mAwContents->CanZoomOut();

    return NOERROR;
}

//@Override
ECode WebViewChromium::ZoomIn(
    /* [out] */ Boolean* result)
{
#if 0
    mFactory.startYourEngines(true);
    if (checkNeedsPost()) {
        AutoPtr<ICallable> callable = new ZoomInCallable(this);
        boolean ret = runOnUiThreadBlocking(callable);
        return ret;
    }
    return mAwContents.zoomIn();
#endif
}

//@Override
ECode WebViewChromium::ZoomOut(
    /* [out] */ Boolean* result)
{
#if 0
    mFactory.startYourEngines(true);
    if (checkNeedsPost()) {
        AutoPtr<ICallable> callable = new ZoomOutCallable(this);
        boolean ret = runOnUiThreadBlocking(callable);
        return ret;
    }
    return mAwContents.zoomOut();
#endif
}

//@Override
ECode WebViewChromium::ZoomBy(
    /* [in] */ Float factor,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    mFactory->StartYourEngines(TRUE);
    // This is an L API and therefore we can enforce stricter threading constraints.
    CheckThread();
    *result = mAwContents->ZoomBy(factor);

    return NOERROR;
}

//@Override
ECode WebViewChromium::DumpViewHierarchyWithProperties(
    /* [in] */ IBufferedWriter* out,
    /* [in] */ Int32 level)
{
    // Intentional no-op
    return NOERROR;
}

//@Override
ECode WebViewChromium::FindHierarchyView(
    /* [in] */ const String& className,
    /* [in] */ Int32 hashCode,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    // Intentional no-op
    *view = NULL;
    return NOERROR;
}

// WebViewProvider glue methods ---------------------------------------------------------------

//@Override
// This needs to be kept thread safe!
ECode WebViewChromium::GetViewDelegate(
    /* [out] */ IWebViewProviderViewDelegate** delegate)
{
    VALIDATE_NOT_NULL(delegate);
    *delegate = this;
    return NOERROR;
}

//@Override
// This needs to be kept thread safe!
ECode WebViewChromium::GetScrollDelegate(
    /* [out] */ IWebViewProviderScrollDelegate** delegate)
{
    VALIDATE_NOT_NULL(delegate);
    *delegate = this;
    return NOERROR;
}

// WebViewProvider.ViewDelegate implementation ------------------------------------------------

// TODO: remove from WebViewProvider and use default implementation from
// ViewGroup.
// @Override
ECode WebViewChromium::ShouldDelayChildPressedState(
    /* [out] */ Boolean* state)
{
#if 0
    mFactory.startYourEngines(false);
    if (checkNeedsPost()) {
        AutoPtr<ICallable> callable = new ShouldDelayChildPressedStateCallable(this);
        boolean ret = runOnUiThreadBlocking(callable);
        return ret;
    }
    return true;
#endif
}

// @Override
ECode WebViewChromium::GetAccessibilityNodeProvider(
    /* [out] */ IAccessibilityNodeProvider** provider)
{
#if 0
    mFactory.startYourEngines(false);
    if (checkNeedsPost()) {
        AutoPtr<ICallable> callable = new GetAccessibilityNodeProviderCallable(this);
        AccessibilityNodeProvider ret = runOnUiThreadBlocking(callable);
        return ret;
    }
    return mAwContents.getAccessibilityNodeProvider();
#endif
}

//@Override
ECode WebViewChromium::OnInitializeAccessibilityNodeInfo(
    /* [in] */ const IAccessibilityNodeInfo* info)
{
#if 0
    mFactory.startYourEngines(false);
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new OnInitializeAccessibilityNodeInfoRunnable(this, info);
        runVoidTaskOnUiThreadBlocking(runnable);
        return;
    }
    mAwContents.onInitializeAccessibilityNodeInfo(info);
#endif
}

//@Override
ECode WebViewChromium::OnInitializeAccessibilityEvent(
    /* [in] */ const IAccessibilityEvent* event)
{
#if 0
    mFactory.startYourEngines(false);
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new OnInitializeAccessibilityEventRunnable(this, event);
        runVoidTaskOnUiThreadBlocking(runnable);
        return;
    }
    mAwContents.onInitializeAccessibilityEvent(event);
#endif
}

//@Override
ECode WebViewChromium::PerformAccessibilityAction(
    /* [in] */ const Int32 action,
    /* [in] */ const IBundle* arguments,
    /* [out] */ Boolean* result)
{
#if 0
    mFactory.startYourEngines(false);
    if (checkNeedsPost()) {
        AutoPtr<ICallable> callable = new PerformAccessibilityActionCallable(this, action, arguments);
        boolean ret = runOnUiThreadBlocking(callable);
        return ret;
    }
    if (mAwContents.supportsAccessibilityAction(action)) {
        return mAwContents.performAccessibilityAction(action, arguments);
    }
    return mWebViewPrivate.super_performAccessibilityAction(action, arguments);
#endif
}

//@Override
ECode WebViewChromium::SetOverScrollMode(
    /* [in] */ const Int32 mode)
{
#if 0
    // This gets called from the android.view.View c'tor that WebView inherits from. This
    // causes the method to be called when mAwContents == null.
    // It's safe to ignore these calls however since AwContents will read the current value of
    // this setting when it's created.
    if (mAwContents == null) return;

    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new SetOverScrollModeRunnable(this, mode);
        mRunQueue.addTask(runnable);
        return;
    }
    mAwContents.setOverScrollMode(mode);
#endif
}

//@Override
ECode WebViewChromium::SetScrollBarStyle(
    /* [in] */ const Int32 style)
{
#if 0
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new SetScrollBarStyleRunnable(this, style);
        mRunQueue.addTask(runnable);
        return;
    }
    mAwContents.setScrollBarStyle(style);
#endif
}

//@Override
ECode WebViewChromium::OnDrawVerticalScrollBar(
    /* [in] */ const ICanvas* canvas,
    /* [in] */ const IDrawable* scrollBar,
    /* [in] */ const Int32 l,
    /* [in] */ const Int32 t,
    /* [in] */ const Int32 r,
    /* [in] */ const Int32 b)
{
    // WebViewClassic was overriding this method to handle rubberband over-scroll. Since
    // WebViewChromium doesn't support that the vanilla implementation of this method can be
    // used.
    mWebViewPrivate->Super_onDrawVerticalScrollBar(canvas, scrollBar, l, t, r, b);
    return NOERROR;
}

//@Override
ECode WebViewChromium::OnOverScrolled(
    /* [in] */ const Int32 scrollX,
    /* [in] */ const Int32 scrollY,
    /* [in] */ const Boolean clampedX,
    /* [in] */ const Boolean clampedY)
{
#if 0
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new OnOverScrolledRunnable(this, scrollX, scrollY, clampedX, clampedY);
        mRunQueue.addTask(runnable);
        return;
    }
    mAwContents.onContainerViewOverScrolled(scrollX, scrollY, clampedX, clampedY);
#endif
}

//@Override
ECode WebViewChromium::OnWindowVisibilityChanged(
    /* [in] */ const Int32 visibility)
{
#if 0
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new OnWindowVisibilityChangedRunnable(this, visibility);
        mRunQueue.addTask(runnable);
        return;
    }
    mAwContents.onWindowVisibilityChanged(visibility);
#endif
}

//@Override
ECode WebViewChromium::OnDraw(
    /* [in] */ const ICanvas* canvas)
{
#if 0
    mFactory.startYourEngines(true);
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new OnDrawRunnable(this, canvas);
        runVoidTaskOnUiThreadBlocking(runnable);
        return;
    }
    mAwContents.onDraw(canvas);
#endif
}

//@Override
ECode WebViewChromium::SetLayoutParams(
    /* [in] */ const IViewGroupLayoutParams* layoutParams)
{
    // This API is our strongest signal from the View system that this
    // WebView is going to be bound to a View hierarchy and so at this
    // point we must bind Chromium's UI thread to the current thread.
    mFactory->StartYourEngines(FALSE);
    CheckThread();
    mWebViewPrivate->Super_setLayoutParams(layoutParams);
    return NOERROR;
}

//@Override
ECode WebViewChromium::PerformLongClick(
    /* [out] */ Boolean* result)
{
#if  0
    // Return false unless the WebView is attached to a View with a parent
    return mWebView.getParent() != null ? mWebViewPrivate.super_performLongClick() : false;
#endif
}

//@Override
ECode WebViewChromium::OnConfigurationChanged(
    /* [in] */ const IConfiguration* newConfig)
{
#if 0
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new OnConfigurationChangedRunnable(this, newConfig);
        mRunQueue.addTask(runnable);
        return;
    }
    mAwContents.onConfigurationChanged(newConfig);
#endif
}

//@Override
ECode WebViewChromium::OnCreateInputConnection(
    /* [in] */ const IEditorInfo* outAttrs,
    /* [out] */ IInputConnection** connection)
{
#if 0
    mFactory.startYourEngines(false);
    if (checkNeedsPost()) {
       return null;
    }
    return mAwContents.onCreateInputConnection(outAttrs);
#endif
}

//@Override
ECode WebViewChromium::OnKeyMultiple(
    /* [in] */ const Int32 keyCode,
    /* [in] */ const Int32 repeatCount,
    /* [in] */ const IKeyEvent* event,
    /* [out] */ Boolean* result)
{
#if 0
    mFactory.startYourEngines(false);
    if (checkNeedsPost()) {
        AutoPtr<ICallable> callable = new OnKeyMultipleCallable(this, keyCode, repeatCount, event);
        boolean ret = runOnUiThreadBlocking(callable);
        return ret;
    }
    UnimplementedWebViewApi.invoke();
    return false;
#endif
}

//@Override
ECode WebViewChromium::OnKeyDown(
    /* [in] */ const Int32 keyCode,
    /* [in] */ const IKeyEvent* event,
    /* [out] */ Boolean* result)
{
#if 0
    mFactory.startYourEngines(false);
    if (checkNeedsPost()) {
        AutoPtr<ICallable> callable = new OnKeyDownCallable(this, keyCode, event);
        boolean ret = runOnUiThreadBlocking(callable);
        return ret;
    }
    UnimplementedWebViewApi.invoke();
    return false;
#endif
}

//@Override
ECode WebViewChromium::OnKeyUp(
    /* [in] */ const Int32 keyCode,
    /* [in] */ const IKeyEvent* event,
    /* [out] */ Boolean* result)
{
#if 0
    mFactory.startYourEngines(false);
    if (checkNeedsPost()) {
        AutoPtr<ICallable> callable = new OnKeyUpCallable(this, keyCode, event);
        boolean ret = runOnUiThreadBlocking(callable);
        return ret;
    }
    return mAwContents.onKeyUp(keyCode, event);
#endif
}

//@Override
ECode WebViewChromium::OnAttachedToWindow()
{
    // This API is our strongest signal from the View system that this
    // WebView is going to be bound to a View hierarchy and so at this
    // point we must bind Chromium's UI thread to the current thread.
    mFactory->StartYourEngines(FALSE);
    CheckThread();
    mAwContents->OnAttachedToWindow();
    return NOERROR;
}

//@Override
ECode WebViewChromium::OnDetachedFromWindow()
{
#if 0
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new OnDetachedFromWindowRunnable(this);
        mRunQueue.addTask(runnable);
        return;
    }

    mAwContents.onDetachedFromWindow();
#endif
}

//@Override
ECode WebViewChromium::OnVisibilityChanged(
    /* [in] */ const IView* changedView,
    /* [in] */ const Int32 visibility)
{
#if 0
    // The AwContents will find out the container view visibility before the first draw so we
    // can safely ignore onVisibilityChanged callbacks that happen before init().
    if (mAwContents == null) return;

    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new OnVisibilityChangedRunnable(this, changedView, visibility);
        mRunQueue.addTask(runnable);
        return;
    }
    mAwContents.onVisibilityChanged(changedView, visibility);
#endif
}

//@Override
ECode WebViewChromium::OnWindowFocusChanged(
    /* [in] */ const Boolean hasWindowFocus)
{
#if 0
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new OnWindowFocusChangedRunnable(this, hasWindowFocus);
        mRunQueue.addTask(runnable);
        return;
    }
    mAwContents.onWindowFocusChanged(hasWindowFocus);
#endif
}

//@Override
ECode WebViewChromium::OnFocusChanged(
    /* [in] */ const Boolean focused,
    /* [in] */ const Int32 direction,
    /* [in] */ const IRect* previouslyFocusedRect)
{
#if 0
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new OnFocusChangedRunnable(this, focused, direction, previouslyFocusedRect);
        mRunQueue.addTask(runnable);
        return;
    }
    mAwContents.onFocusChanged(focused, direction, previouslyFocusedRect);
#endif
}

//@Override
ECode WebViewChromium::SetFrame(
    /* [in] */ const Int32 left,
    /* [in] */ const Int32 top,
    /* [in] */ const Int32 right,
    /* [in] */ const Int32 bottom,
    /* [out] */ Boolean* result)
{
    return mWebViewPrivate->Super_setFrame(left, top, right, bottom);
}

//@Override
ECode WebViewChromium::OnSizeChanged(
    /* [in] */ const Int32 w,
    /* [in] */ const Int32 h,
    /* [in] */ const Int32 ow,
    /* [in] */ const Int32 oh)
{
#if 0
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new OnSizeChangedRunnable(this, w, h, ow, oh);
        mRunQueue.addTask(runnable);
        return;
    }
    mAwContents.onSizeChanged(w, h, ow, oh);
#endif
}

//@Override
ECode WebViewChromium::OnScrollChanged(
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 oldl,
    /* [in] */ Int32 oldt)
{
    return NOERROR;
}

//@Override
ECode WebViewChromium::DispatchKeyEvent(
    /* [in] */ const IKeyEvent* event,
    /* [out] */ Boolean* result)
{
#if 0
    mFactory.startYourEngines(false);
    if (checkNeedsPost()) {
        AutoPtr<ICallable> callable = new DispatchKeyEventCallable(this, event);
        boolean ret = runOnUiThreadBlocking(callable);
        return ret;
    }
    return mAwContents.dispatchKeyEvent(event);
#endif
}

//@Override
ECode WebViewChromium::OnTouchEvent(
    /* [in] */ const IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
#if 0
    mFactory.startYourEngines(false);
    if (checkNeedsPost()) {
        AutoPtr<ICallable> callable = new OnTouchEventCallable(this, ev);
        boolean ret = runOnUiThreadBlocking(callable);
        return ret;
    }
    return mAwContents.onTouchEvent(ev);
#endif
}

//@Override
ECode WebViewChromium::OnHoverEvent(
    /* [in] */ const IMotionEvent* event,
    /* [out] */ Boolean* result)
{
#if 0
    mFactory.startYourEngines(false);
    if (checkNeedsPost()) {
        AutoPtr<ICallable> callable = new OnHoverEventCallable(this, event);
        boolean ret = runOnUiThreadBlocking(callable);
        return ret;
    }
    return mAwContents.onHoverEvent(event);
#endif
}

//@Override
ECode WebViewChromium::OnGenericMotionEvent(
    /* [in] */ const IMotionEvent* event,
    /* [out] */ Boolean* result)
{
#if 0
    mFactory.startYourEngines(false);
    if (checkNeedsPost()) {
        AutoPtr<ICallable> callable = new OnGenericMotionEventCallable(this, event);
        boolean ret = runOnUiThreadBlocking(callable);
        return ret;
    }
    return mAwContents.onGenericMotionEvent(event);
#endif
}

//@Override
ECode WebViewChromium::OnTrackballEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // Trackball event not handled, which eventually gets converted to DPAD keyevents
    *result = FALSE;
    return NOERROR;
}

//@Override
ECode WebViewChromium::RequestFocus(
    /* [in] */ const Int32 direction,
    /* [in] */ const IRect* previouslyFocusedRect,
    /* [out] */ Boolean* result)
{
#if 0
    mFactory.startYourEngines(false);
    if (checkNeedsPost()) {
        AutoPtr<ICallable> callable = new RequestFocusCallable(this, direction, previouslyFocusedRect);
        boolean ret = runOnUiThreadBlocking(callable);
        return ret;
    }
    mAwContents.requestFocus();
    return mWebViewPrivate.super_requestFocus(direction, previouslyFocusedRect);
#endif
}

//@Override
ECode WebViewChromium::OnMeasure(
    /* [in] */ const Int32 widthMeasureSpec,
    /* [in] */ const Int32 heightMeasureSpec)
{
#if 0
    mFactory.startYourEngines(false);
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new OnMeasureRunnable(this, widthMeasureSpec, heightMeasureSpec);
        runVoidTaskOnUiThreadBlocking(runnable);
        return;
    }
    mAwContents.onMeasure(widthMeasureSpec, heightMeasureSpec);
#endif
}

//@Override
ECode WebViewChromium::RequestChildRectangleOnScreen(
    /* [in] */ const IView* child,
    /* [in] */ const IRect* rect,
    /* [in] */ const Boolean immediate,
    /* [out] */ Boolean* result)
{
#if 0
    mFactory.startYourEngines(false);
    if (checkNeedsPost()) {
        AutoPtr<ICallable> callable = new RequestChildRectangleOnScreenCallable(this, child, rect, immediate);
        boolean ret = runOnUiThreadBlocking(callable);
        return ret;
    }
    return mAwContents.requestChildRectangleOnScreen(child, rect, immediate);
#endif
}

//@Override
ECode WebViewChromium::SetBackgroundColor(
    /* [in] */ const Int32 color)
{
#if 0
    mFactory.startYourEngines(false);
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new SetBackgroundColorRunnable(this, color);
        ThreadUtils.postOnUiThread(runnable);
        return;
    }
    mAwContents.setBackgroundColor(color);
#endif
}

//@Override
ECode WebViewChromium::SetLayerType(
    /* [in] */ const Int32 layerType,
    /* [in] */ const IPaint* paint)
{
#if 0
    // This can be called from WebView constructor in which case mAwContents
    // is still null. We set the layer type in initForReal in that case.
    if (mAwContents == null) return;
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new SetLayerTypeRunnable(this, layerType, paint);
        ThreadUtils.postOnUiThread(runnable);
        return;
    }
    mAwContents.setLayerType(layerType, paint);
#endif
}

// Remove from superclass
void WebViewChromium::PreDispatchDraw(
    /* [in] */ ICanvas* canvas)
{
    // TODO(leandrogracia): remove this method from WebViewProvider if we think
    // we won't need it again.
}

void WebViewChromium::OnStartTemporaryDetach()
{
    mAwContents->OnStartTemporaryDetach();
}

void WebViewChromium::OnFinishTemporaryDetach()
{
    mAwContents->OnFinishTemporaryDetach();
}

// WebViewProvider.ScrollDelegate implementation ----------------------------------------------

//@Override
ECode WebViewChromium::ComputeHorizontalScrollRange(
    /* [out] */ Int32* range)
{
#if 0
    mFactory.startYourEngines(false);
    if (checkNeedsPost()) {
        AutoPtr<ICallable> callable = new ComputeHorizontalScrollRangeCallable(this);
        int ret = runOnUiThreadBlocking(callable);
        return ret;
    }
    return mAwContents.computeHorizontalScrollRange();
#endif
}

//@Override
ECode WebViewChromium::ComputeHorizontalScrollOffset(
    /* [out] */ Int32* offset)
{
#if 0
    mFactory.startYourEngines(false);
    if (checkNeedsPost()) {
        AutoPtr<ICallable> callable = new ComputeHorizontalScrollOffsetCallable(this);
        int ret = runOnUiThreadBlocking(callable);
        return ret;
    }
    return mAwContents.computeHorizontalScrollOffset();
#endif
}

//@Override
ECode WebViewChromium::ComputeVerticalScrollRange(
    /* [out] */ Int32* range)
{
#if 0
    mFactory.startYourEngines(false);
    if (checkNeedsPost()) {
        AutoPtr<ICallable> callable = new ComputeVerticalScrollRangeCallable(this);
        int ret = runOnUiThreadBlocking(callable);
        return ret;
    }
    return mAwContents.computeVerticalScrollRange();
#endif
}

//@Override
ECode WebViewChromium::ComputeVerticalScrollOffset(
    /* [out] */ Int32* offset)
{
#if 0
    mFactory.startYourEngines(false);
    if (checkNeedsPost()) {
        AutoPtr<ICallable> callable = new ComputeVerticalScrollOffsetCallable(this);
        int ret = runOnUiThreadBlocking(callable);
        return ret;
    }
    return mAwContents.computeVerticalScrollOffset();
#endif
}

//@Override
ECode WebViewChromium::ComputeVerticalScrollExtent(
    /* [out] */ Int32* extent)
{
#if 0
    mFactory.startYourEngines(false);
    if (checkNeedsPost()) {
        AutoPtr<ICallable> callable = new ComputeVerticalScrollExtentCallable(this);
        int ret = runOnUiThreadBlocking(callable);
        return ret;
    }
    return mAwContents.computeVerticalScrollExtent();
#endif
}

//@Override
ECode WebViewChromium::ComputeScroll()
{
#if 0
    mFactory.startYourEngines(false);
    if (checkNeedsPost()) {
        AutoPtr<IRunnable> runnable = new ComputeScrollRunnable(this);
        runVoidTaskOnUiThreadBlocking(runnable);
        return;
    }
    mAwContents.computeScroll();
#endif
}

// TODO(sgurun) this is only to have master-gpl compiling.
AutoPtr<IPrintDocumentAdapter> WebViewChromium::CreatePrintDocumentAdapter()
{
     return CreatePrintDocumentAdapter("default");
}

//@Override TODO(sgurun) commenting this out to have master-gpl compiling.
AutoPtr<IPrintDocumentAdapter> WebViewChromium::CreatePrintDocumentAdapter(
    /* [in] */ const String& documentName)
{
    CheckThread();
    return new AwPrintDocumentAdapter(mAwContents.getPdfExporter(), documentName);
}

// Implements SmartClipProvider
//@Override
ECode WebViewChromium::ExtractSmartClipData(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    CheckThread();
    mAwContents->ExtractSmartClipData(x, y, width, height);
    return NOERROR;
}

// Implements SmartClipProvider
//@Override
ECode WebViewChromium::SetSmartClipResultHandler(
    /* [in] */ const IHandler* resultHandler)
{
    CheckThread();
    mAwContents->SetSmartClipResultHandler(resultHandler);
    return NOERROR;
}

} // namespace Chromium
} // namespace Chromium
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
