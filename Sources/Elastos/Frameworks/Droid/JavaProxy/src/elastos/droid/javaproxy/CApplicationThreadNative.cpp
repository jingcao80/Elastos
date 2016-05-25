
#include "elastos/droid/javaproxy/CApplicationThreadNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
// #include "Elastos.Droid.Net.h"
// #include "Elastos.Droid.Os.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::EIID_IApplicationThread;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CApplicationThreadNative::TAG("CApplicationThreadNative");

HashMap<AutoPtr<IBinder>, jobject> CApplicationThreadNative::sTokenMap;
HashMap<AutoPtr<IBinder>, jobject> CApplicationThreadNative::sServiceTokenMap;

CAR_INTERFACE_IMPL_2(CApplicationThreadNative, Object, IApplicationThread, IBinder)

CAR_OBJECT_IMPL(CApplicationThreadNative)

CApplicationThreadNative::~CApplicationThreadNative(){
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CApplicationThreadNative::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CApplicationThreadNative::SchedulePauseActivity(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean finished,
    /* [in] */ Boolean userLeaving,
    /* [in] */ Int32 configChanges,
    /* [in] */ Boolean dontReport)
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::SchedulePauseActivity()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jToken = NULL;
    if (token != NULL) {
        HashMap<AutoPtr<IBinder>, jobject>::Iterator it = sTokenMap.Find(token);
        if (it != sTokenMap.End()) {
            jToken = it->mSecond;
        }
        else {
            LOGGERE(TAG, "CApplicationThreadNative::SchedulePauseActivity() cannot find token!");
        }
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IApplicationThread", __LINE__);

    jmethodID m = env->GetMethodID(c, "schedulePauseActivity", "(Landroid/os/IBinder;ZZIZ)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: schedulePauseActivity", __LINE__);

    env->CallVoidMethod(mJInstance, m, jToken, finished, userLeaving, configChanges, dontReport);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod:", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CApplicationThreadNative::SchedulePauseActivity()");
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleLaunchActivity(
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
    /* [in] */ IProfilerInfo* pi)
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::ScheduleLaunchActivity()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jIntent = NULL;
    jobject jToken = NULL;
    jobject jInfo = NULL;
    jobject jCurConfig = NULL;
    jobject jCompatInfo = NULL;
    jobject jVoiceInteractor = NULL;
    jobject jState = NULL;
    jobject jPersistentState = NULL;
    jobject jPendingResults = NULL;
    jobject jPendingNewIntents = NULL;
    jobject jPi = NULL;

    if (intent != NULL) {
        jIntent = Util::ToJavaIntent(env, intent);
        if (jIntent == NULL) {
            LOGGERE(TAG, "ScheduleLaunchActivity: jIntent is NULL!");
        }
    }

    if (token != NULL) {
        HashMap<AutoPtr<IBinder>, jobject>::Iterator it = sTokenMap.Find(token);
        if (it != sTokenMap.End()) {
            jToken = it->mSecond;
        }
        else {
            jclass c = env->FindClass("android/view/ElApplicationTokenProxy");
            Util::CheckErrorAndLog(env, TAG, "FindClass: ElApplicationTokenProxy", __LINE__);

            jmethodID m = env->GetMethodID(c, "<init>", "(J)V");
            Util::CheckErrorAndLog(env, TAG, "GetMethodID: ElApplicationTokenProxy", __LINE__);

            jobject tempToken = env->NewObject(c, m, (jlong)token);
            Util::CheckErrorAndLog(env, TAG, "NewObject: ElApplicationTokenProxy", __LINE__);
            token->AddRef();

            env->DeleteLocalRef(c);

            jToken = env->NewGlobalRef(tempToken);
            env->DeleteLocalRef(tempToken);

            sTokenMap[token] = jToken;
        }
    }

    if (info != NULL) {
        jInfo = Util::ToJavaActivityInfo(env, info);
        if (jInfo == NULL) {
            LOGGERE(TAG, "ScheduleLaunchActivity: jInfo is NULL!");
        }
    }

    if (curConfig != NULL) {
        jCurConfig = Util::ToJavaConfiguration(env, curConfig);
        if (jCurConfig == NULL) {
            LOGGERE(TAG, "ScheduleLaunchActivity: jCurConfig is NULL!");
        }
    }

    if (compatInfo != NULL) {
        jCompatInfo = Util::ToJavaCompatibilityInfo(env, compatInfo);
    }

    if (voiceInteractor != NULL) {
        jclass c = env->FindClass("android/app/ElVoiceInteractor");
        Util::CheckErrorAndLog(env, "ScheduleLaunchActivity", "FindClass: ElVoiceInteractor %d", __LINE__);

        jmethodID m = env->GetMethodID(c, "<init>", "(J)V");
        Util::CheckErrorAndLog(env, "ScheduleLaunchActivity", "GetMethodID: ElVoiceInteractor %d", __LINE__);

        jVoiceInteractor = env->NewObject(c, m, (jlong)voiceInteractor);
        Util::CheckErrorAndLog(env, "ScheduleLaunchActivity", "NewObject: ElVoiceInteractor %d", __LINE__);

        voiceInteractor->AddRef();
        env->DeleteLocalRef(c);
    }

    if (state != NULL) {
        jState = Util::ToJavaBundle(env, state);
        if (jState == NULL) {
            LOGGERE(TAG, "ScheduleLaunchActivity: jState is NULL!");
        }
    }

    if (persistentState != NULL) {
        jPersistentState = Util::ToJavaPersistableBundle(env, persistentState);
    }

    if (pendingResults != NULL) {
        jclass c = env->FindClass("java/util/ArrayList");
        Util::CheckErrorAndLog(env, "ScheduleLaunchActivity", "Fail FindClass: ArrayList", __LINE__);
        jmethodID m = env->GetMethodID(c,"<init>","()V");
        Util::CheckErrorAndLog(env, "ScheduleLaunchActivity", "Fail GetMethodID: ArrayList", __LINE__);
        jPendingResults = env->NewObject(c, m);
        Util::CheckErrorAndLog(env, "ScheduleLaunchActivity", "Fail NewObject: ArrayList", __LINE__);
        m = env->GetMethodID(c, "add", "(Ljava/lang/Object;)Z");
        Util::CheckErrorAndLog(env, "ScheduleLaunchActivity", "Fail GetMethodID: add", __LINE__);

        Int32 size;
        pendingResults->GetSize(&size);
        for (Int32 i = 0; i < size; ++i) {
            AutoPtr<IInterface> obj;
            pendingResults->Get(i, (IInterface**)&obj);
            jobject jpendingResult = Util::ToJavaResultInfo(env, IResultInfo::Probe(obj));
            env->CallBooleanMethod(jPendingResults, m, jpendingResult);
            env->DeleteLocalRef(jpendingResult);
        }

        env->DeleteLocalRef(c);
    }

    if (pendingNewIntents != NULL) {
        jclass c = env->FindClass("java/util/ArrayList");
        Util::CheckErrorAndLog(env, "ScheduleLaunchActivity", "Fail FindClass: ArrayList", __LINE__);
        jmethodID m = env->GetMethodID(c,"<init>","()V");
        Util::CheckErrorAndLog(env, "ScheduleLaunchActivity", "Fail GetMethodID: ArrayList", __LINE__);
        jPendingNewIntents = env->NewObject(c, m);
        Util::CheckErrorAndLog(env, "ScheduleLaunchActivity", "Fail NewObject: ArrayList", __LINE__);
        m = env->GetMethodID(c, "add", "(Ljava/lang/Object;)Z");
        Util::CheckErrorAndLog(env, "ScheduleLaunchActivity", "Fail GetMethodID: add", __LINE__);

        Int32 size;
        pendingNewIntents->GetSize(&size);
        for (Int32 i = 0; i < size; ++i) {
            AutoPtr<IInterface> obj;
            pendingNewIntents->Get(i, (IInterface**)&obj);
            jobject jpendingNewIntent = Util::ToJavaIntent(env, IIntent::Probe(obj));
            env->CallBooleanMethod(jPendingNewIntents, m, jpendingNewIntent);
            env->DeleteLocalRef(jpendingNewIntent);
        }

        env->DeleteLocalRef(c);
    }

    if (pi != NULL) {
        jPi = Util::ToJavaProfilerInfo(env, pi);
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IApplicationThread", __LINE__);

    jmethodID m = env->GetMethodID(c, "scheduleLaunchActivity",
        "(Landroid/content/Intent;"\
         "Landroid/os/IBinder;"\
         "ILandroid/content/pm/ActivityInfo;"\
         "Landroid/content/res/Configuration;"\
         "Landroid/content/res/CompatibilityInfo;"\
         "Lcom/android/internal/app/IVoiceInteractor;"\
         "ILandroid/os/Bundle;"\
         "Landroid/os/PersistableBundle;"\
         "Ljava/util/List;"\
         "Ljava/util/List;"\
         "ZZLandroid/app/ProfilerInfo;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: scheduleLaunchActivity", __LINE__);

    env->CallVoidMethod(mJInstance, m, jIntent, jToken, (jint)ident, jInfo, jCurConfig,
        jCompatInfo, jVoiceInteractor, (jint)procState, jState, jPersistentState, jPendingResults,
        jPendingNewIntents, (jboolean)notResumed, (jboolean)isForward, jPi);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: scheduleLaunchActivity", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jIntent);
    env->DeleteLocalRef(jInfo);
    env->DeleteLocalRef(jCurConfig);
    env->DeleteLocalRef(jCompatInfo);
    env->DeleteLocalRef(jVoiceInteractor);
    env->DeleteLocalRef(jState);
    env->DeleteLocalRef(jPersistentState);
    env->DeleteLocalRef(jPendingResults);
    env->DeleteLocalRef(jPendingNewIntents);
    env->DeleteLocalRef(jPi);

    // LOGGERD(TAG, "- CApplicationThreadNative::ScheduleLaunchActivity()");
    return NOERROR;
}

ECode CApplicationThreadNative::BindApplication(
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
    /* [in] */ IBundle* coreSettings)
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::BindApplication()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jstring jprocessName = NULL;
    jobject jAppInfo = NULL;
    jobject jProviders = NULL;
    jobject jInstrumentationName = NULL;
    jobject jPi = NULL;
    jobject jInstrumentationArgs = NULL;
    jobject jInstrumentationWatcher = NULL;
    jobject jInstrumentationUiConnection = NULL;
    jobject jConfig = NULL;
    jobject jCompatInfo = NULL;
    jobject jServices = NULL;
    jobject jCoreSettings = NULL;

    if (!processName.IsNull()) {
        jprocessName = Util::ToJavaString(env, processName);
    }

    if (appInfo != NULL) {
        jAppInfo = Util::ToJavaApplicationInfo(env, appInfo);
    }

    if (providers != NULL) {
        jclass c = env->FindClass("java/util/ArrayList");
        Util::CheckErrorAndLog(env, "BindApplication", "Fail FindClass: ArrayList", __LINE__);
        jmethodID m = env->GetMethodID(c,"<init>","()V");
        Util::CheckErrorAndLog(env, "BindApplication", "Fail GetMethodID: ArrayList", __LINE__);
        jProviders = env->NewObject(c, m);
        Util::CheckErrorAndLog(env, "BindApplication", "Fail NewObject: ArrayList", __LINE__);
        m = env->GetMethodID(c, "add", "(Ljava/lang/Object;)Z");
        Util::CheckErrorAndLog(env, "BindApplication", "Fail GetMethodID: add", __LINE__);

        Int32 size;
        providers->GetSize(&size);
        for (Int32 i = 0; i < size; ++i) {
            AutoPtr<IInterface> obj;
            providers->Get(i, (IInterface**)&obj);
            jobject jproviderInfo = Util::ToJavaProviderInfo(env, IProviderInfo::Probe(obj));
            env->CallBooleanMethod(jProviders, m, jproviderInfo);
            env->DeleteLocalRef(jproviderInfo);
        }

        env->DeleteLocalRef(c);
    }

    if (instrumentationName != NULL) {
        jInstrumentationName = Util::ToJavaComponentName(env, instrumentationName);
    }

    if (pi != NULL) {
        jPi = Util::ToJavaProfilerInfo(env, pi);
    }

    if (instrumentationArgs != NULL) {
        jInstrumentationArgs = Util::ToJavaBundle(env, instrumentationArgs);
    }

    if (instrumentationWatcher != NULL) {
        jclass c = env->FindClass("android/app/ElInstrumentationWatcherProxy");
        Util::CheckErrorAndLog(env, "BindApplication", "FindClass: ElInstrumentationWatcherProxy %d", __LINE__);

        jmethodID m = env->GetMethodID(c, "<init>", "(J)V");
        Util::CheckErrorAndLog(env, "BindApplication", "GetMethodID: ElInstrumentationWatcherProxy %d", __LINE__);

        jInstrumentationWatcher = env->NewObject(c, m, (jlong)instrumentationWatcher);
        Util::CheckErrorAndLog(env, "BindApplication", "NewObject: ElInstrumentationWatcherProxy %d", __LINE__);

        instrumentationWatcher->AddRef();
        env->DeleteLocalRef(c);
    }

    if (instrumentationUiConnection != NULL) {
        jclass c = env->FindClass("android/app/ElUiAutomationConnection");
        Util::CheckErrorAndLog(env, "BindApplication", "FindClass: ElUiAutomationConnection %d", __LINE__);

        jmethodID m = env->GetMethodID(c, "<init>", "(J)V");
        Util::CheckErrorAndLog(env, "BindApplication", "GetMethodID: ElUiAutomationConnection %d", __LINE__);

        jInstrumentationUiConnection = env->NewObject(c, m, (jlong)instrumentationUiConnection);
        Util::CheckErrorAndLog(env, "BindApplication", "NewObject: ElUiAutomationConnection %d", __LINE__);

        instrumentationUiConnection->AddRef();
        env->DeleteLocalRef(c);
    }

    if (config != NULL) {
        jConfig = Util::ToJavaConfiguration(env, config);
    }

    if (compatInfo != NULL) {
        jCompatInfo = Util::ToJavaCompatibilityInfo(env, compatInfo);
    }

    if (services != NULL) {
        // Int32 count = 0;
        // services->GetSize(&count);
        // jclass c = env->FindClass("java/util/HashMap");
        // Util::CheckErrorAndLog(env, "BindApplication", "FindClass: HashMap %d", __LINE__);

        // jmethodID m = env->GetMethodID(c,"<init>","()V");
        // Util::CheckErrorAndLog(env, "BindApplication", "GetMethodID: HashMap %d", __LINE__);

        // jServices = env->NewObject(c, m);
        // Util::CheckErrorAndLog(env, "BindApplication", "NewObject: HashMap %d", __LINE__);

        // m = env->GetMethodID(c, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
        // Util::CheckErrorAndLog(env, "BindApplication", "GetMethodID: put %d", __LINE__);

        // if (count > 0) {
        //     AutoPtr<ArrayOf<String> > keys;
        //     services->GetKeys((ArrayOf<String>**)&keys);
        //     if (keys != NULL) {
        //         Int32 keyCount = keys->GetLength();
        //         for (Int32 i = 0; i < keyCount; i++) {
        //             String key = (*keys)[i];
        //             AutoPtr<IInterface> value;
        //             services->Get(key, (IInterface**)&value);
        //             AutoPtr<IBinder> service = IBinder::Probe(value);
        //             if (service != NULL) {
        //                 jstring jkey = Util::ToJavaString(env, key);
        //                 LOGGERE(TAG, key + String(": //TODO: serivice BindApplication()"));
        //             } else {
        //                 LOGGERE(TAG, key + String(": BindApplication: service is NULL!"));
        //             }
        //         }
        //     }
        // }

        // env->DeleteLocalRef(c);
    }

    if (coreSettings != NULL) {
        jCoreSettings = Util::ToJavaBundle(env, coreSettings);
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, "BindApplication", "Fail FindClass: IApplicationThread", __LINE__);

    jmethodID m = env->GetMethodID(c, "bindApplication",
        "(Ljava/lang/String;"\
         "Landroid/content/pm/ApplicationInfo;"\
         "Ljava/util/List;"\
         "Landroid/content/ComponentName;"\
         "Landroid/app/ProfilerInfo;"\
         "Landroid/os/Bundle;"\
         "Landroid/app/IInstrumentationWatcher;"\
         "Landroid/app/IUiAutomationConnection;"\
         "IZZZLandroid/content/res/Configuration;"\
         "Landroid/content/res/CompatibilityInfo;"\
         "Ljava/util/Map;"\
         "Landroid/os/Bundle;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: bindApplication", __LINE__);

    env->CallVoidMethod(mJInstance, m, jprocessName, jAppInfo, jProviders, jInstrumentationName, jPi,
        jInstrumentationArgs, jInstrumentationWatcher, jInstrumentationUiConnection, (jint)debugMode,
        (jboolean)enableOpenGlTrace, (jboolean)isRestrictedBackupMode, (jboolean)persistent, jConfig,
        jCompatInfo, jServices, jCoreSettings);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: bindApplication", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jprocessName);
    env->DeleteLocalRef(jAppInfo);
    env->DeleteLocalRef(jProviders);
    env->DeleteLocalRef(jInstrumentationName);
    env->DeleteLocalRef(jPi);
    env->DeleteLocalRef(jInstrumentationArgs);
    env->DeleteLocalRef(jInstrumentationWatcher);
    env->DeleteLocalRef(jInstrumentationUiConnection);
    env->DeleteLocalRef(jConfig);
    env->DeleteLocalRef(jCompatInfo);
    env->DeleteLocalRef(jServices);
    env->DeleteLocalRef(jCoreSettings);

    // LOGGERD(TAG, "- CApplicationThreadNative::BindApplication()");
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleTrimMemory(
    /* [in] */ Int32 level)
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::ScheduleTrimMemory()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, "ScheduleTrimMemory", "Fail FindClass: IApplicationThread", __LINE__);

    jmethodID m = env->GetMethodID(c, "scheduleTrimMemory", "(I)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: scheduleTrimMemory", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)level);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: scheduleTrimMemory", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CApplicationThreadNative::ScheduleTrimMemory()");
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleRelaunchActivity(
    /* [in] */ IBinder* token,
    /* [in] */ IList* pendingResults,
    /* [in] */ IList* pendingNewIntents,
    /* [in] */ Int32 configChanges,
    /* [in] */ Boolean notResumed,
    /* [in] */ IConfiguration* config)
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::ScheduleRelaunchActivity()");
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jToken = NULL;
    jobject jpendingResults = NULL;
    jobject jpendingNewIntents = NULL;
    jobject jconfig = NULL;

    if (token != NULL) {
        HashMap<AutoPtr<IBinder>, jobject>::Iterator it = sTokenMap.Find(token);
        if (it != sTokenMap.End()) {
            jToken = it->mSecond;
        }
        else {
            LOGGERE(TAG, "CApplicationThreadNative::ScheduleRelaunchActivity() cannot find token!");
        }
    }

    if (pendingResults != NULL) {
        jclass c = env->FindClass("java/util/ArrayList");
        Util::CheckErrorAndLog(env, "ScheduleRelaunchActivity", "Fail FindClass: ArrayList", __LINE__);
        jmethodID m = env->GetMethodID(c,"<init>","()V");
        Util::CheckErrorAndLog(env, "ScheduleRelaunchActivity", "Fail GetMethodID: ArrayList", __LINE__);
        jpendingResults = env->NewObject(c, m);
        Util::CheckErrorAndLog(env, "ScheduleRelaunchActivity", "Fail NewObject: ArrayList", __LINE__);
        m = env->GetMethodID(c, "add", "(Ljava/lang/Object;)Z");
        Util::CheckErrorAndLog(env, "ScheduleRelaunchActivity", "Fail GetMethodID: add", __LINE__);

        Int32 size;
        pendingResults->GetSize(&size);
        for (Int32 i = 0; i < size; ++i) {
            AutoPtr<IInterface> obj;
            pendingResults->Get(i, (IInterface**)&obj);
            jobject jpendingResult = Util::ToJavaResultInfo(env, IResultInfo::Probe(obj));
            env->CallBooleanMethod(jpendingResults, m, jpendingResult);
            env->DeleteLocalRef(jpendingResult);
        }

        env->DeleteLocalRef(c);
    }

    if (pendingNewIntents != NULL) {
        jclass c = env->FindClass("java/util/ArrayList");
        Util::CheckErrorAndLog(env, "ScheduleRelaunchActivity", "Fail FindClass: ArrayList", __LINE__);
        jmethodID m = env->GetMethodID(c,"<init>","()V");
        Util::CheckErrorAndLog(env, "ScheduleRelaunchActivity", "Fail GetMethodID: ArrayList", __LINE__);
        jpendingNewIntents = env->NewObject(c, m);
        Util::CheckErrorAndLog(env, "ScheduleRelaunchActivity", "Fail NewObject: ArrayList", __LINE__);
        m = env->GetMethodID(c, "add", "(Ljava/lang/Object;)Z");
        Util::CheckErrorAndLog(env, "ScheduleRelaunchActivity", "Fail GetMethodID: add", __LINE__);

        Int32 size;
        pendingNewIntents->GetSize(&size);
        for (Int32 i = 0; i < size; ++i) {
            AutoPtr<IInterface> obj;
            pendingNewIntents->Get(i, (IInterface**)&obj);
            jobject jpendingNewIntent = Util::ToJavaIntent(env, IIntent::Probe(obj));
            env->CallBooleanMethod(jpendingNewIntents, m, jpendingNewIntent);
            env->DeleteLocalRef(jpendingNewIntent);
        }

        env->DeleteLocalRef(c);
    }

    if(config != NULL) {
        jconfig = Util::ToJavaConfiguration(env, config);
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, "ScheduleRelaunchActivity", "Fail FindClass: IApplicationThread", __LINE__);

    jmethodID m = env->GetMethodID(c, "scheduleRelaunchActivity",
        "(Landroid/os/IBinder;"\
         "Ljava/util/List;"\
         "Ljava/util/List;"\
         "IZLandroid/content/res/Configuration;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: scheduleRelaunchActivity", __LINE__);

    env->CallVoidMethod(mJInstance, m, jToken, jpendingResults, jpendingNewIntents,
        (jint)configChanges, (jboolean)notResumed, jconfig);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: ScheduleRelaunchActivity", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jpendingResults);
    env->DeleteLocalRef(jpendingNewIntents);
    env->DeleteLocalRef(jconfig);

    // LOGGERD(TAG, "- CApplicationThreadNative::ScheduleRelaunchActivity()");
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleStopActivity(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean showWindow,
    /* [in] */ Int32 configChanges)
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::ScheduleStopActivity()");
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jToken = NULL;

    if (token != NULL) {
        HashMap<AutoPtr<IBinder>, jobject>::Iterator it = sTokenMap.Find(token);
        if (it != sTokenMap.End()) {
            jToken = it->mSecond;
        }
        else {
            LOGGERE(TAG, "ScheduleStopActivity() cannot find token!");
        }
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, "ScheduleStopActivity", "Fail FindClass: IApplicationThread", __LINE__);

    jmethodID m = env->GetMethodID(c, "scheduleStopActivity", "(Landroid/os/IBinder;ZI)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: scheduleStopActivity", __LINE__);

    env->CallVoidMethod(mJInstance, m, jToken, (jboolean)showWindow, (jint)configChanges);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: scheduleStopActivity", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CApplicationThreadNative::ScheduleStopActivity()");
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleDestroyActivity(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean finished,
    /* [in] */ Int32 configChanges)
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::ScheduleDestroyActivity()");
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jToken = NULL;

    if (token != NULL) {
        HashMap<AutoPtr<IBinder>, jobject>::Iterator it = sTokenMap.Find(token);
        if (it != sTokenMap.End()) {
            jToken = it->mSecond;
            sTokenMap.Erase(it);
        }
        else {
            LOGGERE(TAG, "ScheduleDestroyActivity() cannot find token!");
        }
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, "ScheduleDestroyActivity", "Fail FindClass: IApplicationThread", __LINE__);

    jmethodID m = env->GetMethodID(c, "scheduleDestroyActivity", "(Landroid/os/IBinder;ZI)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: scheduleDestroyActivity", __LINE__);

    env->CallVoidMethod(mJInstance, m, jToken, (jboolean)finished, (jint)configChanges);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: scheduleDestroyActivity", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteGlobalRef(jToken);

    // LOGGERD(TAG, "- CApplicationThreadNative::ScheduleDestroyActivity()");
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleSendResult(
    /* [in] */ IBinder* token,
    /* [in] */ IList* results)
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::ScheduleSendResult()");
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    jobject jToken = NULL;

    if (token != NULL) {
        HashMap<AutoPtr<IBinder>, jobject>::Iterator it = sTokenMap.Find(token);
        if (it != sTokenMap.End()) {
            jToken = it->mSecond;
        }
        else {
            LOGGERE(TAG, "ScheduleSendResult() cannot find token!");
        }
    }
    jobject jresults = NULL;
    if (results != NULL) {
        jclass listKlass = env->FindClass("java/util/ArrayList");
        Util::CheckErrorAndLog(env, "scheduleSendResult", "Fail FindClass: ArrayList ", __LINE__);

        jmethodID m = env->GetMethodID(listKlass, "<init>", "()V");
        Util::CheckErrorAndLog(env, "scheduleSendResult", "GetMethodID: ArrayList ", __LINE__);

        jresults = env->NewObject(listKlass, m);
        Util::CheckErrorAndLog(env, "scheduleSendResult", "Fail NewObject: ArrayList ", __LINE__);

        jmethodID mAdd = env->GetMethodID(listKlass, "add", "(Ljava/lang/Object;)Z");
        Util::CheckErrorAndLog(env, "scheduleSendResult", "Fail GetMethodID: add ", __LINE__);

        jclass rnfoKlass = env->FindClass("android/app/ResultInfo");
        Util::CheckErrorAndLog(env, "scheduleSendResult", "Fail FindClass: InputDevice ", __LINE__);

        m = env->GetMethodID(rnfoKlass, "<init>", "(Ljava/lang/String;IILandroid/content/Intent;)V");
        Util::CheckErrorAndLog(env, "scheduleSendResult", "Fail GetMethodID: ResultInfo ", __LINE__);

        Int32 size;
        results->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            results->Get(i, (IInterface**)&obj);
            AutoPtr<IResultInfo> rInfo = IResultInfo::Probe(obj);
            String resultWho;
            rInfo->GetResultWho(&resultWho);
            jstring jresultWho = Util::ToJavaString(env, resultWho);
            Int32 requestCode;
            rInfo->GetRequestCode(&requestCode);
            Int32 resultCode;
            rInfo->GetResultCode(&resultCode);
            AutoPtr<IIntent> data;
            rInfo->GetData((IIntent**)&data);
            jobject jdata = NULL;
            if (data != NULL) {
                jdata = Util::ToJavaIntent(env, data);
            }
            jobject jrInfo = env->NewObject(rnfoKlass, m, jresultWho, (jint)requestCode, (jint)resultCode, jdata);
            Util::CheckErrorAndLog(env, "scheduleSendResult", "Fail NewObject: ResultInfo ", __LINE__);
            env->CallBooleanMethod(jresults, mAdd, jrInfo);
            Util::CheckErrorAndLog(env, "scheduleSendResult", "Fail CallBooleanMethod: mAdd ", __LINE__);
            env->DeleteLocalRef(jresultWho);
            env->DeleteLocalRef(jrInfo);

            if(jdata){
                env->DeleteLocalRef(jdata);
            }
        }

        env->DeleteLocalRef(listKlass);
        env->DeleteLocalRef(rnfoKlass);
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, "scheduleSendResult", "Fail FindClass: IApplicationThread", __LINE__);

    jmethodID m = env->GetMethodID(c, "scheduleSendResult", "(Landroid/os/IBinder;Ljava/util/List;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: scheduleSendResult", __LINE__);

    env->CallVoidMethod(mJInstance, m, jToken, jresults);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: scheduleSendResult", __LINE__);

    if(jresults){
        env->DeleteLocalRef(jresults);
    }

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CApplicationThreadNative::ScheduleSendResult()");
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleResumeActivity(
    /* [in] */ IBinder* token,
    /* [in] */ Int32 processState,
    /* [in] */ Boolean isForward,
    /* [in] */ IBundle* resumeArgs)
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::ScheduleResumeActivity()");
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    jobject jToken = NULL;

    if (token != NULL) {
        HashMap<AutoPtr<IBinder>, jobject>::Iterator it = sTokenMap.Find(token);
        if (it != sTokenMap.End()) {
            jToken = it->mSecond;
        }
        else {
            LOGGERE(TAG, "ScheduleResumeActivity() cannot find token!");
        }
    }
    jobject jresumeArgs = NULL;
    if (resumeArgs != NULL) {
        jresumeArgs = Util::ToJavaBundle(env, resumeArgs);
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, "ScheduleResumeActivity", "Fail FindClass: IApplicationThread", __LINE__);

    jmethodID m = env->GetMethodID(c, "scheduleResumeActivity", "(Landroid/os/IBinder;IZLandroid/os/Bundle;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: scheduleResumeActivity", __LINE__);

    env->CallVoidMethod(mJInstance, m, jToken, processState, (jboolean)isForward, jresumeArgs);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: scheduleResumeActivity", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jresumeArgs);

    // LOGGERD(TAG, "- CApplicationThreadNative::ScheduleResumeActivity()");
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleNewIntent(
    /* [in] */ IList* intents,
    /* [in] */ IBinder* token)
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::ScheduleNewIntent()");
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    jobject jToken = NULL;

    jobject jintents = NULL;
    if (intents != NULL) {
        Int32 count;
        intents->GetSize(&count);

        jclass listKlass = env->FindClass("java/util/ArrayList");
        Util::CheckErrorAndLog(env, TAG, "FindClass: ArrayList ", __LINE__);

        jmethodID m = env->GetMethodID(listKlass, "<init>", "()V");
        Util::CheckErrorAndLog(env, TAG, "GetMethodID: ArrayList ", __LINE__);

        jintents = env->NewObject(listKlass, m);
        Util::CheckErrorAndLog(env, TAG, "NewObject: ArrayList ", __LINE__);

        jmethodID mAdd = env->GetMethodID(listKlass, "add", "(Ljava/lang/Object;)Z");
        Util::CheckErrorAndLog(env, TAG, "GetMethodID: add ", __LINE__);

        if (count > 0) {
            for (Int32 i = 0; i < count; i++) {
                AutoPtr<IInterface> obj;
                intents->Get(i, (IInterface**)&obj);
                AutoPtr<IIntent> intent = IIntent::Probe(obj);
                if (intent != NULL) {
                    jobject jintent =  Util::ToJavaIntent(env, intent);
                    env->CallBooleanMethod(jintents, mAdd, jintent);
                    Util::CheckErrorAndLog(env, TAG, "CallObjectMethod: mAdd ", __LINE__);
                    env->DeleteLocalRef(jintent);
                }
            }
        }

        env->DeleteLocalRef(listKlass);
    }

    if (token != NULL) {
        HashMap<AutoPtr<IBinder>, jobject>::Iterator it = sTokenMap.Find(token);
        if (it != sTokenMap.End()) {
            jToken = it->mSecond;
        }
        else {
            LOGGERE(TAG, "ScheduleNewIntent() cannot find token!");
        }
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, "ScheduleNewIntent", "Fail FindClass: IApplicationThread", __LINE__);

    jmethodID m = env->GetMethodID(c, "scheduleNewIntent", "(Ljava/util/List;Landroid/os/IBinder;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: scheduleNewIntent", __LINE__);

    env->CallVoidMethod(mJInstance, m, jintents, jToken);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: scheduleNewIntent", __LINE__);

    if(jintents){
        env->DeleteLocalRef(jintents);
    }

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CApplicationThreadNative::ScheduleNewIntent()");
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleCreateService(
    /* [in] */ IBinder* token,
    /* [in] */ IServiceInfo* info,
    /* [in] */ ICompatibilityInfo* compatInfo,
    /* [in] */ Int32 processState)
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::ScheduleCreateService()");
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    jobject jToken = NULL;

    if (token != NULL) {
        HashMap<AutoPtr<IBinder>, jobject>::Iterator it = sServiceTokenMap.Find(token);
        if (it != sServiceTokenMap.End()) {
            jToken = it->mSecond;
        }
        else {
            jclass c = env->FindClass("android/os/ElBinderProxy");
            Util::CheckErrorAndLog(env, TAG, "FindClass: ElBinderProxy", __LINE__);

            jmethodID m = env->GetMethodID(c, "<init>", "(J)V");
            Util::CheckErrorAndLog(env, TAG, "GetMethodID: ElBinderProxy", __LINE__);

            jobject tempToken = env->NewObject(c, m, (jlong)token);
            Util::CheckErrorAndLog(env, TAG, "NewObject: ElBinderProxy", __LINE__);
            token->AddRef();

            env->DeleteLocalRef(c);

            jToken = env->NewGlobalRef(tempToken);
            env->DeleteLocalRef(tempToken);
            sServiceTokenMap[token] = jToken;
        }
    }

    jobject jinfo = NULL;
    if (info != NULL) {
        jinfo = Util::ToJavaServiceInfo(env, info);
    }

    jobject jcompatInfo = NULL;
    if (compatInfo != NULL) {
        jcompatInfo = Util::ToJavaCompatibilityInfo(env, compatInfo);
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, "ScheduleCreateService", "Fail FindClass: IApplicationThread", __LINE__);

    jmethodID m = env->GetMethodID(c, "scheduleCreateService", "(Landroid/os/IBinder;Landroid/content/pm/ServiceInfo;Landroid/content/res/CompatibilityInfo;I)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: ScheduleCreateService", __LINE__);

    env->CallVoidMethod(mJInstance, m, jToken, jinfo, jcompatInfo, processState);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: ScheduleCreateService", __LINE__);

    env->DeleteLocalRef(c);

    if(jinfo){
        env->DeleteLocalRef(jinfo);
    }

    if(jcompatInfo){
        env->DeleteLocalRef(jcompatInfo);
    }

    // LOGGERD(TAG, "- CApplicationThreadNative::ScheduleCreateService()");
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleServiceArgs(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean taskRemoved,
    /* [in] */ Int32 startId,
    /* [in] */ Int32 flags,
    /* [in] */ IIntent* args)
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::ScheduleServiceArgs()");
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    jobject jToken = NULL;

    if (token != NULL) {
        HashMap<AutoPtr<IBinder>, jobject>::Iterator it = sServiceTokenMap.Find(token);
        if (it != sServiceTokenMap.End()) {
            jToken = it->mSecond;
        }
        else {
            LOGGERE(TAG, "ScheduleServiceArgs() cannot find token!");
        }
    }

    jobject jargs = NULL;
    if (args != NULL) {
        jargs = Util::ToJavaIntent(env, args);
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, "ScheduleServiceArgs", "Fail FindClass: IApplicationThread", __LINE__);

    jmethodID m = env->GetMethodID(c, "scheduleServiceArgs", "(Landroid/os/IBinder;ZIILandroid/content/Intent;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: ScheduleServiceArgs", __LINE__);

    env->CallVoidMethod(mJInstance, m, jToken, (jboolean)taskRemoved, (jint)startId, (jint)flags, jargs);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: ScheduleServiceArgs", __LINE__);

    env->DeleteLocalRef(c);

    if(jargs){
        env->DeleteLocalRef(jargs);
    }

    // LOGGERD(TAG, "- CApplicationThreadNative::ScheduleServiceArgs()");
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleWindowVisibility(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean showWindow)
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::ScheduleWindowVisibility()");
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    jobject jToken = NULL;

    if (token != NULL) {
        HashMap<AutoPtr<IBinder>, jobject>::Iterator it = sTokenMap.Find(token);
        if (it != sTokenMap.End()) {
            jToken = it->mSecond;
        }
        else {
            LOGGERE(TAG, "CApplicationThreadNative() cannot find token!");
        }
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, "scheduleWindowVisibility", "Fail FindClass: IApplicationThread", __LINE__);

    jmethodID m = env->GetMethodID(c, "scheduleWindowVisibility", "(Landroid/os/IBinder;Z)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: scheduleWindowVisibility", __LINE__);

    env->CallVoidMethod(mJInstance, m, jToken, (jboolean)showWindow);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: scheduleWindowVisibility", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CApplicationThreadNative::ScheduleWindowVisibility()");
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleRegisteredReceiver(
    /* [in] */ IIntentReceiver* receiver,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String& data,
    /* [in] */ IBundle* extras,
    /* [in] */ Boolean ordered,
    /* [in] */ Boolean sticky,
    /* [in] */ Int32 sendingUser,
    /* [in] */ Int32 processState)
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::ScheduleRegisteredReceiver()");
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jreceiver = NULL;
    if (receiver != NULL) {
        jreceiver = Util::ToJavaIntentReceiver(env, receiver);
    }

    jobject jintent = NULL;
    if (intent != NULL) {
        jintent = Util::ToJavaIntent(env, intent);
    }

    jstring jdata = Util::ToJavaString(env, data);

    jobject jextras = NULL;
    if (extras != NULL) {
        jextras = Util::ToJavaBundle(env, extras);
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IApplicationThread Line: %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "scheduleRegisteredReceiver", "(Landroid/content/IIntentReceiver;"
        "Landroid/content/Intent;ILjava/lang/String;Landroid/os/Bundle;ZZII)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: scheduleRegisteredReceiver Line: %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jreceiver, jintent, (jint)resultCode, jdata, jextras,
        (jboolean)ordered, (jboolean)sticky, (jint)sendingUser, processState);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: scheduleRegisteredReceiver Line: %d", __LINE__);

    env->DeleteLocalRef(c);

    if(jreceiver){
        env->DeleteLocalRef(jreceiver);
    }

    if(jintent){
        env->DeleteLocalRef(jintent);
    }

    env->DeleteLocalRef(jdata);

    if(jextras){
        env->DeleteLocalRef(jextras);
    }
    // LOGGERD(TAG, "- CApplicationThreadNative::ScheduleRegisteredReceiver()");
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleReceiver(
    /* [in] */ IIntent* intent,
    /* [in] */ IActivityInfo* info,
    /* [in] */ ICompatibilityInfo* compatInfo,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String& data,
    /* [in] */ IBundle* extras,
    /* [in] */ Boolean sync,
    /* [in] */ Int32 sendingUser,
    /* [in] */ Int32 processState)
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::ScheduleReceiver()");
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jintent = NULL;
    if (intent != NULL) {
        jintent = Util::ToJavaIntent(env, intent);
    }

    jobject jinfo = NULL;
    if (info != NULL) {
        jinfo = Util::ToJavaActivityInfo(env, info);
    }

    jobject jcompatInfo = NULL;
    if (compatInfo != NULL) {
        jcompatInfo = Util::ToJavaCompatibilityInfo(env, compatInfo);
    }

    jstring jdata = Util::ToJavaString(env, data);

    jobject jextras = NULL;
    if (extras != NULL) {
        jextras = Util::ToJavaBundle(env, extras);
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IApplicationThread Line: %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "scheduleReceiver", "(Landroid/content/Intent;Landroid/content/pm/ActivityInfo;"
        "Landroid/content/res/CompatibilityInfo;ILjava/lang/String;Landroid/os/Bundle;ZII)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: scheduleReceiver Line: %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jintent, jinfo, jcompatInfo, (jint)resultCode, jdata, jextras,
        (jboolean)sync, (jint)sendingUser, processState);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: scheduleReceiver Line: %d", __LINE__);

    env->DeleteLocalRef(c);
    if(jintent){
        env->DeleteLocalRef(jintent);
    }

    if(jinfo){
        env->DeleteLocalRef(jinfo);
    }

    if(jcompatInfo){
        env->DeleteLocalRef(jcompatInfo);
    }

    env->DeleteLocalRef(jdata);

    if(jextras){
        env->DeleteLocalRef(jextras);
    }

    // LOGGERD(TAG, "- CApplicationThreadNative::ScheduleReceiver()");
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleBindService(
    /* [in] */ IBinder* token,
    /* [in] */ IIntent* intent,
    /* [in] */ Boolean rebind,
    /* [in] */ Int32 processState)
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::ScheduleBindService()");
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jtoken = NULL;

    if (token != NULL) {
        HashMap<AutoPtr<IBinder>, jobject>::Iterator it = sServiceTokenMap.Find(token);
        if (it != sServiceTokenMap.End()) {
            jtoken = it->mSecond;
        }
        else {
            LOGGERE(TAG, "CApplicationThreadNative() cannot find token!");
        }
    }

    jobject jintent = NULL;
    if (intent != NULL) {
        jintent = Util::ToJavaIntent(env, intent);
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IApplicationThread Line: %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "scheduleBindService", "(Landroid/os/IBinder;Landroid/content/Intent;ZI)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: scheduleReceiver Line: %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jtoken, jintent, (jboolean)rebind, processState);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: scheduleReceiver Line: %d", __LINE__);

    env->DeleteLocalRef(c);

    if(jintent){
        env->DeleteLocalRef(jintent);
    }

    // LOGGERD(TAG, "- CApplicationThreadNative::ScheduleBindService()");
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleUnbindService(
    /* [in] */ IBinder* token,
    /* [in] */ IIntent* intent)
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::ScheduleUnbindService()");
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jtoken = NULL;

    if (token != NULL) {
        HashMap<AutoPtr<IBinder>, jobject>::Iterator it = sServiceTokenMap.Find(token);
        if (it != sServiceTokenMap.End()) {
            jtoken = it->mSecond;
        }
        else {
            LOGGERE(TAG, "ScheduleUnbindService() cannot find token!");
        }
    }

    jobject jintent = NULL;
    if (intent != NULL) {
        jintent = Util::ToJavaIntent(env, intent);
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IApplicationThread Line: %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "scheduleUnbindService", "(Landroid/os/IBinder;Landroid/content/Intent;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: scheduleUnbindService Line: %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jtoken, jintent);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: scheduleUnbindService Line: %d", __LINE__);

    env->DeleteLocalRef(c);

    if(jintent){
        env->DeleteLocalRef(jintent);
    }

    // LOGGERD(TAG, "- CApplicationThreadNative::ScheduleUnbindService()");
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleStopService(
    /* [in] */ IBinder* token)
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::ScheduleStopService()");
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jToken = NULL;

    if (token != NULL) {
        HashMap<AutoPtr<IBinder>, jobject>::Iterator it = sServiceTokenMap.Find(token);
        if (it != sServiceTokenMap.End()) {
            jToken = it->mSecond;
            sServiceTokenMap.Erase(it);
        }
        else {
            LOGGERE(TAG, "ScheduleStopService() cannot find token!");
        }
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, "ScheduleStopService", "Fail FindClass: IApplicationThread %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "scheduleStopService", "(Landroid/os/IBinder;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: scheduleStopService %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jToken);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: ScheduleStopService %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteGlobalRef(jToken);

    // LOGGERD(TAG, "- CApplicationThreadNative::ScheduleStopService()");
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleConfigurationChanged(
    /* [in] */ IConfiguration* config)
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::ScheduleConfigurationChanged()");
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jconfig = NULL;
    if (config != NULL) {
        jconfig = Util::ToJavaConfiguration(env, config);
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IApplicationThread %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "scheduleConfigurationChanged", "(Landroid/content/res/Configuration;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: scheduleConfigurationChanged %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jconfig);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: scheduleConfigurationChanged %d", __LINE__);

    env->DeleteLocalRef(c);

    if(jconfig){
        env->DeleteLocalRef(jconfig);
    }

    // LOGGERD(TAG, "- CApplicationThreadNative::ScheduleConfigurationChanged()");
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleActivityConfigurationChanged(
    /* [in] */ IBinder* token)
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::ScheduleActivityConfigurationChanged()");
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jToken = NULL;
    if (token != NULL) {
        HashMap<AutoPtr<IBinder>, jobject>::Iterator it = sTokenMap.Find(token);
        if (it != sTokenMap.End()) {
            jToken = it->mSecond;
        }
        else {
            LOGGERE(TAG, "ScheduleActivityConfigurationChanged() cannot find token!");
        }
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IApplicationThread %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "scheduleActivityConfigurationChanged", "(Landroid/os/IBinder;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: scheduleActivityConfigurationChanged %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jToken);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: scheduleActivityConfigurationChanged %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CApplicationThreadNative::ScheduleActivityConfigurationChanged()");
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleSleeping(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean sleeping)
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::ScheduleSleeping()");
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jToken = NULL;
    if (token != NULL) {
        HashMap<AutoPtr<IBinder>, jobject>::Iterator it = sTokenMap.Find(token);
        if (it != sTokenMap.End()) {
            jToken = it->mSecond;
        }
        else {
            LOGGERE(TAG, "ScheduleActivityConfigurationChanged() cannot find token!");
        }
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IApplicationThread %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "scheduleSleeping", "(Landroid/os/IBinder;Z)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: scheduleSleeping %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jToken, (jboolean)sleeping);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: scheduleSleeping %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CApplicationThreadNative::ScheduleSleeping()");
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleSuicide()
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::ScheduleSuicide()");
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IApplicationThread %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "scheduleSuicide", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: scheduleSuicide %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: scheduleSuicide %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CApplicationThreadNative::ScheduleSuicide()");
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleExit()
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::ScheduleExit()");
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IApplicationThread %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "scheduleExit", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: scheduleExit %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: scheduleExit %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CApplicationThreadNative::ScheduleExit()");
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleCrash(
    /* [in] */ const String& msg)
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::ScheduleCrash()");
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jstring jmsg = Util::ToJavaString(env, msg);

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IApplicationThread %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "scheduleCrash", "(Ljava/lang/String;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: scheduleCrash %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jmsg);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: scheduleCrash %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jmsg);

    // LOGGERD(TAG, "- CApplicationThreadNative::ScheduleCrash()");
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleLowMemory()
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::ScheduleLowMemory()");
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IApplicationThread %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "scheduleLowMemory", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: scheduleLowMemory %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: scheduleLowMemory %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CApplicationThreadNative::ScheduleLowMemory()");
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleDestroyBackupAgent(
    /* [in] */ IApplicationInfo* app,
    /* [in] */ ICompatibilityInfo* compatInfo)
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::ScheduleDestroyBackupAgent()");
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject japp = NULL;
    if (app != NULL) {
        japp = Util::ToJavaApplicationInfo(env, app);
    }

    jobject jcompatInfo = NULL;
    if (compatInfo != NULL) {
        jcompatInfo = Util::ToJavaCompatibilityInfo(env, compatInfo);
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IApplicationThread %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "scheduleDestroyBackupAgent", "(Landroid/content/pm/ApplicationInfo;Landroid/content/res/CompatibilityInfo;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: scheduleDestroyBackupAgent %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, japp, jcompatInfo);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: scheduleDestroyBackupAgent %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(japp);
    env->DeleteLocalRef(jcompatInfo);

    // LOGGERD(TAG, "- CApplicationThreadNative::ScheduleDestroyBackupAgent()");
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleCreateBackupAgent(
    /* [in] */ IApplicationInfo* app,
    /* [in] */ ICompatibilityInfo* compatInfo,
    /* [in] */ Int32 backupMode)
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::ScheduleCreateBackupAgent()");
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject japp = NULL;
    if (app != NULL) {
        japp = Util::ToJavaApplicationInfo(env, app);
    }

    jobject jcompatInfo = NULL;
    if (compatInfo != NULL) {
        jcompatInfo = Util::ToJavaCompatibilityInfo(env, compatInfo);
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IApplicationThread %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "scheduleCreateBackupAgent", "(Landroid/content/pm/ApplicationInfo;Landroid/content/res/CompatibilityInfo;I)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: scheduleCreateBackupAgent %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, japp, jcompatInfo, (jint)backupMode);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: scheduleCreateBackupAgent %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(japp);
    env->DeleteLocalRef(jcompatInfo);

    // LOGGERD(TAG, "- CApplicationThreadNative::ScheduleCreateBackupAgent()");
    return NOERROR;
}

ECode CApplicationThreadNative::UpdateTimeZone()
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::UpdateTimeZone()");
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IApplicationThread %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "updateTimeZone", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: updateTimeZone %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: updateTimeZone %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CApplicationThreadNative::UpdateTimeZone()");
    return NOERROR;
}

ECode CApplicationThreadNative::ClearDnsCache()
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::ClearDnsCache()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IApplicationThread %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "clearDnsCache", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: clearDnsCache %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: clearDnsCache %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CApplicationThreadNative::ClearDnsCache()");
    return NOERROR;
}

