
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/ThreadedRenderer.h"
#include "elastos/droid/content/res/CResources.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/os/ServiceManager.h"

using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::CResources;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::CBitmap;
using Elastos::Droid::Graphics::BitmapConfig;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::Graphics::Drawable::IDrawableConstantState;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Utility::IInt64SparseArray;

using Elastos::Core::IInteger64;
using Elastos::Core::CInteger64;
using Elastos::IO::IFlushable;
using Elastos::Utility::IHashSet;
using Elastos::Utility::CHashSet;

namespace Elastos {
namespace Droid {
namespace View {

//========================================================================================
//              ThreadedRenderer::
//========================================================================================

const String ThreadedRenderer::LOGTAG("ThreadedRenderer");

const Int32 ThreadedRenderer::SYNC_OK = 0;

const Int32 ThreadedRenderer::SYNC_INVALIDATE_REQUIRED = 1 << 0;

const AutoPtr<ArrayOf<String> > ThreadedRenderer::VISUALIZERS = ArrayOf<String>::Alloc(1);

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
{
//    (*VISUALIZERS)[0] = PROFILE_PROPERTY_VISUALIZE_BARS;
}

ECode ThreadedRenderer::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Boolean translucent)
{
    assert(0 && "TODO");
    AutoPtr<ITypedArray> a;
    // context->ObtainStyledAttributes(NULL, R.styleable.Lighting, 0, 0, (ITypedArray**)&a);
    // a->GetDimension(R.styleable.Lighting_lightY, 0, &mLightY);
    // a->GetDimension(R.styleable.Lighting_lightZ, 0, &mLightZ);
    // a->GetDimension(R.styleable.Lighting_lightRadius, 0, &mLightRadius);
    Float f1 = 0.f, f2 = 0.f;
    // a->GetFloat(R.styleable.Lighting_ambientShadowAlpha, 0, &f1);
    mAmbientShadowAlpha = (Int32) (255 * f1 + 0.5f);
    // a->GetFloat(R.styleable.Lighting_spotShadowAlpha, 0, &f2);
    mSpotShadowAlpha = (Int32) (255 * f2 + 0.5f);
    a->Recycle();

    Int64 rootNodePtr = NativeCreateRootRenderNode();
    AutoPtr<IRenderNodeHelper> hlp;
    // CRenderNodeHelper::AcquireSingleton((IRenderNodeHelper**)&hlp);
    hlp->Adopt(rootNodePtr, (IRenderNode**)&mRootNode);
    Boolean r;
    mRootNode->SetClipToBounds(FALSE, &r);
    mNativeProxy = NativeCreateProxy(translucent, rootNodePtr);

//    AtlasInitializer::sInstance->Init(context, mNativeProxy);

    // Setup timing
    AutoPtr<IChoreographerHelper> cghlp;
//    CChoreographerHelper::AcquireSingleton((IChoreographerHelper**)&cghlp);
    cghlp->GetInstance((IChoreographer**)&mChoreographer);
    Int64 n = 0;
    mChoreographer->GetFrameIntervalNanos(&n);
    NativeSetFrameInterval(mNativeProxy, n);

    LoadSystemProperties();
    return NOERROR;
}

void ThreadedRenderer::Destroy()
{
    mInitialized = FALSE;
    UpdateEnabledState(NULL);
    NativeDestroy(mNativeProxy);
}

void ThreadedRenderer::UpdateEnabledState(
    /* [in] */ ISurface* surface)
{
    Boolean vld = FALSE;
    if (surface == NULL || !(surface->IsValid(&vld), vld)) {
//        SetEnabled(FALSE);
    }
    else {
//        SetEnabled(mInitialized);
    }
}

// throws OutOfResourcesException
Boolean ThreadedRenderer::Initialize(
    /* [in] */ ISurface* surface)
{
    mInitialized = TRUE;
    UpdateEnabledState(surface);
    Boolean status = NativeInitialize(mNativeProxy, surface);
    surface->AllocateBuffers();
    return status;
}

// throws OutOfResourcesException
ECode ThreadedRenderer::UpdateSurface(
    /* [in] */ ISurface* surface)
{
    UpdateEnabledState(surface);
    NativeUpdateSurface(mNativeProxy, surface);
    return NOERROR;
}

void ThreadedRenderer::PauseSurface(
    /* [in] */ ISurface* surface)
{
    NativePauseSurface(mNativeProxy, surface);
}

void ThreadedRenderer::DestroyHardwareResources(
        /* [in] */ IView* view)
{
    DestroyResources(view);
    NativeDestroyHardwareResources(mNativeProxy);
}

void ThreadedRenderer::DestroyResources(
    /* [in] */ IView* view)
{
//    ((View*)view)->DestroyHardwareResources();

    if (IViewGroup::Probe(view) != NULL) {
        AutoPtr<IViewGroup> group = IViewGroup::Probe(view);

        Int32 count = 0;
        group->GetChildCount(&count);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IView> v;
            group->GetChildAt(i, (IView**)&v);
            DestroyResources(v);
        }
    }
}

void ThreadedRenderer::Invalidate(
    /* [in] */ ISurface* surface)
{
    UpdateSurface(surface);
}

void ThreadedRenderer::DetachSurfaceTexture(
    /* [in] */ Int64 hardwareLayer)
{
    NativeDetachSurfaceTexture(mNativeProxy, hardwareLayer);
}

void ThreadedRenderer::Setup(
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
}

void ThreadedRenderer::SetOpaque(
    /* [in] */ Boolean opaque)
{
    NativeSetOpaque(mNativeProxy, opaque && !mHasInsets);
}

Int32 ThreadedRenderer::GetWidth()
{
    return mWidth;
}

Int32 ThreadedRenderer::GetHeight()
{
    return mHeight;
}

void ThreadedRenderer::DumpGfxInfo(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ IFileDescriptor* fd)
{
    IFlushable::Probe(pw)->Flush();
    NativeDumpProfileInfo(mNativeProxy, fd);
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
//    SystemProperties::Get(HardwareRenderer::PROFILE_PROPERTY, &profiling);
    Int32 graphType = Search(VISUALIZERS, profiling);
    assert(0 && "TODO");
//    return (graphType >= 0) || Boolean::ParseBoolean(profiling);
    return FALSE;
}

Boolean ThreadedRenderer::LoadSystemProperties()
{
    Boolean changed = NativeLoadSystemProperties(mNativeProxy);
    Boolean wantProfiling = CheckIfProfilingRequested();
    if (wantProfiling != mProfilingEnabled) {
        mProfilingEnabled = wantProfiling;
        changed = TRUE;
    }
    return changed;
}

void ThreadedRenderer::UpdateViewTreeDisplayList(
    /* [in] */ IView* view)
{
    // AutoPtr<View> cv = (View*)view;
    // cv->mPrivateFlags |= View::PFLAG_DRAWN;
    // cv->mRecreateDisplayList = (cv->mPrivateFlags & View::PFLAG_INVALIDATED)
    //         == View::PFLAG_INVALIDATED;
    // cv->mPrivateFlags &= ~View::PFLAG_INVALIDATED;
    // view->GetDisplayList();
    // cv->mRecreateDisplayList = FALSE;
}

// void ThreadedRenderer::UpdateRootDisplayList(
//     /* [in] */ IView* view,
//     /* [in] */ IHardwareDrawCallbacks* callbacks)
// {
//     assert(0 && "TODO");
// //    Trace::TraceBegin(Trace::TRACE_TAG_VIEW, "getDisplayList");
//     UpdateViewTreeDisplayList(view);

//     Boolean bVld = FALSE;
//     if (mRootNodeNeedsUpdate || !(mRootNode->IsValid(&bVld), bVld)) {
//         AutoPtr<IHardwareCanvas> canvas;
//         mRootNode->Start(mSurfaceWidth, mSurfaceHeight, (IHardwareCanvas**)&canvas);
//         Int32 saveCount = 0;
//         canvas->Save(&saveCount);
//         canvas->Translate(mInsetLeft, mInsetTop);
//         callbacks->OnHardwarePreDraw(canvas);

//         canvas->InsertReorderBarrier();
//         AutoPtr<IRenderNode> dl;
//         view->GetDisplayList((IRenderNode**)&dl);
//         canvas->DrawRenderNode(dl);
//         canvas->InsertInorderBarrier();

//         callbacks->OnHardwarePostDraw(canvas);
//         canvas->RestoreToCount(saveCount);
//         mRootNodeNeedsUpdate = FALSE;

//         mRootNode->End(canvas);
//     }
// //    Trace::TraceEnd(Trace::TRACE_TAG_VIEW);
// }

void ThreadedRenderer::InvalidateRoot()
{
    mRootNodeNeedsUpdate = TRUE;
}

// ECode ThreadedRenderer::Draw(
//     /* [in] */ IView* view,
//     /* [in] */ IAttachInfo* attachInfo,
//     /* [in] */ IHardwareDrawCallbacks* callbacks)
// {
//     attachInfo->mIgnoreDirtyState = TRUE;
//     Int64 frameTimeNanos = 0;
//     mChoreographer->GetFrameTimeNanos(&frameTimeNanos);
//     attachInfo->mDrawingTime = frameTimeNanos / ITimeUtils::NANOS_PER_MS;

//     Int64 recordDuration = 0;
//     if (mProfilingEnabled) {
//         recordDuration = System::NanoTime();
//     }

//     UpdateRootDisplayList(view, callbacks);

//     if (mProfilingEnabled) {
//         recordDuration = System::NanoTime() - recordDuration;
//     }

//     attachInfo->mIgnoreDirtyState = FALSE;

//     // register animating rendernodes which started animating prior to renderer
//     // creation, which is typical for animators started prior to first draw
//     if (attachInfo->mPendingAnimatingRenderNodes != NULL) {
//         Int32 count = attachInfo->mPendingAnimatingRenderNodes->GetSize();
//         for (Int32 i = 0; i < count; i++) {
//             RegisterAnimatingRenderNode(
//                     attachInfo->mPendingAnimatingRenderNodes->Get(i));
//         }
//         attachInfo->mPendingAnimatingRenderNodes->Clear();
//         // We don't need this anymore as subsequent calls to
//         // ViewRootImpl#attachRenderNodeAnimator will go directly to us.
//         attachInfo->mPendingAnimatingRenderNodes = NULL;
//     }

//     Int32 syncResult = NativeSyncAndDrawFrame(mNativeProxy, frameTimeNanos,
//             recordDuration, view->GetResources()->GetDisplayMetrics()->mDensity);
//     if ((syncResult & SYNC_INVALIDATE_REQUIRED) != 0) {
//         attachInfo->mViewRootImpl->Invalidate();
//     }
//     return NOERROR;
// }

void ThreadedRenderer::InvokeFunctor(
    /* [in] */ Int64 functor,
    /* [in] */ Boolean waitForCompletion)
{
    NativeInvokeFunctor(functor, waitForCompletion);
}

AutoPtr<IHardwareLayer> ThreadedRenderer::CreateTextureLayer()
{
    Int64 layer = NativeCreateTextureLayer(mNativeProxy);
//    return HardwareLayer::AdoptTextureLayer(this, layer);
    return NULL;
}

void ThreadedRenderer::BuildLayer(
    /* [in] */ IRenderNode* node)
{
    assert(0 && "TODO");
    Int64 dl = 0; // node->GetNativeDisplayList();
    NativeBuildLayer(mNativeProxy, dl);
}

Boolean ThreadedRenderer::CopyLayerInto(
    /* [in] */ IHardwareLayer* layer,
    /* [in] */ IBitmap* bitmap)
{
    Int64 dlu = 0;
    layer->GetDeferredLayerUpdater(&dlu);
    AutoPtr<CBitmap> cbmp = (CBitmap*)bitmap;
    return NativeCopyLayerInto(mNativeProxy,
            dlu, cbmp->mNativeBitmap);
}

void ThreadedRenderer::PushLayerUpdate(
    /* [in] */ IHardwareLayer* layer)
{
    Int64 dlu = 0;
    layer->GetDeferredLayerUpdater(&dlu);
    NativePushLayerUpdate(mNativeProxy, dlu);
}

void ThreadedRenderer::OnLayerDestroyed(
    /* [in] */ IHardwareLayer* layer)
{
    Int64 dlu = 0;
    layer->GetDeferredLayerUpdater(&dlu);
    NativeCancelLayerUpdate(mNativeProxy, dlu);
}

void ThreadedRenderer::SetName(
    /* [in] */ const String& name)
{
}

void ThreadedRenderer::Fence()
{
    NativeFence(mNativeProxy);
}

void ThreadedRenderer::StopDrawing()
{
    NativeStopDrawing(mNativeProxy);
}

void ThreadedRenderer::NotifyFramePending()
{
    NativeNotifyFramePending(mNativeProxy);
}

void ThreadedRenderer::RegisterAnimatingRenderNode(
    /* [in] */ IRenderNode* animator)
{
//    NativeRegisterAnimatingRenderNode(mRootNode->mNativeRenderNode, animator->mNativeRenderNode);
}

// throws Throwable
ECode ThreadedRenderer::Finalize()
{
    NativeDeleteProxy(mNativeProxy);
    mNativeProxy = 0;
//    HardwareRenderer::Finalize();
    return NOERROR;
}

void ThreadedRenderer::TrimMemory(
    /* [in] */ Int32 level)
{
    NativeTrimMemory(level);
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
    AutoPtr<IBinder> binder = IBinder::Probe(p);
    if (binder == NULL) return;

//     AutoPtr<IAssetAtlas> atlas = IAssetAtlas.Stub.asInterface(binder);
// //     try {
//         if (atlas->IsCompatible(android.os.Process.myPpid())) {
//             AutoPtr<IGraphicBuffer> buffer = atlas->GetBuffer();
//             if (buffer != NULL) {
//                 AutoPtr<ArrayOf<Int64> > map = atlas->GetMap();
//                 if (map != NULL) {
//                     // TODO Remove after fixing b/15425820
//                     ValidateMap(context, map);
//                     NativeSetAtlas(renderProxy, buffer, map);
//                     mInitialized = TRUE;
//                 }
//                 // If IAssetAtlas is not the same class as the IBinder
//                 // we are using a remote service and we can safely
//                 // destroy the graphic buffer
//                 if (atlas->GetClass() != binder->GetClass()) {
//                     buffer->Destroy();
//                 }
//             }
//         }
    // } catch (RemoteException e) {
    //     Log.w(LOG_TAG, "Could not acquire atlas", e);
    // }
}

void ThreadedRenderer::AtlasInitializer::ValidateMap(
    /* [in] */ IContext* context,
    /* [in] */ ArrayOf<Int64>* map)
{
//    Log.d("Atlas", "Validating map...");
    AutoPtr<IHashSet> preloadedPointers;
    CHashSet::New((IHashSet**)&preloadedPointers);

    // We only care about drawables that hold bitmaps
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr<IInt64SparseArray> drawables;
    AutoPtr<CResources> cr = (CResources*)resources.Get();
//    cr->GetPreloadedDrawables((IInt64SparseArray**)&drawables);

    Int32 count = 0;
    drawables->GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> v;
        drawables->ValueAt(i, (IInterface**)&v);
        AutoPtr<IDrawableConstantState> rv = IDrawableConstantState::Probe(v);
        AutoPtr<IBitmap> bitmap;
        rv->GetBitmap((IBitmap**)&bitmap);
        BitmapConfig cf;
        if (bitmap != NULL && (bitmap->GetConfig(&cf), cf) == BitmapConfig_ARGB_8888) {
            AutoPtr<CBitmap> cbmp = (CBitmap*)bitmap.Get();
            AutoPtr<IInteger64> pInt;
            CInteger64::New(cbmp->mNativeBitmap, (IInteger64**)&pInt);
            preloadedPointers->Add(pInt);
        }
    }

