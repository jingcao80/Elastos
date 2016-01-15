
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/CSurfaceControlHelper.h"
#include "elastos/droid/view/SurfaceControl.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace View {

CAR_SINGLETON_IMPL(CSurfaceControlHelper)

CAR_INTERFACE_IMPL(CSurfaceControlHelper, Singleton, ISurfaceControlHelper)

ECode CSurfaceControlHelper::OpenTransaction()
{
    SurfaceControl::OpenTransaction();
    return NOERROR;
}

ECode CSurfaceControlHelper::CloseTransaction()
{
    SurfaceControl::CloseTransaction();
    return NOERROR;
}

ECode CSurfaceControlHelper::SetAnimationTransaction()
{
    SurfaceControl::SetAnimationTransaction();
    return NOERROR;
}

ECode CSurfaceControlHelper::ClearAnimationFrameStats(
    /* [out] */ Boolean* stats)
{
    return SurfaceControl::ClearAnimationFrameStats(stats);
}

ECode CSurfaceControlHelper::GetAnimationFrameStats(
    /* [in] */ IWindowAnimationFrameStats* outStats,
    /* [out] */ Boolean* stats)
{
    return SurfaceControl::GetAnimationFrameStats(outStats, stats);
}

ECode CSurfaceControlHelper::SetDisplayPowerMode(
    /* [in] */ IBinder* displayToken,
    /* [in] */ Int32 mode)
{
    SurfaceControl::SetDisplayPowerMode(displayToken, mode);
    return NOERROR;
}

ECode CSurfaceControlHelper::GetDisplayConfigs(
    /* [in] */ IBinder* displayToken,
    /* [out, callee] */ ArrayOf<IPhysicalDisplayInfo*>** infos)
{
    return SurfaceControl::GetDisplayConfigs(displayToken, infos);
}

ECode CSurfaceControlHelper::GetActiveConfig(
    /* [in] */ IBinder* displayToken,
    /* [out] */ Int32* config)
{
    return SurfaceControl::GetActiveConfig(displayToken, config);
}

ECode CSurfaceControlHelper::SetActiveConfig(
    /* [in] */ IBinder* displayToken,
    /* [in] */ Int32 id,
    /* [out] */ Boolean* result)
{
    return SurfaceControl::SetActiveConfig(displayToken, id, result);
}

ECode CSurfaceControlHelper::SetDisplayProjection(
    /* [in] */ IBinder* displayToken,
    /* [in] */ Int32 orientation,
    /* [in] */ IRect* layerStackRect,
    /* [in] */ IRect* displayRect)
{
    return SurfaceControl::SetDisplayProjection(displayToken, orientation, layerStackRect, displayRect);
}

ECode CSurfaceControlHelper::SetDisplayLayerStack(
    /* [in] */ IBinder* displayToken,
    /* [in] */ Int32 layerStack)
{
    return SurfaceControl::SetDisplayLayerStack(displayToken, layerStack);
}

ECode CSurfaceControlHelper::SetDisplaySurface(
    /* [in] */ IBinder* displayToken,
    /* [in] */ ISurface* surface)
{
    return SurfaceControl::SetDisplaySurface(displayToken, surface);
}

ECode CSurfaceControlHelper::SetDisplaySize(
    /* [in] */ IBinder* displayToken,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    return SurfaceControl::SetDisplaySize(displayToken, width, height);
}

ECode CSurfaceControlHelper::CreateDisplay(
    /* [in] */ const String& name,
    /* [in] */ Boolean secure,
    /* [out] */ IBinder** display)
{
    return SurfaceControl::CreateDisplay(name, secure, display);
}

ECode CSurfaceControlHelper::DestroyDisplay(
    /* [in] */ IBinder* displayToken)
{
    SurfaceControl::DestroyDisplay(displayToken);
    return NOERROR;
}

ECode CSurfaceControlHelper::GetBuiltInDisplay(
    /* [in] */ Int32 builtInDisplayId,
    /* [out] */ IBinder** display)
{
    AutoPtr<IBinder> tmp = SurfaceControl::GetBuiltInDisplay(builtInDisplayId);
    *display = tmp;
    REFCOUNT_ADD(*display)
    return NOERROR;
}

ECode CSurfaceControlHelper::Screenshot(
    /* [in] */ IBinder* display,
    /* [in] */ ISurface* consumer,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 minLayer,
    /* [in] */ Int32 maxLayer,
    /* [in] */ Boolean useIdentityTransform)
{
    SurfaceControl::Screenshot(display, consumer, width, height,
        minLayer, maxLayer, useIdentityTransform);
    return NOERROR;
}

ECode CSurfaceControlHelper::Screenshot(
    /* [in] */ IBinder* display,
    /* [in] */ ISurface* consumer,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    SurfaceControl::Screenshot(display, consumer, width, height);
    return NOERROR;
}

ECode CSurfaceControlHelper::Screenshot(
    /* [in] */ IBinder* display,
    /* [in] */ ISurface* consumer)
{
    SurfaceControl::Screenshot(display, consumer);
    return NOERROR;
}

ECode CSurfaceControlHelper::Screenshot(
    /* [in] */ IRect* sourceCrop,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 minLayer,
    /* [in] */ Int32 maxLayer,
    /* [in] */ Boolean useIdentityTransform,
    /* [in] */ Int32 rotation,
    /* [out] */ IBitmap** bm)
{
    AutoPtr<IBitmap> tmp = SurfaceControl::Screenshot(sourceCrop, width, height,
            minLayer, maxLayer, useIdentityTransform, rotation);
    *bm = tmp;
    REFCOUNT_ADD(*bm)
    return NOERROR;
}

ECode CSurfaceControlHelper::Screenshot(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [out] */ IBitmap** bm)
{
    AutoPtr<IBitmap> tmp = SurfaceControl::Screenshot(width, height);
    *bm = tmp;
    REFCOUNT_ADD(*bm)
    return NOERROR;
}

} // namespace View
} // namespace Droid
} // namespace Elastos
