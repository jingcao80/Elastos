//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/server/dreams/CDreamManagerService.h"
#include "elastos/droid/server/dreams/CDreamManagerBinderService.h"
#include "elastos/droid/server/FgThread.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/R.h"
#include "elastos/droid/Manifest.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.View.h>
#include <Elastos.Droid.Provider.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.IO.h>

using Elastos::Droid::R;
using Elastos::Droid::Manifest;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::CBinder;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::EIID_IPowerManagerInternal;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::IDisplay;
// using Elastos::Droid::View::IDisplayHelper;
// using Elastos::Droid::View::CDisplayHelper;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IComponentNameHelper;
using Elastos::Droid::Content::CComponentNameHelper;
using Elastos::Droid::Content::EIID_IBroadcastReceiver;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Server::FgThread;
using Elastos::Droid::Service::Dreams::IDreamService;
using Elastos::Droid::Service::Dreams::EIID_IIDreamManager;
using Elastos::Droid::Service::Dreams::EIID_IDreamManagerInternal;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::AutoLock;
using Elastos::Utility::IList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Dreams {

//============================================================================
// CDreamManagerService::SRBroadcastReceiver
//============================================================================
ECode CDreamManagerService::SRBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    AutoLock lock(mHost->mLock);
    mHost->StopDreamLocked(FALSE /*immediate*/);
    return NOERROR;
}

ECode CDreamManagerService::StartDreamRunnable::Run()
{
    mHost->mController->StartDream(mToken, mName, mIsTest, mCanDoze, mUserId);
    return NOERROR;
}

ECode CDreamManagerService::StopDreamRunnable::Run()
{
    mHost->mController->StopDream(mImmediate);
    return NOERROR;
}

ECode CDreamManagerService::SystemPropertiesChangedRunnable::Run()
{
    if (CDreamManagerService::DEBUG) Slogger::D(CDreamManagerService::TAG, "System properties changed");
    AutoLock lock(mHost->mLock);
    if (mHost->mCurrentDreamName != NULL && mHost->mCurrentDreamCanDoze) {
        AutoPtr<IComponentName> cn = mHost->GetDozeComponent();
        if (!Object::Equals(mHost->mCurrentDreamName, cn)) {
            // May have updated the doze component, wake up
            return mHost->mPowerManager->WakeUp(SystemClock::GetUptimeMillis());
        }
    }
    return NOERROR;
}

//============================================================================
// CDreamManagerService::BinderService
//============================================================================

CAR_INTERFACE_IMPL_2(CDreamManagerService::BinderService, Object, IIDreamManager, IBinder);

CDreamManagerService::BinderService::BinderService()
{}

ECode CDreamManagerService::BinderService::constructor(
    /* [in] */ ISystemService* dreamManagerService)
{
    mHost = (CDreamManagerService*)dreamManagerService;
    return NOERROR;
}

ECode CDreamManagerService::BinderService::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    Int32 perm;
    mHost->mContext->CheckCallingOrSelfPermission(Manifest::permission::DUMP, &perm);
    if (perm != IPackageManager::PERMISSION_GRANTED) {
        pw->Print(String("Permission Denial: can't dump DreamManager from from pid="));
        pw->Print(Binder::GetCallingPid());
        pw->Print(String(", uid="));
        pw->Println(Binder::GetCallingUid());
        return NOERROR;
    }

    Int64 ident = Binder::ClearCallingIdentity();
    mHost->DumpInternal(pw);
    Binder::RestoreCallingIdentity(ident);
    return NOERROR;
}

ECode CDreamManagerService::BinderService::GetDreamComponents(
    /* [out, callee] */ ArrayOf<IComponentName*>** results)
{
    VALIDATE_NOT_NULL(results)
    *results = NULL;

    FAIL_RETURN(mHost->CheckPermission(Manifest::permission::READ_DREAM_STATE))

    Int32 userId = UserHandle::GetCallingUserId();
    Int64 ident = Binder::ClearCallingIdentity();
    AutoPtr<ArrayOf<IComponentName*> > names = mHost->GetDreamComponentsForUser(userId);
    Binder::RestoreCallingIdentity(ident);

    *results = names;
    REFCOUNT_ADD(*results)
    return NOERROR;
}