ECode CApplicationThreadNative::SetHttpProxy(
    /* [in] */ const String& proxy,
    /* [in] */ const String& port,
    /* [in] */ const String& exclList,
    /* [in] */ IUri* pacFileUrl)
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::SetHttpProxy()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jstring jproxy = Util::ToJavaString(env, proxy);
    jstring jport = Util::ToJavaString(env, port);
    jstring jexclList = Util::ToJavaString(env, exclList);
    jobject jpacFileUrl = Util::ToJavaUri(env, pacFileUrl);

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IApplicationThread %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "setHttpProxy", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Landroid/net/Uri;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: setHttpProxy %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jproxy, jport, jexclList, jpacFileUrl);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: setHttpProxy %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jproxy);
    env->DeleteLocalRef(jport);
    env->DeleteLocalRef(jexclList);
    env->DeleteLocalRef(jpacFileUrl);

    // LOGGERD(TAG, "- CApplicationThreadNative::SetHttpProxy()");
    return NOERROR;
}

ECode CApplicationThreadNative::ProcessInBackground()
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::ProcessInBackground()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IApplicationThread %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "processInBackground", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: processInBackground %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: processInBackground %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CApplicationThreadNative::ProcessInBackground()");
    return NOERROR;
}

ECode CApplicationThreadNative::ProfilerControl(
    /* [in] */ Boolean start,
    /* [in] */ IProfilerInfo* profilerInfo,
    /* [in] */ Int32 profileType)
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::ProfilerControl()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jprofilerInfo = NULL;
    if (profilerInfo != NULL) {
        jprofilerInfo = Util::ToJavaProfilerInfo(env, profilerInfo);
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IApplicationThread %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "profilerControl", "(ZLandroid/app/ProfilerInfo;I)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: profilerControl %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jboolean)start, jprofilerInfo, (jint)profileType);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: profilerControl %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jprofilerInfo);

    // LOGGERD(TAG, "- CApplicationThreadNative::ProfilerControl()");
    return NOERROR;
}

