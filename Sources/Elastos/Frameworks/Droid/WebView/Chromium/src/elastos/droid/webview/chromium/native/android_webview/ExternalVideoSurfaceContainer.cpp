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

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/ExternalVideoSurfaceContainer.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/api/ExternalVideoSurfaceContainer_dec.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IViewManager;
using Elastos::Droid::View::EIID_ISurfaceHolderCallback;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

//===============================================================
//    ExternalVideoSurfaceContainer::NoPunchingSurfaceView
//===============================================================
CAR_INTERFACE_IMPL(ExternalVideoSurfaceContainer, Object, ISurfaceHolderCallback);

ExternalVideoSurfaceContainer::NoPunchingSurfaceView::NoPunchingSurfaceView(
    /* [in] */ IContext* context)
{
    SurfaceView::constructor(context);
}
// SurfaceView.dispatchDraw implementation punches a hole in the view hierarchy.
// Disable this by making this a no-op.
//@Override
ECode ExternalVideoSurfaceContainer::NoPunchingSurfaceView::DispatchDraw(
    /* [in] */ ICanvas* canvas)
{
    return NOERROR;
}

//===============================================================
//           ExternalVideoSurfaceContainer::Factory
//===============================================================

AutoPtr<ExternalVideoSurfaceContainer> ExternalVideoSurfaceContainer::Factory::Create(
    /* [in] */ Handle64 nativeExternalVideoSurfaceContainer,
    /* [in] */ ContentViewCore* contentViewCore)
{
    AutoPtr<ExternalVideoSurfaceContainer> container = new ExternalVideoSurfaceContainer(
            nativeExternalVideoSurfaceContainer, contentViewCore);
    return container;
}

//===============================================================
//                ExternalVideoSurfaceContainer
//===============================================================

const Int32 ExternalVideoSurfaceContainer::INVALID_PLAYER_ID;
AutoPtr<IWeakReference> ExternalVideoSurfaceContainer::sActiveContainer;
AutoPtr<ExternalVideoSurfaceContainer::Factory> ExternalVideoSurfaceContainer::sFactory = new Factory();

ExternalVideoSurfaceContainer::ExternalVideoSurfaceContainer(
    /* [in] */ Handle64 nativeExternalVideoSurfaceContainer,
    /* [in] */ ContentViewCore* contentViewCore)
    : mNativeExternalVideoSurfaceContainer(nativeExternalVideoSurfaceContainer)
    , mContentViewCore(contentViewCore)
    , mPlayerId(INVALID_PLAYER_ID)
    , mLeft(0.0f)
    , mTop(0.0f)
    , mRight(0.0f)
    , mBottom(0.0f)
    , mX(0)
    , mY(0)
    , mWidth(0)
    , mHeight(0)
{
    assert(contentViewCore != NULL);
    InitializeCurrentPositionOfSurfaceView();
}


//@VisibleForTesting
void ExternalVideoSurfaceContainer::SetFactory(
    /* [in] */ Factory* factory)
{
    sFactory = factory;
}

//@CalledByNative return ExternalVideoSurfaceContainer
AutoPtr<IInterface> ExternalVideoSurfaceContainer::Create(
    /* [in] */ Handle64 nativeExternalVideoSurfaceContainer,
    /* [in] */ ContentViewCore* contentViewCore)
{
    AutoPtr<ExternalVideoSurfaceContainer> externalVideoSurfaceContainer =
        sFactory->Create(nativeExternalVideoSurfaceContainer, contentViewCore);
    AutoPtr<IInterface> result = TO_IINTERFACE(externalVideoSurfaceContainer);
    return result;
}

/**
 * Called when a media player wants to request an external video surface.
 * @param playerId The ID of the media player.
 */
//@CalledByNative
void ExternalVideoSurfaceContainer::RequestExternalVideoSurface(
    /* [in] */ Int32 playerId)
{
    if (mPlayerId == playerId) return;

    if (mPlayerId == INVALID_PLAYER_ID) {
        SetActiveContainer(this);
    }

    mPlayerId = playerId;
    InitializeCurrentPositionOfSurfaceView();

    CreateSurfaceView();
}

/**
 * Called when a media player wants to release an external video surface.
 * @param playerId The ID of the media player.
 */
//@CalledByNative
void ExternalVideoSurfaceContainer::ReleaseExternalVideoSurface(
    /* [in] */ Int32 playerId)
{
    if (mPlayerId != playerId) return;

    ReleaseIfActiveContainer(this);

    mPlayerId = INVALID_PLAYER_ID;
}

//@CalledByNative
void ExternalVideoSurfaceContainer::Destroy()
{
    ReleaseExternalVideoSurface(mPlayerId);
}

void ExternalVideoSurfaceContainer::InitializeCurrentPositionOfSurfaceView()
{
    mX = Elastos::Core::Math::INT32_MAX_VALUE;
    mY = Elastos::Core::Math::INT32_MIN_VALUE;
    mWidth = 0;
    mHeight = 0;
}

