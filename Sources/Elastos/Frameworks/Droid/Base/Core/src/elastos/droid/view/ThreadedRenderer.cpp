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

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/view/ThreadedRenderer.h"
#include "elastos/droid/view/HardwareRenderer.h"
#include "elastos/droid/view/CRenderNodeHelper.h"
#include "elastos/droid/view/RenderNode.h"
#include "elastos/droid/view/HardwareLayer.h"
#include "elastos/droid/view/Choreographer.h"
#include "elastos/droid/view/Surface.h"
#include "elastos/droid/view/IGraphicsStats.h"
#include "elastos/droid/view/ViewRootImpl.h"
#include "elastos/droid/content/res/CResources.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/graphics/GraphicsNative.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/utility/TimeUtils.h"
#include "elastos/droid/R.h"

#include <elastos/core/StringUtils.h>
#include <elastos/utility/etl/Vector.h>
#include <elastos/utility/logging/Logger.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <EGL/egl_cache.h>

#include <utils/Looper.h>
#include <system/window.h>

#include <IContextFactory.h>
#include <hwui/AnimationContext.h>
#include <hwui/PropertyValuesAnimatorSet.h>
#include <hwui/RenderNode.h>
#include <hwui/renderthread/RenderProxy.h>
#include <gui/Surface.h>

using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::CResources;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::CBitmap;
using Elastos::Droid::Graphics::BitmapConfig;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::Graphics::BitmapWrapper;
using Elastos::Droid::Graphics::Drawable::IDrawableConstantState;
using Elastos::Droid::Graphics::Drawable::EIID_IDrawableConstantState;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Utility::IInt64SparseArray;
using Elastos::Droid::Utility::TimeUtils;
using Elastos::Droid::R;

using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::IInteger64;
using Elastos::Core::CInteger64;
using Elastos::Core::StringUtils;
using Elastos::IO::IFlushable;
using Elastos::Utility::IHashSet;
using Elastos::Utility::CHashSet;
using Elastos::Utility::Logging::Logger;

using namespace android;
using namespace android::uirenderer;
using namespace android::uirenderer::renderthread;

namespace Elastos {
namespace Droid {
namespace View {

//========================================================================================
//              ThreadedRenderer::
//========================================================================================

const String ThreadedRenderer::LOGTAG("ThreadedRenderer");

const Int32 ThreadedRenderer::SYNC_OK = 0;

const Int32 ThreadedRenderer::SYNC_INVALIDATE_REQUIRED = 1 << 0;

const Int32 ThreadedRenderer::SYNC_LOST_SURFACE_REWARD_IF_FOUND = 1 << 1;

static AutoPtr<ArrayOf<String> > InitArray()
{
    AutoPtr<ArrayOf<String> > result = ArrayOf<String>::Alloc(1);
    (*result)[0] = HardwareRenderer::PROFILE_PROPERTY_VISUALIZE_BARS;
    return result;
}

const AutoPtr<ArrayOf<String> > ThreadedRenderer::VISUALIZERS = InitArray();

ThreadedRenderer::ThreadedRenderer()
    : mWidth(0)
    , mHeight(0)
    , mSurfaceWidth(0)
    , mSurfaceHeight(0)
    , mInsetTop(0)
    , mInsetLeft(0)
    , mHasInsets(FALSE)
    , mLightY(0.f)
    , mLightZ(0.f)
    , mLightRadius(0.f)
    , mAmbientShadowAlpha(0)
    , mSpotShadowAlpha(0)
    , mNativeProxy(0)
    , mInitialized(FALSE)
    , mProfilingEnabled(FALSE)
    , mRootNodeNeedsUpdate(FALSE)
{}

ECode ThreadedRenderer::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Boolean translucent)
{
    AutoPtr<ITypedArray> a;
    AutoPtr<ArrayOf<Int32> > lightings = TO_ATTRS_ARRAYOF(R::styleable::Lighting);
    context->ObtainStyledAttributes(NULL, lightings, 0, 0, (ITypedArray**)&a);
    a->GetDimension(R::styleable::Lighting_lightY, 0, &mLightY);
    a->GetDimension(R::styleable::Lighting_lightZ, 0, &mLightZ);
    a->GetDimension(R::styleable::Lighting_lightRadius, 0, &mLightRadius);
    Float f1 = 0.f, f2 = 0.f;
    a->GetFloat(R::styleable::Lighting_ambientShadowAlpha, 0, &f1);
    mAmbientShadowAlpha = (Int32) (255 * f1 + 0.5f);
    a->GetFloat(R::styleable::Lighting_spotShadowAlpha, 0, &f2);
    mSpotShadowAlpha = (Int32) (255 * f2 + 0.5f);
    a->Recycle();

    Int64 rootNodePtr = NativeCreateRootRenderNode();
    AutoPtr<IRenderNodeHelper> hlp;
    CRenderNodeHelper::AcquireSingleton((IRenderNodeHelper**)&hlp);
    hlp->Adopt(rootNodePtr, (IRenderNode**)&mRootNode);
    Boolean r;
    mRootNode->SetClipToBounds(FALSE, &r);
    mNativeProxy = NativeCreateProxy(translucent, rootNodePtr);

    ProcessInitializer::sInstance->Init(context, mNativeProxy);

    Boolean tmp;
    LoadSystemProperties(&tmp);
    return NOERROR;
}

ECode ThreadedRenderer::Destroy()
{
    mInitialized = FALSE;
    UpdateEnabledState(NULL);
    NativeDestroy(mNativeProxy);
    return NOERROR;
}

void ThreadedRenderer::UpdateEnabledState(
    /* [in] */ ISurface* surface)
{
    Boolean vld = FALSE;
    if (surface == NULL || !(surface->IsValid(&vld), vld)) {
       SetEnabled(FALSE);
    }
    else {
       SetEnabled(mInitialized);
    }
}

// throws OutOfResourcesException
ECode ThreadedRenderer::Initialize(
    /* [in] */ ISurface* surface,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = FALSE;
    mInitialized = TRUE;
    UpdateEnabledState(surface);
    Boolean status = NativeInitialize(mNativeProxy, surface);
    FAIL_RETURN(surface->AllocateBuffers());
    *res = status;
    return NOERROR;
}

// throws OutOfResourcesException
ECode ThreadedRenderer::UpdateSurface(
    /* [in] */ ISurface* surface)
{
    UpdateEnabledState(surface);
    NativeUpdateSurface(mNativeProxy, surface);
    return NOERROR;
}

ECode ThreadedRenderer::PauseSurface(
    /* [in] */ ISurface* surface)
{
    NativePauseSurface(mNativeProxy, surface);
    return NOERROR;
}

ECode ThreadedRenderer::DestroyHardwareResources(
    /* [in] */ IView* view)
{
    DestroyResources(view);
    NativeDestroyHardwareResources(mNativeProxy);
    return NOERROR;
}

void ThreadedRenderer::DestroyResources(
    /* [in] */ IView* view)
{
    ((View*)view)->DestroyHardwareResources();

    IViewGroup* group = IViewGroup::Probe(view);
    if (group != NULL) {
        Int32 count = 0;
        group->GetChildCount(&count);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IView> v;
            group->GetChildAt(i, (IView**)&v);
            DestroyResources(v);
        }
    }
}

ECode ThreadedRenderer::Invalidate(
    /* [in] */ ISurface* surface)
{
    UpdateSurface(surface);
    return NOERROR;
}

ECode ThreadedRenderer::DetachSurfaceTexture(
    /* [in] */ Int64 hardwareLayer)
{
    NativeDetachSurfaceTexture(mNativeProxy, hardwareLayer);
    return NOERROR;
}

ECode ThreadedRenderer::Setup(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ IRect* surfaceInsets)
{
    Float lightX = width / 2.0f;
    mWidth = width;
    mHeight = height;
    Boolean bEmp = FALSE;
    if (surfaceInsets != NULL && !(surfaceInsets->IsEmpty(&bEmp), bEmp)) {
        mHasInsets = TRUE;
        AutoPtr<CRect> csi = (CRect*)surfaceInsets;
        mInsetLeft = csi->mLeft;
        mInsetTop = csi->mTop;
        mSurfaceWidth = width + mInsetLeft + csi->mRight;
        mSurfaceHeight = height + mInsetTop + csi->mBottom;

        // If the surface has insets, it can't be opaque.
        SetOpaque(FALSE);
    }
    else {
        mHasInsets = FALSE;
        mInsetLeft = 0;
        mInsetTop = 0;
        mSurfaceWidth = width;
        mSurfaceHeight = height;
    }
    Boolean res = FALSE;
    mRootNode->SetLeftTopRightBottom(-mInsetLeft, -mInsetTop, mSurfaceWidth, mSurfaceHeight, &res);
    NativeSetup(mNativeProxy, mSurfaceWidth, mSurfaceHeight,
            lightX, mLightY, mLightZ, mLightRadius,
            mAmbientShadowAlpha, mSpotShadowAlpha);
    return NOERROR;
}

ECode ThreadedRenderer::SetOpaque(
    /* [in] */ Boolean opaque)
{
    NativeSetOpaque(mNativeProxy, opaque && !mHasInsets);
    return NOERROR;
}

ECode ThreadedRenderer::GetWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width)
    *width = mWidth;
    return NOERROR;
}