ECode CApplicationThreadNative::SetSchedulingGroup(
    /* [in] */ Int32 group)
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::SetSchedulingGroup()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IApplicationThread %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "setSchedulingGroup", "(I)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: setSchedulingGroup %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)group);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: setSchedulingGroup %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CApplicationThreadNative::SetSchedulingGroup()");
    return NOERROR;
}

ECode CApplicationThreadNative::DispatchPackageBroadcast(
    /* [in] */ Int32 cmd,
    /* [in] */ ArrayOf<String>* packages)
{
     // LOGGERD(TAG, "+ CApplicationThreadNative::DispatchPackageBroadcast()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobjectArray jpackages = Util::ToJavaStringArray(env, packages);

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IApplicationThread Line: %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "dispatchPackageBroadcast", "(I[Ljava/lang/String;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: dispatchPackageBroadcast Line: %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)cmd, jpackages);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: dispatchPackageBroadcast Line: %d", __LINE__);

    env->DeleteLocalRef(c);

    if(jpackages){
        env->DeleteLocalRef(jpackages);
    }

    // LOGGERD(TAG, "- CApplicationThreadNative::DispatchPackageBroadcast()");

    return NOERROR;
}

ECode CApplicationThreadNative::SetCoreSettings(
    /* [in] */ IBundle* coreSettings)
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::SetCoreSettings()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jcoreSettings = NULL;
    if (coreSettings != NULL) {
        jcoreSettings = Util::ToJavaBundle(env, coreSettings);
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IApplicationThread %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "setCoreSettings", "(Landroid/os/Bundle;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: setCoreSettings %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jcoreSettings);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: setCoreSettings %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jcoreSettings);

    // LOGGERD(TAG, "- CApplicationThreadNative::SetCoreSettings()");
    return NOERROR;
}

