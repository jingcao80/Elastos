
#include "CApplicationThreadNative.h"
#include "Util.h"
#include "elastos/droid/ext/frameworkhash.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;
using Elastos::Droid::JavaProxy::Util;
using Elastos::Droid::Content::EIID_IIntent;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CApplicationThreadNative::TAG("CApplicationThreadNative");

HashMap<AutoPtr<IBinder>, jobject> CApplicationThreadNative::sTokenMap;
HashMap<AutoPtr<IBinder>, jobject> CApplicationThreadNative::sServiceTokenMap;

CApplicationThreadNative::~CApplicationThreadNative(){
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CApplicationThreadNative::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CApplicationThreadNative::SchedulePauseActivity(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean finished,
    /* [in] */ Boolean userLeaving,
    /* [in] */ Int32 configChanges)
{
    // LOGGERD(TAG, String("+ CApplicationThreadNative::SchedulePauseActivity()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jToken = NULL;
    if (token != NULL) {
        HashMap<AutoPtr<IBinder>, jobject>::Iterator it = sTokenMap.Find(token);
        if (it != sTokenMap.End()) {
            jToken = it->mSecond;
        }
        else {
            LOGGERE(TAG, String("CApplicationThreadNative::SchedulePauseActivity() cannot find token!"));
        }
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, String("FindClass: IApplicationThread"), __LINE__);

    jmethodID m = env->GetMethodID(c, "schedulePauseActivity", "(Landroid/os/IBinder;ZZI)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: schedulePauseActivity"), __LINE__);

    env->CallVoidMethod(mJInstance, m, jToken, finished, userLeaving, configChanges);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod:"), __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CApplicationThreadNative::SchedulePauseActivity()"));
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleLaunchActivity(
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
    /* [in] */ Boolean autoStopProfiler)
{
    // LOGGERD(TAG, String("+ CApplicationThreadNative::ScheduleLaunchActivity()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jIntent = NULL;
    jobject jToken = NULL;
    jobject jInfo = NULL;
    jobject jCurConfig = NULL;
    jobject jCompatInfo = NULL;
    jobject jState = NULL;
    jobject jPendingResults = NULL;
    jobject jPendingNewIntents = NULL;
    jstring jProfileName = NULL;
    jobject jProfileFd = NULL;

    if (intent != NULL) {
        jIntent = Util::ToJavaIntent(env, intent);
        if (jIntent == NULL) {
            LOGGERE(TAG, String("ScheduleLaunchActivity: jIntent is NULL!"));
        }
    }

    if (token != NULL) {
        HashMap<AutoPtr<IBinder>, jobject>::Iterator it = sTokenMap.Find(token);
        if (it != sTokenMap.End()) {
            jToken = it->mSecond;
        }
        else {
            jclass c = env->FindClass("android/view/ElApplicationTokenProxy");
            Util::CheckErrorAndLog(env, TAG, String("FindClass: ElApplicationTokenProxy"), __LINE__);

            jmethodID m = env->GetMethodID(c, "<init>", "(I)V");
            Util::CheckErrorAndLog(env, TAG, String("GetMethodID: ElApplicationTokenProxy"), __LINE__);

            jobject tempToken = env->NewObject(c, m, (jint)token);
            Util::CheckErrorAndLog(env, TAG, String("NewObject: ElApplicationTokenProxy"), __LINE__);
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
            LOGGERE(TAG, String("ScheduleLaunchActivity: jInfo is NULL!"));
        }
    }

    if (curConfig != NULL) {
        jCurConfig = Util::ToJavaConfiguration(env, curConfig);
        if (jCurConfig == NULL) {
            LOGGERE(TAG, String("ScheduleLaunchActivity: jCurConfig is NULL!"));
        }
    }

    if (compatInfo != NULL) {
        jCompatInfo = Util::ToJavaCompatibilityInfo(env, compatInfo);
    }

    if (state != NULL) {
        jState = Util::ToJavaBundle(env, state);
        if (jState == NULL) {
            LOGGERE(TAG, String("ScheduleLaunchActivity: jState is NULL!"));
        }
    }

    if (pendingResults != NULL) {
        LOGGERE(TAG, String("ScheduleLaunchActivity: pendingResults not NULL!"));
    }

    if (pendingNewIntents != NULL) {
        LOGGERE(TAG, String("ScheduleLaunchActivity: pendingNewIntents not NULL!"));
    }

    if (!profileName.IsNull()) {
        LOGGERD(TAG, String("ScheduleLaunchActivity: profileName:") + profileName);
        jProfileName = Util::ToJavaString(env, profileName);
    }

    if (profileFd != NULL) {
        jProfileFd = Util::ToJavaParcelFileDescriptor(env, profileFd);
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, String("FindClass: IApplicationThread"), __LINE__);

    jmethodID m = env->GetMethodID(c, "scheduleLaunchActivity",
        "(Landroid/content/Intent;"\
         "Landroid/os/IBinder;"\
         "ILandroid/content/pm/ActivityInfo;"\
         "Landroid/content/res/Configuration;"\
         "Landroid/content/res/CompatibilityInfo;"\
         "Landroid/os/Bundle;"\
         "Ljava/util/List;"\
         "Ljava/util/List;"\
         "ZZLjava/lang/String;"\
         "Landroid/os/ParcelFileDescriptor;Z)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: scheduleLaunchActivity"), __LINE__);

    env->CallVoidMethod(mJInstance, m, jIntent, jToken, (jint)ident, jInfo, jCurConfig,
    jCompatInfo, jState, jPendingResults, jPendingNewIntents, (jboolean)notResumed, (jboolean)isForward,
    jProfileName, jProfileFd, (jboolean)autoStopProfiler);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: scheduleLaunchActivity"), __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jIntent);
    env->DeleteLocalRef(jInfo);
    env->DeleteLocalRef(jCurConfig);
    env->DeleteLocalRef(jCompatInfo);
    env->DeleteLocalRef(jState);
    env->DeleteLocalRef(jPendingResults);
    env->DeleteLocalRef(jPendingNewIntents);
    env->DeleteLocalRef(jProfileName);
    env->DeleteLocalRef(jProfileFd);

    // LOGGERD(TAG, String("- CApplicationThreadNative::ScheduleLaunchActivity()"));
    return NOERROR;
}

