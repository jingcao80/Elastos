
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/webkit/native/content/browser/ScreenOrientationListener.h"
#include "elastos/droid/webkit/native/ui/gfx/DeviceDisplayInfo.h"
#include "elastos/droid/webkit/native/base/ThreadUtils.h"
#include <elastos/utility/logging/Slogger.h>
#include "elastos/droid/os/Build.h"

using Elastos::Droid::Content::EIID_IComponentCallbacks;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Webkit::Base::ThreadUtils;
using Elastos::Droid::Webkit::Ui::Gfx::DeviceDisplayInfo;
using Elastos::Droid::Hardware::Display::IDisplayManager;
using Elastos::Droid::Hardware::Display::EIID_IDisplayListener;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {

//=====================================================================
//               ScreenOrientationListener::InnerRunnable
//=====================================================================
ScreenOrientationListener::InnerRunnable::InnerRunnable(
    /* [in] */ ScreenOrientationListener* owner,
    /* [in] */ ScreenOrientationObserver* obs)
    : mOwner(owner)
    , mObs(obs)
{
}

ECode ScreenOrientationListener::InnerRunnable::Run()
{
    mObs->OnScreenOrientationChanged(mOwner->mOrientation);
    return NOERROR;
}

//=====================================================================
//  ScreenOrientationListener::ScreenOrientationConfigurationListener
//=====================================================================

ScreenOrientationListener::ScreenOrientationConfigurationListener::ScreenOrientationConfigurationListener(
    /* [in] */ ScreenOrientationListener* owner)
    : mOwner(owner)
{
}

CAR_INTERFACE_IMPL(ScreenOrientationListener::ScreenOrientationConfigurationListener, ScreenOrientationListenerBackend, IComponentCallbacks);

ECode ScreenOrientationListener::ScreenOrientationConfigurationListener::StartListening()
{
    mOwner->mAppContext->RegisterComponentCallbacks(this);
    return NOERROR;
}

ECode ScreenOrientationListener::ScreenOrientationConfigurationListener::StopListening()
{
    mOwner->mAppContext->UnregisterComponentCallbacks(this);
    return NOERROR;
}

ECode ScreenOrientationListener::ScreenOrientationConfigurationListener::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    VALIDATE_NOT_NULL(newConfig);
    mOwner->NotifyObservers();
    return NOERROR;
}

ECode ScreenOrientationListener::ScreenOrientationConfigurationListener::OnLowMemory()
{
    return NOERROR;
}

//=====================================================================
//     ScreenOrientationListener::ScreenOrientationDisplayListener
//=====================================================================

ScreenOrientationListener::ScreenOrientationDisplayListener::ScreenOrientationDisplayListener(
    /* [in] */ ScreenOrientationListener* owner)
    : mOwner(owner)
{
}

CAR_INTERFACE_IMPL(ScreenOrientationListener::ScreenOrientationDisplayListener, ScreenOrientationListenerBackend, IDisplayListener);

ECode ScreenOrientationListener::ScreenOrientationDisplayListener::StartListening()
{
    AutoPtr<IDisplayManager> displayManager;
    mOwner->mAppContext->GetSystemService(IContext::DISPLAY_SERVICE, (IInterface**)&displayManager);
    return displayManager->RegisterDisplayListener(this, NULL);
}

ECode ScreenOrientationListener::ScreenOrientationDisplayListener::StopListening()
{
    AutoPtr<IDisplayManager> displayManager;
    mOwner->mAppContext->GetSystemService(IContext::DISPLAY_SERVICE, (IInterface**)&displayManager);
    return displayManager->UnregisterDisplayListener(this);
}

ECode ScreenOrientationListener::ScreenOrientationDisplayListener::OnDisplayAdded(
    /* [in] */ Int32 displayId)
{
    return NOERROR;
}

ECode ScreenOrientationListener::ScreenOrientationDisplayListener::OnDisplayRemoved(
    /* [in] */ Int32 displayId)
{
    return NOERROR;
}

ECode ScreenOrientationListener::ScreenOrientationDisplayListener::OnDisplayChanged(
    /* [in] */ Int32 displayId)
{
    mOwner->NotifyObservers();
    return NOERROR;
}

//=====================================================================
//                      ScreenOrientationListener
//=====================================================================

const String ScreenOrientationListener::TAG("ScreenOrientationListener");
AutoPtr<ScreenOrientationListener> ScreenOrientationListener::sInstance;

ScreenOrientationListener::ScreenOrientationListener()
{
    mBackend = Build::VERSION::SDK_INT >= 17 ?
            (ScreenOrientationListenerBackend*)(new ScreenOrientationDisplayListener(this)) :
            (ScreenOrientationListenerBackend*)(new ScreenOrientationConfigurationListener(this));
}

AutoPtr<ScreenOrientationListener> ScreenOrientationListener::GetInstance()
{
    ThreadUtils::AssertOnUiThread();

    if (sInstance == NULL) {
        sInstance = new ScreenOrientationListener();
    }

    return sInstance;
}

ECode ScreenOrientationListener::InjectConfigurationListenerBackendForTest()
{
    mBackend = (ScreenOrientationListenerBackend*)(new ScreenOrientationConfigurationListener(this));
    return NOERROR;
}

ECode ScreenOrientationListener::AddObserver(
    /* [in] */ ScreenOrientationObserver* observer,
    /* [in] */ IContext* context)
{
    VALIDATE_NOT_NULL(observer);
    VALIDATE_NOT_NULL(context);

    if (mAppContext == NULL) {
        context->GetApplicationContext((IContext**)&mAppContext);
    }

    AutoPtr<IContext> con;
    context->GetApplicationContext((IContext**)&con);
    assert(mAppContext == con);
    assert(mAppContext != NULL);

    if (!mObservers.AddObserver((IObject*)observer)) {
        Slogger::W(TAG, "Adding an observer that is already present!");
        return NOERROR;
    }

    // If we got our first observer, we should start listening.
    if (mObservers.Size() == 1) {
        UpdateOrientation();
        mBackend->StartListening();
    }

    // We need to send the current value to the added observer as soon as
    // possible but outside of the current stack.
    AutoPtr<ScreenOrientationObserver> obs = observer;
    ThreadUtils::AssertOnUiThread();
    AutoPtr<IRunnable> runnable = new InnerRunnable(this, obs);
    ThreadUtils::PostOnUiThread(runnable);

    return NOERROR;
}

ECode ScreenOrientationListener::RemoveObserver(
    /* [in] */ ScreenOrientationObserver* observer)
{
    VALIDATE_NOT_NULL(observer);

    if (!mObservers.RemoveObserver((IObject*)observer)) {
        Slogger::W(TAG, "Removing an inexistent observer!");
        return NOERROR;
    }

    if (mObservers.IsEmpty()) {
        // The last observer was removed, we should just stop listening.
        mBackend->StopListening();
    }

    return NOERROR;
}

ECode ScreenOrientationListener::NotifyObservers()
{
    Int32 previousOrientation = mOrientation;
    UpdateOrientation();

    DeviceDisplayInfo::Create(mAppContext)->UpdateNativeSharedDisplayInfo();

    if (mOrientation == previousOrientation) {
        return NOERROR;
    }

    AutoPtr<IIterator> iter;
    mObservers.GetIterator((IIterator**)&iter);
    Boolean bNext;
    for (iter->HasNext(&bNext); bNext; iter->HasNext(&bNext)) {
        AutoPtr<ScreenOrientationObserver> observer;
        iter->GetNext((IInterface**)&observer);
        observer->OnScreenOrientationChanged(mOrientation);
    }

    return NOERROR;
}

ECode ScreenOrientationListener::UpdateOrientation()
{
    AutoPtr<IWindowManager> windowManager;
    mAppContext->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&windowManager);

    AutoPtr<IDisplay> display;
    windowManager->GetDefaultDisplay((IDisplay**)&display);
    Int32 rotation;
    display->GetRotation(&rotation);
    switch (rotation) {
        case ISurface::ROTATION_0:
            mOrientation = 0;
            break;
        case ISurface::ROTATION_90:
            mOrientation = 90;
            break;
        case ISurface::ROTATION_180:
            mOrientation = 180;
            break;
        case ISurface::ROTATION_270:
            mOrientation = -90;
            break;
        default:
            assert(0);
            // throw new IllegalStateException(
            //         "Display.getRotation() shouldn't return that value");
    }

    return NOERROR;
}

} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
