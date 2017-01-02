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
#include "elastos/droid/view/ThreadedRenderer.h"
#include "elastos/droid/view/HardwareRenderer.h"
#include "elastos/droid/view/CRenderNodeHelper.h"
#include "elastos/droid/view/RenderNode.h"
#include "elastos/droid/view/HardwareLayer.h"
#include "elastos/droid/view/CChoreographerHelper.h"
#include "elastos/droid/view/Surface.h"
#include "elastos/droid/content/res/CResources.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/utility/TimeUtils.h"
#include "elastos/droid/R.h"

#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <EGL/egl_cache.h>

#include <utils/StrongPointer.h>
#include <system/window.h>

#include <Animator.h>
#include <AnimationContext.h>
#include <IContextFactory.h>
#include <hwui/RenderNode.h>
#include <renderthread/CanvasContext.h>
#include <renderthread/RenderProxy.h>
#include <renderthread/RenderTask.h>
#include <renderthread/RenderThread.h>
#include <gui/Surface.h>
#include <elastos/utility/etl/Vector.h>

using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::CResources;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::CBitmap;
using Elastos::Droid::Graphics::BitmapConfig;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
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

    AtlasInitializer::sInstance->Init(context, mNativeProxy);

    // Setup timing
    AutoPtr<IChoreographerHelper> cghlp;
    CChoreographerHelper::AcquireSingleton((IChoreographerHelper**)&cghlp);
    cghlp->GetInstance((IChoreographer**)&mChoreographer);
    Int64 n = 0;
    mChoreographer->GetFrameIntervalNanos(&n);
    NativeSetFrameInterval(mNativeProxy, n);

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
    Int64 frameTimeNanos = 0;
    mChoreographer->GetFrameTimeNanos(&frameTimeNanos);
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

    AutoPtr<IResources> res;
    view->GetResources((IResources**)&res);
    AutoPtr<IDisplayMetrics> metrics;
    res->GetDisplayMetrics((IDisplayMetrics**)&metrics);
    Float density;
    metrics->GetDensity(&density);
    Int32 syncResult = NativeSyncAndDrawFrame(mNativeProxy, frameTimeNanos,
            recordDuration, density);
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
//              ThreadedRenderer::AtlasInitializer::
//========================================================================================

AutoPtr<ThreadedRenderer::AtlasInitializer> ThreadedRenderer::AtlasInitializer::sInstance = new AtlasInitializer();

ThreadedRenderer::AtlasInitializer::AtlasInitializer()
{
    mInitialized = FALSE;
}

void ThreadedRenderer::AtlasInitializer::Init(
    /* [in] */ IContext* context,
    /* [in] */ Int64 renderProxy)
{
    if (mInitialized) return;
    AutoPtr<IInterface> p = ServiceManager::GetService(String("assetatlas"));
    AutoPtr<Elastos::Droid::Os::IBinder> binder = Elastos::Droid::Os::IBinder::Probe(p);
    if (binder == NULL) {
        Logger::E(LOGTAG, "Failed to get service assetatlas.");
        return;
    }

    AutoPtr<IIAssetAtlas> atlas = IIAssetAtlas::Probe(binder);
//     try {
    Int32 ppid = Process::MyPpid();
    Boolean compatible;
    if (atlas->IsCompatible(ppid, &compatible), compatible) {
        AutoPtr<IGraphicBuffer> buffer;
        atlas->GetBuffer((IGraphicBuffer**)&buffer);
        if (buffer != NULL) {
            AutoPtr<ArrayOf<Int64> > map;
            atlas->GetMap((ArrayOf<Int64>**)&map);
            if (map != NULL) {
                // TODO Remove after fixing b/15425820
                ValidateMap(context, map);
                NativeSetAtlas(renderProxy, buffer, map);
                mInitialized = TRUE;
            }

            // If IAssetAtlas is not the same class as the IBinder
            // we are using a remote service and we can safely
            // destroy the graphic buffer
            AutoPtr< ::IProxy > proxy = (::IProxy*)binder->Probe(::EIID_IProxy);
            if (proxy != NULL) {
                buffer->Destroy();
            }
        }
    }
    // } catch (RemoteException e) {
    //     Log.w(LOG_TAG, "Could not acquire atlas", e);
    // }
}

