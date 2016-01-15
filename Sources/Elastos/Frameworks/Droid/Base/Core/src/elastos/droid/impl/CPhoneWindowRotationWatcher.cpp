#include "elastos/droid/impl/CPhoneWindowRotationWatcher.h"
#include "elastos/droid/impl/CPhoneWindow.h"
#include "elastos/droid/os/CHandler.h"

using Elastos::Droid::Os::CHandler;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

CPhoneWindowRotationWatcher::MyRunnable::MyRunnable(
    /* [in] */ CPhoneWindowRotationWatcher* host)
    : mHost(host)
{}

ECode CPhoneWindowRotationWatcher::MyRunnable::Run()
{
    return mHost->DispatchRotationChanged();
}

CPhoneWindowRotationWatcher::CPhoneWindowRotationWatcher()
    : mIsWatching(FALSE)
{
    mRotationChanged = new MyRunnable(this);
}

//ECode CPhoneWindowRotationWatcher::constructor()
//{
//    return NOERROR;
//}

ECode CPhoneWindowRotationWatcher::OnRotationChanged(
/* [in] */ Int32 rotation)
{
    Boolean rst;
    return mHandler->Post(mRotationChanged, &rst);
}

ECode CPhoneWindowRotationWatcher::AddWindow(
/* [in] */ IWindow* phoneWindow)
{
    AutoLock locking(mLock);
    if (!mIsWatching) {
        Int32 rotation;
        ECode ec = CPhoneWindow::WindowManagerHolder::sWindowManager->WatchRotation(this, &rotation);
        if(FAILED(ec)) {
            SLOGGERE("CPhoneWindowRotationWatcher", "Couldn't start watching for device rotation");
        } else {
            CHandler::New((IHandler**)&mHandler);
            mIsWatching = TRUE;
        }
    }

    AutoPtr<IWeakReference> weakWindow;
    ((IWeakReferenceSource*)phoneWindow->Probe(EIID_IWeakReferenceSource))->GetWeakReference(
        (IWeakReference**)&weakWindow);
    mWindows.PushBack(weakWindow);
    return NOERROR;
}

ECode CPhoneWindowRotationWatcher::RemoveWindow(
/* [in] */ IWindow* phoneWindow)
{
    AutoLock locking(mLock);
    List<AutoPtr<IWeakReference> >::Iterator it = mWindows.Begin();
    while (it != mWindows.End()) {
        AutoPtr<IWeakReference> weakWindow = *it;
        AutoPtr<IWindow> win;
        weakWindow->Resolve(EIID_IWindow, (IInterface**)&win);

        List<AutoPtr<IWeakReference> >::Iterator old = it++;
        if (win == NULL || win.Get() == phoneWindow) {
            mWindows.Erase(old);
        }
    }
    return NOERROR;
}

ECode CPhoneWindowRotationWatcher::DispatchRotationChanged()
{
    AutoLock locking(mLock);
    List<AutoPtr<IWeakReference> >::Iterator it = mWindows.Begin();
    while (it != mWindows.End()) {
        AutoPtr<IWeakReference> weakWindow = *it;
        AutoPtr<IWindow> win;
        weakWindow->Resolve(EIID_IWindow, (IInterface**)&win);
        List<AutoPtr<IWeakReference> >::Iterator old = it++;
        if (win != NULL) {
            reinterpret_cast<CPhoneWindow*>(win->Probe(EIID_CPhoneWindow))->OnOptionsPanelRotationChanged();
        } else {
            mWindows.Erase(old);
        }
    }
    return NOERROR;
}

}//namespace Impl
}// namespace Policy
}// namespace Internal
}// namespace Droid
}// namespace Elastos