ECode ThreadedRenderer::GetHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height)
    *height = mHeight;
    return NOERROR;
}

ECode ThreadedRenderer::DumpGfxInfo(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ IFileDescriptor* fd)
{
    IFlushable::Probe(pw)->Flush();
    NativeDumpProfileInfo(mNativeProxy, fd);
    return NOERROR;
}

Int32 ThreadedRenderer::Search(
    /* [in] */ ArrayOf<String>* values,
    /* [in] */ const String& value)
{
    for (Int32 i = 0; i < values->GetLength(); i++) {
        if ((*values)[i].Equals(value)) return i;
    }
    return -1;
}

Boolean ThreadedRenderer::CheckIfProfilingRequested()
{
    String profiling;
    SystemProperties::Get(HardwareRenderer::PROFILE_PROPERTY, &profiling);
    Int32 graphType = Search(VISUALIZERS, profiling);
    return (graphType >= 0) || StringUtils::ParseBoolean(profiling);
}

ECode ThreadedRenderer::LoadSystemProperties(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    Boolean changed = NativeLoadSystemProperties(mNativeProxy);
    Boolean wantProfiling = CheckIfProfilingRequested();
    if (wantProfiling != mProfilingEnabled) {
        mProfilingEnabled = wantProfiling;
        changed = TRUE;
    }
    *res = changed;
    return NOERROR;
}

void ThreadedRenderer::UpdateViewTreeDisplayList(
    /* [in] */ IView* view)
{
    View* cv = (View*)view;
    cv->mPrivateFlags |= View::PFLAG_DRAWN;
    cv->mRecreateDisplayList =
        (cv->mPrivateFlags & View::PFLAG_INVALIDATED) == View::PFLAG_INVALIDATED;
    cv->mPrivateFlags &= ~View::PFLAG_INVALIDATED;
    AutoPtr<IRenderNode> nodes;
    view->GetDisplayList((IRenderNode**)&nodes);
    cv->mRecreateDisplayList = FALSE;
}