ECode CApplicationThreadNative::UpdatePackageCompatibilityInfo(
    /* [in] */ const String& pkg,
    /* [in] */ ICompatibilityInfo* info)
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::UpdatePackageCompatibilityInfo()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jstring jpkg = Util::ToJavaString(env, pkg);

    jobject jinfo = NULL;
    if (info != NULL) {
        jinfo = Util::ToJavaCompatibilityInfo(env, info);
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IApplicationThread %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "updatePackageCompatibilityInfo", "(Ljava/lang/String;Landroid/content/res/CompatibilityInfo;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: updatePackageCompatibilityInfo %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jpkg, jinfo);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: updatePackageCompatibilityInfo %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jpkg);
    env->DeleteLocalRef(jinfo);

    // LOGGERD(TAG, "- CApplicationThreadNative::UpdatePackageCompatibilityInfo()");
    return NOERROR;
}

ECode CApplicationThreadNative::UnstableProviderDied(
    /* [in] */ IBinder* provider)
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::UnstableProviderDied()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jToken = NULL;
    if (provider != NULL) {
        HashMap<AutoPtr<IBinder>, jobject>::Iterator it = sServiceTokenMap.Find(provider);
        if (it != sServiceTokenMap.End()) {
            jToken = it->mSecond;
        }
        else {
            LOGGERE(TAG, "UnstableProviderDied() cannot find token!");
        }
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IApplicationThread %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "unstableProviderDied", "(Landroid/os/IBinder;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: unstableProviderDied %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jToken);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: unstableProviderDied %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CApplicationThreadNative::UnstableProviderDied()");
    return NOERROR;
}

