
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/app/admin/CDeviceAdminReceiver.h"

using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace App {
namespace Admin {

const String TAG("DevicePolicy");

Boolean CDeviceAdminReceiver::sLocalLOGV = FALSE;

ECode CDeviceAdminReceiver::GetManager(
    /* [in] */ IContext* context,
    /* [out] */ IDevicePolicyManager** manager)
{
    VALIDATE_NOT_NULL(context);
    VALIDATE_NOT_NULL(manager);
    *manager = NULL;

    if (mManager != null) {
        *manager = mManager;
        REFCOUNT_ADD(*manager);
        return NOERROR;
    }
    AutoPtr<IInterface> service;
    FAIL_RETURN(context->GetSystemService(IContext::DEVICE_POLICY_SERVICE, (IInterface**)&service))
    *manager = IDevicePolicyManager::Probe(service)
    REFCOUNT_ADD(*manager)
    return NOERROR;
}

ECode CDeviceAdminReceiver::GetWho(
    /* [in] */ IContext* context,
    /* [out] */ IComponentName** component)
{
    VALIDATE_NOT_NULL(context);
    VALIDATE_NOT_NULL(component);

    if (mWho != null) {
        *component = mWho;
        REFCOUNT_ADD(*component);
        return NOERROR;
    }

    //mWho = new ComponentName(context, getClass());
    String className("CDeviceAdminReceiver");
    CComponentName::New(context, className, (IComponentName**)&mWho);
    *component = mWho;
    REFCOUNT_ADD(*component);
    return NOERROR;
}

ECode CDeviceAdminReceiver::OnEnabled(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    return NOERROR;
}

ECode CDeviceAdminReceiver::OnDisableRequested(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent,
    /* [out] */ ICharSequence** message)
{
    VALIDATE_NOT_NULL(message);
    *message = NULL;
    return NOERROR;
}

ECode CDeviceAdminReceiver::OnDisabled(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    return NOERROR;
}

ECode CDeviceAdminReceiver::OnPasswordChanged(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    return NOERROR;
}

ECode CDeviceAdminReceiver::OnPasswordFailed(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    return NOERROR;
}

ECode CDeviceAdminReceiver::OnPasswordSucceeded(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    return NOERROR;
}

ECode CDeviceAdminReceiver::OnPasswordExpiring(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    return NOERROR;
}

ECode CDeviceAdminReceiver::OnProfileProvisioningComplete(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    return NOERROR;
}

ECode CDeviceAdminReceiver::OnLockTaskModeEntering(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent,
    /* [in] */ const String& pkg
{
    return NOERROR;
}

ECode CDeviceAdminReceiver::OnLockTaskModeExiting(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    return NOERROR;
}

ECode CDeviceAdminReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);

    if (IDeviceAdminReceiver::ACTION_PASSWORD_CHANGED.Equals(action)) {
        OnPasswordChanged(context, intent);
    }
    else if (IDeviceAdminReceiver::ACTION_PASSWORD_FAILED.Equals(action)) {
        OnPasswordFailed(context, intent);
    }
    else if (IDeviceAdminReceiver::ACTION_PASSWORD_SUCCEEDED.Equals(action)) {
        OnPasswordSucceeded(context, intent);
    }
    else if (IDeviceAdminReceiver::ACTION_DEVICE_ADMIN_ENABLED.Equals(action)) {
        OnEnabled(context, intent);
    }
    else if (IDeviceAdminReceiver::ACTION_DEVICE_ADMIN_DISABLE_REQUESTED.Equals(action)) {
        AutoPtr<ICharSequence> res;
        OnDisableRequested(context, intent, (ICharSequence**)&res);
        if (res != NULL) {
            AutoPtr<IBundle> extras;
            GetResultExtras(TRUE, (IBundle**)&extras);
            extras->PutCharSequence(IDeviceAdminReceiver::EXTRA_DISABLE_WARNING, res);
        }
    }
    else if (IDeviceAdminReceiver::ACTION_DEVICE_ADMIN_DISABLED.Equals(action)) {
        OnDisabled(context, intent);
    }
    else if (IDeviceAdminReceiver::ACTION_PASSWORD_EXPIRING.Equals(action)) {
        OnPasswordExpiring(context, intent);
    }
    else if (IDeviceAdminReceiver::ACTION_PROFILE_PROVISIONING_COMPLETE.equals(action)) {
        OnProfileProvisioningComplete(context, intent);
    }
    else if (IDeviceAdminReceiver::ACTION_LOCK_TASK_ENTERING.Equals(action)) {
        String pkg;
        intent->GetStringExtra(EXTRA_LOCK_TASK_PACKAGE, &pkg);
        OnLockTaskModeEntering(context, intent, pkg);
    }
    else if (IDeviceAdminReceiver::ACTION_LOCK_TASK_EXITING.Equals(action)) {
        OnLockTaskModeExiting(context, intent);
    }
    return NOERROR;
}

} // namespace Admin
} // namespace App
} // namespace Droid
} // namespace Elastos
