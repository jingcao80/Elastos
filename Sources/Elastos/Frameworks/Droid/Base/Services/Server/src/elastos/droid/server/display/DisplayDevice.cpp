#include <Elastos.Droid.View.h>
#include <Elastos.Droid.Graphics.h>
#include <Elastos.Droid.Hardware.h>
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/server/display/DisplayDevice.h"

using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::View::ISurfaceControlHelper;
using Elastos::Droid::View::CSurfaceControlHelper;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Display {

DisplayDevice::DisplayDevice(
    /* [in] */ DisplayAdapter* displayAdapter,
    /* [in] */ IBinder* displayToken)
    : mDisplayAdapter(displayAdapter)
    , mDisplayToken(displayToken)
    , mCurrentLayerStack(-1)
    , mCurrentOrientation(-1)
{}

AutoPtr<DisplayAdapter> DisplayDevice::GetAdapterLocked()
{
    return mDisplayAdapter;
}

AutoPtr<IBinder> DisplayDevice::GetDisplayTokenLocked()
{
    return mDisplayToken;
}

String DisplayDevice::GetNameLocked()
{
    return GetDisplayDeviceInfoLocked()->mName;
}

void DisplayDevice::ApplyPendingDisplayDeviceInfoChangesLocked()
{
}

void DisplayDevice::PerformTraversalInTransactionLocked()
{
}

AutoPtr<IRunnable> DisplayDevice::RequestDisplayStateLocked(
    /* [in] */ Int32 state)
{
    return NULL;
}

void DisplayDevice::RequestRefreshRateLocked(
    /* [in] */ Float refreshRate)
{
}

void DisplayDevice::SetLayerStackInTransactionLocked(
    /* [in] */ Int32 layerStack)
{
    if (mCurrentLayerStack != layerStack) {
        mCurrentLayerStack = layerStack;
        AutoPtr<ISurfaceControlHelper> surfaceControl;
        CSurfaceControlHelper::AcquireSingleton((ISurfaceControlHelper**)&surfaceControl);
        surfaceControl->SetDisplayLayerStack(mDisplayToken, layerStack);
    }
}

void DisplayDevice::SetProjectionInTransactionLocked(
    /* [in] */ Int32 orientation,
    /* [in] */ IRect* layerStackRect,
    /* [in] */ IRect* displayRect)
{
    Boolean isEquals;
    if (mCurrentOrientation != orientation
        || mCurrentLayerStackRect == NULL
        || !(mCurrentLayerStackRect->Equals(layerStackRect, &isEquals), isEquals)
        || mCurrentDisplayRect == NULL
        || !(mCurrentDisplayRect->Equals(displayRect, &isEquals), isEquals)) {
        mCurrentOrientation = orientation;

        if (mCurrentLayerStackRect == NULL) {
            ASSERT_SUCCEEDED(CRect::New((IRect**)&mCurrentLayerStackRect));
        }
        mCurrentLayerStackRect->Set(layerStackRect);

        if (mCurrentDisplayRect == NULL) {
            ASSERT_SUCCEEDED(CRect::New((IRect**)&mCurrentDisplayRect));
        }
        mCurrentDisplayRect->Set(displayRect);

        AutoPtr<ISurfaceControlHelper> surfaceControl;
        CSurfaceControlHelper::AcquireSingleton((ISurfaceControlHelper**)&surfaceControl);
        surfaceControl->SetDisplayProjection(mDisplayToken,
            orientation, layerStackRect, displayRect);
    }
}

void DisplayDevice::SetSurfaceInTransactionLocked(
    /* [in] */ ISurface* surface)
{
    if (mCurrentSurface.Get() != surface) {
        mCurrentSurface = surface;
        AutoPtr<ISurfaceControlHelper> surfaceControl;
        CSurfaceControlHelper::AcquireSingleton((ISurfaceControlHelper**)&surfaceControl);
        surfaceControl->SetDisplaySurface(mDisplayToken, surface);
    }
}

void DisplayDevice::PopulateViewportLocked(
    /* [in] */ IDisplayViewport* viewport)
{
    viewport->SetOrientation(mCurrentOrientation);

    AutoPtr<IRect> logicalFrame;
    viewport->GetLogicalFrame((IRect**)&logicalFrame);
    if (mCurrentLayerStackRect != NULL) {
        logicalFrame->Set(mCurrentLayerStackRect);
    }
    else {
        logicalFrame->SetEmpty();
    }

    AutoPtr<IRect> physicalFrame;
    viewport->GetPhysicalFrame((IRect**)&physicalFrame);
    if (mCurrentDisplayRect != NULL) {
        physicalFrame->Set(mCurrentDisplayRect);
    }
    else {
        physicalFrame->SetEmpty();
    }

    Boolean isRotated = (mCurrentOrientation == ISurface::ROTATION_90
        || mCurrentOrientation == ISurface::ROTATION_270);
    AutoPtr<DisplayDeviceInfo> info = GetDisplayDeviceInfoLocked();
    viewport->SetDeviceWidth(isRotated ? info->mHeight : info->mWidth);
    viewport->SetDeviceHeight(isRotated ? info->mWidth : info->mHeight);
}

void DisplayDevice::DumpLocked(
    /* [in] */ IPrintWriter* pw)
{
    //pw->Println(String("mAdapter=") + mDisplayAdapter->GetName());
    // pw->Println(String("mDisplayToken=") + mDisplayToken);
    // pw->Println(String("mCurrentLayerStack=") + mCurrentLayerStack);
    // pw->Println(String("mCurrentOrientation=") + mCurrentOrientation);
    // pw->Println(String("mCurrentLayerStackRect=") + mCurrentLayerStackRect);
    // pw->Println(String("mCurrentDisplayRect=") + mCurrentDisplayRect);
    // pw->Println(String("mCurrentSurface=") + mCurrentSurface);
    assert(0);
}

} // namespace Display
} // namespace Server
} // namespace Droid
} // namespace Elastos