void ThreadedRenderer::UpdateRootDisplayList(
    /* [in] */ IView* view,
    /* [in] */ IHardwareDrawCallbacks* callbacks)
{
//    Trace::TraceBegin(Trace::TRACE_TAG_VIEW, "getDisplayList");
    UpdateViewTreeDisplayList(view);

    Boolean bval = FALSE;
    if (mRootNodeNeedsUpdate || (mRootNode->IsValid(&bval), !bval)) {
        AutoPtr<IHardwareCanvas> canvas;
        mRootNode->Start(mSurfaceWidth, mSurfaceHeight, (IHardwareCanvas**)&canvas);
        Int32 saveCount = 0;
        ICanvas* c = ICanvas::Probe(canvas);
        c->Save(&saveCount);
        c->Translate(mInsetLeft, mInsetTop);
        callbacks->OnHardwarePreDraw(canvas);

        c->InsertReorderBarrier();
        AutoPtr<IRenderNode> dl;
        view->GetDisplayList((IRenderNode**)&dl);
        canvas->DrawRenderNode(dl);
        c->InsertInorderBarrier();

        callbacks->OnHardwarePostDraw(canvas);
        c->RestoreToCount(saveCount);
        mRootNodeNeedsUpdate = FALSE;

        mRootNode->End(canvas);
    }
//    Trace::TraceEnd(Trace::TRACE_TAG_VIEW);
}

ECode ThreadedRenderer::InvalidateRoot()
{
    mRootNodeNeedsUpdate = TRUE;
    return NOERROR;
}

ECode ThreadedRenderer::Draw(
    /* [in] */ IView* view,
    /* [in] */ View::AttachInfo* attachInfo,
    /* [in] */ IHardwareDrawCallbacks* callbacks)
{
    attachInfo->mIgnoreDirtyState = TRUE;

    Choreographer* choreographer = (Choreographer*)((ViewRootImpl*)attachInfo->mViewRootImpl)->mChoreographer.Get();
    choreographer->mFrameInfo->MarkDrawStart();
    Int64 frameTimeNanos = 0;
    choreographer->GetFrameTimeNanos(&frameTimeNanos);
    attachInfo->mDrawingTime = frameTimeNanos / TimeUtils::NANOS_PER_MS;

    Int64 recordDuration = 0;

    if (mProfilingEnabled) {
        AutoPtr<ISystem> system;
        Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
        Int64 nanoTime;
        system->GetNanoTime(&nanoTime);
        recordDuration = nanoTime;
    }

    UpdateRootDisplayList(view, callbacks);

    if (mProfilingEnabled) {
        AutoPtr<ISystem> system;
        Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
        Int64 nanoTime;
        system->GetNanoTime(&nanoTime);
        recordDuration = nanoTime - recordDuration;
    }

    attachInfo->mIgnoreDirtyState = FALSE;

    // register animating rendernodes which started animating prior to renderer
    // creation, which is typical for animators started prior to first draw
    if (attachInfo->mPendingAnimatingRenderNodes != NULL) {
        Int32 count;
        attachInfo->mPendingAnimatingRenderNodes->GetSize(&count);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IInterface> tmp;
            attachInfo->mPendingAnimatingRenderNodes->Get(i, (IInterface**)&tmp);
            RegisterAnimatingRenderNode(IRenderNode::Probe(tmp));
        }
        attachInfo->mPendingAnimatingRenderNodes->Clear();
        // We don't need this anymore as subsequent calls to
        // ViewRootImpl#attachRenderNodeAnimator will go directly to us.
        attachInfo->mPendingAnimatingRenderNodes = NULL;
    }

    AutoPtr< ArrayOf<Int64> > frameInfo = choreographer->mFrameInfo->mFrameInfo;
    Int32 syncResult = NativeSyncAndDrawFrame(mNativeProxy, frameInfo, frameInfo->GetLength());
    if ((syncResult & SYNC_LOST_SURFACE_REWARD_IF_FOUND) != 0) {
        assert(0);
        // setEnabled(false);
        // attachInfo.mViewRootImpl.mSurface.release();
        // Invalidate since we failed to draw. This should fetch a Surface
        // if it is still needed or do nothing if we are no longer drawing
        attachInfo->mViewRootImpl->Invalidate();
    }
    if ((syncResult & SYNC_INVALIDATE_REQUIRED) != 0) {
        attachInfo->mViewRootImpl->Invalidate();
    }
    return NOERROR;
}

ECode ThreadedRenderer::InvokeFunctor(
    /* [in] */ Int64 functor,
    /* [in] */ Boolean waitForCompletion)
{
    NativeInvokeFunctor(functor, waitForCompletion);
    return NOERROR;
}

ECode ThreadedRenderer::CreateTextureLayer(
    /* [out] */ IHardwareLayer** layer)
{
    VALIDATE_NOT_NULL(layer)
    Int64 nLayer = NativeCreateTextureLayer(mNativeProxy);
    AutoPtr<IHardwareLayer> hl = HardwareLayer::AdoptTextureLayer(this, nLayer);
    *layer = hl;
    REFCOUNT_ADD(*layer)
    return NOERROR;
}

ECode ThreadedRenderer::BuildLayer(
    /* [in] */ IRenderNode* node)
{
    RenderNode* nodeImpl = (RenderNode*)node;
    Int64 dl = 0;
    nodeImpl->GetNativeDisplayList(&dl);
    NativeBuildLayer(mNativeProxy, dl);
    return NOERROR;
}

ECode ThreadedRenderer::CopyLayerInto(
    /* [in] */ IHardwareLayer* layer,
    /* [in] */ IBitmap* bitmap,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    Int64 dlu = 0;
    layer->GetDeferredLayerUpdater(&dlu);
    AutoPtr<CBitmap> cbmp = (CBitmap*)bitmap;
    *res = NativeCopyLayerInto(mNativeProxy,
            dlu, cbmp->mNativeBitmap);
    return NOERROR;
}

ECode ThreadedRenderer::PushLayerUpdate(
    /* [in] */ IHardwareLayer* layer)
{
    Int64 dlu = 0;
    layer->GetDeferredLayerUpdater(&dlu);
    NativePushLayerUpdate(mNativeProxy, dlu);
    return NOERROR;
}

ECode ThreadedRenderer::OnLayerDestroyed(
    /* [in] */ IHardwareLayer* layer)
{
    Int64 dlu = 0;
    layer->GetDeferredLayerUpdater(&dlu);
    NativeCancelLayerUpdate(mNativeProxy, dlu);
    return NOERROR;
}