ECode CApplicationThreadNative::DumpService(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IBinder* servicetoken,
    /* [in] */ ArrayOf<String>* args)
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::DumpService()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jfd = NULL;
    if (fd != NULL) {
        jfd = Util::ToJavaFileDescriptor(env, fd);
    }

    jobject jToken = NULL;
    if (servicetoken != NULL) {
        HashMap<AutoPtr<IBinder>, jobject>::Iterator it = sServiceTokenMap.Find(servicetoken);
        if (it != sServiceTokenMap.End()) {
            jToken = it->mSecond;
        }
        else {
            LOGGERE(TAG, "DumpService() cannot find token!");
        }
    }

    jobjectArray jargs;
    if (args != NULL) {
        jargs = Util::ToJavaStringArray(env, args);
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IApplicationThread %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "dumpService", "(Ljava/io/FileDescriptor;Landroid/os/IBinder;[Ljava/lang/String;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: dumpService %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jfd, jToken, jargs);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: dumpService %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jfd);
    env->DeleteLocalRef(jargs);

    // LOGGERD(TAG, "- CApplicationThreadNative::DumpService()");
    return NOERROR;
}

ECode CApplicationThreadNative::DumpProvider(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IBinder* servicetoken,
    /* [in] */ ArrayOf<String>* args)
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::DumpProvider()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jfd = NULL;
    if (fd != NULL) {
        jfd = Util::ToJavaFileDescriptor(env, fd);
    }

    jobject jToken = NULL;
    if (servicetoken != NULL) {
        HashMap<AutoPtr<IBinder>, jobject>::Iterator it = sServiceTokenMap.Find(servicetoken);
        if (it != sServiceTokenMap.End()) {
            jToken = it->mSecond;
        }
        else {
            LOGGERE(TAG, "DumpService() cannot find token!");
        }
    }

    jobjectArray jargs;
    if (args != NULL) {
        jargs = Util::ToJavaStringArray(env, args);
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IApplicationThread %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "dumpProvider", "(Ljava/io/FileDescriptor;Landroid/os/IBinder;[Ljava/lang/String;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: dumpProvider %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jfd, jToken, jargs);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: dumpProvider %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jfd);
    env->DeleteLocalRef(jargs);

    // LOGGERD(TAG, "- CApplicationThreadNative::DumpProvider()");
    return NOERROR;
}

