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

#ifndef __JAVAPROXY_UTIL_H_
#define __JAVAPROXY_UTIL_H_

#include <jni.h>
#include <elastos.h>
#include <Elastos.Droid.Core.h>

using Elastos::Core::ICharSequence;
using Elastos::IO::IFileDescriptor;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::IProfilerInfo;
using Elastos::Droid::App::IResultInfo;
using Elastos::Droid::App::Backup::IRestoreSet;
using Elastos::Droid::AppWidget::IAppWidgetProviderInfo;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContentProviderOperation;
using Elastos::Droid::Content::IContentProviderResult;
using Elastos::Droid::Content::IClipData;
using Elastos::Droid::Content::IClipDataItem;
using Elastos::Droid::Content::IClipDescription;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIIntentSender;
using Elastos::Droid::Content::IIntentReceiver;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Pm::IContainerEncryptionParams;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IProviderInfo;
using Elastos::Droid::Content::Pm::IPackageInfoLite;
using Elastos::Droid::Content::Pm::IPackageStats;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Pm::IVerifierInfo;
using Elastos::Droid::Content::Res::IAssetFileDescriptor;
using Elastos::Droid::Content::Res::ICompatibilityInfo;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Res::IObbInfo;
using Elastos::Droid::Database::ICharArrayBuffer;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::PorterDuffMode;
using Elastos::Droid::Hardware::Display::IWifiDisplay;
using Elastos::Droid::Hardware::Display::IWifiDisplayStatus;
using Elastos::Droid::Hardware::Display::IWifiDisplaySessionInfo;
using Elastos::Droid::Internal::Location::IProviderProperties;
using Elastos::Droid::Internal::Location::IProviderRequest;
using Elastos::Droid::Location::ILocation;
using Elastos::Droid::Location::ILocationRequest;
using Elastos::Droid::Media::IAudioAttributes;
using Elastos::Droid::Media::IRating;
using Elastos::Droid::Os::IBaseBundle;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IDebugMemoryInfo;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IMessenger;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Os::IPersistableBundle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IWorkSource;
using Elastos::Droid::Os::Storage::IStorageVolume;
using Elastos::Droid::Location::ICountry;
using Elastos::Droid::Media::IAudioRoutesInfo;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Net::ILinkAddress;
using Elastos::Droid::Net::IIpConfiguration;
using Elastos::Droid::Net::IStaticIpConfiguration;
using Elastos::Droid::Net::IIpPrefix;
using Elastos::Droid::Net::ILinkProperties;
using Elastos::Droid::Net::IProxyProperties;
using Elastos::Droid::Net::IProxyInfo;
using Elastos::Droid::Net::IRouteInfo;
using Elastos::Droid::Wifi::ISupplicantState;
using Elastos::Droid::Wifi::IWifiConfiguration;
using Elastos::Droid::Wifi::IWifiEnterpriseConfig;
using Elastos::Droid::Wifi::IWifiInfo;
using Elastos::Droid::Wifi::IWifiSsid;
using Elastos::Droid::Wifi::SupplicantState;
using Elastos::Droid::Wifi::P2p::IWifiP2pDevice;
using Elastos::Droid::Wifi::P2p::IWifiP2pDeviceList;
using Elastos::Droid::Wifi::P2p::IWifiP2pGroup;
using Elastos::Droid::Wifi::P2p::IWifiP2pInfo;
using Elastos::Droid::Wifi::P2p::IWifiP2pWfdInfo;
using Elastos::Droid::View::IDragEvent;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IInputChannel;
using Elastos::Droid::View::InputMethod::ICompletionInfo;
using Elastos::Droid::View::InputMethod::ICorrectionInfo;
using Elastos::Droid::View::InputMethod::IEditorInfo;
using Elastos::Droid::View::InputMethod::IExtractedText;
using Elastos::Droid::View::InputMethod::IExtractedTextRequest;
using Elastos::Droid::View::InputMethod::IInputBinding;
using Elastos::Droid::View::TextService::ISentenceSuggestionsInfo;
using Elastos::Droid::View::TextService::ISuggestionsInfo;
using Elastos::Droid::Internal::Net::IVpnProfile;
using Elastos::Droid::Internal::View::IInputBindResult;
using Elastos::Droid::Widget::IRemoteViews;
using Elastos::Net::IInetAddress;
using Elastos::IO::ISerializable;
using Elastos::Security::IPublicKey;
using Elastos::Utility::IBitSet;
using Elastos::Utility::ILocale;
using Elastos::Utility::IHashMap;
using Elastosx::Crypto::Spec::IIvParameterSpec;

#ifndef DUMP_CLSID
#define DUMP_CLSID(CLSID, TAG) \
    do { \
        LOGGERD(TAG, "======== DUMP_CLSID ========\n"); \
        LOGGERD(TAG, "{%p, %p, %p, {%p, %p, %p, %p, %p, %p, %p, %p} }\n", \
                CLSID.mClsid.mData1, CLSID.mClsid.mData2, CLSID.mClsid.mData3, \
                CLSID.mClsid.mData4[0], CLSID.mClsid.mData4[1], \
                CLSID.mClsid.mData4[2], CLSID.mClsid.mData4[3], \
                CLSID.mClsid.mData4[4], CLSID.mClsid.mData4[5], \
                CLSID.mClsid.mData4[6], CLSID.mClsid.mData4[7]); \
        LOGGERD(TAG, "============================\n"); \
    } while(0);

#endif

namespace Elastos {
namespace Droid {
namespace JavaProxy {

class Util
{
public:
    static ECode CheckErrorAndLog(
        /* [in] */ JNIEnv* env,
        /* [in] */ const char* errlog,
        /* [in] */ int line);

    static ECode CheckErrorAndLog(
        /* [in] */ JNIEnv* env,
        /* [in] */ const char* tag,
        /* [in] */ const char* errlog,
        /* [in] */ int line);

    static ECode CheckErrorAndLog(
        /* [in] */ JNIEnv* env,
        /* [in] */ const String& tag,
        /* [in] */ const char* errlog,
        /* [in] */ int line);

    static ECode CheckErrorAndLog(
        /* [in] */ JNIEnv* env,
        /* [in] */ const char* errlog,
        /* [in] */ const char* paramname,
        /* [in] */ const char* tag,
        /* [in] */ Int32 line);

    static String GetJavaToStringResult(
        /* [in] */ JNIEnv* env,
        /* [in] */ jobject obj);

    static jstring ToJavaString(
        /* [in] */ JNIEnv* env,
        /* [in] */ const String& str);

    static jobjectArray ToJavaStringArray(
        /* [in] */ JNIEnv* env,
        /* [in] */ ArrayOf<String>* strArray);

    static jbyteArray ToJavaByteArray(
        /* [in] */ JNIEnv* env,
        /* [in] */ ArrayOf<Byte>* byteArray);

    static jcharArray ToJavaCharArray(
        /* [in] */ JNIEnv* env,
        /* [in] */ ArrayOf<Char32>* charArray);

    static jintArray ToJavaIntArray(
        /* [in] */ JNIEnv* env,
        /* [in] */ ArrayOf<Int32>* intArray);

    static jlongArray ToJavaLongArray(
        /* [in] */ JNIEnv* env,
        /* [in] */ ArrayOf<Int64>* lArray);

    static jfloatArray ToJavaFloatArray(
        /* [in] */ JNIEnv* env,
        /* [in] */ ArrayOf<Float>* floatArray);

    static bool GetElInt32Array(
        /* [in] */ JNIEnv* env,
        /* [in] */ jintArray jiArr,
        /* [out] */ ArrayOf<Int32>** iArr);

    static bool GetElInt64Array(
        /* [in] */ JNIEnv* env,
        /* [in] */ jlongArray jlArr,
        /* [out] */ ArrayOf<Int64>** lArr);

    static bool GetElByteArray(
        /* [in] */ JNIEnv* env,
        /* [in] */ jbyteArray jarray,
        /* [out] */ ArrayOf<Byte>** byteArray);

    static bool GetElStringArray(
        /* [in] */ JNIEnv* env,
        /* [in] */ jobjectArray jarr,
        /* [out] */ ArrayOf<String>** strArray);

    static jobject ToJavaComponentName(
        /* [in] */ JNIEnv* env,
        /* [in] */ IComponentName* componentName);

    static jobject ToJavaIntent(
        /* [in] */ JNIEnv* env,
        /* [in] */ IIntent* intent);

    static jobject ToJavaApplicationInfo(
        /* [in] */ JNIEnv* env,
        /* [in] */ IApplicationInfo* appInfo);

    static jobject ToJavaActivityInfo(
        /* [in] */ JNIEnv* env,
        /* [in] */ IActivityInfo* actInfo);

    static jobject ToJavaBundle(
        /* [in] */ JNIEnv* env,
        /* [in] */ IBundle* bundle);

    static Boolean SetJavaBaseBundle(
        /* [in] */ JNIEnv* env,
        /* [in] */ IBaseBundle* bundle,
        /* [in] */ jclass bundleKlass,
        /* [in] */ jobject jbundle);

    static jobject ToJavaBitmap(
        /* [in] */ JNIEnv* env,
        /* [in] */ IBitmap* bitmap);

    static jobject ToJavaConfiguration(
        /* [in] */ JNIEnv* env,
        /* [in] */ IConfiguration* bundle);

    static jobject ToJavaLocale(
        /* [in] */ JNIEnv* env,
        /* [in] */ ILocale* locale);

    static jobject ToJavaRect(
        /* [in] */ JNIEnv* env,
        /* [in] */ IRect* rect);

    static jobject ToJavaProviderInfo(
        /* [in] */ JNIEnv* env,
        /* [in] */ IProviderInfo* providerInfo);

    static jobject ToJavaUri(
        /* [in] */ JNIEnv* env,
        /* [in] */ IUri* uri);

    static jobject ToJavaClipData(
        /* [in] */ JNIEnv* env,
        /* [in] */ IClipData* clipData);

    static jobject ToJavaInputBindResult(
        /* [in] */ JNIEnv* env,
        /* [in] */ IInputBindResult* result);

    static jobject ToJavaInputChannel(
        /* [in] */ JNIEnv* env,
        /* [in] */ IInputChannel* channel);

    static jobject ToJavaKeyEvent(
        /* [in] */ JNIEnv* env,
        /* [in] */ IKeyEvent* keyEvent);

    static jobject ToJavaServiceInfo(
        /* [in] */ JNIEnv* env,
        /* [in] */ IServiceInfo* sInfo);

    static jobject ToJavaCompatibilityInfo(
        /* [in] */ JNIEnv* env,
        /* [in] */ ICompatibilityInfo* cInfo);

    static jobject ToJavaMessage(
        /* [in] */ JNIEnv* env,
        /* [in] */ IMessage* msg);

    static jobject ToJavaIntentReceiver(
        /* [in] */ JNIEnv* env,
        /* [in] */ IIntentReceiver* receiver);

    static jobject ToJavaLocation(
        /* [in] */ JNIEnv* env,
        /* [in] */ ILocation* location);

    static jobject ToJavaInputBinding(
        /* [in] */ JNIEnv* env,
        /* [in] */ IInputBinding* binding);

    static jobject ToJavaEditorInfo(
        /* [in] */ JNIEnv* env,
        /* [in] */ IEditorInfo* info);

    static jobject ToJavaFileDescriptor(
        /* [in] */ JNIEnv* env,
        /* [in] */ IFileDescriptor* fileDescriptor);

    static jobject ToJavaParcelFileDescriptor(
        /* [in] */ JNIEnv* env,
        /* [in] */ IParcelFileDescriptor* pfd);

    static jobject ToJavaNetworkInfo(
        /* [in] */ JNIEnv* env,
        /* [in] */ INetworkInfo* info);

    static jobject ToJavaWifiInfo(
        /* [in] */ JNIEnv* env,
        /* [in] */ IWifiInfo* info);

    static jobject ToJavaInetAddress(
        /* [in] */ JNIEnv* env,
        /* [in] */ IInetAddress* address);

    static jobject ToJavaInetAddress(
        /* [in] */ JNIEnv* env,
        /* [in] */ Int32 family,
        /* [in] */ Int32 ipaddress,
        /* [in] */ const String& hostName);

    static jobject ToJavaWifiSsid(
        /* [in] */ JNIEnv* env,
        /* [in] */ IWifiSsid* address);

    static jobject ToJavaSupplicantState(
        /* [in] */ JNIEnv* env,
        /* [in] */ SupplicantState state);

    static jobject ToJavaSupplicantState(
        /* [in] */ JNIEnv* env,
        /* [in] */ ISupplicantState* state);

    static jobject ToJavaLinkProperties(
        /* [in] */ JNIEnv* env,
        /* [in] */ ILinkProperties* properties);

    static jobject ToJavaRouteInfo(
        /* [in] */ JNIEnv* env,
        /* [in] */ IRouteInfo* info);

    static jobject ToJavaProxyProperties(
        /* [in] */ JNIEnv* env,
        /* [in] */ IProxyProperties* pproperties);

    static jobject ToJavaProxyInfo(
        /* [in] */ JNIEnv* env,
        /* [in] */ IProxyInfo* proxyInfo);

    static jobject ToJavaExtractedText(
        /* [in] */ JNIEnv* env,
        /* [in] */ IExtractedText* ert);

    static jobject ToJavaExtractedTextRequest(
        /* [in] */ JNIEnv* env,
        /* [in] */ IExtractedTextRequest* ertr);

    static jobject ToJavaCompletionInfo(
        /* [in] */ JNIEnv* env,
        /* [in] */ ICompletionInfo* completion);

    static jobject ToJavaCorrectionInfo(
        /* [in] */ JNIEnv* env,
        /* [in] */ ICorrectionInfo* correction);

    static jobject ToJavaStorageVolume(
        /* [in] */ JNIEnv* env,
        /* [in] */ IStorageVolume* volume);

    static jobject ToJavaUserHandle(
        /* [in] */ JNIEnv* env,
        /* [in] */ IUserHandle* handle);

    static jobject ToJavaContentValues(
        /* [in] */ JNIEnv* env,
        /* [in] */ IContentValues* contentValues);

    static jobject ToJavaDragEvent(
        /* [in] */ JNIEnv* env,
        /* [in] */ IDragEvent* event);

    static jobject ToJavaMotionEvent(
        /* [in] */ JNIEnv* env,
        /* [in] */ IMotionEvent* event);

    static jobject ToJavaPackageStats(
        /* [in] */ JNIEnv* env,
        /* [in] */ IPackageStats* stats);

    static jobject ToJavaHashMap(
        /* [in] */ JNIEnv* env,
        /* [in] */ IHashMap* map);

    static jobject ToJavaVpnProfile(
        /* [in] */ JNIEnv* env,
        /* [in] */ IVpnProfile* profile);

    static jobject ToJavaDebugMemoryInfo(
        /* [in] */ JNIEnv* env,
        /* [in] */ IDebugMemoryInfo* info);

    static jobject ToJavaProviderRequest(
        /* [in] */ JNIEnv* env,
        /* [in] */ IProviderRequest* request);

    static jobject ToJavaLocationRequest(
        /* [in] */ JNIEnv* env,
        /* [in] */ ILocationRequest* request);

    static jobject ToJavaWorkSource(
        /* [in] */ JNIEnv* env,
        /* [in] */ IWorkSource* ws);

    static jobject ToJavaWifiP2pInfo(
        /* [in] */ JNIEnv* env,
        /* [in] */ IWifiP2pInfo* info);

    static jobject ToJavaWifiP2pDevice(
        /* [in] */ JNIEnv* env,
        /* [in] */ IWifiP2pDevice* dev);

    static jobject ToJavaWifiP2pWfdInfo(
        /* [in] */ JNIEnv* env,
        /* [in] */ IWifiP2pWfdInfo* info);

    static jobject ToJavaWifiP2pDeviceList(
        /* [in] */ JNIEnv* env,
        /* [in] */ IWifiP2pDeviceList* info);

    static jobject ToJavaWifiP2pGroup(
        /* [in] */ JNIEnv* env,
        /* [in] */ IWifiP2pGroup* group);

    static jobject ToJavaWifiConfiguration(
        /* [in] */ JNIEnv* env,
        /* [in] */ IWifiConfiguration* config);

    static jobject ToJavaWifiEnterpriseConfig(
        /* [in] */ JNIEnv* env,
        /* [in] */ IWifiEnterpriseConfig* config);

    static jobject ToJavaIpConfiguration(
        /* [in] */ JNIEnv* env,
        /* [in] */ IIpConfiguration* config);

    static jobject ToJavaStaticIpConfiguration(
        /* [in] */ JNIEnv* env,
        /* [in] */ IStaticIpConfiguration* config);

    static jobject ToJavaWifiDisplayStatus(
        /* [in] */ JNIEnv* env,
        /* [in] */ IWifiDisplayStatus* status);

    static jobject ToJavaWifiDisplaySessionInfo(
        /* [in] */ JNIEnv* env,
        /* [in] */ IWifiDisplaySessionInfo* info);

    static jobject ToJavaWifiDisplay(
        /* [in] */ JNIEnv* env,
        /* [in] */ IWifiDisplay* display);

    static jobject ToJavaBitSet(
        /* [in] */ JNIEnv* env,
        /* [in] */ IBitSet* bitSet);

    static jobject ToJavaContainerEncryptionParams(
        /* [in] */ JNIEnv* env,
        /* [in] */ IContainerEncryptionParams* params);

    static jobject ToJavaIvParameterSpec(
        /* [in] */ JNIEnv* env,
        /* [in] */ IIvParameterSpec* spec);

    static jobject ToJavaClipDescription(
        /* [in] */ JNIEnv* env,
        /* [in] */ IClipDescription* clipDesc);

    static jobject ToJavaClipDataItem(
        /* [in] */ JNIEnv* env,
        /* [in] */ IClipDataItem* item);

    static jobject ToJavaLinkAddress(
        /* [in] */ JNIEnv* env,
        /* [in] */ ILinkAddress* info);

    static jobject ToJavaIpPrefix(
        /* [in] */ JNIEnv* env,
        /* [in] */ IIpPrefix* ipPrefix);

    static jobject ToJavaContentProviderOperation(
        /* [in] */ JNIEnv* env,
        /* [in] */ IContentProviderOperation* operation);

    static jobject ToJavaParcelable(
        /* [in] */ JNIEnv* env,
        /* [in] */ IParcelable* parcelable);

    static jobject ToJavaRestoreSet(
        /* [in] */ JNIEnv* env,
        /* [in] */ IRestoreSet* rs);

    static jobject ToJavaCountry(
        /* [in] */ JNIEnv* env,
        /* [in] */ ICountry* country);

    static jobject ElByteArrayToJavaObject(
        /* [in] */ JNIEnv* env,
        /* [in] */ ISerializable* serializable);

    static jint JavaIntegerToInt(
        /* [in] */ JNIEnv* env,
        /* [in] */ jobject jinteger);

    static jobject ToJavaRemoteViews(
        /* [in] */ JNIEnv* env,
        /* [in] */ IRemoteViews* obj);

    static jobject ToJavaPorterDuffMode(
        /* [in] */ JNIEnv* env,
        /* [in] */ PorterDuffMode mode);

    static jobject ToJavaAppWidgetProviderInfo(
        /* [in] */ JNIEnv* env,
        /* [in] */ IAppWidgetProviderInfo* obj);

    static jobject ToJavaPendingIntent(
        /* [in] */ JNIEnv* env,
        /* [in] */ IPendingIntent* pendingIntent);

    static jobject ToJavaIIntentSender(
        /* [in] */ JNIEnv* env,
        /* [in] */ IIIntentSender* isender);

    static jobject ToJavaCharArrayBuffer(
        /* [in] */ JNIEnv* env,
        /* [in] */ ICharArrayBuffer* buffer);

    static jobject ToJavaAudioRoutesInfo(
        /* [in] */ JNIEnv* env,
        /* [in] */ IAudioRoutesInfo* event);

    static jobject ToJavaSuggestionsInfo(
        /* [in] */ JNIEnv* env,
        /* [in] */ ISuggestionsInfo* info);

    static jobject ToJavaSentenceSuggestionsInfo(
        /* [in] */ JNIEnv* env,
        /* [in] */ ISentenceSuggestionsInfo* info);

    static jobject ToJavaMessenger(
        /* [in] */ JNIEnv* env,
        /* [in] */ IMessenger* msgr);

    static jobject ToJavaPersistableBundle(
        /* [in] */ JNIEnv* env,
        /* [in] */ IPersistableBundle* persistableBundle);

    static jobject ToJavaProfilerInfo(
        /* [in] */ JNIEnv* env,
        /* [in] */ IProfilerInfo* profilerInfo);

    static jobject ToJavaResultInfo(
        /* [in] */ JNIEnv* env,
        /* [in] */ IResultInfo* resultInfo);

    static jobject ToJavaRating(
        /* [in] */ JNIEnv* env,
        /* [in] */ IRating* rating);

    static jobject ToJavaAudioAttributes(
        /* [in] */ JNIEnv* env,
        /* [in] */ IAudioAttributes* aa);

    static String GetElString(
        /* [in] */ JNIEnv* env,
        /* [in] */ jstring str);

    static Boolean GetElBitmap(
        /* [in] */ JNIEnv* env,
        /* [in] */ jobject jbitmap,
        /* [out] */ IBitmap** bitmap);

    static Boolean GetElCharSequence(
        /* [in] */ JNIEnv* env,
        /* [in] */ jobject jEditorInfo,
        /* [out] */ ICharSequence** elEditorInfo);

    static Boolean GetElApplicationInfo(
        /* [in] */ JNIEnv* env,
        /* [in] */ jobject jappInfo,
        /* [out] */ IApplicationInfo** appInfo);

    static Boolean GetElComponentName(
        /* [in] */ JNIEnv* env,
        /* [in] */ jobject jcomponentName,
        /* [out] */ IComponentName** elcomponentName);

    static Boolean GetElIntent(
        /* [in] */ JNIEnv* env,
        /* [in] */ jobject jintent,
        /* [out] */ IIntent** elIntent);

    static Boolean GetElUri(
        /* [in] */ JNIEnv* env,
        /* [in] */ jobject jUri,
        /* [out] */ IUri** elUri);

    static Boolean GetElBundle(
        /* [in] */ JNIEnv* env,
        /* [in] */ jobject jbundle,
        /* [out] */ IBundle** bundle);

    static Boolean GetElBundle(
        /* [in] */ JNIEnv* env,
        /* [in] */ jobject jbundle,
        /* [in] */ IBundle* bundle);

    static Boolean GetElProviderInfo(
        /* [in] */ JNIEnv* env,
        /* [in] */ jobject jproviderInfo,
        /* [out] */ IProviderInfo** providerInfo);

    static Boolean GetElRect(
        /* [in] */ JNIEnv* env,
        /* [in] */ jobject jrect,
        /* [out] */ IRect** elRect);

    static Boolean GetElPackageInfoLite(
        /* [in] */ JNIEnv* env,
        /* [in] */ jobject jpkgLite,
        /* [out] */ IPackageInfoLite** pkgLite);

    static Boolean GetElFileDescriptor(
        /* [in] */ JNIEnv* env,
        /* [in] */ jobject jfDescptor,
        /* [out] */ IFileDescriptor** fDescptor);

    static Boolean GetElParcelFileDescriptor(
        /* [in] */ JNIEnv* env,
        /* [in] */ jobject jpFDescptor,
        /* [out] */ IParcelFileDescriptor** pFDescptor);

    static Boolean GetElAssetFileDescriptor(
        /* [in] */ JNIEnv* env,
        /* [in] */ jobject jaFDescptor,
        /* [out] */ IAssetFileDescriptor** aFDescptor);

    static Boolean GetElObbInfo(
        /* [in] */ JNIEnv* env,
        /* [in] */ jobject jobbInfo,
        /* [out] */ IObbInfo** obbInfo);

    static Boolean GetElDebugMemoryInfo(
        /* [in] */ JNIEnv* env,
        /* [in] */ jobject jinfo,
        /* [out] */ IDebugMemoryInfo** info);

    static Boolean GetElDebugMemoryInfo(
        /* [in] */ JNIEnv* env,
        /* [in] */ jobject jinfo,
        /* [in] */ IDebugMemoryInfo* info);

    static Boolean GetElProviderProperties(
        /* [in] */ JNIEnv* env,
        /* [in] */ jobject jproperties,
        /* [out] */ IProviderProperties** properties);

    static Boolean GetElClipData(
        /* [in] */ JNIEnv* env,
        /* [in] */ jobject jclip,
        /* [out] */ IClipData** clip);

    static Boolean GetElClipDataItem(
        /* [in] */ JNIEnv* env,
        /* [in] */ jobject jitem,
        /* [out] */ IClipDataItem** item);

    static Boolean GetElClipDescription(
        /* [in] */ JNIEnv* env,
        /* [in] */ jobject jclipDesc,
        /* [out] */ IClipDescription** clipDesc);

    static Boolean GetElPendingIntent(
        /* [in] */ JNIEnv* env,
        /* [in] */ jobject jpintent,
        /* [out] */ IPendingIntent** pintent);

    static Boolean GetElRemoteViews(
        /* [in] */ JNIEnv* env,
        /* [in] */ jobject jremoteview,
        /* [out] */ IRemoteViews** remoteview);

    static Boolean GetElContentProviderResult(
        /* [in] */ JNIEnv* env,
        /* [in] */ jobject jresult,
        /* [out] */ IContentProviderResult** result);

    static Boolean GetElVerifierInfo(
        /* [in] */ JNIEnv* env,
        /* [in] */ jobject jinfo,
        /* [out] */ IVerifierInfo** info);

    static Boolean GetElPublicKey(
        /* [in] */ JNIEnv* env,
        /* [in] */ jobject jkey,
        /* [out] */ IPublicKey** key);

    static jint GetJavaIntegerField(
        /* [in] */ JNIEnv* env,
        /* [in] */ jclass klass,
        /* [in] */ jobject jobj,
        /* [in] */ const char* fieldName,
        /* [in] */ jint defaultInt,
        /* [in] */ const char* tag);

    static jfloat GetJavafloatField(
        /* [in] */ JNIEnv* env,
        /* [in] */ jclass klass,
        /* [in] */ jobject jobj,
        /* [in] */ const char* fieldName,
        /* [in] */ const char* tag);

    static jdouble GetJavadoubleField(
        /* [in] */ JNIEnv* env,
        /* [in] */ jclass klass,
        /* [in] */ jobject jobj,
        /* [in] */ const char* fieldName,
        /* [in] */ const char* tag);

    static jchar GetJavaCharField(
        /* [in] */ JNIEnv* env,
        /* [in] */ jclass klass,
        /* [in] */ jobject jobj,
        /* [in] */ const char* fieldName,
        /* [in] */ const char* tag);

    static jshort GetJavaShortField(
        /* [in] */ JNIEnv* env,
        /* [in] */ jclass klass,
        /* [in] */ jobject jobj,
        /* [in] */ const char* fieldName,
        /* [in] */ const char* tag);

    static jint GetJavaIntField(
        /* [in] */ JNIEnv* env,
        /* [in] */ jclass klass,
        /* [in] */ jobject jobj,
        /* [in] */ const char* fieldName,
        /* [in] */ const char* tag);

    static jlong GetJavalongField(
        /* [in] */ JNIEnv* env,
        /* [in] */ jobject jobj,
        /* [in] */ const char* fieldName,
        /* [in] */ const char* tag);

    static jbyte GetJavabyteField(
        /* [in] */ JNIEnv* env,
        /* [in] */ jclass klass,
        /* [in] */ jobject jobj,
        /* [in] */ const char* fieldName,
        /* [in] */ const char* tag);

    static jboolean GetJavaBoolField(
        /* [in] */ JNIEnv* env,
        /* [in] */ jobject jobj,
        /* [in] */ const char* fieldName,
        /* [in] */ const char* tag);

    static String GetJavaStringField(
        /* [in] */ JNIEnv* env,
        /* [in] */ jclass klass,
        /* [in] */ jobject jobj,
        /* [in] */ const char* fieldName,
        /* [in] */ const char* tag);

    static bool SetJavaIntegerField(
        /* [in] */ JNIEnv* env,
        /* [in] */ jclass klass,
        /* [in] */ jobject jobj,
        /* [in] */ Int32 jintvalue,
        /* [in] */ const char* fieldName,
        /* [in] */ const char* tag);

    static bool SetJavaIntField(
        /* [in] */ JNIEnv* env,
        /* [in] */ jclass klass,
        /* [in] */ jobject jobj,
        /* [in] */ Int32 jintvalue,
        /* [in] */ const char* fieldName,
        /* [in] */ const char* tag);

    static bool SetJavabyteField(
        /* [in] */ JNIEnv* env,
        /* [in] */ jclass klass,
        /* [in] */ jobject jobj,
        /* [in] */ Byte jbytevalue,
        /* [in] */ const char* fieldName,
        /* [in] */ const char* tag);

    static bool SetJavalongField(
        /* [in] */ JNIEnv* env,
        /* [in] */ jclass klass,
        /* [in] */ jobject jobj,
        /* [in] */ Int64 longValue,
        /* [in] */ const char* fieldName,
        /* [in] */ const char* tag);

    static bool SetJavaBoolField(
        /* [in] */ JNIEnv* env,
        /* [in] */ jclass klass,
        /* [in] */ jobject jobj,
        /* [in] */ Boolean boolValue,
        /* [in] */ const char* fieldName,
        /* [in] */ const char* tag);

    static bool SetJavafloatField(
        /* [in] */ JNIEnv* env,
        /* [in] */ jclass klass,
        /* [in] */ jobject jobj,
        /* [in] */ Float floatValue,
        /* [in] */ const char* fieldName,
        /* [in] */ const char* tag);

    static bool SetJavadoubleField(
        /* [in] */ JNIEnv* env,
        /* [in] */ jclass klass,
        /* [in] */ jobject jobj,
        /* [in] */ Double floatValue,
        /* [in] */ const char* fieldName,
        /* [in] */ const char* tag);

    static bool SetJavaStringField(
        /* [in] */ JNIEnv* env,
        /* [in] */ jclass klass,
        /* [in] */ jobject jobj,
        /* [in] */ const String& strValue,
        /* [in] */ const char* fieldName,
        /* [in] */ const char* tag);

    static String GetClassName(
        /* [in] */ JNIEnv* env,
        /* [in] */ jobject obj);

private:
    static void SetComponentInfo(
        /* [in] */ JNIEnv* env,
        /* [in] */ jclass parentClass,
        /* [in] */ jobject jparent,
        /* [in] */ IComponentInfo* comInfo);

    static void SetPackageItemInfo(
        /* [in] */ JNIEnv* env,
        /* [in] */ jclass parentClass,
        /* [in] */ jobject jparent,
        /* [in] */ IPackageItemInfo* pkgInfo);
};

} // JavaProxy
} // Droid
} // Elastos

#endif // __JAVAPROXY_UTIL_H_