ECode ThreadedRenderer::SetName(
    /* [in] */ const String& name)
{
    return NOERROR;
}

ECode ThreadedRenderer::Fence()
{
    NativeFence(mNativeProxy);
    return NOERROR;
}

ECode ThreadedRenderer::StopDrawing()
{
    NativeStopDrawing(mNativeProxy);
    return NOERROR;
}

ECode ThreadedRenderer::NotifyFramePending()
{
    NativeNotifyFramePending(mNativeProxy);
    return NOERROR;
}

ECode ThreadedRenderer::RegisterAnimatingRenderNode(
    /* [in] */ IRenderNode* animator)
{
    Int64 nativeRenderNode1 = ((RenderNode*)mRootNode.Get())->mNativeRenderNode;
    Int64 nativeRenderNode2 = ((RenderNode*)animator)->mNativeRenderNode;
    NativeRegisterAnimatingRenderNode(nativeRenderNode1, nativeRenderNode2);
    return NOERROR;
}

// throws Throwable
ThreadedRenderer::~ThreadedRenderer()
{
    if (mNativeProxy != 0) {
        NativeDeleteProxy(mNativeProxy);
        mNativeProxy = 0;
    }
}

ECode ThreadedRenderer::TrimMemory(
    /* [in] */ Int32 level)
{
    NativeTrimMemory(level);
    return NOERROR;
}


//========================================================================================
//              ThreadedRenderer::ProcessInitializer::IGraphicsStatsCallbackStub
//========================================================================================

CAR_INTERFACE_IMPL(ThreadedRenderer::ProcessInitializer::IGraphicsStatsCallbackStub,
        Object, IIGraphicsStatsCallback);

ThreadedRenderer::ProcessInitializer::IGraphicsStatsCallbackStub::IGraphicsStatsCallbackStub(
    /* [in] */ ProcessInitializer* host)
    : mHost(host)
{}

ECode ThreadedRenderer::ProcessInitializer::IGraphicsStatsCallbackStub::OnRotateGraphicsStatsBuffer()
{
    mHost->RotateBuffer();
    return NOERROR;
}

void ThreadedRenderer::ProcessInitializer::IGraphicsStatsCallbackStub::onLastStrongRef(
    /* [in] */ const void* id)
{}

android::status_t ThreadedRenderer::ProcessInitializer::IGraphicsStatsCallbackStub::onTransact(
    /* [in] */ uint32_t code,
    /* [in] */ const android::Parcel& data,
    /* [out] */ android::Parcel* reply,
    /* [in] */ uint32_t flags)
{
    assert(0);
    return android::BBinder::onTransact(code, data, reply, flags);
}


//========================================================================================
//              ThreadedRenderer::ProcessInitializer::
//========================================================================================

AutoPtr<ThreadedRenderer::ProcessInitializer> ThreadedRenderer::ProcessInitializer::sInstance =
        new ProcessInitializer();

ThreadedRenderer::ProcessInitializer::ProcessInitializer()
    : mInitialized(FALSE)
{
    mGraphicsStatsCallback = new IGraphicsStatsCallbackStub(this);
}

void ThreadedRenderer::ProcessInitializer::Init(
    /* [in] */ IContext* context,
    /* [in] */ Int64 renderProxy)
{
    if (mInitialized) return;
    mInitialized = TRUE;
    context->GetApplicationContext((IContext**)&mAppContext);
    InitSched(context, renderProxy);
    InitGraphicsStats();
}

void ThreadedRenderer::ProcessInitializer::InitSched(
    /* [in] */ IContext* context,
    /* [in] */ Int64 renderProxy)
{
    // try {
    Int32 tid = ThreadedRenderer::NativeGetRenderThreadTid(renderProxy);
    ActivityManagerNative::GetDefault()->SetRenderThread(tid);
    // } catch (Throwable t) {
    //     Log.w(LOG_TAG, "Failed to set scheduler for RenderThread", t);
    // }
}

void ThreadedRenderer::ProcessInitializer::InitGraphicsStats()
{
    // try {
    android::sp<android::IBinder> binder = ServiceManager::GetAndroidService(String("graphicsstats"));
    assert(binder != NULL);
    if (binder == NULL) return;
    mGraphicsStatsService = new IGrphicsStatsProxy(binder);
    RequestBuffer();
    // } catch (Throwable t) {
    //     Log.w(LOG_TAG, "Could not acquire gfx stats buffer", t);
    // }
}

void ThreadedRenderer::ProcessInitializer::RotateBuffer()
{
    ThreadedRenderer::NativeRotateProcessStatsBuffer();
    RequestBuffer();
}

void ThreadedRenderer::ProcessInitializer::RequestBuffer()
{
    // try {
    AutoPtr<IApplicationInfo> appInfo;
    mAppContext->GetApplicationInfo((IApplicationInfo**)&appInfo);
    String pkg;
    IPackageItemInfo::Probe(appInfo)->GetPackageName(&pkg);
    AutoPtr<IParcelFileDescriptor> pfd;
    mGraphicsStatsService->RequestBufferForProcess(pkg, mGraphicsStatsCallback,
            (IParcelFileDescriptor**)&pfd);
    Int32 fd;
    pfd->GetFd(&fd);
    NativeSetProcessStatsBuffer(fd);
    pfd->Close();
    // } catch (Throwable t) {
    //     Log.w(LOG_TAG, "Could not acquire gfx stats buffer", t);
    // }
}


//------------Native Methord-----------


class OnFinishedEvent
{
public:
    OnFinishedEvent(BaseRenderNodeAnimator* animator, AnimationListener* listener)
            : animator(animator), listener(listener) {}
    sp<BaseRenderNodeAnimator> animator;
    sp<AnimationListener> listener;
};

class InvokeAnimationListeners : public MessageHandler
{
public:
    InvokeAnimationListeners(std::vector<OnFinishedEvent>& events) {
        mOnFinishedEvents.swap(events);
    }