ECode CApplicationThreadNative::DumpHeap(
    /* [in] */ Boolean managed,
    /* [in] */ const String& path,
    /* [in] */ IParcelFileDescriptor* fd)
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::DumpHeap()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jstring jpath = Util::ToJavaString(env, path);

    jobject jfd = NULL;
    if (fd != NULL) {
        jfd = Util::ToJavaParcelFileDescriptor(env, fd);
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IApplicationThread %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "dumpHeap", "(ZLjava/lang/String;Landroid/os/ParcelFileDescriptor;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: dumpHeap %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jboolean)managed, jpath, jfd);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: dumpHeap %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jpath);
    env->DeleteLocalRef(jfd);

    // LOGGERD(TAG, "- CApplicationThreadNative::DumpHeap()");
    return NOERROR;
}

ECode CApplicationThreadNative::DumpActivity(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IBinder* servicetoken,
    /* [in] */ const String& prefix,
    /* [in] */ ArrayOf<String>* args)
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::DumpActivity()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jfd = NULL;
    if (fd != NULL) {
        jfd = Util::ToJavaFileDescriptor(env, fd);
    }

    jobject jToken = NULL;
    if (servicetoken != NULL) {
        HashMap<AutoPtr<IBinder>, jobject>::Iterator it = sServiceTokenMap.Find(servicetoken);
        if (it != sServiceTokenMap.End()) {
            jToken = it->mSecond;
        }
        else {
            LOGGERE(TAG, "DumpService() cannot find token!");
        }
    }

    jstring jprefix = Util::ToJavaString(env, prefix);

    jobjectArray jargs;
    if (args != NULL) {
        jargs = Util::ToJavaStringArray(env, args);
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IApplicationThread %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "dumpActivity", "(Ljava/io/FileDescriptor;Landroid/os/IBinder;Ljava/lang/String;[Ljava/lang/String;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: dumpActivity %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jfd, jToken, jprefix, jargs);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: dumpActivity %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jprefix);
    env->DeleteLocalRef(jargs);
    env->DeleteLocalRef(jfd);

    // LOGGERD(TAG, "- CApplicationThreadNative::DumpActivity()");
    return NOERROR;
}

