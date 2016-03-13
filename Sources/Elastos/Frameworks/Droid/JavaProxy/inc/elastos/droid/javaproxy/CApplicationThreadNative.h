
#ifndef __ELASTOS_DROID_JAVAPROXY_CAPPLICATIONTHREADNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CAPPLICATIONTHREADNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CApplicationThreadNative.h"
#include <jni.h>
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::App::IInstrumentationWatcher;
using Elastos::Droid::App::IResultInfo;
using Elastos::Utility::Etl::HashMap;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentReceiver;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Res::ICompatibilityInfo;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IDebugMemoryInfo;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::IO::IFileDescriptor;
using Elastos::Utility::IObjectStringMap;


_ETL_NAMESPACE_BEGIN

template<> struct Hash<IBinder*>
{
    size_t operator()(IBinder* x) const { return (size_t)x; }
};
_ETL_NAMESPACE_END


namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CApplicationThreadNative)
{
public:

    ~CApplicationThreadNative();

    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI SchedulePauseActivity(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean finished,
        /* [in] */ Boolean userLeaving,
        /* [in] */ Int32 configChanges);

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
        /* [in] */ Boolean isForward);

    CARAPI ScheduleSendResult(
        /* [in] */ IBinder* token,
        /* [in] */ IObjectContainer* results);

    CARAPI ScheduleLaunchActivity(
        /* [in] */ IIntent* intent,
        /* [in] */ IBinder* token,
        /* [in] */ Int32 ident,
        /* [in] */ IActivityInfo* info,
        /* [in] */ IConfiguration* curConfig,
        /* [in] */ ICompatibilityInfo* compatInfo,
        /* [in] */ IBundle* state,
        /* [in] */ IObjectContainer* pendingResults,
        /* [in] */ IObjectContainer* pendingNewIntents,
        /* [in] */ Boolean notResumed,
        /* [in] */ Boolean isForward,
        /* [in] */ const String& profileName,
        /* [in] */ IParcelFileDescriptor* profileFd,
        /* [in] */ Boolean autoStopProfiler);

    CARAPI ScheduleRelaunchActivity(
        /* [in] */ IBinder* token,
        /* [in] */ ArrayOf<IResultInfo*>* pendingResults,
        /* [in] */ ArrayOf<IIntent*>* pendingNewIntents,
        /* [in] */ Int32 configChanges,
        /* [in] */ Boolean notResumed,
        /* [in] */ IConfiguration* config);

    CARAPI ScheduleNewIntent(
        /* [in] */ IObjectContainer* intents,
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
        /* [in] */ Int32 sendingUser);

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
        /* [in] */ ICompatibilityInfo* compatInfo);

    CARAPI ScheduleBindService(
        /* [in] */ IBinder* token,
        /* [in] */ IIntent* intent,
        /* [in] */ Boolean rebind);

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
        /* [in] */ const String& packageName,
        /* [in] */ IApplicationInfo* appInfo,
        /* [in] */ IObjectContainer* providers,
        /* [in] */ IComponentName* instrumentationName,
        /* [in] */ const String& profileName,
        /* [in] */ IParcelFileDescriptor* profileFd,
        /* [in] */ Boolean autoStopProfiler,
        /* [in] */ IBundle* instrumentationArgs,
        /* [in] */ IInstrumentationWatcher* instrumentationWatcher,
        /* [in] */ Int32 debugMode,
        /* [in] */ Boolean openGlTrace,
        /* [in] */ Boolean restrictedBackupMode,
        /* [in] */ Boolean persistent,
        /* [in] */ IConfiguration* config,
        /* [in] */ ICompatibilityInfo* compatInfo,
        /* [in] */ IObjectStringMap* services,
        /* [in] */ IBundle* coreSettings);

    CARAPI ScheduleExit();

    CARAPI ScheduleSuicide();

    CARAPI RequestThumbnail(
        /* [in] */ IBinder* token);

    CARAPI ScheduleConfigurationChanged(
        /* [in] */ IConfiguration* config);

    CARAPI UpdateTimeZone();

    CARAPI ClearDnsCache();

    CARAPI SetHttpProxy(
        /* [in] */ const String& proxy,
        /* [in] */ const String& port,
        /* [in] */ const String& exclList);

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
        /* [in] */ Int32 sendingUser);

    CARAPI ScheduleLowMemory();

    CARAPI ScheduleActivityConfigurationChanged(
        /* [in] */ IBinder* token);

    CARAPI ProfilerControl(
        /* [in] */ Boolean start,
        /* [in] */ const String& path,
        /* [in] */ IParcelFileDescriptor* fd,
        /* [in] */ Int32 profileType);

    CARAPI DumpHeap(
        /* [in] */ Boolean managed,
        /* [in] */ const String& path,
        /* [in] */ IParcelFileDescriptor* fd);

    CARAPI SetSchedulingGroup(
        /* [in] */ Int32 group);

    CARAPI GetMemoryInfo(
        /* [in] */ IDebugMemoryInfo* outInfo);

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
        /* [in] */ Boolean checkin,
        /* [in] */ Boolean all,
        /* [in] */ ArrayOf<String>* args,
        /* [out] */ IDebugMemoryInfo** info);

    CARAPI DumpGfxInfo(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ ArrayOf<String>* args);

    CARAPI DumpDbInfo(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ ArrayOf<String>* args);

    CARAPI UnstableProviderDied(
        /* [in] */ IBinder* provider);

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