    static void callOnFinished(OnFinishedEvent& event) {
        event.listener->onAnimationFinished(event.animator.get());
    }

    virtual void handleMessage(const Message& message) {
        std::for_each(mOnFinishedEvents.begin(), mOnFinishedEvents.end(), callOnFinished);
        mOnFinishedEvents.clear();
    }

private:
    std::vector<OnFinishedEvent> mOnFinishedEvents;
};

class FinishAndInvokeListener : public MessageHandler
{
public:
    explicit FinishAndInvokeListener(PropertyValuesAnimatorSet* anim)
            : mAnimator(anim) {
        mListener = anim->getOneShotListener();
        mRequestId = anim->getRequestId();
    }

    virtual void handleMessage(const Message& message) {
        if (mAnimator->getRequestId() == mRequestId) {
            // Request Id has not changed, meaning there's no animation lifecyle change since the
            // message is posted, so go ahead and call finish to make sure the PlayState is properly
            // updated. This is needed because before the next frame comes in from UI thread to
            // trigger an animation update, there could be reverse/cancel etc. So we need to update
            // the playstate in time to ensure all the subsequent events get chained properly.
            mAnimator->end();
        }
        mListener->onAnimationFinished(nullptr);
    }
private:
    sp<PropertyValuesAnimatorSet> mAnimator;
    sp<AnimationListener> mListener;
    uint32_t mRequestId;
};

class RenderingException : public MessageHandler
{
public:
    RenderingException(const std::string& message)
            : mMessage(message) {
    }

    virtual void handleMessage(const Message&) {
        throwException(mMessage);
    }

    static void throwException(const std::string& message) {
        Logger::E("ThreadedRenderer", message.c_str());
    }

private:
    std::string mMessage;
};

class RootRenderNode : public android::uirenderer::RenderNode, ErrorHandler
{
public:
    RootRenderNode() : RenderNode() {
        mLooper = Looper::getForThread();
        LOG_ALWAYS_FATAL_IF(!mLooper.get(),
                "Must create RootRenderNode on a thread with a looper!");
    }

    virtual ~RootRenderNode() {}

    virtual void onError(const std::string& message) {
        mLooper->sendMessage(new RenderingException(message), 0);
    }

    virtual void prepareTree(TreeInfo& info) {
        info.errorHandler = this;

        for (auto& anim : mRunningVDAnimators) {
            // Assume that the property change in VD from the animators will not be consumed. Mark
            // otherwise if the VDs are found in the display list tree. For VDs that are not in
            // the display list tree, we stop providing animation pulses by 1) removing them from
            // the animation list, 2) post a delayed message to end them at end time so their
            // listeners can receive the corresponding callbacks.
            anim->getVectorDrawable()->setPropertyChangeWillBeConsumed(false);
            // Mark the VD dirty so it will damage itself during prepareTree.
            anim->getVectorDrawable()->markDirty();
        }
        if (info.mode == TreeInfo::MODE_FULL) {
            for (auto &anim : mPausedVDAnimators) {
                anim->getVectorDrawable()->setPropertyChangeWillBeConsumed(false);
                anim->getVectorDrawable()->markDirty();
            }
        }
        // TODO: This is hacky
        info.updateWindowPositions = true;
        RenderNode::prepareTree(info);
        info.updateWindowPositions = false;
        info.errorHandler = nullptr;
    }

    void sendMessage(const sp<MessageHandler>& handler) {
        mLooper->sendMessage(handler, 0);
    }

    void sendMessageDelayed(const sp<MessageHandler>& handler, nsecs_t delayInMs) {
        mLooper->sendMessageDelayed(ms2ns(delayInMs), handler, 0);
    }

    void attachAnimatingNode(RenderNode* animatingNode) {
        mPendingAnimatingRenderNodes.push_back(animatingNode);
    }

    void attachPendingVectorDrawableAnimators() {
        mRunningVDAnimators.insert(mPendingVectorDrawableAnimators.begin(),
                mPendingVectorDrawableAnimators.end());
        mPendingVectorDrawableAnimators.clear();
    }

    void detachAnimators() {
        // Remove animators from the list and post a delayed message in future to end the animator
        // For infinite animators, remove the listener so we no longer hold a global ref to the AVD
        // java object, and therefore the AVD objects in both native and Java can be properly
        // released.
        for (auto& anim : mRunningVDAnimators) {
            detachVectorDrawableAnimator(anim.get());
            anim->clearOneShotListener();
        }
        for (auto& anim : mPausedVDAnimators) {
            anim->clearOneShotListener();
        }
        mRunningVDAnimators.clear();
        mPausedVDAnimators.clear();
    }

    // Move all the animators to the paused list, and send a delayed message to notify the finished
    // listener.
    void pauseAnimators() {
        mPausedVDAnimators.insert(mRunningVDAnimators.begin(), mRunningVDAnimators.end());
        for (auto& anim : mRunningVDAnimators) {
            detachVectorDrawableAnimator(anim.get());
        }
        mRunningVDAnimators.clear();
    }

    void doAttachAnimatingNodes(AnimationContext* context) {
        for (size_t i = 0; i < mPendingAnimatingRenderNodes.size(); i++) {
            RenderNode* node = mPendingAnimatingRenderNodes[i].get();
            context->addAnimatingRenderNode(*node);
        }
        mPendingAnimatingRenderNodes.clear();
    }

