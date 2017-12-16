
#include "Elastos.Droid.Os.h"
#include "elastos/droid/app/IApplicationThread.h"
#include "elastos/droid/os/AndroidParcelUtils.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Res::ICompatibilityInfo;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Internal::App::IIVoiceInteractor;
using Elastos::Droid::Os::AndroidParcelUtils;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IPersistableBundle;
using Elastos::Utility::IList;
using Elastos::Utility::IMap;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace App {

const Boolean IApplicationThreadStub::DEBUG = TRUE;
const String IApplicationThreadStub::TAG("IApplicationThreadStub");
const String IApplicationThreadStub::DESCRIPTOR("android.app.IApplicationThread");

const Int32 IApplicationThreadStub::TRANSACTION_schedulePauseActivity = android::IBinder::FIRST_CALL_TRANSACTION + 0;
const Int32 IApplicationThreadStub::TRANSACTION_scheduleStopActivity = android::IBinder::FIRST_CALL_TRANSACTION + 1;
const Int32 IApplicationThreadStub::TRANSACTION_scheduleWindowVisibility = android::IBinder::FIRST_CALL_TRANSACTION + 2;
const Int32 IApplicationThreadStub::TRANSACTION_scheduleResumeActivity = android::IBinder::FIRST_CALL_TRANSACTION + 3;
const Int32 IApplicationThreadStub::TRANSACTION_scheduleLaunchActivity = android::IBinder::FIRST_CALL_TRANSACTION + 5;
const Int32 IApplicationThreadStub::TRANSACTION_bindApplication = android::IBinder::FIRST_CALL_TRANSACTION + 11;
const Int32 IApplicationThreadStub::TRANSACTION_scheduleConfigurationChanged = android::IBinder::FIRST_CALL_TRANSACTION + 13;
const Int32 IApplicationThreadStub::TRANSACTION_scheduleRelaunchActivity = android::IBinder::FIRST_CALL_TRANSACTION + 24;
const Int32 IApplicationThreadStub::TRANSACTION_scheduleSleeping = android::IBinder::FIRST_CALL_TRANSACTION + 25;
const Int32 IApplicationThreadStub::TRANSACTION_scheduleTrimMemory = android::IBinder::FIRST_CALL_TRANSACTION + 40;
const Int32 IApplicationThreadStub::TRANSACTION_setProcessState = android::IBinder::FIRST_CALL_TRANSACTION + 48;
const Int32 IApplicationThreadStub::TRANSACTION_scheduleEnterAnimationComplete = android::IBinder::FIRST_CALL_TRANSACTION + 53;

IApplicationThreadStub::IApplicationThreadStub(
    /* [in] */ IApplicationThread* appThread)
    : mAppThread(appThread)
{}

void IApplicationThreadStub::onLastStrongRef(
    /* [in] */ const void* id)
{
    if (DEBUG) {
        Logger::D(TAG, "call onLastStrongRef");
    }
}

android::status_t IApplicationThreadStub::onTransact(
    /* [in] */ uint32_t code,
    /* [in] */ const android::Parcel& data,
    /* [out] */ android::Parcel* reply,
    /* [in] */ uint32_t flags)
{
    switch(code) {
        case TRANSACTION_schedulePauseActivity: {
            data.enforceInterface(android::String16(DESCRIPTOR.string()));
            AutoPtr<Elastos::Droid::Os::IBinder> token = AndroidParcelUtils::ReadStrongBinder(data);
            Boolean finished = AndroidParcelUtils::ReadInt32(data) != 0;
            Boolean userLeaving = AndroidParcelUtils::ReadInt32(data) != 0;
            Int32 configChanges = AndroidParcelUtils::ReadInt32(data);
            Boolean dontReport = AndroidParcelUtils::ReadInt32(data) != 0;
            mAppThread->SchedulePauseActivity(token, finished, userLeaving, configChanges, dontReport);
            return android::NO_ERROR;
        }

        case TRANSACTION_scheduleStopActivity: {
            data.enforceInterface(android::String16(DESCRIPTOR.string()));
            AutoPtr<Elastos::Droid::Os::IBinder> token = AndroidParcelUtils::ReadStrongBinder(data);
            Boolean showWindow = AndroidParcelUtils::ReadInt32(data) != 0;
            Int32 configChanges = AndroidParcelUtils::ReadInt32(data);
            mAppThread->ScheduleStopActivity(token, showWindow, configChanges);
            return android::NO_ERROR;
        }

        case TRANSACTION_scheduleWindowVisibility: {
            data.enforceInterface(android::String16(DESCRIPTOR.string()));
            AutoPtr<Elastos::Droid::Os::IBinder> token = AndroidParcelUtils::ReadStrongBinder(data);
            Boolean showWindow = AndroidParcelUtils::ReadInt32(data) != 0;
            mAppThread->ScheduleWindowVisibility(token, showWindow);
            return android::NO_ERROR;
        }

        case TRANSACTION_scheduleResumeActivity: {
            data.enforceInterface(android::String16(DESCRIPTOR.string()));
            AutoPtr<Elastos::Droid::Os::IBinder> token = AndroidParcelUtils::ReadStrongBinder(data);
            Int32 processState = AndroidParcelUtils::ReadInt32(data);
            Boolean isForward = AndroidParcelUtils::ReadInt32(data) != 0;
            AutoPtr<IBundle> resumeArgs;
            if (AndroidParcelUtils::ReadInt32(data) != 0) {
                resumeArgs = AndroidParcelUtils::ReadBundle(data);
            }
            mAppThread->ScheduleResumeActivity(token, processState, isForward, resumeArgs);
            return android::NO_ERROR;
        }

        case TRANSACTION_scheduleLaunchActivity: {
            data.enforceInterface(android::String16(DESCRIPTOR.string()));
            AutoPtr<IIntent> intent;
            if (AndroidParcelUtils::ReadInt32(data) != 0) {
                intent = AndroidParcelUtils::ReadIntent(data);
            }
            AutoPtr<Elastos::Droid::Os::IBinder> token = AndroidParcelUtils::ReadStrongBinder(data);
            Int32 ident = AndroidParcelUtils::ReadInt32(data);
            AutoPtr<IActivityInfo> info;
            if (AndroidParcelUtils::ReadInt32(data) != 0) {
                info = AndroidParcelUtils::ReadActivityInfo(data);
            }
            AutoPtr<IConfiguration> curConfig;
            if (AndroidParcelUtils::ReadInt32(data) != 0) {
                curConfig = AndroidParcelUtils::ReadConfiguration(data);
            }
            AutoPtr<IConfiguration> overrideConfig;
            if (AndroidParcelUtils::ReadInt32(data) != 0) {
                overrideConfig = AndroidParcelUtils::ReadConfiguration(data);
            }
            AutoPtr<ICompatibilityInfo> compatInfo;
            if (AndroidParcelUtils::ReadInt32(data) != 0) {
                compatInfo = AndroidParcelUtils::ReadCompatibilityInfo(data);
            }
            String referrer = AndroidParcelUtils::ReadString(data);
            AutoPtr<IIVoiceInteractor> voiceInteractor =
                    AndroidParcelUtils::ReadIVoiceInteractor(data);
            Int32 procState = AndroidParcelUtils::ReadInt32(data);
            AutoPtr<IBundle> state;
            if (AndroidParcelUtils::ReadInt32(data) != 0) {
                state = AndroidParcelUtils::ReadBundle(data);
            }
            AutoPtr<IPersistableBundle> persistentState;
            if (AndroidParcelUtils::ReadInt32(data) != 0) {
                persistentState = AndroidParcelUtils::ReadPersistableBundle(data);
            }
            AutoPtr<IList> pendingResults = AndroidParcelUtils::CreateResultInfoList(data);
            AutoPtr<IList> pendingNewIntents =  AndroidParcelUtils::CreateReferrerIntentList(data);
            Boolean notResumed = AndroidParcelUtils::ReadInt32(data) != 0;
            Boolean isForward = AndroidParcelUtils::ReadInt32(data) != 0;
            AutoPtr<IProfilerInfo> profilerInfo;
            if (AndroidParcelUtils::ReadInt32(data) != 0) {
                profilerInfo = AndroidParcelUtils::ReadProfilerInfo(data);
            }
            mAppThread->ScheduleLaunchActivity(intent, token, ident, info, curConfig, compatInfo,
                    voiceInteractor, procState, state, persistentState, pendingResults,
                    pendingNewIntents, notResumed, isForward, profilerInfo);
            return android::NO_ERROR;
        }

        case TRANSACTION_bindApplication: {
            data.enforceInterface(android::String16(DESCRIPTOR.string()));
            String packageName = AndroidParcelUtils::ReadString(data);
            AutoPtr<IApplicationInfo> info;
            if (AndroidParcelUtils::ReadInt32(data) != 0) {
                info = AndroidParcelUtils::ReadApplicationInfo(data);
            }
            AutoPtr<IList> providers = AndroidParcelUtils::CreateProviderInfoList(data);
            AutoPtr<IComponentName> testName;
            if (AndroidParcelUtils::ReadInt32(data) != 0) {
                testName = AndroidParcelUtils::ReadComponentName(data);
            }
            AutoPtr<IProfilerInfo> profilerInfo;
            if (AndroidParcelUtils::ReadInt32(data) != 0) {
                profilerInfo = AndroidParcelUtils::ReadProfilerInfo(data);
            }
            AutoPtr<IBundle> testArguments;
            if (AndroidParcelUtils::ReadInt32(data) != 0) {
                testArguments = AndroidParcelUtils::ReadBundle(data);
            }
            AutoPtr<IInstrumentationWatcher> testWatcher =
                    AndroidParcelUtils::ReadIInstrumentationWatcher(data);
            AutoPtr<IIUiAutomationConnection> uiAutomationConnection =
                    AndroidParcelUtils::ReadIUiAutomationConnection(data);
            Int32 debugMode = AndroidParcelUtils::ReadInt32(data);
            Boolean enableBinderTracking = AndroidParcelUtils::ReadInt32(data) != 0;
            Boolean trackAllocation = AndroidParcelUtils::ReadInt32(data) != 0;
            Boolean openGlTrace = FALSE;
            Boolean restrictedBackupMode = AndroidParcelUtils::ReadInt32(data) != 0;
            Boolean persistent = AndroidParcelUtils::ReadInt32(data) != 0;
            AutoPtr<IConfiguration> config;
            if (AndroidParcelUtils::ReadInt32(data) != 0) {
                config = AndroidParcelUtils::ReadConfiguration(data);
            }
            AutoPtr<ICompatibilityInfo> compatInfo;
            if (AndroidParcelUtils::ReadInt32(data) != 0) {
                compatInfo = AndroidParcelUtils::ReadCompatibilityInfo(data);
            }
            AutoPtr<IMap> services = AndroidParcelUtils::ReadHashMap(data);
            AutoPtr<IBundle> coreSettings;
            if (AndroidParcelUtils::ReadInt32(data) != 0) {
                coreSettings = AndroidParcelUtils::ReadBundle(data);
            }
            String buildSerial = AndroidParcelUtils::ReadString(data);

            mAppThread->BindApplication(packageName, info, providers, testName,
                    profilerInfo, testArguments, testWatcher, uiAutomationConnection,
                    debugMode, openGlTrace, restrictedBackupMode, persistent,
                    config, compatInfo, services, coreSettings);
            return android::NO_ERROR;
        }

        case TRANSACTION_scheduleConfigurationChanged: {
            data.enforceInterface(android::String16(DESCRIPTOR.string()));
            AutoPtr<IConfiguration> config;
            if (AndroidParcelUtils::ReadInt32(data) != 0) {
                config = AndroidParcelUtils::ReadConfiguration(data);
            }
            mAppThread->ScheduleConfigurationChanged(config);
            return android::NO_ERROR;
        }

        case TRANSACTION_scheduleRelaunchActivity: {
            data.enforceInterface(android::String16(DESCRIPTOR.string()));
            AutoPtr<Elastos::Droid::Os::IBinder> token = AndroidParcelUtils::ReadStrongBinder(data);
            AutoPtr<IList> pendingResults = AndroidParcelUtils::CreateResultInfoList(data);
            AutoPtr<IList> pendingNewIntents = AndroidParcelUtils::CreateReferrerIntentList(data);
            Int32 configChanges = AndroidParcelUtils::ReadInt32(data);
            Boolean notResumed = AndroidParcelUtils::ReadInt32(data) != 0;
            AutoPtr<IConfiguration> config;
            if (AndroidParcelUtils::ReadInt32(data) != 0) {
                config = AndroidParcelUtils::ReadConfiguration(data);
            }
            AutoPtr<IConfiguration> overrideConfig;
            if (AndroidParcelUtils::ReadInt32(data) != 0) {
                overrideConfig = AndroidParcelUtils::ReadConfiguration(data);
            }
            Boolean preserveWindow = AndroidParcelUtils::ReadInt32(data) != 0;
            mAppThread->ScheduleRelaunchActivity(token, pendingResults, pendingNewIntents, configChanges, notResumed, config);
            return android::NO_ERROR;
        }

        case TRANSACTION_scheduleSleeping: {
            data.enforceInterface(android::String16(DESCRIPTOR.string()));
            AutoPtr<Elastos::Droid::Os::IBinder> token = AndroidParcelUtils::ReadStrongBinder(data);
            Boolean sleeping = AndroidParcelUtils::ReadInt32(data) != 0;
            mAppThread->ScheduleSleeping(token, sleeping);
            return android::NO_ERROR;
        }

        case TRANSACTION_scheduleTrimMemory: {
            data.enforceInterface(android::String16(DESCRIPTOR.string()));
            Int32 level = AndroidParcelUtils::ReadInt32(data);
            mAppThread->ScheduleTrimMemory(level);
            return android::NO_ERROR;
        }

        case TRANSACTION_setProcessState: {
            data.enforceInterface(android::String16(DESCRIPTOR.string()));
            Int32 state = AndroidParcelUtils::ReadInt32(data);
            mAppThread->SetProcessState(state);
            return android::NO_ERROR;
        }

        case TRANSACTION_scheduleEnterAnimationComplete: {
            data.enforceInterface(android::String16(DESCRIPTOR.string()));
            AutoPtr<Elastos::Droid::Os::IBinder> token = AndroidParcelUtils::ReadStrongBinder(data);
            mAppThread->ScheduleEnterAnimationComplete(token);
            return android::NO_ERROR;
        }

        default:
            ALOGD("==== IApplicationThreadStub::onTransact, code: %d ====", code);
            assert(0);
    }
    return android::BBinder::onTransact(code, data, reply, flags);
}

} // namespace App
} // namespace Droid
} // namespace Elastos