void ThreadedRenderer::AtlasInitializer::ValidateMap(
    /* [in] */ IContext* context,
    /* [in] */ ArrayOf<Int64>* map)
{
    AutoPtr<IHashSet> preloadedPointers;
    CHashSet::New((IHashSet**)&preloadedPointers);

    // We only care about drawables that hold bitmaps
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr<IInt64SparseArray> drawables;
    resources->GetPreloadedDrawables((IInt64SparseArray**)&drawables);

    Int32 count = 0;
    drawables->GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> v;
        drawables->ValueAt(i, (IInterface**)&v);
        IDrawableConstantState* rv = IDrawableConstantState::Probe(v);
        if (rv != NULL) {
            AutoPtr<IBitmap> bitmap;
            rv->GetBitmap((IBitmap**)&bitmap);
            BitmapConfig cf;
            if (bitmap != NULL && (bitmap->GetConfig(&cf), cf) == BitmapConfig_ARGB_8888) {
                Handle64 nativeBitmap;
                bitmap->GetNativeBitmap(&nativeBitmap);
                AutoPtr<IInteger64> pInt;
                CInteger64::New(nativeBitmap, (IInteger64**)&pInt);
                preloadedPointers->Add(pInt);
            }
        }
    }

    for (Int32 i = 0; i < map->GetLength(); i += 4) {
        AutoPtr<IInteger64> m;
        CInteger64::New((*map)[i], (IInteger64**)&m);
        Boolean bCtains = FALSE;
        if (!(preloadedPointers->Contains(m, &bCtains), bCtains)) {
            Logger::W("ThreadedRenderer", "Pointer at %d 0x%s, not in getPreloadedDrawables?",
                i/4, StringUtils::ToHexString((*map)[i]).string());
            (*map)[i] = 0;
        }
    }
}

//------------Native Methord-----------


class OnFinishedEvent {
public:
    OnFinishedEvent(BaseRenderNodeAnimator* animator, AnimationListener* listener)
            : animator(animator), listener(listener) {}
    sp<BaseRenderNodeAnimator> animator;
    sp<AnimationListener> listener;
};

class InvokeAnimationListeners : public MessageHandler {
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

class RenderingException : public MessageHandler {
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

class RootRenderNode : public android::uirenderer::RenderNode, ErrorHandler {
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
        RenderNode::prepareTree(info);
        info.errorHandler = NULL;
    }

    void sendMessage(const sp<MessageHandler>& handler) {
        mLooper->sendMessage(handler, 0);
    }

    void attachAnimatingNode(RenderNode* animatingNode) {
        mPendingAnimatingRenderNodes.push_back(animatingNode);
    }

    void doAttachAnimatingNodes(AnimationContext* context) {
        for (size_t i = 0; i < mPendingAnimatingRenderNodes.size(); i++) {
            RenderNode* node = mPendingAnimatingRenderNodes[i].get();
            context->addAnimatingRenderNode(*node);
        }
        mPendingAnimatingRenderNodes.clear();
    }

private:
    sp<Looper> mLooper;
    std::vector< sp<RenderNode> > mPendingAnimatingRenderNodes;
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
        }
        AnimationContext::startFrame(mode);
    }

    // Runs any animations still left in mCurrentFrameAnimations
    virtual void runRemainingAnimations(TreeInfo& info) {
        AnimationContext::runRemainingAnimations(info);
        postOnFinishedEvents();
    }

    virtual void callOnFinished(BaseRenderNodeAnimator* animator, AnimationListener* listener) {
        OnFinishedEvent event(animator, listener);
        mOnFinishedEvents.push_back(event);
    }

    virtual void destroy() {
        AnimationContext::destroy();
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

class ContextFactoryImpl : public IContextFactory {
public:
    ContextFactoryImpl(RootRenderNode* rootNode) : mRootNode(rootNode) {}

    virtual AnimationContext* createAnimationContext(renderthread::TimeLord& clock) {
        return new AnimationContextBridge(clock, mRootNode);
    }

private:
    RootRenderNode* mRootNode;
};

static android::sp<android::GraphicBuffer> graphicBufferForJavaObject(IGraphicBuffer* obj)
{
    if (obj) {
        Int64 nativeObject;
        obj->GetNativeObject(&nativeObject);
        if (nativeObject != 0) {
            android::sp<android::GraphicBuffer> gb = reinterpret_cast<android::GraphicBuffer*>(nativeObject);
            return gb;
        }
    }
    return NULL;
}

ECode ThreadedRenderer::NativeSetupShadersDiskCache(
    /* [in] */ const String& cacheFile)
{
    egl_cache_t::get()->setCacheFilename(cacheFile.string());
    return NOERROR;
}

void ThreadedRenderer::NativeSetAtlas(
    /* [in] */ Int64 proxyPtr,
    /* [in] */ IGraphicBuffer* graphicBuffer,
    /* [in] */ ArrayOf<Int64>* atlasMapArray)
{
    Int32 len = atlasMapArray->GetLength();
    if (len <= 0) {
        Logger::W(LOGTAG, "Failed to initialize atlas, invalid map length: %d", len);
        return;
    }
    int64_t* map = new int64_t[len];
    for (Int32 i = 0; i < len; i++) {
        map[i] = (*atlasMapArray)[i];
    }

    sp<android::GraphicBuffer> buffer = graphicBufferForJavaObject(graphicBuffer);
    RenderProxy* proxy = reinterpret_cast<RenderProxy*>(proxyPtr);
    proxy->setTextureAtlas(buffer, map, len);
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
    if (nativeProxy) {
        RenderProxy* proxy = reinterpret_cast<RenderProxy*>(nativeProxy);
        delete proxy;
    }
}

void ThreadedRenderer::NativeSetFrameInterval(
    /* [in] */ Int64 nativeProxy,
    /* [in] */ Int64 frameIntervalNanos)
{
    RenderProxy* proxy = reinterpret_cast<RenderProxy*>(nativeProxy);
    proxy->setFrameInterval(frameIntervalNanos);
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
    sp<ANativeWindow> nWindow = (ANativeWindow*)reinterpret_cast<android::Surface*>(nativeSurf);
    return proxy->initialize(nWindow);
}

void ThreadedRenderer::NativeUpdateSurface(
    /* [in] */ Int64 nativeProxy,
    /* [in] */ ISurface* window)
{
    RenderProxy* proxy = reinterpret_cast<RenderProxy*>(nativeProxy);
    sp<ANativeWindow> nWindow;
    if (window) {
        Int64 nativeSurf;
        window->GetNativeSurface(&nativeSurf);
        nWindow = (ANativeWindow*)reinterpret_cast<android::Surface*>(nativeSurf);
    }
    proxy->updateSurface(nWindow);
}

void ThreadedRenderer::NativePauseSurface(
    /* [in] */ Int64 nativeProxy,
    /* [in] */ ISurface* window)
{
    RenderProxy* proxy = reinterpret_cast<RenderProxy*>(nativeProxy);
    sp<ANativeWindow> nWindow;
    if (window) {
        Int64 nativeSurf;
        window->GetNativeSurface(&nativeSurf);
        nWindow = (ANativeWindow*)reinterpret_cast<android::Surface*>(nativeSurf);
    }
    proxy->pauseSurface(nWindow);
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
    proxy->setup(width, height, (Vector3){lightX, lightY, lightZ}, lightRadius,
            ambientShadowAlpha, spotShadowAlpha);
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
    /* [in] */ Int64 frameTimeNanos,
    /* [in] */ Int64 recordDuration,
    /* [in] */ Float density)
{
    RenderProxy* proxy = reinterpret_cast<RenderProxy*>(nativeProxy);
    return proxy->syncAndDrawFrame(frameTimeNanos, recordDuration, density);
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
    SkBitmap* bitmap = reinterpret_cast<SkBitmap*>(bitmapPtr);
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
    proxy->dumpProfileInfo(fd);
}

} // namespace View
} // namespace Droid
} // namespace Elastos