ECode CDreamManagerService::BinderService::SetDreamComponents(
    /* [in] */ ArrayOf<IComponentName*>* componentNames)
{
    FAIL_RETURN(mHost->CheckPermission(Manifest::permission::WRITE_DREAM_STATE))

    Int32 userId = UserHandle::GetCallingUserId();
    Int64 ident = Binder::ClearCallingIdentity();

    ECode ec = mHost->SetDreamComponentsForUser(userId, componentNames);
    Binder::RestoreCallingIdentity(ident);
    return ec;
}

ECode CDreamManagerService::BinderService::GetDefaultDreamComponent(
    /* [out] */ IComponentName** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    FAIL_RETURN(mHost->CheckPermission(Manifest::permission::READ_DREAM_STATE))

    Int32 userId = UserHandle::GetCallingUserId();
    Int64 ident = Binder::ClearCallingIdentity();

    AutoPtr<IComponentName> cn = mHost->GetDefaultDreamComponentForUser(userId);
    Binder::RestoreCallingIdentity(ident);

    *result = cn;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CDreamManagerService::BinderService::IsDreaming(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    FAIL_RETURN(mHost->CheckPermission(Manifest::permission::READ_DREAM_STATE))

    Int64 ident = Binder::ClearCallingIdentity();
    *result = mHost->IsDreamingInternal();
    Binder::RestoreCallingIdentity(ident);
    return NOERROR;
}

ECode CDreamManagerService::BinderService::Dream()
{
    FAIL_RETURN(mHost->CheckPermission(Manifest::permission::WRITE_DREAM_STATE))

    Int64 ident = Binder::ClearCallingIdentity();

    ECode ec = mHost->RequestDreamInternal();
    Binder::RestoreCallingIdentity(ident);
    return ec;
}

ECode CDreamManagerService::BinderService::TestDream(
    /* [in] */ IComponentName* dream)
{
    if (dream == NULL) {
        Slogger::E("CDreamManagerService", "dream must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    FAIL_RETURN(mHost->CheckPermission(Manifest::permission::WRITE_DREAM_STATE))

    Int32 callingUserId = UserHandle::GetCallingUserId();
    Int32 currentUserId;

    AutoPtr<IActivityManagerHelper> amHelper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&amHelper);
    amHelper->GetCurrentUser(&currentUserId);

    if (callingUserId != currentUserId) {
        // This check is inherently prone to races but at least it's something.
        Slogger::W("CDreamManagerService", "Aborted attempt to start a test dream while a different "
            " user is active: callingUserId=%d, currentUserId=%d", callingUserId, currentUserId);
        return NOERROR;
    }

    Int64 ident = Binder::ClearCallingIdentity();
    ECode ec = mHost->TestDreamInternal(dream, callingUserId);
    Binder::RestoreCallingIdentity(ident);
    return ec;
}

ECode CDreamManagerService::BinderService::Awaken()
{
    FAIL_RETURN(mHost->CheckPermission(Manifest::permission::WRITE_DREAM_STATE))

    Int64 ident = Binder::ClearCallingIdentity();
    ECode ec = mHost->RequestAwakenInternal();
    Binder::RestoreCallingIdentity(ident);
    return ec;
}

ECode CDreamManagerService::BinderService::FinishSelf(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean immediate)
{
    // Requires no permission, called by Dream from an arbitrary process.
    if (token == NULL) {
        Slogger::E("CDreamManagerService", "token must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int64 ident = Binder::ClearCallingIdentity();

    ECode ec = mHost->FinishSelfInternal(token, immediate);
    Binder::RestoreCallingIdentity(ident);
    return ec;
}

ECode CDreamManagerService::BinderService::StartDozing(
    /* [in] */ IBinder* token,
    /* [in] */ Int32 screenState,
    /* [in] */ Int32 screenBrightness)
{
    // Requires no permission, called by Dream from an arbitrary process.
    if (token == NULL) {
        Slogger::E("CDreamManagerService", "token must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int64 ident = Binder::ClearCallingIdentity();
    ECode ec = mHost->StartDozingInternal(token, screenState, screenBrightness);
    Binder::RestoreCallingIdentity(ident);
    return ec;
}

ECode CDreamManagerService::BinderService::StopDozing(
    /* [in] */ IBinder* token)
{
    // Requires no permission, called by Dream from an arbitrary process.
    if (token == NULL) {
        Slogger::E("CDreamManagerService", "token must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int64 ident = Binder::ClearCallingIdentity();

    ECode ec = mHost->StopDozingInternal(token);
    Binder::RestoreCallingIdentity(ident);
    return ec;
}

ECode CDreamManagerService::BinderService::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = "CDreamManagerService::BinderService";
    return NOERROR;
}

//============================================================================
// CDreamManagerService
//============================================================================
CAR_INTERFACE_IMPL(CDreamManagerService::LocalService, Object, IDreamManagerInternal)

//@Override
ECode CDreamManagerService::LocalService::StartDream(
    /* [in] */ Boolean doze)
{
    mHost->StartDreamInternal(doze);
    return NOERROR;
}

//@Override
ECode CDreamManagerService::LocalService::StopDream(
    /* [in] */ Boolean immediate)
{
     mHost->StopDreamInternal(immediate);
    return NOERROR;
}

//@Override
ECode CDreamManagerService::LocalService::IsDreaming(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result =  mHost->IsDreamingInternal();
    return NOERROR;
}

ECode CDreamManagerService::LocalService::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = "CDreamManagerService::LocalService";
    return NOERROR;
}

CDreamManagerService::DreamControllerListener::DreamControllerListener(
    /* [in] */ CDreamManagerService* host)
    : mHost(host)
{}

// @Override
ECode CDreamManagerService::DreamControllerListener::OnDreamStopped(
    /* [in] */ IBinder* token)
{
    AutoLock lock(mHost->mLock);
    if (mHost->mCurrentDreamToken.Get() == token) {
        mHost->CleanupDreamLocked();
    }

    return NOERROR;
}

//============================================================================
// CDreamManagerService
//============================================================================
const Boolean CDreamManagerService::DEBUG = FALSE;
const String CDreamManagerService::TAG("CDreamManagerService");

CAR_OBJECT_IMPL(CDreamManagerService)

CDreamManagerService::CDreamManagerService()
    : mCurrentDreamUserId(0)
    , mCurrentDreamIsTest(FALSE)
    , mCurrentDreamCanDoze(FALSE)
    , mCurrentDreamIsDozing(FALSE)
    , mCurrentDreamIsWaking(FALSE)
    , mCurrentDreamDozeScreenState(IDisplay::STATE_UNKNOWN)
    , mCurrentDreamDozeScreenBrightness(IPowerManager::BRIGHTNESS_DEFAULT)
{}

ECode CDreamManagerService::constructor(
    /* [in] */ IContext* context)
{
    FAIL_RETURN(SystemService::constructor(context));

    mContext = context;
    AutoPtr<ILooper> looper;
    FgThread::Get()->GetLooper((ILooper**)&looper);
    mHandler = new DreamHandler(looper);
    mControllerListener = new DreamControllerListener(this);
    mController = new DreamController(context, IHandler::Probe(mHandler), mControllerListener);
    AutoPtr<IInterface> systemService;
    context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&systemService);
    mPowerManager = IPowerManager::Probe(systemService);
    mPowerManagerInternal = IPowerManagerInternal::Probe(GetLocalService(EIID_IPowerManagerInternal));
    mPowerManager->NewWakeLock(IPowerManager::DOZE_WAKE_LOCK, TAG, (IPowerManagerWakeLock**)&mDozeWakeLock);
    mSystemPropertiesChanged = new SystemPropertiesChangedRunnable(this);
    return NOERROR;
}

ECode CDreamManagerService::OnStart()
{
    AutoPtr<IBinder> binder;
    CDreamManagerBinderService::New(this, (IBinder**)&binder);
    PublishBinderService(IDreamService::DREAM_SERVICE, binder);

    AutoPtr<LocalService> local = new LocalService(this);
    PublishLocalService(EIID_IDreamManagerInternal, TO_IINTERFACE(local));
    return NOERROR;
}

ECode CDreamManagerService::OnBootPhase(
    /* [in] */ Int32 phase)
{
    if (phase == ISystemService::PHASE_THIRD_PARTY_APPS_CAN_START) {
        if (Build::IS_DEBUGGABLE) {
            AutoPtr<IRunnable> runnable = new SystemPropertiesChangedRunnable(this);
            AutoPtr<ISystemProperties> sysProp;
            CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
            sysProp->AddChangeCallback(runnable);
        }

        AutoPtr<SRBroadcastReceiver> br = new SRBroadcastReceiver(this);
        AutoPtr<IIntentFilter> filter;
        CIntentFilter::New(IIntent::ACTION_USER_SWITCHED, (IIntentFilter**)&filter);
        AutoPtr<IIntent> intent;
        FAIL_RETURN(mContext->RegisterReceiver((IBroadcastReceiver*)br.Get(),
                filter, String(NULL), mHandler, (IIntent**)&intent));
    }
    return NOERROR;
}

ECode CDreamManagerService::DumpInternal(
    /* [in] */ IPrintWriter* pw)
{
    pw->Println(String("DREAM MANAGER (dumpsys dreams)"));
    pw->Println();

    pw->Print(String("mCurrentDreamToken="));
    pw->Println(Object::ToString(mCurrentDreamToken));

    pw->Print(String("mCurrentDreamName="));
    pw->Println(Object::ToString(mCurrentDreamName));

    pw->Print(String("mCurrentDreamUserId="));
    pw->Println(mCurrentDreamUserId);

    pw->Print(String("mCurrentDreamIsTest="));
    pw->Println(mCurrentDreamIsTest);

    pw->Print(String("mCurrentDreamCanDoze="));
    pw->Println(mCurrentDreamCanDoze);

    pw->Print(String("mCurrentDreamIsDozing="));
    pw->Println(mCurrentDreamIsDozing);

    pw->Print(String("mCurrentDreamIsWaking="));
    pw->Println(mCurrentDreamIsWaking);

    // AutoPtr<IDisplayHelper> dsHelper;
    // // CDisplayHelper::AcquireSingleton((IDisplayHelper**)&dsHelper);
    // String stateStr;
    // dsHelper->StateToString(mCurrentDreamDozeScreenState, &stateStr);
    // pw->Print(String("mCurrentDreamDozeScreenState="));
    // pw->Println(stateStr);

    pw->Print(String("mCurrentDreamDozeScreenBrightness="));
    pw->Println(mCurrentDreamDozeScreenBrightness);

    pw->Print(String("GetDozeComponent()="));
    pw->Println(Object::ToString(GetDozeComponent()));

    pw->Println();

//TODO:
//     DumpUtils.dumpAsync(mHandler, new DumpUtils.Dump() {
//         //@Override
//         ECode CDreamManagerService::dump(PrintWriter pw) {
//             mController.dump(pw);
//         }
//     }, pw, 200);
    return NOERROR;
}

Boolean CDreamManagerService::IsDreamingInternal()
{
    AutoLock syncLock(mLock);
    Boolean bval = mCurrentDreamToken != NULL
        && !mCurrentDreamIsTest && !mCurrentDreamIsWaking;
    return bval;
}

ECode CDreamManagerService::RequestDreamInternal()
{
    // Ask the power manager to nap.  It will eventually call back into
    // startDream() if/when it is appropriate to start dreaming.
    // Because napping could cause the screen to turn off immediately if the dream
    // cannot be started, we keep one eye open and gently poke user activity.
    Int64 time = SystemClock::GetUptimeMillis();
    mPowerManager->UserActivity(time, TRUE /*noChangeLights*/);
    return mPowerManager->Nap(time);
}

ECode CDreamManagerService::RequestAwakenInternal()
{
    // Treat an explicit request to awaken as user activity so that the
    // device doesn't immediately go to sleep if the timeout expired,
    // for example when being undocked.
    Int64 time = SystemClock::GetUptimeMillis();
    mPowerManager->UserActivity(time, FALSE /*noChangeLights*/);
    return StopDreamInternal(FALSE /*immediate*/);
}

ECode CDreamManagerService::FinishSelfInternal(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean immediate)
{
    if (DEBUG) {
        Slogger::D(TAG, "Dream finished: %s, immediate=%d",
            TO_CSTR(token), immediate);
    }

    // Note that a dream finishing and self-terminating is not
    // itself considered user activity.  If the dream is ending because
    // the user interacted with the device then user activity will already
    // have been poked so the device will stay awake a bit longer.
    // If the dream is ending on its own for other reasons and no wake
    // locks are held and the user activity timeout has expired then the
    // device may simply go to sleep.
    AutoLock syncLock(mLock);
    if (mCurrentDreamToken.Get() == token) {
        return StopDreamLocked(immediate);
    }
    return NOERROR;
}

ECode CDreamManagerService::TestDreamInternal(
    /* [in] */ IComponentName* dream,
    /* [in] */ Int32 userId)
{
    AutoLock syncLock(mLock);
    return StartDreamLocked(dream, TRUE /*isTest*/, FALSE /*canDoze*/, userId);
}

ECode CDreamManagerService::StartDreamInternal(
    /* [in] */ Boolean doze)
{
    AutoPtr<IActivityManagerHelper> amHelper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&amHelper);
    Int32 userId;
    amHelper->GetCurrentUser(&userId);

    AutoPtr<IComponentName> dream = ChooseDreamForUser(doze, userId);
    if (dream != NULL) {
        AutoLock syncLock(mLock);
        StartDreamLocked(dream, FALSE /*isTest*/, doze, userId);
    }
    return NOERROR;
}

ECode CDreamManagerService::StopDreamInternal(
    /* [in] */ Boolean immediate)
{
    AutoLock syncLock(mLock);
    return StopDreamLocked(immediate);
}

ECode CDreamManagerService::StartDozingInternal(
    /* [in] */ IBinder* token,
    /* [in] */ Int32 screenState,
    /* [in] */ Int32 screenBrightness)
{
    if (DEBUG) {
        Slogger::D(TAG, "Dream requested to start dozing: %s, screenState=%d, screenBrightness=",
            TO_CSTR(token),  screenState, screenBrightness);
    }

    AutoLock syncLock(mLock);
    if (mCurrentDreamToken.Get() == token && mCurrentDreamCanDoze) {
        mCurrentDreamDozeScreenState = screenState;
        mCurrentDreamDozeScreenBrightness = screenBrightness;
        mPowerManagerInternal->SetDozeOverrideFromDreamManager(screenState, screenBrightness);
        if (!mCurrentDreamIsDozing) {
            mCurrentDreamIsDozing = TRUE;
            mDozeWakeLock->AcquireLock();
        }
    }
    return NOERROR;
}

ECode CDreamManagerService::StopDozingInternal(
    /* [in] */ IBinder* token)
{
    if (DEBUG) {
        Slogger::D(TAG, "Dream requested to stop dozing: %s", TO_CSTR(token));
    }

    AutoLock syncLock(mLock);
    if (mCurrentDreamToken.Get() == token && mCurrentDreamIsDozing) {
        mCurrentDreamIsDozing = FALSE;
        mDozeWakeLock->ReleaseLock();
        return mPowerManagerInternal->SetDozeOverrideFromDreamManager(
            IDisplay::STATE_UNKNOWN, IPowerManager::BRIGHTNESS_DEFAULT);
    }
    return NOERROR;
}

AutoPtr<IComponentName> CDreamManagerService::ChooseDreamForUser(
    /* [in] */ Boolean doze,
    /* [in] */ Int32 userId)
{
    if (doze) {
        AutoPtr<IComponentName> dozeComponent = GetDozeComponent(userId);
        return ValidateDream(dozeComponent) ? dozeComponent : NULL;
    }
    AutoPtr< ArrayOf<IComponentName*> > dreams = GetDreamComponentsForUser(userId);
    return dreams != NULL && dreams->GetLength() != 0 ? (*dreams)[0] : NULL;
}

Boolean CDreamManagerService::ValidateDream(
    /* [in] */ IComponentName* component)
{
    if (component == NULL) return FALSE;
    AutoPtr<IServiceInfo> serviceInfo = GetServiceInfo(component);
    if (serviceInfo == NULL) {
        Slogger::W(TAG, "Dream %s does not exist", TO_CSTR(component));
        return FALSE;
    }

    AutoPtr<IApplicationInfo> appInfo;
    IComponentInfo::Probe(serviceInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
    Int32 version;
    appInfo->GetTargetSdkVersion(&version);
    String perm;
    serviceInfo->GetPermission(&perm);

    if (version >= Build::VERSION_CODES::LOLLIPOP && !Manifest::permission::BIND_DREAM_SERVICE.Equals(perm)) {
        Slogger::W(TAG, "Dream %s is not available because its manifest is missing the %s"
            " permission on the dream service declaration.",
            TO_CSTR(component), Manifest::permission::BIND_DREAM_SERVICE.string());
        return FALSE;
    }
    return TRUE;
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
            ISettingsSecure::SCREENSAVER_COMPONENTS, userId, &names);

   AutoPtr<ArrayOf< IComponentName*> > components = ComponentsFromString(names);

    // first, ensure components point to valid services
    AutoPtr<IList> validComponents;//List<ComponentName>
    CArrayList::New((IList**)&validComponents);
    if (components != NULL) {
        for (Int32 i = 0; i < components->GetLength(); ++i) {
            AutoPtr<IComponentName> component = (*components)[i];
            if (ValidateDream(component)) {
                validComponents->Add(component);
            }
        }
    }

    // fallback to the default dream component if necessary
    Boolean res;
    if (validComponents->IsEmpty(&res), res) {
        AutoPtr<IComponentName> defaultDream = GetDefaultDreamComponentForUser(userId);
        if (defaultDream != NULL) {
            Slogger::W(TAG, "Falling back to default dream %p", defaultDream.Get());
            validComponents->Add(defaultDream);
        }
    }

    Int32 size;
    validComponents->GetSize(&size);
    AutoPtr<ArrayOf<IInterface*> > inArray = ArrayOf<IInterface*>::Alloc(size);

    AutoPtr<ArrayOf<IInterface*> > outArray;
    validComponents->ToArray(inArray, (ArrayOf<IInterface*>**)&outArray);

    AutoPtr<ArrayOf<IComponentName*> > validComponentsArray = ArrayOf<IComponentName*>::Alloc(outArray->GetLength());
    for(Int32 i = 0; i < outArray->GetLength(); i++) {
        validComponentsArray->Set(i, IComponentName::Probe((*outArray)[i]));
    }

    return validComponentsArray;
}

ECode CDreamManagerService::CDreamManagerService::SetDreamComponentsForUser(
    /* [in] */ Int32 userId,
    /* [in] */ ArrayOf<IComponentName*>* componentNames)
{
    AutoPtr<ISettingsSecure> ss;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&ss);
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    Boolean bval;
    return ss->PutStringForUser(cr, ISettingsSecure::SCREENSAVER_COMPONENTS,
        ComponentsToString(componentNames), userId, &bval);
}

AutoPtr<IComponentName> CDreamManagerService::GetDefaultDreamComponentForUser(
    /* [in] */ Int32 userId)
{
    AutoPtr<ISettingsSecure> ss;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&ss);
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    String name;
    ss->GetStringForUser(cr, ISettingsSecure::SCREENSAVER_DEFAULT_COMPONENT, userId, &name);
    AutoPtr<IComponentName> result;
    if (!name.IsNull()) {
        AutoPtr<IComponentNameHelper> helper;
        CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&helper);
        helper->UnflattenFromString(name, (IComponentName**)&result);
    }
    return result;
}

AutoPtr<IComponentName> CDreamManagerService::GetDozeComponent()
{
    AutoPtr<IActivityManagerHelper> amHelper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&amHelper);
    Int32 currentUserId;
    amHelper->GetCurrentUser(&currentUserId);

    return GetDozeComponent(currentUserId);
}