    // Run VectorDrawable animators after prepareTree.
    void runVectorDrawableAnimators(AnimationContext* context, TreeInfo& info) {
        // Push staging.
        if (info.mode == TreeInfo::MODE_FULL) {
            pushStagingVectorDrawableAnimators(context);
        }

        // Run the animators in the running list.
        for (auto it = mRunningVDAnimators.begin(); it != mRunningVDAnimators.end();) {
            if ((*it)->animate(*context)) {
                it = mRunningVDAnimators.erase(it);
            } else {
                it++;
            }
        }

        // Run the animators in paused list during full sync.
        if (info.mode == TreeInfo::MODE_FULL) {
            // During full sync we also need to pulse paused animators, in case their targets
            // have been added back to the display list. All the animators that passed the
            // scheduled finish time will be removed from the paused list.
            for (auto it = mPausedVDAnimators.begin(); it != mPausedVDAnimators.end();) {
                if ((*it)->animate(*context)) {
                    // Animator has finished, remove from the list.
                    it = mPausedVDAnimators.erase(it);
                } else {
                    it++;
                }
            }
        }

        // Move the animators with a target not in DisplayList to paused list.
        for (auto it = mRunningVDAnimators.begin(); it != mRunningVDAnimators.end();) {
            if (!(*it)->getVectorDrawable()->getPropertyChangeWillBeConsumed()) {
                // Vector Drawable is not in the display list, we should remove this animator from
                // the list, put it in the paused list, and post a delayed message to end the
                // animator.
                detachVectorDrawableAnimator(it->get());
                mPausedVDAnimators.insert(*it);
                it = mRunningVDAnimators.erase(it);
            } else {
                it++;
            }
        }

        // Move the animators with a target in DisplayList from paused list to running list, and
        // trim paused list.
        if (info.mode == TreeInfo::MODE_FULL) {
            // Check whether any paused animator's target is back in Display List. If so, put the
            // animator back in the running list.
            for (auto it = mPausedVDAnimators.begin(); it != mPausedVDAnimators.end();) {
                if ((*it)->getVectorDrawable()->getPropertyChangeWillBeConsumed()) {
                    mRunningVDAnimators.insert(*it);
                    it = mPausedVDAnimators.erase(it);
                } else {
                    it++;
                }
            }
            // Trim paused VD animators at full sync, so that when Java loses reference to an
            // animator, we know we won't be requested to animate it any more, then we remove such
            // animators from the paused list so they can be properly freed. We also remove the
            // animators from paused list when the time elapsed since start has exceeded duration.
            trimPausedVDAnimators(context);
        }

        info.out.hasAnimations |= !mRunningVDAnimators.empty();
    }

    void trimPausedVDAnimators(AnimationContext* context) {
        // Trim paused vector drawable animator list.
        for (auto it = mPausedVDAnimators.begin(); it != mPausedVDAnimators.end();) {
            // Remove paused VD animator if no one else is referencing it. Note that animators that
            // have passed scheduled finish time are removed from list when they are being pulsed
            // before prepare tree.
            // TODO: this is a bit hacky, need to figure out a better way to track when the paused
            // animators should be freed.
            if ((*it)->getStrongCount() == 1) {
                it = mPausedVDAnimators.erase(it);
            } else {
                it++;
            }
        }
    }

    void pushStagingVectorDrawableAnimators(AnimationContext* context) {
        for (auto& anim : mRunningVDAnimators) {
            anim->pushStaging(*context);
        }
    }

    void destroy() {
        for (auto& renderNode : mPendingAnimatingRenderNodes) {
            renderNode->animators().endAllStagingAnimators();
        }
        mPendingAnimatingRenderNodes.clear();
        mPendingVectorDrawableAnimators.clear();
    }

    void addVectorDrawableAnimator(PropertyValuesAnimatorSet* anim) {
        mPendingVectorDrawableAnimators.insert(anim);
    }

private:
    sp<Looper> mLooper;
    std::vector< sp<RenderNode> > mPendingAnimatingRenderNodes;
    std::set< sp<PropertyValuesAnimatorSet> > mPendingVectorDrawableAnimators;
    std::set< sp<PropertyValuesAnimatorSet> > mRunningVDAnimators;
    // mPausedVDAnimators stores a list of animators that have not yet passed the finish time, but
    // their VectorDrawable targets are no longer in the DisplayList. We skip these animators when
    // render thread runs animators independent of UI thread (i.e. RT_ONLY mode). These animators
    // need to be re-activated once their VD target is added back into DisplayList. Since that could
    // only happen when we do a full sync, we need to make sure to pulse these paused animators at
    // full sync. If any animator's VD target is found in DisplayList during a full sync, we move
    // the animator back to the running list.
    std::set< sp<PropertyValuesAnimatorSet> > mPausedVDAnimators;
    void detachVectorDrawableAnimator(PropertyValuesAnimatorSet* anim) {
        if (anim->isInfinite() || !anim->isRunning()) {
            // Do not need to post anything if the animation is infinite (i.e. no meaningful
            // end listener action), or if the animation has already ended.
            return;
        }
        nsecs_t remainingTimeInMs = anim->getRemainingPlayTime();
        // Post a delayed onFinished event that is scheduled to be handled when the animator ends.
        if (anim->getOneShotListener()) {
            // VectorDrawable's oneshot listener is updated when there are user triggered animation
            // lifecycle changes, such as start(), end(), etc. By using checking and clearing
            // one shot listener, we ensure the same end listener event gets posted only once.
            // Therefore no duplicates. Another benefit of using one shot listener is that no
            // removal is necessary: the end time of animation will not change unless triggered by
            // user events, in which case the already posted listener's id will become stale, and
            // the onFinished callback will then be ignored.
            sp<FinishAndInvokeListener> message
                    = new FinishAndInvokeListener(anim);
            sendMessageDelayed(message, remainingTimeInMs);
            anim->clearOneShotListener();
        }
    }
};

class AnimationContextBridge : public AnimationContext {
public:
    AnimationContextBridge(renderthread::TimeLord& clock, RootRenderNode* rootNode)
            : AnimationContext(clock), mRootNode(rootNode) {
    }

    virtual ~AnimationContextBridge() {}

    // Marks the start of a frame, which will update the frame time and move all
    // next frame animations into the current frame
    virtual void startFrame(TreeInfo::TraversalMode mode) {
        if (mode == TreeInfo::MODE_FULL) {
            mRootNode->doAttachAnimatingNodes(this);
            mRootNode->attachPendingVectorDrawableAnimators();
        }
        AnimationContext::startFrame(mode);
    }

