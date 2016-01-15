#include "elastos/droid/webkit/native/android_webview/AwZoomControls.h"
#include "elastos/droid/webkit/native/android_webview/AwContents.h"
//TODO #include "elastos/droid/widget/CZoomButtonsController.h"

using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IFrameLayout;
using Elastos::Droid::Widget::IZoomButtonsController;
//TODO using Elastos::Droid::Widget::CZoomButtonsController;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::Widget::IFrameLayoutLayoutParams;
using Elastos::Droid::Widget::EIID_IFrameLayoutLayoutParams;
using Elastos::Droid::Widget::EIID_IOnZoomListener;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace AndroidWebview {

//===============================================================
//                  AwZoomControls::ZoomListener
//===============================================================
CAR_INTERFACE_IMPL(AwZoomControls::ZoomListener, Object, IOnZoomListener);

AwZoomControls::ZoomListener::ZoomListener(
    /* [in] */ AwZoomControls* owner)
    : mOwner(owner)
{
}

//@Override
ECode AwZoomControls::ZoomListener::OnVisibilityChanged(
    /* [in] */ Boolean visible)
{
    if (visible) {
        // Bring back the hidden zoom controls.
        AutoPtr<IView> controls;
        mOwner->mZoomButtonsController->GetZoomControls((IView**)&controls);
        controls->SetVisibility(IView::VISIBLE);
        mOwner->UpdateZoomControls();
    }

    return NOERROR;
}

//@Override
ECode AwZoomControls::ZoomListener::OnZoom(
    /* [in] */ Boolean zoomIn)
{
    if (zoomIn) {
        mOwner->mAwContents->ZoomIn();
    }
    else {
        mOwner->mAwContents->ZoomOut();
    }
    // ContentView will call updateZoomControls after its current page scale
    // is got updated from the native code.

    return NOERROR;
}

//===============================================================
//                        AwZoomControls
//===============================================================

AwZoomControls::AwZoomControls(
    /* [in] */ AwContents* awContents)
    : mAwContents(awContents)
{
}

//@Override
void AwZoomControls::InvokeZoomPicker()
{
    AutoPtr<IZoomButtonsController> zoomController = GetZoomController();
    if (zoomController != NULL) {
        zoomController->SetVisible(TRUE);
    }
}

//@Override
void AwZoomControls::DismissZoomPicker()
{
    AutoPtr<IZoomButtonsController> zoomController = GetZoomController();
    if (zoomController != NULL) {
        zoomController->SetVisible(FALSE);
    }
}

//@Override
void AwZoomControls::UpdateZoomControls()
{
    AutoPtr<IZoomButtonsController> zoomController = GetZoomController();
    if (zoomController == NULL) {
        return;
    }

    Boolean canZoomIn = FALSE;
    canZoomIn = mAwContents->CanZoomIn();
    Boolean canZoomOut = FALSE;
    canZoomOut = mAwContents->CanZoomOut();
    if (!canZoomIn && !canZoomOut) {
        // Hide the zoom in and out buttons if the page cannot zoom
        AutoPtr<IView> controls;
        zoomController->GetZoomControls((IView**)&controls);
        controls->SetVisibility(IView::GONE);
    }
    else {
        // Set each one individually, as a page may be able to zoom in or out
        zoomController->SetZoomInEnabled(canZoomIn);
        zoomController->SetZoomOutEnabled(canZoomOut);
    }
}

// This method is used in tests. It doesn't modify the state of zoom controls.
AutoPtr<IView> AwZoomControls::GetZoomControlsViewForTest()
{
    AutoPtr<IView> controls;
    return mZoomButtonsController != NULL ? (mZoomButtonsController->GetZoomControls((IView**)&controls), controls) : NULL;
}

AutoPtr<IZoomButtonsController> AwZoomControls::GetZoomController()
{
    Boolean shouldDZC = FALSE;
    shouldDZC = mAwContents->GetSettings()->ShouldDisplayZoomControls();
    if (mZoomButtonsController == NULL && shouldDZC) {
        AutoPtr<IView> containerView;
        AutoPtr<IViewGroup> viewGroup;
        viewGroup = mAwContents->GetContentViewCore()->GetContainerView();
        containerView = IView::Probe(viewGroup);
        //TODO CZoomButtonsController::New( containerView,(IZoomButtonsController**)&mZoomButtonsController);
        AutoPtr<ZoomListener> listener = new ZoomListener(this);
        mZoomButtonsController->SetOnZoomListener(listener);
        // ZoomButtonsController positions the buttons at the bottom, but in
        // the middle. Change their layout parameters so they appear on the
        // right.
        AutoPtr<IView> controls;
        mZoomButtonsController->GetZoomControls((IView**)&controls);
        AutoPtr<IViewGroupLayoutParams> params;
        controls->GetLayoutParams((IViewGroupLayoutParams**)&params);
        if (IFrameLayoutLayoutParams::Probe(params)) {
            AutoPtr<IFrameLayoutLayoutParams> layoutParams = IFrameLayoutLayoutParams::Probe(params);
            layoutParams->SetGravity(IGravity::RIGHT);
        }
    }

    return mZoomButtonsController;
}

} // namespace AndroidWebview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