AutoPtr<IComponentName> CDreamManagerService::GetDozeComponent(
    /* [in] */ Int32 userId)
{
    // Read the component from a system property to facilitate debugging.
    // Note that for production devices, the dream should actually be declared in
    // a config.xml resource.
    String name;
    if (Build::IS_DEBUGGABLE) {
        AutoPtr<ISystemProperties> sysProp;
        CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
        sysProp->Get(String("debug.doze.component"), &name);
    }
    if (TextUtils::IsEmpty(name)) {
        // Read the component from a config.xml resource.
        // The value should be specified in a resource overlay for the product.
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        res->GetString(R::string::config_dozeComponent, &name);
    }

    AutoPtr<ISettingsSecure> ss;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&ss);
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    Int32 ival;
    ss->GetInt32ForUser(cr, ISettingsSecure::DOZE_ENABLED, 1, userId, &ival);
    Boolean enabled = ival != 0;
    if (TextUtils::IsEmpty(name) || !enabled) {
        return NULL;
    }
    AutoPtr<IComponentNameHelper> cnHelper;
    CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&cnHelper);
    AutoPtr<IComponentName> cn;
    cnHelper->UnflattenFromString(name, (IComponentName**)&cn);
    return cn;
}

AutoPtr<IServiceInfo> CDreamManagerService::GetServiceInfo(
    /* [in] */ IComponentName* name)
{
    AutoPtr<IPackageManager> pkgManager;
    if (FAILED(mContext->GetPackageManager((IPackageManager**)&pkgManager))) {
        return NULL;
    }
    AutoPtr<IServiceInfo> serviceInfo;
    if (FAILED(pkgManager->GetServiceInfo(name, 0, (IServiceInfo**)&serviceInfo))) {
        return NULL;
    }
    return serviceInfo;
}

