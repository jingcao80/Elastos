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

#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/incallui/CVideoCallFragment.h"
#include "elastos/droid/incallui/VideoCallPresenter.h"
#include "elastos/droid/incallui/InCallPresenter.h"
#include "R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::InCallUI::EIID_IUi;
using Elastos::Droid::InCallUI::EIID_IVideoCallUi;
using Elastos::Droid::View::EIID_ISurfaceTextureListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IOnGlobalLayoutListener;
using Elastos::Droid::View::CSurface;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IViewTreeObserver;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace InCallUI {

//================================================================
// CVideoCallFragment::VideoCallSurface
//================================================================
CAR_INTERFACE_IMPL_2(CVideoCallFragment::VideoCallSurface, Object, ISurfaceTextureListener, IViewOnClickListener);

CVideoCallFragment::VideoCallSurface::VideoCallSurface(
    /* [in] */ Int32 surfaceId,
    /* [in] */ ITextureView* textureView,
    /* [in] */ CVideoCallFragment* host)
    : mSurfaceId(surfaceId)
    , mHost(host)
{
    mHost->mWidth = DIMENSIONS_NOT_SET;
    mHost->mHeight = DIMENSIONS_NOT_SET;
    RecreateView(textureView);
}

CVideoCallFragment::VideoCallSurface::VideoCallSurface(
    /* [in] */ Int32 surfaceId,
    /* [in] */ ITextureView* textureView,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ CVideoCallFragment* host)
    : mSurfaceId(surfaceId)
    , mHost(host)
{
    mHost->mWidth = width;
    mHost->mHeight = height;
    RecreateView(textureView);
}

void CVideoCallFragment::VideoCallSurface::RecreateView(
    /* [in] */ ITextureView* view)
{
    mTextureView = view;
    mTextureView->SetSurfaceTextureListener(this);          // no memory-leak
    IView::Probe(mTextureView)->SetOnClickListener(this);   // no memory-leak

    if (mSavedSurfaceTexture != NULL) {
        mTextureView->SetSurfaceTexture(mSavedSurfaceTexture);
    }
}

ECode CVideoCallFragment::VideoCallSurface::OnSurfaceTextureAvailable(
    /* [in] */ ISurfaceTexture* surfaceTexture,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    Boolean surfaceCreated;
    // Where there is no saved {@link SurfaceTexture} available, use the newly created one.
    // If a saved {@link SurfaceTexture} is available, we are re-creating after an
    // orientation change.
    if (mSavedSurfaceTexture == NULL) {
        mSavedSurfaceTexture = surfaceTexture;
        surfaceCreated = CreateSurface();
    }
    else {
        // A saved SurfaceTexture was found.
        surfaceCreated = FALSE;
    }

    // Inform presenter that the surface is available.
    if (surfaceCreated) {
        ((VideoCallPresenter*)mHost->GetPresenter().Get())->OnSurfaceCreated(mSurfaceId);
    }
    return NOERROR;
}

ECode CVideoCallFragment::VideoCallSurface::OnSurfaceTextureSizeChanged(
    /* [in] */ ISurfaceTexture* surfaceTexture,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    return NOERROR;
}

ECode CVideoCallFragment::VideoCallSurface::OnSurfaceTextureDestroyed(
    /* [in] */ ISurfaceTexture* surfaceTexture,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    /**
     * Destroying the surface texture; inform the presenter so it can null the surfaces.
     */
    if (mSavedSurfaceTexture == NULL) {
        ((VideoCallPresenter*)mHost->GetPresenter().Get())->OnSurfaceDestroyed(mSurfaceId);
        if (mSavedSurface != NULL) {
            mSavedSurface->Release();
            mSavedSurface = NULL;
        }
    }

    // The saved SurfaceTexture will be null if we're shutting down, so we want to
    // return "true" in that case (indicating that TextureView can release the ST).
    *result = (mSavedSurfaceTexture == NULL);
    return NOERROR;
}

ECode CVideoCallFragment::VideoCallSurface::OnSurfaceTextureUpdated(
    /* [in] */ ISurfaceTexture* surface)
{
    return NOERROR;
}

AutoPtr<ITextureView> CVideoCallFragment::VideoCallSurface::GetTextureView()
{
    return mTextureView;
}

void CVideoCallFragment::VideoCallSurface::SetDoneWithSurface()
{
    if (mSavedSurface != NULL) {
        mSavedSurface->Release();
        mSavedSurface = NULL;
    }
    if (mSavedSurfaceTexture != NULL) {
        mSavedSurfaceTexture->Release();
        mSavedSurfaceTexture = NULL;
    }
}

AutoPtr<ISurface> CVideoCallFragment::VideoCallSurface::GetSurface()
{
    return mSavedSurface;
}

void CVideoCallFragment::VideoCallSurface::SetSurfaceDimensions(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    mHost->mWidth = width;
    mHost->mHeight = height;

    if (mSavedSurfaceTexture != NULL) {
        CreateSurface();
    }
}

Boolean CVideoCallFragment::VideoCallSurface::CreateSurface()
{
    if (mHost->mWidth != DIMENSIONS_NOT_SET && mHost->mHeight != DIMENSIONS_NOT_SET &&
            mSavedSurfaceTexture != NULL) {

        mSavedSurfaceTexture->SetDefaultBufferSize(mHost->mWidth, mHost->mHeight);
        AutoPtr<ISurface> surface;
        CSurface::New(mSavedSurfaceTexture, (ISurface**)&surface);
        mSavedSurface = surface;
        return TRUE;
    }
    return FALSE;
}

ECode CVideoCallFragment::VideoCallSurface::OnClick(
    /* [in] */ IView* view)
{
    ((VideoCallPresenter*)mHost->GetPresenter().Get())->OnSurfaceClick(mSurfaceId);
    return NOERROR;
}


//================================================================
// CVideoCallFragment::MyGlobalLayoutListener
//================================================================
CAR_INTERFACE_IMPL(CVideoCallFragment::MyGlobalLayoutListener, Object, IOnGlobalLayoutListener)

ECode CVideoCallFragment::MyGlobalLayoutListener::OnGlobalLayout()
{
    // Check if the layout includes the incoming video surface -- this will only be the
    // case for a video call.
    AutoPtr<IView> displayVideo;
    mView->FindViewById(Elastos::Droid::Dialer::R::id::incomingVideo, (IView**)&displayVideo);
    if (displayVideo != NULL) {
        mHost->CenterDisplayView(displayVideo);
    }

    mHost->mIsLayoutComplete = TRUE;

    // Remove the listener so we don't continually re-layout.
    AutoPtr<IViewTreeObserver> observer;
    mView->GetViewTreeObserver((IViewTreeObserver**)&observer);
    Boolean isAlive;
    if (observer->IsAlive(&isAlive), isAlive) {
        observer->RemoveOnGlobalLayoutListener(this);
    }
    return NOERROR;
}


//================================================================
// CVideoCallFragment
//================================================================
const Int32 CVideoCallFragment::DIMENSIONS_NOT_SET;
Boolean CVideoCallFragment::sVideoSurfacesInUse = FALSE;
AutoPtr<CVideoCallFragment::VideoCallSurface> CVideoCallFragment::sPreviewSurface;
AutoPtr<CVideoCallFragment::VideoCallSurface> CVideoCallFragment::sDisplaySurface;

CAR_INTERFACE_IMPL_3(CVideoCallFragment, BaseFragment, IVideoCallFragment, IVideoCallUi, IUi);

CAR_OBJECT_IMPL(CVideoCallFragment)

CVideoCallFragment::CVideoCallFragment()
    : mIsActivityRestart(FALSE)
    , mIsLayoutComplete(FALSE)
    , mIsLandscape(FALSE)
    , mWidth(DIMENSIONS_NOT_SET)
    , mHeight(DIMENSIONS_NOT_SET)
{
    BaseFragment::constructor();
}

ECode CVideoCallFragment::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    FAIL_RETURN(BaseFragment::OnCreate(savedInstanceState))
    mIsActivityRestart = sVideoSurfacesInUse;
    return NOERROR;
}