    for (Int32 i = 0; i < map->GetLength(); i += 4) {
        AutoPtr<IInteger64> m;
        CInteger64::New((*map)[i], (IInteger64**)&m);
        Boolean bCtains = FALSE;
        if (!(preloadedPointers->Contains(m, &bCtains), bCtains)) {
        //     Log.w("Atlas", String.format("Pointer 0x%X, not in getPreloadedDrawables?", map[i]));
            (*map)[i] = 0;
        }
    }
}

//------------Native Methord-----------

void ThreadedRenderer::NativeSetupShadersDiskCache(
    /* [in] */ const String& cacheFile)
{
    // Zhangyu JNI TODO
}

void ThreadedRenderer::NativeSetAtlas(
    /* [in] */ Int64 nativeProxy,
    /* [in] */ IGraphicBuffer* buffer,
    /* [in] */ ArrayOf<Int64>* map)
{
    // Zhangyu JNI TODO
}

Int64 ThreadedRenderer::NativeCreateRootRenderNode()
{
    // Zhangyu JNI TODO
    return 0;
}

Int64 ThreadedRenderer::NativeCreateProxy(
    /* [in] */ Boolean translucent,
    /* [in] */ Int64 rootRenderNode)
{
    // Zhangyu JNI TODO
    return 0;
}

