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

#ifndef __ELASTOS_DROID_JAVAPROXY_CAPPLICATIONTHREADNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CAPPLICATIONTHREADNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CApplicationThreadNative.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>
#define HASH_FOR_OS
#include "elastos/droid/ext/frameworkhash.h"
#include <jni.h>

using Elastos::Droid::App::IActivityOptions;
using Elastos::Droid::App::IApplicationThread;
using Elastos::Droid::App::IInstrumentationWatcher;
using Elastos::Droid::App::IIUiAutomationConnection;
using Elastos::Droid::App::IProfilerInfo;
using Elastos::Droid::App::IResultInfo;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentReceiver;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Pm::IProviderInfo;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Res::ICompatibilityInfo;
using Elastos::Droid::Internal::App::IIVoiceInteractor;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IDebugMemoryInfo;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Os::IPersistableBundle;

using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IList;
using Elastos::Utility::IMap;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CApplicationThreadNative)
    , public Object
    , public IApplicationThread
    , public IBinder
{
public:
    ~CApplicationThreadNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI SchedulePauseActivity(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean finished,
        /* [in] */ Boolean userLeaving,
        /* [in] */ Int32 configChanges,
        /* [in] */ Boolean dontReport);

    CARAPI ScheduleStopActivity(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean showWindow,
        /* [in] */ Int32 configChanges);

    CARAPI ScheduleWindowVisibility(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean showWindow);

    CARAPI ScheduleSleeping(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean sleeping);

    CARAPI ScheduleResumeActivity(
        /* [in] */ IBinder* token,
        /* [in] */ Int32 processState,
        /* [in] */ Boolean isForward,
        /* [in] */ IBundle* resumeArgs);

    CARAPI ScheduleSendResult(
        /* [in] */ IBinder* token,
        /* [in] */ IList* results);

    CARAPI ScheduleLaunchActivity(
        /* [in] */ IIntent* intent,
        /* [in] */ IBinder* token,
        /* [in] */ Int32 ident,
        /* [in] */ IActivityInfo* info,
        /* [in] */ IConfiguration* curConfig,
        /* [in] */ ICompatibilityInfo* compatInfo,
        /* [in] */ IIVoiceInteractor* voiceInteractor,
        /* [in] */ Int32 procState,
        /* [in] */ IBundle* state,
        /* [in] */ IPersistableBundle* persistentState,
        /* [in] */ IList* pendingResults,   //List<ResultInfo>
        /* [in] */ IList* pendingNewIntents, //List<Intent>
        /* [in] */ Boolean notResumed,
        /* [in] */ Boolean isForward,
        /* [in] */ IProfilerInfo* pi);

    CARAPI ScheduleRelaunchActivity(
        /* [in] */ IBinder* token,
        /* [in] */ IList* pendingResults,
        /* [in] */ IList* pendingNewIntents,
        /* [in] */ Int32 configChanges,
        /* [in] */ Boolean notResumed,
        /* [in] */ IConfiguration* config);

    CARAPI ScheduleNewIntent(
        /* [in] */ IList* intents,
        /* [in] */ IBinder* token);

    CARAPI ScheduleDestroyActivity(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean finished,
        /* [in] */ Int32 configChanges);

    CARAPI ScheduleReceiver(
        /* [in] */ IIntent* intent,
        /* [in] */ IActivityInfo* info,
        /* [in] */ ICompatibilityInfo* compatInfo,
        /* [in] */ Int32 resultCode,
        /* [in] */ const String& data,
        /* [in] */ IBundle* extras,
        /* [in] */ Boolean sync,
        /* [in] */ Int32 sendingUser,
        /* [in] */ Int32 processState);

    CARAPI ScheduleCreateBackupAgent(
        /* [in] */ IApplicationInfo* app,
        /* [in] */ ICompatibilityInfo* compatInfo,
        /* [in] */ Int32 backupMode);

    CARAPI ScheduleDestroyBackupAgent(
        /* [in] */ IApplicationInfo* app,
        /* [in] */ ICompatibilityInfo* compatInfo);

    CARAPI ScheduleCreateService(
        /* [in] */ IBinder* token,
        /* [in] */ IServiceInfo* info,
        /* [in] */ ICompatibilityInfo* compatInfo,
        /* [in] */ Int32 processState);

    CARAPI ScheduleBindService(
        /* [in] */ IBinder* token,
        /* [in] */ IIntent* intent,
        /* [in] */ Boolean rebind,
        /* [in] */ Int32 processState);

    CARAPI ScheduleUnbindService(
        /* [in] */ IBinder* token,
        /* [in] */ IIntent* intent);

    CARAPI ScheduleServiceArgs(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean taskRemoved,
        /* [in] */ Int32 startId,
        /* [in] */ Int32 flags,
        /* [in] */ IIntent* args);

    CARAPI ScheduleStopService(
        /* [in] */ IBinder* token);

    CARAPI BindApplication(
        /* [in] */ const String& processName,
        /* [in] */ IApplicationInfo* appInfo,
        /* [in] */ IList* providers,
        /* [in] */ IComponentName* instrumentationName,
        /* [in] */ IProfilerInfo* pi,
        /* [in] */ IBundle* instrumentationArgs,
        /* [in] */ IInstrumentationWatcher* instrumentationWatcher,
        /* [in] */ IIUiAutomationConnection* instrumentationUiConnection,
        /* [in] */ Int32 debugMode,
        /* [in] */ Boolean enableOpenGlTrace,
        /* [in] */ Boolean isRestrictedBackupMode,
        /* [in] */ Boolean persistent,
        /* [in] */ IConfiguration* config,
        /* [in] */ ICompatibilityInfo* compatInfo,
        /* [in] */ IMap* services,
        /* [in] */ IBundle* coreSettings);

    CARAPI ScheduleExit();

    CARAPI ScheduleSuicide();

    CARAPI ScheduleConfigurationChanged(
        /* [in] */ IConfiguration* config);

    CARAPI UpdateTimeZone();

    CARAPI ClearDnsCache();

    CARAPI SetHttpProxy(
        /* [in] */ const String& proxy,
        /* [in] */ const String& port,
        /* [in] */ const String& exclList,
        /* [in] */ IUri* pacFileUrl);

    CARAPI ProcessInBackground();

    CARAPI DumpService(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IBinder* servicetoken,
        /* [in] */ ArrayOf<String>* args);

    CARAPI DumpProvider(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IBinder* servicetoken,
        /* [in] */ ArrayOf<String>* args);

    CARAPI ScheduleRegisteredReceiver(
        /* [in] */ IIntentReceiver* receiver,
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 resultCode,
        /* [in] */ const String& data,
        /* [in] */ IBundle* extras,
        /* [in] */ Boolean ordered,
        /* [in] */ Boolean sticky,
        /* [in] */ Int32 sendingUser,
        /* [in] */ Int32 processState);

    CARAPI ScheduleLowMemory();

    CARAPI ScheduleActivityConfigurationChanged(
        /* [in] */ IBinder* token);

    CARAPI ProfilerControl(
        /* [in] */ Boolean start,
        /* [in] */ IProfilerInfo* profilerInfo,
        /* [in] */ Int32 profileType);

    CARAPI DumpHeap(
        /* [in] */ Boolean managed,
        /* [in] */ const String& path,
        /* [in] */ IParcelFileDescriptor* fd);

    CARAPI SetSchedulingGroup(
        /* [in] */ Int32 group);

    CARAPI DispatchPackageBroadcast(
        /* [in] */ Int32 cmd,
        /* [in] */ ArrayOf<String>* packages);

    CARAPI ScheduleCrash(
        /* [in] */ const String& msg);

    CARAPI DumpActivity(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IBinder* servicetoken,
        /* [in] */ const String& prefix,
        /* [in] */ ArrayOf<String>* args);

    CARAPI SetCoreSettings(
        /* [in] */ IBundle* coreSettings);

    CARAPI UpdatePackageCompatibilityInfo(
        /* [in] */ const String& pkg,
        /* [in] */ ICompatibilityInfo* info);

    CARAPI ScheduleTrimMemory(
        /* [in] */ Int32 level);

    CARAPI DumpMemInfo(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IDebugMemoryInfo* mem,
        /* [in] */ Boolean checkin,
        /* [in] */ Boolean dumpFullInfo,
        /* [in] */ Boolean dumpDalvik,
        /* [in] */ ArrayOf<String>* args);

    CARAPI DumpGfxInfo(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ ArrayOf<String>* args);

    CARAPI DumpDbInfo(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ ArrayOf<String>* args);

    CARAPI UnstableProviderDied(
        /* [in] */ IBinder* provider);

    CARAPI RequestAssistContextExtras(
        /* [in] */ IBinder* activityToken,
        /* [in] */ IBinder* requestToken,
        /* [in] */ Int32 requestType);

    CARAPI ScheduleTranslucentConversionComplete(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean drawComplete);

    CARAPI ScheduleOnNewActivityOptions(
        /* [in] */ IBinder* token,
        /* [in] */ IActivityOptions* options);

    CARAPI SetProcessState(
        /* [in] */ Int32 state);

    CARAPI ScheduleInstallProvider(
        /* [in] */ IProviderInfo* provider);

    CARAPI UpdateTimePrefs(
        /* [in] */ Boolean is24Hour);

    CARAPI ScheduleCancelVisibleBehind(
        /* [in] */ IBinder* token);

    CARAPI ScheduleBackgroundVisibleBehindChanged(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean visible);

    CARAPI ScheduleEnterAnimationComplete(
        /* [in] */ IBinder* token);

private:
    static const String TAG;

    JavaVM* mJVM;
    jobject mJInstance;

    static HashMap<AutoPtr<IBinder>, jobject> sTokenMap;
    static HashMap<AutoPtr<IBinder>, jobject> sServiceTokenMap;
};

} // JavaProxy
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_JAVAPROXY_CAPPLICATIONTHREADNATIVE_H__
