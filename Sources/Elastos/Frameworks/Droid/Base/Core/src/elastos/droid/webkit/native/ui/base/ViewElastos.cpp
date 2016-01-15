
#include "elastos/droid/webkit/native/ui/base/ViewElastos.h"
#include "elastos/droid/webkit/native/ui/api/ViewElastos_dec.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Ui {
namespace Base {

//=====================================================================
//                             ViewElastos
//=====================================================================
ViewElastos::ViewElastos(
    /* [in] */ WindowElastos* nativeWindow,
    /* [in] */ ViewElastosDelegate* viewElastosDelegate)
    : mNativeViewElastos(0)
    , mViewElastosDelegate(viewElastosDelegate)
    , mWindowElastos(nativeWindow)
    , mKeepScreenOnCount(0)
    , mKeepScreenOnView(NULL)
{
    // ==================before translated======================
    // mWindowElastos = nativeWindow;
    // mViewElastosDelegate = viewAndroidDelegate;
    // mNativeViewElastos = nativeInit(mWindowElastos.getNativePointer());

    mNativeViewElastos = NativeInit(mWindowElastos->GetNativePointer());
}

AutoPtr<ViewElastosDelegate> ViewElastos::GetViewElastosDelegate()
{
    // ==================before translated======================
    // return mViewElastosDelegate;

    return mViewElastosDelegate;
}

ECode ViewElastos::Destroy()
{
    // ==================before translated======================
    // if (mNativeViewElastos != 0) {
    //     nativeDestroy(mNativeViewElastos);
    //     mNativeViewElastos = 0;
    // }

    if (mNativeViewElastos != 0) {
        NativeDestroy(mNativeViewElastos);
        mNativeViewElastos = 0;
    }
    return NOERROR;
}

Int64 ViewElastos::GetNativePointer()
{
    // ==================before translated======================
    // return mNativeViewElastos;

    return mNativeViewElastos;
}

ECode ViewElastos::IncrementKeepScreenOnCount()
{
    // ==================before translated======================
    // mKeepScreenOnCount++;
    // if (mKeepScreenOnCount == 1) {
    //     mKeepScreenOnView = mViewElastosDelegate.acquireAnchorView();
    //     mViewElastosDelegate.setAnchorViewPosition(mKeepScreenOnView, 0, 0, 0, 0);
    //     mKeepScreenOnView.setKeepScreenOn(true);
    // }

    mKeepScreenOnCount++;
    if (mKeepScreenOnCount == 1) {
        mKeepScreenOnView = mViewElastosDelegate->AcquireAnchorView();
        mViewElastosDelegate->SetAnchorViewPosition(mKeepScreenOnView, 0, 0, 0, 0);
        mKeepScreenOnView->SetKeepScreenOn(TRUE);
    }
    return NOERROR;
}

ECode ViewElastos::DecrementKeepScreenOnCount()
{
    // ==================before translated======================
    // assert mKeepScreenOnCount > 0;
    // mKeepScreenOnCount--;
    // if (mKeepScreenOnCount == 0) {
    //     mViewElastosDelegate.releaseAnchorView(mKeepScreenOnView);
    //     mKeepScreenOnView = null;
    // }

    assert (mKeepScreenOnCount > 0);
    --mKeepScreenOnCount;
    if (mKeepScreenOnCount == 0) {
        mViewElastosDelegate->ReleaseAnchorView(mKeepScreenOnView);
        mKeepScreenOnView = NULL;
    }
    return NOERROR;
}

Int64 ViewElastos::NativeInit(
    /* [in] */ Int64 windowPtr)
{
    return Elastos_ViewAndroid_nativeInit(THIS_PROBE(IInterface), windowPtr);
}

ECode ViewElastos::NativeDestroy(
    /* [in] */ Int64 nativeViewElastos)
{
    Elastos_ViewAndroid_nativeDestroy(THIS_PROBE(IInterface), (Handle32)nativeViewElastos);
    return NOERROR;
}

} // namespace Base
} // namespace Ui
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