ECode CApplicationThreadNative::DumpMemInfo(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IDebugMemoryInfo* mem,
    /* [in] */ Boolean checkin,
    /* [in] */ Boolean dumpFullInfo,
    /* [in] */ Boolean dumpDalvik,
    /* [in] */ ArrayOf<String>* args)
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::DumpMemInfo()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jfd = NULL;
    if (fd != NULL) {
        jfd = Util::ToJavaFileDescriptor(env, fd);
    }

    jobject jmem = NULL;
    if (mem != NULL) {
        jmem = Util::ToJavaDebugMemoryInfo(env, mem);
    }

    jobjectArray jargs;
    if (args != NULL) {
        jargs = Util::ToJavaStringArray(env, args);
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IApplicationThread %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "dumpMemInfo", "(Ljava/io/FileDescriptor;Landroid/os/Debug$MemoryInfo;ZZZ[Ljava/lang/String;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: dumpMemInfo %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jfd, jmem, (jboolean)checkin, (jboolean)dumpFullInfo, dumpDalvik, jargs);
    Util::CheckErrorAndLog(env, TAG, "CallObjectMethod: dumpMemInfo %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jfd);
    env->DeleteLocalRef(jmem);
    env->DeleteLocalRef(jargs);

    // LOGGERD(TAG, "- CApplicationThreadNative::DumpMemInfo()");
    return NOERROR;
}

ECode CApplicationThreadNative::DumpGfxInfo(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<String>* args)
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::DumpGfxInfo()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jfd = NULL;
    if (fd != NULL) {
        jfd = Util::ToJavaFileDescriptor(env, fd);
    }

    jobjectArray jargs;
    if (args != NULL) {
        jargs = Util::ToJavaStringArray(env, args);
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IApplicationThread %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "dumpGfxInfo", "(Ljava/io/FileDescriptor;[Ljava/lang/String;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: dumpGfxInfo %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jfd, jargs);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: dumpGfxInfo %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jfd);
    env->DeleteLocalRef(jargs);

    // LOGGERD(TAG, "- CApplicationThreadNative::DumpGfxInfo()");
    return NOERROR;
}

ECode CApplicationThreadNative::DumpDbInfo(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<String>* args)
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::DumpDbInfo()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jfd = NULL;
    if (fd != NULL) {
        jfd = Util::ToJavaFileDescriptor(env, fd);
    }

    jobjectArray jargs;
    if (args != NULL) {
        jargs = Util::ToJavaStringArray(env, args);
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IApplicationThread %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "dumpDbInfo", "(Ljava/io/FileDescriptor;[Ljava/lang/String;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: dumpDbInfo %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jfd, jargs);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: dumpDbInfo %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jfd);
    env->DeleteLocalRef(jargs);

    // LOGGERD(TAG, "- CApplicationThreadNative::DumpDbInfo()");
    return NOERROR;
}

ECode CApplicationThreadNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::ToString()");
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("java/lang/Object");
    Util::CheckErrorAndLog(env, "ToString", "FindClass: Object", __LINE__);

    jmethodID m = env->GetMethodID(c, "toString", "()Ljava/lang/String;");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: toString", __LINE__);

    jstring jstr = (jstring)env->CallObjectMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: toString", __LINE__);

    *str = Util::GetElString(env, jstr);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jstr);

    // LOGGERD(TAG, "- CApplicationThreadNative::ToString()");
    return NOERROR;
}