ECode CVideoCallFragment::OnActivityCreated(
    /* [in] */ IBundle* savedInstanceState)
{
    FAIL_RETURN(BaseFragment::OnActivityCreated(savedInstanceState))

    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    AutoPtr<IConfiguration> config;
    res->GetConfiguration((IConfiguration**)&config);
    Int32 orientation;
    config->GetOrientation(&orientation);
    mIsLandscape = orientation == IConfiguration::ORIENTATION_LANDSCAPE;

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    ((VideoCallPresenter*)GetPresenter().Get())->Init(IContext::Probe(activity));
    return NOERROR;
}

ECode CVideoCallFragment::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IView** _view)
{
    VALIDATE_NOT_NULL(_view)
    FAIL_RETURN(BaseFragment::OnCreateView(inflater, container, savedInstanceState, _view))

    AutoPtr<IView> view;
    inflater->Inflate(Elastos::Droid::Dialer::R::layout::video_call_fragment, container, FALSE, (IView**)&view);

    // Attempt to center the incoming video view, if it is in the layout.
    AutoPtr<IViewTreeObserver> observer;
    view->GetViewTreeObserver((IViewTreeObserver**)&observer);
    AutoPtr<IOnGlobalLayoutListener> listener = (IOnGlobalLayoutListener*)new MyGlobalLayoutListener(view, this);
    observer->AddOnGlobalLayoutListener(listener);

    *_view = view;
    REFCOUNT_ADD(*_view)
    return NOERROR;
}