    // Runs any animations still left in mCurrentFrameAnimations
    virtual void runRemainingAnimations(TreeInfo& info) {
        AnimationContext::runRemainingAnimations(info);
        mRootNode->runVectorDrawableAnimators(this, info);
        postOnFinishedEvents();
    }

    virtual void pauseAnimators() override {
        mRootNode->pauseAnimators();
    }

    virtual void callOnFinished(BaseRenderNodeAnimator* animator, AnimationListener* listener) {
        OnFinishedEvent event(animator, listener);
        mOnFinishedEvents.push_back(event);
    }

    virtual void destroy() {
        AnimationContext::destroy();
        mRootNode->detachAnimators();
        postOnFinishedEvents();
    }

private:
    sp<RootRenderNode> mRootNode;
    std::vector<OnFinishedEvent> mOnFinishedEvents;

    void postOnFinishedEvents() {
        if (mOnFinishedEvents.size()) {
            sp<InvokeAnimationListeners> message
                    = new InvokeAnimationListeners(mOnFinishedEvents);
            mRootNode->sendMessage(message);
        }
    }
};

class ContextFactoryImpl : public IContextFactory
{
public:
    ContextFactoryImpl(RootRenderNode* rootNode) : mRootNode(rootNode) {}

    virtual AnimationContext* createAnimationContext(renderthread::TimeLord& clock) {
        return new AnimationContextBridge(clock, mRootNode);
    }

private:
    RootRenderNode* mRootNode;
};

ECode ThreadedRenderer::NativeSetupShadersDiskCache(
    /* [in] */ const String& cacheFile)
{
    egl_cache_t::get()->setCacheFilename(cacheFile.string());
    return NOERROR;
}

Int64 ThreadedRenderer::NativeCreateRootRenderNode()
{
    RootRenderNode* node = new RootRenderNode();
    node->incStrong(0);
    node->setName("RootRenderNode");
    return reinterpret_cast<Int64>(node);
}

Int64 ThreadedRenderer::NativeCreateProxy(
    /* [in] */ Boolean translucent,
    /* [in] */ Int64 rootRenderNodePtr)
{
    RootRenderNode* rootRenderNode = reinterpret_cast<RootRenderNode*>(rootRenderNodePtr);
    ContextFactoryImpl factory(rootRenderNode);
    RenderProxy* rp = new RenderProxy(translucent, rootRenderNode, &factory);
    return reinterpret_cast<Int64>(rp);
}

void ThreadedRenderer::NativeDeleteProxy(
    /* [in] */ Int64 nativeProxy)
{
    RenderProxy* proxy = reinterpret_cast<RenderProxy*>(nativeProxy);
    delete proxy;
}

Boolean ThreadedRenderer::NativeLoadSystemProperties(
    /* [in] */ Int64 nativeProxy)
{
    RenderProxy* proxy = reinterpret_cast<RenderProxy*>(nativeProxy);
    return proxy->loadSystemProperties();
}

Boolean ThreadedRenderer::NativeInitialize(
    /* [in] */ Int64 nativeProxy,
    /* [in] */ ISurface* window)
{
    RenderProxy* proxy = reinterpret_cast<RenderProxy*>(nativeProxy);
    Int64 nativeSurf;
    window->GetNativeSurface(&nativeSurf);
    sp<android::Surface> surface = reinterpret_cast<android::Surface*>(nativeSurf);
    proxy->initialize(surface);
    return TRUE;
}

void ThreadedRenderer::NativeUpdateSurface(
    /* [in] */ Int64 nativeProxy,
    /* [in] */ ISurface* window)
{
    RenderProxy* proxy = reinterpret_cast<RenderProxy*>(nativeProxy);
    sp<android::Surface> surface;
    if (window) {
        Int64 nativeSurf;
        window->GetNativeSurface(&nativeSurf);
        surface = reinterpret_cast<android::Surface*>(nativeSurf);
    }
    proxy->updateSurface(surface);
}

void ThreadedRenderer::NativePauseSurface(
    /* [in] */ Int64 nativeProxy,
    /* [in] */ ISurface* window)
{
    RenderProxy* proxy = reinterpret_cast<RenderProxy*>(nativeProxy);
    sp<android::Surface> surface;
    if (window) {
        Int64 nativeSurf;
        window->GetNativeSurface(&nativeSurf);
        surface = reinterpret_cast<android::Surface*>(nativeSurf);
    }
    proxy->pauseSurface(surface);
}

void ThreadedRenderer::NativeSetup(
    /* [in] */ Int64 nativeProxy,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Float lightX,
    /* [in] */ Float lightY,
    /* [in] */ Float lightZ,
    /* [in] */ Float lightRadius,
    /* [in] */ Int32 ambientShadowAlpha,
    /* [in] */ Int32 spotShadowAlpha)
{
    RenderProxy* proxy = reinterpret_cast<RenderProxy*>(nativeProxy);
    proxy->setup(lightRadius, ambientShadowAlpha, spotShadowAlpha);
    proxy->setLightCenter((Vector3){lightX, lightY, lightZ});
}

void ThreadedRenderer::NativeSetOpaque(
    /* [in] */ Int64 nativeProxy,
    /* [in] */ Boolean opaque)
{
    RenderProxy* proxy = reinterpret_cast<RenderProxy*>(nativeProxy);
    proxy->setOpaque(opaque);
}

Int32 ThreadedRenderer::NativeSyncAndDrawFrame(
    /* [in] */ Int64 nativeProxy,
    /* [in] */ ArrayOf<Int64>* frameInfo,
    /* [in] */ Int32 size)
{
    RenderProxy* proxy = reinterpret_cast<RenderProxy*>(nativeProxy);
    int64_t* buf = proxy->frameInfo();
    for (Int32 i = 0; i < size; i++) {
        buf[i] = (*frameInfo)[i];
    }
    return proxy->syncAndDrawFrame();
}