void ExternalVideoSurfaceContainer::SetActiveContainer(
    /* [in] */ ExternalVideoSurfaceContainer* container)
{
    AutoPtr<IInterface> ws;
    sActiveContainer->Resolve(EIID_IInterface, (IInterface**)&ws);
    if (ws != NULL) {
        AutoPtr<ExternalVideoSurfaceContainer> activeContainer =
            (ExternalVideoSurfaceContainer*)(IObject::Probe(ws));
        activeContainer->RemoveSurfaceView();
    }
    //sActiveContainer = new WeakReference<ExternalVideoSurfaceContainer>(container);
    sActiveContainer = NULL;
    AutoPtr<IWeakReferenceSource> source = IWeakReferenceSource::Probe((IWeakReferenceSource*)container);
    source->GetWeakReference((IWeakReference**)&sActiveContainer);
}

void ExternalVideoSurfaceContainer::ReleaseIfActiveContainer(
    /* [in] */ ExternalVideoSurfaceContainer* container)
{
    AutoPtr<IInterface> ws;
    sActiveContainer->Resolve(EIID_IInterface, (IInterface**)&ws);
    if (ws != NULL) {
        AutoPtr<ExternalVideoSurfaceContainer> activeContainer =
            (ExternalVideoSurfaceContainer*)(IObject::Probe(ws));
        if (activeContainer.Get() == container) {
            SetActiveContainer(NULL);
        }
    }
}

void ExternalVideoSurfaceContainer::CreateSurfaceView()
{
    ///TODO remove the type-cast
    mSurfaceView = (ISurfaceView*)(new NoPunchingSurfaceView(mContentViewCore->GetContext()));
    AutoPtr<ISurfaceHolder> holder;
    mSurfaceView->GetHolder((ISurfaceHolder**)&holder);
    holder->AddCallback(this);
    // SurfaceHoder.surfaceCreated() will be called after the SurfaceView is attached to
    // the Window and becomes visible.
    AutoPtr<IView> view = IView::Probe(mSurfaceView);
    mContentViewCore->GetContainerView()->AddView(view);
}

void ExternalVideoSurfaceContainer::RemoveSurfaceView()
{
    // SurfaceHoder.surfaceDestroyed() will be called in ViewGroup.removeView()
    // as soon as the SurfaceView is detached from the Window.
    AutoPtr<IViewGroup> viewGroup = mContentViewCore->GetContainerView();
    AutoPtr<IViewManager> viewManager = IViewManager::Probe(viewGroup);
    AutoPtr<IView> view = IView::Probe(mSurfaceView);
    viewManager->RemoveView(view);
    mSurfaceView = NULL;
}

/**
 * Called when the position of the video element which uses the external
 * video surface is changed.
 * @param playerId The ID of the media player.
 * @param left The absolute CSS X coordinate of the left side of the video element.
 * @param top The absolute CSS Y coordinate of the top side of the video element.
 * @param right The absolute CSS X coordinate of the right side of the video element.
 * @param bottom The absolute CSS Y coordinate of the bottom side of the video element.
 */
//@CalledByNative
void ExternalVideoSurfaceContainer::OnExternalVideoSurfacePositionChanged(
    /* [in] */ Int32 playerId,
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom)
{
    if (mPlayerId != playerId) return;

    mLeft = left;
    mTop = top;
    mRight = right;
    mBottom = bottom;

    LayOutSurfaceView();
}

/**
 * Called when the page that contains the video element is scrolled or zoomed.
 */
//@CalledByNative
void ExternalVideoSurfaceContainer::OnFrameInfoUpdated()
{
    if (mPlayerId == INVALID_PLAYER_ID) return;

    LayOutSurfaceView();
}

void ExternalVideoSurfaceContainer::LayOutSurfaceView()
{
    AutoPtr<RenderCoordinates> renderCoordinates = mContentViewCore->GetRenderCoordinates();
    AutoPtr<RenderCoordinates::NormalizedPoint> topLeft = renderCoordinates->CreateNormalizedPoint();
    AutoPtr<RenderCoordinates::NormalizedPoint> bottomRight = renderCoordinates->CreateNormalizedPoint();
    topLeft->SetAbsoluteCss(mLeft, mTop);
    bottomRight->SetAbsoluteCss(mRight, mBottom);
    Float top = topLeft->GetYPix();
    Float left = topLeft->GetXPix();
    Float bottom = bottomRight->GetYPix();
    Float right = bottomRight->GetXPix();

    Int32 x = Elastos::Core::Math::Round(left + renderCoordinates->GetScrollXPix());
    Int32 y = Elastos::Core::Math::Round(top + renderCoordinates->GetScrollYPix());
    Int32 width = Elastos::Core::Math::Round(right - left);
    Int32 height = Elastos::Core::Math::Round(bottom - top);
    if (mX == x && mY == y && mWidth == width && mHeight == height) return;
    mX = x;
    mY = y;
    mWidth = width;
    mHeight = height;

    AutoPtr<IView> view = IView::Probe(mSurfaceView);
    view->SetX(x);
    view->SetY(y);
    AutoPtr<IViewGroupLayoutParams> layoutParams;
    view->GetLayoutParams((IViewGroupLayoutParams**)&layoutParams);
    layoutParams->SetWidth(width);
    layoutParams->SetHeight(height);
    view->RequestLayout();
}

