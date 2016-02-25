//==========================================================================
// Copyright (c) 2000-2009,  Elastos, Inc.  All Rights Reserved.
//==========================================================================
#include <Elastos.Droid.Server.h>
#include <elastos/utility/logging/Slogger.h>
#include <elautoptr.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>
#include <dlfcn.h>
#include <unistd.h>
#include "cutils/properties.h"

_ELASTOS_NAMESPACE_USING

using Elastos::Utility::Logging::Slogger;

using Elastos::Droid::Privacy::IIPrivacySettingsManager;
using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Server::ISystemServer;
using Elastos::Droid::Server::CSystemServer;

#ifndef ASSERT_SUCCEEDED
#define ASSERT_SUCCEEDED(expr) \
   do { \
       ECode ec = expr; \
       assert(SUCCEEDED(ec)); \
   } while(0);
#endif

static const String TAG("superexe");

static Boolean s_bDebugFlag = FALSE;
Int32 option = 0;                       //For Framework App Testing

void Usage()
{
    printf("superexe.ecx [-debug] <Option> [args ...] \n");
    printf("Options: \n");
    printf("\t-debug\t\tFor debugging\n");
    printf("\t-app\tRun an application\n");
    printf("\t-test\t\tRun a test\n");
    printf("\t-?\t\t\tFor Help\n");
}

void DebugBreak()
{
    printf("DebugBreak not supported yet.\n");
}

int LaunchApp(void *handle, char *appName);
int LaunchTest(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    if (argc == 4){
        option = atoi(argv[3]);
    }

    if (argc < 2) {
        Usage();
        return 0;
    }

    if (!strcmp(argv[1], "-debug")) {
        s_bDebugFlag = TRUE;
        argv += 2;
        argc -= 3;
    }
    else {
        argv += 1;
        argc -= 2;
    }

    if (argv[0][0] == '-') {
        if (!strcmp(argv[0] + 1, "app")) {
            return LaunchApp(NULL, argv[1]);
        }
        else if (!strcmp(argv[0] + 1, "test")) {
            return LaunchTest(argc, argv + 1);
        }
        else {
            Usage();
            return 0;
        }
    }
    else {
        Usage();
        return 0;
    }
}

#include <media/AudioSystem.h>

//TODO::
//
ECode InitAudio()
{
    android::AudioSystem::initStreamVolume(AUDIO_STREAM_VOICE_CALL, 0, 5);
    android::AudioSystem::initStreamVolume(AUDIO_STREAM_SYSTEM, 0, 7);
    android::AudioSystem::initStreamVolume(AUDIO_STREAM_RING, 0, 7);
    android::AudioSystem::initStreamVolume(AUDIO_STREAM_MUSIC, 0, 15);
    android::AudioSystem::initStreamVolume(AUDIO_STREAM_ALARM, 0, 7);
    android::AudioSystem::initStreamVolume(AUDIO_STREAM_NOTIFICATION, 0, 7);
    android::AudioSystem::initStreamVolume(AUDIO_STREAM_BLUETOOTH_SCO, 0, 15);
    android::AudioSystem::initStreamVolume(AUDIO_STREAM_ENFORCED_AUDIBLE, 0, 7);
    android::AudioSystem::initStreamVolume(AUDIO_STREAM_DTMF, 0, 15);
    android::AudioSystem::initStreamVolume(AUDIO_STREAM_TTS, 0, 15);

    android::AudioSystem::setStreamVolumeIndex(AUDIO_STREAM_VOICE_CALL, 4, AUDIO_DEVICE_OUT_DEFAULT);
    android::AudioSystem::setStreamVolumeIndex(AUDIO_STREAM_SYSTEM, 5, AUDIO_DEVICE_OUT_DEFAULT);
    android::AudioSystem::setStreamVolumeIndex(AUDIO_STREAM_RING, 5, AUDIO_DEVICE_OUT_DEFAULT);
    android::AudioSystem::setStreamVolumeIndex(AUDIO_STREAM_MUSIC, 11, AUDIO_DEVICE_OUT_DEFAULT);
    android::AudioSystem::setStreamVolumeIndex(AUDIO_STREAM_ALARM, 6, AUDIO_DEVICE_OUT_DEFAULT);
    android::AudioSystem::setStreamVolumeIndex(AUDIO_STREAM_NOTIFICATION, 5, AUDIO_DEVICE_OUT_DEFAULT);
    android::AudioSystem::setStreamVolumeIndex(AUDIO_STREAM_BLUETOOTH_SCO, 7, AUDIO_DEVICE_OUT_DEFAULT);
    android::AudioSystem::setStreamVolumeIndex(AUDIO_STREAM_ENFORCED_AUDIBLE, 5, AUDIO_DEVICE_OUT_DEFAULT);
    android::AudioSystem::setStreamVolumeIndex(AUDIO_STREAM_DTMF, 11, AUDIO_DEVICE_OUT_DEFAULT);
    android::AudioSystem::setStreamVolumeIndex(AUDIO_STREAM_TTS, 11, AUDIO_DEVICE_OUT_DEFAULT);

    return NOERROR;
}

