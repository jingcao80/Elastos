
#include "elastos/droid/view/WindowManagerImpl.h"
#include "elastos/droid/view/CWindowManagerGlobal.h"
#include "elastos/droid/ext/frameworkdef.h"

namespace Elastos {
namespace Droid {
namespace View {

CAR_INTERFACE_IMPL(WindowManagerImpl, Object, IWindowManager);
WindowManagerImpl::WindowManagerImpl(
    /* [in] */ IDisplay* display)
    : mDisplay(display)
    , mParentWindow(NULL)
{
    mGlobal = CWindowManagerGlobal::GetInstance();
}

WindowManagerImpl::WindowManagerImpl(
    /* [in] */ IDisplay* display,
    /* [in] */ IWindow* parentWindow)
    : mDisplay(display)
    , mParentWindow(parentWindow)
{
    mGlobal = CWindowManagerGlobal::GetInstance();
}

AutoPtr<WindowManagerImpl> WindowManagerImpl::CreateLocalWindowManager(
    /* [in] */ IWindow* parentWindow)
{
    return new WindowManagerImpl(mDisplay, parentWindow);
}

AutoPtr<WindowManagerImpl> WindowManagerImpl::CreatePresentationWindowManager(
    /* [in] */ IDisplay* display)
{
    return new WindowManagerImpl(display, mParentWindow);
}

ECode WindowManagerImpl::AddView(
    /* [in] */ IView* view,
    /* [in] */ IViewGroupLayoutParams* params)
{
    return mGlobal->AddView(view, params, mDisplay, mParentWindow);
}

ECode WindowManagerImpl::UpdateViewLayout(
    /* [in] */ IView* view,
    /* [in] */ IViewGroupLayoutParams* params)
{
    return mGlobal->UpdateViewLayout(view, params);
}

ECode WindowManagerImpl::RemoveView(
    /* [in] */ IView* view)
{
    return mGlobal->RemoveView(view, FALSE);
}

ECode WindowManagerImpl::RemoveViewImmediate(
    /* [in] */ IView* view)
{
    return mGlobal->RemoveView(view, TRUE);
}

ECode WindowManagerImpl::GetDefaultDisplay(
    /* [out] */ IDisplay** display)
{
    VALIDATE_NOT_NULL(display);
    *display = mDisplay;
    REFCOUNT_ADD(*display);
    return NOERROR;
}

} // namespace View
} // namepsace Droid
} // namespace Elastos