// SurfaceHolder.Callback methods.
//@Override
ECode ExternalVideoSurfaceContainer::SurfaceChanged(
    /* [in] */ ISurfaceHolder* holder,
    /* [in] */ Int32 format,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    return NOERROR;
}

//@Override
// surfaceCreated() callback can be called regardless of requestExternalVideoSurface,
// if the activity comes back from the background and becomes visible.
ECode ExternalVideoSurfaceContainer::SurfaceCreated(
    /* [in] */ ISurfaceHolder* holder)
{
    if (mPlayerId != INVALID_PLAYER_ID) {
        AutoPtr<ISurface> surface;
        holder->GetSurface((ISurface**)&surface);
        NativeSurfaceCreated(
                mNativeExternalVideoSurfaceContainer, mPlayerId, surface);
    }

    return NOERROR;
}

// surfaceDestroyed() callback can be called regardless of releaseExternalVideoSurface,
// if the activity moves to the backgound and becomes invisible.
//@Override
ECode ExternalVideoSurfaceContainer::SurfaceDestroyed(
    /* [in] */ ISurfaceHolder* holder)
{
    if (mPlayerId != INVALID_PLAYER_ID) {
        NativeSurfaceDestroyed(mNativeExternalVideoSurfaceContainer, mPlayerId);
    }

    return NOERROR;
}

void ExternalVideoSurfaceContainer::NativeSurfaceCreated(
    /* [in] */ Handle64 nativeExternalVideoSurfaceContainerImpl,
    /* [in] */ Int32 playerId,
    /* [in] */ ISurface* surface)
{
    Elastos_ExternalVideoSurfaceContainer_nativeSurfaceCreated(TO_IINTERFACE(this),
            nativeExternalVideoSurfaceContainerImpl, playerId, TO_IINTERFACE(surface));
}

void ExternalVideoSurfaceContainer::NativeSurfaceDestroyed(
    /* [in] */ Handle64 nativeExternalVideoSurfaceContainerImpl,
    /* [in] */ Int32 playerId)
{
    Elastos_ExternalVideoSurfaceContainer_nativeSurfaceDestroyed(TO_IINTERFACE(this),
            nativeExternalVideoSurfaceContainerImpl, playerId);
}
//callback function definition
AutoPtr<IInterface> ExternalVideoSurfaceContainer::Create(
    /* [in] */ Handle64 nativeExternalVideoSurfaceContainer,
    /* [in] */ IInterface* contentViewCore)
{
    AutoPtr<ContentViewCore> c = (ContentViewCore*)(IObject::Probe(contentViewCore));
    return Create(nativeExternalVideoSurfaceContainer, c);
}

void ExternalVideoSurfaceContainer::RequestExternalVideoSurface(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 playerId)
{
    AutoPtr<ExternalVideoSurfaceContainer> mObj = (ExternalVideoSurfaceContainer*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("ExternalVideoSurfaceContainer", "ExternalVideoSurfaceContainer::RequestExternalVideoSurface, mObj is NULL");
        return;
    }
    mObj->RequestExternalVideoSurface(playerId);
}

void ExternalVideoSurfaceContainer::ReleaseExternalVideoSurface(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 playerId)
{
    AutoPtr<ExternalVideoSurfaceContainer> mObj = (ExternalVideoSurfaceContainer*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("ExternalVideoSurfaceContainer", "ExternalVideoSurfaceContainer::ReleaseExternalVideoSurface, mObj is NULL");
        return;
    }
    mObj->ReleaseExternalVideoSurface(playerId);
}

void ExternalVideoSurfaceContainer::Destroy(
    /* [in] */ IInterface* obj)
{
    AutoPtr<ExternalVideoSurfaceContainer> mObj = (ExternalVideoSurfaceContainer*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("ExternalVideoSurfaceContainer", "ExternalVideoSurfaceContainer::Destroy, mObj is NULL");
        return;
    }
    mObj->Destroy();
}

void ExternalVideoSurfaceContainer::OnExternalVideoSurfacePositionChanged(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 playerId,
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom)
{
    AutoPtr<ExternalVideoSurfaceContainer> mObj = (ExternalVideoSurfaceContainer*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("ExternalVideoSurfaceContainer", "ExternalVideoSurfaceContainer::OnExternalVideoSurfacePositionChanged, mObj is NULL");
        return;
    }
    mObj->OnExternalVideoSurfacePositionChanged(playerId, left, top, right, bottom);
}

void ExternalVideoSurfaceContainer::OnFrameInfoUpdated(
    /* [in] */ IInterface* obj)
{
    AutoPtr<ExternalVideoSurfaceContainer> mObj = (ExternalVideoSurfaceContainer*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("ExternalVideoSurfaceContainer", "ExternalVideoSurfaceContainer::OnFrameInfoUpdated, mObj is NULL");
        return;
    }
    mObj->OnFrameInfoUpdated();
}

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