ECode CApplicationThreadNative::BindApplication(
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
    /* [in] */ IBundle* coreSettings)
{
    // LOGGERD(TAG, String("+ CApplicationThreadNative::BindApplication()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jstring jPackageName = NULL;
    jobject jAppInfo = NULL;
    jobject jProviders = NULL;
    jobject jInstrumentationName = NULL;
    jstring jProfileName = NULL;
    jobject jProfileFd = NULL;
    jobject jInstrumentationArgs = NULL;
    jobject jInstrumentationWatcher = NULL;
    jobject jConfig = NULL;
    jobject jCompatInfo = NULL;
    jobject jServices = NULL;
    jobject jCoreSettings = NULL;

    if (!packageName.IsNull()) {
        jPackageName = Util::ToJavaString(env, packageName);
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

        Boolean hasNext = FALSE;
        AutoPtr<IObjectEnumerator> enumerator;
        providers->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
        while (enumerator->MoveNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            enumerator->Current((IInterface**)&obj);
            AutoPtr<IProviderInfo> pInfo = IProviderInfo::Probe(obj);

            jobject jproviderInfo = Util::ToJavaProviderInfo(env, pInfo);
            env->CallBooleanMethod(jProviders, m, jproviderInfo);
            env->DeleteLocalRef(jproviderInfo);
        }

        env->DeleteLocalRef(c);
    }

    if (instrumentationName != NULL) {
        LOGGERE(TAG, String("BindApplication: instrumentationName not NULL!"));
        jInstrumentationName = Util::ToJavaComponentName(env, instrumentationName);
    }

    if (!profileName.IsNull()) {
        LOGGERD(TAG, String("BindApplication: profileName: ") + profileName);
        jProfileName = Util::ToJavaString(env, profileName);
    }

    if (profileFd != NULL) {
        LOGGERE(TAG, String("BindApplication: profileFd not NULL!"));
        jProfileFd = Util::ToJavaParcelFileDescriptor(env, profileFd);
    }

    if (instrumentationArgs != NULL) {
        LOGGERE(TAG, String("BindApplication: instrumentationArgs not NULL!"));
        jInstrumentationArgs = Util::ToJavaBundle(env, instrumentationArgs);
    }

    if (instrumentationWatcher != NULL) {
        LOGGERE(TAG, String("BindApplication: instrumentationWatcher not NULL!"));

        jclass c = env->FindClass("android/app/ElInstrumentationWatcherProxy");
        Util::CheckErrorAndLog(env, "BindApplication", "FindClass: ElInstrumentationWatcherProxy %d", __LINE__);

        jmethodID m = env->GetMethodID(c, "<init>", "(I)V");
        Util::CheckErrorAndLog(env, "BindApplication", "GetMethodID: ElInstrumentationWatcherProxy %d", __LINE__);

        jInstrumentationWatcher = env->NewObject(c, m, (jint)instrumentationWatcher);
        Util::CheckErrorAndLog(env, "BindApplication", "NewObject: ElInstrumentationWatcherProxy %d", __LINE__);

        instrumentationWatcher->AddRef();
        env->DeleteLocalRef(c);
    }

    if (config != NULL) {
        jConfig = Util::ToJavaConfiguration(env, config);
    }

    if (compatInfo != NULL) {
        jCompatInfo = Util::ToJavaCompatibilityInfo(env, compatInfo);
    }

    if (services != NULL) {
        Int32 count = 0;
        services->GetSize(&count);
        jclass c = env->FindClass("java/util/HashMap");
        Util::CheckErrorAndLog(env, "BindApplication", "FindClass: HashMap %d", __LINE__);

        jmethodID m = env->GetMethodID(c,"<init>","()V");
        Util::CheckErrorAndLog(env, "BindApplication", "GetMethodID: HashMap %d", __LINE__);

        jServices = env->NewObject(c, m);
        Util::CheckErrorAndLog(env, "BindApplication", "NewObject: HashMap %d", __LINE__);

        m = env->GetMethodID(c, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
        Util::CheckErrorAndLog(env, "BindApplication", "GetMethodID: put %d", __LINE__);

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

        env->DeleteLocalRef(c);
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
         "Ljava/lang/String;"\
         "Landroid/os/ParcelFileDescriptor;"\
         "ZLandroid/os/Bundle;"\
         "Landroid/app/IInstrumentationWatcher;"\
         "IZZZLandroid/content/res/Configuration;"\
         "Landroid/content/res/CompatibilityInfo;"\
         "Ljava/util/Map;"\
         "Landroid/os/Bundle;)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: bindApplication"), __LINE__);

    env->CallVoidMethod(mJInstance, m, jPackageName, jAppInfo, jProviders, jInstrumentationName, jProfileName,
        jProfileFd, (jboolean)autoStopProfiler, jInstrumentationArgs, jInstrumentationWatcher, (jint)debugMode,
        (jboolean)openGlTrace, (jboolean)restrictedBackupMode, (jboolean)persistent, jConfig, jCompatInfo, jServices, jCoreSettings);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: bindApplication"), __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jPackageName);
    env->DeleteLocalRef(jAppInfo);
    env->DeleteLocalRef(jProviders);
    env->DeleteLocalRef(jInstrumentationName);
    env->DeleteLocalRef(jProfileName);
    env->DeleteLocalRef(jProfileFd);
    env->DeleteLocalRef(jInstrumentationArgs);
    env->DeleteLocalRef(jInstrumentationWatcher);
    env->DeleteLocalRef(jConfig);
    env->DeleteLocalRef(jCompatInfo);
    env->DeleteLocalRef(jServices);
    env->DeleteLocalRef(jCoreSettings);

    // LOGGERD(TAG, String("- CApplicationThreadNative::BindApplication()"));
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleTrimMemory(
    /* [in] */ Int32 level)
{
    // LOGGERD(TAG, String("+ CApplicationThreadNative::ScheduleTrimMemory()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, "ScheduleTrimMemory", "Fail FindClass: IApplicationThread", __LINE__);

    jmethodID m = env->GetMethodID(c, "scheduleTrimMemory", "(I)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: scheduleTrimMemory"), __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)level);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: scheduleTrimMemory"), __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CApplicationThreadNative::ScheduleTrimMemory()"));
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleRelaunchActivity(
    /* [in] */ IBinder* token,
    /* [in] */ ArrayOf<IResultInfo *>* pendingResults,
    /* [in] */ ArrayOf<IIntent *>* pendingNewIntents,
    /* [in] */ Int32 configChanges,
    /* [in] */ Boolean notResumed,
    /* [in] */ IConfiguration* config)
{
    // LOGGERD(TAG, String("+ CApplicationThreadNative::ScheduleRelaunchActivity()"));
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
            LOGGERE(TAG, String("CApplicationThreadNative::ScheduleRelaunchActivity() cannot find token!"));
        }
    }

    if(pendingResults != NULL) {
        LOGGERE("Eric", String("CApplicationThreadNative::ScheduleRelaunchActivity() pendingResults not NULL!"));
    }

    if(pendingNewIntents != NULL) {
        LOGGERE("Eric", String("CApplicationThreadNative::ScheduleRelaunchActivity() pendingNewIntents not NULL!"));
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
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: scheduleRelaunchActivity"), __LINE__);

    env->CallVoidMethod(mJInstance, m, jToken, jpendingResults, jpendingNewIntents,
        (jint)configChanges, (jboolean)notResumed, jconfig);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: ScheduleRelaunchActivity"), __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jpendingResults);
    env->DeleteLocalRef(jpendingNewIntents);
    env->DeleteLocalRef(jconfig);

    // LOGGERD(TAG, String("- CApplicationThreadNative::ScheduleRelaunchActivity()"));
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleStopActivity(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean showWindow,
    /* [in] */ Int32 configChanges)
{
    // LOGGERD(TAG, String("+ CApplicationThreadNative::ScheduleStopActivity()"));
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jToken = NULL;

    if (token != NULL) {
        HashMap<AutoPtr<IBinder>, jobject>::Iterator it = sTokenMap.Find(token);
        if (it != sTokenMap.End()) {
            jToken = it->mSecond;
        }
        else {
            LOGGERE(TAG, String("ScheduleStopActivity() cannot find token!"));
        }
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, "ScheduleStopActivity", "Fail FindClass: IApplicationThread", __LINE__);

    jmethodID m = env->GetMethodID(c, "scheduleStopActivity", "(Landroid/os/IBinder;ZI)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: scheduleStopActivity"), __LINE__);

    env->CallVoidMethod(mJInstance, m, jToken, (jboolean)showWindow, (jint)configChanges);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: scheduleStopActivity"), __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CApplicationThreadNative::ScheduleStopActivity()"));
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleDestroyActivity(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean finished,
    /* [in] */ Int32 configChanges)
{
    // LOGGERD(TAG, String("+ CApplicationThreadNative::ScheduleDestroyActivity()"));
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
            LOGGERE(TAG, String("ScheduleDestroyActivity() cannot find token!"));
        }
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, "ScheduleDestroyActivity", "Fail FindClass: IApplicationThread", __LINE__);

    jmethodID m = env->GetMethodID(c, "scheduleDestroyActivity", "(Landroid/os/IBinder;ZI)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: scheduleDestroyActivity"), __LINE__);

    env->CallVoidMethod(mJInstance, m, jToken, (jboolean)finished, (jint)configChanges);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: scheduleDestroyActivity"), __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteGlobalRef(jToken);

    // LOGGERD(TAG, String("- CApplicationThreadNative::ScheduleDestroyActivity()"));
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleSendResult(
    /* [in] */ IBinder* token,
    /* [in] */ IObjectContainer* results)
{
    // LOGGERD(TAG, String("+ CApplicationThreadNative::ScheduleSendResult()"));
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    jobject jToken = NULL;

    if (token != NULL) {
        HashMap<AutoPtr<IBinder>, jobject>::Iterator it = sTokenMap.Find(token);
        if (it != sTokenMap.End()) {
            jToken = it->mSecond;
        }
        else {
            LOGGERE(TAG, String("ScheduleSendResult() cannot find token!"));
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

        AutoPtr<IObjectEnumerator> it;
        results->GetObjectEnumerator((IObjectEnumerator**)&it);
        Boolean hasNext;
        while (it->MoveNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->Current((IInterface**)&obj);
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
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: scheduleSendResult"), __LINE__);

    env->CallVoidMethod(mJInstance, m, jToken, jresults);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: scheduleSendResult"), __LINE__);

    if(jresults){
        env->DeleteLocalRef(jresults);
    }

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CApplicationThreadNative::ScheduleSendResult()"));
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleResumeActivity(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean isForward)
{
    // LOGGERD(TAG, String("+ CApplicationThreadNative::ScheduleResumeActivity()"));
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    jobject jToken = NULL;

    if (token != NULL) {
        HashMap<AutoPtr<IBinder>, jobject>::Iterator it = sTokenMap.Find(token);
        if (it != sTokenMap.End()) {
            jToken = it->mSecond;
        }
        else {
            LOGGERE(TAG, String("ScheduleResumeActivity() cannot find token!"));
        }
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, "ScheduleResumeActivity", "Fail FindClass: IApplicationThread", __LINE__);

    jmethodID m = env->GetMethodID(c, "scheduleResumeActivity", "(Landroid/os/IBinder;Z)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: scheduleResumeActivity"), __LINE__);

    env->CallVoidMethod(mJInstance, m, jToken, (jboolean)isForward);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: scheduleResumeActivity"), __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CApplicationThreadNative::ScheduleResumeActivity()"));
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleNewIntent(
    /* [in] */ IObjectContainer* intents,
    /* [in] */ IBinder* token)
{
    // LOGGERD(TAG, String("+ CApplicationThreadNative::ScheduleNewIntent()"));
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    jobject jToken = NULL;

    jobject jintents = NULL;
    if (intents != NULL) {
        Int32 count;
        intents->GetObjectCount(&count);

        jclass listKlass = env->FindClass("java/util/ArrayList");
        Util::CheckErrorAndLog(env, TAG, String("FindClass: ArrayList "), __LINE__);

        jmethodID m = env->GetMethodID(listKlass, "<init>", "()V");
        Util::CheckErrorAndLog(env, TAG, String("GetMethodID: ArrayList "), __LINE__);

        jintents = env->NewObject(listKlass, m);
        Util::CheckErrorAndLog(env, TAG, String("NewObject: ArrayList "), __LINE__);

        jmethodID mAdd = env->GetMethodID(listKlass, "add", "(Ljava/lang/Object;)Z");
        Util::CheckErrorAndLog(env, TAG, String("GetMethodID: add "), __LINE__);

        if (count > 0) {
            AutoPtr<IObjectEnumerator> objEnumerator;
            intents->GetObjectEnumerator((IObjectEnumerator**)&objEnumerator);
            Boolean hasNext = FALSE;
            while ((objEnumerator->MoveNext(&hasNext), hasNext)) {
                AutoPtr<IInterface> obj;
                objEnumerator->Current((IInterface**)&obj);
                AutoPtr<IIntent> intent = IIntent::Probe(obj);
                if (intent != NULL) {
                    jobject jintent =  Util::ToJavaIntent(env, intent);
                    env->CallBooleanMethod(jintents, mAdd, jintent);
                    Util::CheckErrorAndLog(env, TAG, String("CallObjectMethod: mAdd "), __LINE__);
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
            LOGGERE(TAG, String("ScheduleNewIntent() cannot find token!"));
        }
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, "ScheduleNewIntent", "Fail FindClass: IApplicationThread", __LINE__);

    jmethodID m = env->GetMethodID(c, "scheduleNewIntent", "(Ljava/util/List;Landroid/os/IBinder;)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: scheduleNewIntent"), __LINE__);

    env->CallVoidMethod(mJInstance, m, jintents, jToken);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: scheduleNewIntent"), __LINE__);

    if(jintents){
        env->DeleteLocalRef(jintents);
    }

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CApplicationThreadNative::ScheduleNewIntent()"));
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleCreateService(
    /* [in] */ IBinder* token,
    /* [in] */ IServiceInfo* info,
    /* [in] */ ICompatibilityInfo* compatInfo)
{
    // LOGGERD(TAG, String("+ CApplicationThreadNative::ScheduleCreateService()"));
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
            Util::CheckErrorAndLog(env, TAG, String("FindClass: ElBinderProxy"), __LINE__);

            jmethodID m = env->GetMethodID(c, "<init>", "(I)V");
            Util::CheckErrorAndLog(env, TAG, String("GetMethodID: ElBinderProxy"), __LINE__);

            jobject tempToken = env->NewObject(c, m, (jint)token);
            Util::CheckErrorAndLog(env, TAG, String("NewObject: ElBinderProxy"), __LINE__);
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

    jmethodID m = env->GetMethodID(c, "scheduleCreateService", "(Landroid/os/IBinder;Landroid/content/pm/ServiceInfo;Landroid/content/res/CompatibilityInfo;)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: ScheduleCreateService"), __LINE__);

    env->CallVoidMethod(mJInstance, m, jToken, jinfo, jcompatInfo);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: ScheduleCreateService"), __LINE__);

    env->DeleteLocalRef(c);

    if(jinfo){
        env->DeleteLocalRef(jinfo);
    }

    if(jcompatInfo){
        env->DeleteLocalRef(jcompatInfo);
    }

    // LOGGERD(TAG, String("- CApplicationThreadNative::ScheduleCreateService()"));
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleServiceArgs(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean taskRemoved,
    /* [in] */ Int32 startId,
    /* [in] */ Int32 flags,
    /* [in] */ IIntent* args)
{
    // LOGGERD(TAG, String("+ CApplicationThreadNative::ScheduleServiceArgs()"));
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    jobject jToken = NULL;

    if (token != NULL) {
        HashMap<AutoPtr<IBinder>, jobject>::Iterator it = sServiceTokenMap.Find(token);
        if (it != sServiceTokenMap.End()) {
            jToken = it->mSecond;
        }
        else {
            LOGGERE(TAG, String("ScheduleServiceArgs() cannot find token!"));
        }
    }

    jobject jargs = NULL;
    if (args != NULL) {
        jargs = Util::ToJavaIntent(env, args);
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, "ScheduleServiceArgs", "Fail FindClass: IApplicationThread", __LINE__);

    jmethodID m = env->GetMethodID(c, "scheduleServiceArgs", "(Landroid/os/IBinder;ZIILandroid/content/Intent;)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: ScheduleServiceArgs"), __LINE__);

    env->CallVoidMethod(mJInstance, m, jToken, (jboolean)taskRemoved, (jint)startId, (jint)flags, jargs);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: ScheduleServiceArgs"), __LINE__);

    env->DeleteLocalRef(c);

    if(jargs){
        env->DeleteLocalRef(jargs);
    }

    // LOGGERD(TAG, String("- CApplicationThreadNative::ScheduleServiceArgs()"));
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleWindowVisibility(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean showWindow)
{
    // LOGGERD(TAG, String("+ CApplicationThreadNative::ScheduleWindowVisibility()"));
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    jobject jToken = NULL;

    if (token != NULL) {
        HashMap<AutoPtr<IBinder>, jobject>::Iterator it = sTokenMap.Find(token);
        if (it != sTokenMap.End()) {
            jToken = it->mSecond;
        }
        else {
            LOGGERE(TAG, String("CApplicationThreadNative() cannot find token!"));
        }
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, "scheduleWindowVisibility", "Fail FindClass: IApplicationThread", __LINE__);

    jmethodID m = env->GetMethodID(c, "scheduleWindowVisibility", "(Landroid/os/IBinder;Z)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: scheduleWindowVisibility"), __LINE__);

    env->CallVoidMethod(mJInstance, m, jToken, (jboolean)showWindow);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: scheduleWindowVisibility"), __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CApplicationThreadNative::ScheduleWindowVisibility()"));
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
    /* [in] */ Int32 sendingUser)
{
    // LOGGERD(TAG, String("+ CApplicationThreadNative::ScheduleRegisteredReceiver()"));
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

    jmethodID m = env->GetMethodID(c, "scheduleRegisteredReceiver", "(Landroid/content/IIntentReceiver;Landroid/content/Intent;ILjava/lang/String;Landroid/os/Bundle;ZZI)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: scheduleRegisteredReceiver Line: %d"), __LINE__);

    env->CallVoidMethod(mJInstance, m, jreceiver, jintent, (jint)resultCode, jdata, jextras, (jboolean)ordered, (jboolean)sticky, (jint)sendingUser);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: scheduleRegisteredReceiver Line: %d"), __LINE__);

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
    // LOGGERD(TAG, String("- CApplicationThreadNative::ScheduleRegisteredReceiver()"));
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
    /* [in] */ Int32 sendingUser)
{
    // LOGGERD(TAG, String("+ CApplicationThreadNative::ScheduleReceiver()"));
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

    jmethodID m = env->GetMethodID(c, "scheduleReceiver", "(Landroid/content/Intent;Landroid/content/pm/ActivityInfo;Landroid/content/res/CompatibilityInfo;ILjava/lang/String;Landroid/os/Bundle;ZI)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: scheduleReceiver Line: %d"), __LINE__);

    env->CallVoidMethod(mJInstance, m, jintent, jinfo, jcompatInfo, (jint)resultCode, jdata, jextras, (jboolean)sync, (jint)sendingUser);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: scheduleReceiver Line: %d"), __LINE__);

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

    // LOGGERD(TAG, String("- CApplicationThreadNative::ScheduleReceiver()"));
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleBindService(
    /* [in] */ IBinder* token,
    /* [in] */ IIntent* intent,
    /* [in] */ Boolean rebind)
{
    // LOGGERD(TAG, String("+ CApplicationThreadNative::ScheduleBindService()"));
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jtoken = NULL;

    if (token != NULL) {
        HashMap<AutoPtr<IBinder>, jobject>::Iterator it = sServiceTokenMap.Find(token);
        if (it != sServiceTokenMap.End()) {
            jtoken = it->mSecond;
        }
        else {
            LOGGERE(TAG, String("CApplicationThreadNative() cannot find token!"));
        }
    }

    jobject jintent = NULL;
    if (intent != NULL) {
        jintent = Util::ToJavaIntent(env, intent);
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IApplicationThread Line: %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "scheduleBindService", "(Landroid/os/IBinder;Landroid/content/Intent;Z)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: scheduleReceiver Line: %d"), __LINE__);

    env->CallVoidMethod(mJInstance, m, jtoken, jintent, (jboolean)rebind);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: scheduleReceiver Line: %d"), __LINE__);

    env->DeleteLocalRef(c);

    if(jintent){
        env->DeleteLocalRef(jintent);
    }

    // LOGGERD(TAG, String("- CApplicationThreadNative::ScheduleBindService()"));
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleUnbindService(
    /* [in] */ IBinder* token,
    /* [in] */ IIntent* intent)
{
    // LOGGERD(TAG, String("+ CApplicationThreadNative::ScheduleUnbindService()"));
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jtoken = NULL;

    if (token != NULL) {
        HashMap<AutoPtr<IBinder>, jobject>::Iterator it = sServiceTokenMap.Find(token);
        if (it != sServiceTokenMap.End()) {
            jtoken = it->mSecond;
        }
        else {
            LOGGERE(TAG, String("ScheduleUnbindService() cannot find token!"));
        }
    }

    jobject jintent = NULL;
    if (intent != NULL) {
        jintent = Util::ToJavaIntent(env, intent);
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IApplicationThread Line: %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "scheduleUnbindService", "(Landroid/os/IBinder;Landroid/content/Intent;)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: scheduleUnbindService Line: %d"), __LINE__);

    env->CallVoidMethod(mJInstance, m, jtoken, jintent);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: scheduleUnbindService Line: %d"), __LINE__);

    env->DeleteLocalRef(c);

    if(jintent){
        env->DeleteLocalRef(jintent);
    }

    // LOGGERD(TAG, String("- CApplicationThreadNative::ScheduleUnbindService()"));
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleStopService(
    /* [in] */ IBinder* token)
{
    // LOGGERD(TAG, String("+ CApplicationThreadNative::ScheduleStopService()"));
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
            LOGGERE(TAG, String("ScheduleStopService() cannot find token!"));
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

    // LOGGERD(TAG, String("- CApplicationThreadNative::ScheduleStopService()"));
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleConfigurationChanged(
    /* [in] */ IConfiguration* config)
{
    // LOGGERD(TAG, String("+ CApplicationThreadNative::ScheduleConfigurationChanged()"));
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

    // LOGGERD(TAG, String("- CApplicationThreadNative::ScheduleConfigurationChanged()"));
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleActivityConfigurationChanged(
    /* [in] */ IBinder* token)
{
    LOGGERD(TAG, String("+ CApplicationThreadNative::ScheduleActivityConfigurationChanged()"));
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jToken = NULL;
    if (token != NULL) {
        HashMap<AutoPtr<IBinder>, jobject>::Iterator it = sTokenMap.Find(token);
        if (it != sTokenMap.End()) {
            jToken = it->mSecond;
        }
        else {
            LOGGERE(TAG, String("ScheduleActivityConfigurationChanged() cannot find token!"));
        }
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IApplicationThread %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "scheduleActivityConfigurationChanged", "(Landroid/os/IBinder;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: scheduleActivityConfigurationChanged %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jToken);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: scheduleActivityConfigurationChanged %d", __LINE__);

    env->DeleteLocalRef(c);

    LOGGERD(TAG, String("- CApplicationThreadNative::ScheduleActivityConfigurationChanged()"));
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleSleeping(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean sleeping)
{
    LOGGERD(TAG, String("+ CApplicationThreadNative::ScheduleSleeping()"));
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jToken = NULL;
    if (token != NULL) {
        HashMap<AutoPtr<IBinder>, jobject>::Iterator it = sTokenMap.Find(token);
        if (it != sTokenMap.End()) {
            jToken = it->mSecond;
        }
        else {
            LOGGERE(TAG, String("ScheduleActivityConfigurationChanged() cannot find token!"));
        }
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IApplicationThread %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "scheduleSleeping", "(Landroid/os/IBinder;Z)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: scheduleSleeping %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jToken, (jboolean)sleeping);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: scheduleSleeping %d", __LINE__);

    env->DeleteLocalRef(c);

    LOGGERD(TAG, String("- CApplicationThreadNative::ScheduleSleeping()"));
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleSuicide()
{
    LOGGERD(TAG, String("+ CApplicationThreadNative::ScheduleSuicide()"));
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IApplicationThread %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "scheduleSuicide", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: scheduleSuicide %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: scheduleSuicide %d", __LINE__);

    env->DeleteLocalRef(c);

    LOGGERD(TAG, String("- CApplicationThreadNative::ScheduleSuicide()"));
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleExit()
{
    LOGGERD(TAG, String("+ CApplicationThreadNative::ScheduleExit()"));
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IApplicationThread %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "scheduleExit", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: scheduleExit %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: scheduleExit %d", __LINE__);

    env->DeleteLocalRef(c);

    LOGGERD(TAG, String("- CApplicationThreadNative::ScheduleExit()"));
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleCrash(
    /* [in] */ const String& msg)
{
    LOGGERD(TAG, String("+ CApplicationThreadNative::ScheduleCrash()"));
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

    LOGGERD(TAG, String("- CApplicationThreadNative::ScheduleCrash()"));
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleLowMemory()
{
    LOGGERD(TAG, String("+ CApplicationThreadNative::ScheduleLowMemory()"));
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IApplicationThread %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "scheduleLowMemory", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: scheduleLowMemory %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: scheduleLowMemory %d", __LINE__);

    env->DeleteLocalRef(c);

    LOGGERD(TAG, String("- CApplicationThreadNative::ScheduleLowMemory()"));
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleDestroyBackupAgent(
    /* [in] */ IApplicationInfo* app,
    /* [in] */ ICompatibilityInfo* compatInfo)
{
    // LOGGERD(TAG, String("+ CApplicationThreadNative::ScheduleDestroyBackupAgent()"));
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

    // LOGGERD(TAG, String("- CApplicationThreadNative::ScheduleDestroyBackupAgent()"));
    return NOERROR;
}

ECode CApplicationThreadNative::ScheduleCreateBackupAgent(
    /* [in] */ IApplicationInfo* app,
    /* [in] */ ICompatibilityInfo* compatInfo,
    /* [in] */ Int32 backupMode)
{
    // LOGGERD(TAG, String("+ CApplicationThreadNative::ScheduleCreateBackupAgent()"));
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

    // LOGGERD(TAG, String("- CApplicationThreadNative::ScheduleCreateBackupAgent()"));
    return NOERROR;
}

ECode CApplicationThreadNative::UpdateTimeZone()
{
    // LOGGERD(TAG, String("+ CApplicationThreadNative::UpdateTimeZone()"));
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IApplicationThread %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "updateTimeZone", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: updateTimeZone %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: updateTimeZone %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CApplicationThreadNative::UpdateTimeZone()"));
    return NOERROR;
}

ECode CApplicationThreadNative::ClearDnsCache()
{
    // LOGGERD(TAG, String("+ CApplicationThreadNative::ClearDnsCache()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IApplicationThread %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "clearDnsCache", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: clearDnsCache %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: clearDnsCache %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CApplicationThreadNative::ClearDnsCache()"));
    return NOERROR;
}

ECode CApplicationThreadNative::SetHttpProxy(
    /* [in] */ const String& proxy,
    /* [in] */ const String& port,
    /* [in] */ const String& exclList)
{
    LOGGERD(TAG, String("+ CApplicationThreadNative::SetHttpProxy()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jstring jproxy = Util::ToJavaString(env, proxy);
    jstring jport = Util::ToJavaString(env, port);
    jstring jexclList = Util::ToJavaString(env, exclList);

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IApplicationThread %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "setHttpProxy", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: setHttpProxy %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jproxy, jport, jexclList);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: setHttpProxy %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jproxy);
    env->DeleteLocalRef(jport);
    env->DeleteLocalRef(jexclList);

    LOGGERD(TAG, String("- CApplicationThreadNative::SetHttpProxy()"));
    return NOERROR;
}

ECode CApplicationThreadNative::ProcessInBackground()
{
    LOGGERD(TAG, String("+ CApplicationThreadNative::ProcessInBackground()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IApplicationThread %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "processInBackground", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: processInBackground %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: processInBackground %d", __LINE__);

    env->DeleteLocalRef(c);

    LOGGERD(TAG, String("- CApplicationThreadNative::ProcessInBackground()"));
    return NOERROR;
}

ECode CApplicationThreadNative::ProfilerControl(
    /* [in] */ Boolean start,
    /* [in] */ const String& path,
    /* [in] */ IParcelFileDescriptor* fd,
    /* [in] */ Int32 profileType)
{
    LOGGERD(TAG, String("+ CApplicationThreadNative::ProfilerControl()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jstring jpath = Util::ToJavaString(env, path);

    jobject jfd = NULL;
    if (fd != NULL) {
        jfd = Util::ToJavaParcelFileDescriptor(env, fd);
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IApplicationThread %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "profilerControl", "(ZLjava/lang/String;Landroid/os/ParcelFileDescriptor;I)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: profilerControl %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jboolean)start, jpath, jfd, (jint)profileType);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: profilerControl %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jpath);
    env->DeleteLocalRef(jfd);

    LOGGERD(TAG, String("- CApplicationThreadNative::ProfilerControl()"));
    return NOERROR;
}

ECode CApplicationThreadNative::SetSchedulingGroup(
    /* [in] */ Int32 group)
{
    LOGGERD(TAG, String("+ CApplicationThreadNative::SetSchedulingGroup()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IApplicationThread %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "setSchedulingGroup", "(I)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: setSchedulingGroup %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)group);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: setSchedulingGroup %d", __LINE__);

    env->DeleteLocalRef(c);

    LOGGERD(TAG, String("- CApplicationThreadNative::SetSchedulingGroup()"));
    return NOERROR;
}

ECode CApplicationThreadNative::GetMemoryInfo(
    /* [in] */ IDebugMemoryInfo* outInfo)
{
    LOGGERD(TAG, String("+ CApplicationThreadNative::GetMemoryInfo()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject joutInfo = Util::ToJavaDebugMemoryInfo(env, outInfo);

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IApplicationThread %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "getMemoryInfo", "(Landroid/os/Debug$MemoryInfo;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: getMemoryInfo %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, joutInfo);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: getMemoryInfo %d", __LINE__);

    if (!Util::GetElDebugMemoryInfo(env, joutInfo, outInfo)) {
        LOGGERE(TAG, "GetMemoryInfo() GetElDebugMemoryInfo fail!");
    }

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(joutInfo);

    LOGGERD(TAG, String("- CApplicationThreadNative::GetMemoryInfo()"));
    return NOERROR;
}

ECode CApplicationThreadNative::DispatchPackageBroadcast(
    /* [in] */ Int32 cmd,
    /* [in] */ ArrayOf<String>* packages)
{
     // LOGGERD(TAG, String("+ CApplicationThreadNative::DispatchPackageBroadcast()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobjectArray jpackages = Util::ToJavaStringArray(env, packages);

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IApplicationThread Line: %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "dispatchPackageBroadcast", "(I[Ljava/lang/String;)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: dispatchPackageBroadcast Line: %d"), __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)cmd, jpackages);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: dispatchPackageBroadcast Line: %d"), __LINE__);

    env->DeleteLocalRef(c);

    if(jpackages){
        env->DeleteLocalRef(jpackages);
    }

    // LOGGERD(TAG, String("- CApplicationThreadNative::DispatchPackageBroadcast()"));

    return NOERROR;
}

ECode CApplicationThreadNative::SetCoreSettings(
    /* [in] */ IBundle* coreSettings)
{
    LOGGERD(TAG, String("+ CApplicationThreadNative::SetCoreSettings()"));

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

    LOGGERD(TAG, String("- CApplicationThreadNative::SetCoreSettings()"));
    return NOERROR;
}

ECode CApplicationThreadNative::UpdatePackageCompatibilityInfo(
    /* [in] */ const String& pkg,
    /* [in] */ ICompatibilityInfo* info)
{
    LOGGERD(TAG, String("+ CApplicationThreadNative::UpdatePackageCompatibilityInfo()"));

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

    LOGGERD(TAG, String("- CApplicationThreadNative::UpdatePackageCompatibilityInfo()"));
    return NOERROR;
}

ECode CApplicationThreadNative::RequestThumbnail(
    /* [in] */ IBinder* token)
{
    LOGGERD(TAG, String("+ CApplicationThreadNative::RequestThumbnail()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jToken = NULL;
    if (token != NULL) {
        HashMap<AutoPtr<IBinder>, jobject>::Iterator it = sTokenMap.Find(token);
        if (it != sTokenMap.End()) {
            jToken = it->mSecond;
        }
        else {
            LOGGERE(TAG, String("RequestThumbnail() cannot find token!"));
        }
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IApplicationThread %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "requestThumbnail", "(Landroid/os/IBinder;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: requestThumbnail %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jToken);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: requestThumbnail %d", __LINE__);

    env->DeleteLocalRef(c);

    LOGGERD(TAG, String("- CApplicationThreadNative::RequestThumbnail()"));
    return NOERROR;
}

ECode CApplicationThreadNative::UnstableProviderDied(
    /* [in] */ IBinder* provider)
{
    LOGGERD(TAG, String("+ CApplicationThreadNative::UnstableProviderDied()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jToken = NULL;
    if (provider != NULL) {
        HashMap<AutoPtr<IBinder>, jobject>::Iterator it = sServiceTokenMap.Find(provider);
        if (it != sServiceTokenMap.End()) {
            jToken = it->mSecond;
        }
        else {
            LOGGERE(TAG, String("UnstableProviderDied() cannot find token!"));
        }
    }

    jclass c = env->FindClass("android/app/IApplicationThread");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IApplicationThread %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "unstableProviderDied", "(Landroid/os/IBinder;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: unstableProviderDied %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jToken);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: unstableProviderDied %d", __LINE__);

    env->DeleteLocalRef(c);

    LOGGERD(TAG, String("- CApplicationThreadNative::UnstableProviderDied()"));
    return NOERROR;
}

ECode CApplicationThreadNative::DumpService(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IBinder* servicetoken,
    /* [in] */ ArrayOf<String>* args)
{
    LOGGERD(TAG, String("+ CApplicationThreadNative::DumpService()"));

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
            LOGGERE(TAG, String("DumpService() cannot find token!"));
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

    LOGGERD(TAG, String("- CApplicationThreadNative::DumpService()"));
    return NOERROR;
}

ECode CApplicationThreadNative::DumpProvider(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IBinder* servicetoken,
    /* [in] */ ArrayOf<String>* args)
{
    LOGGERD(TAG, String("+ CApplicationThreadNative::DumpProvider()"));

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
            LOGGERE(TAG, String("DumpService() cannot find token!"));
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

    LOGGERD(TAG, String("- CApplicationThreadNative::DumpProvider()"));
    return NOERROR;
}

ECode CApplicationThreadNative::DumpHeap(
    /* [in] */ Boolean managed,
    /* [in] */ const String& path,
    /* [in] */ IParcelFileDescriptor* fd)
{
    LOGGERD(TAG, String("+ CApplicationThreadNative::DumpHeap()"));

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

    LOGGERD(TAG, String("- CApplicationThreadNative::DumpHeap()"));
    return NOERROR;
}

ECode CApplicationThreadNative::DumpActivity(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IBinder* servicetoken,
    /* [in] */ const String& prefix,
    /* [in] */ ArrayOf<String>* args)
{
    LOGGERD(TAG, String("+ CApplicationThreadNative::DumpActivity()"));

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
            LOGGERE(TAG, String("DumpService() cannot find token!"));
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

    LOGGERD(TAG, String("- CApplicationThreadNative::DumpActivity()"));
    return NOERROR;
}

ECode CApplicationThreadNative::DumpMemInfo(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Boolean checkin,
    /* [in] */ Boolean all,
    /* [in] */ ArrayOf<String>* args,
    /* [out] */ IDebugMemoryInfo** info)
{
    LOGGERD(TAG, String("+ CApplicationThreadNative::DumpMemInfo()"));

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

    jmethodID m = env->GetMethodID(c, "dumpMemInfo", "(Ljava/io/FileDescriptor;ZZ[Ljava/lang/String;)Landroid/os/Debug$MemoryInfo;");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: dumpMemInfo %d", __LINE__);

    jobject jinfo = env->CallObjectMethod(mJInstance, m, jfd, (jboolean)checkin, (jboolean)all, jargs);
    Util::CheckErrorAndLog(env, TAG, "CallObjectMethod: dumpMemInfo %d", __LINE__);

    if (!Util::GetElDebugMemoryInfo(env, jinfo, info)) {
        LOGGERE(TAG, "DumpMemInfo() GetElDebugMemoryInfo fail!");
    }

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jfd);
    env->DeleteLocalRef(jinfo);
    env->DeleteLocalRef(jargs);

    LOGGERD(TAG, String("- CApplicationThreadNative::DumpMemInfo()"));
    return NOERROR;
}

ECode CApplicationThreadNative::DumpGfxInfo(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<String>* args)
{
    LOGGERD(TAG, String("+ CApplicationThreadNative::DumpGfxInfo()"));

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

    LOGGERD(TAG, String("- CApplicationThreadNative::DumpGfxInfo()"));
    return NOERROR;
}

ECode CApplicationThreadNative::DumpDbInfo(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<String>* args)
{
    LOGGERD(TAG, String("+ CApplicationThreadNative::DumpDbInfo()"));

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

    LOGGERD(TAG, String("- CApplicationThreadNative::DumpDbInfo()"));
    return NOERROR;
}

ECode CApplicationThreadNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, String("+ CApplicationThreadNative::ToString()"));
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("java/lang/Object");
    Util::CheckErrorAndLog(env, "ToString", "FindClass: Object", __LINE__);

    jmethodID m = env->GetMethodID(c, "toString", "()Ljava/lang/String;");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: toString"), __LINE__);

    jstring jstr = (jstring)env->CallObjectMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: toString"), __LINE__);

    *str = Util::GetElString(env, jstr);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jstr);

    // LOGGERD(TAG, String("- CApplicationThreadNative::ToString()"));
    return NOERROR;
}

} // JavaProxy
} // Droid
} // Elastos

