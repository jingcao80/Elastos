
#include "elastos/droid/view/CSurfaceViewWindow.h"
#include "elastos/droid/view/SurfaceView.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace View {

CAR_OBJECT_IMPL(CSurfaceViewWindow)

CSurfaceViewWindow::CSurfaceViewWindow()
{}

ECode CSurfaceViewWindow::constructor(
    /* [in] */ ISurfaceView* surfaceView)
{
    AutoPtr<IWeakReferenceSource> pWeak = IWeakReferenceSource::Probe(surfaceView);
    if (pWeak != NULL) {
        pWeak->GetWeakReference((IWeakReference**)&mSurfaceView);
    }
    return NOERROR;
}

ECode CSurfaceViewWindow::Resized(
        /* [in] */ IRect* frame,
        /* [in] */ IRect* overscanInsets,
        /* [in] */ IRect* coveredInsets,
        /* [in] */ IRect* visibleInsets,
        /* [in] */ IRect* stableInsets,
        /* [in] */ Boolean reportDraw,
        /* [in] */ IConfiguration* newConfig)
{
    AutoPtr<IInterface> obj;
    mSurfaceView->Resolve(EIID_IInterface, (IInterface**)&obj);
    if(obj != NULL) {
        AutoPtr<ISurfaceView> tmp = ISurfaceView::Probe(obj);
        SurfaceView* surfaceView = (SurfaceView*)tmp.Get();

        if (surfaceView != NULL) {
            Int32 w, h, winW, winH;
            frame->GetWidth(&w);
            frame->GetHeight(&h);
            surfaceView->mWinFrame->GetWidth(&winW);
            surfaceView->mWinFrame->GetHeight(&winH);

            if (SurfaceView::DEBUG) {
                Logger::V("SurfaceView", "surfaceView %p got resized: w=%d h=%d, cur w=%d h=%d\n",
                    surfaceView, w, h, mCurWidth, mCurHeight);
            }

            {
                AutoLock lock(surfaceView->mSurfaceLock);
                Boolean result;
    //        try {
                if (reportDraw) {
                    surfaceView->mUpdateWindowNeeded = TRUE;
                    surfaceView->mReportDrawNeeded = TRUE;
                    surfaceView->mHandler->SendEmptyMessage(
                        SurfaceView::UPDATE_WINDOW_MSG, &result);
                }
                else if (winW != w || winH != h) {
                    surfaceView->mUpdateWindowNeeded = TRUE;
                    surfaceView->mHandler->SendEmptyMessage(
                        SurfaceView::UPDATE_WINDOW_MSG, &result);
                }

    //        } finally {
               surfaceView->mSurfaceLock.Unlock();
    //        }
            }
        }
    }

    return NOERROR;
}

ECode CSurfaceViewWindow::DispatchGetNewSurface()
{
    AutoPtr<IInterface> obj;
    mSurfaceView->Resolve(EIID_IInterface, (IInterface**)&obj);
    if(obj != NULL) {
        AutoPtr<ISurfaceView> tmp = ISurfaceView::Probe(obj);
        SurfaceView* surfaceView = (SurfaceView*)tmp.Get();
        Boolean result;
        surfaceView->mHandler->SendEmptyMessage(
            SurfaceView::GET_NEW_SURFACE_MSG, &result);
    }
    return NOERROR;
}

ECode CSurfaceViewWindow::WindowFocusChanged(
    /* [in] */ Boolean hasFocus,
    /* [in] */ Boolean touchEnabled)
{
    Logger::W("CSurfaceViewWindow", "Unexpected focus in surface: focus=%d, touchEnabled=%d\n",
            hasFocus, touchEnabled);
    return NOERROR;
}

ECode CSurfaceViewWindow::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder sb("CSurfaceViewWindow: ");
    sb += (Int32)this;
    *str = sb.ToString();
    return NOERROR;
}

} // namespace View
} // namespace Droid
} // namespace Elastos