ECode CDreamManagerService::StartDreamLocked(
    /* [in] */ IComponentName* name,
    /* [in] */ Boolean isTest,
    /* [in] */ Boolean canDoze,
    /* [in] */ Int32 userId)
{
    if (mCurrentDreamName.Get() == name
            && mCurrentDreamIsTest == isTest
            && mCurrentDreamCanDoze == canDoze
            && mCurrentDreamUserId == userId) {
        return NOERROR;
    }

    StopDreamLocked(TRUE /*immediate*/);

    if (DEBUG) Slogger::I(TAG, "Entering dreamland.");

    AutoPtr<IBinder> newToken;
    CBinder::New((IBinder**)&newToken);
    mCurrentDreamToken = newToken;
    mCurrentDreamName = name;
    mCurrentDreamIsTest = isTest;
    mCurrentDreamCanDoze = canDoze;
    mCurrentDreamUserId = userId;

    AutoPtr<StartDreamRunnable> runnable = new StartDreamRunnable(this, newToken, name, isTest, canDoze, userId);
    Boolean result;
    return mHandler->Post(IRunnable::Probe(runnable), &result);
}

ECode CDreamManagerService::StopDreamLocked(
    /* [in] */ Boolean immediate)
{
    if (mCurrentDreamToken != NULL) {
        if (immediate) {
            if (DEBUG) Slogger::I(TAG, "Leaving dreamland.");
            CleanupDreamLocked();
        }
        else if (mCurrentDreamIsWaking) {
            return NOERROR; // already waking
        }
        else {
            Slogger::I(TAG, "Gently waking up from dream.");
            mCurrentDreamIsWaking = TRUE;
        }

        AutoPtr<StopDreamRunnable> runnable = new StopDreamRunnable(this, immediate);
        Boolean result;
        mHandler->Post(IRunnable::Probe(runnable), &result);
    }
    return NOERROR;
}