ECode CApplicationThreadNative::RequestAssistContextExtras(
    /* [in] */ IBinder* activityToken,
    /* [in] */ IBinder* requestToken,
    /* [in] */ Int32 requestType)
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::RequestAssistContextExtras()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jactivityToken = NULL;
    if (activityToken != NULL) {
        HashMap<AutoPtr<IBinder>, jobject>::Iterator it = sTokenMap.Find(activityToken);
        if (it != sTokenMap.End()) {
            jactivityToken = it->mSecond;
        }
        else {
            LOGGERE(TAG, "RequestAssistContextExtras() cannot find activityToken!");
        }
    }

    jobject jrequestToken = NULL;
    if (requestToken != NULL) {
        jclass c = env->FindClass("android/os/ElBinderProxy");
        Util::CheckErrorAndLog(env, TAG, "FindClass: ElBinderProxy", __LINE__);

        jmethodID m = env->GetMethodID(c, "<init>", "(J)V");
        Util::CheckErrorAndLog(env, TAG, "GetMethodID: ElBinderProxy", __LINE__);

        jrequestToken = env->NewObject(c, m, (jlong)requestToken);
        Util::CheckErrorAndLog(env, TAG, "NewObject: ElBinderProxy", __LINE__);
        requestToken->AddRef();

        env->DeleteLocalRef(c);
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, "RequestAssistContextExtras", "Fail FindClass: IApplicationThread", __LINE__);

    jmethodID m = env->GetMethodID(c, "requestAssistContextExtras", "(Landroid/os/IBinder;Landroid/os/IBinder;I)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: requestAssistContextExtras", __LINE__);

    env->CallVoidMethod(mJInstance, m, jactivityToken, jrequestToken, (jint)requestType);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: requestAssistContextExtras", __LINE__);

    env->DeleteLocalRef(jactivityToken);
    env->DeleteLocalRef(jrequestToken);
    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CApplicationThreadNative::RequestAssistContextExtras()");
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleTranslucentConversionComplete(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean drawComplete)
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::ScheduleTranslucentConversionComplete()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jToken = NULL;
    if (token != NULL) {
        HashMap<AutoPtr<IBinder>, jobject>::Iterator it = sTokenMap.Find(token);
        if (it != sTokenMap.End()) {
            jToken = it->mSecond;
        }
        else {
            LOGGERE(TAG, "ScheduleTranslucentConversionComplete() cannot find token!");
        }
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, "ScheduleTranslucentConversionComplete", "Fail FindClass: IApplicationThread", __LINE__);

    jmethodID m = env->GetMethodID(c, "scheduleTranslucentConversionComplete", "(Landroid/os/IBinder;Z)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: scheduleTranslucentConversionComplete", __LINE__);

    env->CallVoidMethod(mJInstance, m, jToken, (jboolean)drawComplete);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: scheduleTranslucentConversionComplete", __LINE__);

    env->DeleteLocalRef(jToken);
    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CApplicationThreadNative::ScheduleTranslucentConversionComplete()");
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleOnNewActivityOptions(
    /* [in] */ IBinder* token,
    /* [in] */ IActivityOptions* options)
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::ScheduleOnNewActivityOptions()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jToken = NULL;
    if (token != NULL) {
        HashMap<AutoPtr<IBinder>, jobject>::Iterator it = sTokenMap.Find(token);
        if (it != sTokenMap.End()) {
            jToken = it->mSecond;
        }
        else {
            LOGGERE(TAG, "ScheduleActivityConfigurationChanged() cannot find token!");
        }
    }

    jobject joptions = NULL;
    if (options != NULL) {
        AutoPtr<IBundle> bundle;
        options->ToBundle((IBundle**)&bundle);
        if (bundle != NULL) {
            jobject jbundle = Util::ToJavaBundle(env, bundle);
            jclass c = env->FindClass("android/app/ActivityOptions");
            Util::CheckErrorAndLog(env, TAG, "ScheduleOnNewActivityOptions Fail FindClass: ActivityOptions %d", __LINE__);

            jmethodID m = env->GetMethodID(c, "<init>", "(Landroid/os/Bundle;)V");
            Util::CheckErrorAndLog(env, TAG, "ScheduleOnNewActivityOptions Fail GetMethodID: ActivityOptions %d", __LINE__);

            joptions = env->NewObject(c, m, jbundle);
            Util::CheckErrorAndLog(env, TAG, "ScheduleOnNewActivityOptions Fail NewObject: ActivityOptions %d", __LINE__);
            env->DeleteLocalRef(c);
            env->DeleteLocalRef(jbundle);
        }
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "ScheduleOnNewActivityOptions", "Fail FindClass: IApplicationThread", __LINE__);

    jmethodID m = env->GetMethodID(c, "scheduleOnNewActivityOptions", "(Landroid/os/IBinder;Landroid/app/ActivityOptions;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: scheduleOnNewActivityOptions", __LINE__);

    env->CallVoidMethod(mJInstance, m, jToken, joptions);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: scheduleOnNewActivityOptions", __LINE__);

    env->DeleteLocalRef(jToken);
    env->DeleteLocalRef(joptions);
    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CApplicationThreadNative::ScheduleOnNewActivityOptions()");
    return NOERROR;
}

ECode CApplicationThreadNative::SetProcessState(
    /* [in] */ Int32 state)
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::SetProcessState()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, "SetProcessState", "Fail FindClass: IApplicationThread", __LINE__);

    jmethodID m = env->GetMethodID(c, "setProcessState", "(I)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: setProcessState", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)state);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: setProcessState", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CApplicationThreadNative::SetProcessState()");
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleInstallProvider(
    /* [in] */ IProviderInfo* provider)
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::ScheduleInstallProvider()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jProvider;
    if (provider != NULL) {
        jProvider = Util::ToJavaProviderInfo(env, provider);
    }
    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, "ScheduleInstallProvider", "Fail FindClass: IApplicationThread", __LINE__);

    jmethodID m = env->GetMethodID(c, "scheduleInstallProvider", "(Landroid/content/pm/ProviderInfo;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: scheduleInstallProvider", __LINE__);

    env->CallVoidMethod(mJInstance, m, jProvider);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: scheduleInstallProvider", __LINE__);

    env->DeleteLocalRef(jProvider);
    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CApplicationThreadNative::ScheduleInstallProvider()");
    return NOERROR;
}

ECode CApplicationThreadNative::UpdateTimePrefs(
    /* [in] */ Boolean is24Hour)
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::UpdateTimePrefs()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, "UpdateTimePrefs", "Fail FindClass: IApplicationThread", __LINE__);

    jmethodID m = env->GetMethodID(c, "updateTimePrefs", "(I)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: updateTimePrefs", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jboolean)is24Hour);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: updateTimePrefs", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CApplicationThreadNative::UpdateTimePrefs()");
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleCancelVisibleBehind(
    /* [in] */ IBinder* token)
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::ScheduleCancelVisibleBehind()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jToken = NULL;
    if (token != NULL) {
        HashMap<AutoPtr<IBinder>, jobject>::Iterator it = sTokenMap.Find(token);
        if (it != sTokenMap.End()) {
            jToken = it->mSecond;
        }
        else {
            LOGGERE(TAG, "ScheduleActivityConfigurationChanged() cannot find token!");
        }
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, "ScheduleCancelVisibleBehind", "Fail FindClass: IApplicationThread", __LINE__);

    jmethodID m = env->GetMethodID(c, "scheduleCancelVisibleBehind", "(Landroid/os/IBinder;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: scheduleCancelVisibleBehind", __LINE__);

    env->CallVoidMethod(mJInstance, m, jToken);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: scheduleCancelVisibleBehind", __LINE__);

    env->DeleteLocalRef(jToken);
    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CApplicationThreadNative::ScheduleCancelVisibleBehind()");
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleBackgroundVisibleBehindChanged(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean visible)
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::ScheduleBackgroundVisibleBehindChanged()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jToken = NULL;
    if (token != NULL) {
        HashMap<AutoPtr<IBinder>, jobject>::Iterator it = sTokenMap.Find(token);
        if (it != sTokenMap.End()) {
            jToken = it->mSecond;
        }
        else {
            LOGGERE(TAG, "ScheduleActivityConfigurationChanged() cannot find token!");
        }
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, "ScheduleBackgroundVisibleBehindChanged", "Fail FindClass: IApplicationThread", __LINE__);

    jmethodID m = env->GetMethodID(c, "scheduleBackgroundVisibleBehindChanged", "(Landroid/os/IBinder;Z)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: scheduleBackgroundVisibleBehindChanged", __LINE__);

    env->CallVoidMethod(mJInstance, m, token, (jboolean)visible);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: scheduleBackgroundVisibleBehindChanged", __LINE__);

    env->DeleteLocalRef(jToken);
    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CApplicationThreadNative::ScheduleBackgroundVisibleBehindChanged()");
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleEnterAnimationComplete(
    /* [in] */ IBinder* token)
{
    // LOGGERD(TAG, "+ CApplicationThreadNative::ScheduleEnterAnimationComplete()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jToken = NULL;
    if (token != NULL) {
        HashMap<AutoPtr<IBinder>, jobject>::Iterator it = sTokenMap.Find(token);
        if (it != sTokenMap.End()) {
            jToken = it->mSecond;
        }
        else {
            LOGGERE(TAG, "ScheduleActivityConfigurationChanged() cannot find token!");
        }
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, "ScheduleEnterAnimationComplete", "Fail FindClass: IApplicationThread", __LINE__);

    jmethodID m = env->GetMethodID(c, "scheduleEnterAnimationComplete", "(Landroid/os/IBinder;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: scheduleEnterAnimationComplete", __LINE__);

    env->CallVoidMethod(mJInstance, m, jToken);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: scheduleEnterAnimationComplete", __LINE__);

    env->DeleteLocalRef(jToken);
    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CApplicationThreadNative::ScheduleEnterAnimationComplete()");
    return NOERROR;
}

} // JavaProxy
} // Droid
} // Elastos