void ThreadedRenderer::NativeDestroy(
    /* [in] */ Int64 nativeProxy)
{
    RenderProxy* proxy = reinterpret_cast<RenderProxy*>(nativeProxy);
    proxy->destroy();
}

void ThreadedRenderer::NativeRegisterAnimatingRenderNode(
    /* [in] */ Int64 rootNodePtr,
    /* [in] */ Int64 animatingNodePtr)
{
    RootRenderNode* rootRenderNode = reinterpret_cast<RootRenderNode*>(rootNodePtr);
    android::uirenderer::RenderNode* animatingNode = reinterpret_cast<android::uirenderer::RenderNode*>(animatingNodePtr);
    rootRenderNode->attachAnimatingNode(animatingNode);
}

void ThreadedRenderer::NativeInvokeFunctor(
    /* [in] */ Int64 functorPtr,
    /* [in] */ Boolean waitForCompletion)
{
    Functor* functor = reinterpret_cast<Functor*>(functorPtr);
    RenderProxy::invokeFunctor(functor, waitForCompletion);
}

Int64 ThreadedRenderer::NativeCreateTextureLayer(
    /* [in] */ Int64 nativeProxy)
{
    RenderProxy* proxy = reinterpret_cast<RenderProxy*>(nativeProxy);
    DeferredLayerUpdater* layer = proxy->createTextureLayer();
    return reinterpret_cast<Int64>(layer);
}

void ThreadedRenderer::NativeBuildLayer(
    /* [in] */ Int64 nativeProxy,
    /* [in] */ Int64 nodePtr)
{
    RenderProxy* proxy = reinterpret_cast<RenderProxy*>(nativeProxy);
    android::uirenderer::RenderNode* node = reinterpret_cast<android::uirenderer::RenderNode*>(nodePtr);
    proxy->buildLayer(node);
}

Boolean ThreadedRenderer::NativeCopyLayerInto(
    /* [in] */ Int64 nativeProxy,
    /* [in] */ Int64 layerPtr,
    /* [in] */ Int64 bitmapPtr)
{
    RenderProxy* proxy = reinterpret_cast<RenderProxy*>(nativeProxy);
    DeferredLayerUpdater* layer = reinterpret_cast<DeferredLayerUpdater*>(layerPtr);
    SkBitmap bitmap;
    reinterpret_cast<BitmapWrapper*>(bitmapPtr)->getSkBitmap(&bitmap);
    return proxy->copyLayerInto(layer, bitmap);
}

void ThreadedRenderer::NativePushLayerUpdate(
    /* [in] */ Int64 proxyPtr,
    /* [in] */ Int64 layerPtr)
{
    RenderProxy* proxy = reinterpret_cast<RenderProxy*>(proxyPtr);
    DeferredLayerUpdater* layer = reinterpret_cast<DeferredLayerUpdater*>(layerPtr);
    proxy->pushLayerUpdate(layer);
}

void ThreadedRenderer::NativeCancelLayerUpdate(
    /* [in] */ Int64 proxyPtr,
    /* [in] */ Int64 layerPtr)
{
    RenderProxy* proxy = reinterpret_cast<RenderProxy*>(proxyPtr);
    DeferredLayerUpdater* layer = reinterpret_cast<DeferredLayerUpdater*>(layerPtr);
    proxy->cancelLayerUpdate(layer);
}

void ThreadedRenderer::NativeDetachSurfaceTexture(
    /* [in] */ Int64 proxyPtr,
    /* [in] */ Int64 layerPtr)
{
    RenderProxy* proxy = reinterpret_cast<RenderProxy*>(proxyPtr);
    DeferredLayerUpdater* layer = reinterpret_cast<DeferredLayerUpdater*>(layerPtr);
    proxy->detachSurfaceTexture(layer);
}

void ThreadedRenderer::NativeDestroyHardwareResources(
    /* [in] */ Int64 proxyPtr)
{
    RenderProxy* proxy = reinterpret_cast<RenderProxy*>(proxyPtr);
    proxy->destroyHardwareResources();
}

void ThreadedRenderer::NativeTrimMemory(
    /* [in] */ Int32 level)
{
    RenderProxy::trimMemory(level);
}

void ThreadedRenderer::NativeFence(
    /* [in] */ Int64 proxyPtr)
{
    RenderProxy* proxy = reinterpret_cast<RenderProxy*>(proxyPtr);
    proxy->fence();
}

void ThreadedRenderer::NativeStopDrawing(
    /* [in] */ Int64 proxyPtr)
{
    RenderProxy* proxy = reinterpret_cast<RenderProxy*>(proxyPtr);
    proxy->stopDrawing();
}

void ThreadedRenderer::NativeNotifyFramePending(
    /* [in] */ Int64 proxyPtr)
{
    RenderProxy* proxy = reinterpret_cast<RenderProxy*>(proxyPtr);
    proxy->notifyFramePending();
}

void ThreadedRenderer::NativeDumpProfileInfo(
    /* [in] */ Int64 proxyPtr,
    /* [in] */ IFileDescriptor* pFd)
{
    RenderProxy* proxy = reinterpret_cast<RenderProxy*>(proxyPtr);
    Int32 fd;
    pFd->GetDescriptor(&fd);
    proxy->dumpProfileInfo(fd, 0);
}

void ThreadedRenderer::NativeRotateProcessStatsBuffer()
{}

void ThreadedRenderer::NativeSetProcessStatsBuffer(
    /* [in] */ Int32 fd)
{
    RenderProxy::setProcessStatsBuffer(fd);
}

Int32 ThreadedRenderer::NativeGetRenderThreadTid(
    /* [in] */ Int64 nativeProxy)
{
    RenderProxy* proxy = reinterpret_cast<RenderProxy*>(nativeProxy);
    return proxy->getRenderThreadTid();
}

} // namespace View
} // namespace Droid
} // namespace Elastos
