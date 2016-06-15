#include "elastos/droid/internal/policy/impl/PhoneWindowRotationWatcher.h"
#include "elastos/droid/internal/policy/impl/PhoneWindow.h"
#include "elastos/droid/os/CHandler.h"
#include <elastos/core/AutoLock.h>

using Elastos::Droid::Internal::Policy::Impl::PhoneWindow;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::View::EIID_IRotationWatcher;
using Elastos::Core::AutoLock;
using Elastos::Core::EIID_IRunnable;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

//===============================================================================
//                  PhoneWindowRotationWatcher::InnerRunnable1
//===============================================================================

CAR_INTERFACE_IMPL(PhoneWindowRotationWatcher::InnerRunnable1, Object, IRunnable)

PhoneWindowRotationWatcher::InnerRunnable1::InnerRunnable1(
    /* [in] */ PhoneWindowRotationWatcher* owner)
    : mOwner(owner)
{
}

ECode PhoneWindowRotationWatcher::InnerRunnable1::Run()
{
    return mOwner->DispatchRotationChanged();
}

//===============================================================================
//                  PhoneWindowRotationWatcher
//===============================================================================

CAR_INTERFACE_IMPL_3(PhoneWindowRotationWatcher, Object, IBinder, IPhoneWindowRotationWatcher, IRotationWatcher)

PhoneWindowRotationWatcher::PhoneWindowRotationWatcher()
    : mIsWatching(FALSE)
{
    mRotationChanged = new InnerRunnable1(this);
    CArrayList::New((IArrayList**)&mWindows);
    mWindowsLock = new Object();
}

ECode PhoneWindowRotationWatcher::constructor()
{
    return NOERROR;
}

ECode PhoneWindowRotationWatcher::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info)
    return Object::ToString(info);
}

ECode PhoneWindowRotationWatcher::OnRotationChanged(
    /* [in] */ Int32 rotation)
{
    Boolean res;
    return mHandler->Post(mRotationChanged, &res);
}

ECode PhoneWindowRotationWatcher::AddWindow(
    /* [in] */ IPhoneWindow* phoneWindow)
{
    {
        AutoLock lock(mWindowsLock);

        if (!mIsWatching) {
            // try {
            AutoPtr<IIWindowManager> windowManager = PhoneWindow::WindowManagerHolder::GetWindowManager();
            Int32 rot;
            ECode ec = windowManager->WatchRotation(this, &rot);
            if (FAILED(ec)) {
                Logger::E("PhoneWindowRotationWatcher::AddWindow", "Couldn't start watching for device rotation, 0x%08x", ec);
            }
            else {
                CHandler::New((IHandler**)&mHandler);
                mIsWatching = TRUE;
            }
            // } catch (RemoteException ex) {
            //     Logger::E(TAG, "Couldn't start watching for device rotation", ex);
            // }
        }
        AutoPtr<IWeakReference> ref;
        IWeakReferenceSource::Probe(phoneWindow)->GetWeakReference((IWeakReference**)&ref);
        mWindows->Add(ref);
    }

    return NOERROR;
}

ECode PhoneWindowRotationWatcher::RemoveWindow(
    /* [in] */ IPhoneWindow* phoneWindow)
{
    {
        AutoLock lock(mWindowsLock);

        Int32 i = 0, size = 0;
        while (i < (mWindows->GetSize(&size), size)) {
            AutoPtr<IInterface> obj;
            mWindows->Get(i, (IInterface**)&obj);
            AutoPtr<IWeakReference> ref = IWeakReference::Probe(obj);
            AutoPtr<IPhoneWindow> win;
            ref->Resolve(EIID_IPhoneWindow, (IInterface**)&win);
            if (win == NULL || win.Get() == phoneWindow) {
                mWindows->Remove(i);
            }
            else {
                i++;
            }
        }
    }
    return NOERROR;
}

ECode PhoneWindowRotationWatcher::DispatchRotationChanged()
{
    {
        AutoLock lock(mWindowsLock);

        Int32 i = 0, size = 0;
        while (i < (mWindows->GetSize(&size), size)) {
            AutoPtr<IInterface> obj;
            mWindows->Get(i, (IInterface**)&obj);
            AutoPtr<IWeakReference> ref = IWeakReference::Probe(obj);
            AutoPtr<IPhoneWindow> win;
            ref->Resolve(EIID_IPhoneWindow, (IInterface**)&win);
            if (win != NULL) {
                ((PhoneWindow*)win.Get())->OnOptionsPanelRotationChanged();
                i++;
            }
            else {
                mWindows->Remove(i);
            }
        }
    }
    return NOERROR;
}

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos
