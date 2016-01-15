
#include "dreams/CDreamManagerService.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/Manifest.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IComponentNameHelper;
using Elastos::Droid::Content::CComponentNameHelper;
using Elastos::Droid::Content::EIID_IBroadcastReceiver;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::CSettingsSecure;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Dreams {

ECode CDreamManagerService::SRBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    AutoLock lock(mHost->mLock);
    mHost->StopDreamLocked();
    return NOERROR;
}

CAR_INTERFACE_IMPL(CDreamManagerService::StartDreamRunnable, IRunnable);

ECode CDreamManagerService::StartDreamRunnable::Run()
{
    mHost->mController->StartDream(mToken, mName, mIsTest, mUserId);
    return NOERROR;
}

CAR_INTERFACE_IMPL(CDreamManagerService::StopDreamRunnable, IRunnable);

ECode CDreamManagerService::StopDreamRunnable::Run()
{
    mHost->mController->StopDream();
    return NOERROR;
}

const Boolean CDreamManagerService::DEBUG = FALSE;
const String CDreamManagerService::TAG("DreamManagerService");

ECode CDreamManagerService::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IHandler* mainHandler)
{
    mContext = context;
    AutoPtr<ILooper> looper;
    mainHandler->GetLooper((ILooper**)&looper);
    mHandler = new DreamHandler(looper);
    mController = new DreamController(context, IHandler::Probe(mHandler), mControllerListener);
    AutoPtr<IInterface> systemService;
    context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&systemService);
    mPowerManager = IPowerManager::Probe(systemService);
    return NOERROR;
}

ECode CDreamManagerService::SystemReady()
{
    AutoPtr<SRBroadcastReceiver> br = new SRBroadcastReceiver(this);
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New(IIntent::ACTION_USER_SWITCHED, (IIntentFilter**)&filter);
    AutoPtr<IIntent> intent;
    FAIL_RETURN(mContext->RegisterReceiver((IBroadcastReceiver*)br->Probe(EIID_IBroadcastReceiver),
            filter, String(NULL), mHandler, (IIntent**)&intent));
    return NOERROR;
}

ECode CDreamManagerService::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::DUMP, TAG));

    pw->PrintStringln(String("DREAM MANAGER (dumpsys dreams)"));
    pw->Println();

    StringBuilder sb("mCurrentDreamToken=");
    sb += mCurrentDreamToken;
    pw->PrintStringln(sb.ToString());
    StringBuilder sb1("mCurrentDreamName=");
    sb1 += mCurrentDreamName;
    pw->PrintStringln(sb1.ToString());
    StringBuilder sb2("mCurrentDreamUserId=");
    sb2 += mCurrentDreamUserId;
    pw->PrintStringln(sb2.ToString());
    StringBuilder sb3("mCurrentDreamIsTest=");
    sb3 += mCurrentDreamIsTest;
    pw->PrintStringln(sb3.ToString());
    pw->Println();

//TODO:
//     DumpUtils.dumpAsync(mHandler, new DumpUtils.Dump() {
//         @Override
//         public void dump(PrintWriter pw) {
//             mController.dump(pw);
//         }
//     }, pw, 200);
    return NOERROR;
}

ECode CDreamManagerService::GetDreamComponents(
    /* [out,callee] */ ArrayOf<IComponentName*>** result)
{
    FAIL_RETURN(CheckPermission(Elastos::Droid::Manifest::permission::READ_DREAM_STATE));

    const Int32 userId = UserHandle::GetCallingUserId();
    const Int64 ident = Binder::ClearCallingIdentity();
//     try {
        *result = GetDreamComponentsForUser(userId);
        REFCOUNT_ADD(*result);
//     } finally {
        Binder::RestoreCallingIdentity(ident);
//     }
    return NOERROR;
}

ECode CDreamManagerService::SetDreamComponents(
    /* [in] */ ArrayOf<IComponentName*>* componentNames)
{
    FAIL_RETURN(CheckPermission(Elastos::Droid::Manifest::permission::WRITE_DREAM_STATE));

    const Int32 userId = UserHandle::GetCallingUserId();
    const Int64 ident = Binder::ClearCallingIdentity();
//     try {
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    Boolean result;
    FAIL_RETURN(settingsSecure->PutStringForUser(resolver,
            ISettingsSecure::SCREENSAVER_COMPONENTS,
            ComponentsToString(componentNames),
            userId, &result));
//     } finally {
    Binder::RestoreCallingIdentity(ident);
//     }
    return NOERROR;
}

ECode CDreamManagerService::GetDefaultDreamComponent(
    /* [out] */ IComponentName** component)
{
    FAIL_RETURN(CheckPermission(Elastos::Droid::Manifest::permission::READ_DREAM_STATE));

    const Int32 userId = UserHandle::GetCallingUserId();
    const Int64 ident = Binder::ClearCallingIdentity();
//     try {
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    String name;
    FAIL_RETURN(settingsSecure->GetStringForUser(resolver,
            ISettingsSecure::SCREENSAVER_DEFAULT_COMPONENT,
            userId, &name));
    if(name.IsNull()) {
        *component = NULL;
    }
    else {
        AutoPtr<IComponentNameHelper> componentHelper;
        CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&componentHelper);
        componentHelper->UnflattenFromString(name, component);
    }
//     } finally {
    Binder::RestoreCallingIdentity(ident);
//     }
    return NOERROR;
}

ECode CDreamManagerService::IsDreaming(
    /* [out] */ Boolean* result)
{
    FAIL_RETURN(CheckPermission(Elastos::Droid::Manifest::permission::READ_DREAM_STATE));

    {
        AutoLock lock(mLock);
        *result =  mCurrentDreamToken != NULL && !mCurrentDreamIsTest;
    }
    return NOERROR;
}

ECode CDreamManagerService::Dream()
{
    FAIL_RETURN(CheckPermission(Elastos::Droid::Manifest::permission::WRITE_DREAM_STATE));

    const Int64 ident = Binder::ClearCallingIdentity();
//     try {
        // Ask the power manager to nap.  It will eventually call back into
        // startDream() if/when it is appropriate to start dreaming.
        // Because napping could cause the screen to turn off immediately if the dream
        // cannot be started, we keep one eye open and gently poke user activity.
    Int64 time = SystemClock::GetUptimeMillis();
    FAIL_RETURN(mPowerManager->UserActivity(time, TRUE /*noChangeLights*/));
    FAIL_RETURN(mPowerManager->Nap(time));
//     } finally {
    Binder::RestoreCallingIdentity(ident);
//     }
    return NOERROR;
}

ECode CDreamManagerService::TestDream(
    /* [in] */ IComponentName* dream)
{
    FAIL_RETURN(CheckPermission(Elastos::Droid::Manifest::permission::WRITE_DREAM_STATE));

    if (dream == NULL) {
//         throw new IllegalArgumentException("dream must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    const Int32 callingUserId = UserHandle::GetCallingUserId();
    AutoPtr<IActivityManagerHelper> amHelper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&amHelper);
    Int32 currentUserId;
    amHelper->GetCurrentUser(&currentUserId);
    if (callingUserId != currentUserId) {
        // This check is inherently prone to races but at least it's something.
        Slogger::W(TAG, "Aborted attempt to start a test dream while a different  user is active: callingUserId=%d, currentUserId=%d"
                , callingUserId, currentUserId);
        return NOERROR;
    }
    const Int64 ident = Binder::ClearCallingIdentity();
//     try {
    {
        AutoLock lock(mLock);
        StartDreamLocked(dream, TRUE /*isTest*/, callingUserId);
    }
//     } finally {
    Binder::RestoreCallingIdentity(ident);
//     }
    return NOERROR;
}

ECode CDreamManagerService::Awaken()
{
    FAIL_RETURN(CheckPermission(Elastos::Droid::Manifest::permission::WRITE_DREAM_STATE));

    const Int64 ident = Binder::ClearCallingIdentity();
//     try {
    // Treat an explicit request to awaken as user activity so that the
    // device doesn't immediately go to sleep if the timeout expired,
    // for example when being undocked.
    Int64 time = SystemClock::GetUptimeMillis();
    mPowerManager->UserActivity(time, FALSE /*noChangeLights*/);
    StopDream();
//     } finally {
    Binder::RestoreCallingIdentity(ident);
//     }
    return NOERROR;
}

ECode CDreamManagerService::FinishSelf(
    /* [in] */ IBinder* token)
{
    // Requires no permission, called by Dream from an arbitrary process.
    if (token == NULL) {
//         throw new IllegalArgumentException("token must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    const Int64 ident = Binder::ClearCallingIdentity();
//     try {
    if (DEBUG) {
        Slogger::D(TAG, "Dream finished: %p", token);
    }

    // Note that a dream finishing and self-terminating is not
    // itself considered user activity.  If the dream is ending because
    // the user interacted with the device then user activity will already
    // have been poked so the device will stay awake a bit longer.
    // If the dream is ending on its own for other reasons and no wake
    // locks are held and the user activity timeout has expired then the
    // device may simply go to sleep.
    {
        AutoLock lock(mLock);
        if (mCurrentDreamToken.Get() == token) {
            StopDreamLocked();
        }
    }
//     } finally {
    Binder::RestoreCallingIdentity(ident);
//     }
    return NOERROR;
}

ECode CDreamManagerService::StartDream()
{
    AutoPtr<IActivityManagerHelper> amHelper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&amHelper);
    Int32 userId;
    amHelper->GetCurrentUser(&userId);
    AutoPtr<IComponentName> dream = ChooseDreamForUser(userId);
    if (dream != NULL) {
        {
            AutoLock lock(mLock);
            StartDreamLocked(dream, FALSE /*isTest*/, userId);
        }
    }
    return NOERROR;
}

ECode CDreamManagerService::StopDream()
{
    AutoLock lock(mLock);
    StopDreamLocked();
    return NOERROR;
}

AutoPtr<IComponentName> CDreamManagerService::ChooseDreamForUser(
    /* [in] */ Int32 userId)
{
    AutoPtr< ArrayOf<IComponentName*> > dreams = GetDreamComponentsForUser(userId);
    return dreams != NULL && dreams->GetLength() != 0 ? (*dreams)[0] : NULL;
}

AutoPtr< ArrayOf<IComponentName*> > CDreamManagerService::GetDreamComponentsForUser(
    /* [in] */ Int32 userId)
{
    AutoPtr<IContentResolver> cResolver;
    mContext->GetContentResolver((IContentResolver**)&cResolver);
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    String names;
    settingsSecure->GetStringForUser(cResolver,
            ISettingsSecure::SCREENSAVER_COMPONENTS,
            userId, &names);

   AutoPtr<ArrayOf< IComponentName*> > components = ComponentsFromString(names);

    // first, ensure components point to valid services
    List<IComponentName*> validComponents;// = new ArrayList<ComponentName>();
    if (components != NULL) {
        for (Int32 i = 0; i < components->GetLength(); ++i) {
            AutoPtr<IComponentName> component = (*components)[i];
            if (ServiceExists(component)) {
                validComponents.PushBack(component);
            } else {
                Slogger::W(TAG, "Dream %p does not exist", component.Get());
            }
        }
    }

    // fallback to the default dream component if necessary
    if (validComponents.IsEmpty()) {
        AutoPtr<IComponentName> defaultDream;
        GetDefaultDreamComponent((IComponentName**)&defaultDream);
        if (defaultDream != NULL) {
            Slogger::W(TAG, "Falling back to default dream %p", defaultDream.Get());
            validComponents.PushBack(defaultDream);
        }
    }
    AutoPtr<ArrayOf<IComponentName*> > validComponentsArray = ArrayOf<IComponentName*>::Alloc(validComponents.GetSize());
    List<IComponentName*>::Iterator it;
    Int32 index = 0;
    for (it = validComponents.Begin(); it != validComponents.End(); ++it) {
        validComponentsArray->Set(index, *it);
        index ++;
    }
    return validComponentsArray;
}

Boolean CDreamManagerService::ServiceExists(
    /* [in] */ IComponentName* name)
{
//     try {
    AutoPtr<IPackageManager> pkgManager;
    if (FAILED(mContext->GetPackageManager((IPackageManager**)&pkgManager))) {
        return FALSE;
    }
    AutoPtr<IServiceInfo> serviceInfo;
    if (FAILED(pkgManager->GetServiceInfo(name, 0, (IServiceInfo**)&serviceInfo))) {
        return FALSE;
    }
    return name != NULL && serviceInfo != NULL;
//     } catch (NameNotFoundException e) {
//         return false;
//     }
}

void CDreamManagerService::StartDreamLocked(
        /* [in] */ IComponentName* name,
        /* [in] */ Boolean isTest,
        /* [in] */ Int32 userId)
{
    if (mCurrentDreamName.Get() == name
            && mCurrentDreamIsTest == isTest
            && mCurrentDreamUserId == userId) {
        return;
    }

    StopDreamLocked();

    if (DEBUG) Slogger::I(TAG, "Entering dreamland.");

    AutoPtr<IBinder> newToken;
    CBinder::New((IBinder**)&newToken);
    mCurrentDreamToken = newToken;
    mCurrentDreamName = name;
    mCurrentDreamIsTest = isTest;
    mCurrentDreamUserId = userId;

    AutoPtr<StartDreamRunnable> runnable = new StartDreamRunnable(this, newToken, name, isTest, userId);
    Boolean result;
    mHandler->Post(IRunnable::Probe(runnable), &result);
}

void CDreamManagerService::StopDreamLocked()
{
    if (mCurrentDreamToken != NULL) {
        if (DEBUG) Slogger::I(TAG, "Leaving dreamland.");

        CleanupDreamLocked();

        AutoPtr<StopDreamRunnable> runnable = new StopDreamRunnable(this);
        Boolean result;
        mHandler->Post(IRunnable::Probe(runnable), &result);
    }
}

void CDreamManagerService::CleanupDreamLocked()
{
    mCurrentDreamToken = NULL;
    mCurrentDreamName = NULL;
    mCurrentDreamIsTest = FALSE;
    mCurrentDreamUserId = 0;
}

ECode CDreamManagerService::CheckPermission(
    /* [in] */ const String& permission)
{
    Int32 value;
    FAIL_RETURN(mContext->CheckCallingOrSelfPermission(permission, &value));
    if (value != IPackageManager::PERMISSION_GRANTED) {
        // throw new SecurityException("Access denied to process: " + Binder.getCallingPid()
                // + ", must have permission " + permission);
        return E_SECURITY_EXCEPTION;
    }
    return NOERROR;
}

String CDreamManagerService::ComponentsToString(
    /* [in] */ ArrayOf<IComponentName*>* componentNames)
{
    StringBuilder names;
    if (componentNames != NULL) {
        for (Int32 i = 0; i < componentNames->GetLength(); ++i) {
            AutoPtr<IComponentName> componentName = (*componentNames)[i];
            if (names.GetLength() > 0) {
                names.AppendChar(',');
            }
            String componentString;
            componentName->FlattenToString(&componentString);
            names.AppendString(componentString);
        }
    }
    return names.ToString();
}

AutoPtr< ArrayOf<IComponentName*> > CDreamManagerService::ComponentsFromString(
    /* [in] */ const String& names)
{
    if (names.IsNull()) {
        return NULL;
    }
    AutoPtr< ArrayOf<String> > namesArray;
    StringUtils::Split(names, String(","), (ArrayOf<String>**)&namesArray);
    Int32 length = namesArray->GetLength();
    AutoPtr< ArrayOf<IComponentName*> > componentNames = ArrayOf<IComponentName*>::Alloc(length);
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<IComponentNameHelper> cnHelper;
        CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&cnHelper);
        cnHelper->UnflattenFromString((*namesArray)[i], (IComponentName**)&(*componentNames)[i]);
    }
    return componentNames;
}

} // namespace Dreams
} // namespace Server
} // namespace Droid
} // namespace Elastos