ECode CDreamManagerService::CleanupDreamLocked()
{
    mCurrentDreamToken = NULL;
    mCurrentDreamName = NULL;
    mCurrentDreamIsTest = FALSE;
    mCurrentDreamCanDoze = FALSE;
    mCurrentDreamUserId = 0;
    mCurrentDreamIsWaking = FALSE;
    if (mCurrentDreamIsDozing) {
        mCurrentDreamIsDozing = FALSE;
        mDozeWakeLock->ReleaseLock();
    }
    mCurrentDreamDozeScreenState = IDisplay::STATE_UNKNOWN;
    mCurrentDreamDozeScreenBrightness = IPowerManager::BRIGHTNESS_DEFAULT;
    return NOERROR;
}

ECode CDreamManagerService::CheckPermission(
    /* [in] */ const String& permission)
{
    Int32 value;
    FAIL_RETURN(mContext->CheckCallingOrSelfPermission(permission, &value));
    if (value != IPackageManager::PERMISSION_GRANTED) {
        Slogger::E(TAG, "Access denied to process: %d, must have permission %s",
            Binder::GetCallingPid(), permission.string());
        return E_SECURITY_EXCEPTION;
    }
    return NOERROR;
}

String CDreamManagerService::ComponentsToString(
    /* [in] */ ArrayOf<IComponentName*>* componentNames)
{
    StringBuilder names;
    if (componentNames != NULL) {
        String componentString;
        for (Int32 i = 0; i < componentNames->GetLength(); ++i) {
            AutoPtr<IComponentName> componentName = (*componentNames)[i];
            if (names.GetLength() > 0) {
                names.AppendChar(',');
            }
            componentName->FlattenToString(&componentString);
            names.Append(componentString);
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

    AutoPtr<IComponentNameHelper> cnHelper;
    CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&cnHelper);
    AutoPtr< ArrayOf<String> > namesArray;
    StringUtils::Split(names, String(","), (ArrayOf<String>**)&namesArray);
    Int32 length = namesArray->GetLength();
    AutoPtr< ArrayOf<IComponentName*> > componentNames = ArrayOf<IComponentName*>::Alloc(length);
    for (Int32 i = 0; i < length; i++) {
        cnHelper->UnflattenFromString((*namesArray)[i], (IComponentName**)&(*componentNames)[i]);
    }
    return componentNames;
}

} // namespace Dreams
} // namespace Server
} // namespace Droid
} // namespace Elastos