void ThreadedRenderer::NativeDeleteProxy(
    /* [in] */ Int64 nativeProxy)
{
    // Zhangyu JNI TODO
}

void ThreadedRenderer::NativeSetFrameInterval(
    /* [in] */ Int64 nativeProxy,
    /* [in] */ Int64 frameIntervalNanos)
{
    // Zhangyu JNI TODO
}

Boolean ThreadedRenderer::NativeLoadSystemProperties(
    /* [in] */ Int64 nativeProxy)
{
    // Zhangyu JNI TODO
    return FALSE;
}

Boolean ThreadedRenderer::NativeInitialize(
    /* [in] */ Int64 nativeProxy,
    /* [in] */ ISurface* window)
{
    // Zhangyu JNI TODO
    return FALSE;
}

void ThreadedRenderer::NativeUpdateSurface(
    /* [in] */ Int64 nativeProxy,
    /* [in] */ ISurface* window)
{
    // Zhangyu JNI TODO
}

void ThreadedRenderer::NativePauseSurface(
    /* [in] */ Int64 nativeProxy,
    /* [in] */ ISurface* window)
{
    // Zhangyu JNI TODO
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
    // Zhangyu JNI TODO
}

void ThreadedRenderer::NativeSetOpaque(
    /* [in] */ Int64 nativeProxy,
    /* [in] */ Boolean opaque)
{
    // Zhangyu JNI TODO
}

