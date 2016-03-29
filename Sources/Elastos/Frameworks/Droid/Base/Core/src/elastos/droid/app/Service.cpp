
#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/app/Service.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/R.h"
#include "elastos/droid/content/CComponentName.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::R;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::Content::EIID_IComponentCallbacks;
using Elastos::Droid::Content::EIID_IComponentCallbacks2;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace App {

static const String TAG("Service");

CAR_INTERFACE_IMPL_3(Service, ContextWrapper, IService, IComponentCallbacks, IComponentCallbacks2)

Service::Service()
    : mStartCompatibility(FALSE)
{
}

Service::~Service()
{
}

ECode Service::constructor()
{
    return NOERROR;
}

ECode Service::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    StringBuilder sb("Service:(");
    sb += (Int32)this;
    sb += ")";
    sb.ToString(info);
    return NOERROR;
}

ECode Service::GetApplication(
    /* [out] */ IApplication** application)
{
    VALIDATE_NOT_NULL(application);

    *application = mApplication;
    REFCOUNT_ADD(*application);

    return NOERROR;
}

ECode Service::OnApplyThemeResource(
    /* [in] */ IResourcesTheme* theme,
    /* [in] */ Int32 resid,
    /* [in] */ Boolean first)
{
    return theme->ApplyStyle(resid, TRUE);
}

ECode Service::OnCreate()
{
    return NOERROR;
}

ECode Service::OnStart(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 startId)
{
    return NOERROR;
}

ECode Service::OnStartCommand(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 startId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    OnStart(intent, startId);
    *result = mStartCompatibility ?
            IService::START_STICKY_COMPATIBILITY : IService::START_STICKY;
    return NOERROR;
}

ECode Service::OnDestroy()
{
    return NOERROR;
}

ECode Service::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    return NOERROR;
}

ECode Service::OnLowMemory()
{
    return NOERROR;
}

ECode Service::OnTrimMemory(
    /* [in] */ Int32 level)
{
    return NOERROR;
}

ECode Service::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** binder)
{
    VALIDATE_NOT_NULL(binder);
    *binder = NULL;
    return NOERROR;
}

ECode Service::OnUnbind(
    /* [in] */ IIntent* intent,
    /* [out] */ Boolean* callOnRebindLater)
{
    VALIDATE_NOT_NULL(callOnRebindLater);
    *callOnRebindLater = FALSE;
    return NOERROR;
}

ECode Service::OnRebind(
    /* [in] */ IIntent* intent)
{
    return NOERROR;
}

ECode Service::OnTaskRemoved(
    /* [in] */ IIntent* rootIntent)
{
    return NOERROR;
}

ECode Service::StopSelf()
{
    return StopSelf(-1);
}

ECode Service::StopSelf(
    /* [in] */ Int32 startId)
{
    if (mActivityManager == NULL) {
        return NOERROR;
    }

    AutoPtr<IComponentName> cn;
    FAIL_RETURN(CComponentName::New(this, mClassName, (IComponentName**)&cn));

    Boolean res;
    ECode ec = mActivityManager->StopServiceToken(cn, mToken, startId, &res);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        ec = NOERROR;
    }
    return ec;
}

ECode Service::StopSelfResult(
    /* [in] */ Int32 startId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    if (mActivityManager == NULL) {
        return NOERROR;
    }

    AutoPtr<IComponentName> cn;
    FAIL_RETURN(CComponentName::New(this, mClassName, (IComponentName**)&cn));

    ECode ec = mActivityManager->StopServiceToken(cn, mToken, startId, result);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        ec = NOERROR;
    }
    return ec;
}

ECode Service::SetForeground(
    /* [in] */ Boolean isForeground)
{
    Logger::W(TAG, String("setForeground: ignoring old API call on Service"));
    return NOERROR;
}

ECode Service::StartForeground(
    /* [in] */ Int32 id,
    /* [in] */ INotification* notification)
{
    String packageName;
    GetPackageName(&packageName);
    AutoPtr<IComponentName> cn;
    FAIL_RETURN(CComponentName::New(this, mClassName, (IComponentName**)&cn));

    return mActivityManager->SetServiceForeground(cn, mToken, id, notification, TRUE);
}

ECode Service::StopForeground(
    /* [in] */ Boolean removeNotification)
{
    String packageName;
    GetPackageName(&packageName);
    AutoPtr<IComponentName> cn;
    FAIL_RETURN(CComponentName::New(this, mClassName, (IComponentName**)&cn));

    return mActivityManager->SetServiceForeground(cn, mToken, 0, NULL, removeNotification);
}

ECode Service::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* writer,
    /* [in] */ ArrayOf<String>* args)
{
    return writer->Println(String("nothing to dump"));
}

// ------------------ Internal API ------------------

ECode Service::Attach(
    /* [in] */ IContext* context,
    /* [in] */ IActivityThread* thread,
    /* [in] */ const String& className,
    /* [in] */ IBinder* token,
    /* [in] */ IApplication* application,
    /* [in] */ IIActivityManager* activityManager)
{
    FAIL_RETURN(AttachBaseContext(context));

    mThread = thread;            // NOTE:  unused - remove?
    mClassName = className;
    mToken = token;
    mApplication = application;
    mActivityManager = activityManager;

    Int32 targetSdkVersion;
    AutoPtr<IApplicationInfo> appInfo;
    FAIL_RETURN(GetApplicationInfo((IApplicationInfo**)&appInfo));
    FAIL_RETURN(appInfo->GetTargetSdkVersion(&targetSdkVersion));

    mStartCompatibility = targetSdkVersion < Build::VERSION_CODES::ECLAIR;
    return NOERROR;
}

ECode Service::GetClassName(
    /* [out] */ String* className)
{
    VALIDATE_NOT_NULL(className);
    *className = mClassName;
    return NOERROR;
}

} // App
} // Droid
} // Elastos