void CVideoCallFragment::CenterDisplayView(
    /* [in] */ IView* displayVideo)
{
    // In a lansdcape layout we need to ensure we horizontally center the view based on whether
    // the layout is left-to-right or right-to-left.
    // In a left-to-right locale, the space for the video view is to the right of the call card
    // so we need to translate it in the +X direction.
    // In a right-to-left locale, the space for the video view is to the left of the call card
    // so we need to translate it in the -X direction.
    Boolean isLayoutRtl = InCallPresenter::IsRtl();

    Float spaceBesideCallCard = InCallPresenter::GetInstance()->GetSpaceBesideCallCard();
    if (mIsLandscape) {
        Int32 width;
        displayVideo->GetWidth(&width);
        Float videoViewTranslation = width / 2 - spaceBesideCallCard / 2;
        if (isLayoutRtl) {
            displayVideo->SetTranslationX(-videoViewTranslation);
        }
        else {
            displayVideo->SetTranslationX(videoViewTranslation);
        }
    }
    else {
        Int32 height;
        displayVideo->GetHeight(&height);
        Float videoViewTranslation = height / 2 - spaceBesideCallCard / 2;
        displayVideo->SetTranslationY(videoViewTranslation);
    }
}

ECode CVideoCallFragment::OnViewCreated(
    /* [in] */ IView* view,
    /* [in] */ IBundle* savedInstanceState)
{
    FAIL_RETURN(BaseFragment::OnViewCreated(view, savedInstanceState))

    AutoPtr<IView> temp;
    view->FindViewById(Elastos::Droid::Dialer::R::id::videoCallViewsStub, (IView**)&temp);
    mVideoViewsStub = IViewStub::Probe(temp);

    // If the surfaces are already in use, we have just changed orientation or otherwise
    // re-created the fragment.  In this case we need to inflate the video call views and
    // restore the surfaces.
    if (sVideoSurfacesInUse) {
        InflateVideoCallViews();
    }
    return NOERROR;
}

AutoPtr<IPresenter> CVideoCallFragment::CreatePresenter()
{
    AutoPtr<IPresenter> p = (IPresenter*)new VideoCallPresenter();
    return p;
}

AutoPtr<IUi> CVideoCallFragment::GetUi()
{
    return IUi::Probe(this);
}

ECode CVideoCallFragment::ShowVideoUi(
    /* [in] */ Boolean show)
{
    Int32 visibility = show ? IView::VISIBLE : IView::GONE;
    AutoPtr<IView> view;
    GetView((IView**)&view);
    view->SetVisibility(visibility);

    if (show) {
        InflateVideoCallViews();
    }
    else {
        CleanupSurfaces();
    }

    if (mVideoViews != NULL ) {
        mVideoViews->SetVisibility(visibility);
    }
    return NOERROR;
}

ECode CVideoCallFragment::CleanupSurfaces()
{
    if (sDisplaySurface != NULL) {
        sDisplaySurface->SetDoneWithSurface();
        sDisplaySurface = NULL;
    }
    if (sPreviewSurface != NULL) {
        sPreviewSurface->SetDoneWithSurface();
        sPreviewSurface = NULL;
    }
    sVideoSurfacesInUse = FALSE;
    return NOERROR;
}

ECode CVideoCallFragment::IsActivityRestart(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIsActivityRestart;
    return NOERROR;
}

ECode CVideoCallFragment::IsDisplayVideoSurfaceCreated(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = sDisplaySurface != NULL && (sDisplaySurface->GetSurface() != NULL);
    return NOERROR;
}

ECode CVideoCallFragment::IsPreviewVideoSurfaceCreated(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = sPreviewSurface != NULL && sPreviewSurface->GetSurface() != NULL;
    return NOERROR;
}

ECode CVideoCallFragment::GetDisplayVideoSurface(
    /* [out] */ ISurface** surface)
{
    VALIDATE_NOT_NULL(surface)
    AutoPtr<ISurface> s;
    if (sDisplaySurface != NULL) {
        s = sDisplaySurface->GetSurface();
    }
    *surface = s;
    REFCOUNT_ADD(*surface)
    return NOERROR;
}

ECode CVideoCallFragment::GetPreviewVideoSurface(
    /* [out] */ ISurface** surface)
{
    VALIDATE_NOT_NULL(surface)
    AutoPtr<ISurface> s;
    if (sPreviewSurface != NULL) {
        s = sPreviewSurface->GetSurface();
    }
    *surface = s;
    REFCOUNT_ADD(*surface)
    return NOERROR;
}

ECode CVideoCallFragment::SetPreviewSize(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    if (sPreviewSurface != NULL) {
        AutoPtr<ITextureView> preview = sPreviewSurface->GetTextureView();

        if (preview == NULL ) {
            return NOERROR;
        }

        AutoPtr<IView> view = IView::Probe(preview);
        AutoPtr<IViewGroupLayoutParams> params;
        view->GetLayoutParams((IViewGroupLayoutParams**)&params);
        params->SetWidth(width);
        params->SetHeight(height);
        view->SetLayoutParams(params);

        sPreviewSurface->SetSurfaceDimensions(width, height);
    }
    return NOERROR;
}

void CVideoCallFragment::InflateVideoCallViews()
{
    assert(0 && "TODO");
    // if (mVideoViews == NULL) {
    //     mVideoViews = mVideoViewsStub.inflate();
    // }

    // if (mVideoViews != null) {
    //     TextureView displaySurface = (TextureView) mVideoViews.findViewById(R.id.incomingVideo);

    //     Point screenSize = getScreenSize();
    //     setSurfaceSizeAndTranslation(displaySurface, screenSize);

    //     if (!sVideoSurfacesInUse) {
    //         // Where the video surfaces are not already in use (first time creating them),
    //         // setup new VideoCallSurface instances to track them.
    //         sDisplaySurface = new VideoCallSurface(SURFACE_DISPLAY,
    //                 (TextureView) mVideoViews.findViewById(R.id.incomingVideo), screenSize.x,
    //                 screenSize.y);
    //         sPreviewSurface = new VideoCallSurface(SURFACE_PREVIEW,
    //                 (TextureView) mVideoViews.findViewById(R.id.previewVideo));
    //         sVideoSurfacesInUse = true;
    //     } else {
    //         // In this case, the video surfaces are already in use (we are recreating the
    //         // Fragment after a destroy/create cycle resulting from a rotation.
    //         sDisplaySurface.recreateView((TextureView) mVideoViews.findViewById(
    //                 R.id.incomingVideo));
    //         sPreviewSurface.recreateView((TextureView) mVideoViews.findViewById(
    //                 R.id.previewVideo));
    //     }
    // }
}

void CVideoCallFragment::SetSurfaceSizeAndTranslation(
    /* [in] */ ITextureView* textureView,
    /* [in] */ IPoint* size)
{}

AutoPtr<IPoint> CVideoCallFragment::GetScreenSize()
{
    return NOERROR;
}

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos
