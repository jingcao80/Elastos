
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include "elastos/droid/internal/view/RotationPolicy.h"
#include "elastos/droid/graphics/CPoint.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/os/AsyncTask.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/view/CWindowManagerGlobal.h"
#include "elastos/droid/R.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::CPoint;
using Elastos::Droid::Graphics::IPoint;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::View::CWindowManagerGlobal;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::View::IIWindowManager;
using Elastos::Droid::View::ISurface;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {

RotationPolicy::RotationPolicyListener::RotationContentObserver::RotationContentObserver(
    /* [in] */ RotationPolicyListener* owner)
    : mOwner(owner)
{
}

ECode RotationPolicy::RotationPolicyListener::RotationContentObserver::OnChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri)
{
    return mOwner->OnChange();
}
CAR_INTERFACE_IMPL(RotationPolicy::RotationPolicyListener, Object, IRotationPolicyListener)

RotationPolicy::RotationPolicyListener::RotationPolicyListener()
{
    AutoPtr<IHandler> handler;
    CHandler::New((IHandler**)&handler);
    mObserver = new RotationContentObserver(this);
    mObserver->constructor(handler);
}

RotationPolicy::RotationRunnable::RotationRunnable(
    /* [in] */ Boolean enabled,
    /* [in] */ Int32 rotation)
    : mEnabled(enabled)
    , mRotation(rotation)
{
}

ECode RotationPolicy::RotationRunnable::Run()
{
    ECode ec;
    AutoPtr<IIWindowManager> wm = CWindowManagerGlobal::GetWindowManagerService();
    if (mEnabled) {
        ec = wm->FreezeRotation(mRotation);
    }
    else {
        ec = wm->ThawRotation();
    }
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Logger::W(TAG, "Unable to save auto-rotate setting");
    }

    return NOERROR;
}

const String RotationPolicy::TAG("RotationPolicy");
const Int32 RotationPolicy::CURRENT_ROTATION = -1;
const Int32 RotationPolicy::NATURAL_ROTATION = ISurface::ROTATION_0;

Boolean RotationPolicy::IsRotationSupported(
    /* [in] */ IContext* context)
{
    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IResources> resources;
    Boolean r1, r2, r3, r4;
    return (pm->HasSystemFeature(IPackageManager::FEATURE_SENSOR_ACCELEROMETER, &r1), r1)
        && (pm->HasSystemFeature(IPackageManager::FEATURE_SCREEN_PORTRAIT, &r2), r2)
        && (pm->HasSystemFeature(IPackageManager::FEATURE_SCREEN_LANDSCAPE, &r3), r3)
        && (context->GetResources((IResources**)&resources), resources->GetBoolean(
            R::bool_::config_supportAutoRotation, &r4), r4);
}

Int32 RotationPolicy::GetRotationLockOrientation(
    /* [in] */ IContext* context)
{
    if (!AreAllRotationsAllowed(context)) {
        AutoPtr<IPoint> size;
        CPoint::New((IPoint**)&size);
        AutoPtr<IIWindowManager> wm = CWindowManagerGlobal::GetWindowManagerService();
        AutoPtr<IPoint> outSize;
        if (SUCCEEDED(wm->GetInitialDisplaySize(IDisplay::DEFAULT_DISPLAY,
            size, (IPoint**)&outSize))) {
            Int32 x, y;
            outSize->GetX(&x);
            outSize->GetY(&y);
            return x < y ? IConfiguration::ORIENTATION_PORTRAIT
                : IConfiguration::ORIENTATION_LANDSCAPE;
        }
        Logger::W(TAG, "Unable to get the display size");
    }
    return IConfiguration::ORIENTATION_UNDEFINED;
}

Boolean RotationPolicy::IsRotationLockToggleVisible(
    /* [in] */ IContext* context)
{
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    Int32 value;
    return IsRotationSupported(context) &&
        (Settings::System::GetInt32ForUser(cr,
            ISettingsSystem::HIDE_ROTATION_LOCK_TOGGLE_FOR_ACCESSIBILITY, 0,
                IUserHandle::USER_CURRENT, &value), value) == 0;
}

Boolean RotationPolicy::IsRotationLocked(
    /* [in] */ IContext* context)
{
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    Int32 value;
    return (Settings::System::GetInt32ForUser(cr,
        ISettingsSystem::ACCELEROMETER_ROTATION, 0,
            IUserHandle::USER_CURRENT, &value), value) == 0;
}

void RotationPolicy::SetRotationLock(
    /* [in] */ IContext* context,
    /* [in] */ Boolean enabled)
{
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    Boolean res;
    Settings::System::PutInt32ForUser(cr,
        ISettingsSystem::HIDE_ROTATION_LOCK_TOGGLE_FOR_ACCESSIBILITY,
        0, IUserHandle::USER_CURRENT, &res);

    Int32 rotation = AreAllRotationsAllowed(context) ? CURRENT_ROTATION : NATURAL_ROTATION;
    SetRotationLock(enabled, rotation);
}

void RotationPolicy::SetRotationLockForAccessibility(
    /* [in] */ IContext* context,
    /* [in] */ Boolean enabled)
{
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    Boolean res;
    Settings::System::PutInt32ForUser(cr,
        ISettingsSystem::HIDE_ROTATION_LOCK_TOGGLE_FOR_ACCESSIBILITY,
        enabled ? 1 : 0, IUserHandle::USER_CURRENT, &res);

    SetRotationLock(enabled, NATURAL_ROTATION);
}

Boolean RotationPolicy::AreAllRotationsAllowed(
    /* [in] */ IContext* context)
{
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    Boolean res;
    resources->GetBoolean(R::bool_::config_allowAllRotations, &res);
    return res;
}

void RotationPolicy::SetRotationLock(
    /* [in] */ Boolean enabled,
    /* [in] */ Int32 rotation)
{
    AutoPtr<RotationRunnable> runnable = new RotationRunnable(enabled, rotation);
    AsyncTask::Execute(runnable);
}

void RotationPolicy::RegisterRotationPolicyListener(
    /* [in] */ IContext* context,
    /* [in] */ IRotationPolicyListener* listener)
{
    RegisterRotationPolicyListener(context, listener, UserHandle::GetCallingUserId());
}

void RotationPolicy::RegisterRotationPolicyListener(
    /* [in] */ IContext* context,
    /* [in] */ IRotationPolicyListener* _listener,
    /* [in] */ Int32 userHandle)
{
    RotationPolicyListener* listener = (RotationPolicyListener*)_listener;
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<IUri> uri;
    Settings::System::GetUriFor(ISettingsSystem::ACCELEROMETER_ROTATION, (IUri**)&uri);
    cr->RegisterContentObserver(uri, FALSE, listener->mObserver, userHandle);
    uri = NULL;
    Settings::System::GetUriFor(
        ISettingsSystem::HIDE_ROTATION_LOCK_TOGGLE_FOR_ACCESSIBILITY, (IUri**)&uri);
    cr->RegisterContentObserver(uri, FALSE, listener->mObserver, userHandle);
}

void RotationPolicy::UnregisterRotationPolicyListener(
    /* [in] */ IContext* context,
    /* [in] */ IRotationPolicyListener* listener)
{
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    cr->UnregisterContentObserver(((RotationPolicyListener*)listener)->mObserver);
}

} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos
