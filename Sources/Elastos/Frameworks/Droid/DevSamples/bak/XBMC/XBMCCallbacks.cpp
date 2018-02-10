
#include "elastos/droid/ext/frameworkdef.h"
#include "XBMCCallbacks.h"
#include "ElAudioTrack.h"
#include "WifiNative.h"
#include <elastos/utility/etl/Vector.h>
#include <pthread.h>
#include <utils/Log.h>
#include <cutils/properties.h>
#include <android/native_window.h>
#include <gui/Surface.h>
#include "Ethernet.h"

#include <Elastos.Droid.Core.h>

using Elastos::Utility::Etl::Vector;

using Elastos::Droid::Graphics::ISurfaceTexture;
using Elastos::Droid::Graphics::CSurfaceTexture;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::View::CSurface;

static Object gLock;

extern start_browser();
extern start_calculator();
extern finish_activity();

void* CreateElAudioTrack(
    int streamType, int sampleRateInHz, int channelConfig,
    int audioFormat, int bufferSizeInBytes, int mode)
{
    ElAudioTrack* track = new ElAudioTrack(
        streamType, sampleRateInHz, channelConfig,
        audioFormat, bufferSizeInBytes, mode);

    return (void*)track;
}

void DeleteElAudioTrack(void* track)
{
    ElAudioTrack* ptr = (ElAudioTrack*)track;
    delete ptr;
}

void ElAudioTrack_Play(void* track)
{
    if (track) {
        ((ElAudioTrack*)track)->Play();
    }
}

void ElAudioTrack_Stop(void* track)
{
    if (track) {
        ((ElAudioTrack*)track)->Stop();
    }
}

void ElAudioTrack_Flush(void* track)
{
    if (track) {
        ((ElAudioTrack*)track)->Flush();
    }
}

void ElAudioTrack_Release(void* track)
{
    if (track) {
        ((ElAudioTrack*)track)->Release();
    }
}

int ElAudioTrack_Write(void* track, void* audioData, int offsetInBytes, int sizeInBytes)
{
    if (track) {
        return ((ElAudioTrack*)track)->Write(audioData, offsetInBytes, sizeInBytes);
    }
    return -1;
}

int ElAudioTrack_GetPlayState(void* track)
{
    if (track) {
        return ((ElAudioTrack*)track)->GetPlayState();
    }
    return -1;
}

int ElAudioTrack_SetStereoVolume(void* track, float leftVolume, float rightVolume)
{
    if (track) {
        return ((ElAudioTrack*)track)->SetStereoVolume(leftVolume, rightVolume);
    }
    return -1;
}

int ElAudioTrack_GetPlaybackHeadPosition(void* track)
{
    if (track) {
        return ((ElAudioTrack*)track)->GetPlaybackHeadPosition();
    }
    return -1;
}

int ElAudioTrack_GetMinBufferSize(int sampleRateInHz, int channelConfig, int audioFormat)
{
    return ElAudioTrack::GetMinBufferSize(sampleRateInHz, channelConfig, audioFormat);
}

void* ElWifi_Start_Internal(void* arg)
{
    AutoLock lock(&gLock);

    if(!WifiNative::LoadDriver())
        return NULL;
    ALOGD("ElWifi:== LoadDriver succeeded ==\n");

    WifiNative::SetInterfaceDown();

    if(!WifiNative::StartSupplicant()) {
        WifiNative::UnloadDriver();
        return NULL;
    }
    ALOGD("ElWifi:== StartSupplicant succeeded==\n");

    int connectTries = 0;
    while (true) {
        if (WifiNative::ConnectToSupplicant()) {
            ALOGD("ElWifi:== ConnectToSupplicant succeeded!==\n");
            break;
        }
        if (connectTries++ < 5) {
            usleep(1000000);
        }
        else {
            ALOGD("ElWifi:== ConnectToSupplicant failed! ==\n");
            WifiNative::StopSupplicant();
            WifiNative::UnloadDriver();
            return NULL;
        }
    }

    if(!WifiNative::ReloadConfigCommand())
        return NULL;
    ALOGD("ElWifi:== ReloadConfigCommand succeeded==\n");

    if (WifiNative::RunDhcp())
        property_set("net.state", "1");

    return NULL;
}

bool ElWifi_Start()
{
    pthread_t thread;
    return pthread_create(&thread, NULL, ElWifi_Start_Internal, NULL);
}

void* ElWifi_Stop_Internal(void*)
{
    AutoLock lock(&gLock);

    WifiNative::StopDhcp();

    if(!WifiNative::StopSupplicant())
        return NULL;
    ALOGD("ElWifi:== StopSupplicant succeeded==\n");

    WifiNative::SetInterfaceDown();

    if(!WifiNative::UnloadDriver())
        return NULL;
    ALOGD("ElWifi:== UnloadDriver succeeded==\n");

    property_set("net.state", "0");

    return NULL;
}

bool ElWifi_Stop()
{
    pthread_t thread;
    return pthread_create(&thread, NULL, ElWifi_Stop_Internal, NULL);
}

ScanResult* ParseScanResult(String line)
{
    if (line.IsNullOrEmpty()) {
        return NULL;
    }

    ScanResult* scanResult = NULL;

    /*
     * Cache implementation (LinkedHashMap) is not synchronized, thus,
     * must synchronized here!
     */
    String result[5];
    int resultCount = 0;

    AutoPtr<ArrayOf<Char32> > chars = line.GetChars();
    int lineLen = chars->GetLength();
    for (int tabBeg = 0, tabEnd = 0; tabEnd <= lineLen; ++tabEnd) {
        if (tabEnd == lineLen || (*chars)[tabEnd] == '\t') {
            if (tabEnd > tabBeg) {
                result[resultCount] = line.Substring(tabBeg, tabEnd);
                ++resultCount;
            }
            tabBeg = tabEnd + 1;
        }
    }

    if (3 <= resultCount && resultCount <= 5) {
        //String bssid = result[0];
        // bssid | frequency | level | flags | ssid
        // int frequency;
        // int level;
        // try {
        //     frequency = Integer.parseInt(result[1]);
        //     level = Integer.parseInt(result[2]);
            /* some implementations avoid negative values by adding 256
             * so we need to adjust for that here.
             */
        //     if (level > 0) level -= 256;
        // } catch (NumberFormatException e) {
        //     frequency = 0;
        //     level = 0;
        // }

        /*
         * The formatting of the results returned by
         * wpa_supplicant is intended to make the fields
         * line up nicely when printed,
         * not to make them easy to parse. So we have to
         * apply some heuristics to figure out which field
         * is the SSID and which field is the flags.
         */
        String ssid;
        String flags;
        if (resultCount == 4) {
            if (result[3].GetChar(0) == '[') {
                flags = result[3];
                ssid = "";
            }
            else {
                flags = "";
                ssid = result[3];
            }
        }
        else if (resultCount == 5) {
            flags = result[3];
            ssid = result[4];
        }
        else {
            // Here, we must have 3 fields: no flags and ssid
            // set
            flags = "";
            ssid = "";
        }

        // Do not add scan results that have no SSID set
        if (0 < ssid.Trim().GetLength()) {
            int security = -1;
            if (flags.Contains("WPA-PSK") || flags.Contains("WPA2-PSK"))
                security = 0;
            else if (flags.Contains("WEP"))
                security = 1;
            else if (flags.Contains("WPA2-EAP"))
                return NULL;//TODO:
            scanResult = new ScanResult();
            strcpy(scanResult->ssid, ssid);
            scanResult->security = security;
        }
    }

    return scanResult;
}

Vector<ScanResult*>* ParseScanResults(String scanResults)
{
    if (scanResults.IsNullOrEmpty()) {
        return NULL;
    }

    Vector<ScanResult*>* scanList = new Vector<ScanResult*>;
    int lineCount = 0;

    AutoPtr<ArrayOf<Char32> > charArray = scanResults.GetChars();
    int scanResultsLen = charArray->GetLength();
    // Parse the result string, keeping in mind that the last line does
    // not end with a newline.
    for (int lineBeg = 0, lineEnd = 0; lineEnd <= scanResultsLen; ++lineEnd) {
        if (lineEnd == scanResultsLen || (*charArray)[lineEnd] == '\n') {
            ++lineCount;

            if (lineCount == 1) {
                lineBeg = lineEnd + 1;
                continue;
            }
            if (lineEnd > lineBeg) {
                String line = scanResults.Substring(lineBeg, lineEnd - lineBeg);
                ALOGD("ElWifi: ParseScanResult %d:%s", lineCount, (const char*)line);
                ScanResult* scanResult = ParseScanResult(line);
                ALOGD("ElWifi: ParseScanResult end");
                if (scanResult != NULL) {
                    scanList->PushBack(scanResult);
                } else {
                    //TODO: hidden network handling
                }
            }
            lineBeg = lineEnd + 1;
        }
    }

    if (lineCount == 1) {
        delete scanList;
        scanList = NULL;
    }

    return scanList;
}

Vector<ScanResult*>* ElWifi_ScanResults()
{
    if (WifiNative::ScanCommand(true)) {
        String str = WifiNative::ScanResultsCommand();
        ALOGD("ElWifi:scanResult:\n%s\n", (const char*)str);
        return ParseScanResults(str);
    }

    return NULL;
}

int ElWifi_AddNetwork(const char* ssid, int security, const char* password)
{
    int netId;
    netId = WifiNative::AddNetworkCommand();
    if (-1 == netId)
        return -1;
    ALOGD("ElWifi:netId = %d\n", netId);

    String strSsid("\"");
    strSsid.Append(ssid);
    strSsid.Append("\"");
    if (!WifiNative::SetNetworkVariableCommand(netId, "ssid", strSsid)) {
        WifiNative::RemoveNetworkCommand(netId);
        return -1;
    }

    String strPassword("\"");
    if (password != NULL) {
        strPassword.Append(password);
        strPassword.Append("\"");
    }
    if (0 == security) {
        if (!WifiNative::SetNetworkVariableCommand(netId, "psk", strPassword)) {
            WifiNative::RemoveNetworkCommand(netId);
            return -1;
        }
    }
    else {
        if (!WifiNative::SetNetworkVariableCommand(netId, "key_mgmt", "NONE")) {
            WifiNative::RemoveNetworkCommand(netId);
            return -1;
        }

        if (1 == security) {
            if (!WifiNative::SetNetworkVariableCommand(netId, "wep_key0", strPassword)) {
                WifiNative::RemoveNetworkCommand(netId);
                return -1;
            }

            if (!WifiNative::SetNetworkVariableCommand(netId, "wep_tx_keyidx", 0)) {
                WifiNative::RemoveNetworkCommand(netId);
                return -1;
            }
        }
    }

    if (!WifiNative::EnableNetworkCommand(netId, true)) {
        WifiNative::RemoveNetworkCommand(netId);
        return -1;
    }

    if (!WifiNative::RunDhcp()) {
        WifiNative::RemoveNetworkCommand(netId);
        return -1;
    }

    property_set("net.state", "1");

    ALOGD("ElWifi:netId = %d, Enable!\n", netId);

    WifiNative::SaveConfigCommand();


    return netId;
}

bool ElWifi_SaveConfig()
{
    return WifiNative::SaveConfigCommand();
}

bool ElWifi_ReloadConfig()
{
    return WifiNative::ReloadConfigCommand();
}

void StartBrowser()
{
    printf("======StartBrowser====\n");
    start_browser();
}

void StartCalculator()
{
    printf("======StartCalculator====\n");
    start_calculator();
}

void FinishActivity()
{
    printf("======FinishActivity====\n");
    finish_activity();
}

void* ConnectEthernet_Internal(void*)
{
    Boolean connected = Ethernet::RunDhcpEthe();
    if (connected) {
        printf("Ethernet Connected.\n");
    }
    else {
        printf("Ethernet not Connected.\n");
    }
    return NULL;
}

bool ConnectEthernet(bool connect)
{
    printf("ConnectEthernet\n");
    if (connect) {
        pthread_t thread;
        return pthread_create(&thread, NULL, ConnectEthernet_Internal, NULL);
    }
    else {
        return Ethernet::StopDhcpEthe();
    }

}

eobject CreateSurfaceTexture(int texName)
{
    ISurfaceTexture* st;
    ASSERT_SUCCEEDED(CSurfaceTexture::New(texName, &st));
    return (eobject)st;
}

void SurfaceTextureSetDefaultBufferSize(eobject object, int width, int height)
{
    ((ISurfaceTexture*)object)->SetDefaultBufferSize(width, height);
}

void SurfaceTextureUpdateTexImage(eobject object)
{
    ((ISurfaceTexture*)object)->UpdateTexImage();
}

void SurfaceTextureDetachFromGLContext(eobject object)
{
    ((ISurfaceTexture*)object)->DetachFromGLContext();
}

void SurfaceTextureAttachToGLContext(eobject object, int texName)
{
    ((ISurfaceTexture*)object)->AttachToGLContext(texName);
}

void SurfaceTextureGetTransformMatrix(eobject object, float* mtx)
{
    ALOGD("SurfaceTextureGetTransformMatrix 1");
    assert(mtx != NULL);
    AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(16);
    ALOGD("SurfaceTextureGetTransformMatrix 2");
    ((ISurfaceTexture*)object)->GetTransformMatrix(*array);
    ALOGD("SurfaceTextureGetTransformMatrix 3");
    memcpy(mtx, array->GetPayload(), sizeof(float) * 16);
    ALOGD("SurfaceTextureGetTransformMatrix 4");
}

void SurfaceTextureRelease(eobject object)
{
    ((ISurfaceTexture*)object)->ReleaseResources();
}

int64_t SurfaceTextureGetTimestamp(eobject object)
{
    Int64 ts;
    ((ISurfaceTexture*)object)->GetTimestamp(&ts);
    return ts;
}

eobject CreateSurface(eobject surfaceTexture)
{
    ISurface* sf;
    ASSERT_SUCCEEDED(CSurface::New((ISurfaceTexture*)surfaceTexture, &sf));
    return (eobject)sf;
}

bool SurfaceIsValid(eobject object)
{
    Boolean isValid;
    ((ISurface*)object)->IsValid(&isValid);
    return isValid;
}

int SurfaceGetNativeWindow(eobject object)
{
    assert(object != NULL);

    android::Surface* nsf = NULL;
    ((ISurface*)object)->GetSurface((HANDLE*)&nsf);
    assert(nsf != NULL);

    android::sp<ANativeWindow> nw = nsf;
    nw->incStrong(NULL);
    return (int)nw.get();
}

void AddRefObject(eobject object)
{
    printf("==== Call AddRefObject ====\n");
    ((ISurface*)object)->AddRef();
}

void ReleaseObject(eobject object)
{
    printf("==== Call ReleaseObject ====\n");
    ((ISurface*)object)->Release();
}

IXBMCCallbacks* CreateXBMCCallbacks()
{
    IXBMCCallbacks* callbacks = new IXBMCCallbacks;

    callbacks->mElAudioTrack = new IElAudioTrackInterface;
    callbacks->mElAudioTrack->CreateElAudioTrack = CreateElAudioTrack;
    callbacks->mElAudioTrack->DeleteElAudioTrack = DeleteElAudioTrack;
    callbacks->mElAudioTrack->Play = ElAudioTrack_Play;
    callbacks->mElAudioTrack->Stop = ElAudioTrack_Stop;
    callbacks->mElAudioTrack->Flush = ElAudioTrack_Flush;
    callbacks->mElAudioTrack->Release = ElAudioTrack_Release;
    callbacks->mElAudioTrack->Write = ElAudioTrack_Write;
    callbacks->mElAudioTrack->GetPlayState = ElAudioTrack_GetPlayState;
    callbacks->mElAudioTrack->SetStereoVolume = ElAudioTrack_SetStereoVolume;
    callbacks->mElAudioTrack->GetPlaybackHeadPosition = ElAudioTrack_GetPlaybackHeadPosition;
    callbacks->mElAudioTrack->GetMinBufferSize = ElAudioTrack_GetMinBufferSize;

    callbacks->mElWifi = new IElWifiInterface;
    callbacks->mElWifi->Start = ElWifi_Start;
    callbacks->mElWifi->Stop = ElWifi_Stop;
    callbacks->mElWifi->ScanResults = ElWifi_ScanResults;
    callbacks->mElWifi->AddNetwork = ElWifi_AddNetwork;
    callbacks->mElWifi->SaveConfig = ElWifi_SaveConfig;
    callbacks->mElWifi->ReloadConfig = ElWifi_ReloadConfig;

    callbacks->mEnv = new DroidEnv;
    callbacks->mEnv->CreateSurfaceTexture = CreateSurfaceTexture;
    callbacks->mEnv->SurfaceTextureSetDefaultBufferSize = SurfaceTextureSetDefaultBufferSize;
    callbacks->mEnv->SurfaceTextureUpdateTexImage = SurfaceTextureUpdateTexImage;
    callbacks->mEnv->SurfaceTextureDetachFromGLContext = SurfaceTextureDetachFromGLContext;
    callbacks->mEnv->SurfaceTextureAttachToGLContext = SurfaceTextureAttachToGLContext;
    callbacks->mEnv->SurfaceTextureGetTransformMatrix = SurfaceTextureGetTransformMatrix;
    callbacks->mEnv->SurfaceTextureGetTimestamp = SurfaceTextureGetTimestamp;
    callbacks->mEnv->SurfaceTextureRelease = SurfaceTextureRelease;

    callbacks->mEnv->CreateSurface = CreateSurface;
    callbacks->mEnv->SurfaceIsValid = SurfaceIsValid;
    callbacks->mEnv->SurfaceGetNativeWindow = SurfaceGetNativeWindow;

    callbacks->mEnv->AddRefObject = AddRefObject;
    callbacks->mEnv->ReleaseObject = ReleaseObject;

    callbacks->StartBrowser = StartBrowser;
    callbacks->StartCalculator = StartCalculator;
    callbacks->FinishActivity = FinishActivity;
    callbacks->ConnectEthernet = ConnectEthernet;

    return callbacks;
}