Int32 ThreadedRenderer::NativeSyncAndDrawFrame(
    /* [in] */ Int64 nativeProxy,
    /* [in] */ Int64 frameTimeNanos,
    /* [in] */ Int64 recordDuration,
    /* [in] */ Float density)
{
    // Zhangyu JNI TODO
    return 0;
}

void ThreadedRenderer::NativeDestroy(
    /* [in] */ Int64 nativeProxy)
{
    // Zhangyu JNI TODO
}

void ThreadedRenderer::NativeRegisterAnimatingRenderNode(
    /* [in] */ Int64 rootRenderNode,
    /* [in] */ Int64 animatingNode)
{
    // Zhangyu JNI TODO
}

void ThreadedRenderer::NativeInvokeFunctor(
    /* [in] */ Int64 functor,
    /* [in] */ Boolean waitForCompletion)
{
    // Zhangyu JNI TODO
}

Int64 ThreadedRenderer::NativeCreateTextureLayer(
    /* [in] */ Int64 nativeProxy)
{
    // Zhangyu JNI TODO
    return 0;
}

void ThreadedRenderer::NativeBuildLayer(
    /* [in] */ Int64 nativeProxy,
    /* [in] */ Int64 node)
{
    // Zhangyu JNI TODO
}

Boolean ThreadedRenderer::NativeCopyLayerInto(
    /* [in] */ Int64 nativeProxy,
    /* [in] */ Int64 layer,
    /* [in] */ Int64 bitmap)
{
    // Zhangyu JNI TODO
    return FALSE;
}