int LaunchApp(void *handle, char *appName)
{
    if (s_bDebugFlag == TRUE) {
        DebugBreak();
    }

    Int32 pid = fork();
    if(pid == 0){
        char * argv[] = { (char*)"elserviceManager", NULL };
        if(execv("/system/bin/elserviceManager", argv) < 0){
            Slogger::E(TAG, "Execute elserviceManager.ecx fail!\n");
        }
        return 0;
    }
    else if(pid == -1) {
        Slogger::E(TAG, "Create elserviceManager process fail!\n");
        return -1;
    }

    InitAudio();

    pid = fork();
    if(pid == 0){
        char * argv[] = { "/system/bin/ElApp_process", "/system/lib", "--zygote", "--start-system-server", NULL };
        if(execv("/system/bin/ElApp_process", argv) < 0){
            Slogger::E(TAG, "Execute ElApp_process.ecx fail!\n");
        }
        return 0;
    }
    else if(pid == -1) {
        Slogger::E(TAG, "Create ElApp_process process fail!\n");
        return -1;
    }

#ifdef FOR_TEST
    // wait for systemserver init complete
    Slogger::D(TAG, "Sleep 3 seconds!");
    sleep(3);

    AutoPtr<IServiceManager> serviceManager;
    CServiceManager::AcquireSingleton((IServiceManager**)&serviceManager);
    int nWaitTime = 3;
    AutoPtr<IIPrivacySettingsManager> privacyManager;
    for (int i = 0; i < 15; i++) {
        serviceManager->GetService(String("privacy"), (IInterface**)&privacyManager);
        if (NULL != privacyManager) break;
        else {
            printf("---- %s, wait for SystemServer! %ds\n", __FUNCTION__, ++nWaitTime);
            sleep(1);
        }
    }

    AutoPtr<IIActivityManager> activityManager;
    serviceManager->GetService(IContext::ACTIVITY_SERVICE, (IInterface**)&activityManager);
    assert(activityManager != NULL);

    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->SetPackage(String(appName));
    intent->SetAction(String("android.intent.action.MAIN"));
    if (option != 0){
        intent->PutExtra(String("ARG"), option);
    }
    Int32 status;
    if (FAILED(activityManager->StartActivity(NULL, intent, String(NULL),
            NULL, String(NULL), 0, 0, String(NULL), NULL, NULL, &status))) {
        Slogger::E(TAG, "Create ActivityManager StartActivity fail!\n");
    }

    sem_t m_event;
    sem_init(&m_event, 0, 0);
    sem_wait(&m_event);
#endif

    return 0;
}

int LaunchTest(int argc, char *argv[])
{
    return -1;
}