void ThreadedRenderer::NativePushLayerUpdate(
    /* [in] */ Int64 nativeProxy,
    /* [in] */ Int64 layer)
{
    // Zhangyu JNI TODO
}

void ThreadedRenderer::NativeCancelLayerUpdate(
    /* [in] */ Int64 nativeProxy,
    /* [in] */ Int64 layer)
{
    // Zhangyu JNI TODO
}

void ThreadedRenderer::NativeDetachSurfaceTexture(
    /* [in] */ Int64 nativeProxy,
    /* [in] */ Int64 layer)
{
    // Zhangyu JNI TODO
}

void ThreadedRenderer::NativeDestroyHardwareResources(
    /* [in] */ Int64 nativeProxy)
{
    // Zhangyu JNI TODO
}

void ThreadedRenderer::NativeTrimMemory(
    /* [in] */ Int32 level)
{
    // Zhangyu JNI TODO
}

void ThreadedRenderer::NativeFence(
    /* [in] */ Int64 nativeProxy)
{
    // Zhangyu JNI TODO
}

void ThreadedRenderer::NativeStopDrawing(
    /* [in] */ Int64 nativeProxy)
{
    // Zhangyu JNI TODO
}

void ThreadedRenderer::NativeNotifyFramePending(
    /* [in] */ Int64 nativeProxy)
{
    // Zhangyu JNI TODO
}

void ThreadedRenderer::NativeDumpProfileInfo(
    /* [in] */ Int64 nativeProxy,
    /* [in] */ IFileDescriptor* fd)
{
    // Zhangyu JNI TODO
}

} // namespace View
} // namespace Droid
} // namespace Elastos
