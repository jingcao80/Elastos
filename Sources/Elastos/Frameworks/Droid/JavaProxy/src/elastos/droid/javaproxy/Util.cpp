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

#include "elastos/droid/javaproxy/Util.h"
#include "elastos/droid/javaproxy/CIParcelableNative.h"
#include "elastos/droid/javaproxy/CISerializableNative.h"
#include "elastos/droid/javaproxy/CIIntentSenderNative.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/droid/text/TextUtils.h>
#include <Elastos.Droid.JavaProxy.h>
#include <input/Input.h>
#include <binder/Parcel.h>
#include <unistd.h>

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.AppWidget.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Text.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.Droid.Wifi.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.Extensions.h"
#include "Elastos.CoreLibrary.Net.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Security.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/net/Uri.h"

#define TAG "Util"

using Elastos::Droid::App::CPendingIntent;
using Elastos::Droid::Os::CBundle;
// using Elastos::Droid::Os::CDebugMemoryInfo;
using Elastos::Droid::Os::CPatternMatcher;
using Elastos::Droid::Os::CParcelFileDescriptor;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IIMessenger;
using Elastos::Droid::Os::IPatternMatcher;
using Elastos::Droid::Net::ECLSID_CStringUri;
using Elastos::Droid::Net::ECLSID_CHierarchicalUri;
using Elastos::Droid::Net::ECLSID_COpaqueUri;
using Elastos::Droid::Net::CStringUri;
using Elastos::Droid::Net::CHierarchicalUri;
using Elastos::Droid::Net::COpaqueUri;
using Elastos::Droid::Net::NetworkInfoState;
using Elastos::Droid::Net::NetworkInfoDetailedState;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Wifi::IWifiConfiguration;
using Elastos::Droid::Wifi::IWifiInfo;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CClipData;
using Elastos::Droid::Content::CClipDataItem;
using Elastos::Droid::Content::CClipDescription;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::CContentProviderResult;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::IIntentShortcutIconResource;
using Elastos::Droid::Content::Pm::CApplicationInfo;
using Elastos::Droid::Content::Pm::CPathPermission;
using Elastos::Droid::Content::Pm::CPackageInfoLite;
using Elastos::Droid::Content::Pm::CProviderInfo;
using Elastos::Droid::Content::Pm::CVerifierInfo;
using Elastos::Droid::Content::Pm::IPathPermission;
using Elastos::Droid::Content::Res::CAssetFileDescriptor;
using Elastos::Droid::Content::Res::CObbInfo;
using Elastos::Droid::Graphics::CBitmap;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Hardware::Display::IWifiDisplayStatus;
using Elastos::Droid::Internal::View::IIInputContext;
using Elastos::Droid::Internal::View::IIInputMethodSession;
using Elastos::Droid::Internal::Location::CProviderProperties;
using Elastos::Droid::Os::ECLSID_CUserHandle;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::InputMethod::IInputConnection;
using Elastos::Droid::Widget::CRemoteViews;
using Elastos::Droid::Widget::CRemoteViewsBitmapCache;
using Elastos::Droid::Widget::IBitmapCache;
using Elastos::Core::IByte;
using Elastos::Core::IString;
using Elastos::Core::IInteger16;
using Elastos::Core::IInteger32;
using Elastos::Core::IInteger64;
using Elastos::Core::IFloat;
using Elastos::Core::IDouble;
using Elastos::Core::IBoolean;
using Elastos::Core::IArrayOf;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::StringUtils;
using Elastos::IO::CFileDescriptor;
using Elastos::IO::ISerializable;
using Elastos::Security::IPublicKey;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::IList;
using Elastos::Utility::ISet;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

static jclass sIllegalArgumentExceptionClass = NULL;
static jclass sUnsupportedOperationExceptionClass = NULL;
static jclass sSQLiteExceptionClass = NULL;

static void InitJavaExceptionClass(
    /* [in] */ JNIEnv* env)
{
    if (sIllegalArgumentExceptionClass == NULL) {
        sIllegalArgumentExceptionClass = (jclass)env->NewGlobalRef(
                env->FindClass("java/lang/IllegalArgumentException"));
    }
    if (sUnsupportedOperationExceptionClass == NULL) {
        sUnsupportedOperationExceptionClass = (jclass)env->NewGlobalRef(
                env->FindClass("java/lang/UnsupportedOperationException"));
    }
    if (sSQLiteExceptionClass == NULL) {
        sSQLiteExceptionClass = (jclass)env->NewGlobalRef(
                env->FindClass("android/database/SQLException"));
    }
}

static ECode JavaException2ECode(
    /* [in] */ JNIEnv* env,
    /* [in] */ jthrowable exception)
{
    if (env->IsInstanceOf(exception, sIllegalArgumentExceptionClass)) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else if (env->IsInstanceOf(exception, sUnsupportedOperationExceptionClass)) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    else if (env->IsInstanceOf(exception, sSQLiteExceptionClass)) {
        return E_SQLITE_EXCEPTION;
    }
    // TODO:
    return E_FAIL;
}

ECode Util::CheckErrorAndLog(
    /* [in] */ JNIEnv* env,
    /* [in] */ const char* errlog,
    /* [in] */ int line)
{
    InitJavaExceptionClass(env);
    if (env->ExceptionCheck() != 0) {
        LOGGERE(TAG, errlog, line);
        env->ExceptionDescribe();
        jthrowable exception = env->ExceptionOccurred();
        ECode ec = JavaException2ECode(env, exception);
        env->ExceptionClear();
        return ec;
    }
    return NOERROR;
}

ECode Util::CheckErrorAndLog(
    /* [in] */ JNIEnv* env,
    /* [in] */ const char* tag,
    /* [in] */ const char* errlog,
    /* [in] */ int line)
{
    InitJavaExceptionClass(env);
    if (env->ExceptionCheck() != 0) {
        LOGGERE(tag, errlog, line);
        env->ExceptionDescribe();
        jthrowable exception = env->ExceptionOccurred();
        ECode ec = JavaException2ECode(env, exception);
        env->ExceptionClear();
        return ec;
    }
    return NOERROR;
}

ECode Util::CheckErrorAndLog(
    /* [in] */ JNIEnv* env,
    /* [in] */ const String& tag,
    /* [in] */ const char* errlog,
    /* [in] */ int line)
{
    return CheckErrorAndLog(env, tag.string(), errlog, line);
}

ECode Util::CheckErrorAndLog(
    /* [in] */ JNIEnv* env,
    /* [in] */ const char* errlog,
    /* [in] */ const char* paramname,
    /* [in] */ const char* tag,
    /* [in] */ Int32 line)
{
    InitJavaExceptionClass(env);
    if (env->ExceptionCheck() != 0) {
        LOGGERE("Util", errlog, paramname, tag, line);
        env->ExceptionDescribe();
        jthrowable exception = env->ExceptionOccurred();
        ECode ec = JavaException2ECode(env, exception);
        env->ExceptionClear();
        return ec;
    }
    return NOERROR;
}

String Util::GetJavaToStringResult(
    /* [in] */ JNIEnv* env,
    /* [in] */ jobject obj)
{
    jclass c = env->FindClass("java/lang/Object");
    CheckErrorAndLog(env, "GetJavaToStringResult", "FindClass: Object", __LINE__);

    jmethodID m = env->GetMethodID(c, "toString", "()Ljava/lang/String;");
    CheckErrorAndLog(env, "GetJavaToStringResult", "GetMethodID: toString", __LINE__);

    jstring jstr = (jstring)env->CallObjectMethod(obj, m);
    CheckErrorAndLog(env, "GetJavaToStringResult", "CallVoidMethod: toString", __LINE__);

    String str = Util::GetElString(env, jstr);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jstr);
    return str;
}

jstring Util::ToJavaString(
    /* [in] */ JNIEnv* env,
    /* [in] */ const String& str)
{
    if (env == NULL) {
        LOGGERE("ToJavaString", "Invalid arguments!");
        return NULL;
    }

    jstring jstr = NULL;

    if (!str.IsNull()) {
        jstr = env->NewStringUTF(str.string());
    }

    return jstr;
}

jobjectArray Util::ToJavaStringArray(
    /* [in] */ JNIEnv* env,
    /* [in] */ ArrayOf<String>* strArray)
{
    if (env == NULL || strArray == NULL) {
        LOGGERW("ToJavaStringArray", "Invalid arguments!");
        return NULL;
    }

    Int32 count = strArray->GetLength();

    jclass stringKlass = env->FindClass("java/lang/String");
    Util::CheckErrorAndLog(env, "ToJavaStringArray", "Fail FindClass: String %d", __LINE__);

    jobjectArray jstrArray = env->NewObjectArray((jsize)count, stringKlass, 0);
    Util::CheckErrorAndLog(env, "ToJavaStringArray", "Fail FindClass: String %d", __LINE__);

    for(Int32 i = 0; i < count; i++ ) {
        jstring str = ToJavaString(env, (*strArray)[i]);
        if (str != NULL) {
            env->SetObjectArrayElement(jstrArray, i, str);
            env->DeleteLocalRef(str);
        }
    }

    env->DeleteLocalRef(stringKlass);

    return jstrArray;
}

jbyteArray Util::ToJavaByteArray(
    /* [in] */ JNIEnv* env,
    /* [in] */ ArrayOf<Byte>* byteArray)
{

    if (env == NULL || byteArray == NULL) {
        LOGGERW("Util", "ToJavaByteArray() Invalid arguments!");
        return NULL;
    }

    Int32 count = byteArray->GetLength();

    jbyteArray jbytearray = env->NewByteArray((jsize)count);
    CheckErrorAndLog(env, "Util", "ToJavaByteArray, NewByteArray failed %d", __LINE__);

    jbyte* byteArrayInd = (jbyte*)byteArray->GetPayload();

    env->SetByteArrayRegion(jbytearray, 0, count, byteArrayInd);
    CheckErrorAndLog(env, "Util", "ToJavaByteArray, SetByteArrayRegion failed %d", __LINE__);

    return jbytearray;
}

jcharArray Util::ToJavaCharArray(
    /* [in] */ JNIEnv* env,
    /* [in] */ ArrayOf<Char32>* charArray)
{

    if (env == NULL || charArray == NULL) {
        LOGGERW("Util", "ToJavaCharArray() Invalid arguments!");
        return NULL;
    }

    Int32 count = charArray->GetLength();

    jcharArray jchararray = env->NewCharArray((jsize)count);
    CheckErrorAndLog(env, "Util", "ToJavaCharArray, NewCharArray failed %d", __LINE__);

    jchar* charArrayInd = (jchar*)charArray->GetPayload();

    env->SetCharArrayRegion(jchararray, 0, count, charArrayInd);
    CheckErrorAndLog(env, "Util", "ToJavaCharArray, SetCharArrayRegion failed %d", __LINE__);

    return jchararray;
}

jintArray Util::ToJavaIntArray(
    /* [in] */ JNIEnv* env,
    /* [in] */ ArrayOf<Int32>* intArray)
{
    if (env == NULL || intArray == NULL) {
        LOGGERW("Util", "ToJavaIntArray() Invalid arguments!");
        return NULL;
    }

    Int32 count = intArray->GetLength();

    jintArray jintArray = env->NewIntArray((jsize)count);
    CheckErrorAndLog(env, "Util", "ToJavaIntArray, NewIntArray failed %d", __LINE__);

    jint* intArrayInd = (jint*)intArray->GetPayload();

    env->SetIntArrayRegion(jintArray, 0, count, intArrayInd);
    CheckErrorAndLog(env, "Util", "ToJavaIntArray, SetIntArrayRegion: failed %d", __LINE__);

    return jintArray;
}

jlongArray Util::ToJavaLongArray(
    /* [in] */ JNIEnv* env,
    /* [in] */ ArrayOf<Int64>* lArray)
{
    if (env == NULL || lArray == NULL) {
        LOGGERW("Util", "ToJavaLongArray() Invalid arguments!");
        return NULL;
    }

    Int32 count = lArray->GetLength();

    jlongArray jlArray = env->NewLongArray((jsize)count);
    CheckErrorAndLog(env, "Util", "ToJavaLongArray, NewLongArray failed %d", __LINE__);

    jlong* lArrayInd = (jlong*)lArray->GetPayload();

    env->SetLongArrayRegion(jlArray, 0, count, lArrayInd);
    CheckErrorAndLog(env, "Util", "ToJavaLongArray, SetLongArrayRegion %d", __LINE__);

    return jlArray;
}

jfloatArray Util::ToJavaFloatArray(
    /* [in] */ JNIEnv* env,
    /* [in] */ ArrayOf<Float>* floatArray)
{
    if (env == NULL || floatArray == NULL) {
        LOGGERW("Util", "ToJavaFloatArray() Invalid arguments!");
        return NULL;
    }

    Int32 count = floatArray->GetLength();

    jfloatArray jfloatArray = env->NewFloatArray((jsize)count);
    CheckErrorAndLog(env, "Util", "ToJavaFloatArray, NewFloatArray failed %d", __LINE__);

    jfloat* floatArrayInd = (jfloat*)floatArray->GetPayload();

    env->SetFloatArrayRegion(jfloatArray, 0, count, floatArrayInd);
    CheckErrorAndLog(env, "Util", "ToJavaFloatArray, SetFloatArrayRegion: failed %d", __LINE__);

    return jfloatArray;
}

bool Util::GetElInt32Array(
    /* [in] */ JNIEnv* env,
    /* [in] */ jintArray jiArr,
    /* [out] */ ArrayOf<Int32>** iArr)
{
    if (env == NULL || jiArr == NULL || iArr == NULL) {
        LOGGERE("Util", "GetElInt32Array() Invalid arguments!");
        *iArr = NULL;
        return FALSE;
    }

    jint size = env->GetArrayLength(jiArr);
    CheckErrorAndLog(env, "Util", "GetElInt32Array, GetArrayLength:(): %d!\n", __LINE__);

    jint* jpayload = (jint*)malloc(size * sizeof(Int32));

    env->GetIntArrayRegion(jiArr, 0, size, jpayload);
    CheckErrorAndLog(env, "Util", "GetElInt32Array, GetLongArrayRegion:(): %d!\n", __LINE__);

    (*iArr) = ArrayOf<Int32>::Alloc(size);
    (*iArr)->Copy((Int32*)jpayload, size);
    (*iArr)->AddRef();

    free(jpayload);
    return TRUE;
}

bool Util::GetElInt64Array(
    /* [in] */ JNIEnv* env,
    /* [in] */ jlongArray jlArr,
    /* [out] */ ArrayOf<Int64>** lArr)
{
    if (env == NULL || jlArr == NULL || lArr == NULL) {
        LOGGERE("Util", "GetElInt64Array() Invalid arguments!");
        *lArr = NULL;
        return FALSE;
    }

    jint size = env->GetArrayLength(jlArr);
    CheckErrorAndLog(env, "Util", "GetElInt64Array, GetArrayLength:(): %d!\n", __LINE__);

    jlong* jpayload = (jlong*)malloc(size * sizeof(Int64));

    env->GetLongArrayRegion(jlArr, 0, size, jpayload);
    CheckErrorAndLog(env, "Util", "GetElInt64Array, GetLongArrayRegion:(): %d!\n", __LINE__);

    (*lArr) = ArrayOf<Int64>::Alloc(size);
    (*lArr)->Copy((Int64*)jpayload, size);
    (*lArr)->AddRef();

    free(jpayload);
    return TRUE;
}

bool Util::GetElByteArray(
    /* [in] */ JNIEnv* env,
    /* [in] */ jbyteArray jarray,
    /* [out] */ ArrayOf<Byte>** byteArray)
{
    if (env == NULL || jarray == NULL || byteArray == NULL) {
        LOGGERE("GetElByteArray", "Invalid arguments!");
        return false;
    }

    jint size = env->GetArrayLength(jarray);
    Util::CheckErrorAndLog(env, "GetElByteArray", "GetArrayLength:(): %d!\n", __LINE__);

    jbyte* jpayload = (jbyte*)malloc(size);
    env->GetByteArrayRegion(jarray, 0, size, jpayload);
    Util::CheckErrorAndLog(env, "GetElByteArray", "GetByteArrayRegion:(): %d!\n", __LINE__);

    (*byteArray) = ArrayOf<Byte>::Alloc(size);
    (*byteArray)->Copy((Byte*)jpayload, size);
    (*byteArray)->AddRef();

    free(jpayload);
    return true;
}

bool Util::GetElStringArray(
    /* [in] */ JNIEnv* env,
    /* [in] */ jobjectArray jarr,
    /* [out] */ ArrayOf<String>** strArray)
{
    if (env == NULL || jarr == NULL || strArray == NULL) {
        ALOGE("GetElStringArray() Invalid arguments!");
        return false;
    }

    int size = env->GetArrayLength(jarr);
    Util::CheckErrorAndLog(env, "GetElStringArray", "GetArrayLength failed: %d!\n", __LINE__);

    AutoPtr<ArrayOf<String> > elStrArr = ArrayOf<String>::Alloc(size);

    for(int i = 0; i < size; i++){
        jstring strTemp = (jstring)env->GetObjectArrayElement(jarr, i);
        Util::CheckErrorAndLog(env, "GetElStringArray", "GetObjectArrayelement failed : %d!\n", __LINE__);
        elStrArr->Set(i, Util::GetElString(env, strTemp));
        env->DeleteLocalRef(strTemp);
    }

    *strArray = elStrArr.Get();
    (*strArray)->AddRef();
    return true;
}

jobject Util::ToJavaComponentName(
    /* [in] */ JNIEnv* env,
    /* [in] */ IComponentName* componentName)
{
    if (env == NULL || componentName == NULL) {
        LOGGERE("ToJavaComponentName", "Invalid arguments!");
        return NULL;
    }

    jobject jcomponentName = NULL;

    jclass componentNameKlass = env->FindClass("android/content/ComponentName");
    Util::CheckErrorAndLog(env, "ToJavaComponentName", "Fail FindClass: ComponentName %d", __LINE__);

    jmethodID m = env->GetMethodID(componentNameKlass, "<init>", "(Ljava/lang/String;Ljava/lang/String;)V");
    Util::CheckErrorAndLog(env, "ToJavaComponentName", "Fail GetMethodID: componentNameKlass %d", __LINE__);

    String packageName;
    String className;
    componentName->GetPackageName(&packageName);
    componentName->GetClassName(&className);
    jstring jPackageName = Util::ToJavaString(env, packageName);
    jstring jClassName = Util::ToJavaString(env, className);

    jcomponentName = env->NewObject(componentNameKlass, m, jPackageName, jClassName);
    Util::CheckErrorAndLog(env, "ToJavaComponentName", "Fail NewObject: componentNameKlass %d", __LINE__);

    env->DeleteLocalRef(componentNameKlass);
    env->DeleteLocalRef(jPackageName);
    env->DeleteLocalRef(jClassName);

    return jcomponentName;
}

jobject Util::ToJavaIntent(
    /* [in] */ JNIEnv* env,
    /* [in] */ IIntent* intent)
{
    if (env == NULL || intent == NULL) {
        LOGGERE("ToJavaIntent", "Invalid arguments!");
        return NULL;
    }

    jobject jIntent = NULL;

    jclass intentKlass = env->FindClass("android/content/Intent");
    Util::CheckErrorAndLog(env, "ToJavaIntent", "Fail FindClass: Intent %d", __LINE__);

    jmethodID m = env->GetMethodID(intentKlass, "<init>", "()V");
    Util::CheckErrorAndLog(env, "ToJavaIntent", "Fail GetMethodID: intentKlass %d", __LINE__);

    jIntent = env->NewObject(intentKlass, m);
    Util::CheckErrorAndLog(env, "ToJavaIntent", "Fail NewObject: intentKlass %d", __LINE__);

    String mAction;
    intent->GetAction(&mAction);
    if (!mAction.IsNull()) {
        jfieldID f = env->GetFieldID(intentKlass, "mAction", "Ljava/lang/String;");
        Util::CheckErrorAndLog(env, "ToJavaIntent", "GetFieldID: mAction %d", __LINE__);

        jstring jmAction = Util::ToJavaString(env, mAction);
        env->SetObjectField(jIntent, f, jmAction);
        Util::CheckErrorAndLog(env, "ToJavaIntent", "SetObjectField: SetObjectField %d", __LINE__);
        env->DeleteLocalRef(jmAction);
    }

    AutoPtr<IUri> mData;
    intent->GetData((IUri**)&mData);
    if (mData != NULL)  {
        jobject jmData = Util::ToJavaUri(env, mData);
        jfieldID f = env->GetFieldID(intentKlass, "mData", "Landroid/net/Uri;");
        Util::CheckErrorAndLog(env, "ToJavaIntent", "GetFieldID: mData %d", __LINE__);

        env->SetObjectField(jIntent, f, jmData);
        Util::CheckErrorAndLog(env, "ToJavaIntent", "SetObjectField: jdata %d", __LINE__);

        env->DeleteLocalRef(jmData);
    }

    String mType;
    intent->GetType(&mType);
    if (!mType.IsNull()) {
        jfieldID f = env->GetFieldID(intentKlass, "mType", "Ljava/lang/String;");
        Util::CheckErrorAndLog(env, "ToJavaIntent", "GetFieldID: mType %d", __LINE__);

        jobject jmType = Util::ToJavaString(env, mType);
        env->SetObjectField(jIntent, f, jmType);
        Util::CheckErrorAndLog(env, "ToJavaIntent", "SetObjectField: jtype %d", __LINE__);
        env->DeleteLocalRef(jmType);
    }

    Int32 tempInt;
    intent->GetFlags(&tempInt);
    jfieldID f = env->GetFieldID(intentKlass, "mFlags", "I");
    Util::CheckErrorAndLog(env, "ToJavaIntent", "GetFieldID: mFlags %d", __LINE__);

    env->SetIntField(jIntent, f, (jint)tempInt);
    Util::CheckErrorAndLog(env, "ToJavaIntent", "SetIntField: mFlags %d", __LINE__);

    String mPackage;
    intent->GetPackage(&mPackage);
    if (!mPackage.IsNull())  {
        jfieldID f = env->GetFieldID(intentKlass, "mPackage", "Ljava/lang/String;");
        Util::CheckErrorAndLog(env, "ToJavaIntent", "GetFieldID: mPackage %d", __LINE__);

        jobject jmPackage = Util::ToJavaString(env, mPackage);
        env->SetObjectField(jIntent, f, jmPackage);
        Util::CheckErrorAndLog(env, "ToJavaIntent", "SetObjectField: jmPackage %d", __LINE__);
        env->DeleteLocalRef(jmPackage);
    }

    AutoPtr<IComponentName> componentName;
    intent->GetComponent((IComponentName**)&componentName);
    if (componentName != NULL) {
        jobject jComponentName = Util::ToJavaComponentName(env, componentName);
        if (jComponentName != NULL) {
            jfieldID f = env->GetFieldID(intentKlass, "mComponent", "Landroid/content/ComponentName;");
            Util::CheckErrorAndLog(env, "ToJavaIntent", "Fail GetFieldID: mComponent %d", __LINE__);

            env->SetObjectField(jIntent, f, jComponentName);
            Util::CheckErrorAndLog(env, "ToJavaIntent", "Fail SetObjectField: jComponent %d", __LINE__);
            env->DeleteLocalRef(jComponentName);
        }
        else {
            LOGGERD("ToJavaIntent", "Error: jComponentName is NULL!");
        }
    }

    AutoPtr<IRect> mSourceBounds;
    intent->GetSourceBounds((IRect**)&mSourceBounds);
    if (mSourceBounds != NULL) {
        jobject jmSourceBounds = Util::ToJavaRect(env, mSourceBounds);
        if (jmSourceBounds != NULL) {
            jfieldID f = env->GetFieldID(intentKlass, "mSourceBounds", "Landroid/graphics/Rect;");
            Util::CheckErrorAndLog(env, "ToJavaIntent", "GetFieldID: mSourceBounds %d", __LINE__);

            env->SetObjectField(jIntent, f, jmSourceBounds);
            Util::CheckErrorAndLog(env, "ToJavaIntent", "SetObjectField jmSourceBounds %d", __LINE__);
            env->DeleteLocalRef(jmSourceBounds);
        }
        else {
            LOGGERD("ToJavaIntent", "Error: jmSourceBounds is NULL!");
        }
    }

    AutoPtr<ArrayOf<String> > categories;
    intent->GetCategories((ArrayOf<String>**)&categories);
    if (categories != NULL) {
        Int32 count = categories->GetLength();
        if (count > 0) {
            jmethodID m = env->GetMethodID(intentKlass, "addCategory", "(Ljava/lang/String;)Landroid/content/Intent;");
            Util::CheckErrorAndLog(env, "ToJavaIntent", "Fail GetMethodID: size %d", __LINE__);

            for (int i = 0; i < count; i++) {
                jstring jcategorie = Util::ToJavaString(env, (*categories)[i]);
                jobject jres = env->CallObjectMethod(jIntent, m, jcategorie);
                Util::CheckErrorAndLog(env, "ToJavaIntent", "Fail CallObjectMethod: addCategory %d", __LINE__);
                env->DeleteLocalRef(jcategorie);
                env->DeleteLocalRef(jres);
            }
        }
    }

    AutoPtr<IIntent> selector;
    intent->GetSelector((IIntent**)&selector);
    if (selector != NULL) {
        jobject jselector = Util::ToJavaIntent(env, selector);
        if (jselector != NULL) {
            jfieldID f = env->GetFieldID(intentKlass, "mSelector", "Landroid/content/Intent;");
            Util::CheckErrorAndLog(env, "ToJavaIntent", "GetFieldID: mSelector %d", __LINE__);

            env->SetObjectField(jIntent, f, jselector);
            Util::CheckErrorAndLog(env, "ToJavaIntent", "SetObjectField jselector %d", __LINE__);
            env->DeleteLocalRef(jselector);
        }
        else {
            LOGGERD("ToJavaIntent", "Error: jselector is NULL!");
        }
    }

    AutoPtr<IClipData> clipData;
    intent->GetClipData((IClipData**)&clipData);
    if (clipData != NULL) {
        jobject jclipData = Util::ToJavaClipData(env, clipData);
        if (jclipData != NULL) {
            jfieldID f = env->GetFieldID(intentKlass, "mClipData", "Landroid/content/ClipData;");
            Util::CheckErrorAndLog(env, "ToJavaIntent", "GetFieldID: mClipData %d", __LINE__);
            env->SetObjectField(jIntent, f, jclipData);
            Util::CheckErrorAndLog(env, "ToJavaIntent", "SetObjectField jclipData %d", __LINE__);
            env->DeleteLocalRef(jclipData);
        }
        else {
            LOGGERD("ToJavaIntent", "Error: jclipData is NULL!");
        }
    }

    Int32 contentUserHint;
    intent->GetContentUserHint(&contentUserHint);
    f = env->GetFieldID(intentKlass, "mContentUserHint", "I");
    Util::CheckErrorAndLog(env, "ToJavaIntent", "GetFieldID: mContentUserHint %d", __LINE__);

    env->SetIntField(jIntent, f, (jint)contentUserHint);
    Util::CheckErrorAndLog(env, "ToJavaIntent", "SetIntField: mContentUserHint %d", __LINE__);

    AutoPtr<IBundle> extras;
    intent->GetExtras((IBundle**)&extras);
    if (extras != NULL) {
        jobject jExtras = Util::ToJavaBundle(env, extras);
        if (jExtras != NULL) {
            jfieldID f = env->GetFieldID(intentKlass, "mExtras", "Landroid/os/Bundle;");
            Util::CheckErrorAndLog(env, "ToJavaIntent", "Fail GetFieldID: mExtras %d", __LINE__);
            env->SetObjectField(jIntent, f, jExtras);
            Util::CheckErrorAndLog(env, "ToJavaIntent", "Fail SetObjectField: jExtras %d", __LINE__);
            env->DeleteLocalRef(jExtras);
        }
        else {
            LOGGERD("ToJavaIntent", "Error: jExtras is NULL!");
        }
    }

    env->DeleteLocalRef(intentKlass);

    return jIntent;
}

jobject Util::ToJavaApplicationInfo(
    /* [in] */ JNIEnv* env,
    /* [in] */ IApplicationInfo* appInfo)
{
    if (env == NULL || appInfo == NULL) {
        LOGGERE("ToJavaApplicationInfo", "Invalid arguments!");
        return NULL;
    }

    jobject jAppInfo = NULL;

    jclass appInfoKlass = env->FindClass("android/content/pm/ApplicationInfo");
    Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail FindClass: ApplicationInfo %d", __LINE__);

    jmethodID m = env->GetMethodID(appInfoKlass, "<init>", "()V");
    Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail GetMethodID: appInfoKlass %d", __LINE__);

    jAppInfo = env->NewObject(appInfoKlass, m);
    Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail NewObject: appInfoKlass %d", __LINE__);


    String taskAffinity;
    appInfo->GetTaskAffinity(&taskAffinity);
    jstring jtaskAffinity = Util::ToJavaString(env, taskAffinity);
    if (jtaskAffinity != NULL) {
        jfieldID f = env->GetFieldID(appInfoKlass, "taskAffinity", "Ljava/lang/String;");
        Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail GetFieldID: taskAffinity %d", __LINE__);

        env->SetObjectField(jAppInfo, f, jtaskAffinity);
        Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail SetIntField: jtaskAffinity %d", __LINE__);
        env->DeleteLocalRef(jtaskAffinity);
    }

    String permission;
    appInfo->GetPermission(&permission);
    jstring jpermission = Util::ToJavaString(env, permission);
    if (jpermission != NULL) {
        jfieldID f = env->GetFieldID(appInfoKlass, "permission", "Ljava/lang/String;");
        Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail GetFieldID: permission %d", __LINE__);

        env->SetObjectField(jAppInfo, f, jpermission);
        Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail SetIntField: jpermission %d", __LINE__);
        env->DeleteLocalRef(jpermission);
    }

    String processName;
    appInfo->GetProcessName(&processName);
    jstring jprocessName = Util::ToJavaString(env, processName);
    if (jprocessName != NULL) {
        jfieldID f = env->GetFieldID(appInfoKlass, "processName", "Ljava/lang/String;");
        Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail GetFieldID: processName %d", __LINE__);

        env->SetObjectField(jAppInfo, f, jprocessName);
        Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail SetIntField: jprocessName %d", __LINE__);
        env->DeleteLocalRef(jprocessName);
    }

    String className;
    appInfo->GetClassName(&className);
    jstring jclassName = Util::ToJavaString(env, className);
    if (jclassName != NULL) {
        jfieldID f = env->GetFieldID(appInfoKlass, "className", "Ljava/lang/String;");
        Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail GetFieldID: className %d", __LINE__);

        env->SetObjectField(jAppInfo, f, jclassName);
        Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail SetIntField: jclassName %d", __LINE__);
        env->DeleteLocalRef(jclassName);
    }

    Int32 tempInt;
    appInfo->GetTheme(&tempInt);
    jfieldID f = env->GetFieldID(appInfoKlass, "theme", "I");
    Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail GetFieldID: theme %d", __LINE__);

    env->SetIntField(jAppInfo, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail SetIntField: theme %d", __LINE__);

    appInfo->GetFlags(&tempInt);
    f = env->GetFieldID(appInfoKlass, "flags", "I");
    Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail GetFieldID: flags %d", __LINE__);

    env->SetIntField(jAppInfo, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail SetIntField: flags %d", __LINE__);

    appInfo->GetRequiresSmallestWidthDp(&tempInt);
    f = env->GetFieldID(appInfoKlass, "requiresSmallestWidthDp", "I");
    Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail GetFieldID: requiresSmallestWidthDp %d", __LINE__);

    env->SetIntField(jAppInfo, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail SetIntField: requiresSmallestWidthDp %d", __LINE__);

    appInfo->GetCompatibleWidthLimitDp(&tempInt);
    f = env->GetFieldID(appInfoKlass, "compatibleWidthLimitDp", "I");
    Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail GetFieldID: compatibleWidthLimitDp %d", __LINE__);

    env->SetIntField(jAppInfo, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail SetIntField: compatibleWidthLimitDp  %d", __LINE__);

    appInfo->GetCompatibleWidthLimitDp(&tempInt);
    f = env->GetFieldID(appInfoKlass, "compatibleWidthLimitDp", "I");
    Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail GetFieldID: compatibleWidthLimitDp %d", __LINE__);

    env->SetIntField(jAppInfo, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail SetIntField: compatibleWidthLimitDp %d", __LINE__);

    appInfo->GetLargestWidthLimitDp(&tempInt);
    f = env->GetFieldID(appInfoKlass, "largestWidthLimitDp", "I");
    Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail GetFieldID: largestWidthLimitDp %d", __LINE__);

    env->SetIntField(jAppInfo, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail SetIntField: largestWidthLimitDp %d", __LINE__);

    String scanSourceDir;
    appInfo->GetScanSourceDir(&scanSourceDir);
    jstring jscanSourceDir = Util::ToJavaString(env, scanSourceDir);
    if (jscanSourceDir != NULL) {
        jfieldID f = env->GetFieldID(appInfoKlass, "scanSourceDir", "Ljava/lang/String;");
        Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail GetFieldID: scanSourceDir %d", __LINE__);

        env->SetObjectField(jAppInfo, f, jscanSourceDir);
        Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail SetIntField: jscanSourceDir %d", __LINE__);
        env->DeleteLocalRef(jscanSourceDir);
    }

    String scanPublicSourceDir;
    appInfo->GetScanPublicSourceDir(&scanPublicSourceDir);
    jstring jscanPublicSourceDir = Util::ToJavaString(env, scanPublicSourceDir);
    if (jscanPublicSourceDir != NULL) {
        jfieldID f = env->GetFieldID(appInfoKlass, "scanPublicSourceDir", "Ljava/lang/String;");
        Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail GetFieldID: scanPublicSourceDir %d", __LINE__);

        env->SetObjectField(jAppInfo, f, jscanPublicSourceDir);
        Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail SetIntField: jscanPublicSourceDir %d", __LINE__);
        env->DeleteLocalRef(jscanPublicSourceDir);
    }

    String sourceDir;
    appInfo->GetSourceDir(&sourceDir);
    jstring jsourceDir = Util::ToJavaString(env, sourceDir);
    if (jsourceDir != NULL) {
        jfieldID f = env->GetFieldID(appInfoKlass, "sourceDir", "Ljava/lang/String;");
        Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail GetFieldID: sourceDir %d", __LINE__);

        env->SetObjectField(jAppInfo, f, jsourceDir);
        Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail SetIntField: jsourceDir %d", __LINE__);
        env->DeleteLocalRef(jsourceDir);
    }

    String publicSourceDir;
    appInfo->GetPublicSourceDir(&publicSourceDir);
    jstring jpublicSourceDir = Util::ToJavaString(env, publicSourceDir);
    if (jpublicSourceDir != NULL) {
        jfieldID f = env->GetFieldID(appInfoKlass, "publicSourceDir", "Ljava/lang/String;");
        Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail GetFieldID: publicSourceDir %d", __LINE__);

        env->SetObjectField(jAppInfo, f, jpublicSourceDir);
        Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail SetIntField: jpublicSourceDir %d", __LINE__);
        env->DeleteLocalRef(jpublicSourceDir);
    }

    AutoPtr<ArrayOf<String> > splitSourceDirs;
    appInfo->GetSplitSourceDirs((ArrayOf<String>**)&splitSourceDirs);
    if (splitSourceDirs != NULL) {
        jobjectArray jsplitSourceDirs = Util::ToJavaStringArray(env, splitSourceDirs);
        if (jsplitSourceDirs != NULL) {
            jfieldID f = env->GetFieldID(appInfoKlass, "splitSourceDirs", "[Ljava/lang/String;");
            Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail GetFieldID: splitSourceDirs %d", __LINE__);

            env->SetObjectField(jAppInfo, f, jsplitSourceDirs);
            Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail SetIntField: jsplitSourceDirs %d", __LINE__);
            env->DeleteLocalRef(jsplitSourceDirs);
        }
    }

    AutoPtr<ArrayOf<String> > splitPublicSourceDirs;
    appInfo->GetSplitPublicSourceDirs((ArrayOf<String>**)&splitPublicSourceDirs);
    if (splitPublicSourceDirs != NULL) {
        jobjectArray jsplitPublicSourceDirs = Util::ToJavaStringArray(env, splitPublicSourceDirs);
        if (jsplitPublicSourceDirs != NULL) {
            jfieldID f = env->GetFieldID(appInfoKlass, "splitPublicSourceDirs", "[Ljava/lang/String;");
            Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail GetFieldID: splitPublicSourceDirs %d", __LINE__);

            env->SetObjectField(jAppInfo, f, jsplitPublicSourceDirs);
            Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail SetIntField: jsplitPublicSourceDirs %d", __LINE__);
            env->DeleteLocalRef(jsplitPublicSourceDirs);
        }
    }

    String nativeLibraryDir;
    appInfo->GetNativeLibraryDir(&nativeLibraryDir);
    jstring jnativeLibraryDir = Util::ToJavaString(env, nativeLibraryDir);
    if (jnativeLibraryDir != NULL) {
        jfieldID f = env->GetFieldID(appInfoKlass, "nativeLibraryDir", "Ljava/lang/String;");
        Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail GetFieldID: nativeLibraryDir %d", __LINE__);

        env->SetObjectField(jAppInfo, f, jnativeLibraryDir);
        Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail SetIntField: jnativeLibraryDir %d", __LINE__);
        env->DeleteLocalRef(jnativeLibraryDir);
    }

    String secondaryNativeLibraryDir;
    appInfo->GetSecondaryNativeLibraryDir(&secondaryNativeLibraryDir);
    jstring jsecondaryNativeLibraryDir = Util::ToJavaString(env, secondaryNativeLibraryDir);
    if (jsecondaryNativeLibraryDir != NULL) {
        jfieldID f = env->GetFieldID(appInfoKlass, "secondaryNativeLibraryDir", "Ljava/lang/String;");
        Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail GetFieldID: secondaryNativeLibraryDir %d", __LINE__);

        env->SetObjectField(jAppInfo, f, jsecondaryNativeLibraryDir);
        Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail SetIntField: jsecondaryNativeLibraryDir %d", __LINE__);
        env->DeleteLocalRef(jsecondaryNativeLibraryDir);
    }

    String nativeLibraryRootDir;
    appInfo->GetNativeLibraryRootDir(&nativeLibraryRootDir);
    jstring jnativeLibraryRootDir = Util::ToJavaString(env, nativeLibraryRootDir);
    if (jnativeLibraryRootDir != NULL) {
        jfieldID f = env->GetFieldID(appInfoKlass, "nativeLibraryRootDir", "Ljava/lang/String;");
        Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail GetFieldID: nativeLibraryRootDir %d", __LINE__);

        env->SetObjectField(jAppInfo, f, jnativeLibraryRootDir);
        Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail SetIntField: jnativeLibraryRootDir %d", __LINE__);
        env->DeleteLocalRef(jnativeLibraryRootDir);
    }

    Boolean nativeLibraryRootRequiresIsa;
    appInfo->GetNativeLibraryRootRequiresIsa(&nativeLibraryRootRequiresIsa);
    f = env->GetFieldID(appInfoKlass, "nativeLibraryRootRequiresIsa", "Z");
    Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail GetFieldID: nativeLibraryRootRequiresIsa %d", __LINE__);

    env->SetBooleanField(jAppInfo, f, nativeLibraryRootRequiresIsa);
    Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail SetIntField: nativeLibraryRootRequiresIsa %d", __LINE__);

    String primaryCpuAbi;
    appInfo->GetPrimaryCpuAbi(&primaryCpuAbi);
    jstring jprimaryCpuAbi = Util::ToJavaString(env, primaryCpuAbi);
    if (jprimaryCpuAbi != NULL) {
        jfieldID f = env->GetFieldID(appInfoKlass, "primaryCpuAbi", "Ljava/lang/String;");
        Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail GetFieldID: primaryCpuAbi %d", __LINE__);

        env->SetObjectField(jAppInfo, f, jprimaryCpuAbi);
        Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail SetIntField: jprimaryCpuAbi %d", __LINE__);
        env->DeleteLocalRef(jprimaryCpuAbi);
    }

    String secondaryCpuAbi;
    appInfo->GetSecondaryCpuAbi(&secondaryCpuAbi);
    jstring jsecondaryCpuAbi = Util::ToJavaString(env, secondaryCpuAbi);
    if (jsecondaryCpuAbi != NULL) {
        jfieldID f = env->GetFieldID(appInfoKlass, "secondaryCpuAbi", "Ljava/lang/String;");
        Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail GetFieldID: secondaryCpuAbi %d", __LINE__);

        env->SetObjectField(jAppInfo, f, jsecondaryCpuAbi);
        Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail SetIntField: jsecondaryCpuAbi %d", __LINE__);
        env->DeleteLocalRef(jsecondaryCpuAbi);
    }

    AutoPtr<ArrayOf<String> > resourceDirs;
    appInfo->GetResourceDirs((ArrayOf<String>**)&resourceDirs);
    if (resourceDirs != NULL) {
        jobjectArray jresourceDirs = Util::ToJavaStringArray(env, resourceDirs);
        if (jresourceDirs != NULL) {
            jfieldID f = env->GetFieldID(appInfoKlass, "resourceDirs", "[Ljava/lang/String;");
            Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail GetFieldID: resourceDirs %d", __LINE__);

            env->SetObjectField(jAppInfo, f, jresourceDirs);
            Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail SetIntField: jresourceDirs %d", __LINE__);
            env->DeleteLocalRef(jresourceDirs);
        }
    }

    String seinfo;
    appInfo->GetSeinfo(&seinfo);
    jstring jseinfo = Util::ToJavaString(env, seinfo);
    if (jseinfo != NULL) {
        jfieldID f = env->GetFieldID(appInfoKlass, "seinfo", "Ljava/lang/String;");
        Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail GetFieldID: seinfo %d", __LINE__);

        env->SetObjectField(jAppInfo, f, jseinfo);
        Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail SetIntField: jseinfo %d", __LINE__);
        env->DeleteLocalRef(jseinfo);
    }

    AutoPtr<ArrayOf<String> > sharedLibraryFiles;
    appInfo->GetSharedLibraryFiles((ArrayOf<String>**)&sharedLibraryFiles);
    if (sharedLibraryFiles != NULL) {
        jobjectArray jsharedLibraryFiles = Util::ToJavaStringArray(env, sharedLibraryFiles);
        if (jsharedLibraryFiles != NULL) {
            jfieldID f = env->GetFieldID(appInfoKlass, "sharedLibraryFiles", "[Ljava/lang/String;");
            Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail GetFieldID: sharedLibraryFiles %d", __LINE__);

            env->SetObjectField(jAppInfo, f, jsharedLibraryFiles);
            Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail SetIntField: jsharedLibraryFiles %d", __LINE__);
            env->DeleteLocalRef(jsharedLibraryFiles);
        }
    }

    String dataDir;
    appInfo->GetDataDir(&dataDir);
    jstring jdataDir = Util::ToJavaString(env, dataDir);
    if (jdataDir != NULL) {
        jfieldID f = env->GetFieldID(appInfoKlass, "dataDir", "Ljava/lang/String;");
        Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail GetFieldID: dataDir %d", __LINE__);

        env->SetObjectField(jAppInfo, f, jdataDir);
        Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail SetIntField: jdataDir %d", __LINE__);
        env->DeleteLocalRef(jdataDir);
    }

    appInfo->GetUid(&tempInt);
    f = env->GetFieldID(appInfoKlass, "uid", "I");
    Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail GetFieldID: uid %d", __LINE__);

    env->SetIntField(jAppInfo, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail SetIntField: uid %d", __LINE__);

    appInfo->GetTargetSdkVersion(&tempInt);
    f = env->GetFieldID(appInfoKlass, "targetSdkVersion", "I");
    Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail GetFieldID: targetSdkVersion %d", __LINE__);

    env->SetIntField(jAppInfo, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail SetIntField: targetSdkVersion %d", __LINE__);

    appInfo->GetVersionCode(&tempInt);
    f = env->GetFieldID(appInfoKlass, "versionCode", "I");
    Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail GetFieldID: versionCode %d", __LINE__);

    env->SetIntField(jAppInfo, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail SetIntField: versionCode %d", __LINE__);

    Boolean enabled;
    appInfo->GetEnabled(&enabled);
    f = env->GetFieldID(appInfoKlass, "enabled", "Z");
    Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail GetFieldID: enabled %d", __LINE__);

    env->SetBooleanField(jAppInfo, f, enabled);
    Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail SetIntField: enabled %d", __LINE__);

    appInfo->GetEnabledSetting(&tempInt);
    f = env->GetFieldID(appInfoKlass, "enabledSetting", "I");
    Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail GetFieldID: enabledSetting %d", __LINE__);

    env->SetIntField(jAppInfo, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail SetIntField: enabledSetting %d", __LINE__);

    appInfo->GetInstallLocation(&tempInt);
    f = env->GetFieldID(appInfoKlass, "installLocation", "I");
    Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail GetFieldID: installLocation %d", __LINE__);

    env->SetIntField(jAppInfo, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail SetIntField: installLocation %d", __LINE__);

    String manageSpaceActivityName;
    appInfo->GetManageSpaceActivityName(&manageSpaceActivityName);
    jstring jmanageSpaceActivityName = Util::ToJavaString(env, manageSpaceActivityName);
    if (jmanageSpaceActivityName != NULL) {
        jfieldID f = env->GetFieldID(appInfoKlass, "manageSpaceActivityName", "Ljava/lang/String;");
        Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail GetFieldID: manageSpaceActivityName %d", __LINE__);

        env->SetObjectField(jAppInfo, f, jmanageSpaceActivityName);
        Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail SetIntField: jmanageSpaceActivityName %d", __LINE__);
        env->DeleteLocalRef(jmanageSpaceActivityName);
    }

    String backupAgentName;
    appInfo->GetBackupAgentName(&backupAgentName);
    jstring jbackupAgentName = Util::ToJavaString(env, backupAgentName);
    if (jbackupAgentName != NULL) {
        jfieldID f = env->GetFieldID(appInfoKlass, "backupAgentName", "Ljava/lang/String;");
        Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail GetFieldID: backupAgentName %d", __LINE__);

        env->SetObjectField(jAppInfo, f, jbackupAgentName);
        Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail SetIntField: jbackupAgentName %d", __LINE__);
        env->DeleteLocalRef(jbackupAgentName);
    }

    appInfo->GetDescriptionRes(&tempInt);
    f = env->GetFieldID(appInfoKlass, "descriptionRes", "I");
    Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail GetFieldID: descriptionRes %d", __LINE__);

    env->SetIntField(jAppInfo, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail SetIntField: descriptionRes %d", __LINE__);

    appInfo->GetUiOptions(&tempInt);
    f = env->GetFieldID(appInfoKlass, "uiOptions", "I");
    Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail GetFieldID: uiOptions %d", __LINE__);

    env->SetIntField(jAppInfo, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail SetIntField: uiOptions %d", __LINE__);

    Boolean protect;
    appInfo->GetProtect(&protect);
    f = env->GetFieldID(appInfoKlass, "protect", "Z");
    Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail GetFieldID: protect %d", __LINE__);

    env->SetBooleanField(jAppInfo, f, protect);
    Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail SetIntField: protect %d", __LINE__);

    Boolean isThemeable;
    appInfo->GetIsThemeable(&isThemeable);
    f = env->GetFieldID(appInfoKlass, "isThemeable", "Z");
    Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail GetFieldID: isThemeable %d", __LINE__);

    env->SetBooleanField(jAppInfo, f, isThemeable);
    Util::CheckErrorAndLog(env, "ToJavaApplicationInfo", "Fail SetIntField: isThemeable %d", __LINE__);

    // ----------------------------------- PackageItemInfo ---------------------------------------------------
    AutoPtr<IPackageItemInfo> pkgInfo = IPackageItemInfo::Probe(appInfo);
    SetPackageItemInfo(env, appInfoKlass, jAppInfo, pkgInfo);
    env->DeleteLocalRef(appInfoKlass);

    return jAppInfo;
}

jobject Util::ToJavaActivityInfo(
    /* [in] */ JNIEnv* env,
    /* [in] */ IActivityInfo* actInfo)
{
    if (env == NULL || actInfo == NULL) {
        LOGGERE("ToJavaActivityInfo", "Invalid arguments!");
        return NULL;
    }

    jobject jactInfo = NULL;

    jclass actInfoKlass = env->FindClass("android/content/pm/ActivityInfo");
    Util::CheckErrorAndLog(env, "ToJavaActivityInfo", "Fail FindClass: ActivityInfo %d", __LINE__);

    jmethodID m = env->GetMethodID(actInfoKlass, "<init>", "()V");
    Util::CheckErrorAndLog(env, "ToJavaActivityInfo", "Fail GetMethodID: actInfoKlass %d", __LINE__);

    jactInfo = env->NewObject(actInfoKlass, m);
    Util::CheckErrorAndLog(env, "ToJavaActivityInfo", "Fail NewObject: actInfoKlass %d", __LINE__);

    Int32 tempInt = 0;
    actInfo->GetTheme(&tempInt);
    jfieldID f = env->GetFieldID(actInfoKlass, "theme", "I");
    Util::CheckErrorAndLog(env, "ToJavaActivityInfo", "Fail GetFieldID: theme %d", __LINE__);

    env->SetIntField(jactInfo, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaActivityInfo", "Fail SetIntField: theme %d", __LINE__);

    actInfo->GetLaunchMode(&tempInt);
    f = env->GetFieldID(actInfoKlass, "launchMode", "I");
    Util::CheckErrorAndLog(env, "ToJavaActivityInfo", "Fail GetFieldID: launchMode %d", __LINE__);

    env->SetIntField(jactInfo, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaActivityInfo", "Fail SetIntField: launchMode %d", __LINE__);

    String permission;
    actInfo->GetPermission(&permission);
    jstring jpermission = Util::ToJavaString(env, permission);
    if (jpermission != NULL) {
        jfieldID f = env->GetFieldID(actInfoKlass, "permission", "Ljava/lang/String;");
        Util::CheckErrorAndLog(env, "ToJavaActivityInfo", "Fail GetFieldID: permission %d", __LINE__);

        env->SetObjectField(jactInfo, f, jpermission);
        Util::CheckErrorAndLog(env, "ToJavaActivityInfo", "Fail SetIntField: jpermission %d", __LINE__);
        env->DeleteLocalRef(jpermission);
    }

    String taskAffinity;
    actInfo->GetTaskAffinity(&taskAffinity);
    jstring jtaskAffinity = Util::ToJavaString(env, taskAffinity);
    if (jtaskAffinity != NULL) {
        jfieldID f = env->GetFieldID(actInfoKlass, "taskAffinity", "Ljava/lang/String;");
        Util::CheckErrorAndLog(env, "ToJavaActivityInfo", "Fail GetFieldID: taskAffinity %d", __LINE__);

        env->SetObjectField(jactInfo, f, jtaskAffinity);
        Util::CheckErrorAndLog(env, "ToJavaActivityInfo", "Fail SetIntField: jtaskAffinity %d", __LINE__);
        env->DeleteLocalRef(jtaskAffinity);
    }

    String targetActivity;
    actInfo->GetTargetActivity(&targetActivity);
    jstring jtargetActivity = Util::ToJavaString(env, targetActivity);
    if (jtargetActivity != NULL) {
        jfieldID f = env->GetFieldID(actInfoKlass, "targetActivity", "Ljava/lang/String;");
        Util::CheckErrorAndLog(env, "ToJavaActivityInfo", "Fail GetFieldID: targetActivity %d", __LINE__);

        env->SetObjectField(jactInfo, f, jtargetActivity);
        Util::CheckErrorAndLog(env, "ToJavaActivityInfo", "Fail SetIntField: jtargetActivity %d", __LINE__);
        env->DeleteLocalRef(jtargetActivity);
    }

    actInfo->GetFlags(&tempInt);
    f = env->GetFieldID(actInfoKlass, "flags", "I");
    Util::CheckErrorAndLog(env, "ToJavaActivityInfo", "Fail GetFieldID: flags %d", __LINE__);

    env->SetIntField(jactInfo, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaActivityInfo", "Fail SetIntField: flags %d", __LINE__);

    actInfo->GetScreenOrientation(&tempInt);
    f = env->GetFieldID(actInfoKlass, "screenOrientation", "I");
    Util::CheckErrorAndLog(env, "ToJavaActivityInfo", "Fail GetFieldID: screenOrientation %d", __LINE__);

    env->SetIntField(jactInfo, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaActivityInfo", "Fail SetIntField: screenOrientation %d", __LINE__);

    actInfo->GetConfigChanges(&tempInt);
    f = env->GetFieldID(actInfoKlass, "configChanges", "I");
    Util::CheckErrorAndLog(env, "ToJavaActivityInfo", "Fail GetFieldID: configChanges %d", __LINE__);

    env->SetIntField(jactInfo, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaActivityInfo", "Fail SetIntField: configChanges %d", __LINE__);

    actInfo->GetSoftInputMode(&tempInt);
    f = env->GetFieldID(actInfoKlass, "softInputMode", "I");
    Util::CheckErrorAndLog(env, "ToJavaActivityInfo", "Fail GetFieldID: softInputMode %d", __LINE__);

    env->SetIntField(jactInfo, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaActivityInfo", "Fail SetIntField: softInputMode %d", __LINE__);

    actInfo->GetUiOptions(&tempInt);
    f = env->GetFieldID(actInfoKlass, "uiOptions", "I");
    Util::CheckErrorAndLog(env, "ToJavaActivityInfo", "Fail GetFieldID: uiOptions %d", __LINE__);

    env->SetIntField(jactInfo, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaActivityInfo", "Fail SetIntField: uiOptions %d", __LINE__);

    String parentActivityName;
    actInfo->GetParentActivityName(&parentActivityName);
    jstring jparentActivityName = Util::ToJavaString(env, parentActivityName);
    if (jparentActivityName != NULL) {
        jfieldID f = env->GetFieldID(actInfoKlass, "parentActivityName", "Ljava/lang/String;");
        Util::CheckErrorAndLog(env, "ToJavaActivityInfo", "Fail GetFieldID: parentActivityName %d", __LINE__);

        env->SetObjectField(jactInfo, f, jparentActivityName);
        Util::CheckErrorAndLog(env, "ToJavaActivityInfo", "Fail SetIntField: jparentActivityName %d", __LINE__);
        env->DeleteLocalRef(jparentActivityName);
    }

    actInfo->GetPersistableMode(&tempInt);
    f = env->GetFieldID(actInfoKlass, "persistableMode", "I");
    Util::CheckErrorAndLog(env, "ToJavaActivityInfo", "Fail GetFieldID: persistableMode %d", __LINE__);

    env->SetIntField(jactInfo, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaActivityInfo", "Fail SetIntField: persistableMode %d", __LINE__);

    actInfo->GetMaxRecents(&tempInt);
    f = env->GetFieldID(actInfoKlass, "maxRecents", "I");
    Util::CheckErrorAndLog(env, "ToJavaActivityInfo", "Fail GetFieldID: maxRecents %d", __LINE__);

    env->SetIntField(jactInfo, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaActivityInfo", "Fail SetIntField: maxRecents %d", __LINE__);

    AutoPtr<IComponentInfo> comInfo = IComponentInfo::Probe(actInfo);
    SetComponentInfo(env, actInfoKlass, jactInfo, comInfo);

    env->DeleteLocalRef(actInfoKlass);

    return jactInfo;
}


jobject Util::ToJavaBundle(
    /* [in] */ JNIEnv* env,
    /* [in] */ IBundle* bundle)
{
    if (env == NULL || bundle == NULL) {
        LOGGERE("ToJavaBundle", "Invalid arguments!");
        return NULL;
    }

    jclass bundleKlass = env->FindClass("android/os/Bundle");
    Util::CheckErrorAndLog(env, "ToJavaBundle", "Fail FindClass: Bundle %d", __LINE__);

    jmethodID m = env->GetMethodID(bundleKlass, "<init>", "()V");
    Util::CheckErrorAndLog(env, "ToJavaBundle", "Fail GetMethodID: bundleKlass %d", __LINE__);

    jobject jBundle = env->NewObject(bundleKlass, m);
    Util::CheckErrorAndLog(env, "ToJavaBundle", "Fail NewObject: bundleKlass %d", __LINE__);

    if (!SetJavaBaseBundle(env, IBaseBundle::Probe(bundle), bundleKlass, jBundle)) {
        LOGGERE(TAG, "ToJavaBundle SetJavaBaseBundle failed!");
    }
    env->DeleteLocalRef(bundleKlass);
    return jBundle;
}

Boolean Util::SetJavaBaseBundle(
    /* [in] */ JNIEnv* env,
    /* [in] */ IBaseBundle* bundle,
    /* [in] */ jclass bundleKlass,
    /* [in] */ jobject jbundle)
{
    if (env == NULL || bundle == NULL || bundleKlass == NULL || jbundle == NULL) {
        LOGGERE("SetJavaBaseBundle", "Invalid arguments!");
        return FALSE;
    }

    AutoPtr<ArrayOf<Byte> > data;
    bundle->GetJavaData((ArrayOf<Byte>**)&data);
    if (data != NULL) {
        jclass parcelClass = env->FindClass("android/os/Parcel");
        Util::CheckErrorAndLog(env, "ToJavaBundle", "FindClass: Parcel : %d", __LINE__);

        jmethodID m = env->GetStaticMethodID(parcelClass, "obtain", "()Landroid/os/Parcel;");
        Util::CheckErrorAndLog(env, "ToJavaBundle", "GetStaticMethodID: obtain : %d", __LINE__);

        jobject jparcel = env->CallStaticObjectMethod(parcelClass, m);
        Util::CheckErrorAndLog(env, "ToJavaBundle", "GetStaticMethodID: obtain : %d", __LINE__);

        Int64 nativePtr = Util::GetJavalongField(env, jparcel, "mNativePtr", "ToJavaBundle");
        android::Parcel* parcel =  reinterpret_cast< android::Parcel*>(nativePtr);
        assert(parcel);
        Int32 length = data->GetLength() * sizeof(Byte);
        parcel->setDataSize(length);
        parcel->setDataPosition(0);

        void* raw = parcel->writeInplace(length);
        memcpy(raw, data->GetPayload(), length);
        parcel->setDataPosition(0);

        m = env->GetMethodID(bundleKlass, "readFromParcel", "(Landroid/os/Parcel;)V");
        CheckErrorAndLog(env, "ToJavaBundle", "GetMethodID readFromParcel : %d!\n", __LINE__);

        env->CallVoidMethod(jbundle, m, jparcel);
        CheckErrorAndLog(env, "ToJavaBundle", "Bundle readFromParcel : %d!\n", __LINE__);

        env->DeleteLocalRef(parcelClass);
        env->DeleteLocalRef(jparcel);

        return TRUE;
    }

    AutoPtr<ISet> keySet;
    bundle->GetKeySet((ISet**)&keySet);
    AutoPtr<ArrayOf<IInterface*> > keys;
    keySet->ToArray((ArrayOf<IInterface*>**)&keys);
    for (Int32 i = 0; i < keys->GetLength(); i++) {
        AutoPtr<IInterface> key = (*keys)[i];
        assert(key != NULL && ICharSequence::Probe(key) != NULL);
        String keyStr;
        ICharSequence::Probe(key)->ToString(&keyStr);
        jstring jKey = Util::ToJavaString(env, keyStr);
        AutoPtr<IInterface> value;
        bundle->Get(keyStr, (IInterface**)&value);
        if (value != NULL) {
            if (IByte::Probe(value) != NULL){
                Byte bv;
                IByte::Probe(value)->GetValue(&bv);

                jmethodID m = env->GetMethodID(bundleKlass, "putByte", "(Ljava/lang/String;B)V");
                Util::CheckErrorAndLog(env, "ToJavaBundle", "Fail GetMethodID: putByte %d", __LINE__);

                env->CallVoidMethod(jbundle, m, jKey, (jbyte)bv);
            }
            else if (IInteger16::Probe(value) != NULL) {
                Int16 iv;
                IInteger16::Probe(value)->GetValue(&iv);

                jmethodID m = env->GetMethodID(bundleKlass, "putShort", "(Ljava/lang/String;S)V");
                Util::CheckErrorAndLog(env, "ToJavaBundle", "Fail GetMethodID: putShort %d", __LINE__);

                env->CallVoidMethod(jbundle, m, jKey, iv);
            }
            else if (IInteger32::Probe(value) != NULL) {
                Int32 iv;
                IInteger32::Probe(value)->GetValue(&iv);

                jmethodID m = env->GetMethodID(bundleKlass, "putInt", "(Ljava/lang/String;I)V");
                Util::CheckErrorAndLog(env, "ToJavaBundle", "Fail GetMethodID: putInt %d", __LINE__);

                env->CallVoidMethod(jbundle, m, jKey, iv);
            }
            else if (IInteger64::Probe(value) != NULL) {
                Int64 longValue;
                IInteger64::Probe(value)->GetValue(&longValue);

                jmethodID m = env->GetMethodID(bundleKlass, "putLong", "(Ljava/lang/String;J)V");
                Util::CheckErrorAndLog(env, "ToJavaBundle", "Fail GetMethodID: putLong %d", __LINE__);

                env->CallVoidMethod(jbundle, m, jKey, longValue);
            }
            else if (IBoolean::Probe(value) != NULL) {
                Boolean bv;
                IBoolean::Probe(value)->GetValue(&bv);
                jmethodID m = env->GetMethodID(bundleKlass, "putBoolean", "(Ljava/lang/String;Z)V");
                Util::CheckErrorAndLog(env, "ToJavaBundle", "Fail GetMethodID: putBoolean %d", __LINE__);

                env->CallVoidMethod(jbundle, m, jKey, bv);
            }
            else if (ICharSequence::Probe(value) != NULL) {
                String sv;
                ICharSequence::Probe(value)->ToString(&sv);
                jstring jValue = Util::ToJavaString(env, sv);

                jmethodID m = env->GetMethodID(bundleKlass, "putString", "(Ljava/lang/String;Ljava/lang/String;)V");
                Util::CheckErrorAndLog(env, "ToJavaBundle", "Fail GetMethodID: putString %d", __LINE__);

                env->CallVoidMethod(jbundle, m, jKey, jValue);
                env->DeleteLocalRef(jValue);
            }
            else if (IIntent::Probe(value) != NULL) {
                AutoPtr<IIntent> intent = IIntent::Probe(value);
                jobject jintent = ToJavaIntent(env, intent.Get());
                jmethodID m = env->GetMethodID(bundleKlass, "putParcelable", "(Ljava/lang/String;Landroid/os/Parcelable;)V");
                Util::CheckErrorAndLog(env, "ToJavaBundle", "Fail GetMethodID: putParcelable %d", __LINE__);
                env->CallVoidMethod(jbundle, m, jKey, jintent);
                env->DeleteLocalRef(jintent);
            }
            else if (IBundle::Probe(value) != NULL) {
                AutoPtr<IBundle> bundle = IBundle::Probe(value);
                jobject jbundle = ToJavaBundle(env, bundle.Get());
                jmethodID m = env->GetMethodID(bundleKlass, "putBundle", "(Ljava/lang/String;Landroid/os/Bundle;)V");
                Util::CheckErrorAndLog(env, "ToJavaBundle", "Fail GetMethodID: putBundle %d", __LINE__);
                env->CallVoidMethod(jbundle, m, jKey, jbundle);
                env->DeleteLocalRef(jbundle);
            }
            else if (IArrayOf::Probe(value) != NULL){
                AutoPtr<IArrayOf> array = IArrayOf::Probe(value);
                Int32 size = 0;
                array->GetLength(&size);
                LOGGERD("ToJavaBundle", "ToJavaBundle() found ArrayOf type! key is: %s, size = %d", keyStr.string(), size);

                // Check size
                if (size > 0){
                    AutoPtr<IInterface> firstItem;
                    array->Get(0, (IInterface**)&firstItem); // Get first item to check the type of the interface

                    if (firstItem != NULL){
                        if (IIntent::Probe(firstItem) != NULL) { // It's an intent array
                            jclass intentKlass = env->FindClass("android/content/Intent");
                            jobjectArray jintentArray = env->NewObjectArray(size, intentKlass, NULL);

                            // Process the elements in array
                            for (Int32 i = 0; i < size; ++i) {
                                AutoPtr<IInterface> elem;
                                array->Get(i, (IInterface**)&elem);
                                if (elem != NULL && IIntent::Probe(elem) != NULL) {
                                    jobject jintent = ToJavaIntent(env, IIntent::Probe(elem));
                                    env->SetObjectArrayElement(jintentArray, i, jintent);
                                    Util::CheckErrorAndLog(env, "ToJavaBundle", "Fail SetObjectArrayElement %d", __LINE__);
                                    env->DeleteLocalRef(jintent);
                                }
                            }

                            jmethodID m = env->GetMethodID(bundleKlass, "putParcelableArray", "(Ljava/lang/String;[Landroid/os/Parcelable;)V");
                            Util::CheckErrorAndLog(env, "ToJavaBundle", "Fail GetMethodID: putParcelableArray %d", __LINE__);
                            env->CallVoidMethod(jbundle, m, jKey, jintentArray);
                            env->DeleteLocalRef(intentKlass);
                            env->DeleteLocalRef(jintentArray);
                            Logger::D("ToJavaBundle", "ToJavaBundle() Intent array set into bundle for key:%s", keyStr.string());
                        }
                        else if (IInteger32::Probe(firstItem) != NULL){
                            jintArray jarr = env->NewIntArray(size);
                            CheckErrorAndLog(env, "ToJavaBundle", "NewIntArray failed  %d", __LINE__);

                            for(Int32 i = 0; i < size; ++i){
                                AutoPtr<IInterface> elem;
                                array->Get(i, (IInterface**)&elem);
                                if (elem != NULL && IInteger32::Probe(elem) != NULL) {
                                    Int32 iv;
                                    IInteger32::Probe(elem)->GetValue(&iv);
                                    env->SetIntArrayRegion(jarr, i, 1, &iv);
                                    CheckErrorAndLog(env, "Util", "ToJavaBundle, SetIntArrayRegion: failed, %d", __LINE__);
                                }
                            }

                            jmethodID m = env->GetMethodID(bundleKlass, "putIntArray", "(Ljava/lang/String;[I)V");
                            Util::CheckErrorAndLog(env, "ToJavaBundle", "Fail GetMethodID: putIntArray %d", __LINE__);
                            env->CallVoidMethod(jbundle, m, jKey, jarr);
                            env->DeleteLocalRef(jarr);
                            Logger::D("ToJavaBundle", "ToJavaBundle() int array set into bundle for key:%s", keyStr.string());
                        }
                        else if (IByte::Probe(firstItem) != NULL){
                            if (keyStr.Equals(String("pdus"))) {
                                //LOGGERE("Javaproxy-Util", "leliang find pdus in Bundle!value:%p", value.Get());

                                //for pdus there is special format. see InboundSmsHandler.cpp for more info
                                jclass byteArrayClass = env->FindClass("[B");
                                jobjectArray jpdus;
                                Int32 jpdusLength = 0;
                                Int32 i = 0;
                                AutoPtr<IInterface> elem;
                                array->Get(i++, (IInterface**)&elem);//index indicate how many byte[] exist;
                                if (IByte::Probe(elem) != NULL) {
                                    Byte bv;
                                    IByte::Probe(elem)->GetValue(&bv);
                                    jpdus = env->NewObjectArray(bv, byteArrayClass, NULL);
                                    jpdusLength = bv;
                                    //LOGGERE("Javaproxy-Util", "leliang pdus have %d pdu", bv);
                                    CheckErrorAndLog(env, "ToJavaBundle", "NewObjectArray failed  %d", __LINE__);
                                }
                                else {
                                    LOGGERE("ToJavaBundle", "pdus len is invalid, set to zero!!! line:%d", __LINE__);
                                    jpdusLength = 0;
                                    jpdus = env->NewObjectArray(0, byteArrayClass, NULL);
                                }
                                for(Int32  arrayIndex = 0; arrayIndex < jpdusLength; ++arrayIndex) {
                                    Int32 pduLength = 0;
                                    AutoPtr<IInterface> elem;
                                    array->Get(i++, (IInterface**)&elem);//the first byte is the len of the pdu
                                    //because the pdu contains the UTF-8 format string
                                    //here we have to transfer to Unicode which java used
                                    if (IByte::Probe(elem) != NULL) {
                                        Byte bv;
                                        IByte::Probe(elem)->GetValue(&bv);
                                        pduLength = bv;
                                        //LOGGERE("Javaproxy-Util", "leliang the %d pdu have %d byte", arrayIndex, bv);
                                    }
                                    else {
                                        LOGGERE("ToJavaBundle", "pdu len is invalid, set to zero!!! line:%d", __LINE__);
                                        pduLength = 0;
                                    }
                                    AutoPtr<ArrayOf<Byte> > arrayByte = ArrayOf<Byte>::Alloc(pduLength);
                                    for (Int32 elemIndex = i; elemIndex < i + pduLength; ++elemIndex) {
                                        AutoPtr<IInterface> elem;
                                        array->Get(elemIndex, (IInterface**)&elem);
                                        Byte bv;
                                        if (IByte::Probe(elem) != NULL) {
                                            IByte::Probe(elem)->GetValue(&bv);
                                            arrayByte->Set(elemIndex-i, bv);
                                        }
                                        //LOGGERE("Javaproxy-Util", "leliang the %d pdu[%d] is 0x%x", arrayIndex, elemIndex-i, bv);
                                    }
                                    //try to get the unicode byteArray from UTF-8 byteArray.
                                    AutoPtr<ArrayOf<Byte> > unicodeByteArray = arrayByte;

                                    Int32 unicodeByteArrayLen = unicodeByteArray->GetLength();
                                    jbyteArray byteArray = env->NewByteArray(unicodeByteArrayLen);
                                    Util::CheckErrorAndLog(env, "Javaproxy-Util", "NewByteArray: %d!\n", __LINE__);
                                    Byte* payload = unicodeByteArray->GetPayload();
                                    env->SetByteArrayRegion(byteArray, 0, unicodeByteArrayLen, (jbyte *)payload);
                                    Util::CheckErrorAndLog(env, "Javaproxy-Util", "SetByteArrayRegion: %d!\n", __LINE__);
                                    env->SetObjectArrayElement(jpdus, arrayIndex, byteArray);
                                    Util::CheckErrorAndLog(env, "Javaproxy-Util", "SetObjectArrayElement: %d!\n", __LINE__);
                                    env->DeleteLocalRef(byteArray);

                                    i += pduLength;
                                }

                                jmethodID m = env->GetMethodID(bundleKlass, "putSerializable", "(Ljava/lang/String;Ljava/io/Serializable;)V");
                                Util::CheckErrorAndLog(env, "ToJavaBundle", "Fail GetMethodID: putSerializable %d", __LINE__);
                                env->CallVoidMethod(jbundle, m, jKey, jpdus);

                                env->DeleteLocalRef(byteArrayClass);
                                env->DeleteLocalRef(jpdus);
                                LOGGERE("Javaproxy-Util", "leliang find pdus in Bundle!line:%d", __LINE__);
                            }
                            else {
                                jbyteArray jarr = env->NewByteArray(size);
                                CheckErrorAndLog(env, "ToJavaBundle", "NewByteArray failed  %d", __LINE__);

                                for(Int32 i = 0; i < size; ++i){
                                    AutoPtr<IInterface> elem;
                                    array->Get(i, (IInterface**)&elem);
                                    if (IByte::Probe(elem) != NULL) {
                                        Byte bv;
                                        IByte::Probe(elem)->GetValue(&bv);
                                        env->SetByteArrayRegion(jarr, i, 1, (jbyte*)&bv);
                                        CheckErrorAndLog(env, "Util", "ToJavaBundle, SetByteArrayRegion: failed, %d", __LINE__);
                                    }
                                }

                                jmethodID m = env->GetMethodID(bundleKlass, "putByteArray", "(Ljava/lang/String;[B)V");
                                Util::CheckErrorAndLog(env, "ToJavaBundle", "Fail GetMethodID: putByteArray %d", __LINE__);
                                env->CallVoidMethod(jbundle, m, jKey, jarr);
                                env->DeleteLocalRef(jarr);
                            }
                            Logger::D("ToJavaBundle", "ToJavaBundle() byte array set into bundle for key:%s", keyStr.string());
                        }
                        else if (IString::Probe(firstItem) != NULL) {
                            jclass stringKlass = env->FindClass("java/lang/String");
                            jobjectArray jarr = env->NewObjectArray(size, stringKlass, NULL);
                            CheckErrorAndLog(env, "ToJavaBundle", "NewStringArray failed  %d", __LINE__);

                            IString* strObj;
                            String str;
                            for(Int32 i = 0; i < size; ++i) {
                                AutoPtr<IInterface> elem;
                                array->Get(i, (IInterface**)&elem);
                                strObj = IString::Probe(elem);
                                if (strObj != NULL) {
                                    jstring jitem = Util::ToJavaString(env, Object::ToString(strObj));
                                    env->SetObjectArrayElement(jarr, i, jitem);
                                    CheckErrorAndLog(env, "Util", "ToJavaBundle, SetObjectArrayElement: String[] failed, %d", __LINE__);
                                }
                            }

                            jmethodID m = env->GetMethodID(bundleKlass, "putStringArray", "(Ljava/lang/String;[Ljava/lang/String;)V");
                            Util::CheckErrorAndLog(env, "ToJavaBundle", "Fail GetMethodID: putStringArray %d", __LINE__);
                            env->CallVoidMethod(jbundle, m, jKey, jarr);
                            env->DeleteLocalRef(jarr);
                            Logger::D("ToJavaBundle", "ToJavaBundle() string array set into bundle for key:%s", keyStr.string());
                        }
                        else {
                            LOGGERE("ToJavaBundle", "ArrayOf item [%s] is not implemented!!!", TO_CSTR(firstItem));
                        }
                    }
                    else {
                        LOGGERE("ToJavaBundle", "ToJavaBundle()  ArrayOf item is Unknown!");
                    }
                }
                else {
                    LOGGERE("ToJavaBundle", "ToJavaBundle()  ArrayOf type first item is NULL!");
                }
            }
            else if (IBitmap::Probe(value) != NULL) {
                AutoPtr<IBitmap> bitmap = IBitmap::Probe(value);
                jobject jbitmap = ToJavaBitmap(env, bitmap);

                jmethodID m = env->GetMethodID(bundleKlass, "putParcelable", "(Ljava/lang/String;Landroid/os/Parcelable;)V");
                CheckErrorAndLog(env, "Util", "ToJavaBundle Fail GetMethodID: putParcelable %d", __LINE__);

                env->CallVoidMethod(jbundle, m, jKey, jbitmap);
                env->DeleteLocalRef(jbitmap);
            }
            else if (INetworkInfo::Probe(value) != NULL) {
                AutoPtr<INetworkInfo> info = INetworkInfo::Probe(value);
                jobject jinfo = ToJavaNetworkInfo(env, info);
                jmethodID m = env->GetMethodID(bundleKlass, "putParcelable", "(Ljava/lang/String;Landroid/os/Parcelable;)V");
                Util::CheckErrorAndLog(env, "ToJavaBundle", "GetMethodID: putParcelable  %d", __LINE__);
                env->CallVoidMethod(jbundle, m, jKey, jinfo);
                env->DeleteLocalRef(jinfo);
            }
            else if (IWifiInfo::Probe(value) != NULL) {
                AutoPtr<IWifiInfo> info = IWifiInfo::Probe(value);
                jobject jinfo = ToJavaWifiInfo(env, info);
                jmethodID m = env->GetMethodID(bundleKlass, "putParcelable", "(Ljava/lang/String;Landroid/os/Parcelable;)V");
                Util::CheckErrorAndLog(env, "ToJavaBundle", "GetMethodID: putParcelable  %d", __LINE__);
                env->CallVoidMethod(jbundle, m, jKey, jinfo);
                env->DeleteLocalRef(jinfo);
            }
            else if (ILinkProperties::Probe(value) != NULL) {
                AutoPtr<ILinkProperties> properties = ILinkProperties::Probe(value);
                jobject jproperties = ToJavaLinkProperties(env, properties);
                jmethodID m = env->GetMethodID(bundleKlass, "putParcelable", "(Ljava/lang/String;Landroid/os/Parcelable;)V");
                Util::CheckErrorAndLog(env, "ToJavaBundle", "GetMethodID: putParcelable %d", __LINE__);
                env->CallVoidMethod(jbundle, m, jKey, jproperties);
                env->DeleteLocalRef(jproperties);
            }
            else if (IStorageVolume::Probe(value) != NULL) {
                AutoPtr<IStorageVolume> volume = IStorageVolume::Probe(value);
                jobject jvolume = ToJavaStorageVolume(env, volume);
                jmethodID m = env->GetMethodID(bundleKlass, "putParcelable", "(Ljava/lang/String;Landroid/os/Parcelable;)V");
                Util::CheckErrorAndLog(env, "ToJavaBundle", "GetMethodID: putParcelable %d", __LINE__);
                env->CallVoidMethod(jbundle, m, jKey, jvolume);
                env->DeleteLocalRef(jvolume);
            }
            else if (IApplicationInfo::Probe(value) != NULL) {
                AutoPtr<IApplicationInfo> info = IApplicationInfo::Probe(value);
                jobject jinfo = ToJavaApplicationInfo(env, info);
                jmethodID m = env->GetMethodID(bundleKlass, "putParcelable", "(Ljava/lang/String;Landroid/os/Parcelable;)V");
                Util::CheckErrorAndLog(env, "ToJavaBundle", "GetMethodID: putParcelable %d", __LINE__);
                env->CallVoidMethod(jbundle, m, jKey, jinfo);
                env->DeleteLocalRef(jinfo);
            }
            else if (IHashMap::Probe(value) != NULL) {
                AutoPtr<IHashMap> map = IHashMap::Probe(value);
                jobject jmap = ToJavaHashMap(env, map);
                jmethodID m = env->GetMethodID(bundleKlass, "putSerializable", "(Ljava/lang/String;Ljava/io/Serializable;)V");
                Util::CheckErrorAndLog(env, "ToJavaBundle", "GetMethodID: putSerializable %d", __LINE__);
                env->CallVoidMethod(jbundle, m, jKey, jmap);
                env->DeleteLocalRef(jmap);
            }
            else if (IIntentShortcutIconResource::Probe(value) != NULL) {
                AutoPtr<IIntentShortcutIconResource> res = IIntentShortcutIconResource::Probe(value);

                jclass iconResKlass = env->FindClass("android/content/Intent$ShortcutIconResource");
                Util::CheckErrorAndLog(env, "ToJavaBundle", "Fail FindClass: ShortcutIconResource %d", __LINE__);

                jmethodID mconstructor = env->GetMethodID(iconResKlass, "<init>", "()V");
                Util::CheckErrorAndLog(env, "ToJavaBundle", "Fail GetMethodID: iconResKlass %d", __LINE__);

                jobject jiconRes = env->NewObject(iconResKlass, mconstructor);
                Util::CheckErrorAndLog(env, "ToJavaBundle", "Fail NewObject: iconResKlass %d", __LINE__);

                String packageName;
                String resName;
                res->GetPackageName(&packageName);
                res->GetResourceName(&resName);

                SetJavaStringField(env, iconResKlass, jiconRes, packageName, "packageName", "ToJavaBundle");
                SetJavaStringField(env, iconResKlass, jiconRes, resName, "resourceName", "ToJavaBundle");

                jmethodID m = env->GetMethodID(bundleKlass, "putParcelable", "(Ljava/lang/String;Landroid/os/Parcelable;)V");
                Util::CheckErrorAndLog(env, "ToJavaBundle", "GetMethodID: putParcelable %d", __LINE__);
                env->CallVoidMethod(jbundle, m, jKey, jiconRes);
                env->DeleteLocalRef(iconResKlass);
                env->DeleteLocalRef(jiconRes);
            }
            else if (IVpnProfile::Probe(value) != NULL) {
                AutoPtr<IVpnProfile> profile = IVpnProfile::Probe(value);
                jobject jprofile = ToJavaVpnProfile(env, profile);
                jmethodID m = env->GetMethodID(bundleKlass, "putParcelable", "(Ljava/lang/String;Landroid/os/Parcelable;)V");
                Util::CheckErrorAndLog(env, "ToJavaBundle", "GetMethodID: putParcelable %d", __LINE__);
                env->CallVoidMethod(jbundle, m, jKey, jprofile);
                env->DeleteLocalRef(jprofile);
            }
            else if (IComponentName::Probe(value) != NULL) {
                AutoPtr<IComponentName> componentName = IComponentName::Probe(value);
                jobject jcomponentName = ToJavaComponentName(env, componentName);
                jmethodID m = env->GetMethodID(bundleKlass, "putParcelable", "(Ljava/lang/String;Landroid/os/Parcelable;)V");
                Util::CheckErrorAndLog(env, "ToJavaBundle", "GetMethodID: putParcelable %d", __LINE__);
                env->CallVoidMethod(jbundle, m, jKey, jcomponentName);
                env->DeleteLocalRef(jcomponentName);
            }
            else if (ISupplicantState::Probe(value) != NULL) {
                AutoPtr<ISupplicantState> supState = ISupplicantState::Probe(value);
                jobject jSupState = ToJavaSupplicantState(env, supState);
                jmethodID m = env->GetMethodID(bundleKlass, "putParcelable", "(Ljava/lang/String;Landroid/os/Parcelable;)V");
                Util::CheckErrorAndLog(env, "ToJavaBundle", "GetMethodID: putParcelable %d", __LINE__);
                env->CallVoidMethod(jbundle, m, jKey, jSupState);
                env->DeleteLocalRef(jSupState);
            }
            else if (IWifiP2pInfo::Probe(value) != NULL) {
                AutoPtr<IWifiP2pInfo> info = IWifiP2pInfo::Probe(value);
                jobject jinfo = ToJavaWifiP2pInfo(env, info);
                jmethodID m = env->GetMethodID(bundleKlass, "putParcelable", "(Ljava/lang/String;Landroid/os/Parcelable;)V");
                Util::CheckErrorAndLog(env, "ToJavaBundle", "GetMethodID: putParcelable %d", __LINE__);
                env->CallVoidMethod(jbundle, m, jKey, jinfo);
                env->DeleteLocalRef(jinfo);
            }
            else if (IWifiP2pDevice::Probe(value) != NULL) {
                AutoPtr<IWifiP2pDevice> device = IWifiP2pDevice::Probe(value);
                jobject jdevice = ToJavaWifiP2pDevice(env, device);
                jmethodID m = env->GetMethodID(bundleKlass, "putParcelable", "(Ljava/lang/String;Landroid/os/Parcelable;)V");
                Util::CheckErrorAndLog(env, "ToJavaBundle", "GetMethodID: putParcelable %d", __LINE__);
                env->CallVoidMethod(jbundle, m, jKey, jdevice);
                env->DeleteLocalRef(jdevice);
            }
            else if (IWifiConfiguration::Probe(value) != NULL) {
                AutoPtr<IWifiConfiguration> configuration = IWifiConfiguration::Probe(value);
                jobject jconfiguration = ToJavaWifiConfiguration(env, configuration);
                jmethodID m = env->GetMethodID(bundleKlass, "putParcelable", "(Ljava/lang/String;Landroid/os/Parcelable;)V");
                Util::CheckErrorAndLog(env, "ToJavaBundle", "GetMethodID: putParcelable %d", __LINE__);
                env->CallVoidMethod(jbundle, m, jKey, jconfiguration);
                env->DeleteLocalRef(jconfiguration);
            }
            else if (IPendingIntent::Probe(value) != NULL) {
                AutoPtr<IPendingIntent> pIntent = IPendingIntent::Probe(value);
                jobject jpIntent = ToJavaPendingIntent(env, pIntent);
                jmethodID m = env->GetMethodID(bundleKlass, "putParcelable", "(Ljava/lang/String;Landroid/os/Parcelable;)V");
                Util::CheckErrorAndLog(env, "ToJavaBundle", "GetMethodID: putParcelable %d", __LINE__);
                env->CallVoidMethod(jbundle, m, jKey, jpIntent);
                env->DeleteLocalRef(jpIntent);
            }
            else if (IWifiDisplayStatus::Probe(value) != NULL) {
                AutoPtr<IWifiDisplayStatus> pDataObj = IWifiDisplayStatus::Probe(value);
                jobject jpDataObj = ToJavaWifiDisplayStatus(env, pDataObj);
                jmethodID m = env->GetMethodID(bundleKlass, "putParcelable", "(Ljava/lang/String;Landroid/os/Parcelable;)V");
                Util::CheckErrorAndLog(env, "ToJavaBundle", "GetMethodID: putParcelable %d", __LINE__);
                env->CallVoidMethod(jbundle, m, jKey, jpDataObj);
                env->DeleteLocalRef(jpDataObj);
            }
            else if (IList::Probe(value) != NULL) {
                AutoPtr<IList> list = IList::Probe(value);

                jclass listKlass = env->FindClass("java/util/ArrayList");
                Util::CheckErrorAndLog(env, "ToJavaBundle", "FindClass: ArrayList %d", __LINE__);

                jmethodID m = env->GetMethodID(listKlass, "<init>", "()V");
                Util::CheckErrorAndLog(env, "ToJavaBundle", "GetMethodID: ArrayList %d", __LINE__);

                jobject jlist = env->NewObject(listKlass, m);
                Util::CheckErrorAndLog(env, "ToJavaBundle", "NewObject: ArrayList %d", __LINE__);

                Int32 count = 0;
                list->GetSize(&count);
                if (count > 0) {
                    jmethodID mAdd = env->GetMethodID(listKlass, "add", "(Ljava/lang/Object;)Z");
                    Util::CheckErrorAndLog(env, "ToJavaBundle", "GetMethodID: add %d", __LINE__);

                    for (Int32 j = 0; j < count; j++) {
                        AutoPtr<IInterface> item;
                        list->Get(i, (IInterface**)&item);
                        ClassID clsid;
                        IObject::Probe(item)->GetClassID(&clsid);
                        if (ICharSequence::Probe(item)) {
                            AutoPtr<ICharSequence> csitem = ICharSequence::Probe(item);
                            String sitem;
                            csitem->ToString(&sitem);
                            jstring jitem = Util::ToJavaString(env, sitem);

                            env->CallBooleanMethod(jlist, mAdd, jitem);
                            CheckErrorAndLog(env, "GetJavaInputDevice Fail CallObjectMethod: mAdd : %d!\n", __LINE__);
                            env->DeleteLocalRef(jitem);
                        } else if (ECLSID_CIParcelableNative == clsid) {
                            jobject jitem = Util::ToJavaParcelable(env, IParcelable::Probe(item));
                            if (jitem != NULL) {
                                env->CallBooleanMethod(jlist, mAdd, jitem);
                                CheckErrorAndLog(env, "GetJavaInputDevice Fail CallObjectMethod: mAdd : %d!\n", __LINE__);

                                env->DeleteLocalRef(jitem);
                            }
                            else {
                                LOGGERE("ToJavaBundle", "IObjectContainer Unknown type!");
                            }
                        } else {
                            LOGGERE("ToJavaBundle", "IObjectContainer ToJavaParcelable fail!");
                        }
                    }

                    m = env->GetMethodID(bundleKlass, "putParcelableArrayList", "(Ljava/lang/String;Ljava/util/ArrayList;)V");
                    Util::CheckErrorAndLog(env, "ToJavaBundle", "GetMethodID: putParcelableArrayList %d", __LINE__);

                    env->CallVoidMethod(jbundle, m, jKey, jlist);
                    Util::CheckErrorAndLog(env, "ToJavaBundle", "CallVoidMethod: putParcelableArrayList %d", __LINE__);

                    env->DeleteLocalRef(listKlass);
                    env->DeleteLocalRef(jlist);
                }
            }
            else if (ISerializable::Probe(value) != NULL) {
                ClassID clsid;
                IObject::Probe(value)->GetClassID(&clsid);

                if (ECLSID_CISerializableNative == clsid) {
                    jobject jserializable = Util::ElByteArrayToJavaObject(env, ISerializable::Probe(value));
                    LOGGERD("ToJavaBundle", "ISerializable jserializable: %p");

                    jmethodID m = env->GetMethodID(bundleKlass, "putSerializable", "(Ljava/lang/String;Ljava/io/Serializable;)V");
                    Util::CheckErrorAndLog(env, "ToJavaBundle", "GetMethodID: putSerializable %d", __LINE__);

                    env->CallVoidMethod(jbundle, m, jKey, jserializable);
                    Util::CheckErrorAndLog(env, "ToJavaBundle", "CallVoidMethod: putSerializable %d", __LINE__);

                    env->DeleteLocalRef(jserializable);
                }
                else {
                    LOGGERE("ToJavaBundle", "ToJavaBundle() Unknown ISerializable type not implemented! key is:%s\n", keyStr.string());
                    DUMP_CLSID(clsid, "ToJavaBundle");
                }
            }
            else if (IParcelable::Probe(value) != NULL){
                AutoPtr<IObject> object = IObject::Probe(value);
                ClassID clsid;
                object->GetClassID(&clsid);

                if (ECLSID_CIParcelableNative == clsid) {
                    jobject jparcelable = Util::ToJavaParcelable(env, IParcelable::Probe(value));
                    if (jparcelable != NULL) {
                        jmethodID m = env->GetMethodID(bundleKlass, "putParcelable", "(Ljava/lang/String;Landroid/os/Parcelable;)V");
                        Util::CheckErrorAndLog(env, "ToJavaBundle", "Fail GetMethodID: putParcelable %d", __LINE__);

                        env->CallVoidMethod(jbundle, m, jKey, jparcelable);
                        env->DeleteLocalRef(jparcelable);
                        Util::CheckErrorAndLog(env, "ToJavaBundle", "CallVoidMethod: putParcelable %d", __LINE__);
                    }
                    else {
                        LOGGERE("ToJavaBundle", "ToJavaParcelable fail!");
                    }
                }
                else if (ECLSID_CUserHandle == clsid) {
                    jobject jparcelable = Util::ToJavaUserHandle(env, IUserHandle::Probe(value));
                    if (jparcelable != NULL) {
                        jmethodID m = env->GetMethodID(bundleKlass, "putParcelable", "(Ljava/lang/String;Landroid/os/Parcelable;)V");
                        Util::CheckErrorAndLog(env, "ToJavaBundle", "Fail GetMethodID: putParcelable %d", __LINE__);

                        env->CallVoidMethod(jbundle, m, jKey, jparcelable);
                        env->DeleteLocalRef(jparcelable);
                        Util::CheckErrorAndLog(env, "ToJavaBundle", "CallVoidMethod: putParcelable %d", __LINE__);
                    }
                    else {
                        LOGGERE("ToJavaBundle", "ToJavaParcelable fail!");
                    }
                }
                else if (ECLSID_CHierarchicalUri == clsid) {
                    jobject jparcelable = Util::ToJavaUri(env, IUri::Probe(value));
                    if (jparcelable != NULL) {
                        jmethodID m = env->GetMethodID(bundleKlass, "putParcelable", "(Ljava/lang/String;Landroid/os/Parcelable;)V");
                        Util::CheckErrorAndLog(env, "ToJavaBundle", "Fail GetMethodID: putParcelable %d", __LINE__);

                        env->CallVoidMethod(jbundle, m, jKey, jparcelable);
                        env->DeleteLocalRef(jparcelable);
                        Util::CheckErrorAndLog(env, "ToJavaBundle", "CallVoidMethod: putParcelable %d", __LINE__);
                    }
                    else {
                        LOGGERE("ToJavaBundle", "ToJavaParcelable fail!");
                    }
                }
            }
            else{
                LOGGERE("ToJavaBundle", "ToJavaBundle() Unsupported type! key is:%s\n", keyStr.string());
                AutoPtr<IObject> object = IObject::Probe(value);
                ClassID clsid;
                object->GetClassID(&clsid);
                DUMP_CLSID(clsid, "ToJavaBundle");
            }

            env->DeleteLocalRef(jKey);
        }
        else {
            jmethodID m = env->GetMethodID(bundleKlass, "putParcelable", "(Ljava/lang/String;Landroid/os/Parcelable;)V");
            Util::CheckErrorAndLog(env, "ToJavaBundle", "Fail GetMethodID: putParcelable %d", __LINE__);

            env->CallVoidMethod(jbundle, m, jKey, NULL);
            Util::CheckErrorAndLog(env, "ToJavaBundle", "CallVoidMethod: putParcelable %d", __LINE__);
        }
    }
    return TRUE;
}

Boolean Util::GetElBitmap(
    /* [in] */ JNIEnv* env,
    /* [in] */ jobject jbitmap,
    /* [out] */ IBitmap** bitmap)
{
    if (bitmap == NULL){
        Logger::E("GetElBitmap", "GetElBitmap() INVALID params");
        return FALSE;
    }

    *bitmap = NULL;

    if (jbitmap == NULL){
        Logger::W("GetElBitmap", "GetElBitmap() input jbitmap null");
        return TRUE;
    }

    jclass parcelClass = env->FindClass("android/os/Parcel");
    CheckErrorAndLog(env, "FindClass: Parcel : %d!\n", __LINE__);

    jmethodID m = env->GetStaticMethodID(parcelClass, "obtain", "()Landroid/os/Parcel;");
    CheckErrorAndLog(env, "GetMethodID: obtain : %d!\n", __LINE__);

    jobject jparcel = env->CallStaticObjectMethod(parcelClass, m);
    CheckErrorAndLog(env, "CallStaticObjectMethod: obtain : %d!\n", __LINE__);

    jclass bitmapClass = env->FindClass("android/graphics/Bitmap");
    CheckErrorAndLog(env, "ToJavaBitmap", "FindClass: Bitmap : %d!\n", __LINE__);

    m = env->GetMethodID(bitmapClass, "writeToParcel", "(Landroid/os/Parcel;I)V");
    CheckErrorAndLog(env, "GetMethodID: writeToParcel : %d!\n", __LINE__);

    env->CallVoidMethod(jbitmap, m, jparcel, 0);
    CheckErrorAndLog(env, "CallVoidMethod: writeToParcel : %d!\n", __LINE__);

    Int64 nativePtr = GetJavalongField(env, jparcel, "mNativePtr", "GetElBitmap");
    android::Parcel* source = reinterpret_cast< android::Parcel*>(nativePtr);

    AutoPtr<IParcel> parcel;
    CParcel::New((IParcel**)&parcel);

    android::Parcel* dest;
    parcel->GetElementPayload((Handle32*)&dest);

    dest->appendFrom(source, 0, source->dataSize());
    dest->setDataPosition(0);

    CBitmap::New(bitmap);
    IParcelable::Probe(*bitmap)->ReadFromParcel(parcel);

    env->DeleteLocalRef(parcelClass);
    env->DeleteLocalRef(jparcel);
    env->DeleteLocalRef(bitmapClass);

    return TRUE;
}

jobject Util::ToJavaBitmap(
    /* [in] */ JNIEnv* env,
    /* [in] */ IBitmap* bitmap)
{
    if (env == NULL || bitmap == NULL){
        Logger::E("Util", "ToJavaBitmap() INVALID params");
        return NULL;
    }

    AutoPtr<IParcel> parcel;
    CParcel::New((IParcel**)&parcel);

    IParcelable::Probe(bitmap)->ReadFromParcel(parcel);
    parcel->SetDataPosition(0);
    Handle32 source;
    parcel->GetElementPayload(&source);

    jclass parcelClass = env->FindClass("android/os/Parcel");
    CheckErrorAndLog(env, "FindClass: Parcel : %d!\n", __LINE__);

    jmethodID m = env->GetStaticMethodID(parcelClass, "obtain", "(J)Landroid/os/Parcel;");
    CheckErrorAndLog(env, "GetMethodID: obtain : %d!\n", __LINE__);

    jobject jparcel = env->CallStaticObjectMethod(parcelClass, m, (Int64)source);
    CheckErrorAndLog(env, "CallStaticObjectMethod: obtain : %d!\n", __LINE__);

    jclass bitmapClass = env->FindClass("android/graphics/Bitmap");
    CheckErrorAndLog(env, "ToJavaBitmap", "FindClass: Bitmap : %d!\n", __LINE__);

    m = env->GetStaticMethodID(bitmapClass, "nativeCreateFromParcel", "(Landroid/os/Parcel;)Landroid/graphics/Bitmap;");
    CheckErrorAndLog(env, "GetMethodID: nativeCreateFromParcel : %d!\n", __LINE__);

    jobject jbitmap = env->CallStaticObjectMethod(bitmapClass, m, jparcel);
    CheckErrorAndLog(env, "CallStaticObjectMethod: nativeCreateFromParcel : %d!\n", __LINE__);

    env->DeleteLocalRef(parcelClass);
    env->DeleteLocalRef(jparcel);
    env->DeleteLocalRef(bitmapClass);

    return jbitmap;
}

jobject Util::ToJavaConfiguration(
    /* [in] */ JNIEnv* env,
    /* [in] */ IConfiguration* configuration)
{
    if (env == NULL || configuration == NULL) {
        LOGGERE("ToJavaConfiguration", "Invalid arguments!");
        return NULL;
    }

    jobject jConfiguration = NULL;

    jclass configKlass = env->FindClass("android/content/res/Configuration");
    Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail FindClass: Configuration %d", __LINE__);

    jmethodID m = env->GetMethodID(configKlass, "<init>", "()V");
    Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail GetMethodID: configKlass %d", __LINE__);

    jConfiguration = env->NewObject(configKlass, m);
    Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail NewObject: configKlass %d", __LINE__);

    Float tmpFloat;
    configuration->GetFontScale(&tmpFloat);
    jfieldID f = env->GetFieldID(configKlass, "fontScale", "F");
    Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail GetFieldID: fontScale %d", __LINE__);

    env->SetFloatField(jConfiguration, f, tmpFloat);
    Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail SetIntField: fontScale %d", __LINE__);

    Int32 tempInt;
    configuration->GetMcc(&tempInt);
    f = env->GetFieldID(configKlass, "mcc", "I");
    Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail GetFieldID: mcc %d", __LINE__);

    env->SetIntField(jConfiguration, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail SetIntField: mcc %d", __LINE__);

    configuration->GetMnc(&tempInt);
    f = env->GetFieldID(configKlass, "mnc", "I");
    Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail GetFieldID: mnc %d", __LINE__);

    env->SetIntField(jConfiguration, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail SetIntField: mnc %d", __LINE__);

    AutoPtr<ILocale> locale;
    configuration->GetLocale((ILocale**)&locale);
    if (locale != NULL) {
        jobject jLocale = Util::ToJavaLocale(env, locale);

        f = env->GetFieldID(configKlass, "locale", "Ljava/util/Locale;");
        Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail GetFieldID: locale %d", __LINE__);

        env->SetObjectField(jConfiguration, f, jLocale);
        Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail SetObjectField: locale %d", __LINE__);
        env->DeleteLocalRef(jLocale);
    }
    else {
        // TODO: Eric locale is NULL ?
        LOGGERE("Eric", "ToJavaConfiguration() locale is NULL");
        jclass localKlass = env->FindClass("java/util/Locale");
        Util::CheckErrorAndLog(env, "ToJavaLocale", "Fail FindClass: Locale %d", __LINE__);

        f = env->GetStaticFieldID(localKlass, "SIMPLIFIED_CHINESE", "Ljava/util/Locale;");
        Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail GetStaticFieldID: SIMPLIFIED_CHINESE : %d", __LINE__);

        jobject jLocale = env->GetStaticObjectField(localKlass, f);
        Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail GetStaticObjectField: %d", __LINE__);

        f = env->GetFieldID(configKlass, "locale", "Ljava/util/Locale;");
        Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail GetFieldID: locale %d", __LINE__);

        env->SetObjectField(jConfiguration, f, jLocale);
        Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail SetObjectField: locale %d", __LINE__);
        env->DeleteLocalRef(localKlass);
        env->DeleteLocalRef(jLocale);
    }

    Boolean tempBool;
    configuration->IsUserSetLocale(&tempBool);
    f = env->GetFieldID(configKlass, "userSetLocale", "Z");
    Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail GetFieldID: userSetLocale %d", __LINE__);

    env->SetBooleanField(jConfiguration, f, (jboolean)tempBool);
    Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail SetBooleanField: userSetLocale %d", __LINE__);

    configuration->GetTouchscreen(&tempInt);
    f = env->GetFieldID(configKlass, "touchscreen", "I");
    Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail GetFieldID: touchscreen %d", __LINE__);

    env->SetIntField(jConfiguration, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail SetIntField: touchscreen %d", __LINE__);\

    configuration->GetKeyboard(&tempInt);
    f = env->GetFieldID(configKlass, "keyboard", "I");
    Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail GetFieldID: keyboard %d", __LINE__);

    env->SetIntField(jConfiguration, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail SetIntField: keyboard %d", __LINE__);

    configuration->GetKeyboardHidden(&tempInt);
    f = env->GetFieldID(configKlass, "keyboardHidden", "I");
    Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail GetFieldID: keyboardHidden %d", __LINE__);

    env->SetIntField(jConfiguration, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail SetIntField: keyboardHidden %d", __LINE__);

    configuration->GetHardKeyboardHidden(&tempInt);
    f = env->GetFieldID(configKlass, "hardKeyboardHidden", "I");
    Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail GetFieldID: hardKeyboardHidden %d", __LINE__);

    env->SetIntField(jConfiguration, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail SetIntField: hardKeyboardHidden %d", __LINE__);

    configuration->GetNavigation(&tempInt);
    f = env->GetFieldID(configKlass, "navigation", "I");
    Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail GetFieldID: navigation %d", __LINE__);

    env->SetIntField(jConfiguration, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail SetIntField: navigation %d", __LINE__);

    configuration->GetNavigationHidden(&tempInt);
    f = env->GetFieldID(configKlass, "navigationHidden", "I");
    Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail GetFieldID: navigationHidden %d", __LINE__);

    env->SetIntField(jConfiguration, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail SetIntField: navigationHidden %d", __LINE__);

    configuration->GetOrientation(&tempInt);
    f = env->GetFieldID(configKlass, "orientation", "I");
    Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail GetFieldID: orientation %d", __LINE__);

    env->SetIntField(jConfiguration, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail SetIntField: orientation %d", __LINE__);

    configuration->GetScreenLayout(&tempInt);
    f = env->GetFieldID(configKlass, "screenLayout", "I");
    Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail GetFieldID: screenLayout %d", __LINE__);

    env->SetIntField(jConfiguration, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail SetIntField: screenLayout %d", __LINE__);

    configuration->GetUiMode(&tempInt);
    f = env->GetFieldID(configKlass, "uiMode", "I");
    Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail GetFieldID: uiMode %d", __LINE__);

    env->SetIntField(jConfiguration, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail SetIntField: uiMode %d", __LINE__);

    configuration->GetScreenWidthDp(&tempInt);
    f = env->GetFieldID(configKlass, "screenWidthDp", "I");
    Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail GetFieldID: screenWidthDp %d", __LINE__);

    env->SetIntField(jConfiguration, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail SetIntField: screenWidthDp %d", __LINE__);

    configuration->GetScreenHeightDp(&tempInt);
    f = env->GetFieldID(configKlass, "screenHeightDp", "I");
    Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail GetFieldID: screenHeightDp %d", __LINE__);

    env->SetIntField(jConfiguration, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail SetIntField: screenHeightDp %d", __LINE__);

    configuration->GetSmallestScreenWidthDp(&tempInt);
    f = env->GetFieldID(configKlass, "smallestScreenWidthDp", "I");
    Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail GetFieldID: smallestScreenWidthDp %d", __LINE__);

    env->SetIntField(jConfiguration, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail SetIntField: smallestScreenWidthDp %d", __LINE__);

    configuration->GetDensityDpi(&tempInt);
    f = env->GetFieldID(configKlass, "densityDpi", "I");
    Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail GetFieldID: densityDpi %d", __LINE__);

    env->SetIntField(jConfiguration, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail SetIntField: densityDpi %d", __LINE__);

    configuration->GetCompatScreenWidthDp(&tempInt);
    f = env->GetFieldID(configKlass, "compatScreenWidthDp", "I");
    Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail GetFieldID: compatScreenWidthDp %d", __LINE__);

    env->SetIntField(jConfiguration, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail SetIntField: compatScreenWidthDp %d", __LINE__);

    configuration->GetCompatScreenHeightDp(&tempInt);
    f = env->GetFieldID(configKlass, "compatScreenHeightDp", "I");
    Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail GetFieldID: compatScreenHeightDp %d", __LINE__);

    env->SetIntField(jConfiguration, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail SetIntField: compatScreenHeightDp %d", __LINE__);

    configuration->GetCompatSmallestScreenWidthDp(&tempInt);
    f = env->GetFieldID(configKlass, "compatSmallestScreenWidthDp", "I");
    Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail GetFieldID: compatSmallestScreenWidthDp %d", __LINE__);

    env->SetIntField(jConfiguration, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail SetIntField: compatSmallestScreenWidthDp %d", __LINE__);

    configuration->GetSeq(&tempInt);
    f = env->GetFieldID(configKlass, "seq", "I");
    Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail GetFieldID: seq %d", __LINE__);

    env->SetIntField(jConfiguration, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaConfiguration", "Fail SetIntField: seq %d", __LINE__);

    //TODO:CM12 themeConfig

    env->DeleteLocalRef(configKlass);

    return jConfiguration;
}

jobject Util::ToJavaLocale(
    /* [in] */ JNIEnv* env,
    /* [in] */ ILocale* locale)
{
    if (env == NULL || locale == NULL) {
        LOGGERE("ToJavaLocale", "Invalid arguments!");
        return NULL;
    }

    jobject jLocale = NULL;

    jclass localKlass = env->FindClass("java/util/Locale");
    Util::CheckErrorAndLog(env, "ToJavaLocale", "Fail FindClass: Locale %d", __LINE__);

    AutoPtr<ILocaleHelper> localeHelp;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&localeHelp);

    jfieldID f = NULL;
    AutoPtr<ILocale> slocale;
    Boolean result = FALSE;

    if ((localeHelp->GetSIMPLIFIED_CHINESE((ILocale**)&slocale), slocale->Equals(locale, &result), result)) {
        f = env->GetStaticFieldID(localKlass, "SIMPLIFIED_CHINESE", "Ljava/util/Locale;");
        Util::CheckErrorAndLog(env, "ToJavaLocale", "Fail GetStaticFieldID: SIMPLIFIED_CHINESE : %d", __LINE__);
    }
    else if ((localeHelp->GetTRADITIONAL_CHINESE((ILocale**)&slocale), slocale->Equals(locale, &result), result)) {
        f = env->GetStaticFieldID(localKlass, "TRADITIONAL_CHINESE", "Ljava/util/Locale;");
        Util::CheckErrorAndLog(env, "ToJavaLocale", "Fail GetStaticFieldID: TRADITIONAL_CHINESE : %d", __LINE__);
    }
    else if ((localeHelp->GetUS((ILocale**)&slocale), slocale->Equals(locale, &result), result)) {
        f = env->GetStaticFieldID(localKlass, "US", "Ljava/util/Locale;");
        Util::CheckErrorAndLog(env, "ToJavaLocale", "Fail GetStaticFieldID: US : %d", __LINE__);
    }  else if ((localeHelp->GetUK((ILocale**)&slocale), slocale->Equals(locale, &result), result)) {
        f = env->GetStaticFieldID(localKlass, "UK", "Ljava/util/Locale;");
        Util::CheckErrorAndLog(env, "ToJavaLocale", "Fail GetStaticFieldID: UK : %d", __LINE__);
    }
    else if ((localeHelp->GetCANADA((ILocale**)&slocale), slocale->Equals(locale, &result), result)) {
        f = env->GetStaticFieldID(localKlass, "CANADA", "Ljava/util/Locale;");
        Util::CheckErrorAndLog(env, "ToJavaLocale", "Fail GetStaticFieldID: CANADA : %d", __LINE__);
    }
    else if ((localeHelp->GetCANADA_FRENCH((ILocale**)&slocale), slocale->Equals(locale, &result), result)) {
        f = env->GetStaticFieldID(localKlass, "CANADA_FRENCH", "Ljava/util/Locale;");
        Util::CheckErrorAndLog(env, "ToJavaLocale", "Fail GetStaticFieldID: CANADA_FRENCH : %d", __LINE__);
    }
    else if ((localeHelp->GetCHINA((ILocale**)&slocale), slocale->Equals(locale, &result), result)) {
        f = env->GetStaticFieldID(localKlass, "CHINA", "Ljava/util/Locale;");
        Util::CheckErrorAndLog(env, "ToJavaLocale", "Fail GetStaticFieldID: CHINA : %d", __LINE__);
    }
    else if ((localeHelp->GetCHINESE((ILocale**)&slocale), slocale->Equals(locale, &result), result)) {
        f = env->GetStaticFieldID(localKlass, "CHINESE", "Ljava/util/Locale;");
        Util::CheckErrorAndLog(env, "ToJavaLocale", "Fail GetStaticFieldID: CHINESE : %d", __LINE__);
    }
    else if ((localeHelp->GetENGLISH((ILocale**)&slocale), slocale->Equals(locale, &result), result)) {
        f = env->GetStaticFieldID(localKlass, "ENGLISH", "Ljava/util/Locale;");
        Util::CheckErrorAndLog(env, "ToJavaLocale", "Fail GetStaticFieldID: ENGLISH : %d", __LINE__);
    }
    else if ((localeHelp->GetFRANCE((ILocale**)&slocale), slocale->Equals(locale, &result), result)) {
        f = env->GetStaticFieldID(localKlass, "FRANCE", "Ljava/util/Locale;");
        Util::CheckErrorAndLog(env, "ToJavaLocale", "Fail GetStaticFieldID: FRANCE : %d", __LINE__);
    }
    else if ((localeHelp->GetGERMAN((ILocale**)&slocale), slocale->Equals(locale, &result), result)) {
        f = env->GetStaticFieldID(localKlass, "GERMAN", "Ljava/util/Locale;");
        Util::CheckErrorAndLog(env, "ToJavaLocale", "Fail GetStaticFieldID: GERMAN : %d", __LINE__);
    }
    else if ((localeHelp->GetGERMANY((ILocale**)&slocale), slocale->Equals(locale, &result), result)) {
        f = env->GetStaticFieldID(localKlass, "GERMANY", "Ljava/util/Locale;");
        Util::CheckErrorAndLog(env, "ToJavaLocale", "Fail GetStaticFieldID: GERMANY : %d", __LINE__);
    }
    else if ((localeHelp->GetITALIAN((ILocale**)&slocale), slocale->Equals(locale, &result), result)) {
        f = env->GetStaticFieldID(localKlass, "ITALIAN", "Ljava/util/Locale;");
        Util::CheckErrorAndLog(env, "ToJavaLocale", "Fail GetStaticFieldID: ITALIAN : %d", __LINE__);
    }
    else if ((localeHelp->GetITALY((ILocale**)&slocale), slocale->Equals(locale, &result), result)) {
        f = env->GetStaticFieldID(localKlass, "ITALY", "Ljava/util/Locale;");
        Util::CheckErrorAndLog(env, "ToJavaLocale", "Fail GetStaticFieldID: ITALY : %d", __LINE__);
    }
    else if ((localeHelp->GetJAPAN((ILocale**)&slocale), slocale->Equals(locale, &result), result)) {
        f = env->GetStaticFieldID(localKlass, "JAPAN", "Ljava/util/Locale;");
        Util::CheckErrorAndLog(env, "ToJavaLocale", "Fail GetStaticFieldID: JAPAN : %d", __LINE__);
    }
    else if ((localeHelp->GetJAPANESE((ILocale**)&slocale), slocale->Equals(locale, &result), result)) {
        f = env->GetStaticFieldID(localKlass, "JAPANESE", "Ljava/util/Locale;");
        Util::CheckErrorAndLog(env, "ToJavaLocale", "Fail GetStaticFieldID: JAPANESE : %d", __LINE__);
    }
    else if ((localeHelp->GetKOREA((ILocale**)&slocale), slocale->Equals(locale, &result), result)) {
        f = env->GetStaticFieldID(localKlass, "KOREA", "Ljava/util/Locale;");
        Util::CheckErrorAndLog(env, "ToJavaLocale", "Fail GetStaticFieldID: KOREA : %d", __LINE__);
    }
    else if ((localeHelp->GetKOREAN((ILocale**)&slocale), slocale->Equals(locale, &result), result)) {
        f = env->GetStaticFieldID(localKlass, "KOREAN", "Ljava/util/Locale;");
        Util::CheckErrorAndLog(env, "ToJavaLocale", "Fail GetStaticFieldID: KOREAN : %d", __LINE__);
    }
    else if ((localeHelp->GetPRC((ILocale**)&slocale), slocale->Equals(locale, &result), result)) {
        f = env->GetStaticFieldID(localKlass, "PRC", "Ljava/util/Locale;");
        Util::CheckErrorAndLog(env, "ToJavaLocale", "Fail GetStaticFieldID: PRC : %d", __LINE__);
    }
    else if ((localeHelp->GetROOT((ILocale**)&slocale), slocale->Equals(locale, &result), result)) {
        f = env->GetStaticFieldID(localKlass, "ROOT", "Ljava/util/Locale;");
        Util::CheckErrorAndLog(env, "ToJavaLocale", "Fail GetStaticFieldID: ROOT : %d", __LINE__);
    }
    else if ((localeHelp->GetTAIWAN((ILocale**)&slocale), slocale->Equals(locale, &result), result)) {
        f = env->GetStaticFieldID(localKlass, "TAIWAN", "Ljava/util/Locale;");
        Util::CheckErrorAndLog(env, "ToJavaLocale", "Fail GetStaticFieldID: TAIWAN : %d", __LINE__);
    }
    else {
        String language;
        locale->GetLanguage(&language);
        jstring jlanguage = ToJavaString(env, language);
        String country;
        locale->GetCountry(&country);
        jstring jcountry = ToJavaString(env, country);
        String variant;
        locale->GetVariant(&variant);
        jstring jvariant = ToJavaString(env, variant);

        jmethodID m = env->GetMethodID(localKlass, "<init>", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
        CheckErrorAndLog(env, "ToJavaLocale", "Fail GetMethodID: Locale %d", __LINE__);

        jLocale = env->NewObject(localKlass, m, jlanguage, jcountry, jvariant);
        CheckErrorAndLog(env, "ToJavaLocale", "Fail NewObject: Locale %d", __LINE__);

        env->DeleteLocalRef(localKlass);
        env->DeleteLocalRef(jlanguage);
        env->DeleteLocalRef(jcountry);
        env->DeleteLocalRef(jvariant);

        return jLocale;
    }

    jLocale = env->GetStaticObjectField(localKlass, f);
    Util::CheckErrorAndLog(env, "ToJavaLocale", "Fail GetStaticObjectField: %d", __LINE__);

    env->DeleteLocalRef(localKlass);

    return jLocale;
}

jobject Util::ToJavaRect(
    /* [in] */ JNIEnv* env,
    /* [in] */ IRect* rect)
{
    if (env == NULL || rect == NULL) {
        LOGGERE("ToJavaRect", "Invalid arguments!");
        return NULL;
    }

    jobject jRect = NULL;

    jclass rectKlass = env->FindClass("android/graphics/Rect");
    CheckErrorAndLog(env, "ToJavaRect", "Fail FindClass: Rect %d", __LINE__);

    jmethodID m = env->GetMethodID(rectKlass, "<init>", "(IIII)V");
    CheckErrorAndLog(env, "ToJavaRect", "Fail GetMethodID: appInfoKlass %d", __LINE__);

    Int32 left;
    rect->GetLeft(&left);
    Int32 top;
    rect->GetTop(&top);
    Int32 right;
    rect->GetRight(&right);
    Int32 bottom;
    rect->GetBottom(&bottom);

    jRect = env->NewObject(rectKlass, m, left, top, right, bottom);
    CheckErrorAndLog(env, "ToJavaRect", "Fail NewObject: Rect %d", __LINE__);

    env->DeleteLocalRef(rectKlass);

    return jRect;
}

jobject Util::ToJavaProviderInfo(
    /* [in] */ JNIEnv* env,
    /* [in] */ IProviderInfo* providerInfo)
{
    if (env == NULL || providerInfo == NULL) {
        LOGGERE("ToJavaProviderInfo", "Invalid arguments!");
        return NULL;
    }

    jobject jproviderInfo = NULL;

    jclass piKlass = env->FindClass("android/content/pm/ProviderInfo");
    CheckErrorAndLog(env, "ToJavaProviderInfo", "Fail FindClass: ProviderInfo %d", __LINE__);

    jmethodID m = env->GetMethodID(piKlass, "<init>", "()V");
    CheckErrorAndLog(env, "ToJavaProviderInfo", "Fail GetMethodID: ProviderInfo %d", __LINE__);

    jproviderInfo = env->NewObject(piKlass, m);
    CheckErrorAndLog(env, "ToJavaProviderInfo", "Fail NewObject: ProviderInfo %d", __LINE__);

    String authority;
    providerInfo->GetAuthority(&authority);
    jstring jauthority = Util::ToJavaString(env, authority);
    if (jauthority != NULL) {
        jfieldID f = env->GetFieldID(piKlass, "authority", "Ljava/lang/String;");
        Util::CheckErrorAndLog(env, "ToJavaProviderInfo", "Fail GetFieldID: authority %d", __LINE__);

        env->SetObjectField(jproviderInfo, f, jauthority);
        Util::CheckErrorAndLog(env, "ToJavaProviderInfo", "Fail SetIntField: jauthority %d", __LINE__);
        env->DeleteLocalRef(jauthority);
    }

    String readPermission;
    providerInfo->GetReadPermission(&readPermission);
    jstring jreadPermission = Util::ToJavaString(env, readPermission);
    if (jreadPermission != NULL) {
        jfieldID f = env->GetFieldID(piKlass, "readPermission", "Ljava/lang/String;");
        Util::CheckErrorAndLog(env, "ToJavaProviderInfo", "Fail GetFieldID: readPermission %d", __LINE__);

        env->SetObjectField(jproviderInfo, f, jreadPermission);
        Util::CheckErrorAndLog(env, "ToJavaProviderInfo", "Fail SetIntField: jreadPermission %d", __LINE__);
        env->DeleteLocalRef(jreadPermission);
    }

    String writePermission;
    providerInfo->GetWritePermission(&writePermission);
    jstring jwritePermission = Util::ToJavaString(env, writePermission);
    if (jwritePermission != NULL) {
        jfieldID f = env->GetFieldID(piKlass, "writePermission", "Ljava/lang/String;");
        Util::CheckErrorAndLog(env, "ToJavaProviderInfo", "Fail GetFieldID: writePermission %d", __LINE__);

        env->SetObjectField(jproviderInfo, f, jwritePermission);
        Util::CheckErrorAndLog(env, "ToJavaProviderInfo", "Fail SetIntField: jwritePermission %d", __LINE__);
        env->DeleteLocalRef(jwritePermission);
    }

    Boolean tempBool;
    providerInfo->GetGrantUriPermissions(&tempBool);
    jfieldID f = env->GetFieldID(piKlass, "grantUriPermissions", "Z");
    Util::CheckErrorAndLog(env, "ToJavaProviderInfo", "Fail GetFieldID: grantUriPermissions %d", __LINE__);

    env->SetBooleanField(jproviderInfo, f, (jboolean)tempBool);
    Util::CheckErrorAndLog(env, "ToJavaProviderInfo", "Fail SetBooleanField: grantUriPermissions %d", __LINE__);

    AutoPtr<ArrayOf<IPatternMatcher*> > uriPermissionPatterns;
    providerInfo->GetUriPermissionPatterns((ArrayOf<IPatternMatcher*>**)&uriPermissionPatterns);
    if (uriPermissionPatterns != NULL) {
        jclass pmerKlass = env->FindClass("android/os/PatternMatcher");
        Util::CheckErrorAndLog(env, "ToJavaProviderInfo", "Fail FindClass: PatternMatcher %d", __LINE__);

        Int32 count = uriPermissionPatterns->GetLength();
        jobjectArray jpmerArray = env->NewObjectArray((jsize)count, pmerKlass, NULL);
        Util::CheckErrorAndLog(env, "ToJavaProviderInfo", "Fail NewObjectArray: PatternMatcher %d", __LINE__);

        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IPatternMatcher> matcher = (*uriPermissionPatterns)[i];
            String path;
            matcher->GetPath(&path);
            jstring jpath = Util::ToJavaString(env, path);
            Int32 type;
            matcher->GetType(&type);

            m = env->GetMethodID(pmerKlass, "<init>", "(Ljava/lang/String;I)V");
            CheckErrorAndLog(env, "ToJavaProviderInfo", "Fail GetMethodID: PatternMatcher %d", __LINE__);

            jobject jmatcher = env->NewObject(pmerKlass, m, jpath, (jint)type);
            CheckErrorAndLog(env, "ToJavaProviderInfo", "Fail NewObject: PatternMatcher %d", __LINE__);

            env->SetObjectArrayElement(jpmerArray, i, jmatcher);
            CheckErrorAndLog(env, "ToJavaProviderInfo", "Fail SetObjectArrayElement: PatternMatcher %d", __LINE__);

            env->DeleteLocalRef(jpath);
            env->DeleteLocalRef(jmatcher);
        }

        f = env->GetFieldID(piKlass, "uriPermissionPatterns", "[Landroid/os/PatternMatcher;");
        Util::CheckErrorAndLog(env, "ToJavaProviderInfo", "Fail GetFieldID: uriPermissionPatterns[] %d", __LINE__);

        env->SetObjectField(jproviderInfo, f, jpmerArray);
        Util::CheckErrorAndLog(env, "ToJavaProviderInfo", "Fail SetObjectField: jpmerArray %d", __LINE__);

        env->DeleteLocalRef(jpmerArray);
        env->DeleteLocalRef(pmerKlass);
    }

    AutoPtr<ArrayOf<IPathPermission*> > pathPermissions;
    providerInfo->GetPathPermissions((ArrayOf<IPathPermission*>**)&pathPermissions);
    if (pathPermissions != NULL) {
        jclass ppmissionsKlass = env->FindClass("android/content/pm/PathPermission");
        Util::CheckErrorAndLog(env, "ToJavaProviderInfo", "Fail FindClass: PathPermission %d", __LINE__);

        Int32 count = pathPermissions->GetLength();
        jobjectArray jppmissionsArray = env->NewObjectArray((jsize)count, ppmissionsKlass, 0);
        Util::CheckErrorAndLog(env, "ToJavaProviderInfo", "Fail NewObjectArray: PathPermission %d", __LINE__);

        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IPathPermission> permission = (*pathPermissions)[i];
            String path;
            IPatternMatcher::Probe(permission)->GetPath(&path);
            jstring jpath = Util::ToJavaString(env, path);
            Int32 type;
            IPatternMatcher::Probe(permission)->GetType(&type);
            String readPermission;
            permission->GetReadPermission(&readPermission);
            jstring jreadPermission = Util::ToJavaString(env, readPermission);
            String writePermission;
            permission->GetWritePermission(&writePermission);
            jstring jwritePermission = Util::ToJavaString(env, writePermission);

            m = env->GetMethodID(ppmissionsKlass, "<init>", "(Ljava/lang/String;ILjava/lang/String;Ljava/lang/String;)V");
            CheckErrorAndLog(env, "ToJavaProviderInfo", "Fail GetMethodID: PathPermission %d", __LINE__);

            jobject jppmissions = env->NewObject(ppmissionsKlass, m, jpath, (jint)type, jreadPermission, jwritePermission);
            CheckErrorAndLog(env, "ToJavaProviderInfo", "Fail NewObject: PathPermission %d", __LINE__);

            env->SetObjectArrayElement(jppmissionsArray, i, jppmissions);
            CheckErrorAndLog(env, "ToJavaProviderInfo", "Fail SetObjectArrayElement: PathPermission %d", __LINE__);

            env->DeleteLocalRef(jpath);
            env->DeleteLocalRef(jreadPermission);
            env->DeleteLocalRef(jwritePermission);
            env->DeleteLocalRef(jppmissions);
        }

        f = env->GetFieldID(piKlass, "pathPermissions", "[Landroid/content/pm/PathPermission;");
        Util::CheckErrorAndLog(env, "ToJavaProviderInfo", "Fail GetFieldID: pathPermissions[] %d", __LINE__);

        env->SetObjectField(jproviderInfo, f, jppmissionsArray);
        Util::CheckErrorAndLog(env, "ToJavaProviderInfo", "Fail SetObjectField: jppmissionsArray %d", __LINE__);

        env->DeleteLocalRef(ppmissionsKlass);
        env->DeleteLocalRef(jppmissionsArray);
    }

    providerInfo->GetMultiprocess(&tempBool);
    f = env->GetFieldID(piKlass, "multiprocess", "Z");
    Util::CheckErrorAndLog(env, "ToJavaProviderInfo", "Fail GetFieldID: multiprocess %d", __LINE__);

    env->SetBooleanField(jproviderInfo, f, (jboolean)tempBool);
    Util::CheckErrorAndLog(env, "ToJavaProviderInfo", "Fail SetBooleanField: multiprocess %d", __LINE__);

    Int32 tempInt;
    providerInfo->GetInitOrder(&tempInt);
    f = env->GetFieldID(piKlass, "initOrder", "I");
    Util::CheckErrorAndLog(env, "ToJavaProviderInfo", "Fail GetFieldID: initOrder %d", __LINE__);

    env->SetIntField(jproviderInfo, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaProviderInfo", "Fail SetIntField: initOrder %d", __LINE__);

    providerInfo->GetFlags(&tempInt);
    f = env->GetFieldID(piKlass, "flags", "I");
    Util::CheckErrorAndLog(env, "ToJavaProviderInfo", "Fail GetFieldID: flags %d", __LINE__);

    env->SetIntField(jproviderInfo, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaProviderInfo", "Fail SetIntField: flags %d", __LINE__);

    providerInfo->GetIsSyncable(&tempBool);
    f = env->GetFieldID(piKlass, "isSyncable", "Z");
    Util::CheckErrorAndLog(env, "ToJavaProviderInfo", "Fail GetFieldID: isSyncable %d", __LINE__);

    env->SetBooleanField(jproviderInfo, f, (jboolean)tempBool);
    Util::CheckErrorAndLog(env, "ToJavaProviderInfo", "Fail SetBooleanField: isSyncable %d", __LINE__);

    AutoPtr<IComponentInfo> comInfo = IComponentInfo::Probe(providerInfo);
    SetComponentInfo(env, piKlass, jproviderInfo, comInfo);

    env->DeleteLocalRef(piKlass);

    return jproviderInfo;
}

String Util::GetElString(
    /* [in] */ JNIEnv* env,
    /* [in] */ jstring jstr)
{
    if (env == NULL || jstr == NULL) {
        return String(NULL);
    }

    const char* str = NULL;
    str = env->GetStringUTFChars(jstr, NULL);
    String elStr = String(str);
    env->ReleaseStringUTFChars(jstr, str);

    return elStr;
}

Boolean Util::GetElCharSequence(
    /* [in] */ JNIEnv* env,
    /* [in] */ jobject jcs,
    /* [out] */ ICharSequence** elCs)
{
    if (env == NULL || elCs == NULL) {
        LOGGERD(TAG, "GetElCharSequence: Invalid argumenet!");
        *elCs = NULL;
        return false;
    }

    jclass csClass = env->FindClass("java/lang/CharSequence");
    CheckErrorAndLog(env, "FindClass CharSequence : %d!\n", __LINE__);

    jmethodID m = env->GetMethodID(csClass, "toString", "()Ljava/lang/String;");
    CheckErrorAndLog(env, "GetMethodID toString : %d!\n", __LINE__);

    env->DeleteLocalRef(csClass);

    jstring jtmpString = (jstring)env->CallObjectMethod(jcs, m);
    CheckErrorAndLog(env, "CallObjectMethod toString : %d!\n", __LINE__);

    if (jtmpString != NULL) {
        if (NOERROR != CString::New(GetElString(env, jtmpString), elCs)) {
            LOGGERE(TAG, "GetElCharSequence: create CConfiguration fail!");
            *elCs = NULL;
            return false;
        }

        env->DeleteLocalRef(jtmpString);
    }

    return true;
}

Boolean Util::GetElApplicationInfo(
    /* [in] */ JNIEnv* env,
    /* [in] */ jobject jappInfo,
    /* [out] */ IApplicationInfo** appInfo)
{
    if (env == NULL || jappInfo == NULL || appInfo == NULL) {
        LOGGERE("GetElApplicationInfo", "Invalid arguments!");
        return FALSE;
    }

    if (NOERROR != CApplicationInfo::New(appInfo)) {
        LOGGERE("GetElApplicationInfo", "Create CApplicationInfo fail!");
        return FALSE;
    }

    jclass appInfoKlass = env->FindClass("android/content/pm/ApplicationInfo");
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail FindClass: ApplicationInfo %d", __LINE__);

    jfieldID f = env->GetFieldID(appInfoKlass, "taskAffinity", "Ljava/lang/String;");
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetFieldID: taskAffinity %d", __LINE__);

    jstring jtaskAffinity = (jstring)env->GetObjectField(jappInfo, f);
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetObjectField: jtaskAffinity %d", __LINE__);
    if (jtaskAffinity != NULL) {
        String taskAffinity = Util::GetElString(env, jtaskAffinity);
        env->DeleteLocalRef(jtaskAffinity);
        (*appInfo)->SetTaskAffinity(taskAffinity);
    }

    f = env->GetFieldID(appInfoKlass, "permission", "Ljava/lang/String;");
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetFieldID: permission %d", __LINE__);

    jstring jpermission = (jstring)env->GetObjectField(jappInfo, f);
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetObjectField: jpermission %d", __LINE__);
    if (jpermission != NULL) {
        String permission = Util::GetElString(env, jpermission);
        env->DeleteLocalRef(jpermission);
        (*appInfo)->SetPermission(permission);
    }

    f = env->GetFieldID(appInfoKlass, "processName", "Ljava/lang/String;");
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetFieldID: processName %d", __LINE__);

    jstring jprocessName = (jstring)env->GetObjectField(jappInfo, f);
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetObjectField: jprocessName %d", __LINE__);
    if (jprocessName != NULL) {
        String processName = Util::GetElString(env, jprocessName);
        env->DeleteLocalRef(jprocessName);
        (*appInfo)->SetProcessName(processName);
    }

    f = env->GetFieldID(appInfoKlass, "className", "Ljava/lang/String;");
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetFieldID: className %d", __LINE__);

    jstring jclassName = (jstring)env->GetObjectField(jappInfo, f);
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetObjectField: jclassName %d", __LINE__);
    if (jclassName != NULL) {
        String className = Util::GetElString(env, jclassName);
        env->DeleteLocalRef(jclassName);
        (*appInfo)->SetClassName(className);
    }

    f = env->GetFieldID(appInfoKlass, "theme", "I");
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetFieldID: theme %d", __LINE__);

    jint jtheme = env->GetIntField(jappInfo, f);
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetIntField: theme %d", __LINE__);
    (*appInfo)->SetTheme((Int32)jtheme);

    f = env->GetFieldID(appInfoKlass, "flags", "I");
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetFieldID: flags %d", __LINE__);

    jint jflags = env->GetIntField(jappInfo, f);
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetIntField: flags %d", __LINE__);
    (*appInfo)->SetFlags((Int32)jflags);

    f = env->GetFieldID(appInfoKlass, "requiresSmallestWidthDp", "I");
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetFieldID: requiresSmallestWidthDp %d", __LINE__);

    jint jrequiresSmallestWidthDp = env->GetIntField(jappInfo, f);
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetIntField: requiresSmallestWidthDp %d", __LINE__);
    (*appInfo)->SetRequiresSmallestWidthDp((Int32)jrequiresSmallestWidthDp);

    f = env->GetFieldID(appInfoKlass, "compatibleWidthLimitDp", "I");
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetFieldID: compatibleWidthLimitDp %d", __LINE__);

    jint jcompatibleWidthLimitDp = env->GetIntField(jappInfo, f);
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetIntField: compatibleWidthLimitDp %d", __LINE__);
    (*appInfo)->SetCompatibleWidthLimitDp((Int32)jcompatibleWidthLimitDp);

    f = env->GetFieldID(appInfoKlass, "largestWidthLimitDp", "I");
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetFieldID: largestWidthLimitDp %d", __LINE__);

    jint jlargestWidthLimitDp = env->GetIntField(jappInfo, f);
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetIntField: largestWidthLimitDp %d", __LINE__);
    (*appInfo)->SetLargestWidthLimitDp((Int32)jlargestWidthLimitDp);

    f = env->GetFieldID(appInfoKlass, "scanSourceDir", "Ljava/lang/String;");
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetFieldID: scanSourceDir %d", __LINE__);

    jstring jscanSourceDir = (jstring)env->GetObjectField(jappInfo, f);
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetObjectField: jscanSourceDir %d", __LINE__);
    if (jscanSourceDir != NULL) {
        String scanSourceDir = Util::GetElString(env, jscanSourceDir);
        env->DeleteLocalRef(jscanSourceDir);
        (*appInfo)->SetScanSourceDir(scanSourceDir);
    }

    f = env->GetFieldID(appInfoKlass, "scanPublicSourceDir", "Ljava/lang/String;");
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetFieldID: scanPublicSourceDir %d", __LINE__);

    jstring jscanPublicSourceDir = (jstring)env->GetObjectField(jappInfo, f);
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetObjectField: jscanPublicSourceDir %d", __LINE__);
    if (jscanPublicSourceDir != NULL) {
        String scanPublicSourceDir = Util::GetElString(env, jscanPublicSourceDir);
        env->DeleteLocalRef(jscanPublicSourceDir);
        (*appInfo)->SetScanPublicSourceDir(scanPublicSourceDir);
    }

    f = env->GetFieldID(appInfoKlass, "sourceDir", "Ljava/lang/String;");
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetFieldID: sourceDir %d", __LINE__);

    jstring jsourceDir = (jstring)env->GetObjectField(jappInfo, f);
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetObjectField: jsourceDir %d", __LINE__);
    if (jsourceDir != NULL) {
        String sourceDir = Util::GetElString(env, jsourceDir);
        env->DeleteLocalRef(jsourceDir);
        (*appInfo)->SetSourceDir(sourceDir);
    }

    f = env->GetFieldID(appInfoKlass, "publicSourceDir", "Ljava/lang/String;");
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetFieldID: publicSourceDir %d", __LINE__);

    jstring jpublicSourceDir = (jstring)env->GetObjectField(jappInfo, f);
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetObjectField: jpublicSourceDir %d", __LINE__);
    if (jpublicSourceDir != NULL) {
        String publicSourceDir = Util::GetElString(env, jpublicSourceDir);
        env->DeleteLocalRef(jpublicSourceDir);
        (*appInfo)->SetPublicSourceDir(publicSourceDir);
    }

    f = env->GetFieldID(appInfoKlass, "splitSourceDirs", "[Ljava/lang/String;");
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetFieldID: splitSourceDirs %d", __LINE__);

    jobjectArray jsplitSourceDirs = (jobjectArray)env->GetObjectField(jappInfo, f);
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetObjectField: jsplitSourceDirs %d", __LINE__);
    if (jsplitSourceDirs != NULL) {
        AutoPtr<ArrayOf<String> > splitSourceDirs;
        GetElStringArray(env, jsplitSourceDirs, (ArrayOf<String>**)&splitSourceDirs);
        if (splitSourceDirs != NULL) {
            (*appInfo)->SetSplitSourceDirs(splitSourceDirs);
        }

        env->DeleteLocalRef(jsplitSourceDirs);
    }

    f = env->GetFieldID(appInfoKlass, "splitPublicSourceDirs", "[Ljava/lang/String;");
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetFieldID: splitPublicSourceDirs %d", __LINE__);

    jobjectArray jsplitPublicSourceDirs = (jobjectArray)env->GetObjectField(jappInfo, f);
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetObjectField: jsplitPublicSourceDirs %d", __LINE__);
    if (jsplitPublicSourceDirs != NULL) {
        AutoPtr<ArrayOf<String> > splitPublicSourceDirs;
        GetElStringArray(env, jsplitPublicSourceDirs, (ArrayOf<String>**)&splitPublicSourceDirs);
        if (splitPublicSourceDirs != NULL) {
            (*appInfo)->SetSplitPublicSourceDirs(splitPublicSourceDirs);
        }

        env->DeleteLocalRef(jsplitPublicSourceDirs);
    }

    f = env->GetFieldID(appInfoKlass, "nativeLibraryDir", "Ljava/lang/String;");
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetFieldID: nativeLibraryDir %d", __LINE__);

    jstring jnativeLibraryDir = (jstring)env->GetObjectField(jappInfo, f);
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetObjectField: jnativeLibraryDir %d", __LINE__);
    if (jnativeLibraryDir != NULL) {
        String nativeLibraryDir = Util::GetElString(env, jnativeLibraryDir);
        env->DeleteLocalRef(jnativeLibraryDir);
        (*appInfo)->SetNativeLibraryDir(nativeLibraryDir);
    }

    f = env->GetFieldID(appInfoKlass, "secondaryNativeLibraryDir", "Ljava/lang/String;");
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetFieldID: secondaryNativeLibraryDir %d", __LINE__);

    jstring jsecondaryNativeLibraryDir = (jstring)env->GetObjectField(jappInfo, f);
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetObjectField: jsecondaryNativeLibraryDir %d", __LINE__);
    if (jsecondaryNativeLibraryDir != NULL) {
        String secondaryNativeLibraryDir = Util::GetElString(env, jsecondaryNativeLibraryDir);
        env->DeleteLocalRef(jsecondaryNativeLibraryDir);
        (*appInfo)->SetSecondaryNativeLibraryDir(secondaryNativeLibraryDir);
    }

    f = env->GetFieldID(appInfoKlass, "nativeLibraryRootDir", "Ljava/lang/String;");
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetFieldID: nativeLibraryRootDir %d", __LINE__);

    jstring jnativeLibraryRootDir = (jstring)env->GetObjectField(jappInfo, f);
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetObjectField: jnativeLibraryRootDir %d", __LINE__);
    if (jnativeLibraryRootDir != NULL) {
        String nativeLibraryRootDir = Util::GetElString(env, jnativeLibraryRootDir);
        env->DeleteLocalRef(jnativeLibraryRootDir);
        (*appInfo)->SetNativeLibraryRootDir(nativeLibraryRootDir);
    }

    f = env->GetFieldID(appInfoKlass, "nativeLibraryRootRequiresIsa", "Z");
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetFieldID: nativeLibraryRootRequiresIsa %d", __LINE__);

    jboolean jnativeLibraryRootRequiresIsa = env->GetBooleanField(jappInfo, f);
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetIntField: nativeLibraryRootRequiresIsa %d", __LINE__);
    (*appInfo)->SetNativeLibraryRootRequiresIsa((Boolean)jnativeLibraryRootRequiresIsa);

    f = env->GetFieldID(appInfoKlass, "primaryCpuAbi", "Ljava/lang/String;");
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetFieldID: primaryCpuAbi %d", __LINE__);

    jstring jprimaryCpuAbi = (jstring)env->GetObjectField(jappInfo, f);
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetObjectField: jprimaryCpuAbi %d", __LINE__);
    if (jprimaryCpuAbi != NULL) {
        String primaryCpuAbi = Util::GetElString(env, jprimaryCpuAbi);
        env->DeleteLocalRef(jprimaryCpuAbi);
        (*appInfo)->SetPrimaryCpuAbi(primaryCpuAbi);
    }

    f = env->GetFieldID(appInfoKlass, "secondaryCpuAbi", "Ljava/lang/String;");
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetFieldID: secondaryCpuAbi %d", __LINE__);

    jstring jsecondaryCpuAbi = (jstring)env->GetObjectField(jappInfo, f);
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetObjectField: jsecondaryCpuAbi %d", __LINE__);
    if (jsecondaryCpuAbi != NULL) {
        String secondaryCpuAbi = Util::GetElString(env, jsecondaryCpuAbi);
        env->DeleteLocalRef(jsecondaryCpuAbi);
        (*appInfo)->SetSecondaryCpuAbi(secondaryCpuAbi);
    }

    f = env->GetFieldID(appInfoKlass, "resourceDirs", "[Ljava/lang/String;");
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetFieldID: resourceDirs %d", __LINE__);

    jobjectArray jresourceDirs = (jobjectArray)env->GetObjectField(jappInfo, f);
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetObjectField: jresourceDirs %d", __LINE__);
    if (jresourceDirs != NULL) {
        jint jcount = env->GetArrayLength(jresourceDirs);
        if (jcount > 0) {
            AutoPtr<ArrayOf<String> > resourceDirs = ArrayOf<String>::Alloc((Int32)jcount);
            if (resourceDirs != NULL) {
                for (Int32 i = 0; i < jcount; i++) {
                    jstring jresourceDir = (jstring)env->GetObjectArrayElement(jresourceDirs, i);
                    CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetObjectArrayElement: jresourceDir %d", __LINE__);
                    String resourceDir = Util::GetElString(env, jresourceDir);
                    env->DeleteLocalRef(jresourceDir);
                    resourceDirs->Set(i, resourceDir);
                }
                (*appInfo)->SetResourceDirs(resourceDirs);
            }
        }

        env->DeleteLocalRef(jresourceDirs);
    }

    f = env->GetFieldID(appInfoKlass, "seinfo", "Ljava/lang/String;");
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetFieldID: seinfo %d", __LINE__);

    jstring jseinfo = (jstring)env->GetObjectField(jappInfo, f);
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetObjectField: jseinfo %d", __LINE__);
    if (jseinfo != NULL) {
        String seinfo = Util::GetElString(env, jseinfo);
        env->DeleteLocalRef(jseinfo);
        (*appInfo)->SetSeinfo(seinfo);
    }

    f = env->GetFieldID(appInfoKlass, "sharedLibraryFiles", "[Ljava/lang/String;");
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetFieldID: sharedLibraryFiles %d", __LINE__);

    jobjectArray jsharedLibraryFiles = (jobjectArray)env->GetObjectField(jappInfo, f);
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetObjectField: jsharedLibraryFiles %d", __LINE__);
    if (jsharedLibraryFiles != NULL) {
        jint jcount = env->GetArrayLength(jsharedLibraryFiles);
        if (jcount > 0) {
            AutoPtr<ArrayOf<String> > sharedLibraryFiles = ArrayOf<String>::Alloc((Int32)jcount);
            if (sharedLibraryFiles != NULL) {
                for (Int32 i = 0; i < jcount; i++) {
                    jstring jsharedLibraryFile = (jstring)env->GetObjectArrayElement(jsharedLibraryFiles, i);
                    CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetObjectArrayElement: jsharedLibraryFile %d", __LINE__);
                    String sharedLibraryFile = Util::GetElString(env, jsharedLibraryFile);
                    env->DeleteLocalRef(jsharedLibraryFile);
                    sharedLibraryFiles->Set(i, sharedLibraryFile);
                }
                (*appInfo)->SetResourceDirs(sharedLibraryFiles);
            }
        }

        env->DeleteLocalRef(jsharedLibraryFiles);
    }

    f = env->GetFieldID(appInfoKlass, "dataDir", "Ljava/lang/String;");
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetFieldID: dataDir %d", __LINE__);

    jstring jdataDir = (jstring)env->GetObjectField(jappInfo, f);
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetObjectField: jdataDir %d", __LINE__);
    if (jdataDir != NULL) {
        String dataDir = Util::GetElString(env, jdataDir);
        env->DeleteLocalRef(jdataDir);
        (*appInfo)->SetDataDir(dataDir);
    }

    f = env->GetFieldID(appInfoKlass, "uid", "I");
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetFieldID: uid %d", __LINE__);

    jint juid = env->GetIntField(jappInfo, f);
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetIntField: uid %d", __LINE__);
    (*appInfo)->SetUid((Int32)juid);

    f = env->GetFieldID(appInfoKlass, "targetSdkVersion", "I");
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetFieldID: targetSdkVersion %d", __LINE__);

    jint jtargetSdkVersion = env->GetIntField(jappInfo, f);
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetIntField: targetSdkVersion %d", __LINE__);
    (*appInfo)->SetTargetSdkVersion((Int32)jtargetSdkVersion);

    f = env->GetFieldID(appInfoKlass, "versionCode", "I");
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetFieldID: versionCode %d", __LINE__);

    jint jversionCode = env->GetIntField(jappInfo, f);
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetIntField: versionCode %d", __LINE__);
    (*appInfo)->SetVersionCode((Int32)jversionCode);

    f = env->GetFieldID(appInfoKlass, "enabled", "Z");
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetFieldID: enabled %d", __LINE__);

    jboolean jenabled = env->GetBooleanField(jappInfo, f);
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetIntField: enabled %d", __LINE__);
    (*appInfo)->SetEnabled((Boolean)jenabled);

    f = env->GetFieldID(appInfoKlass, "enabledSetting", "I");
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetFieldID: enabledSetting %d", __LINE__);

    jint jenabledSetting = env->GetIntField(jappInfo, f);
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetIntField: enabledSetting %d", __LINE__);
    (*appInfo)->SetEnabledSetting((Int32)jenabledSetting);

    f = env->GetFieldID(appInfoKlass, "installLocation", "I");
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetFieldID: installLocation %d", __LINE__);

    jint jinstallLocation = env->GetIntField(jappInfo, f);
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetIntField: installLocation %d", __LINE__);
    (*appInfo)->SetInstallLocation((Int32)jinstallLocation);

    f = env->GetFieldID(appInfoKlass, "manageSpaceActivityName", "Ljava/lang/String;");
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetFieldID: manageSpaceActivityName %d", __LINE__);

    jstring jmanageSpaceActivityName = (jstring)env->GetObjectField(jappInfo, f);
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetObjectField: jmanageSpaceActivityName %d", __LINE__);
    if (jmanageSpaceActivityName != NULL) {
        String manageSpaceActivityName = Util::GetElString(env, jmanageSpaceActivityName);
        env->DeleteLocalRef(jmanageSpaceActivityName);
        (*appInfo)->SetManageSpaceActivityName(manageSpaceActivityName);
    }

    f = env->GetFieldID(appInfoKlass, "descriptionRes", "I");
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetFieldID: descriptionRes %d", __LINE__);

    jint jdescriptionRes = env->GetIntField(jappInfo, f);
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetIntField: descriptionRes %d", __LINE__);
    (*appInfo)->SetDescriptionRes((Int32)jdescriptionRes);

    f = env->GetFieldID(appInfoKlass, "uiOptions", "I");
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetFieldID: uiOptions %d", __LINE__);

    jint juiOptions = env->GetIntField(jappInfo, f);
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetIntField: uiOptions %d", __LINE__);
    (*appInfo)->SetUiOptions((Int32)juiOptions);

    f = env->GetFieldID(appInfoKlass, "backupAgentName", "Ljava/lang/String;");
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetFieldID: backupAgentName %d", __LINE__);

    jstring jbackupAgentName = (jstring)env->GetObjectField(jappInfo, f);
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetObjectField: jbackupAgentName %d", __LINE__);
    if (jbackupAgentName != NULL) {
        String backupAgentName = Util::GetElString(env, jbackupAgentName);
        env->DeleteLocalRef(jbackupAgentName);
        (*appInfo)->SetBackupAgentName(backupAgentName);
    }

    f = env->GetFieldID(appInfoKlass, "protect", "Z");
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetFieldID: protect %d", __LINE__);

    jboolean jprotect = env->GetBooleanField(jappInfo, f);
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetIntField: protect %d", __LINE__);
    (*appInfo)->SetProtect((Boolean)jprotect);

    f = env->GetFieldID(appInfoKlass, "isThemeable", "Z");
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetFieldID: isThemeable %d", __LINE__);

    jboolean jisThemeable = env->GetBooleanField(jappInfo, f);
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetIntField: isThemeable %d", __LINE__);
    (*appInfo)->SetIsThemeable((Boolean)jisThemeable);

    // ----------------------------------- PackageItemInfo ---------------------------------------------------

    IPackageItemInfo* pkgInfo = IPackageItemInfo::Probe(*appInfo);
    f = env->GetFieldID(appInfoKlass, "name", "Ljava/lang/String;");
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetFieldID: name %d", __LINE__);

    jstring jname = (jstring)env->GetObjectField(jappInfo, f);
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetIntField: jname %d", __LINE__);
    if (jname != NULL) {
        String name = Util::GetElString(env, jname);
        env->DeleteLocalRef(jname);
        pkgInfo->SetName(name);
    }

    f = env->GetFieldID(appInfoKlass, "packageName", "Ljava/lang/String;");
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetFieldID: packageName %d", __LINE__);

    jstring jpackageName = (jstring)env->GetObjectField(jappInfo, f);
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetIntField: jpackageName %d", __LINE__);
    if (jpackageName != NULL) {
        String packageName = Util::GetElString(env, jpackageName);
        env->DeleteLocalRef(jpackageName);
        pkgInfo->SetPackageName(packageName);
    }

    f = env->GetFieldID(appInfoKlass, "labelRes", "I");
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetFieldID: labelRes %d", __LINE__);

    jint jlabelRes = env->GetIntField(jappInfo, f);
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetIntField: labelRes %d", __LINE__);
    pkgInfo->SetLabelRes((Int32)jlabelRes);

    f = env->GetFieldID(appInfoKlass, "nonLocalizedLabel", "Ljava/lang/CharSequence;");
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetFieldID: CharSequence %d", __LINE__);

    jobject jcsnonLocalizedLabel = env->GetObjectField(jappInfo, f);
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetObjectField: jcsnonLocalizedLabel %d", __LINE__);
    if (jcsnonLocalizedLabel != NULL) {
        jclass csClass = env->FindClass("java/lang/CharSequence");
        Util::CheckErrorAndLog(env, "GetElApplicationInfo", "FindClass CharSequence %d", __LINE__);

        jmethodID m = env->GetMethodID(csClass, "toString", "()Ljava/lang/String;");
        Util::CheckErrorAndLog(env, "GetElApplicationInfo", "GetMethodID toString %d", __LINE__);

        jstring jnonLocalizedLabel = (jstring)env->CallObjectMethod(jcsnonLocalizedLabel, m);
        Util::CheckErrorAndLog(env, "GetElApplicationInfo", "CallObjectMethod toString %d", __LINE__);

        String snonLocalizedLabel = Util::GetElString(env, jnonLocalizedLabel);
        env->DeleteLocalRef(jnonLocalizedLabel);
        AutoPtr<ICharSequence> nonLocalizedLabel;
        CString::New(snonLocalizedLabel, (ICharSequence**)&nonLocalizedLabel);

        pkgInfo->SetNonLocalizedLabel(nonLocalizedLabel);

        env->DeleteLocalRef(csClass);
        env->DeleteLocalRef(jcsnonLocalizedLabel);
    }

    f = env->GetFieldID(appInfoKlass, "icon", "I");
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetFieldID: icon %d", __LINE__);

    jint jicon = env->GetIntField(jappInfo, f);
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetIntField: icon %d", __LINE__);
    pkgInfo->SetIcon((Int32)jicon);

    f = env->GetFieldID(appInfoKlass, "logo", "I");
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetFieldID: logo %d", __LINE__);

    jint jlogo = env->GetIntField(jappInfo, f);
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetIntField: logo %d", __LINE__);
    pkgInfo->SetLogo((Int32)jlogo);

    f = env->GetFieldID(appInfoKlass, "metaData", "Landroid/os/Bundle;");
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetFieldID: metaData %d", __LINE__);

    jobject jmetaData = env->GetObjectField(jappInfo, f);
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetObjectField: jmetaData %d", __LINE__);
    if (jmetaData != NULL) {
        AutoPtr<IBundle> metaData;
        if (Util::GetElBundle(env, jmetaData, (IBundle**)&metaData)) {
            pkgInfo->SetMetaData(metaData);
        }
        else {
            LOGGERE("GetElApplicationInfo", "GetElProviderInfo() GetElBundle fail!");
        }

        env->DeleteLocalRef(jmetaData);
    }

    f = env->GetFieldID(appInfoKlass, "banner", "I");
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetFieldID: banner %d", __LINE__);

    jint jbanner = env->GetIntField(jappInfo, f);
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetIntField: banner %d", __LINE__);
    pkgInfo->SetBanner((Int32)jbanner);

    f = env->GetFieldID(appInfoKlass, "showUserIcon", "I");
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetFieldID: showUserIcon %d", __LINE__);

    jint jshowUserIcon = env->GetIntField(jappInfo, f);
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetIntField: showUserIcon %d", __LINE__);
    pkgInfo->SetShowUserIcon((Int32)jshowUserIcon);

    f = env->GetFieldID(appInfoKlass, "themedIcon", "I");
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetFieldID: themedIcon %d", __LINE__);

    jint jthemedIcon = env->GetIntField(jappInfo, f);
    Util::CheckErrorAndLog(env, "GetElApplicationInfo", "Fail GetIntField: themedIcon %d", __LINE__);
    pkgInfo->SetThemedIcon((Int32)jthemedIcon);

    env->DeleteLocalRef(appInfoKlass);
    return TRUE;
}

Boolean Util::GetElUri(
    /* [in] */ JNIEnv* env,
    /* [in] */ jobject jUri,
    /* [out] */ IUri** elUri)
{
    if (env == NULL || jUri == NULL || elUri == NULL) {
        LOGGERE("GetElUri", "Invalid arguments!");
        *elUri = NULL;
        return false;
    }

    jclass suriKlass = env->FindClass("android/net/Uri$StringUri");
    CheckErrorAndLog(env, "GetElUri", "Fail FindClass: Uri : %d!\n", __LINE__);
    jclass ouriKlass = env->FindClass("android/net/Uri$OpaqueUri");
    CheckErrorAndLog(env, "GetElUri", "Fail FindClass: Uri : %d!\n", __LINE__);
    jclass huriKlass = env->FindClass("android/net/Uri$HierarchicalUri");
    CheckErrorAndLog(env, "GetElUri", "Fail FindClass: Uri : %d!\n", __LINE__);

    *elUri = NULL;

    if (env->IsInstanceOf(jUri, suriKlass)) {
        jclass uriKlass = env->FindClass("android/net/Uri");
        CheckErrorAndLog(env, "GetElUri", "Fail FindClass: Uri : %d!\n", __LINE__);

        jmethodID m = env->GetMethodID(uriKlass, "toString", "()Ljava/lang/String;");
        CheckErrorAndLog(env, "GetElUri", "Fail GetMethodID: toString : %d!\n", __LINE__);

        jstring jStrUri = (jstring)env->CallObjectMethod(jUri, m);
        CheckErrorAndLog(env, "GetElUri", "Fail CallObjectMethod: toString : %d!\n", __LINE__);

        String strUri = GetElString(env, jStrUri);
        CStringUri::New(strUri, elUri);

        env->DeleteLocalRef(uriKlass);
        env->DeleteLocalRef(jStrUri);
    }
    else if (env->IsInstanceOf(jUri, ouriKlass)) {
        const char* tag = "UriToElUri() OpaqueUri";
        String NOT_CACHED("NOT CACHED"); // Uri::NOT_CACHED

        AutoPtr<IParcel> parcel;
        Elastos::Droid::Os::CParcel::New((IParcel**)&parcel);
        parcel->WriteInt32(2); //OpaqueUri::TYPE_ID

        jclass uriKlass = env->FindClass("android/net/Uri$OpaqueUri");
        CheckErrorAndLog(env, "GetElUri", "Fail FindClass: OpaqueUri : %d!\n", __LINE__);

        String scheme = GetJavaStringField(env, uriKlass, jUri, "scheme", tag);
        parcel->WriteString(scheme);

        jfieldID f = env->GetFieldID(uriKlass, "ssp", "Landroid/net/Uri$Part;");
        CheckErrorAndLog(env, "%s: Fail get ssp fieldid: %s : %d!\n", "UriToElUri", "ssp", __LINE__);
        jobject jssp = env->GetObjectField(jUri, f);
        CheckErrorAndLog(env, "%s: Fail get path field: %s : %d!\n", "UriToElUri", "ssp", __LINE__);

        jclass partKlass = env->FindClass("android/net/Uri$AbstractPart");
        String encoded = GetJavaStringField(env, partKlass, jssp, "encoded", tag);
        String decoded = GetJavaStringField(env, partKlass, jssp, "decoded", tag);
        Boolean hasEncoded = !encoded.Equals(NOT_CACHED);
        Boolean hasDecoded = !decoded.Equals(NOT_CACHED);
        if (hasEncoded && hasDecoded) {
            parcel->WriteInt32(0/*Representation.BOTH*/);
            parcel->WriteString(encoded);
            parcel->WriteString(decoded);
        }
        else if (hasEncoded) {
            parcel->WriteInt32(1/*Representation.ENCODED*/);
            parcel->WriteString(encoded);
        }
        else if (hasDecoded) {
            parcel->WriteInt32(2/*Representation.DECODED*/);
            parcel->WriteString(decoded);
        }
        else {
            assert(0);
        }

        f = env->GetFieldID(uriKlass, "fragment", "Landroid/net/Uri$Part;");
        CheckErrorAndLog(env, "%s: Fail get fragment fieldid: %s : %d!\n", "UriToElUri", "fragment", __LINE__);
        jobject jfragment = env->GetObjectField(jUri, f);
        CheckErrorAndLog(env, "%s: Fail get path field: %s : %d!\n", "UriToElUri", "fragment", __LINE__);

        encoded = GetJavaStringField(env, partKlass, jfragment, "encoded", tag);
        decoded = GetJavaStringField(env, partKlass, jfragment, "decoded", tag);
        hasEncoded = !encoded.Equals(NOT_CACHED);
        hasDecoded = !decoded.Equals(NOT_CACHED);
        if (hasEncoded && hasDecoded) {
            parcel->WriteInt32(0/*Representation.BOTH*/);
            parcel->WriteString(encoded);
            parcel->WriteString(decoded);
        }
        else if (hasEncoded) {
            parcel->WriteInt32(1/*Representation.ENCODED*/);
            parcel->WriteString(encoded);
        }
        else if (hasDecoded) {
            parcel->WriteInt32(2/*Representation.DECODED*/);
            parcel->WriteString(decoded);
        }

        COpaqueUri::New(elUri);
        parcel->SetDataPosition(0);
        IParcelable::Probe(*elUri)->ReadFromParcel(parcel);

        env->DeleteLocalRef(uriKlass);
        env->DeleteLocalRef(partKlass);
        env->DeleteLocalRef(jssp);
        env->DeleteLocalRef(jfragment);
    }
    else if (env->IsInstanceOf(jUri, huriKlass)) {
        const char* tag = "UriToElUri() HierarchicalUri";
        String NOT_CACHED("NOT CACHED"); // Uri::NOT_CACHED

        AutoPtr<IParcel> parcel;
        Elastos::Droid::Os::CParcel::New((IParcel**)&parcel);
        parcel->WriteInt32(3); //HierarchicalUri::TYPE_ID

        jclass uriKlass = env->FindClass("android/net/Uri$HierarchicalUri");
        CheckErrorAndLog(env, "GetElUri", "Fail FindClass: HierarchicalUri : %d!\n", __LINE__);

        String scheme = GetJavaStringField(env, uriKlass, jUri, "scheme", tag);
        parcel->WriteString(scheme);

        jfieldID f = env->GetFieldID(uriKlass, "authority", "Landroid/net/Uri$Part;");
        CheckErrorAndLog(env, "%s: Fail get authority fieldid: %s : %d!\n", "UriToElUri", "authority", __LINE__);
        jobject jauthority = env->GetObjectField(jUri, f);
        CheckErrorAndLog(env, "%s: Fail get path field: %s : %d!\n", "UriToElUri", "authority", __LINE__);

        jclass partKlass = env->FindClass("android/net/Uri$AbstractPart");
        String encoded = GetJavaStringField(env, partKlass, jauthority, "encoded", tag);
        String decoded = GetJavaStringField(env, partKlass, jauthority, "decoded", tag);
        Boolean hasEncoded = !encoded.Equals(NOT_CACHED);
        Boolean hasDecoded = !decoded.Equals(NOT_CACHED);
        if (hasEncoded && hasDecoded) {
            parcel->WriteInt32(0/*Representation.BOTH*/);
            parcel->WriteString(encoded);
            parcel->WriteString(decoded);
        }
        else if (hasEncoded) {
            parcel->WriteInt32(1/*Representation.ENCODED*/);
            parcel->WriteString(encoded);
        }
        else if (hasDecoded) {
            parcel->WriteInt32(2/*Representation.DECODED*/);
            parcel->WriteString(decoded);
        }
        else {
            assert(0);
        }

        f = env->GetFieldID(uriKlass, "path", "Landroid/net/Uri$PathPart;");
        CheckErrorAndLog(env, "%s: Fail get path fieldid: %s : %d!\n", "UriToElUri", "path", __LINE__);
        jobject jpath = env->GetObjectField(jUri, f);
        CheckErrorAndLog(env, "%s: Fail get path field: %s : %d!\n", "UriToElUri", "path", __LINE__);
        encoded = GetJavaStringField(env, partKlass, jpath, "encoded", tag);
        decoded = GetJavaStringField(env, partKlass, jpath, "decoded", tag);
        hasEncoded = !encoded.Equals(NOT_CACHED);
        hasDecoded = !decoded.Equals(NOT_CACHED);
        if (hasEncoded && hasDecoded) {
            parcel->WriteInt32(0/*Representation.BOTH*/);
            parcel->WriteString(encoded);
            parcel->WriteString(decoded);
        }
        else if (hasEncoded) {
            parcel->WriteInt32(1/*Representation.ENCODED*/);
            parcel->WriteString(encoded);
        }
        else if (hasDecoded) {
            parcel->WriteInt32(2/*Representation.DECODED*/);
            parcel->WriteString(decoded);
        }
        else {
            assert(0);
        }

        f = env->GetFieldID(uriKlass, "query", "Landroid/net/Uri$Part;");
        CheckErrorAndLog(env, "%s: Fail get query fieldid: %s : %d!\n", "UriToElUri", "query", __LINE__);
        jobject jquery = env->GetObjectField(jUri, f);
        CheckErrorAndLog(env, "%s: Fail get path field: %s : %d!\n", "UriToElUri", "query", __LINE__);

        encoded = GetJavaStringField(env, partKlass, jquery, "encoded", tag);
        decoded = GetJavaStringField(env, partKlass, jquery, "decoded", tag);
        hasEncoded = !encoded.Equals(NOT_CACHED);
        hasDecoded = !decoded.Equals(NOT_CACHED);
        if (hasEncoded && hasDecoded) {
            parcel->WriteInt32(0/*Representation.BOTH*/);
            parcel->WriteString(encoded);
            parcel->WriteString(decoded);
        }
        else if (hasEncoded) {
            parcel->WriteInt32(1/*Representation.ENCODED*/);
            parcel->WriteString(encoded);
        }
        else if (hasDecoded) {
            parcel->WriteInt32(2/*Representation.DECODED*/);
            parcel->WriteString(decoded);
        }
        else {
            assert(0);
        }

        f = env->GetFieldID(uriKlass, "fragment", "Landroid/net/Uri$Part;");
        CheckErrorAndLog(env, "%s: Fail get fragment fieldid: %s : %d!\n", "UriToElUri", "fragment", __LINE__);
        jobject jfragment = env->GetObjectField(jUri, f);
        CheckErrorAndLog(env, "%s: Fail get path field: %s : %d!\n", "UriToElUri", "fragment", __LINE__);

        encoded = GetJavaStringField(env, partKlass, jfragment, "encoded", tag);
        decoded = GetJavaStringField(env, partKlass, jfragment, "decoded", tag);
        hasEncoded = !encoded.Equals(NOT_CACHED);
        hasDecoded = !decoded.Equals(NOT_CACHED);
        if (hasEncoded && hasDecoded) {
            parcel->WriteInt32(0/*Representation.BOTH*/);
            parcel->WriteString(encoded);
            parcel->WriteString(decoded);
        }
        else if (hasEncoded) {
            parcel->WriteInt32(1/*Representation.ENCODED*/);
            parcel->WriteString(encoded);
        }
        else if (hasDecoded) {
            parcel->WriteInt32(2/*Representation.DECODED*/);
            parcel->WriteString(decoded);
        }

        CHierarchicalUri::New(elUri);
        parcel->SetDataPosition(0);
        IParcelable::Probe(*elUri)->ReadFromParcel(parcel);

        env->DeleteLocalRef(uriKlass);
        env->DeleteLocalRef(partKlass);
        env->DeleteLocalRef(jauthority);
        env->DeleteLocalRef(jpath);
        env->DeleteLocalRef(jquery);
        env->DeleteLocalRef(jfragment);
    }
    else {
        LOGGERE("GetElUri", "others type Uri!\n");
        assert(0);
    }

    env->DeleteLocalRef(suriKlass);
    env->DeleteLocalRef(ouriKlass);
    env->DeleteLocalRef(huriKlass);
    return true;
}

Boolean Util::GetElComponentName(
    /* [in] */ JNIEnv* env,
    /* [in] */ jobject jcomponentName,
    /* [out] */ IComponentName** elcomponentName)
{
    if (elcomponentName == NULL) {
        LOGGERE("GetElComponentName", "Invalid argumenet!");
        return FALSE;
    }

    jclass componentNameKlass = env->FindClass("android/content/ComponentName");
    Util::CheckErrorAndLog(env, "GetElComponentName", "FindClass: android/content/ComponentName  %d", __LINE__);

    jfieldID f = env->GetFieldID(componentNameKlass, "mPackage", "Ljava/lang/String;");
    Util::CheckErrorAndLog(env, "GetElComponentName", "GetFieldID: mPackage  %d", __LINE__);

    jstring jpackage = (jstring)env->GetObjectField(jcomponentName, f);
    Util::CheckErrorAndLog(env, "GetElComponentName", "GetObjectField: jpackage  %d", __LINE__);

    f = env->GetFieldID(componentNameKlass, "mClass", "Ljava/lang/String;");
    Util::CheckErrorAndLog(env, "GetElComponentName", "GetFieldID: componentNameKlass  %d", __LINE__);

    jstring jclassname = (jstring)env->GetObjectField(jcomponentName, f);
    Util::CheckErrorAndLog(env, "GetElComponentName", "GetObjectField: jclassname  %d", __LINE__);

    String pakcage;
    if (jpackage != NULL) {
        pakcage = GetElString(env, jpackage);
        env->DeleteLocalRef(jpackage);
    }

    String classname;
    if (jclassname != NULL) {
        classname = GetElString(env, jclassname);
        env->DeleteLocalRef(jclassname);
    }

    if (NOERROR != CComponentName::New(pakcage, classname, elcomponentName)) {
        LOGGERE("GetElComponentName", "create CComponentName fail!");
        return FALSE;
    }

    env->DeleteLocalRef(componentNameKlass);
    return FALSE;
}

Boolean Util::GetElIntent(
    /* [in] */ JNIEnv* env,
    /* [in] */ jobject jintent,
    /* [out] */ IIntent** elIntent)
{
    if (elIntent == NULL) {
        LOGGERE("GetElIntent", "Invalid argumenet!");
        return FALSE;
    }

    if (NOERROR != CIntent::New(elIntent)) {
        LOGGERE("GetElIntent", "create CIntent fail!");
        *elIntent = NULL;
        return FALSE;
    }

    jclass intentKlass = env->FindClass("android/content/Intent");
    Util::CheckErrorAndLog(env, "GetElIntent", "FindClass: android/content/Intent %d", __LINE__);

    jfieldID f = env->GetFieldID(intentKlass, "mAction", "Ljava/lang/String;");
    Util::CheckErrorAndLog(env, "GetElIntent", "GetFieldID: mAction %d", __LINE__);

    jstring jaction = (jstring)env->GetObjectField(jintent, f);
    Util::CheckErrorAndLog(env, "GetElIntent", "GetObjectField: jaction %d", __LINE__);

    if (jaction != NULL) {
        String action = GetElString(env, jaction);
        env->DeleteLocalRef(jaction);
        (*elIntent)->SetAction(action);
    }

    f = env->GetFieldID(intentKlass, "mData", "Landroid/net/Uri;");
    Util::CheckErrorAndLog(env, "GetElIntent", "GetFieldID: mData %d", __LINE__);

    jobject jdata = env->GetObjectField(jintent, f);
    Util::CheckErrorAndLog(env, "GetElIntent", "GetObjectField: jdata %d", __LINE__);

    AutoPtr<IUri> data;
    if (jdata != NULL)  {
        if (!GetElUri(env, jdata, (IUri**)&data)) {
            LOGGERE("GetElIntent", "GetElIntent fail %d", __LINE__);
        }

        env->DeleteLocalRef(jdata);
    }

    f = env->GetFieldID(intentKlass, "mType", "Ljava/lang/String;");
    Util::CheckErrorAndLog(env, "GetElIntent", "GetFieldID: mType %d", __LINE__);

    jstring jtype = (jstring)env->GetObjectField(jintent, f);
    Util::CheckErrorAndLog(env, "GetElIntent", "GetObjectField: jtype %d", __LINE__);

    if (jtype != NULL) {
        String type = GetElString(env, jtype);

        if (data != NULL){
            (*elIntent)->SetDataAndType(data, String(type));
        }
        else {
            (*elIntent)->SetType(String(type));
        }

        env->DeleteLocalRef(jtype);
    }
    else if (data != NULL) {
        (*elIntent)->SetData(data);
    }

    f = env->GetFieldID(intentKlass, "mPackage", "Ljava/lang/String;");
    Util::CheckErrorAndLog(env, "GetElIntent", "GetFieldID: mPackage %d", __LINE__);

    jstring jpackage = (jstring)env->GetObjectField(jintent, f);
    Util::CheckErrorAndLog(env, "GetElIntent", "GetObjectField: jpackage %d", __LINE__);

    if (jpackage != NULL)  {
        String packageArray = GetElString(env, jpackage);
        env->DeleteLocalRef(jpackage);
        (*elIntent)->SetPackage(packageArray);
    }

    f = env->GetFieldID(intentKlass, "mComponent", "Landroid/content/ComponentName;");
    Util::CheckErrorAndLog(env, "GetElIntent", "GetFieldID: mComponent %d", __LINE__);

    jobject jComponent = env->GetObjectField(jintent, f);
    Util::CheckErrorAndLog(env, "GetElIntent", "GetObjectField jintent %d", __LINE__);

    if (jComponent != NULL) {
        AutoPtr<IComponentName> elcomponentName;
        if (GetElComponentName(env, jComponent, (IComponentName**)&elcomponentName)) {
            (*elIntent)->SetComponent(elcomponentName);
        }
        else {
            LOGGERE("GetElIntent", "ComponentNameToElComponentName fail %d", __LINE__);
        }
        env->DeleteLocalRef(jComponent);
    }

    f = env->GetFieldID(intentKlass, "mFlags", "I");
    Util::CheckErrorAndLog(env, "GetElIntent", "GetFieldID: mFlags %d", __LINE__);

    jint jflags = env->GetIntField(jintent, f);
    Util::CheckErrorAndLog(env, "GetElIntent", "GetObjectField jflags %d", __LINE__);
    (*elIntent)->SetFlags((Int32)jflags);

    f = env->GetFieldID(intentKlass, "mCategories", "Landroid/util/ArraySet;");
    Util::CheckErrorAndLog(env, "GetElIntent", "GetFieldID: mCategories %d", __LINE__);

    jobject jcategories = env->GetObjectField(jintent, f);
    Util::CheckErrorAndLog(env, "GetElIntent", "GetObjectField jcategories %d", __LINE__);

    if (jcategories != NULL) {
        jclass setKlass = env->FindClass("android/util/ArraySet");
        Util::CheckErrorAndLog(env, "GetElIntent", "GetElIntent FindClass: ArraySet fail %d", __LINE__);

        jmethodID m = env->GetMethodID(setKlass, "size", "()I");
        Util::CheckErrorAndLog(env, "GetElIntent", "GetJavaBundle Fail GetMethodID: size %d", __LINE__);

        jint size = env->CallIntMethod(jcategories, m);
        Util::CheckErrorAndLog(env, "GetElIntent", "GetJavaBundle Fail CallObjectMethod: size %d", __LINE__);

        if (size > 0) {
            m = env->GetMethodID(setKlass, "iterator", "()Ljava/util/Iterator;");
            Util::CheckErrorAndLog(env, "GetElIntent", "GetJavaBundle Fail GetMethodID: iterator %d", __LINE__);

            jobject jiterator = env->CallObjectMethod(jcategories, m);
            Util::CheckErrorAndLog(env, "GetElIntent", "GetJavaBundle Fail CallObjectMethod: size %d", __LINE__);

            jclass iterKlass = env->FindClass("java/util/Iterator");
            Util::CheckErrorAndLog(env, "GetElIntent", "GetElIntent FindClass: Iterator fail %d", __LINE__);

            m = env->GetMethodID(iterKlass, "hasNext", "()Z");
            Util::CheckErrorAndLog(env, "GetElIntent", "GetJavaBundle Fail GetMethodID: iterator %d", __LINE__);

            jboolean jhasNext = env->CallBooleanMethod(jiterator, m);
            Util::CheckErrorAndLog(env, "GetElIntent", "GetJavaBundle Fail CallBooleanMethod: jhasNext %d", __LINE__);

            jmethodID mnext = env->GetMethodID(iterKlass, "next", "()Ljava/lang/Object;");
            Util::CheckErrorAndLog(env, "GetElIntent", "GetJavaBundle Fail GetMethodID: iterator %d", __LINE__);

            while (jhasNext) {
                jstring jCategorie = (jstring)env->CallObjectMethod(jiterator, mnext);
                Util::CheckErrorAndLog(env, "GetElIntent", "GetJavaBundle Fail CallObjectMethod: size %d", __LINE__);

                String categorie = Util::GetElString(env, jCategorie);
                env->DeleteLocalRef(jCategorie);
                (*elIntent)->AddCategory(categorie);

                jhasNext = env->CallBooleanMethod(jiterator, m);
                Util::CheckErrorAndLog(env, "GetElIntent", "GetJavaBundle Fail CallBooleanMethod: jhasNext %d", __LINE__);
            }

            env->DeleteLocalRef(iterKlass);
            env->DeleteLocalRef(jiterator);
        }

        env->DeleteLocalRef(setKlass);
        env->DeleteLocalRef(jcategories);
    }

    f = env->GetFieldID(intentKlass, "mExtras", "Landroid/os/Bundle;");
    Util::CheckErrorAndLog(env, "GetElIntent", "GetFieldID: mExtras Landroid/os/Bundle; %d", __LINE__);

    jobject jBundle = env->GetObjectField(jintent, f);
    Util::CheckErrorAndLog(env, "GetElIntent", "GetObjectField: jintent %d", __LINE__);

    if (jBundle != NULL) {
        AutoPtr<IBundle> elBundle;
        if (GetElBundle(env, jBundle, (IBundle**)&elBundle)) {
            (*elIntent)->PutExtras(elBundle);
        }
        else {
            LOGGERE("GetElIntent", "BundleToElBundle(jBundle) fail %d", __LINE__);
        }
        env->DeleteLocalRef(jBundle);
    }

    f = env->GetFieldID(intentKlass, "mSourceBounds", "Landroid/graphics/Rect;");
    Util::CheckErrorAndLog(env, "GetElIntent", "GetFieldID: mSourceBounds %d", __LINE__);

    jobject jsourceBounds = env->GetObjectField(jintent, f);
    Util::CheckErrorAndLog(env, "GetElIntent", "GetObjectField jsourceBounds %d", __LINE__);

    if (jsourceBounds != NULL) {
        AutoPtr<IRect> sourceBounds;
        if (GetElRect(env, jsourceBounds, (IRect**)&sourceBounds)) {
            (*elIntent)->SetSourceBounds(sourceBounds);
        }
        else {
            LOGGERE("GetElIntent", "RectToElRect() fail %d", __LINE__);
        }

        env->DeleteLocalRef(jsourceBounds);
    }

    f = env->GetFieldID(intentKlass, "mSelector", "Landroid/content/Intent;");
    Util::CheckErrorAndLog(env, "GetElIntent", "GetFieldID: mSelector %d", __LINE__);

    jobject jselector = env->GetObjectField(jintent, f);
    Util::CheckErrorAndLog(env, "GetElIntent", "GetObjectField jselector %d", __LINE__);

    if (jselector != NULL) {
        AutoPtr<IIntent> intent;
        if (GetElIntent(env, jselector, (IIntent**)&intent)) {
            (*elIntent)->SetSelector(intent);
        }
        else {
            LOGGERE("GetElIntent", "GetElIntent(jselector) fail %d", __LINE__);
        }

        env->DeleteLocalRef(jselector);
    }

    f = env->GetFieldID(intentKlass, "mClipData", "Landroid/content/ClipData;");
    Util::CheckErrorAndLog(env, "GetElIntent", "GetFieldID: mClipData %d", __LINE__);

    jobject jclipData = env->GetObjectField(jintent, f);
    Util::CheckErrorAndLog(env, "GetElIntent", "GetObjectField jclipData %d", __LINE__);

    if (jclipData != NULL) {
        AutoPtr<IClipData> clipData;
        if (Util::GetElClipData(env, jclipData, (IClipData**)&clipData)) {
            (*elIntent)->SetClipData(clipData);
        }
        else {
            LOGGERE("GetElIntent", "GetElClipData() fail %d", __LINE__);
        }

        env->DeleteLocalRef(jclipData);
    }

    f = env->GetFieldID(intentKlass, "mContentUserHint", "I");
    Util::CheckErrorAndLog(env, "GetElIntent", "GetFieldID: mContentUserHint %d", __LINE__);

    jint jcontentUserHint = env->GetIntField(jintent, f);
    Util::CheckErrorAndLog(env, "GetElIntent", "GetObjectField mContentUserHint %d", __LINE__);
    (*elIntent)->SetContentUserHint((Int32)jcontentUserHint);

    env->DeleteLocalRef(intentKlass);
    return TRUE;
}

Boolean Util::GetElBundle(
    /* [in] */ JNIEnv* env,
    /* [in] */ jobject jbundle,
    /* [out] */ IBundle** bundle)
{
    if (env == NULL || jbundle == NULL || bundle == NULL) {
        LOGGERE("GetElBundle", "Invalid arguments!");
        return FALSE;
    }

    if (NOERROR != CBundle::New(bundle)) {
        LOGGERE("GetElBundle", "Create CBundle fail!");
        return FALSE;
    }

    return GetElBundle(env, jbundle, *bundle);
}

Boolean Util::GetElBundle(
    /* [in] */ JNIEnv* env,
    /* [in] */ jobject jbundle,
    /* [in] */ IBundle* bundle)
{
    if (env == NULL || jbundle == NULL || bundle == NULL) {
        LOGGERE("GetElBundle", "Invalid arguments!");
        return FALSE;
    }

    jclass bundleKlass = env->FindClass("android/os/BaseBundle");
    Util::CheckErrorAndLog(env, "GetElBundle", "FindClass: android/os/Bundle %d", __LINE__);

    jmethodID m = env->GetMethodID(bundleKlass, "keySet", "()Ljava/util/Set;");
    Util::CheckErrorAndLog(env, "GetElBundle", "GetMethodID: keySet %d", __LINE__);

    jobject jKeySet = env->CallObjectMethod(jbundle, m);
    Util::CheckErrorAndLog(env, "GetElBundle", "CallObjectMethod: jKeySet %d", __LINE__);

    if (jKeySet != NULL) {
        jclass setKlass = env->FindClass("java/util/Set");
        Util::CheckErrorAndLog(env, "GetElBundle", "FindClass: java/util/Set %d", __LINE__);

        m = env->GetMethodID(setKlass, "iterator", "()Ljava/util/Iterator;");
        Util::CheckErrorAndLog(env, "GetElBundle", "GetMethodID: iterator ()Ljava/util/Iterator; %d", __LINE__);

        jobject jIterator = env->CallObjectMethod(jKeySet, m);
        Util::CheckErrorAndLog(env, "GetElBundle", "CallObjectMethod: jIterator %d", __LINE__);

        if (jIterator != NULL) {
            jclass iteratorKlass = env->FindClass("java/util/Iterator");
            Util::CheckErrorAndLog(env, "GetElBundle", "FindClass: java/util/Iterator %d", __LINE__);

            jmethodID jHasNext = env->GetMethodID(iteratorKlass, "hasNext", "()Z");
            Util::CheckErrorAndLog(env, "GetElBundle", "GetMethodID: hasNext %d", __LINE__);

            jmethodID jNext = env->GetMethodID(iteratorKlass, "next", "()Ljava/lang/Object;");
            Util::CheckErrorAndLog(env, "GetElBundle", "GetMethodID: next %d", __LINE__);

            jboolean hasNext = env->CallBooleanMethod(jIterator, jHasNext);
            while (hasNext) {
                jstring jKey = (jstring)env->CallObjectMethod(jIterator, jNext);
                String key = Util::GetElString(env, jKey);

                //call Bundle.get(String)
                m = env->GetMethodID(bundleKlass, "get", "(Ljava/lang/String;)Ljava/lang/Object;");
                Util::CheckErrorAndLog(env, "GetElBundle", "GetMethodID: get %d", __LINE__);

                jobject jValue = env->CallObjectMethod(jbundle, m, jKey);
                Util::CheckErrorAndLog(env, "GetElBundle", "CallObjectMethod: jValue %d", __LINE__);

                env->DeleteLocalRef(jKey);

                if (jValue != NULL) {
                    String objClassName = GetClassName(env, jValue);

                    if (objClassName.Equals("java.lang.String")) {
                        String value = Util::GetElString(env, (jstring)jValue);
                        bundle->PutString(key, value);
                    }
                    else if (objClassName.Equals("java.lang.Boolean")) {
                        jboolean value = GetJavaBoolField(env, jValue, "value", "GetElBundle()");
                        bundle->PutBoolean(key, value);
                    }
                    else if (objClassName.Equals("java.lang.Integer")) {
                        jclass c = env->GetObjectClass(jValue);
                        jint value = GetJavaIntField(env, c, jValue, "value", "GetElBundle");
                        env->DeleteLocalRef(c);
                        bundle->PutInt32(key, value);
                    }
                    else if (objClassName.Equals("android.content.Intent")) {
                        AutoPtr<IIntent> intent;
                        Util::GetElIntent(env, jValue, (IIntent**)&intent);
                        AutoPtr<IParcelable> pracelable = IParcelable::Probe(intent);
                        bundle->PutParcelable(key, pracelable);
                    }
                    else if (objClassName.Equals("java.lang.Long")){
                        jlong jlongValue = GetJavalongField(env, jValue, "value", "GetElBundle()");
                        bundle->PutInt64(key, (Int64)jlongValue);
                    }
                    else if (objClassName.Equals("android.os.Bundle")){
                        AutoPtr<IBundle> bundleItem;
                        GetElBundle(env, jValue, (IBundle**)&bundleItem);
                        bundle->PutBundle(key, bundleItem);
                    }
                    else if (objClassName.Equals("[Landroid.os.Parcelable;")){
                        jobjectArray jArrayValue = (jobjectArray)jValue;
                        jsize arraySize = env->GetArrayLength(jArrayValue);
                        AutoPtr<ArrayOf<IParcelable*> > parcelableArray = ArrayOf<IParcelable*>::Alloc(arraySize);

                        for (int i = 0; i < arraySize; ++i) {
                            jobject jParcelItem = env->GetObjectArrayElement(jArrayValue, i);
                            String itemClassName = GetClassName(env, jParcelItem);
                            // Logger::E("GetElBundle", "GetElBundle(); item in arrayOf type:%s", itemClassName.string());

                            if (itemClassName.Equals("android.content.Intent")){
                                AutoPtr<IIntent> intent;
                                Util::GetElIntent(env, jParcelItem, (IIntent**)&intent);
                                AutoPtr<IParcelable> pracelable = IParcelable::Probe(intent);
                                parcelableArray->Set(i, pracelable);
                            }
                            else{
                                Logger::E("GetElBundle", "GetElBundle(); Unsupported arrayOf type%s", itemClassName.string());
                            }

                            env->DeleteLocalRef(jParcelItem);
                        }

                        env->DeleteLocalRef(jArrayValue);

                        bundle->PutParcelableArray(key, parcelableArray);
                    }
                    else {
                        Logger::E("GetElBundle", "GetElBundle, Unsupported type:%s; Key:%s", objClassName.string(), key.string());
                    }

                    env->DeleteLocalRef(jValue);
                }
                else {
                    bundle->PutParcelable(key, NULL);
                }

                hasNext = env->CallBooleanMethod(jIterator, jHasNext);
            }
            env->DeleteLocalRef(iteratorKlass);
            env->DeleteLocalRef(jIterator);
        }
        env->DeleteLocalRef(setKlass);
        env->DeleteLocalRef(jKeySet);
    }

    env->DeleteLocalRef(bundleKlass);

    return TRUE;
}

Boolean Util::GetElProviderInfo(
    /* [in] */ JNIEnv* env,
    /* [in] */ jobject jproviderInfo,
    /* [out] */ IProviderInfo** providerInfo)
{
    if (env == NULL || jproviderInfo == NULL || providerInfo == NULL) {
        LOGGERE("GetElProviderInfo", "Invalid arguments!");
        return FALSE;
    }

    if (NOERROR != CProviderInfo::New(providerInfo)) {
        LOGGERE("GetElProviderInfo", "Create CProviderInfo fail!");
        return FALSE;
    }

    jclass piKlass = env->FindClass("android/content/pm/ProviderInfo");
    CheckErrorAndLog(env, "GetElProviderInfo", "Fail FindClass: ProviderInfo%d", __LINE__);

    jfieldID f = env->GetFieldID(piKlass, "authority", "Ljava/lang/String;");
    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetFieldID: authority %d", __LINE__);

    jstring jauthority = (jstring)env->GetObjectField(jproviderInfo, f);
    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetObjectField: jauthority %d", __LINE__);
    if (jauthority != NULL) {
        String authority = Util::GetElString(env, jauthority);
        env->DeleteLocalRef(jauthority);
        (*providerInfo)->SetAuthority(authority);
    }

    f = env->GetFieldID(piKlass, "readPermission", "Ljava/lang/String;");
    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetFieldID: readPermission %d", __LINE__);

    jstring jreadPermission = (jstring)env->GetObjectField(jproviderInfo, f);
    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetObjectField: jreadPermission %d", __LINE__);
    if (jreadPermission != NULL) {
        String readPermission = Util::GetElString(env, jreadPermission);
        env->DeleteLocalRef(jreadPermission);
        (*providerInfo)->SetReadPermission(readPermission);
    }

    f = env->GetFieldID(piKlass, "writePermission", "Ljava/lang/String;");
    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetFieldID: writePermission %d", __LINE__);

    jstring jwritePermission = (jstring)env->GetObjectField(jproviderInfo, f);
    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetObjectField: jwritePermission %d", __LINE__);
    if (jwritePermission != NULL) {
        String writePermission = Util::GetElString(env, jwritePermission);
        env->DeleteLocalRef(jwritePermission);
        (*providerInfo)->SetWritePermission(writePermission);
    }

    f = env->GetFieldID(piKlass, "grantUriPermissions", "Z");
    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetFieldID: grantUriPermissions %d", __LINE__);

    jboolean jgrantUriPermissions = env->GetBooleanField(jproviderInfo, f);
    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetBooleanField: grantUriPermissions %d", __LINE__);
    (*providerInfo)->SetGrantUriPermissions((Boolean)jgrantUriPermissions);

    f = env->GetFieldID(piKlass, "uriPermissionPatterns", "[Landroid/os/PatternMatcher;");
    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetFieldID: uriPermissionPatterns[] %d", __LINE__);

    jobjectArray juriPermissionPatterns = (jobjectArray)env->GetObjectField(jproviderInfo, f);
    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetObjectField: juriPermissionPatterns %d", __LINE__);
    if (juriPermissionPatterns != NULL) {
        jint jcount = env->GetArrayLength(juriPermissionPatterns);
        if (jcount > 0) {
            jclass pmerKlass = env->FindClass("android/os/PatternMatcher");
            Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail FindClass: PatternMatcher %d", __LINE__);

            AutoPtr<ArrayOf<IPatternMatcher*> > uriPermissionPatterns = ArrayOf<IPatternMatcher*>::Alloc((Int32)jcount);
            if (uriPermissionPatterns != NULL) {
                for (Int32 i = 0; i < jcount; i++) {
                    jobject jmatcher = env->GetObjectArrayElement(juriPermissionPatterns, i);
                    CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetObjectArrayElement: PatternMatcher %d", __LINE__);

                    f = env->GetFieldID(pmerKlass, "mPattern", "Ljava/lang/String;");
                    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetFieldID: mPattern %d", __LINE__);

                    jstring jmPattern = (jstring)env->GetObjectField(jmatcher, f);
                    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetObjectField: mPattern %d", __LINE__);
                    String mPattern = Util::GetElString(env, jmPattern);

                    f = env->GetFieldID(pmerKlass, "mType", "I");
                    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetFieldID: mPattern %d", __LINE__);

                    jint jmType = env->GetIntField(jmatcher, f);
                    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetIntField: mType %d", __LINE__);

                    AutoPtr<IPatternMatcher> paternMatcher;
                    CPatternMatcher::New(mPattern, (Int32)jmType, (IPatternMatcher**)&paternMatcher);
                    uriPermissionPatterns->Set(i, paternMatcher);

                    env->DeleteLocalRef(jmPattern);
                    env->DeleteLocalRef(jmatcher);
                }
            }

            (*providerInfo)->SetUriPermissionPatterns(uriPermissionPatterns);
            env->DeleteLocalRef(pmerKlass);
        }

        env->DeleteLocalRef(juriPermissionPatterns);
    }

    f = env->GetFieldID(piKlass, "pathPermissions", "[Landroid/content/pm/PathPermission;");
    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetFieldID: pathPermissions[] %d", __LINE__);

    jobjectArray jpathPermissions = (jobjectArray)env->GetObjectField(jproviderInfo, f);
    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetObjectField: pathPermissions %d", __LINE__);
    if (jpathPermissions != NULL) {
        jint jcount = env->GetArrayLength(jpathPermissions);
        if (jcount > 0) {
            jclass ppmnKlass = env->FindClass("android/content/pm/PathPermission");
            Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail FindClass: PathPermission %d", __LINE__);

            AutoPtr<ArrayOf<IPathPermission*> > pathPermissions = ArrayOf<IPathPermission*>::Alloc((Int32)jcount);
            if (pathPermissions != NULL) {
                for (Int32 i = 0; i < jcount; i++) {
                    jobject jpathmatcher = env->GetObjectArrayElement(jpathPermissions, i);
                    CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetObjectArrayElement: PatternMatcher %d", __LINE__);

                    f = env->GetFieldID(ppmnKlass, "mPattern", "Ljava/lang/String;");
                    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetFieldID: mPattern %d", __LINE__);

                    jstring jmPattern = (jstring)env->GetObjectField(jpathmatcher, f);
                    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetObjectField: mPattern %d", __LINE__);
                    String mPattern = Util::GetElString(env, jmPattern);
                    env->DeleteLocalRef(jmPattern);

                    f = env->GetFieldID(ppmnKlass, "mType", "I");
                    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetFieldID: mPattern %d", __LINE__);

                    jint jmType = env->GetIntField(jpathmatcher, f);
                    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetIntField: mType %d", __LINE__);

                    f = env->GetFieldID(ppmnKlass, "mReadPermission", "Ljava/lang/String;");
                    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetFieldID: mReadPermission %d", __LINE__);

                    jstring jmReadPermission = (jstring)env->GetObjectField(jpathmatcher, f);
                    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetObjectField: mReadPermission %d", __LINE__);
                    String mReadPermission = Util::GetElString(env, jmReadPermission);
                    env->DeleteLocalRef(jmReadPermission);

                    f = env->GetFieldID(ppmnKlass, "mWritePermission", "Ljava/lang/String;");
                    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetFieldID: mWritePermission %d", __LINE__);

                    jstring jmWritePermission = (jstring)env->GetObjectField(jpathmatcher, f);
                    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetObjectField: mWritePermission %d", __LINE__);
                    String mWritePermission = Util::GetElString(env, jmWritePermission);
                    env->DeleteLocalRef(jmWritePermission);

                    AutoPtr<IPathPermission> pathPermission;
                    CPathPermission::New(mPattern, (Int32)jmType, mReadPermission, mWritePermission, (IPathPermission**)&pathPermission);
                    pathPermissions->Set(i, pathPermission);

                    env->DeleteLocalRef(jpathmatcher);
                }
            }

            (*providerInfo)->SetPathPermissions(pathPermissions);
            env->DeleteLocalRef(ppmnKlass);
        }

        env->DeleteLocalRef(jpathPermissions);
    }

    f = env->GetFieldID(piKlass, "multiprocess", "Z");
    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetFieldID: multiprocess %d", __LINE__);

    jboolean jmultiprocess = env->GetBooleanField(jproviderInfo, f);
    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetBooleanField: multiprocess %d", __LINE__);
    (*providerInfo)->SetMultiprocess((Boolean)jmultiprocess);

    f = env->GetFieldID(piKlass, "initOrder", "I");
    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetFieldID: initOrder %d", __LINE__);

    jint jinitOrder = env->GetIntField(jproviderInfo, f);
    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetIntField: initOrder %d", __LINE__);
    (*providerInfo)->SetInitOrder((Int32)jinitOrder);

    f = env->GetFieldID(piKlass, "flags", "I");
    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetFieldID: flags %d", __LINE__);

    jint jflags = env->GetIntField(jproviderInfo, f);
    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetIntField: flags %d", __LINE__);
    (*providerInfo)->SetFlags((Int32)jflags);

    f = env->GetFieldID(piKlass, "applicationInfo", "Landroid/content/pm/ApplicationInfo;");
    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetFieldID: ApplicationInfo %d", __LINE__);

    IComponentInfo* comInfo = IComponentInfo::Probe(*providerInfo);
    jobject japplicationInfo = env->GetObjectField(jproviderInfo, f);
    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetObjectField: japplicationInfo %d", __LINE__);
    if (japplicationInfo != NULL) {
        AutoPtr<IApplicationInfo> appInfo;
        if (Util::GetElApplicationInfo(env, japplicationInfo, (IApplicationInfo**)&appInfo)) {
            comInfo->SetApplicationInfo(appInfo);
        }
        else {
            LOGGERE("GetElProviderInfo", "GetElProviderInfo() GetElApplicationInfo fail!");
        }

        env->DeleteLocalRef(japplicationInfo);
    }

    f = env->GetFieldID(piKlass, "processName", "Ljava/lang/String;");
    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetFieldID: processName %d", __LINE__);

    jstring jprocessName = (jstring)env->GetObjectField(jproviderInfo, f);
    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetObjectField: processName %d", __LINE__);
    if (jprocessName != NULL) {
        String processName = Util::GetElString(env, jprocessName);
        env->DeleteLocalRef(jprocessName);
        comInfo->SetProcessName(processName);
    }

    f = env->GetFieldID(piKlass, "descriptionRes", "I");
    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetFieldID: descriptionRes %d", __LINE__);

    jint jdescriptionRes = env->GetIntField(jproviderInfo, f);
    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetIntField: descriptionRes %d", __LINE__);
    comInfo->SetDescriptionRes((Int32)jdescriptionRes);

    f = env->GetFieldID(piKlass, "enabled", "Z");
    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetFieldID: enabled %d", __LINE__);

    jboolean jenabled = env->GetBooleanField(jproviderInfo, f);
    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetBooleanField: enabled %d", __LINE__);
    comInfo->SetEnabled((Boolean)jenabled);

    f = env->GetFieldID(piKlass, "exported", "Z");
    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetFieldID: exported %d", __LINE__);

    jboolean jexported = env->GetBooleanField(jproviderInfo, f);
    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetBooleanField: exported %d", __LINE__);
    comInfo->SetExported((Boolean)jexported);

    f = env->GetFieldID(piKlass, "name", "Ljava/lang/String;");
    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetFieldID: name %d", __LINE__);

    IPackageItemInfo* pkgInfo = IPackageItemInfo::Probe(comInfo);
    jstring jname = (jstring)env->GetObjectField(jproviderInfo, f);
    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetObjectField: jname %d", __LINE__);
    if (jname != NULL) {
        String name = Util::GetElString(env, jname);
        env->DeleteLocalRef(jname);
        pkgInfo->SetName(name);
    }

    f = env->GetFieldID(piKlass, "packageName", "Ljava/lang/String;");
    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetFieldID: packageName %d", __LINE__);

    jstring jpackageName = (jstring)env->GetObjectField(jproviderInfo, f);
    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetObjectField: jpackageName %d", __LINE__);
    if (jpackageName != NULL) {
        String packageName = Util::GetElString(env, jpackageName);
        env->DeleteLocalRef(jpackageName);
        pkgInfo->SetPackageName(packageName);
    }

    f = env->GetFieldID(piKlass, "labelRes", "I");
    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetFieldID: labelRes %d", __LINE__);

    jint jlabelRes = env->GetIntField(jproviderInfo, f);
    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetIntField: labelRes %d", __LINE__);
    pkgInfo->SetLabelRes((Int32)jlabelRes);

    f = env->GetFieldID(piKlass, "nonLocalizedLabel", "Ljava/lang/CharSequence;");
    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetFieldID: CharSequence %d", __LINE__);

    jobject jcsnonLocalizedLabel = env->GetObjectField(jproviderInfo, f);
    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetObjectField: jcsnonLocalizedLabel %d", __LINE__);
    if (jcsnonLocalizedLabel != NULL) {
        jclass csClass = env->FindClass("java/lang/CharSequence");
        Util::CheckErrorAndLog(env, "GetElProviderInfo", "FindClass CharSequence  %d", __LINE__);

        jmethodID m = env->GetMethodID(csClass, "toString", "()Ljava/lang/String;");
        Util::CheckErrorAndLog(env, "GetElProviderInfo", "GetMethodID toString  %d", __LINE__);

        jstring jnonLocalizedLabel = (jstring)env->CallObjectMethod(jcsnonLocalizedLabel, m);
        Util::CheckErrorAndLog(env, "GetElProviderInfo", "CallObjectMethod toString  %d", __LINE__);

        String snonLocalizedLabel = Util::GetElString(env, jnonLocalizedLabel);
        env->DeleteLocalRef(jnonLocalizedLabel);

        AutoPtr<ICharSequence> nonLocalizedLabel;
        CString::New(snonLocalizedLabel, (ICharSequence**)&nonLocalizedLabel);

        pkgInfo->SetNonLocalizedLabel(nonLocalizedLabel);

        env->DeleteLocalRef(csClass);
        env->DeleteLocalRef(jcsnonLocalizedLabel);
    }

    f = env->GetFieldID(piKlass, "icon", "I");
    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetFieldID: icon %d", __LINE__);

    jint jicon = env->GetIntField(jproviderInfo, f);
    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetIntField: icon %d", __LINE__);
    pkgInfo->SetIcon((Int32)jicon);

    f = env->GetFieldID(piKlass, "logo", "I");
    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetFieldID: logo %d", __LINE__);

    jint jlogo = env->GetIntField(jproviderInfo, f);
    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetIntField: logo %d", __LINE__);
    pkgInfo->SetLogo((Int32)jlogo);

    f = env->GetFieldID(piKlass, "metaData", "Landroid/os/Bundle;");
    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetFieldID: metaData %d", __LINE__);

    jobject jmetaData = env->GetObjectField(jproviderInfo, f);
    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetObjectField: jmetaData %d", __LINE__);
    if (jmetaData != NULL) {
        AutoPtr<IBundle> metaData;
        if (Util::GetElBundle(env, jmetaData, (IBundle**)&metaData)) {
            pkgInfo->SetMetaData(metaData);
        }
        else {
            LOGGERE("GetElProviderInfo", "GetElProviderInfo() GetElBundle fail!");
        }

        env->DeleteLocalRef(jmetaData);
    }

    f = env->GetFieldID(piKlass, "banner", "I");
    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetFieldID: banner %d", __LINE__);

    jint jbanner = env->GetIntField(jproviderInfo, f);
    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetIntField: banner %d", __LINE__);
    pkgInfo->SetBanner((Int32)jbanner);

    f = env->GetFieldID(piKlass, "showUserIcon", "I");
    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetFieldID: showUserIcon %d", __LINE__);

    jint jshowUserIcon = env->GetIntField(jproviderInfo, f);
    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetIntField: showUserIcon %d", __LINE__);
    pkgInfo->SetShowUserIcon((Int32)jshowUserIcon);

    f = env->GetFieldID(piKlass, "themedIcon", "I");
    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetFieldID: themedIcon %d", __LINE__);

    jint jthemedIcon = env->GetIntField(jproviderInfo, f);
    Util::CheckErrorAndLog(env, "GetElProviderInfo", "Fail GetIntField: themedIcon %d", __LINE__);
    pkgInfo->SetThemedIcon((Int32)jthemedIcon);

    env->DeleteLocalRef(piKlass);

    return TRUE;
}

jobject Util::ToJavaUri(
    /* [in] */ JNIEnv* env,
    /* [in] */ IUri* uri)
{
    if (env == NULL || uri == NULL) {
        LOGGERE("ToJavaUri", "Invalid arguments!");
        return NULL;
    }

    jobject juri = NULL;

    AutoPtr<IObject> object = IObject::Probe(uri);

    ClassID clsid;
    object->GetClassID(&clsid);
    if (ECLSID_CStringUri == clsid) {
        jclass suriKlass = env->FindClass("android/net/Uri$StringUri");
        Util::CheckErrorAndLog(env, "ToJavaUri", "Fail FindClass: Intent %d", __LINE__);

        jmethodID m = env->GetMethodID(suriKlass, "<init>", "(Ljava/lang/String;)V");
        Util::CheckErrorAndLog(env, "ToJavaUri", "Fail GetMethodID: intentKlass %d", __LINE__);

        String suri;
        IObject::Probe(uri)->ToString(&suri);
        jstring jsuri = Util::ToJavaString(env, suri);

        juri = env->NewObject(suriKlass, m, jsuri);
        Util::CheckErrorAndLog(env, "ToJavaUri", "Fail NewObject: intentKlass %d", __LINE__);

        env->DeleteLocalRef(suriKlass);
        env->DeleteLocalRef(jsuri);
    }
    else if (ECLSID_CHierarchicalUri == clsid) {
        AutoPtr<IParcel> parcel;
        Elastos::Droid::Os::CParcel::New((IParcel**)&parcel);
        IParcelable::Probe(uri)->WriteToParcel(parcel);
        parcel->SetDataPosition(0);

        Int32 id;
        parcel->ReadInt32(&id);
        String scheme;
        parcel->ReadString(&scheme);

        jstring jscheme = ToJavaString(env, scheme);

        jclass uriKlass = env->FindClass("android/net/Uri");
        CheckErrorAndLog(env, "ToJavaUri", "Fail FindClass: Uri : %d!\n", __LINE__);

        jfieldID fid = env->GetStaticFieldID(uriKlass, "NOT_CACHED", "Ljava/lang/String;");
        CheckErrorAndLog(env, "ToJavaUri", "GetStaticFieldID: %d!\n", __LINE__);

        jstring jNOT_CACHED = (jstring)env->GetStaticObjectField(uriKlass, fid);
        CheckErrorAndLog(env, "ToJavaNetworkInfo", "GetStaticObjectField: %d!\n", __LINE__);

        Int32 representation;
        parcel->ReadInt32(&representation);
        String encode, decode;
        jstring jencode = NULL, jdecode = NULL;
        switch (representation) {
            case 0/*Representation::BOTH*/:
                parcel->ReadString(&encode);
                jencode = ToJavaString(env, encode);
                parcel->ReadString(&decode);
                jdecode = ToJavaString(env, decode);
                break;

            case 1/*Representation::ENCODED*/:
                parcel->ReadString(&encode);
                jencode = ToJavaString(env, encode);
                jdecode = jNOT_CACHED;
                break;

            case 2/*Representation::DECODED*/:
                jencode = jNOT_CACHED;
                parcel->ReadString(&decode);
                jdecode = ToJavaString(env, decode);
                break;
            default:
                assert(0);
        }

        jclass partKlass = env->FindClass("android/net/Uri$Part");
        CheckErrorAndLog(env, "ToJavaUri", "Fail FindClass: Part %d", __LINE__);

        jmethodID mid = env->GetStaticMethodID(partKlass, "from", "(Ljava/lang/String;Ljava/lang/String;)Landroid/net/Uri$Part;");
        CheckErrorAndLog(env, "ToJavaUri", "Fail GetMethodID: partKlass %d", __LINE__);

        jobject jauthority = env->CallStaticObjectMethod(partKlass, mid, jencode, jdecode);
        CheckErrorAndLog(env, "ToJavaUri", "Fail call static method from() %d", __LINE__);

        if (jencode != jNOT_CACHED) env->DeleteLocalRef(jencode);
        if (jdecode != jNOT_CACHED) env->DeleteLocalRef(jdecode);

        parcel->ReadInt32(&representation);
        switch (representation) {
            case 0/*Representation::BOTH*/:
                parcel->ReadString(&encode);
                jencode = ToJavaString(env, encode);
                parcel->ReadString(&decode);
                jdecode = ToJavaString(env, decode);
                break;

            case 1/*Representation::ENCODED*/:
                parcel->ReadString(&encode);
                jencode = ToJavaString(env, encode);
                jdecode = jNOT_CACHED;
                break;

            case 2/*Representation::DECODED*/:
                jencode = jNOT_CACHED;
                parcel->ReadString(&decode);
                jdecode = ToJavaString(env, decode);
                break;
            default:
                assert(0);
        }

        jclass pathPartKlass = env->FindClass("android/net/Uri$PathPart");
        CheckErrorAndLog(env, "ToJavaUri", "Fail FindClass: PathPart %d", __LINE__);

        jmethodID mid2 = env->GetStaticMethodID(pathPartKlass, "from", "(Ljava/lang/String;Ljava/lang/String;)Landroid/net/Uri$PathPart;");
        CheckErrorAndLog(env, "ToJavaUri", "Fail GetMethodID: pathPartKlass %d", __LINE__);

        jobject jpath = env->CallStaticObjectMethod(pathPartKlass, mid2, jencode, jdecode);
        CheckErrorAndLog(env, "ToJavaUri", "Fail call static method from() %d", __LINE__);
        if (jencode != jNOT_CACHED) env->DeleteLocalRef(jencode);
        if (jdecode != jNOT_CACHED) env->DeleteLocalRef(jdecode);

        parcel->ReadInt32(&representation);
        switch (representation) {
            case 0/*Representation::BOTH*/:
                parcel->ReadString(&encode);
                jencode = ToJavaString(env, encode);
                parcel->ReadString(&decode);
                jdecode = ToJavaString(env, decode);
                break;

            case 1/*Representation::ENCODED*/:
                parcel->ReadString(&encode);
                jencode = ToJavaString(env, encode);
                jdecode = jNOT_CACHED;
                break;

            case 2/*Representation::DECODED*/:
                jencode = jNOT_CACHED;
                parcel->ReadString(&decode);
                jdecode = ToJavaString(env, decode);
                break;
            default:
                assert(0);
        }

        jobject jquery = env->CallStaticObjectMethod(partKlass, mid, jencode, jdecode);
        CheckErrorAndLog(env, "ToJavaUri", "Fail call static method from() %d", __LINE__);
        if (jencode != jNOT_CACHED) env->DeleteLocalRef(jencode);
        if (jdecode != jNOT_CACHED) env->DeleteLocalRef(jdecode);

        parcel->ReadInt32(&representation);
        switch (representation) {
            case 0/*Representation::BOTH*/:
                parcel->ReadString(&encode);
                jencode = ToJavaString(env, encode);
                parcel->ReadString(&decode);
                jdecode = ToJavaString(env, decode);
                break;

            case 1/*Representation::ENCODED*/:
                parcel->ReadString(&encode);
                jencode = ToJavaString(env, encode);
                jdecode = jNOT_CACHED;
                break;

            case 2/*Representation::DECODED*/:
                jencode = jNOT_CACHED;
                parcel->ReadString(&decode);
                jdecode = ToJavaString(env, decode);
                break;
            default:
                assert(0);
        }

        jobject jfragment = env->CallStaticObjectMethod(partKlass, mid, jencode, jdecode);
        CheckErrorAndLog(env, "ToJavaUri", "Fail call static method from() %d", __LINE__);
        if (jencode != jNOT_CACHED) env->DeleteLocalRef(jencode);
        if (jdecode != jNOT_CACHED) env->DeleteLocalRef(jdecode);

        jclass huriKlass = env->FindClass("android/net/Uri$HierarchicalUri");
        CheckErrorAndLog(env, "ToJavaUri", "Fail FindClass: HierarchicalUri %d", __LINE__);

        jmethodID m = env->GetMethodID(huriKlass, "<init>", "(Ljava/lang/String;Landroid/net/Uri$Part;Landroid/net/Uri$PathPart;Landroid/net/Uri$Part;Landroid/net/Uri$Part;)V");
        CheckErrorAndLog(env, "ToJavaUri", "Fail GetMethodID: huriKlass %d", __LINE__);

        juri = env->NewObject(huriKlass, m, jscheme, jauthority, jpath, jquery, jfragment);
        CheckErrorAndLog(env, "ToJavaUri", "Fail NewObject: huriKlass %d", __LINE__);

        env->DeleteLocalRef(partKlass);
        env->DeleteLocalRef(pathPartKlass);
        env->DeleteLocalRef(huriKlass);
        env->DeleteLocalRef(uriKlass);

        env->DeleteLocalRef(jscheme);
        env->DeleteLocalRef(jauthority);
        env->DeleteLocalRef(jpath);
        env->DeleteLocalRef(jquery);
        env->DeleteLocalRef(jfragment);
        env->DeleteLocalRef(jNOT_CACHED);
    }
    else if (ECLSID_COpaqueUri == clsid) {
        AutoPtr<IParcel> parcel;
        Elastos::Droid::Os::CParcel::New((IParcel**)&parcel);
        IParcelable::Probe(uri)->WriteToParcel(parcel);
        parcel->SetDataPosition(0);

        Int32 id;
        parcel->ReadInt32(&id);
        String scheme;
        parcel->ReadString(&scheme);

        jstring jscheme = ToJavaString(env, scheme);

        jclass uriKlass = env->FindClass("android/net/Uri");
        CheckErrorAndLog(env, "ToJavaUri", "Fail FindClass: Uri : %d!\n", __LINE__);

        jfieldID fid = env->GetStaticFieldID(uriKlass, "NOT_CACHED", "Ljava/lang/String;");
        CheckErrorAndLog(env, "ToJavaUri", "GetStaticFieldID: %d!\n", __LINE__);

        jstring jNOT_CACHED = (jstring)env->GetStaticObjectField(uriKlass, fid);
        CheckErrorAndLog(env, "ToJavaNetworkInfo", "GetStaticObjectField: %d!\n", __LINE__);

        Int32 representation;
        parcel->ReadInt32(&representation);
        String encode, decode;
        jstring jencode = NULL, jdecode = NULL;
        switch (representation) {
            case 0/*Representation::BOTH*/:
                parcel->ReadString(&encode);
                jencode = ToJavaString(env, encode);
                parcel->ReadString(&decode);
                jdecode = ToJavaString(env, decode);
                break;

            case 1/*Representation::ENCODED*/:
                parcel->ReadString(&encode);
                jencode = ToJavaString(env, encode);
                jdecode = jNOT_CACHED;
                break;

            case 2/*Representation::DECODED*/:
                jencode = jNOT_CACHED;
                parcel->ReadString(&decode);
                jdecode = ToJavaString(env, decode);
                break;
            default:
                assert(0);
        }

        jclass partKlass = env->FindClass("android/net/Uri$Part");
        CheckErrorAndLog(env, "ToJavaUri", "Fail FindClass: Part %d", __LINE__);

        jmethodID mid = env->GetStaticMethodID(partKlass, "from", "(Ljava/lang/String;Ljava/lang/String;)Landroid/net/Uri$Part;");
        CheckErrorAndLog(env, "ToJavaUri", "Fail GetMethodID: partKlass %d", __LINE__);

        jobject jssp = env->CallStaticObjectMethod(partKlass, mid, jencode, jdecode);
        CheckErrorAndLog(env, "ToJavaUri", "Fail call static method from() %d", __LINE__);
        if (jencode != jNOT_CACHED) env->DeleteLocalRef(jencode);
        if (jdecode != jNOT_CACHED) env->DeleteLocalRef(jdecode);

        parcel->ReadInt32(&representation);
        switch (representation) {
            case 0/*Representation::BOTH*/:
                parcel->ReadString(&encode);
                jencode = ToJavaString(env, encode);
                parcel->ReadString(&decode);
                jdecode = ToJavaString(env, decode);
                break;

            case 1/*Representation::ENCODED*/:
                parcel->ReadString(&encode);
                jencode = ToJavaString(env, encode);
                jdecode = jNOT_CACHED;
                break;

            case 2/*Representation::DECODED*/:
                jencode = jNOT_CACHED;
                parcel->ReadString(&decode);
                jdecode = ToJavaString(env, decode);
                break;
            default:
                assert(0);
        }

        jobject jfragment = env->CallStaticObjectMethod(partKlass, mid, jencode, jdecode);
        CheckErrorAndLog(env, "ToJavaUri", "Fail call static method from() %d", __LINE__);

        if (jencode != jNOT_CACHED) env->DeleteLocalRef(jencode);
        if (jdecode != jNOT_CACHED) env->DeleteLocalRef(jdecode);

        jclass opaqueKlass = env->FindClass("android/net/Uri$OpaqueUri");
        CheckErrorAndLog(env, "ToJavaUri", "Fail FindClass: OpaqueUri %d", __LINE__);

        jmethodID m = env->GetMethodID(opaqueKlass, "<init>", "(Ljava/lang/String;Landroid/net/Uri$Part;Landroid/net/Uri$Part;)V");
        CheckErrorAndLog(env, "ToJavaUri", "Fail GetMethodID: opaqueKlass %d", __LINE__);

        juri = env->NewObject(opaqueKlass, m, jscheme, jssp, jfragment);
        CheckErrorAndLog(env, "ToJavaUri", "Fail NewObject: opaqueKlass %d", __LINE__);

        env->DeleteLocalRef(partKlass);
        env->DeleteLocalRef(opaqueKlass);
        env->DeleteLocalRef(uriKlass);
        env->DeleteLocalRef(jscheme);
        env->DeleteLocalRef(jssp);
        env->DeleteLocalRef(jfragment);
        env->DeleteLocalRef(jNOT_CACHED);
    }
    else {
        LOGGERE("ToJavaUri", "ToJavaUri() others type uri!");
    }

    return juri;
}

jobject Util::ToJavaClipData(
    /* [in] */ JNIEnv* env,
    /* [in] */ IClipData* clipData)
{
    if (env == NULL || clipData == NULL) {
        LOGGERE("ToJavaClipData", "Invalid arguments!");
        return NULL;
    }

    jobject jclipData = NULL;

    jclass cdKlass = env->FindClass("android/content/ClipData");
    CheckErrorAndLog(env, "ToJavaClipData", "FindClass: ClipData : %d!\n", __LINE__);

    jmethodID m = env->GetMethodID(cdKlass, "<init>", "(Landroid/content/ClipDescription;Landroid/content/ClipData$Item;)V");
    CheckErrorAndLog(env, "ToJavaClipData", "GetMethodID: ClipData : %d!\n", __LINE__);

    AutoPtr<IClipDescription> clipDescription;
    clipData->GetDescription((IClipDescription**)&clipDescription);
    jobject jclipDescription = NULL;
    if (clipDescription != NULL) {
        jclipDescription = Util::ToJavaClipDescription(env, clipDescription);
    }

    AutoPtr<IClipDataItem> item;
    clipData->GetItemAt(0, (IClipDataItem**)&item);
    jobject jitem = NULL;
    if (item != NULL) {
        jitem = Util::ToJavaClipDataItem(env, item);
    }

    jclipData = env->NewObject(cdKlass, m, jclipDescription, jitem);
    CheckErrorAndLog(env, "ToJavaClipData", "NewObject: ClipData : %d!\n", __LINE__);

    m = env->GetMethodID(cdKlass, "addItem", "(Landroid/content/ClipData$Item;)V");
    CheckErrorAndLog(env, "ToJavaClipData", "GetFieldID: addItem : %d!", __LINE__);

    Int32 count = 0;
    clipData->GetItemCount(&count);
    for (Int32 i = 1; i < count; i++) {
        AutoPtr<IClipDataItem> it;
        clipData->GetItemAt(i, (IClipDataItem**)&it);

        jobject jit = Util::ToJavaClipDataItem(env, item);

        env->CallVoidMethod(jclipData, m, jit);
        CheckErrorAndLog(env, "ToJavaClipData", "CallVoidMethod: addItem : %d!", __LINE__);
        env->DeleteLocalRef(jit);
    }

    AutoPtr<IBitmap> icon;
    clipData->GetIcon((IBitmap**)&icon);
    if (icon != NULL) {
        jobject jicon = Util::ToJavaBitmap(env, icon);

        jfieldID f = env->GetFieldID(cdKlass, "mIcon", "Landroid/graphics/Bitmap;");
        CheckErrorAndLog(env, "ToJavaClipData", "GetFieldID: mIcon : %d!", __LINE__);

        env->SetObjectField(jclipData, f, jicon);
        CheckErrorAndLog(env, "ToJavaClipData", "SetObjectField: mIcon : %d!", __LINE__);
        env->DeleteLocalRef(jicon);
    }

    env->DeleteLocalRef(cdKlass);
    env->DeleteLocalRef(jclipDescription);
    env->DeleteLocalRef(jitem);
    return jclipData;
}

jobject Util::ToJavaClipDescription(
    /* [in] */ JNIEnv* env,
    /* [in] */ IClipDescription* clipDesc)
{
    if (env == NULL || clipDesc == NULL) {
        ALOGE("ToJavaClipDescription() Invalid arguments!");
        return NULL;
    }

    jclass cdKlass = env->FindClass("android/content/ClipDescription");
    CheckErrorAndLog(env, "ToJavaClipDescription", "FindClass: ClipDescription : %d!\n", __LINE__);

    jmethodID m = env->GetMethodID(cdKlass, "<init>", "(Ljava/lang/CharSequence;[Ljava/lang/String;)V");
    CheckErrorAndLog(env, "ToJavaClipDescription", "GetMethodID: ClipDescription : %d!\n", __LINE__);

    AutoPtr<ICharSequence> label;
    clipDesc->GetLabel((ICharSequence**)&label);
    jobject jlabel = NULL;
    if (label != NULL){
        String slabel;
        label->ToString(&slabel);
        jlabel = Util::ToJavaString(env, slabel);
    }

    Int32 count = 0;
    clipDesc->GetMimeTypeCount(&count);
    jobjectArray jmimeTypes = NULL;
    if (count > 0) {
        AutoPtr<ArrayOf<String> > mimeTypes = ArrayOf<String>::Alloc(count);
        for (Int32 i = 0; i < count; i++) {
            String mimeType;
            clipDesc->GetMimeType(i, &mimeType);
            (*mimeTypes)[i] = mimeType;
        }

        jmimeTypes = Util::ToJavaStringArray(env, mimeTypes);
    }

    jobject jclipDesc = env->NewObject(cdKlass, m, jlabel, jmimeTypes);
    CheckErrorAndLog(env, "ToJavaClipDescription", "NewObject: ClipDescription : %d!\n", __LINE__);

    env->DeleteLocalRef(cdKlass);
    return jclipDesc;
}

jobject Util::ToJavaClipDataItem(
    /* [in] */ JNIEnv* env,
    /* [in] */ IClipDataItem* item)
{
    if (env == NULL || item == NULL) {
        ALOGE("ToJavaClipDataItem() Invalid arguments!");
        return NULL;
    }

    jobject jitem = NULL;

    jclass cdiKlass = env->FindClass("android/content/ClipData$Item");
    CheckErrorAndLog(env, "ToJavaClipDataItem", "FindClass: ClipData$Item : %d!\n", __LINE__);

    jmethodID m = env->GetMethodID(cdiKlass, "<init>", "(Ljava/lang/CharSequence;Ljava/lang/String;Landroid/content/Intent;Landroid/net/Uri;)V");
    CheckErrorAndLog(env, "ToJavaClipDataItem", "GetMethodID: ClipData$Item : %d!\n", __LINE__);

    AutoPtr<ICharSequence> text;
    item->GetText((ICharSequence**)&text);
    jobject jtext = NULL;
    if (text != NULL) {
        String stext;
        text->ToString(&stext);
        jtext = Util::ToJavaString(env, stext);
    }

    String htmlText;
    item->GetHtmlText(&htmlText);
    jstring jhtmlText = Util::ToJavaString(env, htmlText);

    AutoPtr<IIntent> intent;
    item->GetIntent((IIntent**)&intent);
    jobject jintent = NULL;
    if (intent != NULL) {
        jintent = Util::ToJavaIntent(env, intent);
    }

    AutoPtr<IUri> uri;
    item->GetUri((IUri**)&uri);
    jobject juri = NULL;
    if (uri != NULL) {
        juri = Util::ToJavaUri(env, uri);
    }

    jitem = env->NewObject(cdiKlass, m, jtext, jhtmlText, jintent, juri);
    Util::CheckErrorAndLog(env, "ToJavaClipDataItem", "NewObject: ClipData : %d!\n", __LINE__);

    if (juri) env->DeleteLocalRef(juri);
    if (jintent) env->DeleteLocalRef(jintent);
    env->DeleteLocalRef(cdiKlass);
    return jitem;
}

jobject Util::ToJavaInputBindResult(
    /* [in] */ JNIEnv* env,
    /* [in] */ IInputBindResult* result)
{
    if (env == NULL || result == NULL) {
        LOGGERE("ToJavaInputBindResult()", "Invalid arguments!");
        return NULL;
    }

    AutoPtr<IIInputMethodSession> method;
    AutoPtr<IInputChannel> channel;
    String id;
    Int32 sequence, userActionNotificationSequenceNumber;
    result->GetMethod((IIInputMethodSession**)&method);
    result->GetChannel((IInputChannel**)&channel);
    result->GetId(&id);
    result->GetSequence(&sequence);
    result->GetUserActionNotificationSequenceNumber(&userActionNotificationSequenceNumber);

    jobject jIms = NULL;
    if (method != NULL) {
        jclass imsKlass = env->FindClass("android/inputmethodservice/ElIInputMethodSessionProxy");
        Util::CheckErrorAndLog(env, "ToJavaInputBindResult", "FindClass: ElIInputMethodSessionProxy  %d", __LINE__);

        jmethodID m = env->GetMethodID(imsKlass, "<init>", "(J)V");
        Util::CheckErrorAndLog(env, "ToJavaInputBindResult", "GetMethodID: ElIInputMethodSessionProxy  %d", __LINE__);

        jIms = env->NewObject(imsKlass, m, (jlong)method.Get());
        Util::CheckErrorAndLog(env, "ToJavaInputBindResult", "NewObject: ElIInputMethodSessionProxy  %d", __LINE__);
        method->AddRef();

        env->DeleteLocalRef(imsKlass);
    }

    jobject jchannel = NULL;
    if (channel != NULL) {
        jchannel = ToJavaInputChannel(env, channel);
    }

    jstring jid = Util::ToJavaString(env, id);

    jclass ibrKlass = env->FindClass("com/android/internal/view/InputBindResult");
    Util::CheckErrorAndLog(env, "ToJavaInputBindResult", "FindClass: InputBindResult  %d", __LINE__);

    jmethodID m = env->GetMethodID(ibrKlass, "<init>", "(Lcom/android/internal/view/IInputMethodSession;Landroid/view/InputChannel;Ljava/lang/String;II)V");
    Util::CheckErrorAndLog(env, "ToJavaInputBindResult", "GetMethodID: InputBindResult  %d", __LINE__);

    jobject jres = env->NewObject(ibrKlass, m, jIms, jchannel, jid, (jint)sequence, (jint)userActionNotificationSequenceNumber);
    Util::CheckErrorAndLog(env, "ToJavaInputBindResult", "GetMethodID: InputBindResult  %d", __LINE__);
    env->DeleteLocalRef(ibrKlass);
    env->DeleteLocalRef(jid);
    env->DeleteLocalRef(jIms);

    return jres;
}

jobject Util::ToJavaInputChannel(
    /* [in] */ JNIEnv* env,
    /* [in] */ IInputChannel* channel)
{
    if (env == NULL || channel == NULL){
        Logger::E("Util", "ToJavaInputChannel() INVALID params");
        return NULL;
    }

    AutoPtr<IParcel> parcel;
    CParcel::New((IParcel**)&parcel);

    IParcelable::Probe(channel)->WriteToParcel(parcel);
    parcel->SetDataPosition(0);
    Handle32 source;
    parcel->GetElementPayload(&source);

    jclass parcelClass = env->FindClass("android/os/Parcel");
    CheckErrorAndLog(env, "FindClass: Parcel : %d!\n", __LINE__);

    jmethodID m = env->GetStaticMethodID(parcelClass, "obtain", "(J)Landroid/os/Parcel;");
    CheckErrorAndLog(env, "GetMethodID: obtain : %d!\n", __LINE__);

    jobject jparcel = env->CallStaticObjectMethod(parcelClass, m, (Int64)source);
    CheckErrorAndLog(env, "CallStaticObjectMethod: obtain : %d!\n", __LINE__);

    jclass inputChannelClass = env->FindClass("android/view/InputChannel");
    CheckErrorAndLog(env, "ToJavaInputChannel", "FindClass: InputChannel : %d!\n", __LINE__);

    m = env->GetMethodID(inputChannelClass, "<init>", "()V");
    Util::CheckErrorAndLog(env, "ToJavaInputChannel", "Fail GetMethodID: InputChannel %d", __LINE__);

    jobject jchannel = env->NewObject(inputChannelClass, m);
    Util::CheckErrorAndLog(env, "ToJavaInputChannel", "Fail NewObject: InputChannel %d", __LINE__);

    m = env->GetMethodID(inputChannelClass, "readFromParcel", "(Landroid/os/Parcel;)V");
    CheckErrorAndLog(env, "GetMethodID: readFromParcel : %d!\n", __LINE__);

    env->CallVoidMethod(jchannel, m, jparcel);
    CheckErrorAndLog(env, "CallVoidMethod: readFromParcel : %d!\n", __LINE__);

    env->DeleteLocalRef(parcelClass);
    env->DeleteLocalRef(jparcel);
    env->DeleteLocalRef(inputChannelClass);

    return jchannel;
}

Boolean Util::GetElRect(
    /* [in] */ JNIEnv* env,
    /* [in] */ jobject jrect,
    /* [out] */ IRect** elRect)
{
    if (elRect == NULL) {
        LOGGERE("GetElRect()", "Invalid argumenet!");
        return FALSE;
    }

    jclass rectKlass = env->FindClass("android/graphics/Rect");
    Util::CheckErrorAndLog(env, "GetElRect", "Fail FindClass: Rect  %d", __LINE__);

    if (NOERROR != CRect::New(elRect)) {
        LOGGERE("GetElRect", "create CRect fail!");
        return FALSE;
    }

    jfieldID f = env->GetFieldID(rectKlass, "left", "I");
    Util::CheckErrorAndLog(env, "GetElRect", "GetFieldID: left  %d", __LINE__);

    jint jtmpInt = env->GetIntField(jrect, f);
    Util::CheckErrorAndLog(env, "GetElRect", "GetIntField left  %d", __LINE__);
    (*elRect)->SetLeft((Int32)jtmpInt);

    f = env->GetFieldID(rectKlass, "top", "I");
    Util::CheckErrorAndLog(env, "GetElRect", "GetFieldID: top  %d", __LINE__);

    jtmpInt = env->GetIntField(jrect, f);
    Util::CheckErrorAndLog(env, "GetElRect", "GetIntField top  %d", __LINE__);
    (*elRect)->SetTop((Int32)jtmpInt);

    f = env->GetFieldID(rectKlass, "right", "I");
    Util::CheckErrorAndLog(env, "GetElRect", "GetFieldID: right  %d", __LINE__);

    jtmpInt = env->GetIntField(jrect, f);
    Util::CheckErrorAndLog(env, "GetElRect", "GetIntField right  %d", __LINE__);
    (*elRect)->SetRight((Int32)jtmpInt);

    f = env->GetFieldID(rectKlass, "bottom", "I");
    Util::CheckErrorAndLog(env, "GetElRect", "GetFieldID: bottom  %d", __LINE__);

    jtmpInt = env->GetIntField(jrect, f);
    Util::CheckErrorAndLog(env, "GetElRect", "GetIntField bottom  %d", __LINE__);
    (*elRect)->SetBottom((Int32)jtmpInt);

    env->DeleteLocalRef(rectKlass);

    return TRUE;
}

jobject Util::ToJavaKeyEvent(
    /* [in] */ JNIEnv* env,
    /* [in] */ IKeyEvent* keyEvent)
{
    if (env == NULL || keyEvent == NULL) {
        LOGGERE("ToJavaKeyEvent()", "Invalid arguments!");
        return NULL;
    }

    jobject jkeyEvent = NULL;

    jclass eventKlass = env->FindClass("android/view/KeyEvent");
    Util::CheckErrorAndLog(env, "ToJavaKeyEvent", "Fail FindClass: KeyEvent %d", __LINE__);

    jmethodID m = env->GetMethodID(eventKlass, "<init>", "()V");
    Util::CheckErrorAndLog(env, "ToJavaKeyEvent", "Fail GetMethodID: KeyEvent %d", __LINE__);

    jkeyEvent = env->NewObject(eventKlass, m);
    Util::CheckErrorAndLog(env, "ToJavaKeyEvent", "Fail NewObject: KeyEvent %d", __LINE__);

    IInputEvent* inputEvent = IInputEvent::Probe(keyEvent);
    Int32 tempInt;
    inputEvent->GetDeviceId(&tempInt);
    jfieldID f = env->GetFieldID(eventKlass, "mDeviceId", "I");
    Util::CheckErrorAndLog(env, "ToJavaKeyEvent", "Fail GetFieldID: mDeviceId %d", __LINE__);

    env->SetIntField(jkeyEvent, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaKeyEvent", "Fail SetIntField: mDeviceId %d", __LINE__);

    inputEvent->GetSource(&tempInt);
    f = env->GetFieldID(eventKlass, "mSource", "I");
    Util::CheckErrorAndLog(env, "ToJavaKeyEvent", "Fail GetFieldID: mSource %d", __LINE__);

    env->SetIntField(jkeyEvent, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaKeyEvent", "Fail SetIntField: mSource %d", __LINE__);

    keyEvent->GetAction(&tempInt);
    f = env->GetFieldID(eventKlass, "mAction", "I");
    Util::CheckErrorAndLog(env, "ToJavaKeyEvent", "Fail GetFieldID: mAction %d", __LINE__);

    env->SetIntField(jkeyEvent, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaKeyEvent", "Fail SetIntField: mAction %d", __LINE__);

    keyEvent->GetKeyCode(&tempInt);
    f = env->GetFieldID(eventKlass, "mKeyCode", "I");
    Util::CheckErrorAndLog(env, "ToJavaKeyEvent", "Fail GetFieldID: mKeyCode %d", __LINE__);

    env->SetIntField(jkeyEvent, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaKeyEvent", "Fail SetIntField: mKeyCode %d", __LINE__);

    keyEvent->GetRepeatCount(&tempInt);
    f = env->GetFieldID(eventKlass, "mRepeatCount", "I");
    Util::CheckErrorAndLog(env, "ToJavaKeyEvent", "Fail GetFieldID: mRepeatCount %d", __LINE__);

    env->SetIntField(jkeyEvent, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaKeyEvent", "Fail SetIntField: mRepeatCount %d", __LINE__);

    keyEvent->GetMetaState(&tempInt);
    f = env->GetFieldID(eventKlass, "mMetaState", "I");
    Util::CheckErrorAndLog(env, "ToJavaKeyEvent", "Fail GetFieldID: mMetaState %d", __LINE__);

    env->SetIntField(jkeyEvent, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaKeyEvent", "Fail SetIntField: mMetaState %d", __LINE__);

    keyEvent->GetScanCode(&tempInt);
    f = env->GetFieldID(eventKlass, "mScanCode", "I");
    Util::CheckErrorAndLog(env, "ToJavaKeyEvent", "Fail GetFieldID: mScanCode %d", __LINE__);

    env->SetIntField(jkeyEvent, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaKeyEvent", "Fail SetIntField: mScanCode %d", __LINE__);

    keyEvent->GetFlags(&tempInt);
    f = env->GetFieldID(eventKlass, "mFlags", "I");
    Util::CheckErrorAndLog(env, "ToJavaKeyEvent", "Fail GetFieldID: mFlags %d", __LINE__);

    env->SetIntField(jkeyEvent, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaKeyEvent", "Fail SetIntField: mFlags %d", __LINE__);

    Int64 tempLong;
    keyEvent->GetDownTime(&tempLong);
    f = env->GetFieldID(eventKlass, "mDownTime", "J");
    Util::CheckErrorAndLog(env, "ToJavaKeyEvent", "Fail GetFieldID: mDownTime %d", __LINE__);

    env->SetLongField(jkeyEvent, f, tempLong);
    Util::CheckErrorAndLog(env, "ToJavaKeyEvent", "Fail SetLongField: mDownTime %d", __LINE__);

    inputEvent->GetEventTime(&tempLong);
    f = env->GetFieldID(eventKlass, "mEventTime", "J");
    Util::CheckErrorAndLog(env, "ToJavaKeyEvent", "Fail GetFieldID: mEventTime %d", __LINE__);

    env->SetLongField(jkeyEvent, f, tempLong);
    Util::CheckErrorAndLog(env, "ToJavaKeyEvent", "Fail SetLongField: mEventTime %d", __LINE__);

    env->DeleteLocalRef(eventKlass);
    return jkeyEvent;
}

void Util::SetComponentInfo(
    /* [in] */ JNIEnv* env,
    /* [in] */ jclass parentClass,
    /* [in] */ jobject jparent,
    /* [in] */ IComponentInfo* comInfo)
{
    if (env == NULL || parentClass == NULL || jparent == NULL || comInfo == NULL) {
        LOGGERE("SetComponentInfo()", "Invalid arguments!");
        return;
    }

    AutoPtr<IApplicationInfo> appInfo;
    comInfo->GetApplicationInfo((IApplicationInfo**)&appInfo);
    if (appInfo != NULL) {
        jobject jAppInfo = Util::ToJavaApplicationInfo(env, appInfo);
        if (jAppInfo != NULL) {
            jfieldID f = env->GetFieldID(parentClass, "applicationInfo", "Landroid/content/pm/ApplicationInfo;");
            Util::CheckErrorAndLog(env, "ToJavaActivityInfo", "Fail GetFieldID: ApplicationInfo %d", __LINE__);

            env->SetObjectField(jparent, f, jAppInfo);
            Util::CheckErrorAndLog(env, "ToJavaActivityInfo", "Fail SetObjectField: jAppInfo %d", __LINE__);
            env->DeleteLocalRef(jAppInfo);
        }
        else {
            LOGGERD("ToJavaActivityInfo", "Error: jExtras is NULL!");
        }
    }

    String processName;
    comInfo->GetProcessName(&processName);
    jstring jprocessName = Util::ToJavaString(env, processName);
    if (jprocessName != NULL) {
        jfieldID f = env->GetFieldID(parentClass, "processName", "Ljava/lang/String;");
        Util::CheckErrorAndLog(env, "ToJavaActivityInfo", "Fail GetFieldID: processName %d", __LINE__);

        env->SetObjectField(jparent, f, jprocessName);
        Util::CheckErrorAndLog(env, "ToJavaActivityInfo", "Fail SetIntField: jprocessName %d", __LINE__);
        env->DeleteLocalRef(jprocessName);
    }

    Int32 tempInt;
    comInfo->GetDescriptionRes(&tempInt);
    jfieldID f = env->GetFieldID(parentClass, "descriptionRes", "I");
    Util::CheckErrorAndLog(env, "ToJavaActivityInfo", "Fail GetFieldID: descriptionRes %d", __LINE__);

    env->SetIntField(jparent, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaActivityInfo", "Fail SetIntField: descriptionRes %d", __LINE__);

    Boolean tempBool;
    comInfo->GetEnabled(&tempBool);
    f = env->GetFieldID(parentClass, "enabled", "Z");
    Util::CheckErrorAndLog(env, "ToJavaActivityInfo", "Fail GetFieldID: enabled %d", __LINE__);

    env->SetBooleanField(jparent, f, tempBool);
    Util::CheckErrorAndLog(env, "ToJavaActivityInfo", "Fail SetIntField: enabled %d", __LINE__);

    comInfo->GetExported(&tempBool);
    f = env->GetFieldID(parentClass, "exported", "Z");
    Util::CheckErrorAndLog(env, "ToJavaActivityInfo", "Fail GetFieldID: exported %d", __LINE__);

    env->SetBooleanField(jparent, f, tempBool);
    Util::CheckErrorAndLog(env, "ToJavaActivityInfo", "Fail SetIntField: exported %d", __LINE__);

    AutoPtr<IPackageItemInfo> pkgInfo = IPackageItemInfo::Probe(comInfo);
    SetPackageItemInfo(env, parentClass, jparent, pkgInfo);
}

void Util::SetPackageItemInfo(
    /* [in] */ JNIEnv* env,
    /* [in] */ jclass parentClass,
    /* [in] */ jobject jparent,
    /* [in] */ IPackageItemInfo* pkgInfo)
{
    if (env == NULL || parentClass == NULL || jparent == NULL || pkgInfo == NULL) {
        LOGGERE("SetPackageItemInfo()", "Invalid arguments!");
        return;
    }

    String name;
    pkgInfo->GetName(&name);
    jstring jname = Util::ToJavaString(env, name);
    if (jname != NULL) {
        jfieldID f = env->GetFieldID(parentClass, "name", "Ljava/lang/String;");
        Util::CheckErrorAndLog(env, "SetPackageItemInfo", "Fail GetFieldID: name %d", __LINE__);

        env->SetObjectField(jparent, f, jname);
        Util::CheckErrorAndLog(env, "SetPackageItemInfo", "Fail SetIntField: jname %d", __LINE__);
        env->DeleteLocalRef(jname);
    }

    String packageName;
    pkgInfo->GetPackageName(&packageName);
    jstring jpackageName = Util::ToJavaString(env, packageName);
    if (jpackageName != NULL) {
        jfieldID f = env->GetFieldID(parentClass, "packageName", "Ljava/lang/String;");
        Util::CheckErrorAndLog(env, "SetPackageItemInfo", "Fail GetFieldID: packageName %d", __LINE__);

        env->SetObjectField(jparent, f, jpackageName);
        Util::CheckErrorAndLog(env, "SetPackageItemInfo", "Fail SetIntField: jpackageName %d", __LINE__);
        env->DeleteLocalRef(jpackageName);
    }

    Int32 tempInt;
    pkgInfo->GetLabelRes(&tempInt);
    jfieldID f = env->GetFieldID(parentClass, "labelRes", "I");
    Util::CheckErrorAndLog(env, "SetPackageItemInfo", "Fail GetFieldID: labelRes %d", __LINE__);

    env->SetIntField(jparent, f, tempInt);
    Util::CheckErrorAndLog(env, "SetPackageItemInfo", "Fail SetIntField: labelRes %d", __LINE__);

    AutoPtr<ICharSequence> nonLocalizedLabel;
    pkgInfo->GetNonLocalizedLabel((ICharSequence**)&nonLocalizedLabel);
    if (nonLocalizedLabel != NULL) {
        String snonLocalizedLabel;
        nonLocalizedLabel->ToString(&snonLocalizedLabel);
        jstring jnonLocalizedLabel = Util::ToJavaString(env, snonLocalizedLabel);

        f = env->GetFieldID(parentClass, "nonLocalizedLabel", "Ljava/lang/CharSequence;");
        Util::CheckErrorAndLog(env, "SetPackageItemInfo", "Fail GetFieldID: CharSequence %d", __LINE__);

        env->SetObjectField(jparent, f, jnonLocalizedLabel);
        Util::CheckErrorAndLog(env, "SetPackageItemInfo", "Fail SetObjectField: jnonLocalizedLabel %d", __LINE__);
        env->DeleteLocalRef(jnonLocalizedLabel);
    }

    pkgInfo->GetIcon(&tempInt);
    f = env->GetFieldID(parentClass, "icon", "I");
    Util::CheckErrorAndLog(env, "SetPackageItemInfo", "Fail GetFieldID: icon %d", __LINE__);

    env->SetIntField(jparent, f, tempInt);
    Util::CheckErrorAndLog(env, "SetPackageItemInfo", "Fail SetIntField: icon %d", __LINE__);

    pkgInfo->GetLogo(&tempInt);
    f = env->GetFieldID(parentClass, "logo", "I");
    Util::CheckErrorAndLog(env, "SetPackageItemInfo", "Fail GetFieldID: logo %d", __LINE__);

    env->SetIntField(jparent, f, tempInt);
    Util::CheckErrorAndLog(env, "SetPackageItemInfo", "Fail SetIntField: logo %d", __LINE__);

    AutoPtr<IBundle> metaData;
    pkgInfo->GetMetaData((IBundle**)&metaData);
    if (metaData != NULL) {
        jobject jmetaData = Util::ToJavaBundle(env, metaData);
        if (jmetaData != NULL) {
            jfieldID f = env->GetFieldID(parentClass, "metaData", "Landroid/os/Bundle;");
            Util::CheckErrorAndLog(env, "SetPackageItemInfo", "Fail GetFieldID: metaData %d", __LINE__);

            env->SetObjectField(jparent, f, jmetaData);
            Util::CheckErrorAndLog(env, "SetPackageItemInfo", "Fail SetObjectField: jmetaData %d", __LINE__);
            env->DeleteLocalRef(jmetaData);
        }
        else {
            LOGGERD("SetPackageItemInfo", "Error: jmetaData is NULL!");
        }
    }

    pkgInfo->GetBanner(&tempInt);
    f = env->GetFieldID(parentClass, "banner", "I");
    Util::CheckErrorAndLog(env, "SetPackageItemInfo", "Fail GetFieldID: banner %d", __LINE__);

    env->SetIntField(jparent, f, tempInt);
    Util::CheckErrorAndLog(env, "SetPackageItemInfo", "Fail SetIntField: banner %d", __LINE__);

    pkgInfo->GetShowUserIcon(&tempInt);
    f = env->GetFieldID(parentClass, "showUserIcon", "I");
    Util::CheckErrorAndLog(env, "SetPackageItemInfo", "Fail GetFieldID: showUserIcon %d", __LINE__);

    env->SetIntField(jparent, f, tempInt);
    Util::CheckErrorAndLog(env, "SetPackageItemInfo", "Fail SetIntField: showUserIcon %d", __LINE__);

    pkgInfo->GetThemedIcon(&tempInt);
    f = env->GetFieldID(parentClass, "themedIcon", "I");
    Util::CheckErrorAndLog(env, "SetPackageItemInfo", "Fail GetFieldID: themedIcon %d", __LINE__);

    env->SetIntField(jparent, f, tempInt);
    Util::CheckErrorAndLog(env, "SetPackageItemInfo", "Fail SetIntField: themedIcon %d", __LINE__);
}

jobject Util::ToJavaServiceInfo(
    /* [in] */ JNIEnv* env,
    /* [in] */ IServiceInfo* sInfo)
{
    if (env == NULL || sInfo == NULL) {
        LOGGERE("ToJavaServiceInfo()", "Invalid arguments!");
        return NULL;
    }

    jobject jsInfo = NULL;

    jclass sInfoKlass = env->FindClass("android/content/pm/ServiceInfo");
    Util::CheckErrorAndLog(env, "ToJavaServiceInfo", "Fail FindClass: ServiceInfo %d", __LINE__);

    jmethodID m = env->GetMethodID(sInfoKlass, "<init>", "()V");
    Util::CheckErrorAndLog(env, "ToJavaServiceInfo", "Fail GetMethodID: ServiceInfo %d", __LINE__);

    jsInfo = env->NewObject(sInfoKlass, m);
    Util::CheckErrorAndLog(env, "ToJavaServiceInfo", "Fail NewObject: ServiceInfo %d", __LINE__);

    String permission;
    sInfo->GetPermission(&permission);
    jstring jpermission = Util::ToJavaString(env, permission);
    if (jpermission != NULL) {
        jfieldID f = env->GetFieldID(sInfoKlass, "permission", "Ljava/lang/String;");
        Util::CheckErrorAndLog(env, "ToJavaServiceInfo", "Fail GetFieldID: permission %d", __LINE__);

        env->SetObjectField(jsInfo, f, jpermission);
        Util::CheckErrorAndLog(env, "ToJavaServiceInfo", "Fail SetIntField: jpermission %d", __LINE__);
        env->DeleteLocalRef(jpermission);
    }

    Int32 flags = 0;
    sInfo->GetFlags(&flags);
    jfieldID f = env->GetFieldID(sInfoKlass, "flags", "I");
    Util::CheckErrorAndLog(env, "ToJavaServiceInfo", "Fail GetFieldID: flags %d", __LINE__);

    env->SetIntField(jsInfo, f, flags);
    Util::CheckErrorAndLog(env, "ToJavaServiceInfo", "Fail SetIntField: flags %d", __LINE__);

    AutoPtr<IComponentInfo> comInfo = IComponentInfo::Probe(sInfo);
    SetComponentInfo(env, sInfoKlass, jsInfo, comInfo);

    env->DeleteLocalRef(sInfoKlass);

    return jsInfo;
}

jobject Util::ToJavaCompatibilityInfo(
    /* [in] */ JNIEnv* env,
    /* [in] */ ICompatibilityInfo* cInfo)
{
    if (env == NULL || cInfo == NULL) {
        LOGGERE("ToJavaCompatibilityInfo()", "Invalid arguments!");
        return NULL;
    }

    jobject jcInfo = NULL;

    jclass cInfoKlass = env->FindClass("android/content/res/CompatibilityInfo");
    Util::CheckErrorAndLog(env, "ToJavaCompatibilityInfo", "FindClass: CompatibilityInfo %d", __LINE__);

    jmethodID m = env->GetMethodID(cInfoKlass, "<init>", "()V");
    Util::CheckErrorAndLog(env, "ToJavaCompatibilityInfo", "GetMethodID: CompatibilityInfo %d", __LINE__);

    jcInfo = env->NewObject(cInfoKlass, m);
    Util::CheckErrorAndLog(env, "ToJavaCompatibilityInfo", "NewObject: CompatibilityInfo %d", __LINE__);

    const Int32 SCALING_REQUIRED = 1;
    const Int32 ALWAYS_NEEDS_COMPAT = 2;
    const Int32 NEVER_NEEDS_COMPAT = 4;
    const Int32 NEEDS_SCREEN_COMPAT = 8;

    Int32 tempInt = 0;
    Boolean support;
    cInfo->IsScalingRequired(&support);
    if (support) {
        tempInt |= SCALING_REQUIRED;
    }
    cInfo->SupportsScreen(&support);
    if (!support) {
        tempInt |= NEEDS_SCREEN_COMPAT;
    }
    cInfo->NeverSupportsScreen(&support);
    if (support) {
        tempInt |= ALWAYS_NEEDS_COMPAT;
    }
    cInfo->AlwaysSupportsScreen(&support);
    if (support) {
        tempInt |= NEVER_NEEDS_COMPAT;
    }

    jfieldID f = env->GetFieldID(cInfoKlass, "mCompatibilityFlags", "I");
    Util::CheckErrorAndLog(env, "ToJavaCompatibilityInfo", "GetFieldID: mCompatibilityFlags %d", __LINE__);

    env->SetIntField(jcInfo, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaCompatibilityInfo", "SetIntField: mCompatibilityFlags %d", __LINE__);

    cInfo->GetApplicationDensity(&tempInt);
    f = env->GetFieldID(cInfoKlass, "applicationDensity", "I");
    Util::CheckErrorAndLog(env, "ToJavaCompatibilityInfo", "GetFieldID: applicationDensity %d", __LINE__);

    env->SetIntField(jcInfo, f, tempInt);
    Util::CheckErrorAndLog(env, "ToJavaCompatibilityInfo", "SetIntField: applicationDensity %d", __LINE__);

    Float tmpFloat;
    cInfo->GetApplicationScale(&tmpFloat);
    f = env->GetFieldID(cInfoKlass, "applicationScale", "F");
    Util::CheckErrorAndLog(env, "ToJavaCompatibilityInfo", "GetFieldID: applicationScale %d", __LINE__);

    env->SetFloatField(jcInfo, f, tmpFloat);
    Util::CheckErrorAndLog(env, "ToJavaCompatibilityInfo", "SetFloatField: applicationScale %d", __LINE__);

    cInfo->GetApplicationInvertedScale(&tmpFloat);
    f = env->GetFieldID(cInfoKlass, "applicationInvertedScale", "F");
    Util::CheckErrorAndLog(env, "ToJavaCompatibilityInfo", "GetFieldID: applicationInvertedScale %d", __LINE__);

    env->SetFloatField(jcInfo, f, tmpFloat);
    Util::CheckErrorAndLog(env, "ToJavaCompatibilityInfo", "SetFloatField: applicationInvertedScale %d", __LINE__);

    Boolean isThemeable;
    cInfo->GetIsThemeable(&isThemeable);
    f = env->GetFieldID(cInfoKlass, "isThemeable", "Z");
    Util::CheckErrorAndLog(env, "ToJavaCompatibilityInfo", "GetFieldID: isThemeable %d", __LINE__);

    env->SetBooleanField(jcInfo, f, isThemeable);
    Util::CheckErrorAndLog(env, "ToJavaCompatibilityInfo", "SetBooleanField: isThemeable %d", __LINE__);

    env->DeleteLocalRef(cInfoKlass);
    return jcInfo;
}

jint Util::GetJavaIntegerField(
    /* [in] */ JNIEnv* env,
    /* [in] */ jclass klass, jobject jobj, const char* fieldName, jint defaultInt, const char* tag)
{
    if (env == NULL || klass == NULL || jobj == NULL || fieldName == NULL){
        LOGGERE("%s: GetJavaIntegerField() invalid param while get field:%s  : %d!\n", tag, fieldName, __LINE__);
    }
    jfieldID integerField = env->GetFieldID(klass, fieldName, "Ljava/lang/Integer;");
    CheckErrorAndLog(env, "%s: Fail get integer fieldid: %s : %d!\n", tag, fieldName, __LINE__);
    jobject jIntegerValue = env->GetObjectField(jobj ,integerField);

    if (jIntegerValue == NULL){
        LOGGERW("Util::GetJavaIntegerField integer field is null, name:%s", fieldName);
        return defaultInt;
    }

    CheckErrorAndLog(env, "%s Fail get integer field : %s : %d!\n", tag, fieldName, __LINE__);

    jclass jintegerCls = env->FindClass("java/lang/Integer");
    CheckErrorAndLog(env, "%s,GetJavaIntegerField(): FindClass: Integer failed for field: %s, Line:%d!\n", tag, fieldName, __LINE__);
    jmethodID toIntMid = env->GetMethodID(jintegerCls, "intValue", "()I");
    CheckErrorAndLog(env, "%s, GetJavaIntegerField(): FindMethod: toValue failed for field: %s, Line: %d!\n", tag, fieldName, __LINE__);
    jint value = env->CallIntMethod(jIntegerValue, toIntMid);
    CheckErrorAndLog(env, "%s, GetJavaIntegerField: call method toValue failed for field: %s, Line:%d!\n", tag,fieldName,  __LINE__);
    env->DeleteLocalRef(jintegerCls);
    env->DeleteLocalRef(jIntegerValue);
    return value;
}


jfloat Util::GetJavafloatField(
    /* [in] */ JNIEnv* env,
    /* [in] */ jclass klass, jobject jobj, const char* fieldName, const char* tag)
{
    if (env == NULL || klass == NULL || jobj == NULL || fieldName == NULL){
        LOGGERE(TAG, "%s: GetJavafloatField() invalid param while get field:%s  : %d!\n", tag, fieldName, __LINE__);
        return 0.0f;
    }

    jfieldID field = env->GetFieldID(klass, fieldName, "F");
    CheckErrorAndLog(env, "%s: Fail get float field id:%s  : %d!\n", tag, fieldName, __LINE__);
    jfloat value = env->GetFloatField(jobj, field);
    CheckErrorAndLog(env, "%s: Fail get float field: %s : %d!\n", tag, fieldName, __LINE__);
    return value;
}

jdouble Util::GetJavadoubleField(
    /* [in] */ JNIEnv* env,
    /* [in] */ jclass klass, jobject jobj, const char* fieldName, const char* tag)
{
    if (env == NULL || klass == NULL || jobj == NULL || fieldName == NULL){
        LOGGERE(TAG, "%s: GetJavadoubleField() invalid param while get field:%s  : %d!\n", tag, fieldName, __LINE__);
        return 0.0;
    }

    jfieldID field = env->GetFieldID(klass, fieldName, "D");
    CheckErrorAndLog(env, "%s: Fail get double field id:%s  : %d!\n", tag, fieldName, __LINE__);
    jdouble value = env->GetDoubleField(jobj, field);
    CheckErrorAndLog(env, "%s: Fail get double field: %s : %d!\n", tag, fieldName, __LINE__);
    return value;
}

jshort Util::GetJavaShortField(
    /* [in] */ JNIEnv* env,
    /* [in] */ jclass klass, jobject jobj, const char* fieldName, const char* tag)
{
    if (env == NULL || klass == NULL || jobj == NULL || fieldName == NULL){
        LOGGERE(TAG, "%s: GetJavabyteField() invalid param while get field:%s  : %d!\n", tag, fieldName, __LINE__);
        return 0;
    }

    jfieldID fieldID = env->GetFieldID(klass, fieldName, "S");
    CheckErrorAndLog(env, "%s: Fail get byte field id:%s  : %d!\n", tag, fieldName, __LINE__);
    jshort jvalue = env->GetShortField(jobj, fieldID);
    CheckErrorAndLog(env, "%s: Fail get byte field: %s : %d!\n", tag, fieldName, __LINE__);
    return jvalue;
}

jchar Util::GetJavaCharField(
    /* [in] */ JNIEnv* env,
    /* [in] */ jclass klass, jobject jobj, const char* fieldName, const char* tag)
{
    if (env == NULL || klass == NULL || jobj == NULL || fieldName == NULL){
        LOGGERE(TAG, "%s: GetJavabyteField() invalid param while get field:%s  : %d!\n", tag, fieldName, __LINE__);
        return 0;
    }

    jfieldID fieldID = env->GetFieldID(klass, fieldName, "C");
    CheckErrorAndLog(env, "%s: Fail get byte field id:%s  : %d!\n", tag, fieldName, __LINE__);
    jchar jvalue = env->GetCharField(jobj, fieldID);
    CheckErrorAndLog(env, "%s: Fail get byte field: %s : %d!\n", tag, fieldName, __LINE__);
    return jvalue;
}

jint Util::GetJavaIntField(
    /* [in] */ JNIEnv* env,
    /* [in] */ jclass klass, jobject jobj, const char* fieldName, const char* tag)
{
    if (env == NULL || klass == NULL || jobj == NULL || fieldName == NULL){
        LOGGERE("%s: GetJavaIntField() invalid param while get field:%s  : %d!\n", tag, fieldName, __LINE__);
    }
    jfieldID intField = env->GetFieldID(klass, fieldName, "I");
    CheckErrorAndLog(env, "%s: Fail get int field id:%s  : %d!\n", tag, fieldName, __LINE__);
    jint intValue = env->GetIntField(jobj, intField);
    CheckErrorAndLog(env, "%s: Fail get int field: %s : %d!\n", tag, fieldName, __LINE__);
    return intValue;
}

jlong Util::GetJavalongField(
    /* [in] */ JNIEnv* env,
    /* [in] */ jobject jobj, const char* fieldName, const char* tag)
{
    if (env == NULL || jobj == NULL || fieldName == NULL){
        LOGGERE("%s: GetJavalongField() invalid param while get field:%s  : %d!\n", tag, fieldName, __LINE__);
    }

    jclass klass = env->GetObjectClass(jobj);
    CheckErrorAndLog(env, "%s: Fail get object class. %s%d!\n", tag, "", __LINE__);
    jfieldID field = env->GetFieldID(klass, fieldName, "J");
    CheckErrorAndLog(env, "%s: Fail get long field id:%s  : %d!\n", tag, fieldName, __LINE__);
    jlong value = env->GetLongField(jobj, field);
    CheckErrorAndLog(env, "%s: Fail get int field: %s : %d!\n", tag, fieldName, __LINE__);
    env->DeleteLocalRef(klass);
    return value;
}

/**get byte field of java object*/
jbyte Util::GetJavabyteField(
    /* [in] */ JNIEnv* env,
    /* [in] */ jclass klass, jobject jobj, const char* fieldName, const char* tag)
{
    if (env == NULL || klass == NULL || jobj == NULL || fieldName == NULL){
        LOGGERE("%s: GetJavabyteField() invalid param while get field:%s  : %d!\n", tag, fieldName, __LINE__);
    }
    jfieldID byteField = env->GetFieldID(klass, fieldName, "B");
    CheckErrorAndLog(env, "%s: Fail get byte field id:%s  : %d!\n", tag, fieldName, __LINE__);
    jbyte byteValue = env->GetByteField(jobj, byteField);
    CheckErrorAndLog(env, "%s: Fail get byte field: %s : %d!\n", tag, fieldName, __LINE__);
    return byteValue;
}

jboolean Util::GetJavaBoolField(
    /* [in] */ JNIEnv* env,
    /* [in] */ jobject jobj, const char* fieldName, const char* tag)
{
    if (env == NULL || jobj == NULL || fieldName == NULL){
        LOGGERE("%s: GetJavaBoolField() invalid param while get field:%s  : %d!\n", tag, fieldName, __LINE__);
        return JNI_FALSE;
    }

    jclass klass = env->GetObjectClass(jobj);
    CheckErrorAndLog(env, "%s: Fail get object class. %s%d!\n", tag, "", __LINE__);
    jfieldID field = env->GetFieldID(klass, fieldName, "Z");
    CheckErrorAndLog(env, "%s: Fail get boolean field id:%s  : %d!\n", tag, fieldName, __LINE__);
    jboolean value = env->GetBooleanField(jobj, field);
    CheckErrorAndLog(env, "%s: Fail get boolean field: %s : %d!\n", tag, fieldName, __LINE__);
    env->DeleteLocalRef(klass);
    return value;
}


String Util::GetJavaStringField(
    /* [in] */ JNIEnv* env,
    /* [in] */ jclass klass, jobject jobj, const char* fieldName, const char* tag)
{
    if (env == NULL || klass == NULL || jobj == NULL || fieldName == NULL){
        LOGGERE("%s: GetJavaStringField() invalid param while get field:%s  : %d!\n", tag, fieldName, __LINE__);
    }

    jfieldID stringField = env->GetFieldID(klass, fieldName, "Ljava/lang/String;");
    CheckErrorAndLog(env, "%s: Fail get string field id: %s : %d!\n", tag, fieldName, __LINE__);
    jstring stringValue = (jstring)env->GetObjectField(jobj, stringField);
    CheckErrorAndLog(env, "%s: Fail get string field: %s : %d!\n", tag, fieldName, __LINE__);
    String s = GetElString(env, stringValue);
    env->DeleteLocalRef(stringValue);
    return s;
}

bool Util::SetJavaIntegerField(
    /* [in] */ JNIEnv* env,
    /* [in] */ jclass klass,
    /* [in] */ jobject jobj,
    /* [in] */ Int32 intvalue,
    /* [in] */ const char* fieldName,
    /* [in] */ const char* tag)
{
    if (env == NULL || klass == NULL || jobj == NULL || fieldName == NULL){
        LOGGERE("%s: SetJavaIntegerField() invalid param while set field:%s  : %d!\n", tag, fieldName, __LINE__);
    }
    jclass jintegerCls = env->FindClass("java/lang/Integer");
    CheckErrorAndLog(env, "%s,SetJavaIntegerField(): failed FindClass: Integer : %s%d!\n", tag, "", __LINE__);
    jmethodID mid = env->GetStaticMethodID(jintegerCls, "valueOf", "(I)Ljava/lang/Integer;");
    CheckErrorAndLog(env, "%s,SetJavaIntegerField(): failed get statis methond valueof(): %s%d!\n", tag, "", __LINE__);
    jobject jinteger = env->CallStaticObjectMethod(jintegerCls, mid, intvalue);
    CheckErrorAndLog(env, "%s,SetJavaIntegerField(): failed call static method valueOf(): %s%d!\n", tag, "", __LINE__);

    jfieldID integerField = env->GetFieldID(klass, fieldName, "Ljava/lang/Integer;");
    CheckErrorAndLog(env, "%s: Fail get integer fieldid: %s : %d!\n", tag, fieldName, __LINE__);
    env->SetObjectField(jobj, integerField, jinteger);
    CheckErrorAndLog(env, "%s Fail get integer field : %s : %d!\n", tag, fieldName, __LINE__);
    env->DeleteLocalRef(jintegerCls);
    env->DeleteLocalRef(jinteger);
    return true;
}

bool Util::SetJavaIntField(
    /* [in] */ JNIEnv* env,
    /* [in] */ jclass klass,
    /* [in] */ jobject jobj,
    /* [in] */ Int32 intvalue,
    /* [in] */ const char* fieldName,
    /* [in] */ const char* tag)
{
    if (env == NULL || klass == NULL || jobj == NULL || fieldName == NULL){
        LOGGERE("%s: SetJavaIntField() invalid param while set field:%s  : %d!\n", tag, fieldName, __LINE__);
    }
    jfieldID intField = env->GetFieldID(klass, fieldName, "I");
    CheckErrorAndLog(env, "%s: Fail get int field id:%s  : %d!\n", tag, fieldName, __LINE__);
    env->SetIntField(jobj, intField, intvalue);
    CheckErrorAndLog(env, "%s: Fail Set int field: %s : %d!\n", tag, fieldName, __LINE__);
    return true;
}

bool Util::SetJavabyteField(
    /* [in] */ JNIEnv* env,
    /* [in] */ jclass klass,
    /* [in] */ jobject jobj,
    /* [in] */ Byte bytevalue,
    /* [in] */ const char* fieldName,
    /* [in] */ const char* tag)
{
    if (env == NULL || klass == NULL || jobj == NULL || fieldName == NULL){
        LOGGERE("%s: SetJavabyteField() invalid param while set field:%s  : %d!\n", tag, fieldName, __LINE__);
    }
    jfieldID byteField = env->GetFieldID(klass, fieldName, "B");
    CheckErrorAndLog(env, "%s: Fail get byte field id:%s  : %d!\n", tag, fieldName, __LINE__);
    env->SetByteField(jobj, byteField, bytevalue);
    CheckErrorAndLog(env, "%s: Fail set byte field: %s : %d!\n", tag, fieldName, __LINE__);
    return true;
}

bool Util::SetJavalongField(
    /* [in] */ JNIEnv* env,
    /* [in] */ jclass klass,
    /* [in] */ jobject jobj,
    /* [in] */ Int64 longvalue,
    /* [in] */ const char* fieldName,
    /* [in] */ const char* tag)
{
    if (env == NULL || klass == NULL || jobj == NULL || fieldName == NULL){
        LOGGERE("%s: SetJavalongField() invalid param while set field:%s  : %d!\n", tag, fieldName, __LINE__);
        return false;
    }

    jfieldID intField = env->GetFieldID(klass, fieldName, "J");
    CheckErrorAndLog(env, "%s: Fail get long field id:%s  : %d!\n", tag, fieldName, __LINE__);
    env->SetLongField(jobj, intField, longvalue);
    CheckErrorAndLog(env, "%s: Fail Set long field: %s : %d!\n", tag, fieldName, __LINE__);
    return true;
}

bool Util::SetJavaBoolField(
    /* [in] */ JNIEnv* env,
    /* [in] */ jclass klass,
    /* [in] */ jobject jobj,
    /* [in] */ Boolean boolValue,
    /* [in] */ const char* fieldName,
    /* [in] */ const char* tag)
{
    if (env == NULL || klass == NULL || jobj == NULL || fieldName == NULL){
        LOGGERE("Util", "%s: SetJavaBoolField() invalid param while set field:%s  : %d!\n", tag, fieldName, __LINE__);
        return false;
    }
    jfieldID field = env->GetFieldID(klass, fieldName, "Z");
    CheckErrorAndLog(env, "%s: Fail get boolean field id:%s  : %d!\n", tag, fieldName, __LINE__);
    env->SetBooleanField(jobj, field, boolValue);
    CheckErrorAndLog(env, "%s: Fail set boolean field: %s : %d!\n", tag, fieldName, __LINE__);
    return true;
}

bool Util::SetJavafloatField(
    /* [in] */ JNIEnv* env,
    /* [in] */ jclass klass,
    /* [in] */ jobject jobj,
    /* [in] */ Float floatValue,
    /* [in] */ const char* fieldName,
    /* [in] */ const char* tag)
{
    if (env == NULL || klass == NULL || jobj == NULL || fieldName == NULL){
        LOGGERE("Util", "%s: SetJavafloatField() invalid param while set field:%s  : %d!\n", tag, fieldName, __LINE__);
        return false;
    }

    jfieldID field = env->GetFieldID(klass, fieldName, "F");
    CheckErrorAndLog(env, "%s: Fail get float field id:%s  : %d!\n", tag, fieldName, __LINE__);
    env->SetFloatField(jobj, field, floatValue);
    CheckErrorAndLog(env, "%s: Fail set float field: %s : %d!\n", tag, fieldName, __LINE__);
    return true;
}

bool Util::SetJavadoubleField(
    /* [in] */ JNIEnv* env,
    /* [in] */ jclass klass,
    /* [in] */ jobject jobj,
    /* [in] */ Double doubleValue,
    /* [in] */ const char* fieldName,
    /* [in] */ const char* tag)
{
    if (env == NULL || klass == NULL || jobj == NULL || fieldName == NULL){
        LOGGERE("Util", "%s: SetJavadoubleField() invalid param while set field:%s  : %d!\n", tag, fieldName, __LINE__);
        return false;
    }

    jfieldID field = env->GetFieldID(klass, fieldName, "D");
    CheckErrorAndLog(env, "%s: Fail get double field id:%s  : %d!\n", tag, fieldName, __LINE__);
    env->SetDoubleField(jobj, field, doubleValue);
    CheckErrorAndLog(env, "%s: Fail set double field: %s : %d!\n", tag, fieldName, __LINE__);
    return true;
}

bool Util::SetJavaStringField(
    /* [in] */ JNIEnv* env,
    /* [in] */ jclass klass,
    /* [in] */ jobject jobj,
    /* [in] */ const String& strValue,
    /* [in] */ const char* fieldName,
    /* [in] */ const char* tag)
{
    if (env == NULL || klass == NULL || jobj == NULL || fieldName == NULL){
        LOGGERE("Util", "%s: SetJavaStringField() invalid param while set field:%s  : %d!\n", tag, fieldName, __LINE__);
    }
    jstring jvalue = ToJavaString(env, strValue);
    jfieldID stringField = env->GetFieldID(klass, fieldName, "Ljava/lang/String;");
    CheckErrorAndLog(env, "%s: Fail get string field id: %s : %d!\n", tag, fieldName, __LINE__);
    env->SetObjectField(jobj, stringField, jvalue);
    CheckErrorAndLog(env, "%s: Fail Set string field: %s : %d!\n", tag, fieldName, __LINE__);
    env->DeleteLocalRef(jvalue);
    return true;
}

String Util::GetClassName(
    /* [in] */ JNIEnv* env,
    /* [in] */ jobject obj)
{
    jclass cls, clsobj;
    jmethodID getName;
    jstring jstr;
    String className;
    char *temp;
    jboolean isCopy;

    /* Get the class of the object */
    cls = env->GetObjectClass(obj);

    /* take that Class object and get it's Class object */
    clsobj = env->GetObjectClass(cls);

    /* get the Class.getName() methodid */
    getName = env->GetMethodID(clsobj, "getName", "()Ljava/lang/String;");

    /* Get the jstring name of the object, from the Class class. */
    jstr = (jstring)env->CallObjectMethod(cls, getName);

    /* Convert the name into a char* */
    temp = (char *)env->GetStringUTFChars(jstr, &isCopy);
    className = String(temp);
    env->ReleaseStringUTFChars(jstr, temp);

    env->DeleteLocalRef(cls);
    env->DeleteLocalRef(clsobj);
    env->DeleteLocalRef(jstr);
    return className;
}

jobject Util::ToJavaMessage(
    /* [in] */ JNIEnv* env,
    /* [in] */ IMessage* msg)
{
    if (env == NULL || msg == NULL) {
        LOGGERE("ToJavaMessage()", "Invalid arguments!");
        return NULL;
    }

    jobject jmsg = NULL;

    jclass msgKlass = env->FindClass("android/os/Message");
    Util::CheckErrorAndLog(env, "ToJavaMessage", "FindClass: Message line: %d", __LINE__);

    jmethodID m = env->GetStaticMethodID(msgKlass, "obtain", "()Landroid/os/Message;");
    Util::CheckErrorAndLog(env, "ToJavaMessage", "GetStaticMethodID: obtain : %d!\n", __LINE__);

    jmsg = env->CallStaticObjectMethod(msgKlass, m);
    Util::CheckErrorAndLog(env, "ToJavaMessage", "CallStaticObjectMethod: getByAddress(): %d!\n", __LINE__);

    jfieldID f = env->GetFieldID(msgKlass, "what", "I");
    Util::CheckErrorAndLog(env, "ToJavaMessage", "GetFieldID: what : %d!\n", __LINE__);

    Int32 tempInt;
    msg->GetWhat(&tempInt);
    env->SetIntField(jmsg, f, (jint)tempInt);
    Util::CheckErrorAndLog(env, "ToJavaMessage", "SetIntField what : %d!\n", __LINE__);

    f = env->GetFieldID(msgKlass, "arg1", "I");
    Util::CheckErrorAndLog(env, "ToJavaMessage", "GetFieldID: arg1 : %d!\n", __LINE__);

    msg->GetArg1(&tempInt);
    env->SetIntField(jmsg, f, (jint)tempInt);
    Util::CheckErrorAndLog(env, "ToJavaMessage", "SetIntField arg1 : %d!\n", __LINE__);

    f = env->GetFieldID(msgKlass, "arg2", "I");
    Util::CheckErrorAndLog(env, "ToJavaMessage", "GetFieldID: arg2 : %d!\n", __LINE__);

    msg->GetArg2(&tempInt);
    env->SetIntField(jmsg, f, (jint)tempInt);
    Util::CheckErrorAndLog(env, "ToJavaMessage", "SetIntField arg2 : %d!\n", __LINE__);

    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    if (obj != NULL) {
        f = env->GetFieldID(msgKlass, "obj", "Ljava/lang/Object;");
        Util::CheckErrorAndLog(env, "ToJavaMessage", "GetFieldID: obj : %d!\n", __LINE__);

        if (IWifiConfiguration::Probe(obj) != NULL) {
            AutoPtr<IWifiConfiguration> config = IWifiConfiguration::Probe(obj);

            jobject jconfig = Util::ToJavaWifiConfiguration(env, config);
            env->SetObjectField(jmsg, f, jconfig);
            Util::CheckErrorAndLog(env, "ToJavaMessage", "GetObjectField obj : %d!\n", __LINE__);

            env->DeleteLocalRef(jconfig);
        }
        else if (IWifiP2pDeviceList::Probe(obj) != NULL) {
            AutoPtr<IWifiP2pDeviceList> info = IWifiP2pDeviceList::Probe(obj);

            jobject jinfo = Util::ToJavaWifiP2pDeviceList(env, info);
            env->SetObjectField(jmsg, f, jinfo);
            Util::CheckErrorAndLog(env, "ToJavaMessage", "GetObjectField obj : %d!\n", __LINE__);

            env->DeleteLocalRef(jinfo);
        }
        else if (IWifiP2pGroup::Probe(obj) != NULL) {
            AutoPtr<IWifiP2pGroup> group = IWifiP2pGroup::Probe(obj);

            jobject jgroup = Util::ToJavaWifiP2pGroup(env, group);
            env->SetObjectField(jmsg, f, jgroup);
            Util::CheckErrorAndLog(env, "ToJavaMessage", "GetObjectField obj : %d!\n", __LINE__);

            env->DeleteLocalRef(jgroup);
        }
        else {
            AutoPtr<IObject> object = IObject::Probe(obj);
            ClassID clsid;
            object->GetClassID(&clsid);
            DUMP_CLSID(clsid, "ToJavaMessage");
            LOGGERE("ToJavaMessage()", "Unsupported obj line: %d", __LINE__);
        }
    }

    f = env->GetFieldID(msgKlass, "replyTo", "Landroid/os/Messenger;");
    Util::CheckErrorAndLog(env, "ToJavaMessage", "GetFieldID: replyTo : %d!\n", __LINE__);

    AutoPtr<IMessenger> replyTo;
    msg->GetReplyTo((IMessenger**)&replyTo);
    if (replyTo != NULL) {
        LOGGERE("ToJavaMessage()", "replyTo  not NULL line: %d", __LINE__);
        jobject jreplyTo = Util::ToJavaMessenger(env, replyTo);

        env->SetObjectField(jmsg, f, jreplyTo);
        Util::CheckErrorAndLog(env, "ToJavaMessage", "SetObjectField: %d!\n", __LINE__);
    }

    Int64 when;
    msg->GetWhen(&when);
    Util::SetJavalongField(env, msgKlass, jmsg, when, "when", "ToJavaMessage");

    AutoPtr<IBundle> data;
    msg->GetData((IBundle**)&data);
    if (data != NULL) {
        f = env->GetFieldID(msgKlass, "data", "Landroid/os/Bundle;");
        Util::CheckErrorAndLog(env, "ToJavaMessage", "GetFieldID: data : %d!\n", __LINE__);

        jobject jdata = Util::ToJavaBundle(env, data);
        env->SetObjectField(jmsg, f, jdata);
        Util::CheckErrorAndLog(env, "ToJavaMessage", "SetObjectField data : %d!\n", __LINE__);
        env->DeleteLocalRef(jdata);
    }

    f = env->GetFieldID(msgKlass, "sendingUid", "I");
    Util::CheckErrorAndLog(env, "ToJavaMessage", "GetFieldID: sendingUid : %d!\n", __LINE__);

    msg->GetSendingUid(&tempInt);
    env->SetIntField(jmsg, f, (jint)tempInt);
    Util::CheckErrorAndLog(env, "ToJavaMessage", "SetIntField sendingUid : %d!\n", __LINE__);

    env->DeleteLocalRef(msgKlass);
    return jmsg;
}

jobject Util::ToJavaIntentReceiver(
    /* [in] */ JNIEnv* env,
    /* [in] */ IIntentReceiver* receiver)
{
    if (env == NULL || receiver == NULL) {
        LOGGERE("ToJavaIntentReceiver()", "Invalid arguments!");
        return NULL;
    }

    jclass revrKlass = env->FindClass("android/content/ElIIntentReceiverProxy");
    Util::CheckErrorAndLog(env, "ToJavaIntentReceiver", "FindClass: ElIIntentReceiverProxy line: %d", __LINE__);

    jmethodID m = env->GetMethodID(revrKlass, "<init>", "(J)V");
    Util::CheckErrorAndLog(env, "ToJavaIntentReceiver", "GetMethodID: ElIIntentReceiverProxy line: %d", __LINE__);

    jobject jreceiver = env->NewObject(revrKlass, m, (jlong)receiver);
    Util::CheckErrorAndLog(env, "ToJavaIntentReceiver", "NewObject: ElIIntentReceiverProxy line: %d", __LINE__);
    receiver->AddRef();

    env->DeleteLocalRef(revrKlass);
    return jreceiver;
}

jobject Util::ToJavaLocation(
    /* [in] */ JNIEnv* env,
    /* [in] */ ILocation* location)
{
    if (env == NULL || location == NULL) {
        LOGGERE("ToJavaLocation()", "Invalid arguments!");
        return NULL;
    }

    jclass locationKlass = env->FindClass("android/location/Location");
    CheckErrorAndLog(env, "ToJavaLocation", "Error FindClass: Location : %d!\n", __LINE__);

    jmethodID m = env->GetMethodID(locationKlass, "<init>", "(Ljava/lang/String;)V");
    CheckErrorAndLog(env, "ToJavaLocation", "GetMethodID: Location() : %d!\n", __LINE__);

    String mProvider("");
    Int64 mTime = 0;
    Int64 mElapsedRealtimeNanos = 0;
    Double mLatitude = 0.0;
    Double mLongitude = 0.0;
    Boolean mHasAltitude = FALSE;
    Double mAltitude = 0.0f;
    Boolean mHasSpeed = FALSE;
    Float mSpeed = 0.0f;
    Boolean mHasBearing = FALSE;
    Float mBearing = 0.0f;
    Boolean mHasAccuracy = FALSE;
    Float mAccuracy = 0.0f;
    AutoPtr<IBundle> mExtras;
    Boolean mIsFromMockProvider = FALSE;

    location->GetProvider(&mProvider);
    location->GetTime(&mTime);
    location->GetElapsedRealtimeNanos(&mElapsedRealtimeNanos);
    location->GetLatitude(&mLatitude);
    location->GetLongitude(&mLongitude);
    location->HasAltitude(&mHasAltitude);
    location->GetAltitude(&mAltitude);
    location->HasSpeed(&mHasSpeed);
    location->GetSpeed(&mSpeed);
    location->HasBearing(&mHasBearing);
    location->GetBearing(&mBearing);
    location->HasAccuracy(&mHasAccuracy);
    location->GetAccuracy(&mAccuracy);
    location->GetExtras((IBundle**)&mExtras);
    location->IsFromMockProvider(&mIsFromMockProvider);

    jstring jprovider = ToJavaString(env, mProvider);

    jobject jlocation = env->NewObject(locationKlass, m, jprovider);
    env->DeleteLocalRef(jprovider);

    SetJavalongField(env, locationKlass, jlocation, mTime, "mTime", "ToJavaLocation");
    SetJavalongField(env, locationKlass, jlocation, mElapsedRealtimeNanos, "mElapsedRealtimeNanos", "ToJavaLocation");
    SetJavadoubleField(env, locationKlass, jlocation, mLatitude, "mLatitude", "ToJavaLocation");
    SetJavadoubleField(env, locationKlass, jlocation, mLongitude, "mLongitude", "ToJavaLocation");
    SetJavaBoolField(env, locationKlass, jlocation, mHasAltitude, "mHasAltitude", "ToJavaLocation");
    SetJavadoubleField(env, locationKlass, jlocation, mAltitude, "mAltitude", "ToJavaLocation");
    SetJavaBoolField(env, locationKlass, jlocation, mHasSpeed, "mHasSpeed", "ToJavaLocation");
    SetJavafloatField(env, locationKlass, jlocation, mSpeed, "mSpeed", "ToJavaLocation");
    SetJavaBoolField(env, locationKlass, jlocation, mHasBearing, "mHasBearing", "ToJavaLocation");
    SetJavafloatField(env, locationKlass, jlocation, mBearing, "mBearing", "ToJavaLocation");
    SetJavaBoolField(env, locationKlass, jlocation, mHasAccuracy, "mHasAccuracy", "ToJavaLocation");
    SetJavafloatField(env, locationKlass, jlocation, mAccuracy, "mAccuracy", "ToJavaLocation");
    SetJavaBoolField(env, locationKlass, jlocation, mIsFromMockProvider, "mIsFromMockProvider", "ToJavaLocation");

    jobject jextras = ToJavaBundle(env, mExtras);
    if (jextras != NULL) {
        jfieldID f = env->GetFieldID(locationKlass, "mExtras", "Landroid/os/Bundle;");
        CheckErrorAndLog(env, "ToJavaLocation", "Fail GetFieldID: mExtras : %d!\n", __LINE__);

        env->SetObjectField(jlocation, f, jextras);
        CheckErrorAndLog(env, "ToJavaLocation", "Fail SetObjectField: jextras : %d!\n", __LINE__);
        env->DeleteLocalRef(jextras);
    }
    else {
        LOGGERD("Util", "ToJavaLocation Error: jextras is NULL!");
    }

    env->DeleteLocalRef(locationKlass);
    return jlocation;
}

jobject Util::ToJavaInputBinding(
    /* [in] */ JNIEnv* env,
    /* [in] */ IInputBinding* binding)
{
    if (env == NULL || binding == NULL) {
        LOGGERE("ToJavaInputBinding()", "Invalid arguments!");
        return NULL;
    }

    jclass ibKlass = env->FindClass("android/view/inputmethod/InputBinding");
    Util::CheckErrorAndLog(env, "ToJavaIntentReceiver", "FindClass: InputBinding line: %d", __LINE__);

    jmethodID m = env->GetMethodID(ibKlass, "<init>", "(Landroid/view/inputmethod/InputConnection;Landroid/os/IBinder;II)V");
    Util::CheckErrorAndLog(env, "ToJavaIntentReceiver", "GetMethodID: InputBinding line: %d", __LINE__);

    Int32 uid;
    binding->GetUid(&uid);

    Int32 pid;
    binding->GetPid(&pid);

    AutoPtr<IBinder> connToken;
    binding->GetConnectionToken((IBinder**)&connToken);
    jobject jconnToken = NULL;
    if (connToken != NULL) {
        AutoPtr<IObject> object = IObject::Probe(connToken);

        ClassID clsid;
        object->GetClassID(&clsid);
        assert(0);
        if (/*clsid == ECLSID_CInputContextNative ||*/
            clsid == Elastos::Droid::View::InputMethod::ECLSID_CControlledInputConnectionWrapper) {
            jclass conKlass = env->FindClass("com/android/internal/view/ElIInputContextProxy");
            Util::CheckErrorAndLog(env, "ToJavaInputBinding", "FindClass: ElIInputContextProxy line: %d", __LINE__);

            jmethodID m = env->GetMethodID(conKlass, "<init>", "(J)V");
            Util::CheckErrorAndLog(env, "ToJavaInputBinding", "GetMethodID: ElIInputContextProxy line: %d", __LINE__);

            jconnToken = env->NewObject(conKlass, m, (jlong)IIInputContext::Probe(connToken));
            Util::CheckErrorAndLog(env, "ToJavaInputBinding", "NewObject: ElIInputContextProxy line: %d", __LINE__);
            connToken->AddRef();

            env->DeleteLocalRef(conKlass);
        }
        else {
            LOGGERE("ToJavaInputBinding()", "connToken is Unknown!");
            DUMP_CLSID(clsid, "Eric");
        }
    }

    jobject jbinding = env->NewObject(ibKlass, m, NULL, jconnToken, (jint)uid, (jint)pid);
    Util::CheckErrorAndLog(env, "ToJavaIntentReceiver", "NewObject: InputBinding line: %d", __LINE__);
    env->DeleteLocalRef(ibKlass);
    env->DeleteLocalRef(jconnToken);
    return jbinding;
}

jobject Util::ToJavaEditorInfo(
    /* [in] */ JNIEnv* env,
    /* [in] */ IEditorInfo* info)
{
    if (env == NULL || info == NULL) {
        LOGGERE("ToJavaEditorInfo()", "Invalid arguments!");
        return NULL;
    }

    jclass einfoKlass = env->FindClass("android/view/inputmethod/EditorInfo");
    Util::CheckErrorAndLog(env, "ToJavaEditorInfo", "FindClass: EditorInfo line: %d", __LINE__);

    jmethodID m = env->GetMethodID(einfoKlass, "<init>", "()V");
    Util::CheckErrorAndLog(env, "ToJavaEditorInfo", "GetMethodID: EditorInfo line: %d", __LINE__);

    jobject jinfo = env->NewObject(einfoKlass, m);
    Util::CheckErrorAndLog(env, "ToJavaInputBinding", "NewObject: EditorInfo line: %d", __LINE__);

    Int32 tempInt = 0;
    info->GetInputType(&tempInt);
    Util::SetJavaIntField(env, einfoKlass, jinfo, tempInt, "inputType", "ToJavaEditorInfo");

    info->GetImeOptions(&tempInt);
    Util::SetJavaIntField(env, einfoKlass, jinfo, tempInt, "imeOptions", "ToJavaEditorInfo");

    String privateImeOptions;
    info->GetPrivateImeOptions(&privateImeOptions);
    Util::SetJavaStringField(env, einfoKlass, jinfo, privateImeOptions, "privateImeOptions", "ToJavaEditorInfo");

    AutoPtr<ICharSequence> actionLabel;
    info->GetActionLabel((ICharSequence**)&actionLabel);
    if (actionLabel != NULL) {
        String sactionLabel;
        actionLabel->ToString(&sactionLabel);
        jstring jactionLabel = Util::ToJavaString(env, sactionLabel);

        jfieldID f = env->GetFieldID(einfoKlass, "actionLabel", "Ljava/lang/CharSequence;");
        Util::CheckErrorAndLog(env, "ToJavaEditorInfo", "GetFieldID: CharSequence %d", __LINE__);

        env->SetObjectField(jinfo, f, jactionLabel);
        Util::CheckErrorAndLog(env, "ToJavaEditorInfo", "SetObjectField: jactionLabel %d", __LINE__);
        env->DeleteLocalRef(jactionLabel);
    }

    info->GetActionId(&tempInt);
    Util::SetJavaIntField(env, einfoKlass, jinfo, tempInt, "actionId", "ToJavaEditorInfo");

    info->GetInitialSelStart(&tempInt);
    Util::SetJavaIntField(env, einfoKlass, jinfo, tempInt, "initialSelStart", "ToJavaEditorInfo");

    info->GetInitialSelEnd(&tempInt);
    Util::SetJavaIntField(env, einfoKlass, jinfo, tempInt, "initialSelEnd", "ToJavaEditorInfo");

    info->GetInitialCapsMode(&tempInt);
    Util::SetJavaIntField(env, einfoKlass, jinfo, tempInt, "initialCapsMode", "ToJavaEditorInfo");

    AutoPtr<ICharSequence> hintText;
    info->GetHintText((ICharSequence**)&hintText);
    if (hintText != NULL) {
        String shintText;
        hintText->ToString(&shintText);
        jstring jhintText = Util::ToJavaString(env, shintText);

        jfieldID f = env->GetFieldID(einfoKlass, "hintText", "Ljava/lang/CharSequence;");
        Util::CheckErrorAndLog(env, "ToJavaEditorInfo", "GetFieldID: CharSequence %d", __LINE__);

        env->SetObjectField(jinfo, f, jhintText);
        Util::CheckErrorAndLog(env, "ToJavaEditorInfo", "SetObjectField: jhintText %d", __LINE__);
        env->DeleteLocalRef(jhintText);
    }

    AutoPtr<ICharSequence> label;
    info->GetLabel((ICharSequence**)&label);
    if (label != NULL) {
        String slabel;
        label->ToString(&slabel);
        jstring jlabel = Util::ToJavaString(env, slabel);

        jfieldID f = env->GetFieldID(einfoKlass, "label", "Ljava/lang/CharSequence;");
        Util::CheckErrorAndLog(env, "ToJavaEditorInfo", "GetFieldID: CharSequence %d", __LINE__);

        env->SetObjectField(jinfo, f, jlabel);
        Util::CheckErrorAndLog(env, "ToJavaEditorInfo", "SetObjectField: label %d", __LINE__);
        env->DeleteLocalRef(jlabel);
    }

    String packageName;
    info->GetPackageName(&packageName);
    Util::SetJavaStringField(env, einfoKlass, jinfo, packageName, "packageName", "ToJavaEditorInfo");

    info->GetFieldId(&tempInt);
    Util::SetJavaIntField(env, einfoKlass, jinfo, tempInt, "fieldId", "ToJavaEditorInfo");

    String fieldName;
    info->GetFieldName(&fieldName);
    Util::SetJavaStringField(env, einfoKlass, jinfo, fieldName, "fieldName", "ToJavaEditorInfo");

    AutoPtr<IBundle> extras;
    info->GetExtras((IBundle**)&extras);
    if (extras != NULL) {
        jobject jextras = Util::ToJavaBundle(env, extras);
        if (jextras != NULL) {
            jfieldID f = env->GetFieldID(einfoKlass, "extras", "Landroid/os/Bundle;");
            CheckErrorAndLog(env, "ToJavaEditorInfo", "GetFieldID: extras : %d!\n", __LINE__);

            env->SetObjectField(jinfo, f, jextras);
            CheckErrorAndLog(env, "ToJavaEditorInfo", "SetObjectField: jextras : %d!\n", __LINE__);
            env->DeleteLocalRef(jextras);
        }
        else {
            LOGGERE("ToJavaEditorInfo()", "jextras is NULL!");
        }
    }

    env->DeleteLocalRef(einfoKlass);
    return jinfo;
}

jobject Util::ToJavaFileDescriptor(
    /* [in] */ JNIEnv* env,
    /* [in] */ IFileDescriptor* fileDescriptor)
{
    if (env == NULL || fileDescriptor == NULL) {
        LOGGERE("ToJavaFileDescriptor", "Invalid arguments!");
        return NULL;
    }

    jobject jfileDescriptor = NULL;

    jclass fileDescriptorKlass = env->FindClass("java/io/FileDescriptor");
    Util::CheckErrorAndLog(env, "ToJavaFileDescriptor", "Fail FindClass: FileDescriptor %d", __LINE__);

    jmethodID m = env->GetMethodID(fileDescriptorKlass, "<init>", "()V");
    Util::CheckErrorAndLog(env, "ToJavaFileDescriptor", "Fail GetMethodID: fileDescriptorKlass %d", __LINE__);

    jfileDescriptor = env->NewObject(fileDescriptorKlass, m);
    Util::CheckErrorAndLog(env, "ToJavaFileDescriptor", "Fail NewObject: fileDescriptorKlass %d", __LINE__);

    Int32 origfd = -1;
    fileDescriptor->GetDescriptor(&origfd);

    int fd = dup(origfd);
    if (fd < 0) {
        LOGGERE("ToJavaFileDescriptor", "dup fail!");
        fd = origfd;
    }

    m = env->GetMethodID(fileDescriptorKlass, "setInt$", "(I)V");
    Util::CheckErrorAndLog(env, "ToJavaFileDescriptor", "Fail GetMethodID: fileDescriptorKlass %d", __LINE__);

    env->CallObjectMethod(jfileDescriptor, m, (jint)fd);
    Util::CheckErrorAndLog(env, "ToJavaFileDescriptor", "Fail CallObjectMethod: setInt$  %d", __LINE__);

    env->DeleteLocalRef(fileDescriptorKlass);

    return jfileDescriptor;
}

jobject Util::ToJavaParcelFileDescriptor(
    /* [in] */ JNIEnv* env,
    /* [in] */ IParcelFileDescriptor* pfd)
{
    if (env == NULL || pfd == NULL) {
        LOGGERE("ToJavaParcelFileDescriptor", "Invalid arguments!");
        return NULL;
    }

    AutoPtr<IFileDescriptor> fileDescriptor;
    pfd->GetFileDescriptor((IFileDescriptor**)&fileDescriptor);

    jobject jfileDescriptor = ToJavaFileDescriptor(env, fileDescriptor);

    jobject jpfd = NULL;

    jclass pfdKlass = env->FindClass("android/os/ParcelFileDescriptor");
    Util::CheckErrorAndLog(env, "ToJavaParcelFileDescriptor", "Fail FindClass: ParcelFileDescriptor %d", __LINE__);

    jmethodID m = env->GetMethodID(pfdKlass, "<init>", "(Ljava/io/FileDescriptor;)V");
    Util::CheckErrorAndLog(env, "ToJavaParcelFileDescriptor", "Fail GetMethodID: pfdKlass %d", __LINE__);

    jpfd = env->NewObject(pfdKlass, m, jfileDescriptor);
    Util::CheckErrorAndLog(env, "ToJavaParcelFileDescriptor", "Fail NewObject: pfdKlass %d", __LINE__);

    pfd->Close();

    env->DeleteLocalRef(jfileDescriptor);
    env->DeleteLocalRef(pfdKlass);
    return jpfd;
}

jobject Util::ToJavaNetworkInfo(
    /* [in] */ JNIEnv* env,
    /* [in] */ INetworkInfo* info)
{
    if (env == NULL || info == NULL) {
        LOGGERE("ToJavaNetworkInfo", "Invalid argumenet!");
        return NULL;
    }

    jclass netInfoKlass = env->FindClass("android/net/NetworkInfo");
    CheckErrorAndLog(env, "ToJavaNetworkInfo", "FindClass: NetworkInfo : %d!\n %d", __LINE__);

    jmethodID m = env->GetMethodID(netInfoKlass, "<init>", "(IILjava/lang/String;Ljava/lang/String;)V");
    CheckErrorAndLog(env, "ToJavaNetworkInfo", "GetMethodID: NetworkInfo : %d!\n", __LINE__);

    Int32 networktype;
    info->GetType(&networktype);
    Int32 subtype;
    info->GetSubtype(&subtype);
    String typeName;
    info->GetTypeName(&typeName);
    jstring jtypeName = Util::ToJavaString(env, typeName);
    String subtypeName;
    info->GetSubtypeName(&subtypeName);
    jstring jsubtypeName = Util::ToJavaString(env, subtypeName);

    jobject jinfo = env->NewObject(netInfoKlass, m, (jint)networktype, (jint)subtype, jtypeName, jsubtypeName);
    CheckErrorAndLog(env, "ToJavaNetworkInfo", "NewObject: NetworkInfo : %d!\n", __LINE__);

    env->DeleteLocalRef(jtypeName);
    env->DeleteLocalRef(jsubtypeName);

    NetworkInfoState state;
    info->GetState(&state);

    jclass stateKlass = env->FindClass("android/net/NetworkInfo$State");
    CheckErrorAndLog(env, "ToJavaNetworkInfo", "FindClass: NetworkInfo$State : %d!\n", __LINE__);

    jfieldID f = NULL;

    switch(state) {
        case Elastos::Droid::Net::NetworkInfoState_CONNECTING: {
            f = env->GetStaticFieldID(stateKlass, "CONNECTING", "Landroid/net/NetworkInfo$State;");
            break;
        }
        case Elastos::Droid::Net::NetworkInfoState_CONNECTED: {
            f = env->GetStaticFieldID(stateKlass, "CONNECTED", "Landroid/net/NetworkInfo$State;");
            break;
        }
        case Elastos::Droid::Net::NetworkInfoState_SUSPENDED: {
            f = env->GetStaticFieldID(stateKlass, "SUSPENDED", "Landroid/net/NetworkInfo$State;");
            break;
        }
        case Elastos::Droid::Net::NetworkInfoState_DISCONNECTING: {
            f = env->GetStaticFieldID(stateKlass, "DISCONNECTING", "Landroid/net/NetworkInfo$State;");
            break;
        }
        case Elastos::Droid::Net::NetworkInfoState_DISCONNECTED: {
            f = env->GetStaticFieldID(stateKlass, "DISCONNECTED", "Landroid/net/NetworkInfo$State;");
            break;
        }
        default: {
            f = env->GetStaticFieldID(stateKlass, "UNKNOWN", "Landroid/net/NetworkInfo$State;");
            break;
        }
    }
    CheckErrorAndLog(env, "ToJavaNetworkInfo", "GetStaticFieldID: %d!\n", __LINE__);

    jobject jstate = env->GetStaticObjectField(stateKlass, f);
    CheckErrorAndLog(env, "ToJavaNetworkInfo", "GetStaticObjectField: %d!\n", __LINE__);

    f = env->GetFieldID(netInfoKlass, "mState", "Landroid/net/NetworkInfo$State;");
    Util::CheckErrorAndLog(env, "ToJavaNetworkInfo", "GetFieldID: NetworkInfo$State : %d!\n", __LINE__);

    env->SetObjectField(jinfo, f, jstate);
    Util::CheckErrorAndLog(env, "ToJavaNetworkInfo", "SetObjectField: jstate : %d!\n", __LINE__);
    env->DeleteLocalRef(jstate);
    env->DeleteLocalRef(stateKlass);

    NetworkInfoDetailedState detailedState;
    info->GetDetailedState(&detailedState);

    jclass detailStateKlass = env->FindClass("android/net/NetworkInfo$DetailedState");
    CheckErrorAndLog(env, "ToJavaNetworkInfo", "FindClass: NetworkInfo$DetailedState : %d!\n", __LINE__);

    switch(detailedState) {
        case Elastos::Droid::Net::NetworkInfoDetailedState_IDLE: {
            f = env->GetStaticFieldID(detailStateKlass, "IDLE", "Landroid/net/NetworkInfo$DetailedState;");
            break;
        }
        case Elastos::Droid::Net::NetworkInfoDetailedState_SCANNING: {
            f = env->GetStaticFieldID(detailStateKlass, "SCANNING", "Landroid/net/NetworkInfo$DetailedState;");
            break;
        }
        case Elastos::Droid::Net::NetworkInfoDetailedState_CONNECTING: {
            f = env->GetStaticFieldID(detailStateKlass, "CONNECTING", "Landroid/net/NetworkInfo$DetailedState;");
            break;
        }
        case Elastos::Droid::Net::NetworkInfoDetailedState_AUTHENTICATING: {
            f = env->GetStaticFieldID(detailStateKlass, "AUTHENTICATING", "Landroid/net/NetworkInfo$DetailedState;");
            break;
        }
        case Elastos::Droid::Net::NetworkInfoDetailedState_OBTAINING_IPADDR: {
            f = env->GetStaticFieldID(detailStateKlass, "OBTAINING_IPADDR", "Landroid/net/NetworkInfo$DetailedState;");
            break;
        }
        case Elastos::Droid::Net::NetworkInfoDetailedState_CONNECTED: {
            f = env->GetStaticFieldID(detailStateKlass, "CONNECTED", "Landroid/net/NetworkInfo$DetailedState;");
            break;
        }
        case Elastos::Droid::Net::NetworkInfoDetailedState_SUSPENDED: {
            f = env->GetStaticFieldID(detailStateKlass, "SUSPENDED", "Landroid/net/NetworkInfo$DetailedState;");
            break;
        }
        case Elastos::Droid::Net::NetworkInfoDetailedState_DISCONNECTING: {
            f = env->GetStaticFieldID(detailStateKlass, "DISCONNECTING", "Landroid/net/NetworkInfo$DetailedState;");
            break;
        }
        case Elastos::Droid::Net::NetworkInfoDetailedState_DISCONNECTED: {
            f = env->GetStaticFieldID(detailStateKlass, "DISCONNECTED", "Landroid/net/NetworkInfo$DetailedState;");
            break;
        }
        case Elastos::Droid::Net::NetworkInfoDetailedState_FAILED: {
            f = env->GetStaticFieldID(detailStateKlass, "FAILED", "Landroid/net/NetworkInfo$DetailedState;");
            break;
        }
        case Elastos::Droid::Net::NetworkInfoDetailedState_BLOCKED: {
            f = env->GetStaticFieldID(detailStateKlass, "BLOCKED", "Landroid/net/NetworkInfo$DetailedState;");
            break;
        }
        case Elastos::Droid::Net::NetworkInfoDetailedState_VERIFYING_POOR_LINK: {
            f = env->GetStaticFieldID(detailStateKlass, "VERIFYING_POOR_LINK", "Landroid/net/NetworkInfo$DetailedState;");
            break;
        }
        case Elastos::Droid::Net::NetworkInfoDetailedState_CAPTIVE_PORTAL_CHECK: {
            f = env->GetStaticFieldID(detailStateKlass, "CAPTIVE_PORTAL_CHECK", "Landroid/net/NetworkInfo$DetailedState;");
            break;
        }
        default: {
            assert(0);
            break;
        }
    }
    CheckErrorAndLog(env,"ToJavaNetworkInfo", "GetStaticFieldID: %d!\n", __LINE__);

    jobject jdetailState = env->GetStaticObjectField(detailStateKlass, f);
    CheckErrorAndLog(env, "ToJavaNetworkInfo", "GetStaticObjectField: %d!\n", __LINE__);

    f = env->GetFieldID(netInfoKlass, "mDetailedState", "Landroid/net/NetworkInfo$DetailedState;");
    Util::CheckErrorAndLog(env,"ToJavaNetworkInfo", "GetFieldID: NetworkInfo$DetailedState : %d!\n", __LINE__);

    env->SetObjectField(jinfo, f, jdetailState);
    Util::CheckErrorAndLog(env, "ToJavaNetworkInfo", "SetObjectField: jdetailState : %d!\n", __LINE__);
    env->DeleteLocalRef(jdetailState);
    env->DeleteLocalRef(detailStateKlass);

    Boolean booleanTemp;
    info->IsFailover(&booleanTemp);
    Util::SetJavaBoolField(env, netInfoKlass, jinfo, booleanTemp, "mIsFailover", "GetJavaNetworkInfo");

    info->IsAvailable(&booleanTemp);
    Util::SetJavaBoolField(env, netInfoKlass, jinfo, booleanTemp, "mIsAvailable", "GetJavaNetworkInfo");

    info->IsRoaming(&booleanTemp);
    Util::SetJavaBoolField(env, netInfoKlass, jinfo, booleanTemp, "mIsRoaming", "GetJavaNetworkInfo");

    info->IsConnectedToProvisioningNetwork(&booleanTemp);
    Util::SetJavaBoolField(env, netInfoKlass, jinfo, booleanTemp, "mIsConnectedToProvisioningNetwork", "GetJavaNetworkInfo");

    String reason;
    info->GetReason(&reason);
    if (!reason.IsNull()) {
        jstring jreason = Util::ToJavaString(env, reason);

        f = env->GetFieldID(netInfoKlass, "mReason", "Ljava/lang/String;");
        CheckErrorAndLog(env, "ToJavaNetworkInfo", "GetFieldID: mReason : %d!\n", __LINE__);

        env->SetObjectField(jinfo, f, jreason);
        CheckErrorAndLog(env, "ToJavaNetworkInfo", "SetObjectField: jreason : %d!\n", __LINE__);
        env->DeleteLocalRef(jreason);
    }

    String extraInfo;
    info->GetExtraInfo(&extraInfo);
    if (!extraInfo.IsNull()) {
        jstring jextraInfo = Util::ToJavaString(env, extraInfo);

        f = env->GetFieldID(netInfoKlass, "mExtraInfo", "Ljava/lang/String;");
        CheckErrorAndLog(env, "ToJavaNetworkInfo", "GetFieldID: mExtraInfo : %d!\n", __LINE__);

        env->SetObjectField(jinfo, f, jextraInfo);
        CheckErrorAndLog(env, "ToJavaNetworkInfo", "SetObjectField: jextraInfo : %d!\n", __LINE__);
        env->DeleteLocalRef(jextraInfo);
    }

    env->DeleteLocalRef(netInfoKlass);
    return jinfo;
}

jobject Util::ToJavaWifiInfo(
    /* [in] */ JNIEnv* env,
    /* [in] */ IWifiInfo* info)
{
    if (env == NULL || info == NULL) {
        LOGGERE("ToJavaWifiInfo", "Invalid argumenet!");
        return NULL;
    }

    jobject jinfo = NULL;

    jclass wiinfoKlass = env->FindClass("android/net/wifi/WifiInfo");
    Util::CheckErrorAndLog(env, "ToJavaWifiInfo", "FindClass: WifiInfo : %d!\n", __LINE__);

    jmethodID m = env->GetMethodID(wiinfoKlass, "<init>", "()V");
    Util::CheckErrorAndLog(env, "ToJavaWifiInfo", "GetMethodID: WifiInfo : %d!\n", __LINE__);

    jinfo = env->NewObject(wiinfoKlass, m);
    Util::CheckErrorAndLog(env, "ToJavaWifiInfo", "NewObject: WifiInfo : %d!\n", __LINE__);

    Int32 tempInt;
    info->GetNetworkId(&tempInt);
    Util::SetJavaIntField(env, wiinfoKlass, jinfo, tempInt, "mNetworkId", "ToJavaWifiInfo");

    info->GetRssi(&tempInt);
    Util::SetJavaIntField(env, wiinfoKlass, jinfo, tempInt, "mRssi", "ToJavaWifiInfo");

    info->GetLinkSpeed(&tempInt);
    Util::SetJavaIntField(env, wiinfoKlass, jinfo, tempInt, "mLinkSpeed", "ToJavaWifiInfo");

    info->GetFrequency(&tempInt);
    Util::SetJavaIntField(env, wiinfoKlass, jinfo, tempInt, "mFrequency", "ToJavaWifiInfo");

    Int32 iaddress = 0;
    info->GetIpAddress(&iaddress);
    if (iaddress != 0) {
        jobject jaddress = Util::ToJavaInetAddress(env, 0, iaddress, String(NULL));

        jfieldID f = env->GetFieldID(wiinfoKlass, "mIpAddress", "Ljava/net/InetAddress;");
        Util::CheckErrorAndLog(env, "ToJavaWifiInfo", "GetFieldID: InetAddress : %d!\n", __LINE__);

        env->SetObjectField(jinfo, f, jaddress);
        Util::CheckErrorAndLog(env, "ToJavaWifiInfo", "SetIntField: jaddress : %d!\n", __LINE__);
        env->DeleteLocalRef(jaddress);
    }

    AutoPtr<IWifiSsid> wiSsid;
    info->GetWifiSsid((IWifiSsid**)&wiSsid);
    if (wiSsid != NULL) {
        jobject jwiSsid = Util::ToJavaWifiSsid(env, wiSsid);

        jfieldID f = env->GetFieldID(wiinfoKlass, "mWifiSsid", "Landroid/net/wifi/WifiSsid;");
        Util::CheckErrorAndLog(env, "ToJavaWifiInfo", "GetFieldID: WifiSsid : %d!\n", __LINE__);

        env->SetObjectField(jinfo, f, jwiSsid);
        Util::CheckErrorAndLog(env, "ToJavaWifiInfo", "SetIntField: jwiSsid : %d!\n", __LINE__);
        env->DeleteLocalRef(jwiSsid);
    }

    String mBSSID;
    info->GetBSSID(&mBSSID);
    if (!mBSSID.IsNull()) {
        jfieldID f = env->GetFieldID(wiinfoKlass, "mBSSID", "Ljava/lang/String;");
        Util::CheckErrorAndLog(env, "ToJavaWifiInfo", "GetFieldID: mBSSID : %d!\n", __LINE__);

        jobject jmBSSID = Util::ToJavaString(env, mBSSID);
        env->SetObjectField(jinfo, f, jmBSSID);
        Util::CheckErrorAndLog(env, "ToJavaWifiInfo", "SetObjectField: jmBSSID : %d!\n", __LINE__);
        env->DeleteLocalRef(jmBSSID);
    }

    String mMacAddress;
    info->GetMacAddress(&mMacAddress);
    if (!mMacAddress.IsNull()) {
        jfieldID f = env->GetFieldID(wiinfoKlass, "mMacAddress", "Ljava/lang/String;");
        Util::CheckErrorAndLog(env, "ToJavaWifiInfo", "GetFieldID: mMacAddress : %d!\n", __LINE__);

        jobject jmMacAddress = Util::ToJavaString(env, mMacAddress);
        env->SetObjectField(jinfo, f, jmMacAddress);
        Util::CheckErrorAndLog(env, "ToJavaWifiInfo", "SetObjectField: jmMacAddress : %d!\n", __LINE__);
        env->DeleteLocalRef(jmMacAddress);
    }

    Boolean hint;
    info->GetMeteredHint(&hint);
    Util::SetJavaBoolField(env, wiinfoKlass, jinfo, hint, "mMeteredHint", "ToJavaWifiInfo");

    info->GetScore(&tempInt);
    Util::SetJavaIntField(env, wiinfoKlass, jinfo, tempInt, "score", "ToJavaWifiInfo");

    Double tempDouble;
    info->GetTxSuccessRate(&tempDouble);
    Util::SetJavadoubleField(env, wiinfoKlass, jinfo, tempDouble, "txSuccessRate", "ToJavaWifiInfo");

    info->GetTxRetriesRate(&tempDouble);
    Util::SetJavadoubleField(env, wiinfoKlass, jinfo, tempDouble, "txRetriesRate", "ToJavaWifiInfo");

    info->GetTxBadRate(&tempDouble);
    Util::SetJavadoubleField(env, wiinfoKlass, jinfo, tempDouble, "txBadRate", "ToJavaWifiInfo");

    info->GetRxSuccessRate(&tempDouble);
    Util::SetJavadoubleField(env, wiinfoKlass, jinfo, tempDouble, "rxSuccessRate", "ToJavaWifiInfo");

    info->GetBadRssiCount(&tempInt);
    Util::SetJavaIntField(env, wiinfoKlass, jinfo, tempInt, "badRssiCount", "ToJavaWifiInfo");

    info->GetLowRssiCount(&tempInt);
    Util::SetJavaIntField(env, wiinfoKlass, jinfo, tempInt, "lowRssiCount", "ToJavaWifiInfo");

    AutoPtr<ISupplicantState> state;
    info->GetSupplicantState((ISupplicantState**)&state);
    jobject jstate = Util::ToJavaSupplicantState(env, state);
    jfieldID f = env->GetFieldID(wiinfoKlass, "mSupplicantState", "Landroid/net/wifi/SupplicantState;");
    Util::CheckErrorAndLog(env, "ToJavaWifiInfo", "GetFieldID: mSupplicantState : %d!\n", __LINE__);

    env->SetObjectField(jinfo, f, jstate);
    Util::CheckErrorAndLog(env, "ToJavaWifiInfo", "SetObjectField: mSupplicantState : %d!\n", __LINE__);
    env->DeleteLocalRef(jstate);
    env->DeleteLocalRef(wiinfoKlass);
    return jinfo;
}

jobject Util::ToJavaInetAddress(
    /* [in] */ JNIEnv* env,
    /* [in] */ IInetAddress* address)
{
    if (env == NULL || address == NULL) {
        LOGGERE("GetJavaInetAddress", "Invalid argumenet!");
        return NULL;
    }

    jobject jaddress = NULL;

    AutoPtr<ArrayOf<Byte> > baddress;
    address->GetAddress((ArrayOf<Byte>**)&baddress);
    String hostName;
    address->GetHostName(&hostName);

    jbyteArray jbArray = Util::ToJavaByteArray(env, baddress);
    jstring jhostName = Util::ToJavaString(env, hostName);

    jclass inaddKlass = env->FindClass("java/net/InetAddress");
    Util::CheckErrorAndLog(env, "ToJavaInetAddress", "FindClass: InetAddress : %d!\n", __LINE__);

    jmethodID m = env->GetStaticMethodID(inaddKlass, "getByAddress", "(Ljava/lang/String;[B)Ljava/net/InetAddress;");
    Util::CheckErrorAndLog(env, "ToJavaInetAddress", "GetStaticMethodID: getByAddress : %d!\n", __LINE__);

    jaddress = env->CallStaticObjectMethod(inaddKlass, m, jhostName, jbArray);
    Util::CheckErrorAndLog(env, "ToJavaInetAddress", "CallStaticObjectMethod: getByAddress(): %d!\n", __LINE__);

    env->DeleteLocalRef(jbArray);
    env->DeleteLocalRef(jhostName);
    env->DeleteLocalRef(inaddKlass);
    return jaddress;
}

jobject Util::ToJavaInetAddress(
    /* [in] */ JNIEnv* env,
    /* [in] */ Int32 family,
    /* [in] */ Int32 ipaddress,
    /* [in] */ const String& hostName)
{
    if (env == NULL || ipaddress == 0) {
        LOGGERE("ToJavaInetAddress", "Invalid argumenet!");
        return NULL;
    }

    jobject jaddress = NULL;

    jclass addsKlass = env->FindClass("java/net/InetAddress");
    Util::CheckErrorAndLog(env, "ToJavaInetAddress", "FindClass: InetAddress : %d!\n", __LINE__);

    jclass utilsKlass = env->FindClass("android/net/NetworkUtils");
    Util::CheckErrorAndLog(env, "ToJavaInetAddress", "FindClass: NetworkUtils : %d!\n", __LINE__);

    jmethodID m = env->GetStaticMethodID(utilsKlass, "intToInetAddress", "(I)Ljava/net/InetAddress;");
    Util::CheckErrorAndLog(env, "ToJavaInetAddress", "GetStaticMethodID: intToInetAddress : %d!\n", __LINE__);

    jaddress = env->CallStaticObjectMethod(utilsKlass, m, ipaddress);
    Util::CheckErrorAndLog(env, "ToJavaInetAddress", "CallStaticObjectMethod: intToInetAddress(): %d!\n", __LINE__);

    jfieldID f = env->GetFieldID(addsKlass, "family", "I");
    Util::CheckErrorAndLog(env, "ToJavaInetAddress", "GetFieldID: family : %d!\n", __LINE__);

    env->SetIntField(jaddress, f, family);
    Util::CheckErrorAndLog(env, "ToJavaInetAddress", "SetIntField: family : %d!\n", __LINE__);

    if (!hostName.IsNullOrEmpty()) {
        jstring jhostName = Util::ToJavaString(env, hostName);

        f = env->GetFieldID(addsKlass, "hostName", "Ljava/lang/String;");
        Util::CheckErrorAndLog(env, "ToJavaInetAddress", "GetFieldID: hostName : %d!\n", __LINE__);

        env->SetObjectField(jaddress, f, jhostName);
        Util::CheckErrorAndLog(env, "ToJavaInetAddress", "SetObjectField: hostName : %d!\n", __LINE__);
        env->DeleteLocalRef(jhostName);
    }
    env->DeleteLocalRef(addsKlass);
    env->DeleteLocalRef(utilsKlass);
    return jaddress;
}

jobject Util::ToJavaWifiSsid(
    /* [in] */ JNIEnv* env,
    /* [in] */ IWifiSsid* wiSsid)
{
    if (env == NULL || wiSsid == NULL) {
        LOGGERE("ToJavaWifiSsid", "Invalid argumenet!");
        return NULL;
    }

    jobject jwiSsid = NULL;

    jclass ssidKlass = env->FindClass("android/net/wifi/WifiSsid");
    Util::CheckErrorAndLog(env, "ToJavaWifiSsid", "FindClass: WifiSsid : %d!\n", __LINE__);

    jmethodID m = env->GetMethodID(ssidKlass, "<init>", "()V");
    Util::CheckErrorAndLog(env, "ToJavaWifiSsid", "GetMethodID: WifiSsid : %d!\n", __LINE__);

    jwiSsid = env->NewObject(ssidKlass, m);
    Util::CheckErrorAndLog(env, "ToJavaWifiSsid", "NewObject: WifiSsid : %d!\n", __LINE__);

    AutoPtr<ArrayOf<Byte> > octets;
    wiSsid->GetOctets((ArrayOf<Byte>**)&octets);
    if (octets != NULL) {
        jfieldID f = env->GetFieldID(ssidKlass, "octets", "Ljava/io/ByteArrayOutputStream;");
        Util::CheckErrorAndLog(env, "ToJavaWifiSsid", "GetFieldID: ByteArrayOutputStream : %d!\n", __LINE__);

        jobject joctets = env->GetObjectField(jwiSsid, f);
        Util::CheckErrorAndLog(env, "ToJavaWifiSsid", "GetObjectField: octets : %d!\n", __LINE__);

        Int32 length = octets->GetLength();
        jbyteArray jbArray = env->NewByteArray((jint)length);
        Util::CheckErrorAndLog(env, "ToJavaWifiSsid", "NewByteArray: %d!\n", __LINE__);

        Byte* payload = octets->GetPayload();

        env->SetByteArrayRegion(jbArray, 0, length, (jbyte *)payload);
        Util::CheckErrorAndLog(env, "ToJavaWifiSsid", "SetByteArrayRegion: %d!\n", __LINE__);

        jclass bostreamKlass = env->FindClass("java/io/ByteArrayOutputStream");
        Util::CheckErrorAndLog(env, "ToJavaWifiSsid", "FindClass: ByteArrayOutputStream : %d!\n", __LINE__);

        m = env->GetMethodID(bostreamKlass, "write", "([BII)V");
        Util::CheckErrorAndLog(env, "ToJavaWifiSsid", "GetMethodID: write : %d!\n", __LINE__);

        env->CallVoidMethod(joctets, m, jbArray, 0, length);
        Util::CheckErrorAndLog(env, "ToJavaWifiSsid", "CallVoidMethod: write : %d!\n", __LINE__);

        env->SetObjectField(jwiSsid, f, joctets);
        Util::CheckErrorAndLog(env, "ToJavaWifiSsid", "SetObjectField: joctets : %d!\n", __LINE__);

        env->DeleteLocalRef(bostreamKlass);
        env->DeleteLocalRef(joctets);
        env->DeleteLocalRef(jbArray);

    }
    env->DeleteLocalRef(ssidKlass);
    return jwiSsid;
}

jobject Util::ToJavaSupplicantState(
    /* [in] */ JNIEnv* env,
    /* [in] */ SupplicantState state)
{
    if (env == NULL) {
        LOGGERE("ToJavaSupplicantState", "Invalid argumenet!");
        return NULL;
    }

    jobject jstate = NULL;

    jclass supsKlass = env->FindClass("android/net/wifi/SupplicantState");
    Util::CheckErrorAndLog(env, "ToJavaSupplicantState", "FindClass: SupplicantState : %d!\n", __LINE__);

    jfieldID f = NULL;

    switch(state) {
        case Elastos::Droid::Wifi::SupplicantState_DISCONNECTED: {
            f = env->GetStaticFieldID(supsKlass, "DISCONNECTED", "Landroid/net/wifi/SupplicantState;");
            break;
        }
        case Elastos::Droid::Wifi::SupplicantState_INTERFACE_DISABLED: {
            f = env->GetStaticFieldID(supsKlass, "INTERFACE_DISABLED", "Landroid/net/wifi/SupplicantState;");
            break;
        }
        case Elastos::Droid::Wifi::SupplicantState_INACTIVE: {
            f = env->GetStaticFieldID(supsKlass, "INACTIVE", "Landroid/net/wifi/SupplicantState;");
            break;
        }
        case Elastos::Droid::Wifi::SupplicantState_SCANNING: {
            f = env->GetStaticFieldID(supsKlass, "SCANNING", "Landroid/net/wifi/SupplicantState;");
            break;
        }
        case Elastos::Droid::Wifi::SupplicantState_AUTHENTICATING: {
            f = env->GetStaticFieldID(supsKlass, "AUTHENTICATING", "Landroid/net/wifi/SupplicantState;");
            break;
        }
        case Elastos::Droid::Wifi::SupplicantState_ASSOCIATING: {
            f = env->GetStaticFieldID(supsKlass, "ASSOCIATING", "Landroid/net/wifi/SupplicantState;");
            break;
        }
        case Elastos::Droid::Wifi::SupplicantState_ASSOCIATED: {
            f = env->GetStaticFieldID(supsKlass, "ASSOCIATED", "Landroid/net/wifi/SupplicantState;");
            break;
        }
        case Elastos::Droid::Wifi::SupplicantState_FOUR_WAY_HANDSHAKE: {
            f = env->GetStaticFieldID(supsKlass, "FOUR_WAY_HANDSHAKE", "Landroid/net/wifi/SupplicantState;");
            break;
        }
        case Elastos::Droid::Wifi::SupplicantState_GROUP_HANDSHAKE: {
            f = env->GetStaticFieldID(supsKlass, "GROUP_HANDSHAKE", "Landroid/net/wifi/SupplicantState;");
            break;
        }
        case Elastos::Droid::Wifi::SupplicantState_COMPLETED: {
            f = env->GetStaticFieldID(supsKlass, "COMPLETED", "Landroid/net/wifi/SupplicantState;");
            break;
        }
        case Elastos::Droid::Wifi::SupplicantState_DORMANT: {
            f = env->GetStaticFieldID(supsKlass, "DORMANT", "Landroid/net/wifi/SupplicantState;");
            break;
        }
        case Elastos::Droid::Wifi::SupplicantState_UNINITIALIZED: {
            f = env->GetStaticFieldID(supsKlass, "UNINITIALIZED", "Landroid/net/wifi/SupplicantState;");
            break;
        }
        case Elastos::Droid::Wifi::SupplicantState_INVALID: {
            f = env->GetStaticFieldID(supsKlass, "INVALID", "Landroid/net/wifi/SupplicantState;");
            break;
        }
        default:
            assert(0);
            return NULL;
    }
    Util::CheckErrorAndLog(env, "ToJavaSupplicantState", "GetStaticFieldID: %d!\n", __LINE__);

    jstate = env->GetStaticObjectField(supsKlass, f);
    Util::CheckErrorAndLog(env, "ToJavaSupplicantState", "GetStaticObjectField: %d!\n", __LINE__);
    env->DeleteLocalRef(supsKlass);
    return jstate;
}

jobject Util::ToJavaSupplicantState(
    /* [in] */ JNIEnv* env,
    /* [in] */ ISupplicantState* state)
{
    if (env == NULL || state == NULL) {
        LOGGERE("ToJavaSupplicantState", "Invalid argumenet!");
        return NULL;
    }

    SupplicantState suppState;
    state->Get(&suppState);
    return ToJavaSupplicantState(env, suppState);
}

jobject Util::ToJavaLinkProperties(
    /* [in] */ JNIEnv* env,
    /* [in] */ ILinkProperties* properties)
{
    if (env == NULL || properties == NULL) {
        LOGGERE("ToJavaLinkProperties", "Invalid argumenet!");
        return NULL;
    }

    jobject jproperties = NULL;

    jclass lProtiesKlass = env->FindClass("android/net/LinkProperties");
    Util::CheckErrorAndLog(env, "ToJavaLinkProperties", "FindClass: LinkProperties : %d!\n", __LINE__);

    jmethodID m = env->GetMethodID(lProtiesKlass, "<init>", "()V");
    Util::CheckErrorAndLog(env, "ToJavaLinkProperties", "GetMethodID: LinkProperties : %d!\n", __LINE__);

    jproperties = env->NewObject(lProtiesKlass, m);
    Util::CheckErrorAndLog(env, "ToJavaLinkProperties", "NewObject: LinkProperties : %d!\n", __LINE__);

    String ifaceName;
    properties->GetInterfaceName(&ifaceName);
    if (!ifaceName.IsNull()) {
        jfieldID f = env->GetFieldID(lProtiesKlass, "mIfaceName", "Ljava/lang/String;");
        Util::CheckErrorAndLog(env, "ToJavaLinkProperties", "GetFieldID: mIfaceName : %d!\n", __LINE__);

        jstring jifaceName = Util::ToJavaString(env, ifaceName);
        env->SetObjectField(jproperties, f, jifaceName);
        Util::CheckErrorAndLog(env, "ToJavaLinkProperties", "GetObjectField: SetObjectField : %d!\n", __LINE__);
        env->DeleteLocalRef(jifaceName);
    }

    AutoPtr<IList> addresses;
    properties->GetLinkAddresses((IList**)&addresses);
    if (addresses != NULL) {
        Int32 count = 0;
        addresses->GetSize(&count);
        if (count > 0) {
            jmethodID mAdd = env->GetMethodID(lProtiesKlass, "addLinkAddress", "(Landroid/net/LinkAddress;)Z");
            Util::CheckErrorAndLog(env, "ToJavaLinkProperties", "GetMethodID: addLinkAddress : %d!\n", __LINE__);

            jclass laddrKlass = env->FindClass("android/net/LinkAddress");
            Util::CheckErrorAndLog(env, "ToJavaLinkProperties", "FindClass: LinkAddress : %d!\n", __LINE__);

            m = env->GetMethodID(laddrKlass, "<init>", "(Ljava/net/InetAddress;III)V");
            Util::CheckErrorAndLog(env, "ToJavaLinkProperties", "GetMethodID: appInfoKlass : %d!\n", __LINE__);

            for (Int32 i = 0; i < count; i++) {
                AutoPtr<IInterface> obj;
                addresses->Get(i, (IInterface**)&obj);
                jobject jladdr = ToJavaLinkAddress(env, ILinkAddress::Probe(obj));

                env->CallBooleanMethod(jproperties, mAdd, jladdr);
                Util::CheckErrorAndLog(env, "ToJavaLinkProperties", "CallBooleanMethod: addLinkAddress : %d!\n", __LINE__);
                env->DeleteLocalRef(jladdr);
            }

            env->DeleteLocalRef(laddrKlass);
        }
    }

    AutoPtr<IList> dnses;
    properties->GetDnsServers((IList**)&dnses);
    if (dnses != NULL) {
        Int32 count = 0;
        dnses->GetSize(&count);
        if (count > 0) {
            jmethodID mAdd = env->GetMethodID(lProtiesKlass, "addDnsServer", "(Ljava/net/InetAddress;)Z");
            Util::CheckErrorAndLog(env, "ToJavaLinkProperties", "GetMethodID: addDnsServer : %d!\n", __LINE__);

            for (Int32 i = 0; i < count; i++) {
                AutoPtr<IInterface> obj;
                dnses->Get(i, (IInterface**)&obj);
                AutoPtr<IInetAddress> iaddr = IInetAddress::Probe(obj);

                jobject jiaddr = Util::ToJavaInetAddress(env, iaddr);

                env->CallBooleanMethod(jproperties, mAdd, jiaddr);
                Util::CheckErrorAndLog(env, "ToJavaLinkProperties", "CallBooleanMethod: addDnsServer : %d!\n", __LINE__);

                env->DeleteLocalRef(jiaddr);
            }
        }
    }

    String str;
    properties->GetDomains(&str);
    Util::SetJavaStringField(env, lProtiesKlass, jproperties, str, "mDomains", "ToJavaLinkProperties");

    Int32 mtu;
    properties->GetMtu(&mtu);
    Util::SetJavaIntField(env, lProtiesKlass, jproperties, mtu, "mMtu", "ToJavaLinkProperties");

    properties->GetTcpBufferSizes(&str);
    Util::SetJavaStringField(env, lProtiesKlass, jproperties, str, "mTcpBufferSizes", "ToJavaLinkProperties");

    AutoPtr<IList> routes;
    properties->GetRoutes((IList**)&routes);
    if (routes != NULL) {
        Int32 count = 0;
        routes->GetSize(&count);
        if (count > 0) {
            jmethodID mAdd = env->GetMethodID(lProtiesKlass, "addRoute", "(Landroid/net/RouteInfo;)Z");
            Util::CheckErrorAndLog(env, "ToJavaLinkProperties", "GetMethodID: addRoute : %d!\n", __LINE__);

            for (Int32 i = 0; i < count; i++) {
                AutoPtr<IInterface> obj;
                routes->Get(i, (IInterface**)&obj);
                AutoPtr<IRouteInfo> route = IRouteInfo::Probe(obj);

                jobject jroute = Util::ToJavaRouteInfo(env, route);

                env->CallBooleanMethod(jproperties, mAdd, jroute);
                Util::CheckErrorAndLog(env, "ToJavaLinkProperties", "CallBooleanMethod: addRoute : %d!\n", __LINE__);

                env->DeleteLocalRef(jroute);
            }
        }
    }

    AutoPtr<IProxyInfo> httpProxy;
    properties->GetHttpProxy((IProxyInfo**)&httpProxy);
    if (httpProxy != NULL) {
        jobject jhttpProxy = Util::ToJavaProxyInfo(env, httpProxy);

        jmethodID m = env->GetMethodID(lProtiesKlass, "setHttpProxy", "(Landroid/net/ProxyInfo;)V");
        Util::CheckErrorAndLog(env, "ToJavaLinkProperties", "GetMethodID: setHttpProxy : %d!\n", __LINE__);

        env->CallVoidMethod(jproperties, m, jhttpProxy);
        Util::CheckErrorAndLog(env, "ToJavaLinkProperties", "CallVoidMethod: setHttpProxy : %d!\n", __LINE__);
        env->DeleteLocalRef(jhttpProxy);
    }

    AutoPtr<IList> stackedLinks;
    properties->GetStackedLinks((IList**)&stackedLinks);
    if (stackedLinks != NULL) {
        Int32 count = 0;
        stackedLinks->GetSize(&count);
        if (count > 0) {
            jmethodID mAdd = env->GetMethodID(lProtiesKlass, "addStackedLink", "(Landroid/net/LinkProperties;)Z");
            Util::CheckErrorAndLog(env, "ToJavaLinkProperties", "GetMethodID: addStackedLink : %d!\n", __LINE__);

            for (Int32 i = 0; i < count; i++) {
                AutoPtr<IInterface> obj;
                stackedLinks->Get(i, (IInterface**)&obj);
                AutoPtr<ILinkProperties> lps = ILinkProperties::Probe(obj);

                jobject jlps = Util::ToJavaLinkProperties(env, lps);

                env->CallBooleanMethod(jproperties, mAdd, jlps);
                Util::CheckErrorAndLog(env, "ToJavaLinkProperties", "CallBooleanMethod: addStackedLink : %d!\n", __LINE__);

                env->DeleteLocalRef(jlps);
            }
        }
    }

    env->DeleteLocalRef(lProtiesKlass);
    return jproperties;
}

jobject Util::ToJavaRouteInfo(
    /* [in] */ JNIEnv* env,
    /* [in] */ IRouteInfo* info)
{
    if (env == NULL || info == NULL){
        LOGGERE("ToJavaRouteInfo()", "Invalid argumenet!");
        return NULL;
    }

    jclass riKlass = env->FindClass("android/net/RouteInfo");
    CheckErrorAndLog(env, "ToJavaRouteInfo", "FindClass: RouteInfo : %d!\n", __LINE__);

    jmethodID m = env->GetMethodID(riKlass, "<init>", "(Landroid/net/IpPrefix;Ljava/net/InetAddress;Ljava/lang/String;I)V");
    CheckErrorAndLog(env, "ToJavaRouteInfo", "GetMethodID: RouteInfo : %d!\n", __LINE__);

    AutoPtr<IIpPrefix> destination;
    info->GetDestination((IIpPrefix**)&destination);
    jobject jdestination = NULL;
    if (destination != NULL) {
        jdestination = Util::ToJavaIpPrefix(env, destination);
    }

    AutoPtr<IInetAddress> gateway;
    info->GetGateway((IInetAddress**)&gateway);
    jobject jgateway = NULL;
    if (gateway != NULL) {
        jgateway = Util::ToJavaInetAddress(env, gateway);
    }

    String iface;
    info->GetInterface(&iface);
    jstring jiface = Util::ToJavaString(env, iface);

    Int32 type;
    info->GetType(&type);

    jobject jinfo = env->NewObject(riKlass, m, jdestination, jgateway, jiface, (jint)type);
    CheckErrorAndLog(env, "ToJavaRouteInfo", "NewObject: RouteInfo : %d!\n", __LINE__);

    env->DeleteLocalRef(riKlass);
    env->DeleteLocalRef(jiface);

    return jinfo;
}

jobject Util::ToJavaIpPrefix(
    /* [in] */ JNIEnv* env,
    /* [in] */ IIpPrefix* ipPrefix)
{
    if (env == NULL || ipPrefix == NULL) {
        ALOGE("ToJavaIpPrefix() Invalid argumenet!");
        return NULL;
    }

    jclass ipKlass = env->FindClass("android/net/IpPrefix");
    CheckErrorAndLog(env, "ToJavaIpPrefix", "FindClass: IpPrefix : %d!\n", __LINE__);

    jmethodID m = env->GetMethodID(ipKlass, "<init>", "([BI)V");
    CheckErrorAndLog(env, "ToJavaIpPrefix", "GetMethodID: IpPrefix : %d!\n", __LINE__);

    AutoPtr<ArrayOf<Byte> > address;
    ipPrefix->GetRawAddress((ArrayOf<Byte>**)&address);
    jbyteArray jaddress = NULL;
    if (address != NULL) {
        jaddress = Util::ToJavaByteArray(env, address);
    }

    Int32 prefixLength = 0;
    ipPrefix->GetPrefixLength(&prefixLength);

    jobject jipPrefix = env->NewObject(ipKlass, m, jaddress, (jint)prefixLength);
    CheckErrorAndLog(env, "ToJavaIpPrefix", "NewObject: IpPrefix : %d!\n", __LINE__);

    env->DeleteLocalRef(ipKlass);
    env->DeleteLocalRef(jaddress);
    return jipPrefix;
}

jobject Util::ToJavaLinkAddress(
    /* [in] */ JNIEnv* env,
    /* [in] */ ILinkAddress* info)
{
    if (env == NULL || info == NULL) {
        ALOGE("ToJavaLinkAddress() Invalid argumenet!");
        return NULL;
    }

    jclass laKlass = env->FindClass("android/net/LinkAddress");
    CheckErrorAndLog(env, "ToJavaLinkAddress", "FindClass: LinkAddress : %d!\n", __LINE__);

    jmethodID m = env->GetMethodID(laKlass, "<init>", "(Ljava/net/InetAddress;III)V");
    CheckErrorAndLog(env, "ToJavaLinkAddress", "GetMethodID: LinkAddress : %d!\n", __LINE__);

    AutoPtr<IInetAddress> address;
    info->GetAddress((IInetAddress**)&address);
    jobject jaddress = NULL;
    if (address != NULL) {
        jaddress = Util::ToJavaInetAddress(env, address);
    }

    Int32 prefixLength, flags, scope;
    info->GetNetworkPrefixLength(&prefixLength);
    info->GetFlags(&flags);
    info->GetScope(&scope);

    jobject jinfo = env->NewObject(laKlass, m, jaddress, (jint)prefixLength, (jint)flags, (jint)scope);
    CheckErrorAndLog(env, "ToJavaLinkAddress", "NewObject: LinkAddress : %d!\n", __LINE__);

    env->DeleteLocalRef(jaddress);
    env->DeleteLocalRef(laKlass);
    return jinfo;
}

jobject Util::ToJavaProxyProperties(
    /* [in] */ JNIEnv* env,
    /* [in] */ IProxyProperties* pproperties)
{
    if (env == NULL || pproperties == NULL){
        LOGGERE("ToJavaProxyProperties()", "Invalid argumenet!");
        return NULL;
    }

    jclass ppKlass = env->FindClass("android/net/ProxyProperties");
    Util::CheckErrorAndLog(env, "ToJavaLinkProperties", "FindClass: ProxyProperties : %d!\n", __LINE__);

    String host;
    pproperties->GetHost(&host);
    jstring jhost = Util::ToJavaString(env, host);

    Int32 port;
    pproperties->GetPort(&port);

    String exclList;
    pproperties->GetExclusionList(&exclList);
    jstring jexclList = Util::ToJavaString(env, exclList);

    AutoPtr<ArrayOf<String> > parsedExclusionList;
    pproperties->GetParsedExclusionList((ArrayOf<String>**)&parsedExclusionList);
    jobjectArray jparsedExclusionList = Util::ToJavaStringArray(env, parsedExclusionList);

    jmethodID m = env->GetMethodID(ppKlass, "<init>", "(Ljava/lang/String;ILjava/lang/String;[Ljava/lang/String;)V");
    Util::CheckErrorAndLog(env, "ToJavaLinkProperties", "GetMethodID: ProxyProperties : %d!\n", __LINE__);

    jobject jpproperties = env->NewObject(ppKlass, m, jhost, (jint)port, jexclList, jparsedExclusionList);
    Util::CheckErrorAndLog(env, "ToJavaLinkProperties", "NewObject: ProxyProperties : %d!\n", __LINE__);

    env->DeleteLocalRef(ppKlass);
    env->DeleteLocalRef(jhost);
    env->DeleteLocalRef(jexclList);
    env->DeleteLocalRef(jparsedExclusionList);
    return jpproperties;
}

jobject Util::ToJavaProxyInfo(
    /* [in] */ JNIEnv* env,
    /* [in] */ IProxyInfo* proxyInfo)
{
    if (env == NULL || proxyInfo == NULL){
        LOGGERE("ToJavaProxyInfo()", "Invalid argumenet!");
        return NULL;
    }

    jclass piKlass = env->FindClass("android/net/ProxyInfo");
    Util::CheckErrorAndLog(env, "ToJavaProxyInfo", "FindClass: ProxyInfo : %d!\n", __LINE__);

    String host;
    proxyInfo->GetHost(&host);
    jstring jhost = Util::ToJavaString(env, host);

    Int32 port;
    proxyInfo->GetPort(&port);

    String exclList;
    proxyInfo->GetExclusionListAsString(&exclList);
    jstring jexclList = Util::ToJavaString(env, exclList);

    AutoPtr<ArrayOf<String> > parsedExclusionList;
    proxyInfo->GetExclusionList((ArrayOf<String>**)&parsedExclusionList);
    jobjectArray jparsedExclusionList = Util::ToJavaStringArray(env, parsedExclusionList);

    AutoPtr<IUri> pacFileUrl;
    proxyInfo->GetPacFileUrl((IUri**)&pacFileUrl);

    jobject jpacFileUrl = ToJavaUri(env, pacFileUrl);

    jmethodID m = env->GetMethodID(piKlass, "<init>", "(Ljava/lang/String;ILjava/lang/String;[Ljava/lang/String;)V");
    Util::CheckErrorAndLog(env, "ToJavaProxyInfo", "GetMethodID: ProxyInfo : %d!\n", __LINE__);

    jobject jproxyInfo = env->NewObject(piKlass, m, jhost, (jint)port, jexclList, jparsedExclusionList);
    Util::CheckErrorAndLog(env, "ToJavaProxyInfo", "NewObject: ProxyInfo : %d!\n", __LINE__);

    if (jpacFileUrl != NULL) {
        jfieldID f = env->GetFieldID(piKlass, "mPacFileUrl", "Landroid/net/Uri;");
        CheckErrorAndLog(env, "ToJavaProxyInfo", "GetFieldID: mPacFileUrl : %d!\n", __LINE__);

        env->SetObjectField(jproxyInfo, f, jpacFileUrl);
        Util::CheckErrorAndLog(env, "ToJavaProxyInfo Fail SetObjectField: mPacFileUrl : %d!\n", __LINE__);
        env->DeleteLocalRef(jpacFileUrl);
    }

    env->DeleteLocalRef(piKlass);
    env->DeleteLocalRef(jhost);
    env->DeleteLocalRef(jexclList);
    env->DeleteLocalRef(jparsedExclusionList);
    return jproxyInfo;
}

Boolean Util::GetElPackageInfoLite(
    /* [in] */ JNIEnv* env,
    /* [in] */ jobject jpkgLite,
    /* [out] */ IPackageInfoLite** pkgLite)
{
    if (jpkgLite == NULL) {
        LOGGERE("GetElPackageInfoLite", "Invalid argumenet!");
        return FALSE;
    }

    if (NOERROR != CPackageInfoLite::New(pkgLite)) {
        LOGGERE("GetElPackageInfoLite", "create CPackageInfoLite fail!");
        return FALSE;
    }

    jclass pkgLiteKlass = env->FindClass("android/content/pm/PackageInfoLite");
    Util::CheckErrorAndLog(env, "GetElPackageInfoLite", "Fail FindClass: PackageInfoLite  %d", __LINE__);

    const char tag[] = "GetElPackageInfoLite";

    String packageName = GetJavaStringField(env, pkgLiteKlass, jpkgLite, "packageName", tag);
    Int32 versionCode = GetJavaIntField(env, pkgLiteKlass, jpkgLite, "versionCode", tag);
    Int32 recommendedInstallLocation = GetJavaIntField(env, pkgLiteKlass, jpkgLite, "recommendedInstallLocation", tag);
    Int32 installLocation = GetJavaIntField(env, pkgLiteKlass, jpkgLite, "installLocation", tag);
    Boolean multiArch = GetJavaBoolField(env, jpkgLite, "multiArch", tag);
    Boolean isTheme = GetJavaBoolField(env, jpkgLite, "isTheme", tag);

    (*pkgLite)->SetPackageName(packageName);
    (*pkgLite)->SetVersionCode(versionCode);
    (*pkgLite)->SetRecommendedInstallLocation(recommendedInstallLocation);
    (*pkgLite)->SetInstallLocation(installLocation);
    (*pkgLite)->SetMultiArch(multiArch);
    (*pkgLite)->SetIsTheme(isTheme);

    jfieldID m = env->GetFieldID(pkgLiteKlass, "verifiers", "[Landroid/content/pm/VerifierInfo;");
    Util::CheckErrorAndLog(env, "GetElPackageInfoLite", "GetFieldID verifiers  %d", __LINE__);

    jobjectArray jverifiers = (jobjectArray)env->GetObjectField(jpkgLite, m);
    Util::CheckErrorAndLog(env, "GetElPackageInfoLite", "GetObjectField verifiers  %d", __LINE__);

    if (jverifiers != NULL) {
        jint jcount = env->GetArrayLength(jverifiers);
        if (jcount > 0) {
            AutoPtr<ArrayOf<IVerifierInfo*> > verifiers = ArrayOf<IVerifierInfo*>::Alloc((Int32)jcount);
            if (verifiers != NULL) {
                for (Int32 i = 0; i < jcount; i++) {
                    jobject jverifier = env->GetObjectArrayElement(jverifiers, i);
                    CheckErrorAndLog(env, "GetElPackageInfoLite", "GetObjectArrayElement: jverifier %d",  __LINE__);

                    if (jverifier != NULL) {
                        AutoPtr<IVerifierInfo> verifier;
                        if (Util::GetElVerifierInfo(env, jverifier, (IVerifierInfo**)&verifier)) {
                            verifiers->Set(i, verifier);
                        }
                        else {
                            LOGGERE("GetElPackageInfoLite", "GetElVerifierInfo fail!");
                        }
                        env->DeleteLocalRef(jverifier);
                    }
                    (*pkgLite)->SetVerifiers(verifiers);
                }
            }
        }

        env->DeleteLocalRef(jverifiers);
    }

    env->DeleteLocalRef(pkgLiteKlass);
    return TRUE;
}

Boolean Util::GetElRemoteViews(
    /* [in] */ JNIEnv* env,
    /* [in] */ jobject jremoteview,
    /* [in] */ IRemoteViews** remoteview)
{
    if (remoteview) *remoteview = NULL;

    if (env == NULL || jremoteview == NULL || remoteview == NULL) {
        LOGGERE(TAG, "GetElRemoteViews: Invalid argumenet!");
        return FALSE;
    }

    jclass remoteviewClass = env->FindClass("android/widget/RemoteViews");

    jfieldID mLandscapeField = env->GetFieldID(remoteviewClass, "mLandscape", "Landroid/widget/RemoteViews;");
    CheckErrorAndLog(env, "%s: Fail get RemoteViews fieldid: %s : %d!\n", "GetElRemoteViews", "mLandscape ", __LINE__);
    jobject jmLandscape = env->GetObjectField(jremoteview, mLandscapeField);
    CheckErrorAndLog(env, "%s: Fail get RemoteViews field: %s : %d!\n", "GetElRemoteViews", "mLandscape ", __LINE__);

    jfieldID mPortraitField = env->GetFieldID(remoteviewClass, "mPortrait", "Landroid/widget/RemoteViews;");
    CheckErrorAndLog(env, "%s: Fail get RemoteViews fieldid: %s : %d!\n", "GetElRemoteViews", "mPortrait ", __LINE__);
    jobject jmPortrait = env->GetObjectField(jremoteview, mPortraitField);
    CheckErrorAndLog(env, "%s: Fail get RemoteViews field: %s : %d!\n", "GetElRemoteViews", "mPortrait ", __LINE__);

    jfieldID mApplicationField = env->GetFieldID(remoteviewClass, "mApplication", "Landroid/content/pm/ApplicationInfo;");
    CheckErrorAndLog(env, "%s: Fail get RemoteViews fieldid: %s : %d!\n", "GetElRemoteViews", "mApplication ", __LINE__);
    jobject jmApplication = env->GetObjectField(jremoteview, mApplicationField);
    CheckErrorAndLog(env, "%s: Fail get RemoteViews field: %s : %d!\n", "GetElRemoteViews", "mApplication ", __LINE__);

    if (jmLandscape != NULL && jmPortrait != NULL) {
        AutoPtr<IRemoteViews> landscape, portrait;
        GetElRemoteViews(env, jmLandscape, (IRemoteViews**)&landscape);
        GetElRemoteViews(env, jmPortrait, (IRemoteViews**)&portrait);
        CRemoteViews::New(landscape, portrait, remoteview);
        env->DeleteLocalRef(remoteviewClass);
        env->DeleteLocalRef(jmLandscape);
        env->DeleteLocalRef(jmPortrait);
        return TRUE;
    }

    AutoPtr<IApplicationInfo> application;
    if (!GetElApplicationInfo(env, jmApplication, (IApplicationInfo**)&application)) {
        LOGGERE("GetElRemoteViews", "GetElRemoteViews() GetElApplicationInfo fail!");
    }
    Int32 mLayoutId = GetJavaIntField(env, remoteviewClass, jremoteview, "mLayoutId", "GetElRemoteViews");
    CRemoteViews::New(application, mLayoutId, remoteview);

    jboolean mIsRoot = GetJavaBoolField(env, jremoteview, "mIsRoot", "GetElRemoteViews");

    if (!mIsRoot){
        (*remoteview)->SetNotRoot();
    }

    jboolean isWidgetCollectionChild = GetJavaBoolField(env, jremoteview, "mIsWidgetCollectionChild", "GetElRemoteViews");
    (*remoteview)->SetIsWidgetCollectionChild(isWidgetCollectionChild);

    // Process actions
    jfieldID mActionField = env->GetFieldID(remoteviewClass, "mActions", "Ljava/util/ArrayList;");
    CheckErrorAndLog(env, "%s: Fail get ArrayList fieldid: %s : %d!\n", "GetElRemoteViews", "mActions", __LINE__);
    jobject jactions = env->GetObjectField(jremoteview, mActionField);
    CheckErrorAndLog(env, "%s: Fail get ArrayList field: %s : %d!\n", "GetElRemoteViews", "mActions", __LINE__);

    if (jactions != NULL){
        jclass alistKlass = env->FindClass("java/util/ArrayList");
        CheckErrorAndLog(env, "FindClass: ArrayList : %d!\n", __LINE__);

        jmethodID mSize = env->GetMethodID(alistKlass, "size", "()I");
        CheckErrorAndLog(env, "GetMethodID: get : %d!\n", __LINE__);

        jmethodID mGet = env->GetMethodID(alistKlass, "get", "(I)Ljava/lang/Object;");
        CheckErrorAndLog(env, "GetMethodID: get : %d!\n", __LINE__);
        env->DeleteLocalRef(alistKlass);

        jint jsize = env->CallIntMethod(jactions, mSize);
        CheckErrorAndLog(env, "CallIntMethod: mActions : %d!\n", __LINE__);
        if (jsize > 0) {
            // LOGGERD(TAG, "Util::GetElRemoteViews action size = :%d", jsize);
            for (jint i = 0; i < jsize; i++) {
                jobject jaction = env->CallObjectMethod(jactions, mGet, i);
                CheckErrorAndLog(env, "CallObjectMethod: get : %d!\n", __LINE__);
                // Get TAG field to checkout action type
                jclass klass = env->GetObjectClass(jaction);
                CheckErrorAndLog(env, "%s: Fail get object class. %s%d!\n", "GetElRemoteViews", "", __LINE__);
                jfieldID fTag = env->GetStaticFieldID(klass, "TAG", "I");
                CheckErrorAndLog(env, "%s: Fail GetStaticFieldID. %s%d!\n", "GetElRemoteViews", "TAG", __LINE__);
                Int32 tag = env->GetStaticIntField(klass, fTag);
                CheckErrorAndLog(env, "%s: Fail GetStaticIntField. %s%d!\n", "GetElRemoteViews", "TAG", __LINE__);
                Int32 viewId = GetJavaIntField(env, klass, jaction, "viewId", "GetElRemoteViews");

                switch(tag){
                    case 1: //SetOnClickPendingIntent.TAG:
                    {
                        // LOGGERD(TAG, "GetElRemoteViews: found a SetOnClickPendingIntent action.");
                        jfieldID f = env->GetFieldID(klass, "pendingIntent", "Landroid/app/PendingIntent;");
                        Util::CheckErrorAndLog(env, "GetFieldID: pendingIntent : %d!\n", __LINE__);
                        jobject jpendingIntent = env->GetObjectField(jaction, f);
                        Util::CheckErrorAndLog(env, "GetObjectField: pendingIntent : %d!\n", __LINE__);
                        AutoPtr<IPendingIntent> pendingIntent;
                        GetElPendingIntent(env, jpendingIntent, (IPendingIntent**)&pendingIntent);
                        env->DeleteLocalRef(jpendingIntent);

                        (*remoteview)->SetOnClickPendingIntent(viewId, pendingIntent);
                        break;
                    }
                    case 3: //SetDrawableParameters.TAG:
                    {
                        // LOGGERD(TAG, "GetElRemoteViews: found a SetDrawableParameters action.");
                        Boolean targetBackground = (Boolean)GetJavaBoolField(env, jaction, "targetBackground", "GetElRemoteViews");
                        Int32 alpha = GetJavaIntField(env, klass, jaction, "alpha", "GetElRemoteViews");
                        Int32 colorFilter = GetJavaIntField(env, klass, jaction, "colorFilter", "GetElRemoteViews");
                        Int32 level = GetJavaIntField(env, klass, jaction, "level", "GetElRemoteViews");

                        // Translate mode
                        jfieldID modeField = env->GetFieldID(klass, "filterMode", "Landroid/graphics/PorterDuff$Mode;");
                        Util::CheckErrorAndLog(env, "GetFieldID: filterMode : %d!\n", __LINE__);
                        jobject jmode = env->GetObjectField(jaction, modeField);
                        Util::CheckErrorAndLog(env, "GetObjectField: filterMode : %d!\n", __LINE__);

                        Int32 mode = -1;
                        if (jmode != NULL){
                            jclass c = env->GetObjectClass(jmode);
                            mode = GetJavaIntField(env, c, jmode, "nativeInt", "GetElRemoteViews");
                            env->DeleteLocalRef(c);
                            env->DeleteLocalRef(jmode);
                            LOGGERE(TAG, "GetElRemoteViews, SetDrawableParameters, PorterDuff.Mode mode = %d", mode);
                        }
                        else{
                            LOGGERE(TAG, "GetElRemoteViews, SetDrawableParameters, PorterDuff.Mode mode = NULL");
                        }

                        (*remoteview)->SetDrawableParameters(viewId, targetBackground, alpha, colorFilter, PorterDuffMode(mode), level);
                        break;
                    }
                    case 2: //ReflectionAction.TAG:
                    {
                        // LOGGERD(TAG, "GetElRemoteViews: found a ReflectionAction action.");
                        String methodName = GetJavaStringField(env, klass, jaction, "methodName", "GetElRemoteViews");
                        if (!methodName.IsNullOrEmpty()) {
                            methodName = methodName.ToUpperCase(0, 1);
                        }
                        Int32 type = GetJavaIntField(env, klass, jaction, "type", "GetElRemoteViews");
                        jfieldID f = env->GetFieldID(klass, "value", "Ljava/lang/Object;");
                        Util::CheckErrorAndLog(env, "GetFieldID: value : %d!\n", __LINE__);
                        jobject jvalue = env->GetObjectField(jaction, f);
                        Util::CheckErrorAndLog(env, "GetObjectField: value : %d!\n", __LINE__);

                        jclass c = env->GetObjectClass(jvalue);
                        switch (type){
                            case 1: // BOOLEAN
                            {
                                Boolean value = (Boolean)GetJavaBoolField(env, jvalue, "value", "GetElRemoteViews");
                                (*remoteview)->SetBoolean(viewId, methodName, value);
                                break;
                            }
                            case 2: // BYTE
                            {
                                Byte value = (Byte)GetJavabyteField(env, c, jvalue, "value", "GetElRemoteViews");
                                (*remoteview)->SetByte(viewId, methodName, value);
                                break;
                            }
                            case 3: // SHORT
                            {
                                Int16 value = (Int16)GetJavaShortField(env, c, jvalue, "value", "GetElRemoteViews");
                                (*remoteview)->SetInt16(viewId, methodName, value);
                                break;
                            }
                            case 4: // INT
                            {
                                Int32 value = (Int32)GetJavaIntField(env, c, jvalue, "value", "GetElRemoteViews");
                                (*remoteview)->SetInt32(viewId, methodName, value);
                                break;
                            }
                            case 5: // LONG
                            {
                                Int64 value = (Int64)GetJavalongField(env, jvalue, "value", "GetElRemoteViews");
                                (*remoteview)->SetInt64(viewId, methodName, value);
                                break;
                            }
                            case 6: // FLOAT
                            {
                                Float value = (Float)GetJavafloatField(env, c, jvalue, "value", "GetElRemoteViews");
                                (*remoteview)->SetFloat(viewId, methodName, value);
                                break;
                            }
                            case 7: // DOUBLE
                            {
                                Double value = (Double)GetJavadoubleField(env, c, jvalue, "value", "GetElRemoteViews");
                                (*remoteview)->SetDouble(viewId, methodName, value);
                                break;
                            }
                            case 8: // CHAR
                            {
                                Char32 value = (Char32)GetJavaCharField(env, c, jvalue, "value", "GetElRemoteViews");
                                (*remoteview)->SetChar(viewId, methodName, value);
                                break;
                            }
                            case 9: // STRING
                            {
                                String value = GetElString(env, (jstring)jvalue);
                                (*remoteview)->SetString(viewId, methodName, value);
                                break;
                            }
                            case 10: // CHAR_SEQUENCE
                            {
                                AutoPtr<ICharSequence> value;
                                GetElCharSequence(env, jvalue, (ICharSequence**)&value);
                                (*remoteview)->SetCharSequence(viewId, methodName, value);
                                break;
                            }
                            case 11: // URI
                            {
                                AutoPtr<IUri> value;
                                GetElUri(env, jvalue, (IUri**)&value);
                                (*remoteview)->SetUri(viewId, methodName, value);
                                break;
                            }
                            // BITMAP actions are never stored in the list of actions. They are only used locally
                            // to implement BitmapReflectionAction, which eliminates duplicates using BitmapCache.
                            case 12: // BITMAP
                            {
                                AutoPtr<IBitmap> value;
                                GetElBitmap(env, jvalue, (IBitmap**)&value);
                                (*remoteview)->SetBitmap(viewId, methodName, value);
                                break;
                            }
                            case 13: // BUNDLE
                            {
                                AutoPtr<IBundle> value;
                                GetElBundle(env, jvalue, (IBundle**)&value);
                                (*remoteview)->SetBundle(viewId, methodName, value);
                                break;
                            }
                            case 14: // INTENT
                            {
                                AutoPtr<IIntent> value;
                                GetElIntent(env, jvalue, (IIntent**)&value);
                                (*remoteview)->SetIntent(viewId, methodName, value);
                                break;
                            }
                            default:
                            break;
                        }
                        env->DeleteLocalRef(jvalue);
                        env->DeleteLocalRef(c);
                        break;
                    }
                    case 4: // ViewGroupAction.TAG:
                    {
                        // LOGGERD(TAG, "GetElRemoteViews: found a ViewGroupAction action.");
                        jfieldID f = env->GetFieldID(klass, "nestedViews", "Landroid/widget/RemoteViews;");
                        Util::CheckErrorAndLog(env, "GetFieldID: nestedViews : %d!\n", __LINE__);
                        jobject jnestedViews = env->GetObjectField(jaction, f);
                        Util::CheckErrorAndLog(env, "GetObjectField: nestedViews : %d!\n", __LINE__);

                        AutoPtr<IRemoteViews> nestedViews;
                        if (jnestedViews != NULL){
                            GetElRemoteViews(env, jnestedViews, (IRemoteViews**)&nestedViews);
                            env->DeleteLocalRef(jnestedViews);
                        }else{
                            LOGGERW(TAG, "GetElRemoteViews: found a ViewGroupAction action. jnestedViews is null");
                        }

                        if (nestedViews == NULL){
                            (*remoteview)->RemoveAllViews(viewId);
                        }else{
                            (*remoteview)->AddView(viewId, nestedViews);
                        }
                        break;
                    }
                    case 5: // ReflectionActionWithoutParams.TAG:
                    {
                        // LOGGERD(TAG, "GetElRemoteViews: found a ReflectionActionWithoutParams action.");
                        String methodName = GetJavaStringField(env, klass, jaction, "methodName", "GetElRemoteViews");
                        if (!methodName.IsNullOrEmpty()) {
                            methodName = methodName.ToUpperCase(0, 1);
                        }

                        if (methodName.Equals(String("ShowNext"))){
                            (*remoteview)->ShowNext(viewId);
                        }else if (methodName.Equals(String("ShowPrevious"))){
                            (*remoteview)->ShowPrevious(viewId);
                        }

                        break;
                    }
                    case 6: // SetEmptyView.TAG:
                    {
                        // LOGGERD(TAG, "GetElRemoteViews: found a SetEmptyView action.");
                        Int32 emptyViewId = GetJavaIntField(env, klass, jaction, "emptyViewId", "GetElRemoteViews");
                        (*remoteview)->SetEmptyView(viewId, emptyViewId);
                        break;
                    }
                    case 8: // SetPendingIntentTemplate.TAG:
                    {
                        // LOGGERD(TAG, "GetElRemoteViews: found a SetPendingIntentTemplate action.");
                        jfieldID f = env->GetFieldID(klass, "pendingIntentTemplate", "Landroid/app/PendingIntent;");
                        Util::CheckErrorAndLog(env, "GetFieldID: pendingIntentTemplate : %d!\n", __LINE__);
                        jobject jpendingIntent = env->GetObjectField(jaction, f);
                        Util::CheckErrorAndLog(env, "GetObjectField: pendingIntentTemplate : %d!\n", __LINE__);
                        AutoPtr<IPendingIntent> pendingIntent;
                        GetElPendingIntent(env, jpendingIntent, (IPendingIntent**)&pendingIntent);
                        env->DeleteLocalRef(jpendingIntent);
                        (*remoteview)->SetPendingIntentTemplate(viewId, pendingIntent);
                        break;
                    }
                    case 9: // SetOnClickFillInIntent.TAG:
                    {
                        // LOGGERD(TAG, "GetElRemoteViews: found a SetOnClickFillInIntent action.");
                        jfieldID f = env->GetFieldID(klass, "fillInIntent", "Landroid/content/Intent;");
                        Util::CheckErrorAndLog(env, "GetFieldID: fillInIntent : %d!\n", __LINE__);
                        jobject jintent = env->GetObjectField(jaction, f);
                        Util::CheckErrorAndLog(env, "GetObjectField: fillInIntent : %d!\n", __LINE__);
                        AutoPtr<IIntent> intent;
                        GetElIntent(env, jintent, (IIntent**)&intent);
                        env->DeleteLocalRef(jintent);
                        (*remoteview)->SetOnClickFillInIntent(viewId, intent);
                        break;
                    }
                    case 10: // SetRemoteViewsAdapterIntent.TAG:
                    {
                        // LOGGERD(TAG, "GetElRemoteViews: found a SetRemoteViewsAdapterIntent action.");
                        jfieldID f = env->GetFieldID(klass, "intent", "Landroid/content/Intent;");
                        Util::CheckErrorAndLog(env, "GetFieldID: intent : %d!\n", __LINE__);
                        jobject jintent = env->GetObjectField(jaction, f);
                        Util::CheckErrorAndLog(env, "GetObjectField: intent : %d!\n", __LINE__);
                        AutoPtr<IIntent> intent;
                        GetElIntent(env, jintent, (IIntent**)&intent);
                        env->DeleteLocalRef(jintent);
                        (*remoteview)->SetRemoteAdapter(viewId, intent);
                        break;
                    }
                    case 11: // TextViewDrawableAction.TAG:
                    {
                        // LOGGERD(TAG, "GetElRemoteViews: found a TextViewDrawableAction action.");
                        Boolean isRelative = GetJavaBoolField(env, jaction, "isRelative", "GetElRemoteViews");
                        Int32 d1 = GetJavaIntField(env, klass, jaction, "d1", "GetElRemoteViews");
                        Int32 d2 = GetJavaIntField(env, klass, jaction, "d2", "GetElRemoteViews");
                        Int32 d3 = GetJavaIntField(env, klass, jaction, "d3", "GetElRemoteViews");
                        Int32 d4 = GetJavaIntField(env, klass, jaction, "d4", "GetElRemoteViews");

                        if (isRelative){
                            (*remoteview)->SetTextViewCompoundDrawablesRelative(viewId, d1, d2, d3, d4);
                        }else{
                            (*remoteview)->SetTextViewCompoundDrawables(viewId, d1, d2, d3, d4);
                        }
                        break;
                    }
                    case 13: // TextViewSizeAction.TAG:
                    {
                        // LOGGERD(TAG, "GetElRemoteViews: found a TextViewSizeAction action.");
                        Int32 units = GetJavaIntField(env, klass, jaction, "units", "GetElRemoteViews");
                        Float size = (Float)GetJavafloatField(env, klass, jaction, "size", "GetElRemoteViews");
                        (*remoteview)->SetTextViewTextSize(viewId, units, size);
                        break;
                    }
                    case 14: // ViewPaddingAction.TAG:
                    {
                        // LOGGERD(TAG, "GetElRemoteViews: found a ViewPaddingAction action.");
                        Int32 left = GetJavaIntField(env, klass, jaction, "left", "GetElRemoteViews");
                        Int32 top = GetJavaIntField(env, klass, jaction, "top", "GetElRemoteViews");
                        Int32 right = GetJavaIntField(env, klass, jaction, "right", "GetElRemoteViews");
                        Int32 bottom = GetJavaIntField(env, klass, jaction, "bottom", "GetElRemoteViews");
                        (*remoteview)->SetViewPadding(viewId, left, top, right, bottom);
                        break;
                    }
                    case 12: // BitmapReflectionAction.TAG:
                    {
                        // LOGGERD(TAG, "GetElRemoteViews: found a BitmapReflectionAction action.");
                        String methodName = GetJavaStringField(env, klass, jaction, "methodName", "GetElRemoteViews");
                        if (!methodName.IsNullOrEmpty()) {
                            methodName = methodName.ToUpperCase(0, 1);
                        }

                        jfieldID f = env->GetFieldID(klass, "bitmap", "Landroid/graphics/Bitmap;");
                        Util::CheckErrorAndLog(env, "GetFieldID: bitmap : %d!\n", __LINE__);
                        jobject jbitmap = env->GetObjectField(jaction, f);
                        Util::CheckErrorAndLog(env, "GetObjectField: bitmap : %d!\n", __LINE__);
                        AutoPtr<IBitmap> bitmap;
                        GetElBitmap(env, jbitmap, (IBitmap**)&bitmap);
                        env->DeleteLocalRef(jbitmap);

                        (*remoteview)->SetBitmap(viewId, methodName, bitmap);
                        break;
                    }
                    case 15: // SetRemoteViewsAdapterList.TAG:
                    {
                        // LOGGERD(TAG, "GetElRemoteViews: found a SetRemoteViewsAdapterList action.");
                        Int32 viewTypeCount = GetJavaIntField(env, klass, jaction, "viewTypeCount", "GetElRemoteViews");

                        jfieldID f = env->GetFieldID(klass, "list", "Ljava/util/ArrayList;");
                        Util::CheckErrorAndLog(env, "GetFieldID: list : %d!\n", __LINE__);
                        jobject jlist = env->GetObjectField(jaction, f);
                        Util::CheckErrorAndLog(env, "GetObjectField: list : %d!\n", __LINE__);
                        AutoPtr<IArrayList> list;
                        if (jlist != NULL){
                            jint jsize = env->CallIntMethod(jlist, mSize);
                            CheckErrorAndLog(env, "CallIntMethod: list : %d!\n", __LINE__);
                            if (jsize > 0)
                                CArrayList::New((IArrayList**)&list);
                            for (jint j = 0; j < jsize; j++) {
                                jobject jitem = env->CallObjectMethod(jlist, mGet, j);
                                CheckErrorAndLog(env, "CallObjectMethod: get : %d!\n", __LINE__);
                                AutoPtr<IRemoteViews> item;
                                if (GetElRemoteViews(env, jitem, (IRemoteViews**)&item)){
                                    list->Add(item);
                                }
                                else {
                                    LOGGERE(TAG, "GetElRemoteViews SetRemoteViewsAdapterList.list item%d to IRemoteViews failed", j);
                                }
                                env->DeleteLocalRef(jitem);
                            }
                            env->DeleteLocalRef(jlist);
                        }

                        (*remoteview)->SetRemoteAdapter(viewId, list, viewTypeCount);
                        break;
                    }
                    case 17: // TextViewDrawableColorFilterAction.TAG:
                    {
                        // LOGGERD(TAG, "GetElRemoteViews: found a TextViewDrawableColorFilterAction action.");
                        Boolean isRelative = GetJavaBoolField(env, jaction, "isRelative", "GetElRemoteViews");
                        if (!isRelative)
                            LOGGERE(TAG, "GetElRemoteViews: TextViewDrawableColorFilterAction isRelative is FALSE!");
                        Int32 index = GetJavaIntField(env, klass, jaction, "index", "GetElRemoteViews");
                        Int32 color = GetJavaIntField(env, klass, jaction, "color", "GetElRemoteViews");

                        jfieldID modeField = env->GetFieldID(klass, "mode", "Landroid/graphics/PorterDuff$Mode;");
                        Util::CheckErrorAndLog(env, "GetFieldID: mode : %d!\n", __LINE__);
                        jobject jmode = env->GetObjectField(jaction, modeField);
                        Util::CheckErrorAndLog(env, "GetObjectField: mode : %d!\n", __LINE__);

                        Int32 mode = -1;
                        if (jmode != NULL) {
                            jclass c = env->GetObjectClass(jmode);
                            mode = GetJavaIntField(env, c, jmode, "nativeInt", "GetElRemoteViews");
                            env->DeleteLocalRef(c);
                            env->DeleteLocalRef(jmode);
                            LOGGERE(TAG, "GetElRemoteViews, TextViewDrawableColorFilterAction, PorterDuff.Mode mode = %d", mode);
                        }
                        else {
                            LOGGERE(TAG, "GetElRemoteViews, TextViewDrawableColorFilterAction, PorterDuff.Mode mode = NULL");
                        }

                        (*remoteview)->SetTextViewCompoundDrawablesRelativeColorFilter(viewId, index, color, PorterDuffMode(mode));
                        break;
                    }
                    default:
                        LOGGERE(TAG, "GetElRemoteViews action.tag = %d", tag);
                        break;
                        // throw new ActionException("Tag " + tag + " not found");
                }

                env->DeleteLocalRef(jaction);
                env->DeleteLocalRef(klass);
            }
        }

        env->DeleteLocalRef(jactions);
    }
    else{
        LOGGERW(TAG, "GetElRemoteViews, mActions is null");
    }

    env->DeleteLocalRef(remoteviewClass);

    return TRUE;
}

jobject Util::ToJavaRemoteViews(
    /* [in] */ JNIEnv* env,
    /* [in] */ IRemoteViews* obj)
{
    if (env == NULL || obj == NULL) {
        LOGGERE("Util::ToJavaRemoteViews:", " Invalid argumenet!");
        return NULL;
    }

    AutoPtr<IParcel> source;
    CParcel::New((IParcel**)&source);

    AutoPtr<IParcelable> parcelable = IParcelable::Probe(obj);
    assert(parcelable != NULL);
    parcelable->WriteToParcel(source);

    // Try to read the values to init java remoteviews
    source->SetDataPosition(0);
    Int32 mode = 0;
    source->ReadInt32(&mode);

    if (mode != 0) {
        LOGGERE("Util::ToJavaRemoteViews()", "read mode = %d", mode);
        assert(0 && "TODO");
        return NULL;
    }

    // Read bitmapcache
    AutoPtr<IBitmapCache> bitmapCache;
    CRemoteViewsBitmapCache::New(source, (IBitmapCache**)&bitmapCache);

    AutoPtr<IApplicationInfo> application;
    Int32 layoutId = 0;
    source->ReadInterfacePtr((Handle32*)&application);
    source->ReadInt32(&layoutId);

    jobject japplication = ToJavaApplicationInfo(env, application);

    jclass klass = env->FindClass("android/widget/RemoteViews");
    Util::CheckErrorAndLog(env, "ToJavaRemoteViews", "Fail FindClass: RemoteViews : %d!\n", __LINE__);

    jmethodID m = env->GetMethodID(klass, "<init>", "(Landroid/content/pm/ApplicationInfo;I)V");
    Util::CheckErrorAndLog(env, "ToJavaRemoteViews", "Fail GetMethodID: RemoteViews() : %d!\n", __LINE__);

    jobject jremoteview = env->NewObject(klass, m, japplication, (jint)layoutId);
    Util::CheckErrorAndLog(env, "ToJavaRemoteViews", "Fail NewObject: RemoteViews : %d!\n", __LINE__);

    env->DeleteLocalRef(japplication);

    Int32 tmp;
    source->ReadInt32(&tmp);
    // Set bool field mIsWidgetCollectionChild
    SetJavaBoolField(env, klass, jremoteview, tmp == 1, "mIsWidgetCollectionChild", "ToJavaRemoteViews");

    Int32 count = 0;
    source->ReadInt32(&count);
    // LOGGERD("Util::ToJavaRemoteViews()", "read action count = %d", count);
    if (count > 0) {
        for (Int32 i = 0; i < count; i++) {
            Int32 tag = 0;
            source->ReadInt32(&tag);
            switch (tag) {
                case 1: // RemoteViewsSetOnClickPendingIntent::TAG:
                {
                    // LOGGERD("Util::ToJavaRemoteViews()", "read SetOnClickPendingIntent action");
                    Int32 mViewId = 0;
                    source->ReadInt32(&mViewId);

                    AutoPtr<IInterface> item;
                    AutoPtr<IPendingIntent> pendingIntent;
                    Int32 temp = 0;
                    source->ReadInt32(&temp);

                    if (temp == 1){
                        source->ReadInterfacePtr((Handle32*)&item);
                    }

                    if (item != NULL){
                        pendingIntent = IPendingIntent::Probe(item);
                        jobject jpendingIntent = ToJavaPendingIntent(env, pendingIntent);
                        jmethodID method = env->GetMethodID(klass, "setOnClickPendingIntent", "(ILandroid/app/PendingIntent;)V");
                        CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "GetJavaIntegerField(): FindMethod: setOnClickPendingIntent: %d!\n", __LINE__);
                        env->CallVoidMethod(jremoteview, method, (jint)mViewId, jpendingIntent);
                        CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "GetJavaIntegerField: call method setOnClickPendingIntent: %d!\n", __LINE__);
                        env->DeleteLocalRef(jpendingIntent);
                    }
                    break;
                }
                case 3: // RemoteViewsSetDrawableParameters::TAG:
                {
                    // LOGGERD("Util::ToJavaRemoteViews()", "read setDrawableParameters action");
                    Int32 mViewId = 0;
                    Int32 mAlpha = 0;
                    Int32 mColorFilter = 0;
                    Int32 mLevel = 0;

                    source->ReadInt32(&mViewId);
                    Int32 background = 0;
                    source->ReadInt32(&background);
                    Boolean mTargetBackground = background != 0;
                    source->ReadInt32(&mAlpha);
                    source->ReadInt32(&mColorFilter);
                    Int32 mode = 0;
                    source->ReadInt32(&mode);

                    jobject jPorterDuffMode = ToJavaPorterDuffMode(env, mode);
                    jmethodID method = env->GetMethodID(klass, "setDrawableParameters", "(IZIILandroid/graphics/PorterDuff$Mode;I)V");
                    CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews(): FindMethod: setDrawableParameters: %d!\n", __LINE__);
                    env->CallVoidMethod(jremoteview, method, (jint)mViewId, (jboolean)mTargetBackground, mAlpha, mColorFilter, jPorterDuffMode, mLevel);
                    CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews: call method setDrawableParameters: %d!\n", __LINE__);
                    env->DeleteLocalRef(jPorterDuffMode);
                    break;
                }
                case 2: // ReflectionAction::TAG:
                {
                    // LOGGERD("Util::ToJavaRemoteViews()", "read ReflectionAction action");
                    Int32 mViewId = 0;
                    String mMethodName;
                    Int32 mType = 0;
                    source->ReadInt32(&mViewId);
                    source->ReadString(&mMethodName);
                    if (!mMethodName.IsNullOrEmpty()) {
                        mMethodName = mMethodName.ToLowerCase(0, 1);
                    }
                    source->ReadInt32(&mType);

                    if (!mMethodName.IsNullOrEmpty()) {
                        mMethodName = mMethodName.ToLowerCase(0, 1);
                    }

                    jstring jMethodName = ToJavaString(env, mMethodName);
                    // LOGGERD("Util::ToJavaRemoteViews()", "read ReflectionAction action, type = %d", mType);
                    switch (mType) {
                        case 1: // BOOLEAN:
                        {
                            Int32 res = 0;
                            source->ReadInt32(&res);
                            Boolean result = FALSE;
                            result = res != 0;

                            jmethodID method = env->GetMethodID(klass, "setBoolean", "(ILjava/lang/String;Z)V");
                            CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews(): FindMethod: setBoolean: %d!\n", __LINE__);
                            env->CallVoidMethod(jremoteview, method, (jint)mViewId, jMethodName, (jboolean)result);
                            CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews: call method setBoolean: %d!\n", __LINE__);
                            break;
                        }
                        case 2: // BYTE:
                        {
                            Byte byte;
                            source->ReadByte(&byte);
                            jmethodID method = env->GetMethodID(klass, "setByte", "(ILjava/lang/String;B)V");
                            CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews(): FindMethod: setByte: %d!\n", __LINE__);
                            env->CallVoidMethod(jremoteview, method, (jint)mViewId, jMethodName, (jbyte)byte);
                            CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews: call method setByte: %d!\n", __LINE__);
                            break;
                        }
                        case 3: // SHORT:
                        {
                            Int16 s = 0;
                            source->ReadInt16(&s);
                            jmethodID method = env->GetMethodID(klass, "setShort", "(ILjava/lang/String;S)V");
                            CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews(): FindMethod: setShort: %d!\n", __LINE__);
                            env->CallVoidMethod(jremoteview, method, (jint)mViewId, jMethodName, (jshort)s);
                            CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews: call method setShort: %d!\n", __LINE__);
                            break;
                        }
                        case 4: // INT:
                        {
                            Int32 i = 0;
                            source->ReadInt32(&i);
                            jmethodID method = env->GetMethodID(klass, "setInt", "(ILjava/lang/String;I)V");
                            CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews(): FindMethod: setInt: %d!\n", __LINE__);
                            env->CallVoidMethod(jremoteview, method, (jint)mViewId, jMethodName, (jint)i);
                            CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews: call method setInt: %d!\n", __LINE__);
                            break;
                        }
                        case 5: // LONG:
                        {
                            Int64 l = 0;
                            source->ReadInt64(&l);
                            jmethodID method = env->GetMethodID(klass, "setLong", "(ILjava/lang/String;J)V");
                            CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews(): FindMethod: setLong: %d!\n", __LINE__);
                            env->CallVoidMethod(jremoteview, method, (jint)mViewId, jMethodName, (jlong)l);
                            CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews: call method setLong: %d!\n", __LINE__);
                            break;
                        }
                        case 6: // FLOAT:
                        {
                            Float f = 0;
                            source->ReadFloat(&f);
                            jmethodID method = env->GetMethodID(klass, "setFloat", "(ILjava/lang/String;F)V");
                            CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews(): FindMethod: setFloat: %d!\n", __LINE__);
                            env->CallVoidMethod(jremoteview, method, (jint)mViewId, jMethodName, (jfloat)f);
                            CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews: call method setFloat: %d!\n", __LINE__);
                            break;
                        }
                        case 7: // DOUBLE:
                        {
                            Double d = 0;
                            source->ReadDouble(&d);
                            jmethodID method = env->GetMethodID(klass, "setDouble", "(ILjava/lang/String;D)V");
                            CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews(): FindMethod: setDouble: %d!\n", __LINE__);
                            env->CallVoidMethod(jremoteview, method, (jint)mViewId, jMethodName, (jdouble)d);
                            CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews: call method setDouble: %d!\n", __LINE__);
                            break;
                        }
                        case 8: // CHAR:
                        {
                            Int32 c;
                            source->ReadInt32(&c);
                            Char32 ch = (Char32)c;
                            jmethodID method = env->GetMethodID(klass, "setChar", "(ILjava/lang/String;C)V");
                            CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews(): FindMethod: setChar: %d!\n", __LINE__);
                            env->CallVoidMethod(jremoteview, method, (jint)mViewId, jMethodName, (jchar)ch);
                            CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews: call method setChar: %d!\n", __LINE__);
                            break;
                        }
                        case 9: // STRING:
                        {
                            String str;
                            source->ReadString(&str);
                            jstring jstr = ToJavaString(env, str);
                            jmethodID method = env->GetMethodID(klass, "setString", "(ILjava/lang/String;Ljava/lang/String;)V");
                            CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews(): FindMethod: setString: %d!\n", __LINE__);
                            env->CallVoidMethod(jremoteview, method, (jint)mViewId, jMethodName, jstr);
                            CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews: call method setString: %d!\n", __LINE__);
                            env->DeleteLocalRef(jstr);
                            break;
                        }
                        case 10: // CHAR_SEQUENCE:
                        {
                            AutoPtr<ICharSequence> charCsq;
                            TextUtils::CHAR_SEQUENCE_CREATOR::CreateFromParcel(source, (ICharSequence**)&charCsq);
                            String str;
                            charCsq->ToString(&str);
                            jstring jstr = ToJavaString(env, str);
                            jmethodID method = env->GetMethodID(klass, "setCharSequence", "(ILjava/lang/String;Ljava/lang/CharSequence;)V");
                            CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews(): FindMethod: setCharSequence: %d!\n", __LINE__);
                            env->CallVoidMethod(jremoteview, method, (jint)mViewId, jMethodName, jstr);
                            CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews: call method setCharSequence: %d!\n", __LINE__);
                            env->DeleteLocalRef(jstr);
                            break;
                        }
                        case 11: // URI:
                        {
                            Int32 flag = 0;
                            source->ReadInt32(&flag);
                            if (flag != 0) {
                                AutoPtr<IUri> uri;
                                source->ReadInterfacePtr((Handle32*)&uri);
                                if (uri != NULL){
                                    jobject jUri = ToJavaUri(env, uri);
                                    jmethodID method = env->GetMethodID(klass, "setUri", "(ILjava/lang/String;Landroid/net/Uri;)V");
                                    CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews(): FindMethod: setUri: %d!\n", __LINE__);
                                    env->CallVoidMethod(jremoteview, method, (jint)mViewId, jMethodName, jUri);
                                    CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews: call method setUri: %d!\n", __LINE__);
                                    env->DeleteLocalRef(jUri);
                                }
                                else{
                                    LOGGERE("Util::ToJavaRemoteViews()", "read uri null");
                                }
                            }
                            break;
                        }
                        case 12: // BITMAP:
                        {
                            Int32 bitmap = 0;
                            source->ReadInt32(&bitmap);
                            if (bitmap != 0) {
                                AutoPtr<IBitmap> bmp;
                                CBitmap::New((IBitmap**)&bmp);
                                AutoPtr<IParcelable> bitmapParcel = IParcelable::Probe(bmp);
                                bitmapParcel->ReadFromParcel(source);

                                jobject jbitmap = ToJavaBitmap(env, bmp);
                                jmethodID method = env->GetMethodID(klass, "setBitmap", "(ILjava/lang/String;Landroid/graphics/Bitmap;)V");
                                CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews(): FindMethod: setBitmap: %d!\n", __LINE__);
                                env->CallVoidMethod(jremoteview, method, (jint)mViewId, jMethodName, jbitmap);
                                CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews: call method setBitmap: %d!\n", __LINE__);
                                env->DeleteLocalRef(jbitmap);
                            }
                            break;
                        }
                        case 13: // BUNDLE:
                        {
                            AutoPtr<IBundle> bundle;
                            CBundle::New((IBundle**)&bundle);
                            // AutoPtr<CBundle> cBundle = (CBundle*)bundle.Get();
                            AutoPtr<IParcelable> bundleParcel = IParcelable::Probe(bundle);
                            bundleParcel->ReadFromParcel(source);
                            jobject jbundle = ToJavaBundle(env, bundle);
                            jmethodID method = env->GetMethodID(klass, "setBundle", "(ILjava/lang/String;Landroid/os/Bundle;)V");
                            CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews(): FindMethod: setBundle: %d!\n", __LINE__);
                            env->CallVoidMethod(jremoteview, method, (jint)mViewId, jMethodName, jbundle);
                            CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews: call method setBundle: %d!\n", __LINE__);
                            env->DeleteLocalRef(jbundle);
                            break;
                        }
                        case 14: // INTENT:
                        {
                            Int32 intent = 0;
                            source->ReadInt32(&intent);
                            if (intent) {
                                AutoPtr<IIntent> intent;
                                CIntent::New((IIntent**)&intent);
                                AutoPtr<IParcelable> parcel = IParcelable::Probe(intent);
                                // AutoPtr<CIntent> cIntent = (CIntent*)intent.Get();
                                parcel->ReadFromParcel(source);
                                jobject jintent = ToJavaIntent(env, intent);
                                jmethodID method = env->GetMethodID(klass, "setIntent", "(ILjava/lang/String;Landroid/content/Intent;)V");
                                CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews(): FindMethod: setIntent: %d!\n", __LINE__);
                                env->CallVoidMethod(jremoteview, method, (jint)mViewId, jMethodName, jintent);
                                CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews: call method setIntent: %d!\n", __LINE__);
                                env->DeleteLocalRef(jintent);
                            }
                            break;
                        }
                        default:
                            break;
                    }

                    env->DeleteLocalRef(jMethodName);
                    break;
                }
                case 4: // ViewGroupAction::TAG:
                {
                    LOGGERE("Util::ToJavaRemoteViews()", "read ViewGroupAction action, not finish!");
                    Int32 viewId;
                    source->ReadInt32(&viewId);
                    Int32 res = 0;
                    source->ReadInt32(&res);
                    Boolean nestedViewsNull = res == 0;
                    if (!nestedViewsNull) {
                        // addView
                        LOGGERE("Util::ToJavaRemoteViews()", "ToJavaRemoteViews(), read ViewGroupAction action not finished, Please check RemoteViews::ViewGroupAction::WriteToParcel()");

                        AutoPtr<IRemoteViews> remoteviews;
                        // Need to read a remoteview here
                        jobject jremoteviews = ToJavaRemoteViews(env, remoteviews);

                        if (jremoteviews != NULL){
                            jmethodID method = env->GetMethodID(klass, "addView", "(ILandroid/widget/RemoteViews;)V");
                            CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews(): FindMethod: addView: %d!\n", __LINE__);
                            env->CallVoidMethod(jremoteview, method, (jint)viewId, jremoteviews);
                            CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews: call method addView: %d!\n", __LINE__);

                            env->DeleteLocalRef(jremoteviews);
                        }
                    }
                    else {
                        // removeAllViews
                        jmethodID method = env->GetMethodID(klass, "removeAllViews", "(I)V");
                        CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews(): FindMethod: removeAllViews: %d!\n", __LINE__);
                        env->CallVoidMethod(jremoteview, method, (jint)viewId);
                        CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews: call method removeAllViews: %d!\n", __LINE__);
                    }

                    break;
                }
                case 5: // ReflectionActionWithoutParams::TAG:
                {
                    // LOGGERD("Util::ToJavaRemoteViews()", "read ReflectionActionWithoutParams action");
                    Int32 viewId;
                    String methodName;
                    source->ReadInt32(&viewId);
                    source->ReadString(&methodName);

                    if (!methodName.IsNullOrEmpty()) {
                        methodName = methodName.ToLowerCase(0, 1);
                    }

                    jclass jrawp = env->FindClass("android/widget/RemoteViews$ReflectionActionWithoutParams");
                    CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews GetMethodID: ReflectionActionWithoutParams(I,String): %d!\n", __LINE__);
                    jmethodID mconstructor = env->GetMethodID(jrawp, "<init>", "(II)V");
                    CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews GetMethodID: ReflectionActionWithoutParams(I,String) : %d!\n", __LINE__);
                    jstring jMethodName = ToJavaString(env, methodName);
                    jobject jaction = env->NewObject(jrawp, mconstructor, (jint)viewId, jMethodName);
                    CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews NewObject: ReflectionActionWithoutParams : %d!\n", __LINE__);
                    jmethodID method = env->GetMethodID(klass, "addAction", "(android/widget/RemoteViews$Action)V");
                    CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews(): FindMethod: addAction: %d!\n", __LINE__);
                    env->CallVoidMethod(jremoteview, method, jaction);
                    CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews: call method addAction: %d!\n", __LINE__);
                    env->DeleteLocalRef(jrawp);
                    env->DeleteLocalRef(jMethodName);
                    env->DeleteLocalRef(jaction);
                    break;
                }
                case 6: // RemoteViewsSetEmptyView::TAG:
                {
                    // LOGGERD("Util::ToJavaRemoteViews()", "read RemoteViewsSetEmptyView action");
                    Int32 viewId;
                    Int32 emptyViewId;
                    source->ReadInt32(&viewId);
                    source->ReadInt32(&emptyViewId);

                    jmethodID method = env->GetMethodID(klass, "setEmptyView", "(II)V");
                    CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "setEmptyView(): FindMethod: setEmptyView: %d!\n", __LINE__);
                    env->CallVoidMethod(jremoteview, method, (jint)viewId, (jint)emptyViewId);
                    CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews: call method setEmptyView: %d!\n", __LINE__);
                    break;
                }
                case 8: // RemoteViewsSetPendingIntentTemplate::TAG:
                {
                    // LOGGERD("Util::ToJavaRemoteViews()", "read RemoteViewsSetPendingIntentTemplate action");
                    Int32 viewId;
                    source->ReadInt32(&viewId);
                    AutoPtr<IPendingIntent> pendingIntent;
                    source->ReadInterfacePtr((Handle32*)&pendingIntent);
                    jobject jpendingIntent = ToJavaPendingIntent(env, pendingIntent);
                    jmethodID method = env->GetMethodID(klass, "setPendingIntentTemplate", "(ILandroid/app/PendingIntent;)V");
                    CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews(): FindMethod: setPendingIntentTemplate: %d!\n", __LINE__);
                    env->CallVoidMethod(jremoteview, method, (jint)viewId, jpendingIntent);
                    CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews: call method setPendingIntentTemplate: %d!\n", __LINE__);
                    env->DeleteLocalRef(jpendingIntent);
                    break;
                }
                case 9: // RemoteViewsSetOnClickFillInIntent::TAG:
                {
                    // LOGGERD("Util::ToJavaRemoteViews()", "read RemoteViewsSetOnClickFillInIntent action");

                    Int32 viewId;
                    source->ReadInt32(&viewId);
                    AutoPtr<IIntent> intent;
                    source->ReadInterfacePtr((Handle32*)&intent);
                    jobject jintent = ToJavaIntent(env, intent);
                    jmethodID method = env->GetMethodID(klass, "setOnClickFillInIntent", "(ILandroid/content/Intent;)V");
                    CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews(): FindMethod: setOnClickFillInIntent: %d!\n", __LINE__);
                    env->CallVoidMethod(jremoteview, method, (jint)viewId, jintent);
                    CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews: call method setOnClickFillInIntent: %d!\n", __LINE__);
                    env->DeleteLocalRef(jintent);
                    break;
                }
                case 10: // SetRemoteViewsAdapterIntent::TAG:
                {
                    // LOGGERD("Util::ToJavaRemoteViews()", "read SetRemoteViewsAdapterIntent action");
                    Int32 viewId;
                    source->ReadInt32(&viewId);
                    AutoPtr<IIntent> intent;
                    source->ReadInterfacePtr((Handle32*)&intent);
                    jobject jintent = ToJavaIntent(env, intent);
                    jmethodID method = env->GetMethodID(klass, "setRemoteAdapter", "(ILandroid/content/Intent;)V");
                    CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews(): FindMethod: setRemoteAdapter: %d!\n", __LINE__);
                    env->CallVoidMethod(jremoteview, method, (jint)viewId, jintent);
                    CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews: call method setRemoteAdapter: %d!\n", __LINE__);
                    env->DeleteLocalRef(jintent);
                    break;
                }
                case 11: // TextViewDrawableAction::TAG:
                {
                    // LOGGERD("Util::ToJavaRemoteViews()", "read TextViewDrawableAction action");
                    Int32 viewId;
                    source->ReadInt32(&viewId);
                    Int32 res = 0;
                    source->ReadInt32(&res);
                    Boolean isRelative = res != 0;
                    Int32 d1, d2, d3, d4;
                    source->ReadInt32(&d1);
                    source->ReadInt32(&d2);
                    source->ReadInt32(&d3);
                    source->ReadInt32(&d4);

                    jmethodID method;
                    if (isRelative){
                        method = env->GetMethodID(klass, "setTextViewCompoundDrawablesRelative", "(IIIII)V");
                        CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews(): FindMethod: setTextViewCompoundDrawablesRelative: %d!\n", __LINE__);
                    }else{
                        method = env->GetMethodID(klass, "setTextViewCompoundDrawables", "(IIIII)V");
                        CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews(): FindMethod: setTextViewCompoundDrawables: %d!\n", __LINE__);
                    }

                    env->CallVoidMethod(jremoteview, method, (jint)viewId, (jint)d1, (jint)d2, (jint)d3, (jint)d4);
                    CheckErrorAndLog(env, "Util::ToJavaRemoteViews()", "ToJavaRemoteViews: call method setRemoteAdapter: %d!\n", __LINE__);
                    break;
                }
                case 13: // TextViewSizeAction::TAG:
                {
                    // LOGGERD("Util::ToJavaRemoteViews()", "read TextViewSizeAction action");
                    Int32 viewId;
                    Int32 units;
                    Float size;
                    source->ReadInt32(&viewId);
                    source->ReadInt32(&units);
                    source->ReadFloat(&size);

                    jmethodID method = env->GetMethodID(klass, "setTextViewTextSize", "(IIF)V");
                    CheckErrorAndLog(env, "ToJavaRemoteViews(): FindMethod: setTextViewTextSize: %d!\n", __LINE__);
                    env->CallVoidMethod(jremoteview, method, (jint)viewId, (jint)units, (jfloat)size);
                    CheckErrorAndLog(env, "ToJavaRemoteViews: call method setTextViewTextSize: %d!\n", __LINE__);
                    break;
                }
                case 14: // ViewPaddingAction::TAG:
                {
                    LOGGERD("Util::ToJavaRemoteViews()", "read ViewPaddingAction action");
                    Int32 viewId;
                    Int32 left;
                    Int32 top;
                    Int32 right;
                    Int32 bottom;
                    source->ReadInt32(&viewId);
                    source->ReadInt32(&left);
                    source->ReadInt32(&top);
                    source->ReadInt32(&right);
                    source->ReadInt32(&bottom);
                    jmethodID method = env->GetMethodID(klass, "setViewPadding", "(IIIII)V");
                    CheckErrorAndLog(env, "ToJavaRemoteViews(): FindMethod: setViewPadding: %d!\n", __LINE__);
                    env->CallVoidMethod(jremoteview, method, (jint)viewId, (jint)left, (jint)top, (jint)right, (jint)bottom);
                    CheckErrorAndLog(env, "ToJavaRemoteViews: call method setViewPadding: %d!\n", __LINE__);
                    break;
                }
                case 12: // BitmapReflectionAction::TAG:
                {
                    // LOGGERE("Util::ToJavaRemoteViews()", "read BitmapReflectionAction action, not implemented");
                    Int32 viewId;
                    String methodName;
                    Int32 bitmapId;
                    source->ReadInt32(&viewId);
                    source->ReadString(&methodName);

                    if (!methodName.IsNullOrEmpty()) {
                        methodName = methodName.ToLowerCase(0, 1);
                    }

                    source->ReadInt32(&bitmapId);

                    if (bitmapCache != NULL){
                        AutoPtr<IBitmap> bitmap;
                        bitmapCache->GetBitmapForId(bitmapId, (IBitmap**)&bitmap);

                        if (bitmap != NULL){
                            LOGGERE("Util::ToJavaRemoteViews()", "ToJavaRemoteViews(), read BitmapReflectionAction action, Found bitmap in bitmapCache");
                            jobject jbitmap = ToJavaBitmap(env, bitmap);
                            jstring jmethodName = ToJavaString(env, methodName);

                            jmethodID method = env->GetMethodID(klass, "setBitmap", "(ILjava/lang/String;Landroid/graphics/Bitmap;)V");
                            CheckErrorAndLog(env, "ToJavaRemoteViews(): FindMethod: setBitmap: %d!\n", __LINE__);
                            env->CallVoidMethod(jremoteview, method, (jint)viewId, jmethodName, jbitmap);
                            CheckErrorAndLog(env, "ToJavaRemoteViews: call method setBitmap: %d!\n", __LINE__);

                            env->DeleteLocalRef(jbitmap);
                            env->DeleteLocalRef(jmethodName);
                        }else{
                            LOGGERE("Util::ToJavaRemoteViews()", "ToJavaRemoteViews(), read BitmapReflectionAction action, did not found bitmap in bitmapCache");
                        }
                    }else{
                        LOGGERE("Util::ToJavaRemoteViews()", "ToJavaRemoteViews(), read BitmapReflectionAction action, But bitmapCache is null");
                    }

                    break;
                }
                case 15: // SetRemoteViewsAdapterList::TAG:
                {
                    jclass listKlass = env->FindClass("java/util/ArrayList");
                    Util::CheckErrorAndLog(env, "ToJavaRemoteViews", "NewObject: ArrayList line: %d", __LINE__);

                    jmethodID m = env->GetMethodID(listKlass, "<init>", "()V");
                    Util::CheckErrorAndLog(env, "ToJavaRemoteViews", "GetMethodID: ArrayList line: %d", __LINE__);

                    jobject jlist = env->NewObject(listKlass, m);
                    Util::CheckErrorAndLog(env, "ToJavaRemoteViews", "NewObject: ArrayList line: %d", __LINE__);

                    jmethodID mAdd = env->GetMethodID(listKlass, "add", "(Ljava/lang/Object;)Z");
                    Util::CheckErrorAndLog(env, "ToJavaRemoteViews", "GetMethodID: add line: %d", __LINE__);

                    Int32 viewId, viewTypeCount;
                    source->ReadInt32(&viewId);
                    source->ReadInt32(&viewTypeCount);
                    Int32 count;
                    source->ReadInt32(&count);
                    for (Int32 i = 0; i < count; i++) {
                        AutoPtr<IRemoteViews> rv;
                        CRemoteViews::New((IRemoteViews**)&rv);
                        IParcelable::Probe(rv)->ReadFromParcel(source);
                        jobject jrv = ToJavaRemoteViews(env, rv);
                        env->CallBooleanMethod(jlist, mAdd, jrv);
                        Util::CheckErrorAndLog(env, "ToJavaRemoteViews", "CallBooleanMethod: add line: %d", __LINE__);
                        env->DeleteLocalRef(jrv);
                    }
                    jmethodID method = env->GetMethodID(klass, "setRemoteAdapter", "(ILjava/util/ArrayList;I)V");
                    CheckErrorAndLog(env, "ToJavaRemoteViews(): FindMethod: setRemoteAdapter: %d!\n", __LINE__);
                    env->CallVoidMethod(jremoteview, method, (jint)viewId, jlist, (jint)viewTypeCount);
                    CheckErrorAndLog(env, "ToJavaRemoteViews: call method setRemoteAdapter: %d!\n", __LINE__);
                    env->DeleteLocalRef(listKlass);
                    env->DeleteLocalRef(jlist);
                    break;
                }
                case 17: // TextViewDrawableColorFilterAction::TAG:
                {
                    Int32 viewId, index, color, mode;
                    source->ReadInt32(&viewId);
                    Int32 isRelative;
                    source->ReadInt32(&isRelative);
                    source->ReadInt32(&index);
                    source->ReadInt32(&color);
                    source->ReadInt32(&mode);
                    jobject jPorterDuffMode = ToJavaPorterDuffMode(env, mode);
                    jmethodID method = env->GetMethodID(klass, "setTextViewCompoundDrawablesRelativeColorFilter",
                        "(IIILandroid/graphics/PorterDuff$Mode;)V");
                    CheckErrorAndLog(env, "ToJavaRemoteViews(): FindMethod: setTextViewCompoundDrawablesRelativeColorFilter: %d!\n", __LINE__);
                    env->CallVoidMethod(jremoteview, method, (jint)viewId, (jint)index, (jint)color, jPorterDuffMode);
                    CheckErrorAndLog(env, "ToJavaRemoteViews: call method setTextViewCompoundDrawablesRelativeColorFilter: %d!\n", __LINE__);
                    env->DeleteLocalRef(jPorterDuffMode);
                    break;
                }
                default:
                    break;
            }
        }
    }

    env->DeleteLocalRef(klass);
    return jremoteview;
}

jobject Util::ToJavaPorterDuffMode(
    /* [in] */ JNIEnv* env,
    /* [in] */ PorterDuffMode mode)
{
    jclass cPorterDuffMode = env->FindClass("android/graphics/PorterDuff$Mode");
    CheckErrorAndLog(env, "Util::ToJavaPorterDuffMode()", "Fail FindClass: PorterDuff$Mode : %d!\n", __LINE__);
    jfieldID f = NULL;
    switch (mode) {
        case Elastos::Droid::Graphics::PorterDuffMode_NONE:
            f = env->GetStaticFieldID(cPorterDuffMode, "NONE", "Landroid/graphics/PorterDuff$Mode;");
            CheckErrorAndLog(env, "Util::ToJavaPorterDuffMode()", "Fail GetStaticFieldID: NONE : %d!\n", __LINE__);
            break;
        case Elastos::Droid::Graphics::PorterDuffMode_CLEAR:
            f = env->GetStaticFieldID(cPorterDuffMode, "CLEAR", "Landroid/graphics/PorterDuff$Mode;");
            CheckErrorAndLog(env, "Util::ToJavaPorterDuffMode()", "Fail GetStaticFieldID: CLEAR : %d!\n", __LINE__);
            break;
        case Elastos::Droid::Graphics::PorterDuffMode_SRC:
            f = env->GetStaticFieldID(cPorterDuffMode, "SRC", "Landroid/graphics/PorterDuff$Mode;");
            CheckErrorAndLog(env, "Util::ToJavaPorterDuffMode()", "Fail GetStaticFieldID: SRC : %d!\n", __LINE__);
            break;
        case Elastos::Droid::Graphics::PorterDuffMode_DST:
            f = env->GetStaticFieldID(cPorterDuffMode, "DST", "Landroid/graphics/PorterDuff$Mode;");
            CheckErrorAndLog(env, "Util::ToJavaPorterDuffMode()", "Fail GetStaticFieldID: DST : %d!\n", __LINE__);
            break;
        case Elastos::Droid::Graphics::PorterDuffMode_SRC_OVER:
            f = env->GetStaticFieldID(cPorterDuffMode, "SRC_OVER", "Landroid/graphics/PorterDuff$Mode;");
            CheckErrorAndLog(env, "Util::ToJavaPorterDuffMode()", "Fail GetStaticFieldID: SRC_OVER : %d!\n", __LINE__);
            break;
        case Elastos::Droid::Graphics::PorterDuffMode_DST_OVER:
            f = env->GetStaticFieldID(cPorterDuffMode, "DST_OVER", "Landroid/graphics/PorterDuff$Mode;");
            CheckErrorAndLog(env, "Util::ToJavaPorterDuffMode()", "Fail GetStaticFieldID: DST_OVER : %d!\n", __LINE__);
            break;
        case Elastos::Droid::Graphics::PorterDuffMode_SRC_IN:
            f = env->GetStaticFieldID(cPorterDuffMode, "SRC_IN", "Landroid/graphics/PorterDuff$Mode;");
            CheckErrorAndLog(env, "Util::ToJavaPorterDuffMode()", "Fail GetStaticFieldID: SRC_IN : %d!\n", __LINE__);
            break;
        case Elastos::Droid::Graphics::PorterDuffMode_DST_IN:
            f = env->GetStaticFieldID(cPorterDuffMode, "DST_IN", "Landroid/graphics/PorterDuff$Mode;");
            CheckErrorAndLog(env, "Util::ToJavaPorterDuffMode()", "Fail GetStaticFieldID: DST_IN : %d!\n", __LINE__);
            break;
        case Elastos::Droid::Graphics::PorterDuffMode_SRC_OUT:
            f = env->GetStaticFieldID(cPorterDuffMode, "SRC_OUT", "Landroid/graphics/PorterDuff$Mode;");
            CheckErrorAndLog(env, "Util::ToJavaPorterDuffMode()", "Fail GetStaticFieldID: SRC_OUT : %d!\n", __LINE__);
            break;
        case Elastos::Droid::Graphics::PorterDuffMode_DST_OUT:
            f = env->GetStaticFieldID(cPorterDuffMode, "DST_OUT", "Landroid/graphics/PorterDuff$Mode;");
            CheckErrorAndLog(env, "Util::ToJavaPorterDuffMode()", "Fail GetStaticFieldID: DST_OUT : %d!\n", __LINE__);
            break;
        case Elastos::Droid::Graphics::PorterDuffMode_SRC_ATOP:
            f = env->GetStaticFieldID(cPorterDuffMode, "SRC_ATOP", "Landroid/graphics/PorterDuff$Mode;");
            CheckErrorAndLog(env, "Util::ToJavaPorterDuffMode()", "Fail GetStaticFieldID: SRC_ATOP : %d!\n", __LINE__);
            break;
        case Elastos::Droid::Graphics::PorterDuffMode_DST_ATOP:
            f = env->GetStaticFieldID(cPorterDuffMode, "DST_ATOP", "Landroid/graphics/PorterDuff$Mode;");
            CheckErrorAndLog(env, "Util::ToJavaPorterDuffMode()", "Fail GetStaticFieldID: DST_ATOP : %d!\n", __LINE__);
            break;
        case Elastos::Droid::Graphics::PorterDuffMode_XOR:
            f = env->GetStaticFieldID(cPorterDuffMode, "XOR", "Landroid/graphics/PorterDuff$Mode;");
            CheckErrorAndLog(env, "Util::ToJavaPorterDuffMode()", "Fail GetStaticFieldID: XOR : %d!\n", __LINE__);
            break;
        case Elastos::Droid::Graphics::PorterDuffMode_DARKEN:
            f = env->GetStaticFieldID(cPorterDuffMode, "DARKEN", "Landroid/graphics/PorterDuff$Mode;");
            CheckErrorAndLog(env, "Util::ToJavaPorterDuffMode()", "Fail GetStaticFieldID: DARKEN : %d!\n", __LINE__);
            break;
        case Elastos::Droid::Graphics::PorterDuffMode_LIGHTEN:
            f = env->GetStaticFieldID(cPorterDuffMode, "LIGHTEN", "Landroid/graphics/PorterDuff$Mode;");
            CheckErrorAndLog(env, "Util::ToJavaPorterDuffMode()", "Fail GetStaticFieldID: LIGHTEN : %d!\n", __LINE__);
            break;
        case Elastos::Droid::Graphics::PorterDuffMode_MULTIPLY:
            f = env->GetStaticFieldID(cPorterDuffMode, "MULTIPLY", "Landroid/graphics/PorterDuff$Mode;");
            CheckErrorAndLog(env, "Util::ToJavaPorterDuffMode()", "Fail GetStaticFieldID: MULTIPLY : %d!\n", __LINE__);
            break;
        case Elastos::Droid::Graphics::PorterDuffMode_SCREEN:
            f = env->GetStaticFieldID(cPorterDuffMode, "SCREEN", "Landroid/graphics/PorterDuff$Mode;");
            CheckErrorAndLog(env, "Util::ToJavaPorterDuffMode()", "Fail GetStaticFieldID: SCREEN : %d!\n", __LINE__);
            break;
        case Elastos::Droid::Graphics::PorterDuffMode_ADD:
            f = env->GetStaticFieldID(cPorterDuffMode, "ADD", "Landroid/graphics/PorterDuff$Mode;");
            CheckErrorAndLog(env, "Util::ToJavaPorterDuffMode()", "Fail GetStaticFieldID: ADD : %d!\n", __LINE__);
            break;
        case Elastos::Droid::Graphics::PorterDuffMode_OVERLAY:
            f = env->GetStaticFieldID(cPorterDuffMode, "OVERLAY", "Landroid/graphics/PorterDuff$Mode;");
            CheckErrorAndLog(env, "Util::ToJavaPorterDuffMode()", "Fail GetStaticFieldID: OVERLAY : %d!\n", __LINE__);
            break;
        default:
            break;
    }

    jobject jPorterDuffMode = env->GetStaticObjectField(cPorterDuffMode, f);
    CheckErrorAndLog(env, "Util::ToJavaPorterDuffMode()", "Fail GetStaticObjectField: : %d!\n", __LINE__);
    env->DeleteLocalRef(cPorterDuffMode);
    return jPorterDuffMode;
}

jobject Util::ToJavaAppWidgetProviderInfo(
    /* [in] */ JNIEnv* env,
    /* [in] */ IAppWidgetProviderInfo* obj)
{
    if (env == NULL || obj == NULL) {
        LOGGERW(TAG, "ToJavaAppWidgetProviderInfo: Invalid argumenet!");
        return NULL;
    }

    jclass klass = env->FindClass("android/appwidget/AppWidgetProviderInfo");
    Util::CheckErrorAndLog(env, "ToJavaAppWidgetProviderInfo Fail FindClass: AppWidgetProviderInfo : %d!\n", __LINE__);

    jmethodID m = env->GetMethodID(klass, "<init>", "()V");
    Util::CheckErrorAndLog(env, "ToJavaAppWidgetProviderInfo Fail GetMethodID: AppWidgetProviderInfo() : %d!\n", __LINE__);

    jobject jproviderInfo = env->NewObject(klass, m);
    Util::CheckErrorAndLog(env, "ToJavaAppWidgetProviderInfo Fail NewObject: AppWidgetProviderInfo : %d!\n", __LINE__);

    AutoPtr<IComponentName> componentName;
    obj->GetProvider((IComponentName**)&componentName);

    jobject jcomponentName = ToJavaComponentName(env, componentName);

    if (jcomponentName != NULL) {
        jfieldID f = env->GetFieldID(klass, "provider", "Landroid/content/ComponentName;");
        Util::CheckErrorAndLog(env, "ToJavaAppWidgetProviderInfo Fail GetFieldID: provider : %d!\n", __LINE__);

        env->SetObjectField(jproviderInfo, f, jcomponentName);
        Util::CheckErrorAndLog(env, "ToJavaAppWidgetProviderInfo Fail SetObjectField: provider : %d!\n", __LINE__);
        env->DeleteLocalRef(jcomponentName);
    }
    else {
        Logger::W(TAG, "ToJavaAppWidgetProviderInfo() WARN: jcomponentName is NULL!");
    }

    Int32 tempInt = 0;
    obj->GetMinWidth(&tempInt);
    SetJavaIntField(env, klass, jproviderInfo, tempInt, "minWidth", "ToJavaAppWidgetProviderInfo");

    obj->GetMinHeight(&tempInt);
    SetJavaIntField(env, klass, jproviderInfo, tempInt, "minHeight", "ToJavaAppWidgetProviderInfo");

    obj->GetMinResizeWidth(&tempInt);
    SetJavaIntField(env, klass, jproviderInfo, tempInt, "minResizeWidth", "ToJavaAppWidgetProviderInfo");

    obj->GetMinResizeHeight(&tempInt);
    SetJavaIntField(env, klass, jproviderInfo, tempInt, "minResizeHeight", "ToJavaAppWidgetProviderInfo");

    obj->GetUpdatePeriodMillis(&tempInt);
    SetJavaIntField(env, klass, jproviderInfo, tempInt, "updatePeriodMillis", "ToJavaAppWidgetProviderInfo");

    obj->GetInitialLayout(&tempInt);
    SetJavaIntField(env, klass, jproviderInfo, tempInt, "initialLayout", "ToJavaAppWidgetProviderInfo");

    obj->GetInitialKeyguardLayout(&tempInt);
    SetJavaIntField(env, klass, jproviderInfo, tempInt, "initialKeyguardLayout", "ToJavaAppWidgetProviderInfo");

    AutoPtr<IComponentName> configure;
    obj->GetConfigure((IComponentName**)&configure);

    jobject jconfigure = ToJavaComponentName(env, configure);

    if (jconfigure != NULL) {
        jfieldID f = env->GetFieldID(klass, "configure", "Landroid/content/ComponentName;");
        Util::CheckErrorAndLog(env, "ToJavaAppWidgetProviderInfo Fail GetFieldID: configure : %d!\n", __LINE__);

        env->SetObjectField(jproviderInfo, f, jconfigure);
        Util::CheckErrorAndLog(env, "ToJavaAppWidgetProviderInfo Fail SetObjectField: configure : %d!\n", __LINE__);
        env->DeleteLocalRef(jconfigure);
    }
    else {
        Logger::W(TAG, "ToJavaAppWidgetProviderInfo() WARN: jconfigure is NULL!");
    }

    String label;
    obj->GetLabel(&label);
    SetJavaStringField(env, klass, jproviderInfo, label, "label", "ToJavaAppWidgetProviderInfo");

    obj->GetIcon(&tempInt);
    SetJavaIntField(env, klass, jproviderInfo, tempInt, "icon", "ToJavaAppWidgetProviderInfo");

    obj->GetAutoAdvanceViewId(&tempInt);
    SetJavaIntField(env, klass, jproviderInfo, tempInt, "autoAdvanceViewId", "ToJavaAppWidgetProviderInfo");

    obj->GetPreviewImage(&tempInt);
    SetJavaIntField(env, klass, jproviderInfo, tempInt, "previewImage", "ToJavaAppWidgetProviderInfo");

    obj->GetResizeMode(&tempInt);
    SetJavaIntField(env, klass, jproviderInfo, tempInt, "resizeMode", "ToJavaAppWidgetProviderInfo");

    obj->GetWidgetCategory(&tempInt);
    SetJavaIntField(env, klass, jproviderInfo, tempInt, "widgetCategory", "ToJavaAppWidgetProviderInfo");

    AutoPtr<IActivityInfo> aInfo;
    obj->GetProviderInfo((IActivityInfo**)&aInfo);

    jobject jaInfo = ToJavaActivityInfo(env, aInfo);
    jfieldID f = env->GetFieldID(klass, "providerInfo", "Landroid/content/pm/ActivityInfo;");
    Util::CheckErrorAndLog(env, "ToJavaAppWidgetProviderInfo", "GetFieldID: providerInfo", __LINE__);

    env->SetObjectField(jproviderInfo, f, jaInfo);
    Util::CheckErrorAndLog(env, "ToJavaAppWidgetProviderInfo", "SetObjectField: providerInfo", __LINE__);

    env->DeleteLocalRef(klass);
    env->DeleteLocalRef(jaInfo);
    return jproviderInfo;
}

Boolean Util::GetElPendingIntent(
    /* [in] */ JNIEnv* env,
    /* [in] */ jobject jpintent,
    /* [out] */ IPendingIntent** pintent)
{
    if (env == NULL || jpintent == NULL || pintent == NULL) {
        LOGGERD(TAG, "GetElPendingIntent: Invalid argumenet!");
        return FALSE;
    }

    jclass notifKlass = env->FindClass("android/app/PendingIntent");
    CheckErrorAndLog(env, "FindClass: PendingIntent : %d!\n", __LINE__);

    jfieldID f = env->GetFieldID(notifKlass, "mTarget", "Landroid/content/IIntentSender;");
    Util::CheckErrorAndLog(env, "GetFieldID: IIntentSender : %d!\n", __LINE__);
    env->DeleteLocalRef(notifKlass);

    jobject jtarget = env->GetObjectField(jpintent, f);
    Util::CheckErrorAndLog(env, "GetObjectField: IIntentSender : %d!\n", __LINE__);
    if (jtarget == NULL) {
        LOGGERE(TAG, "GetElPendingIntent mTarget is NULL!");
        return FALSE;
    }

    JavaVM* jvm = NULL;
    env->GetJavaVM(&jvm);
    jobject jInstance = env->NewGlobalRef(jtarget);

    AutoPtr<IIIntentSender> iisender;
    if (NOERROR != CIIntentSenderNative::New((Handle64)jvm, (Handle64)jInstance, (IIIntentSender**)&iisender)) {
        LOGGERD(TAG, "GetElPendingIntent new CIIntentSenderNative fail!\n");
        return FALSE;
    }

    if (NOERROR != CPendingIntent::New(iisender, pintent)) {
        LOGGERD(TAG, "GetElPendingIntent: create CPendingIntent fail!");
        return FALSE;
    }

    env->DeleteLocalRef(jtarget);
    return TRUE;
}

jobject Util::ToJavaPendingIntent(
    /* [in] */ JNIEnv* env,
    /* [in] */ IPendingIntent* pendingIntent)
{
    if (env == NULL || pendingIntent == NULL) {
        LOGGERE("Util", "ToJavaPendingIntent: Invalid argumenet!");
        return NULL;
    }

    AutoPtr<IIIntentSender> iIntentSender;

    pendingIntent->GetTarget((IIIntentSender**)&iIntentSender);

    if (iIntentSender == NULL){
        LOGGERE("Util", "ToJavaPendingIntent, Got iIntentSender is NULL");
        return NULL;
    }

    jobject jpendingIntentSender = ToJavaIIntentSender(env, iIntentSender);

    jclass klass = env->FindClass("android/app/PendingIntent");
    CheckErrorAndLog(env, "Util", "FindClass: PendingIntent : %d!\n", __LINE__);

    jmethodID m = env->GetMethodID(klass, "<init>", "(Landroid/content/IIntentSender;)V");
    CheckErrorAndLog(env, "Util", "ToJavaPendingIntent GetMethodID: PendingIntent(IIntentSender) : %d!\n", __LINE__);

    jobject jpendingIntent = env->NewObject(klass, m, jpendingIntentSender);
    CheckErrorAndLog(env, "Util", "ToJavaPendingIntent NewObject: PendingIntent : %d!\n", __LINE__);

    env->DeleteLocalRef(klass);
    env->DeleteLocalRef(jpendingIntentSender);
    return jpendingIntent;
}

jobject Util::ToJavaIIntentSender(
    /* [in] */ JNIEnv* env,
    /* [in] */ IIIntentSender* isender)
{
    if (env == NULL || isender == NULL) {
        LOGGERE("Util", "GetJavaIIentSender: Invalid argumenet!");
        return NULL;
    }

    jclass pergKlass = env->FindClass("android/content/ElIIntentSenderProxy");
    CheckErrorAndLog(env, "Util", "ToJavaIIntentSender FindClass: ElIIntentSenderProxy : %d!\n", __LINE__);

    jmethodID m = env->GetMethodID(pergKlass, "<init>", "(J)V");
    CheckErrorAndLog(env, "Util", "ToJavaIIntentSender GetMethodID: ElIIntentSenderProxy : %d!\n", __LINE__);

    jobject jisender = env->NewObject(pergKlass, m, (jlong)isender);
    CheckErrorAndLog(env, "Util", "ToJavaIIntentSender NewObject: ElIIntentSenderProxy : %d!\n", __LINE__);
    isender->AddRef();

    env->DeleteLocalRef(pergKlass);
    return jisender;
}

jobject Util::ToJavaExtractedText(
    /* [in] */ JNIEnv* env,
    /* [in] */ IExtractedText* ert)
{
    if (env == NULL || ert == NULL){
        LOGGERE("ToJavaExtractedText()", "Invalid argumenet!");
        return NULL;
    }

    jclass ertKlass = env->FindClass("android/view/inputmethod/ExtractedText");
    Util::CheckErrorAndLog(env, "ToJavaExtractedText", "FindClass: ExtractedText : %d!\n", __LINE__);

    jmethodID m = env->GetMethodID(ertKlass, "<init>", "()V");
    Util::CheckErrorAndLog(env, "ToJavaExtractedText", "GetMethodID: ExtractedText : %d!\n", __LINE__);

    jobject jert = env->NewObject(ertKlass, m);
    Util::CheckErrorAndLog(env, "ToJavaExtractedText", "NewObject: ExtractedText : %d!\n", __LINE__);

    AutoPtr<ICharSequence> text;
    ert->GetText((ICharSequence**)&text);
    if (text != NULL) {
        String stext;
        text->ToString(&stext);
        jstring jtext = Util::ToJavaString(env, stext);

        jfieldID f = env->GetFieldID(ertKlass, "text", "Ljava/lang/CharSequence;");
        Util::CheckErrorAndLog(env, "ToJavaExtractedText", "GetFieldID: CharSequence", __LINE__);

        env->SetObjectField(jert, f, jtext);
        Util::CheckErrorAndLog(env, "ToJavaExtractedText", "SetObjectField: text", __LINE__);
        env->DeleteLocalRef(jtext);
    }

    Int32 tempInt;
    ert->GetStartOffset(&tempInt);
    Util::SetJavaIntField(env, ertKlass, jert, tempInt, "startOffset", "ToJavaExtractedText");

    ert->GetPartialStartOffset(&tempInt);
    Util::SetJavaIntField(env, ertKlass, jert, tempInt, "partialStartOffset", "ToJavaExtractedText");

    ert->GetPartialEndOffset(&tempInt);
    Util::SetJavaIntField(env, ertKlass, jert, tempInt, "partialEndOffset", "ToJavaExtractedText");

    ert->GetSelectionStart(&tempInt);
    Util::SetJavaIntField(env, ertKlass, jert, tempInt, "selectionStart", "ToJavaExtractedText");

    ert->GetSelectionEnd(&tempInt);
    Util::SetJavaIntField(env, ertKlass, jert, tempInt, "selectionEnd", "ToJavaExtractedText");

    ert->GetFlags(&tempInt);
    Util::SetJavaIntField(env, ertKlass, jert, tempInt, "flags", "ToJavaExtractedText");
    env->DeleteLocalRef(ertKlass);
    return jert;
}

jobject Util::ToJavaExtractedTextRequest(
    /* [in] */ JNIEnv* env,
    /* [in] */ IExtractedTextRequest* ertr)
{
    if (env == NULL || ertr == NULL){
        LOGGERE("ToJavaExtractedTextRequest()", "Invalid argumenet!");
        return NULL;
    }

    jclass ertrKlass = env->FindClass("android/view/inputmethod/ExtractedTextRequest");
    Util::CheckErrorAndLog(env, "ToJavaExtractedTextRequest", "FindClass: ExtractedTextRequest : %d!\n", __LINE__);

    jmethodID m = env->GetMethodID(ertrKlass, "<init>", "()V");
    Util::CheckErrorAndLog(env, "ToJavaExtractedTextRequest", "GetMethodID: ExtractedTextRequest : %d!\n", __LINE__);

    jobject jertr = env->NewObject(ertrKlass, m);
    Util::CheckErrorAndLog(env, "ToJavaExtractedTextRequest", "NewObject: ExtractedTextRequest : %d!\n", __LINE__);

    Int32 tempInt;
    ertr->GetToken(&tempInt);
    Util::SetJavaIntField(env, ertrKlass, jertr, tempInt, "token", "ToJavaExtractedTextRequest");

    ertr->GetFlags(&tempInt);
    Util::SetJavaIntField(env, ertrKlass, jertr, tempInt, "flags", "ToJavaExtractedTextRequest");

    ertr->GetHintMaxLines(&tempInt);
    Util::SetJavaIntField(env, ertrKlass, jertr, tempInt, "hintMaxLines", "ToJavaExtractedTextRequest");

    ertr->GetHintMaxChars(&tempInt);
    Util::SetJavaIntField(env, ertrKlass, jertr, tempInt, "hintMaxChars", "ToJavaExtractedTextRequest");
    env->DeleteLocalRef(ertrKlass);
    return jertr;
}

jobject Util::ToJavaCompletionInfo(
    /* [in] */ JNIEnv* env,
    /* [in] */ ICompletionInfo* completion)
{
    if (env == NULL || completion == NULL){
        LOGGERE("ToJavaCompletionInfo()", "Invalid argumenet!");
        return NULL;
    }

    jclass cIKlass = env->FindClass("android/view/inputmethod/CompletionInfo");
    Util::CheckErrorAndLog(env, "ToJavaExtractedTextRequest", "FindClass: CompletionInfo : %d!\n", __LINE__);

    jmethodID m = env->GetMethodID(cIKlass, "<init>", "(JILjava/lang/CharSequence;Ljava/lang/CharSequence;)V");
    Util::CheckErrorAndLog(env, "ToJavaExtractedTextRequest", "GetMethodID: CompletionInfo : %d!\n", __LINE__);

    Int64 id = 0;
    completion->GetId(&id);

    Int32 position = 0;
    completion->GetPosition(&position);

    AutoPtr<ICharSequence> text;
    completion->GetText((ICharSequence**)&text);
    jobject jtext = NULL;
    if (text != NULL) {
        String str;
        text->ToString(&str);
        jtext = Util::ToJavaString(env, str);
    }

    AutoPtr<ICharSequence> label;
    completion->GetLabel((ICharSequence**)&label);
    jobject jlabel = NULL;
    if (label != NULL) {
        String str;
        label->ToString(&str);
        jlabel = Util::ToJavaString(env, str);
    }

    jobject jcompletion = env->NewObject(cIKlass, m, (jlong)id, (Int32)position, jtext, jlabel);
    Util::CheckErrorAndLog(env, "ToJavaExtractedTextRequest", "NewObject: CompletionInfo : %d!\n", __LINE__);
    env->DeleteLocalRef(cIKlass);
    env->DeleteLocalRef(jtext);
    env->DeleteLocalRef(jlabel);
    return jcompletion;
}

jobject Util::ToJavaCorrectionInfo(
    /* [in] */ JNIEnv* env,
    /* [in] */ ICorrectionInfo* correction)
{
    if (env == NULL || correction == NULL){
        LOGGERE("ToJavaCorrectionInfo()", "Invalid argumenet!");
        return NULL;
    }

    jclass cIKlass = env->FindClass("android/view/inputmethod/CorrectionInfo");
    Util::CheckErrorAndLog(env, "ToJavaExtractedTextRequest", "FindClass: CorrectionInfo : %d!\n", __LINE__);

    jmethodID m = env->GetMethodID(cIKlass, "<init>", "(ILjava/lang/CharSequence;Ljava/lang/CharSequence;)V");
    Util::CheckErrorAndLog(env, "ToJavaExtractedTextRequest", "GetMethodID: CorrectionInfo : %d!\n", __LINE__);


    Int32 offset = 0;
    correction->GetOffset(&offset);

    AutoPtr<ICharSequence> oldText;
    correction->GetOldText((ICharSequence**)&oldText);
    jobject joldText = NULL;
    if (oldText != NULL) {
        String str;
        oldText->ToString(&str);
        joldText = Util::ToJavaString(env, str);
    }

    AutoPtr<ICharSequence> newText;
    correction->GetNewText((ICharSequence**)&newText);
    jobject jnewText = NULL;
    if (newText != NULL) {
        String str;
        newText->ToString(&str);
        jnewText = Util::ToJavaString(env, str);
    }

    jobject jcorrection = env->NewObject(cIKlass, m, (Int32)offset, joldText, jnewText);
    Util::CheckErrorAndLog(env, "ToJavaExtractedTextRequest", "NewObject: CorrectionInfo : %d!\n", __LINE__);
    env->DeleteLocalRef(cIKlass);
    env->DeleteLocalRef(joldText);
    env->DeleteLocalRef(jnewText);
    return jcorrection;
}

jobject Util::ToJavaStorageVolume(
    /* [in] */ JNIEnv* env,
    /* [in] */ IStorageVolume* volume)
{
    if (env == NULL || volume == NULL){
        LOGGERE("ToJavaStorageVolume()", "Invalid argumenet!");
        return NULL;
    }

    jclass volKlass = env->FindClass("android/os/storage/StorageVolume");
    Util::CheckErrorAndLog(env, "ToJavaStorageVolume", "FindClass: StorageVolume : %d!\n", __LINE__);

    jmethodID m = env->GetMethodID(volKlass, "<init>", "(Ljava/io/File;IZZZIZJLandroid/os/UserHandle;)V");
    Util::CheckErrorAndLog(env, "ToJavaStorageVolume", "GetMethodID: StorageVolume : %d!\n", __LINE__);

    String path;
    volume->GetPath(&path);
    jobject jpathFile = NULL;
    if (!path.IsNullOrEmpty()) {
        jstring jpath = Util::ToJavaString(env, path);

        jclass fileKlass = env->FindClass("java/io/File");
        Util::CheckErrorAndLog(env, "ToJavaStorageVolume", "FindClass: File : %d!\n", __LINE__);

        jmethodID filem = env->GetMethodID(fileKlass, "<init>", "(Ljava/lang/String;)V");
        Util::CheckErrorAndLog(env, "ToJavaStorageVolume", "GetMethodID: File : %d!\n", __LINE__);

        jpathFile = env->NewObject(fileKlass, filem, jpath);
        Util::CheckErrorAndLog(env, "ToJavaStorageVolume", "NewObject: File : %d!\n", __LINE__);

        env->DeleteLocalRef(fileKlass);
        env->DeleteLocalRef(jpath);
    }

    Int32 descriptionId;
    volume->GetDescriptionId(&descriptionId);
    Boolean primary;
    volume->IsPrimary(&primary);
    Boolean removable;
    volume->IsRemovable(&removable);
    Boolean emulated;
    volume->IsEmulated(&emulated);
    Int32 mtpReserveSpace;
    volume->GetMtpReserveSpace(&mtpReserveSpace);
    Boolean allowMassStorage;
    volume->AllowMassStorage(&allowMassStorage);
    Int64 maxFileSize;
    volume->GetMaxFileSize(&maxFileSize);

    AutoPtr<IUserHandle> owner;
    volume->GetOwner((IUserHandle**)&owner);
    jobject jowner = NULL;
    if (owner != NULL) {
        jowner = Util::ToJavaUserHandle(env, owner);
    }

    jobject jvolume = env->NewObject(volKlass, m, jpathFile, (jint)descriptionId, (jboolean)primary, (jboolean)removable,
            (jboolean)emulated, (jint)mtpReserveSpace, (jboolean)allowMassStorage, (jlong)maxFileSize, jowner);
    Util::CheckErrorAndLog(env, "ToJavaStorageVolume", "NewObject: StorageVolume : %d!\n", __LINE__);

    env->DeleteLocalRef(jpathFile);
    env->DeleteLocalRef(jowner);

    Int32 storageId;
    volume->GetStorageId(&storageId);
    Util::SetJavaIntField(env, volKlass, jvolume, storageId, "mStorageId", "ToJavaStorageVolume");

    String uuid;
    volume->GetUuid(&uuid);
    Util::SetJavaStringField(env, volKlass, jvolume, uuid, "mUuid", "ToJavaStorageVolume");

    String userLabel;
    volume->GetUserLabel(&userLabel);
    Util::SetJavaStringField(env, volKlass, jvolume, uuid, "mUserLabel", "ToJavaStorageVolume");

    String state;
    volume->GetState(&state);
    Util::SetJavaStringField(env, volKlass, jvolume, state, "mState", "ToJavaStorageVolume");

    env->DeleteLocalRef(volKlass);
    return jvolume;
}

jobject Util::ToJavaUserHandle(
    /* [in] */ JNIEnv* env,
    /* [in] */ IUserHandle* handle)
{
    if (env == NULL || handle == NULL){
        LOGGERE("ToJavaUserHandle()", "Invalid argumenet!");
        return NULL;
    }

    jclass handleKlass = env->FindClass("android/os/UserHandle");
    Util::CheckErrorAndLog(env, "ToJavaUserHandle", "FindClass: UserHandle : %d!\n", __LINE__);

    jmethodID m = env->GetMethodID(handleKlass, "<init>", "(I)V");
    Util::CheckErrorAndLog(env, "ToJavaUserHandle", "GetMethodID: UserHandle : %d!\n", __LINE__);

    Int32 id;
    handle->GetIdentifier(&id);

    jobject jhandle = env->NewObject(handleKlass, m, (jint)id);
    Util::CheckErrorAndLog(env, "ToJavaUserHandle", "NewObject: UserHandle : %d!\n", __LINE__);
    env->DeleteLocalRef(handleKlass);
    return jhandle;
}

Boolean Util::GetElFileDescriptor(
    /* [in] */ JNIEnv* env,
    /* [in] */ jobject jfDescptor,
    /* [out] */ IFileDescriptor** fDescptor)
{
    if (env == NULL || jfDescptor == NULL || fDescptor == NULL) {
        LOGGERE("GetElFileDescriptor()", "Invalid argumenet!");
        return FALSE;
    }

    if (NOERROR != CFileDescriptor::New(fDescptor)) {
        LOGGERE("GetElFileDescriptor", "create CFileDescriptor fail!");
        return FALSE;
    }

    jclass fdKlass = env->FindClass("java/io/FileDescriptor");
    Util::CheckErrorAndLog(env, "GetElFileDescriptor", "FindClass: FileDescriptor %d", __LINE__);

    Int32 descriptor = GetJavaIntField(env, fdKlass, jfDescptor, "descriptor", "GetElFileDescriptor");

    Int32 fd = dup(descriptor);

    (*fDescptor)->SetDescriptor(fd);
    env->DeleteLocalRef(fdKlass);
    return TRUE;
}

Boolean Util::GetElParcelFileDescriptor(
    /* [in] */ JNIEnv* env,
    /* [in] */ jobject jpFDescptor,
    /* [out] */ IParcelFileDescriptor** pFDescptor)
{
    if (env == NULL || jpFDescptor == NULL || pFDescptor == NULL) {
        LOGGERE("GetElParcelFileDescriptor()", "Invalid argumenet!");
        return FALSE;
    }

    jclass pfdKlass = env->FindClass("android/os/ParcelFileDescriptor");
    Util::CheckErrorAndLog(env, "GetElParcelFileDescriptor", "FindClass: ParcelFileDescriptor %d", __LINE__);

    jfieldID f = env->GetFieldID(pfdKlass, "mFd", "Ljava/io/FileDescriptor;");
    Util::CheckErrorAndLog(env, "GetElParcelFileDescriptor", "GetFieldID: mData  %d", __LINE__);
    env->DeleteLocalRef(pfdKlass);

    jobject jfd = env->GetObjectField(jpFDescptor, f);
    Util::CheckErrorAndLog(env, "GetElParcelFileDescriptor", "GetObjectField: mFd  %d", __LINE__);

    AutoPtr<IFileDescriptor> fd;
    if (!GetElFileDescriptor(env, jfd, (IFileDescriptor**)&fd)) {
        LOGGERE("GetElParcelFileDescriptor", "GetElFileDescriptor() fail ", __LINE__);
    }

    env->DeleteLocalRef(jfd);

    if (NOERROR != CParcelFileDescriptor::New(fd, pFDescptor)) {
        LOGGERE("GetElParcelFileDescriptor", "create CParcelFileDescriptor fail!");
        return FALSE;
    }

    return TRUE;
}

Boolean Util::GetElAssetFileDescriptor(
    /* [in] */ JNIEnv* env,
    /* [in] */ jobject jaFDescptor,
    /* [out] */ IAssetFileDescriptor** aFDescptor)
{
    if (env == NULL || jaFDescptor == NULL || aFDescptor == NULL) {
        LOGGERE("GetElAssetFileDescriptor()", "Invalid argumenet!");
        return FALSE;
    }

    jclass afdKlass = env->FindClass("android/content/res/AssetFileDescriptor");
    Util::CheckErrorAndLog(env, "GetElAssetFileDescriptor", "FindClass: AssetFileDescriptor %d", __LINE__);

    jfieldID f = env->GetFieldID(afdKlass, "mFd", "Landroid/os/ParcelFileDescriptor;");
    Util::CheckErrorAndLog(env, "GetElAssetFileDescriptor", "GetFieldID: mFd  %d", __LINE__);

    jobject jfd = env->GetObjectField(jaFDescptor, f);
    Util::CheckErrorAndLog(env, "GetElAssetFileDescriptor", "GetObjectField: mFd %d", __LINE__);

    AutoPtr<IParcelFileDescriptor> fd;
    if (!GetElParcelFileDescriptor(env, jfd, (IParcelFileDescriptor**)&fd)) {
        LOGGERE("GetElParcelFileDescriptor", "GetElParcelFileDescriptor() fail ", __LINE__);
    }

    env->DeleteLocalRef(jfd);

    Int64 startOffset = GetJavalongField(env, jaFDescptor, "mStartOffset", "GetElAssetFileDescriptor");
    Int64 length = GetJavalongField(env, jaFDescptor, "mLength", "GetElAssetFileDescriptor");

    f = env->GetFieldID(afdKlass, "mExtras", "Landroid/os/Bundle;");
    Util::CheckErrorAndLog(env, "GetElAssetFileDescriptor", "GetFieldID: mExtras  %d", __LINE__);

    jobject jextras = env->GetObjectField(jaFDescptor, f);
    Util::CheckErrorAndLog(env, "GetElAssetFileDescriptor", "GetObjectField: mExtras %d", __LINE__);
    env->DeleteLocalRef(afdKlass);

    AutoPtr<IBundle> extras;
    if (jextras != NULL && !GetElBundle(env, jextras, (IBundle**)&extras)) {
        LOGGERE("GetElParcelFileDescriptor", "GetElBundle() fail ", __LINE__);
    }

    if (NOERROR != CAssetFileDescriptor::New(fd, startOffset, length, extras, aFDescptor)) {
        LOGGERE("GetElAssetFileDescriptor", "create CParcelFileDescriptor fail!");
        return FALSE;
    }

    return TRUE;
}

Boolean Util::GetElObbInfo(
    /* [in] */ JNIEnv* env,
    /* [in] */ jobject jobbInfo,
    /* [out] */ IObbInfo** obbInfo)
{
    if (env == NULL || jobbInfo == NULL || obbInfo == NULL) {
        LOGGERE("GetElObbInfo()", "Invalid argumenet!");
        *obbInfo = NULL;
        return FALSE;
    }

    jclass obbKlass = env->FindClass("android/content/res/ObbInfo");
    Util::CheckErrorAndLog(env, "GetElObbInfo", "FindClass: ObbInfo %d", __LINE__);

    if (NOERROR != CObbInfo::New(obbInfo)) {
        LOGGERE("GetElObbInfo", "create CObbInfo fail!");
        *obbInfo = NULL;
        return FALSE;
    }

    String filename = GetJavaStringField(env, obbKlass, jobbInfo, "filename", "GetElObbInfo");
     (*obbInfo)->SetFilename(filename);

    String packageName = GetJavaStringField(env, obbKlass, jobbInfo, "packageName", "GetElObbInfo");
     (*obbInfo)->SetPackageName(packageName);

    Int32 tempInt = GetJavaIntField(env, obbKlass, jobbInfo, "version", "GetElObbInfo");
    (*obbInfo)->SetVersion(tempInt);

    tempInt = GetJavaIntField(env, obbKlass, jobbInfo, "flags", "GetElObbInfo");
    (*obbInfo)->SetVersion(tempInt);

    jfieldID f = env->GetFieldID(obbKlass, "salt", "[B");
    Util::CheckErrorAndLog(env, "GetElObbInfo", "GetFieldID: otherStats %d", __LINE__);

    jbyteArray jsalt = (jbyteArray)env->GetObjectField(jobbInfo, f);
    Util::CheckErrorAndLog(env, "GetElObbInfo", "GetFieldID: salt %d", __LINE__);

    AutoPtr<ArrayOf<Byte> > salt;
    if (jsalt != NULL) {
        Util::GetElByteArray(env, jsalt, (ArrayOf<Byte>**)&salt);
        (*obbInfo)->SetSalt(salt);

        env->DeleteLocalRef(jsalt);
    }

    env->DeleteLocalRef(obbKlass);
    return TRUE;
}

jobject Util::ToJavaContentValues(
    /* [in] */ JNIEnv* env,
    /* [in] */ IContentValues* contentValues)
{
    if (env == NULL || contentValues == NULL) {
        LOGGERE("ToJavaBundle", "Invalid arguments!");
        return NULL;
    }

    jobject jContentValues = NULL;

    jclass contentValuesKlass = env->FindClass("android/content/ContentValues");
    Util::CheckErrorAndLog(env, "ToJavaContentValues", "Fail FindClass: ContentValues %d", __LINE__);

    jmethodID m = env->GetMethodID(contentValuesKlass, "<init>", "()V");
    Util::CheckErrorAndLog(env, "ToJavaContentValues", "Fail GetMethodID: contentValuesKlass %d", __LINE__);

    jContentValues = env->NewObject(contentValuesKlass, m);
    Util::CheckErrorAndLog(env, "ToJavaContentValues", "Fail NewObject: contentValuesKlass %d", __LINE__);

    AutoPtr<ISet> set;
    contentValues->GetKeySet((ISet**)&set);
    AutoPtr<ArrayOf<IInterface*> > keys;
    set->ToArray((ArrayOf<IInterface*>**)&keys);
    Int32 size = keys->GetLength();
    for (Int32 i = 0; i < size; i++) {
        String keyStr;
        ICharSequence::Probe((*keys)[i])->ToString(&keyStr);
        jstring jKey = Util::ToJavaString(env, keyStr);
        AutoPtr<IInterface> value;
        contentValues->Get(keyStr, (IInterface**)&value);
        if (value != NULL) {
            if (ICharSequence::Probe(value) != NULL) {
                String sv;
                ICharSequence::Probe(value)->ToString(&sv);
                jstring jValue = Util::ToJavaString(env, sv);

                jmethodID m = env->GetMethodID(contentValuesKlass, "put", "(Ljava/lang/String;Ljava/lang/String;)V");
                Util::CheckErrorAndLog(env, "ToJavaContentValues", "Fail GetMethodID: putString %d", __LINE__);

                env->CallVoidMethod(jContentValues, m, jKey, jValue);
                env->DeleteLocalRef(jValue);
            }
            else if (IByte::Probe(value) != NULL) {
                Byte v;
                IByte::Probe(value)->GetValue(&v);

                jmethodID m = env->GetMethodID(contentValuesKlass, "put", "(Ljava/lang/String;Ljava/lang/Byte;)V");
                Util::CheckErrorAndLog(env, "ToJavaContentValues", "Fail GetMethodID: put %d", __LINE__);

                jclass byteKlass = env->FindClass("java/lang/Byte");
                Util::CheckErrorAndLog(env, "ToJavaContentValues", "Fail FindClass: Byte %d", __LINE__);

                jmethodID mInit = env->GetMethodID(byteKlass, "<init>", "(B)V");
                Util::CheckErrorAndLog(env, "ToJavaContentValues", "Fail GetMethodID: Byte %d", __LINE__);

                jobject jValue = env->NewObject(byteKlass, mInit, v);
                Util::CheckErrorAndLog(env, "ToJavaContentValues", "Fail NewObject: Byte %d", __LINE__);

                env->CallVoidMethod(jContentValues, m, jKey, jValue);
                env->DeleteLocalRef(byteKlass);
                env->DeleteLocalRef(jValue);
            }
            else if (IInteger16::Probe(value) != NULL) {
                Int16 v;
                IInteger16::Probe(value)->GetValue(&v);

                jmethodID m = env->GetMethodID(contentValuesKlass, "put", "(Ljava/lang/String;Ljava/lang/Short;)V");
                Util::CheckErrorAndLog(env, "ToJavaContentValues", "Fail GetMethodID: put %d", __LINE__);

                jclass shortKlass = env->FindClass("java/lang/Short");
                Util::CheckErrorAndLog(env, "ToJavaContentValues", "Fail FindClass: Short %d", __LINE__);

                jmethodID mInit = env->GetMethodID(shortKlass, "<init>", "(S)V");
                Util::CheckErrorAndLog(env, "ToJavaContentValues", "Fail GetMethodID: Short %d", __LINE__);

                jobject jValue = env->NewObject(shortKlass, mInit, v);
                Util::CheckErrorAndLog(env, "ToJavaContentValues", "Fail NewObject: Short %d", __LINE__);

                env->CallVoidMethod(jContentValues, m, jKey, jValue);
                env->DeleteLocalRef(shortKlass);
                env->DeleteLocalRef(jValue);
            }
            else if (IInteger32::Probe(value) != NULL) {
                Int32 v;
                IInteger32::Probe(value)->GetValue(&v);

                jmethodID m = env->GetMethodID(contentValuesKlass, "put", "(Ljava/lang/String;Ljava/lang/Integer;)V");
                Util::CheckErrorAndLog(env, "ToJavaContentValues", "Fail GetMethodID: put %d", __LINE__);

                jclass integerKlass = env->FindClass("java/lang/Integer");
                Util::CheckErrorAndLog(env, "ToJavaContentValues", "Fail FindClass: Integer %d", __LINE__);

                jmethodID mInit = env->GetMethodID(integerKlass, "<init>", "(I)V");
                Util::CheckErrorAndLog(env, "ToJavaContentValues", "Fail GetMethodID: Integer %d", __LINE__);

                jobject jValue = env->NewObject(integerKlass, mInit, v);
                Util::CheckErrorAndLog(env, "ToJavaContentValues", "Fail NewObject: Integer %d", __LINE__);

                env->CallVoidMethod(jContentValues, m, jKey, jValue);
                env->DeleteLocalRef(integerKlass);
                env->DeleteLocalRef(jValue);
            }
            else if (IInteger64::Probe(value) != NULL) {
                Int64 v;
                IInteger64::Probe(value)->GetValue(&v);

                jmethodID m = env->GetMethodID(contentValuesKlass, "put", "(Ljava/lang/String;Ljava/lang/Long;)V");
                Util::CheckErrorAndLog(env, "ToJavaContentValues", "Fail GetMethodID: put %d", __LINE__);

                jclass longKlass = env->FindClass("java/lang/Long");
                Util::CheckErrorAndLog(env, "ToJavaContentValues", "Fail FindClass: Long %d", __LINE__);

                jmethodID mInit = env->GetMethodID(longKlass, "<init>", "(J)V");
                Util::CheckErrorAndLog(env, "ToJavaContentValues", "Fail GetMethodID: Long %d", __LINE__);

                jobject jValue = env->NewObject(longKlass, mInit, v);
                Util::CheckErrorAndLog(env, "ToJavaContentValues", "Fail NewObject: Long %d", __LINE__);

                env->CallVoidMethod(jContentValues, m, jKey, jValue);
                env->DeleteLocalRef(longKlass);
                env->DeleteLocalRef(jValue);
            }
            else if (IFloat::Probe(value) != NULL) {
                Float v;
                IFloat::Probe(value)->GetValue(&v);

                jmethodID m = env->GetMethodID(contentValuesKlass, "put", "(Ljava/lang/String;Ljava/lang/Float;)V");
                Util::CheckErrorAndLog(env, "ToJavaContentValues", "Fail GetMethodID: put %d", __LINE__);

                jclass floatKlass = env->FindClass("java/lang/Float");
                Util::CheckErrorAndLog(env, "ToJavaContentValues", "Fail FindClass: Float %d", __LINE__);

                jmethodID mInit = env->GetMethodID(floatKlass, "<init>", "(F)V");
                Util::CheckErrorAndLog(env, "ToJavaContentValues", "Fail GetMethodID: Float %d", __LINE__);

                jobject jValue = env->NewObject(floatKlass, mInit, v);
                Util::CheckErrorAndLog(env, "ToJavaContentValues", "Fail NewObject: Float %d", __LINE__);

                env->CallVoidMethod(jContentValues, m, jKey, jValue);
                env->DeleteLocalRef(floatKlass);
                env->DeleteLocalRef(jValue);
            }
            else if (IDouble::Probe(value) != NULL) {
                Double v;
                IDouble::Probe(value)->GetValue(&v);

                jmethodID m = env->GetMethodID(contentValuesKlass, "put", "(Ljava/lang/String;Ljava/lang/Double;)V");
                Util::CheckErrorAndLog(env, "ToJavaContentValues", "Fail GetMethodID: put %d", __LINE__);

                jclass doubleKlass = env->FindClass("java/lang/Double");
                Util::CheckErrorAndLog(env, "ToJavaContentValues", "Fail FindClass: Double %d", __LINE__);

                jmethodID mInit = env->GetMethodID(doubleKlass, "<init>", "(D)V");
                Util::CheckErrorAndLog(env, "ToJavaContentValues", "Fail GetMethodID: Double %d", __LINE__);

                jobject jValue = env->NewObject(doubleKlass, mInit, v);
                Util::CheckErrorAndLog(env, "ToJavaContentValues", "Fail NewObject: Double %d", __LINE__);

                env->CallVoidMethod(jContentValues, m, jKey, jValue);
                env->DeleteLocalRef(doubleKlass);
                env->DeleteLocalRef(jValue);
            }
            else if (IBoolean::Probe(value) != NULL) {
                Boolean bv;
                IBoolean::Probe(value)->GetValue(&bv);

                jmethodID m = env->GetMethodID(contentValuesKlass, "put", "(Ljava/lang/String;Ljava/lang/Boolean;)V");
                Util::CheckErrorAndLog(env, "ToJavaContentValues", "Fail GetMethodID: put %d", __LINE__);

                jclass booleanKlass = env->FindClass("java/lang/Boolean");
                Util::CheckErrorAndLog(env, "ToJavaContentValues", "Fail FindClass: Boolean %d", __LINE__);

                jmethodID mv = env->GetStaticMethodID(booleanKlass, "valueOf", "(Z)Ljava/lang/Boolean;");
                Util::CheckErrorAndLog(env, "ToJavaContentValues", "Fail GetMethodID: valueOf %d", __LINE__);

                jobject jValue = env->CallStaticObjectMethod(booleanKlass, mv, bv);
                Util::CheckErrorAndLog(env, "ToJavaContentValues", "Fail CallObjectMethod valueOf %d", __LINE__);

                env->CallVoidMethod(jContentValues, m, jKey, jValue);
                env->DeleteLocalRef(booleanKlass);
                env->DeleteLocalRef(jValue);
            }
            else if (IArrayOf::Probe(value) != NULL) {
                AutoPtr<IArrayOf> array = IArrayOf::Probe(value);
                Int32 size = 0;
                array->GetLength(&size);
                AutoPtr<ArrayOf<Byte> > bytes = ArrayOf<Byte>::Alloc(size);
                for (Int32 i = 0; i < size; i++) {
                    AutoPtr<IInterface> item;
                    array->Get(i, (IInterface**)&item);
                    if (IByte::Probe(item)) {
                        Byte v;
                        IByte::Probe(item)->GetValue(&v);
                        (*bytes)[i] = v;
                    }
                    else {
                        LOGGERE("ToJavaContentValues", "Invalid item Line:%d", __LINE__);
                    }
                }

                jbyteArray jbytearray = env->NewByteArray((jsize)size);
                CheckErrorAndLog(env, "ToJavaContentValues", "NewByteArray failed %d", __LINE__);

                env->SetByteArrayRegion(jbytearray, 0, size, (jbyte*)bytes->GetPayload());
                CheckErrorAndLog(env, "ToJavaContentValues", "SetByteArrayRegion failed %d", __LINE__);

                jmethodID m = env->GetMethodID(contentValuesKlass, "put", "(Ljava/lang/String;[B)V");
                Util::CheckErrorAndLog(env, "ToJavaContentValues", "Fail GetMethodID: put %d", __LINE__);

                env->CallVoidMethod(jContentValues, m, jKey, jbytearray);
                env->DeleteLocalRef(jbytearray);
            }
            else {
                LOGGERE("ToJavaContentValues", "Unknown value type!");
                AutoPtr<IObject> object = IObject::Probe(value);
                ClassID clsid;
                object->GetClassID(&clsid);
                DUMP_CLSID(clsid, "ToJavaContentValues");
            }
        }
        else {
            jmethodID m = env->GetMethodID(contentValuesKlass, "putNull", "(Ljava/lang/String;)V");
            Util::CheckErrorAndLog(env, "ToJavaContentValues", "Fail GetMethodID: putNull %d", __LINE__);

            env->CallVoidMethod(jContentValues, m, jKey);
        }

        env->DeleteLocalRef(jKey);
    }
    env->DeleteLocalRef(contentValuesKlass);

    return jContentValues;
}

jobject Util::ToJavaDragEvent(
    /* [in] */ JNIEnv* env,
    /* [in] */ IDragEvent* event)
{
    if (env == NULL || event == NULL) {
        LOGGERE("ToJavaDragEvent", "Invalid arguments!");
        return NULL;
    }

    jclass deKlass = env->FindClass("android/view/DragEvent");
    Util::CheckErrorAndLog(env, "ToJavaDragEvent", "FindClass: DragEvent line: %d", __LINE__);

    AutoPtr<IParcel> parcel;
    Elastos::Droid::Os::CParcel::New((IParcel**)&parcel);

    if (NOERROR != IParcelable::Probe(event)->WriteToParcel(parcel)) {
        LOGGERE("ToJavaDragEvent", "WriteToParcel() failed!");
        return NULL;
    }

    Int32 action = 0;
    parcel->ReadInt32(&action);
    Float X = 0;
    parcel->ReadFloat(&X);
    Float Y = 0;
    parcel->ReadFloat(&Y);
    Int32 result = 0;
    parcel->ReadInt32(&action);

    Int32 hasClipData = 0;
    parcel->ReadInt32(&hasClipData);
    jobject jclipData = NULL;
    if (hasClipData != 0) {
        AutoPtr<IClipData> clipData;
        CClipData::New((IClipData**)&clipData);
        IParcelable* cdparcel = IParcelable::Probe(clipData);
        cdparcel->ReadFromParcel(parcel);
        jclipData = Util::ToJavaClipData(env, clipData);
    }

    Int32 hasClipDescription = 0;
    parcel->ReadInt32(&hasClipDescription);
    jobject jclipDescription = NULL;
    if (hasClipDescription != 0) {
        AutoPtr<IClipDescription> clipDescription;
        CClipDescription::New((IClipDescription**)&clipDescription);
        IParcelable* cdparcel = IParcelable::Probe(clipDescription);
        cdparcel->ReadFromParcel(parcel);
        jclipDescription = Util::ToJavaClipDescription(env, clipDescription);
    }

    AutoPtr<IInterface> localState;
    event->GetLocalState((IInterface**)&localState);
    jobject jlocalState = NULL;
    if (localState != NULL) {
        LOGGERE("ToJavaDragEvent", "localState NOT NULL!");
    }

    jmethodID m = env->GetStaticMethodID(deKlass, "obtain", "(IFFLjava/lang/Object;Landroid/content/ClipData/ClipDescription;Landroid/content/ClipDataZ;)Landroid/view/DragEvent;");
    Util::CheckErrorAndLog(env, "ToJavaDragEvent", "GetMethodID: obtain : %d", __LINE__);

    jobject jevent = env->CallStaticObjectMethod(deKlass, m, (jint)action, (jfloat)X, (jfloat)Y, jlocalState, jclipDescription, jclipData, (result == 1 ? TRUE : FALSE));
    Util::CheckErrorAndLog(env, "ToJavaDragEvent", "CallStaticObjectMethod: obtain : %d", __LINE__);

    env->DeleteLocalRef(deKlass);
    return jevent;
}

jobject Util::ToJavaMotionEvent(
    /* [in] */ JNIEnv* env,
    /* [in] */ IMotionEvent* event)
{
    if (env == NULL || event == NULL) {
        LOGGERE("ToJavaMotionEvent", "Invalid arguments!");
        return NULL;
    }

    jclass eventKlass = env->FindClass("android/view/MotionEvent");
    Util::CheckErrorAndLog(env, "ToJavaMotionEvent", "FindClass: MotionEvent line: %d", __LINE__);

    jmethodID m = env->GetMethodID(eventKlass, "<init>", "()V");
    Util::CheckErrorAndLog(env, "ToJavaMotionEvent", "GetMethodID: MotionEvent line: %d", __LINE__);

    jobject jevent = env->NewObject(eventKlass, m);
    Util::CheckErrorAndLog(env, "ToJavaMotionEvent", "NewObject: MotionEvent line: %d", __LINE__);

    Handle64 nativeEvent;
    event->GetNative(&nativeEvent);

    android::MotionEvent* destEvent = new android::MotionEvent();
    android::MotionEvent* sourceEvent = (android::MotionEvent*)nativeEvent;
    destEvent->copyFrom(sourceEvent, true);

    jfieldID f = env->GetFieldID(eventKlass, "mNativePtr", "J");
    Util::CheckErrorAndLog(env, "ToJavaMotionEvent", "GetFieldID: mNativePtr line: %d", __LINE__);

    env->SetLongField(jevent, f, reinterpret_cast<jlong>(destEvent));
    Util::CheckErrorAndLog(env, "ToJavaMotionEvent", "SetIntField: mNativePtr line: %d", __LINE__);

    env->DeleteLocalRef(eventKlass);

    return jevent;

}

jobject Util::ToJavaPackageStats(
    /* [in] */ JNIEnv* env,
    /* [in] */ IPackageStats* stats)
{
    if (env == NULL || stats == NULL) {
        LOGGERE("ToJavaPackageStats", "Invalid arguments!");
        return NULL;
    }

    jobject jstats = NULL;

    String packageName;
    stats->GetPackageName(&packageName);
    jstring jpackageName = Util::ToJavaString(env, packageName);
    Int32 userHandle;
    stats->GetUserHandle(&userHandle);

    jclass statsKlass = env->FindClass("android/content/pm/PackageStats");
    Util::CheckErrorAndLog(env, "ToJavaPackageStats", "FindClass: PackageStats line: %d", __LINE__);

    jmethodID m = env->GetMethodID(statsKlass, "<init>", "(Ljava/lang/String;I)V");
    Util::CheckErrorAndLog(env, "ToJavaPackageStats", "GetMethodID: PackageStats line: %d", __LINE__);

    jstats = env->NewObject(statsKlass, m, jpackageName, (jint)userHandle);
    Util::CheckErrorAndLog(env, "ToJavaPackageStats", "NewObject: PackageStats line: %d", __LINE__);

    Int64 tempLong;
    stats->GetCodeSize(&tempLong);
    Util::SetJavalongField(env, statsKlass, jstats, tempLong, "codeSize", "ToJavaPackageStats");

    stats->GetDataSize(&tempLong);
    Util::SetJavalongField(env, statsKlass, jstats, tempLong, "dataSize", "ToJavaPackageStats");

    stats->GetCacheSize(&tempLong);
    Util::SetJavalongField(env, statsKlass, jstats, tempLong, "cacheSize", "ToJavaPackageStats");

    stats->GetExternalCodeSize(&tempLong);
    Util::SetJavalongField(env, statsKlass, jstats, tempLong, "externalCodeSize", "ToJavaPackageStats");

    stats->GetExternalDataSize(&tempLong);
    Util::SetJavalongField(env, statsKlass, jstats, tempLong, "externalDataSize", "ToJavaPackageStats");

    stats->GetExternalCacheSize(&tempLong);
    Util::SetJavalongField(env, statsKlass, jstats, tempLong, "externalCacheSize", "ToJavaPackageStats");

    stats->GetExternalMediaSize(&tempLong);
    Util::SetJavalongField(env, statsKlass, jstats, tempLong, "externalMediaSize", "ToJavaPackageStats");

    stats->GetExternalObbSize(&tempLong);
    Util::SetJavalongField(env, statsKlass, jstats, tempLong, "externalObbSize", "ToJavaPackageStats");

    env->DeleteLocalRef(jpackageName);
    env->DeleteLocalRef(statsKlass);

    return jstats;
}

jobject Util::ToJavaHashMap(
    /* [in] */ JNIEnv* env,
    /* [in] */ IHashMap* map)
{
    if (env == NULL || map == NULL) {
        LOGGERE("ToJavaHashMap", "Invalid arguments!");
        return NULL;
    }

    jclass hmKlass = env->FindClass("java/util/HashMap");
    Util::CheckErrorAndLog(env, "ToJavaHashMap", "FindClass: HashMap line: %d", __LINE__);

    jmethodID m = env->GetMethodID(hmKlass, "<init>", "()V");
    Util::CheckErrorAndLog(env, "ToJavaHashMap", "GetMethodID: HashMap line: %d", __LINE__);

    jobject jmap = env->NewObject(hmKlass, m);
    Util::CheckErrorAndLog(env, "ToJavaHashMap", "NewObject: HashMap line: %d", __LINE__);

    m = env->GetMethodID(hmKlass, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
    Util::CheckErrorAndLog(env, "ToJavaHashMap", "GetMethodID: put line: %d", __LINE__);

    AutoPtr<ISet> set;
    map->GetKeySet((ISet**)&set);
    AutoPtr<ArrayOf<IInterface*> > keys;
    set->ToArray((ArrayOf<IInterface*>**)&keys);
    Int32 size = keys->GetLength();
    for (Int32 i = 0; i < size; i++) {
        String key;
        ICharSequence::Probe((*keys)[i])->ToString(&key);
        jstring jkey = Util::ToJavaString(env, key);

        AutoPtr<IInterface> value;
        map->Get((*keys)[i], (IInterface**)&value);
        if (ICharSequence::Probe(value) != NULL) {
            String strValue;
            ICharSequence::Probe(value)->ToString(&strValue);
            jstring jvalue = Util::ToJavaString(env, strValue);

            jobject jresult = env->CallObjectMethod(jmap, m, jkey, jvalue);
            Util::CheckErrorAndLog(env, "ToJavaHashMap", "CallObjectMethod: put line: %d", __LINE__);

            env->DeleteLocalRef(jvalue);
            env->DeleteLocalRef(jresult);
        }
        else if (ISerializable::Probe(value) != NULL){
            AutoPtr<IObject> object = IObject::Probe(value);
            ClassID clsid;
            object->GetClassID(&clsid);

            if (ECLSID_CISerializableNative == clsid) {
                jobject jserializable = Util::ElByteArrayToJavaObject(env, ISerializable::Probe(value));

                jobject jresult = env->CallObjectMethod(jmap, m, jkey, jserializable);
                Util::CheckErrorAndLog(env, "ToJavaHashMap", "CallObjectMethod: put line: %d", __LINE__);

                env->DeleteLocalRef(jserializable);
                env->DeleteLocalRef(jresult);
            }
            else {
                LOGGERE("ToJavaHashMap", "ToJavaHashMap() Unknown ISerializable value not implemented! key is:%s\n", key.string());
                DUMP_CLSID(clsid, "ToJavaBundle");
            }
        }
        else {
            LOGGERE("ToJavaHashMap", "Unsupported value!")
            AutoPtr<IObject> object = IObject::Probe(value);

            ClassID clsid;
            object->GetClassID(&clsid);
            DUMP_CLSID(clsid, "ToJavaHashMap()");
        }

        env->DeleteLocalRef(jkey);
    }

    env->DeleteLocalRef(hmKlass);

    return jmap;
}

jobject Util::ToJavaVpnProfile(
    /* [in] */ JNIEnv* env,
    /* [in] */ IVpnProfile* profile)
{
    if (env == NULL || profile == NULL) {
        LOGGERE("ToJavaVpnProfile", "Invalid arguments!");
        return NULL;
    }

    jclass vpKlass = env->FindClass("com/android/internal/net/VpnProfile");
    Util::CheckErrorAndLog(env, "ToJavaVpnProfile", "FindClass: VpnProfile line: %d", __LINE__);

    jmethodID m = env->GetMethodID(vpKlass, "<init>", "(Ljava/lang/String;)V");
    Util::CheckErrorAndLog(env, "ToJavaVpnProfile", "GetMethodID: VpnProfile line: %d", __LINE__);

    String key;
    profile->GetKey(&key);
    jstring jkey = Util::ToJavaString(env, key);

    jobject jprofile = env->NewObject(vpKlass, m, jkey);
    Util::CheckErrorAndLog(env, "ToJavaVpnProfile", "NewObject: VpnProfile line: %d", __LINE__);

    String tempString;
    profile->GetName(&tempString);
    SetJavaStringField(env, vpKlass, jprofile, tempString, "name", "ToJavaVpnProfile");

    Int32 tempInt;
    profile->GetType(&tempInt);
    Util::SetJavaIntField(env, vpKlass, jprofile, tempInt, "type", "ToJavaVpnProfile");

    profile->GetServer(&tempString);
    Util::SetJavaStringField(env, vpKlass, jprofile, tempString, "server", "ToJavaVpnProfile");

    profile->GetUsername(&tempString);
    Util::SetJavaStringField(env, vpKlass, jprofile, tempString, "username", "ToJavaVpnProfile");

    profile->GetPassword(&tempString);
    Util::SetJavaStringField(env, vpKlass, jprofile, tempString, "password", "ToJavaVpnProfile");

    profile->GetDnsServers(&tempString);
    Util::SetJavaStringField(env, vpKlass, jprofile, tempString, "dnsServers", "ToJavaVpnProfile");

    profile->GetSearchDomains(&tempString);
    Util::SetJavaStringField(env, vpKlass, jprofile, tempString, "searchDomains", "ToJavaVpnProfile");

    profile->GetRoutes(&tempString);
    Util::SetJavaStringField(env, vpKlass, jprofile, tempString, "routes", "ToJavaVpnProfile");

    Boolean booleanTemp;
    profile->GetMppe(&booleanTemp);
    Util::SetJavaBoolField(env, vpKlass, jprofile, booleanTemp, "mppe", "ToJavaVpnProfile");

    profile->GetL2tpSecret(&tempString);
    Util::SetJavaStringField(env, vpKlass, jprofile, tempString, "l2tpSecret", "ToJavaVpnProfile");

    profile->GetIpsecIdentifier(&tempString);
    Util::SetJavaStringField(env, vpKlass, jprofile, tempString, "ipsecIdentifier", "ToJavaVpnProfile");

    profile->GetIpsecSecret(&tempString);
    Util::SetJavaStringField(env, vpKlass, jprofile, tempString, "ipsecSecret", "ToJavaVpnProfile");

    profile->GetIpsecUserCert(&tempString);
    Util::SetJavaStringField(env, vpKlass, jprofile, tempString, "ipsecUserCert", "ToJavaVpnProfile");

    profile->GetIpsecCaCert(&tempString);
    Util::SetJavaStringField(env, vpKlass, jprofile, tempString, "ipsecCaCert", "ToJavaVpnProfile");

    profile->GetIpsecServerCert(&tempString);
    Util::SetJavaStringField(env, vpKlass, jprofile, tempString, "ipsecServerCert", "ToJavaVpnProfile");

    profile->GetSaveLogin(&booleanTemp);
    Util::SetJavaBoolField(env, vpKlass, jprofile, booleanTemp, "saveLogin", "ToJavaVpnProfile");

    env->DeleteLocalRef(jkey);
    env->DeleteLocalRef(vpKlass);

    return jprofile;
}

jobject Util::ToJavaDebugMemoryInfo(
    /* [in] */ JNIEnv* env,
    /* [in] */ IDebugMemoryInfo* info)
{
    if (env == NULL || info == NULL) {
        LOGGERE("ToJavaDebugMemoryInfo", "Invalid arguments!");
        return NULL;
    }

    jclass dmiKlass = env->FindClass("android/os/Debug$MemoryInfo");
    Util::CheckErrorAndLog(env, "ToJavaDebugMemoryInfo", "FindClass: MemoryInfo line: %d", __LINE__);

    jmethodID m = env->GetMethodID(dmiKlass, "<init>", "()V");
    Util::CheckErrorAndLog(env, "ToJavaDebugMemoryInfo", "GetMethodID: MemoryInfo line: %d", __LINE__);

    jobject jinfo = env->NewObject(dmiKlass, m);
    Util::CheckErrorAndLog(env, "ToJavaDebugMemoryInfo", "NewObject: VpnProfile line: %d", __LINE__);

    AutoPtr<IParcel> parcel;
    Elastos::Droid::Os::CParcel::New((IParcel**)&parcel);
    IParcelable::Probe(info)->WriteToParcel(parcel);
    parcel->SetDataPosition(0);

    Int32 tempInt;
    parcel->ReadInt32(&tempInt);
    Util::SetJavaIntField(env, dmiKlass, jinfo, tempInt, "dalvikPss", "ToJavaDebugMemoryInfo");

    parcel->ReadInt32(&tempInt);
    Util::SetJavaIntField(env, dmiKlass, jinfo, tempInt, "dalvikSwappablePss", "ToJavaDebugMemoryInfo");

    parcel->ReadInt32(&tempInt);
    Util::SetJavaIntField(env, dmiKlass, jinfo, tempInt, "dalvikPrivateDirty", "ToJavaDebugMemoryInfo");

    parcel->ReadInt32(&tempInt);
    Util::SetJavaIntField(env, dmiKlass, jinfo, tempInt, "dalvikSharedDirty", "ToJavaDebugMemoryInfo");

    parcel->ReadInt32(&tempInt);
    Util::SetJavaIntField(env, dmiKlass, jinfo, tempInt, "dalvikPrivateClean", "ToJavaDebugMemoryInfo");

    parcel->ReadInt32(&tempInt);
    Util::SetJavaIntField(env, dmiKlass, jinfo, tempInt, "dalvikSharedClean", "ToJavaDebugMemoryInfo");

    parcel->ReadInt32(&tempInt);
    Util::SetJavaIntField(env, dmiKlass, jinfo, tempInt, "dalvikSwappedOut", "ToJavaDebugMemoryInfo");

    parcel->ReadInt32(&tempInt);
    Util::SetJavaIntField(env, dmiKlass, jinfo, tempInt, "nativePss", "ToJavaDebugMemoryInfo");

    parcel->ReadInt32(&tempInt);
    Util::SetJavaIntField(env, dmiKlass, jinfo, tempInt, "nativeSwappablePss", "ToJavaDebugMemoryInfo");

    parcel->ReadInt32(&tempInt);
    Util::SetJavaIntField(env, dmiKlass, jinfo, tempInt, "nativePrivateDirty", "ToJavaDebugMemoryInfo");

    parcel->ReadInt32(&tempInt);
    Util::SetJavaIntField(env, dmiKlass, jinfo, tempInt, "nativeSharedDirty", "ToJavaDebugMemoryInfo");

    parcel->ReadInt32(&tempInt);
    Util::SetJavaIntField(env, dmiKlass, jinfo, tempInt, "nativePrivateClean", "ToJavaDebugMemoryInfo");

    parcel->ReadInt32(&tempInt);
    Util::SetJavaIntField(env, dmiKlass, jinfo, tempInt, "nativeSharedClean", "ToJavaDebugMemoryInfo");

    parcel->ReadInt32(&tempInt);
    Util::SetJavaIntField(env, dmiKlass, jinfo, tempInt, "nativeSwappedOut", "ToJavaDebugMemoryInfo");

    parcel->ReadInt32(&tempInt);
    Util::SetJavaIntField(env, dmiKlass, jinfo, tempInt, "otherPss", "ToJavaDebugMemoryInfo");

    parcel->ReadInt32(&tempInt);
    Util::SetJavaIntField(env, dmiKlass, jinfo, tempInt, "otherSwappablePss", "ToJavaDebugMemoryInfo");

    parcel->ReadInt32(&tempInt);
    Util::SetJavaIntField(env, dmiKlass, jinfo, tempInt, "otherPrivateDirty", "ToJavaDebugMemoryInfo");

    parcel->ReadInt32(&tempInt);
    Util::SetJavaIntField(env, dmiKlass, jinfo, tempInt, "otherSharedDirty", "ToJavaDebugMemoryInfo");

    parcel->ReadInt32(&tempInt);
    Util::SetJavaIntField(env, dmiKlass, jinfo, tempInt, "otherSharedClean", "ToJavaDebugMemoryInfo");

    parcel->ReadInt32(&tempInt);
    Util::SetJavaIntField(env, dmiKlass, jinfo, tempInt, "otherSwappedOut", "ToJavaDebugMemoryInfo");

    AutoPtr<ArrayOf<Int32> > otherStats;
    parcel->ReadArrayOf((Handle32*)(&otherStats));
    if (otherStats != NULL) {
        jfieldID f = env->GetFieldID(dmiKlass, "otherStats", "[I");
        Util::CheckErrorAndLog(env, "ToJavaDebugMemoryInfo", "GetFieldID: otherStats %d", __LINE__);

        jintArray otherIntArray = (jintArray)env->GetObjectField(jinfo, f);
        Util::CheckErrorAndLog(env, "ToJavaDebugMemoryInfo", "GetFieldID: otherStats %d", __LINE__);

        jint* otherArray = (jint*)env->GetPrimitiveArrayCritical(otherIntArray, 0);
        Util::CheckErrorAndLog(env, "ToJavaDebugMemoryInfo", "GetPrimitiveArrayCritical: otherStats %d", __LINE__);

        Int32 count = otherStats->GetLength();
        for (Int32 i = 0; i < count; i++) {
            otherArray[i] = (*otherStats)[i];
        }

        env->ReleasePrimitiveArrayCritical(otherIntArray, otherArray, 0);
        env->DeleteLocalRef(otherIntArray);
    }

    env->DeleteLocalRef(dmiKlass);
    return jinfo;
}

Boolean Util::GetElDebugMemoryInfo(
    /* [in] */ JNIEnv* env,
    /* [in] */ jobject jinfo,
    /* [out] */ IDebugMemoryInfo** info)
{
    if (env == NULL || jinfo == NULL || info == NULL) {
        LOGGERE("GetElDebugMemoryInfo", "Invalid arguments!");
        return FALSE;
    }

    assert(0);
    return FALSE;
    // if (NOERROR != CDebugMemoryInfo::New(info)) {
    //     return FALSE;
    // }

    // return GetElDebugMemoryInfo(env, jinfo, *info);
}

Boolean Util::GetElDebugMemoryInfo(
    /* [in] */ JNIEnv* env,
    /* [in] */ jobject jinfo,
    /* [in] */ IDebugMemoryInfo* info)
{
    if (env == NULL || jinfo == NULL || info == NULL) {
        LOGGERE("GetElDebugMemoryInfo", "Invalid arguments!");
        return FALSE;
    }

    jclass dmiKlass = env->FindClass("android/os/Debug$MemoryInfo");
    Util::CheckErrorAndLog(env, "GetElDebugMemoryInfo", "FindClass: MemoryInfo line: %d", __LINE__);

    AutoPtr<IParcel> parcel;
    CParcel::New((IParcel**)&parcel);

    Int32 tempInt = GetJavaIntField(env, dmiKlass, jinfo, "dalvikPss", "GetElDebugMemoryInfo");
    parcel->WriteInt32(tempInt);
    tempInt = GetJavaIntField(env, dmiKlass, jinfo, "dalvikSwappablePss", "GetElDebugMemoryInfo");
    parcel->WriteInt32(tempInt);
    tempInt = GetJavaIntField(env, dmiKlass, jinfo, "dalvikPrivateDirty", "GetElDebugMemoryInfo");
    parcel->WriteInt32(tempInt);
    tempInt = GetJavaIntField(env, dmiKlass, jinfo, "dalvikSharedDirty", "GetElDebugMemoryInfo");
    parcel->WriteInt32(tempInt);
    tempInt = GetJavaIntField(env, dmiKlass, jinfo, "dalvikPrivateClean", "GetElDebugMemoryInfo");
    parcel->WriteInt32(tempInt);
    tempInt = GetJavaIntField(env, dmiKlass, jinfo, "dalvikSharedClean", "GetElDebugMemoryInfo");
    parcel->WriteInt32(tempInt);
    tempInt = GetJavaIntField(env, dmiKlass, jinfo, "dalvikSwappedOut", "GetElDebugMemoryInfo");
    parcel->WriteInt32(tempInt);
    tempInt = GetJavaIntField(env, dmiKlass, jinfo, "nativePss", "GetElDebugMemoryInfo");
    parcel->WriteInt32(tempInt);
    tempInt = GetJavaIntField(env, dmiKlass, jinfo, "nativeSwappablePss", "GetElDebugMemoryInfo");
    parcel->WriteInt32(tempInt);
    tempInt = GetJavaIntField(env, dmiKlass, jinfo, "nativePrivateDirty", "GetElDebugMemoryInfo");
    parcel->WriteInt32(tempInt);
    tempInt = GetJavaIntField(env, dmiKlass, jinfo, "nativeSharedDirty", "GetElDebugMemoryInfo");
    parcel->WriteInt32(tempInt);
    tempInt = GetJavaIntField(env, dmiKlass, jinfo, "nativePrivateClean", "GetElDebugMemoryInfo");
    parcel->WriteInt32(tempInt);
    tempInt = GetJavaIntField(env, dmiKlass, jinfo, "nativeSharedClean", "GetElDebugMemoryInfo");
    parcel->WriteInt32(tempInt);
    tempInt = GetJavaIntField(env, dmiKlass, jinfo, "nativeSwappedOut", "GetElDebugMemoryInfo");
    parcel->WriteInt32(tempInt);
    tempInt = GetJavaIntField(env, dmiKlass, jinfo, "otherPss", "GetElDebugMemoryInfo");
    parcel->WriteInt32(tempInt);
    tempInt = GetJavaIntField(env, dmiKlass, jinfo, "otherSwappablePss", "GetElDebugMemoryInfo");
    parcel->WriteInt32(tempInt);
    tempInt = GetJavaIntField(env, dmiKlass, jinfo, "otherPrivateDirty", "GetElDebugMemoryInfo");
    parcel->WriteInt32(tempInt);
    tempInt = GetJavaIntField(env, dmiKlass, jinfo, "otherSharedDirty", "GetElDebugMemoryInfo");
    parcel->WriteInt32(tempInt);
    tempInt = GetJavaIntField(env, dmiKlass, jinfo, "otherPrivateClean", "GetElDebugMemoryInfo");
    parcel->WriteInt32(tempInt);
    tempInt = GetJavaIntField(env, dmiKlass, jinfo, "otherSharedClean", "GetElDebugMemoryInfo");
    parcel->WriteInt32(tempInt);
    tempInt = GetJavaIntField(env, dmiKlass, jinfo, "otherSwappedOut", "GetElDebugMemoryInfo");
    parcel->WriteInt32(tempInt);

    jfieldID f = env->GetFieldID(dmiKlass, "otherStats", "[I");
    Util::CheckErrorAndLog(env, "ToJavaDebugMemoryInfo", "GetFieldID: otherStats %d", __LINE__);

    jintArray jotherIntArray = (jintArray)env->GetObjectField(jinfo, f);
    Util::CheckErrorAndLog(env, "ToJavaDebugMemoryInfo", "GetFieldID: otherStats %d", __LINE__);
    AutoPtr<ArrayOf<Int32> > otherIntArray;
    Util::GetElInt32Array(env, jotherIntArray, (ArrayOf<Int32>**)&otherIntArray);
    parcel->WriteArrayOf((Handle32)otherIntArray.Get());

    parcel->SetDataPosition(0);
    AutoPtr<IParcelable> parcelInfo = IParcelable::Probe(info);
    parcelInfo->ReadFromParcel(parcel);

    env->DeleteLocalRef(dmiKlass);
    env->DeleteLocalRef(jotherIntArray);
    return TRUE;
}

jobject Util::ToJavaProviderRequest(
    /* [in] */ JNIEnv* env,
    /* [in] */ IProviderRequest* request)
{
    if (env == NULL || request == NULL) {
        LOGGERE("ToJavaProviderRequest", "Invalid arguments!");
        return NULL;
    }

    jclass prKlass = env->FindClass("com/android/internal/location/ProviderRequest");
    Util::CheckErrorAndLog(env, "ToJavaProviderRequest", "FindClass: ProviderRequest line: %d", __LINE__);

    jmethodID m = env->GetMethodID(prKlass, "<init>", "()V");
    Util::CheckErrorAndLog(env, "ToJavaProviderRequest", "GetMethodID: ProviderRequest line: %d", __LINE__);

    jobject jrequest = env->NewObject(prKlass, m);
    Util::CheckErrorAndLog(env, "ToJavaProviderRequest", "NewObject: ProviderRequest line: %d", __LINE__);

    Boolean reportLocation;
    request->GetReportLocation(&reportLocation);
    Util::SetJavaBoolField(env, prKlass, jrequest, reportLocation, "reportLocation", "ToJavaProviderRequest");

    Int64 interval;
    request->GetInterval(&interval);
    Util::SetJavalongField(env, prKlass, jrequest, interval, "interval", "ToJavaProviderRequest");

    AutoPtr<IList> list;
    request->GetLocationRequests((IList**)&list);
    if (list != NULL) {
        Int32 count = 0;
        list->GetSize(&count);
        if (count > 0) {
            jfieldID f = env->GetFieldID(prKlass, "locationRequests", "Ljava/util/List;");
            Util::CheckErrorAndLog(env, "ToJavaProviderRequest", "GetFieldID: locationRequests line: %d", __LINE__);

            jobject jlocationRequests = env->GetObjectField(jrequest, f);
            Util::CheckErrorAndLog(env, "ToJavaProviderRequest", "GetObjectField: locationRequests line: %d", __LINE__);

            jclass listKlass = env->FindClass("java/util/List");
            CheckErrorAndLog(env, "FindClass: List : %d!\n", __LINE__);

            jmethodID mAdd = env->GetMethodID(listKlass, "add", "(Ljava/lang/Object;)Z");
            CheckErrorAndLog(env, "GetMethodID: get : %d!\n", __LINE__);

            for (Int32 i = 0; i < count; i++) {
                AutoPtr<IInterface> obj;
                list->Get(i, (IInterface**)&obj);
                AutoPtr<ILocationRequest> route = ILocationRequest::Probe(obj);

                jobject jlocationRequest = Util::ToJavaLocationRequest(env, route);

                env->CallBooleanMethod(jlocationRequests, mAdd, jlocationRequest);
                Util::CheckErrorAndLog(env, "ToJavaProviderRequest", "CallBooleanMethod: add : %d!\n", __LINE__);

                env->DeleteLocalRef(jlocationRequest);
            }

            env->SetObjectField(jrequest, f, jlocationRequests);
            Util::CheckErrorAndLog(env, "ToJavaIntent", "SetObjectField: jdata  %d", __LINE__);

            env->DeleteLocalRef(jlocationRequests);
            env->DeleteLocalRef(listKlass);
        }
    }

    env->DeleteLocalRef(prKlass);

    return jrequest;
}

jobject Util::ToJavaLocationRequest(
    /* [in] */ JNIEnv* env,
    /* [in] */ ILocationRequest* request)
{
    if (env == NULL || request == NULL) {
        LOGGERE("ToJavaLocationRequest", "Invalid arguments!");
        return NULL;
    }

    jclass lrKlass = env->FindClass("android/location/LocationRequest");
    Util::CheckErrorAndLog(env, "ToJavaLocationRequest", "FindClass: LocationRequest line: %d", __LINE__);

    jmethodID m = env->GetMethodID(lrKlass, "<init>", "()V");
    Util::CheckErrorAndLog(env, "ToJavaLocationRequest", "GetMethodID: LocationRequest line: %d", __LINE__);

    jobject jrequest = env->NewObject(lrKlass, m);
    Util::CheckErrorAndLog(env, "ToJavaLocationRequest", "NewObject: LocationRequest line: %d", __LINE__);

    Int32 tempInt;
    request->GetQuality(&tempInt);
    Util::SetJavaIntField(env, lrKlass, jrequest, tempInt, "mQuality", "ToJavaLocationRequest");

    Int64 tempLong;
    request->GetFastestInterval(&tempLong);
    Util::SetJavalongField(env, lrKlass, jrequest, tempLong, "mFastestInterval", "ToJavaLocationRequest");

    request->GetInterval(&tempLong);
    Util::SetJavalongField(env, lrKlass, jrequest, tempLong, "mInterval", "ToJavaLocationRequest");

    request->GetExpireAt(&tempLong);
    Util::SetJavalongField(env, lrKlass, jrequest, tempLong, "mExpireAt", "ToJavaLocationRequest");

    request->GetNumUpdates(&tempInt);
    Util::SetJavaIntField(env, lrKlass, jrequest, tempInt, "mNumUpdates", "ToJavaLocationRequest");

    Float smallestDisplacement;
    request->GetSmallestDisplacement(&smallestDisplacement);
    SetJavafloatField(env, lrKlass, jrequest, smallestDisplacement, "mSmallestDisplacement", "ToJavaLocationRequest");

    Boolean hideFromAppOps;
    request->GetHideFromAppOps(&hideFromAppOps);
    SetJavaBoolField(env, lrKlass, jrequest, hideFromAppOps, "mHideFromAppOps", "ToJavaLocationRequest");

    String provider;
    request->GetProvider(&provider);
    SetJavaStringField(env, lrKlass, jrequest, provider, "mProvider", "ToJavaLocationRequest");

    AutoPtr<IWorkSource> workSource;
    request->GetWorkSource((IWorkSource**)&workSource);

    jobject jworkSource = ToJavaWorkSource(env, workSource);
    jfieldID f = env->GetFieldID(lrKlass, "mWorkSource", "Landroid/os/WorkSource;");
    Util::CheckErrorAndLog(env, "ToJavaLocationRequest", "GetFieldID: mWorkSource", __LINE__);

    env->SetObjectField(jrequest, f, jworkSource);
    Util::CheckErrorAndLog(env, "ToJavaLocationRequest", "SetObjectField: mWorkSource", __LINE__);
    env->DeleteLocalRef(jworkSource);

    env->DeleteLocalRef(lrKlass);

    return jrequest;
}

jobject Util::ToJavaWorkSource(
    /* [in] */ JNIEnv* env,
    /* [in] */ IWorkSource* ws)
{
    if (env == NULL || ws == NULL) {
        LOGGERE("ToJavaWorkSource", "Invalid arguments!");
        return NULL;
    }

    jclass wsKlass = env->FindClass("android/os/WorkSource");
    Util::CheckErrorAndLog(env, "ToJavaWorkSource", "FindClass: WorkSource line: %d", __LINE__);

    jmethodID m = env->GetMethodID(wsKlass, "<init>", "()V");
    Util::CheckErrorAndLog(env, "ToJavaWorkSource", "GetMethodID: WorkSource line: %d", __LINE__);

    jobject jws = env->NewObject(wsKlass, m);
    Util::CheckErrorAndLog(env, "ToJavaWorkSource", "NewObject: WorkSource line: %d", __LINE__);

    Int32 num = 0;
    ws->GetSize(&num);
    Util::SetJavaIntField(env, wsKlass, jws, num, "mNum", "WorkSource");

    if (num > 0) {
        m = env->GetMethodID(wsKlass, "set", "(ILjava/lang/String;)V");
        Util::CheckErrorAndLog(env, "ToJavaWorkSource", "GetMethodID: set line: %d", __LINE__);

        Int32 uid = -1;
        ws->Get(0, &uid);
        String name;
        ws->GetName(0, &name);
        jstring jname = ToJavaString(env, name);
        env->CallVoidMethod(jws, m, (jint)uid, jname);
        Util::CheckErrorAndLog(env, "ToJavaWorkSource", "CallBooleanMethod: set %d", __LINE__);
        env->DeleteLocalRef(jname);

        m = env->GetMethodID(wsKlass, "add", "(ILjava/lang/String;)Z");
        Util::CheckErrorAndLog(env, "ToJavaWorkSource", "GetMethodID: add line: %d", __LINE__);

        for (Int32 i = 1; i < num; i++) {
            ws->Get(i, &uid);
            ws->GetName(i, &name);
            jstring jname = ToJavaString(env, name);
            env->CallBooleanMethod(jws, m, (jint)uid, jname);
            Util::CheckErrorAndLog(env, "ToJavaWorkSource", "CallBooleanMethod: add %d", __LINE__);
            env->DeleteLocalRef(jname);
        }
    }

    env->DeleteLocalRef(wsKlass);

    return jws;
}

Boolean Util::GetElProviderProperties(
    /* [in] */ JNIEnv* env,
    /* [in] */ jobject jproperties,
    /* [out] */ IProviderProperties** properties)
{
    if (env == NULL || jproperties == NULL || properties == NULL) {
        LOGGERE("GetElProviderProperties", "Invalid arguments!");
        return FALSE;
    }

    jclass ppKlass = env->FindClass("com/android/internal/location/ProviderProperties");
    Util::CheckErrorAndLog(env, "GetElProviderProperties", "FindClass: ProviderProperties line: %d", __LINE__);

    Boolean requiresNetwork = (Boolean)GetJavaBoolField(env, jproperties, "mRequiresNetwork", "GetElProviderProperties");
    Boolean requiresSatellite = (Boolean)GetJavaBoolField(env, jproperties, "mRequiresSatellite", "GetElProviderProperties");
    Boolean requiresCell = (Boolean)GetJavaBoolField(env, jproperties, "mRequiresCell", "GetElProviderProperties");
    Boolean hasMonetaryCost = (Boolean)GetJavaBoolField(env, jproperties, "mHasMonetaryCost", "GetElProviderProperties");
    Boolean supportsAltitude = (Boolean)GetJavaBoolField(env, jproperties, "mSupportsAltitude", "GetElProviderProperties");
    Boolean supportsSpeed = (Boolean)GetJavaBoolField(env, jproperties, "mSupportsSpeed", "GetElProviderProperties");
    Boolean supportsBearing = (Boolean)GetJavaBoolField(env, jproperties, "mSupportsBearing", "GetElProviderProperties");
    Int32 powerRequirement = (Int32)GetJavaIntField(env, ppKlass, jproperties, "mPowerRequirement", "GetElProviderProperties");
    Int32 accuracy = (Int32)GetJavaIntField(env, ppKlass, jproperties, "mAccuracy", "GetElProviderProperties");

    CProviderProperties::New(requiresNetwork, requiresSatellite, requiresCell, hasMonetaryCost, supportsAltitude,
        supportsSpeed, supportsBearing, powerRequirement, accuracy, properties);

    env->DeleteLocalRef(ppKlass);
    return TRUE;
}

jobject Util::ToJavaCharArrayBuffer(
    /* [in] */ JNIEnv* env,
    /* [in] */ ICharArrayBuffer* buffer)
{
    if (env == NULL || buffer == NULL) {
        LOGGERE("ToJavaCharArrayBuffer", "Invalid arguments!");
        return NULL;
    }

    AutoPtr<ArrayOf<Char32> > data;
    buffer->GetData((ArrayOf<Char32>**)&data);
    if (data == NULL) {
        LOGGERE("ToJavaCharArrayBuffer", "data is NULL!");
        return NULL;
    }

    jcharArray jdata = Util::ToJavaCharArray(env, data);

    jclass cbKlass = env->FindClass("android/database/CharArrayBuffer");
    Util::CheckErrorAndLog(env, "ToJavaCharArrayBuffer", "FindClass: CharArrayBuffer line: %d", __LINE__);

    jmethodID m = env->GetMethodID(cbKlass, "<init>", "([C)V");
    Util::CheckErrorAndLog(env, "ToJavaCharArrayBuffer", "GetMethodID: CharArrayBuffer line: %d", __LINE__);

    jobject jbuffer = env->NewObject(cbKlass, m, jdata);
    Util::CheckErrorAndLog(env, "ToJavaCharArrayBuffer", "NewObject: CharArrayBuffer line: %d", __LINE__);

    Int32 sizeCopied = 0;
    buffer->GetSizeCopied(&sizeCopied);
    Util::SetJavaIntField(env, cbKlass, jbuffer, sizeCopied, "sizeCopied", "ToJavaCharArrayBuffer");

    env->DeleteLocalRef(cbKlass);

    return jbuffer;
}

jobject Util::ToJavaWifiP2pInfo(
    /* [in] */ JNIEnv* env,
    /* [in] */ IWifiP2pInfo* info)
{
    if (env == NULL || info == NULL) {
        LOGGERE("ToJavaWifiP2pInfo", "Invalid arguments!");
        return NULL;
    }

    jclass wpKlass = env->FindClass("android/net/wifi/p2p/WifiP2pInfo");
    Util::CheckErrorAndLog(env, "ToJavaWifiP2pInfo", "FindClass: WifiP2pInfo line: %d", __LINE__);

    jmethodID m = env->GetMethodID(wpKlass, "<init>", "()V");
    Util::CheckErrorAndLog(env, "ToJavaWifiP2pInfo", "GetMethodID: WifiP2pInfo line: %d", __LINE__);

    jobject jinfo = env->NewObject(wpKlass, m);
    Util::CheckErrorAndLog(env, "ToJavaWifiP2pInfo", "NewObject: WifiP2pInfo line: %d", __LINE__);

    Boolean groupFormed;
    info->GetGroupFormed(&groupFormed);
    Util::SetJavaBoolField(env, wpKlass, jinfo, groupFormed, "groupFormed", "ToJavaWifiP2pInfo");

    Boolean isGroupOwner;
    info->GetIsGroupOwner(&isGroupOwner);
    Util::SetJavaBoolField(env, wpKlass, jinfo, isGroupOwner, "isGroupOwner", "ToJavaWifiP2pInfo");

    AutoPtr<IInetAddress> groupOwnerAddress;
    info->GetGroupOwnerAddress((IInetAddress**)&groupOwnerAddress);
    if (groupOwnerAddress != NULL) {
        jobject jgroupOwnerAddress = Util::ToJavaInetAddress(env, groupOwnerAddress);

        jfieldID f = env->GetFieldID(wpKlass, "groupOwnerAddress", "Ljava/net/InetAddress;");
        Util::CheckErrorAndLog(env, "Fail GetFieldID: groupOwnerAddress : %d!\n", __LINE__);

        env->SetObjectField(jinfo, f, jgroupOwnerAddress);
        Util::CheckErrorAndLog(env, "Fail SetObjectField: groupOwnerAddress : %d!\n", __LINE__);
        env->DeleteLocalRef(jgroupOwnerAddress);
    }

    env->DeleteLocalRef(wpKlass);
    return jinfo;
}

jobject Util::ToJavaWifiP2pDevice(
    /* [in] */ JNIEnv* env,
    /* [in] */ IWifiP2pDevice* dev)
{
    if (env == NULL || dev == NULL) {
        LOGGERE("ToJavaWifiP2pDevice", "Invalid arguments!");
        return NULL;
    }

    jclass wpdKlass = env->FindClass("android/net/wifi/p2p/WifiP2pDevice");
    Util::CheckErrorAndLog(env, "ToJavaWifiP2pDevice", "FindClass: WifiP2pDevice line: %d", __LINE__);

    jmethodID m = env->GetMethodID(wpdKlass, "<init>", "()V");
    Util::CheckErrorAndLog(env, "ToJavaWifiP2pDevice", "GetMethodID: WifiP2pDevice line: %d", __LINE__);

    jobject jdev = env->NewObject(wpdKlass, m);
    Util::CheckErrorAndLog(env, "ToJavaWifiP2pDevice", "NewObject: WifiP2pDevice line: %d", __LINE__);

    String deviceName;
    dev->GetDeviceName(&deviceName);
    SetJavaStringField(env, wpdKlass, jdev, deviceName, "deviceName", "ToJavaWifiP2pDevice");

    String deviceAddress;
    dev->GetDeviceAddress(&deviceAddress);
    SetJavaStringField(env, wpdKlass, jdev, deviceAddress, "deviceAddress", "ToJavaWifiP2pDevice");

    String primaryDeviceType;
    dev->GetPrimaryDeviceType(&primaryDeviceType);
    SetJavaStringField(env, wpdKlass, jdev, primaryDeviceType, "primaryDeviceType", "ToJavaWifiP2pDevice");

    String secondaryDeviceType;
    dev->GetSecondaryDeviceType(&secondaryDeviceType);
    SetJavaStringField(env, wpdKlass, jdev, secondaryDeviceType, "secondaryDeviceType", "ToJavaWifiP2pDevice");

    Int32 tempInt = 0;
    dev->GetWpsConfigMethodsSupported(&tempInt);
    Util::SetJavaIntField(env, wpdKlass, jdev, tempInt, "wpsConfigMethodsSupported", "ToJavaWifiP2pDevice");

    dev->GetDeviceCapability(&tempInt);
    Util::SetJavaIntField(env, wpdKlass, jdev, tempInt, "deviceCapability", "ToJavaWifiP2pDevice");

    dev->GetGroupCapability(&tempInt);
    Util::SetJavaIntField(env, wpdKlass, jdev, tempInt, "groupCapability", "ToJavaWifiP2pDevice");

    dev->GetStatus(&tempInt);
    Util::SetJavaIntField(env, wpdKlass, jdev, tempInt, "status", "ToJavaWifiP2pDevice");

    AutoPtr<IWifiP2pWfdInfo> wfdInfo;
    dev->GetWfdInfo((IWifiP2pWfdInfo**)&wfdInfo);
    if (wfdInfo != NULL) {
        jobject jwfdInfo = Util::ToJavaWifiP2pWfdInfo(env, wfdInfo);

        jfieldID f = env->GetFieldID(wpdKlass, "wfdInfo", "Landroid/net/wifi/p2p/WifiP2pWfdInfo;");
        Util::CheckErrorAndLog(env, "Fail GetFieldID: WifiP2pWfdInfo : %d!\n", __LINE__);

        env->SetObjectField(jdev, f, jwfdInfo);
        Util::CheckErrorAndLog(env, "Fail SetObjectField: WifiP2pWfdInfo : %d!\n", __LINE__);
        env->DeleteLocalRef(jwfdInfo);
    }

    env->DeleteLocalRef(wpdKlass);
    return jdev;
}

jobject Util::ToJavaWifiP2pWfdInfo(
    /* [in] */ JNIEnv* env,
    /* [in] */ IWifiP2pWfdInfo* info)
{
    if (env == NULL || info == NULL) {
        LOGGERE("ToJavaWifiP2pWfdInfo", "Invalid arguments!");
        return NULL;
    }

    jclass wpwfdKlass = env->FindClass("android/net/wifi/p2p/WifiP2pWfdInfo");
    Util::CheckErrorAndLog(env, "ToJavaWifiP2pWfdInfo", "FindClass: WifiP2pWfdInfo line: %d", __LINE__);

    jmethodID m = env->GetMethodID(wpwfdKlass, "<init>", "()V");
    Util::CheckErrorAndLog(env, "ToJavaWifiP2pWfdInfo", "GetMethodID: WifiP2pWfdInfo line: %d", __LINE__);

    jobject jinfo = env->NewObject(wpwfdKlass, m);
    Util::CheckErrorAndLog(env, "ToJavaWifiP2pWfdInfo", "NewObject: WifiP2pWfdInfo line: %d", __LINE__);

    AutoPtr<IParcel> parcel;
    Elastos::Droid::Os::CParcel::New((IParcel**)&parcel);
    IParcelable::Probe(info)->WriteToParcel(parcel);
    parcel->SetDataPosition(0);

    Int32 tempInt = 0;
    parcel->ReadInt32(&tempInt);
    Util::SetJavaBoolField(env, wpwfdKlass, jinfo, tempInt == 1, "mWfdEnabled", "ToJavaWifiP2pWfdInfo");

    parcel->ReadInt32(&tempInt);
    Util::SetJavaIntField(env, wpwfdKlass, jinfo, tempInt, "mDeviceInfo", "ToJavaWifiP2pWfdInfo");

    parcel->ReadInt32(&tempInt);
    Util::SetJavaIntField(env, wpwfdKlass, jinfo, tempInt, "mCtrlPort", "ToJavaWifiP2pWfdInfo");

    parcel->ReadInt32(&tempInt);
    Util::SetJavaIntField(env, wpwfdKlass, jinfo, tempInt, "mMaxThroughput", "ToJavaWifiP2pWfdInfo");

    env->DeleteLocalRef(wpwfdKlass);
    return jinfo;
}

jobject Util::ToJavaWifiP2pDeviceList(
    /* [in] */ JNIEnv* env,
    /* [in] */ IWifiP2pDeviceList* info)
{
    if (env == NULL || info == NULL) {
        LOGGERE("ToJavaWifiP2pDeviceList", "Invalid arguments!");
        return NULL;
    }

    jclass wpdlKlass = env->FindClass("android/net/wifi/p2p/WifiP2pDeviceList");
    Util::CheckErrorAndLog(env, "ToJavaWifiP2pDeviceList", "FindClass: WifiP2pDeviceList line: %d", __LINE__);

    AutoPtr<ICollection> list;
    info->GetDeviceList((ICollection**)&list);
    AutoPtr<ArrayOf<IInterface*> > array;
    list->ToArray((ArrayOf<IInterface*>**)&array);

    jobject jinfo = NULL;

    Int32 count = array->GetLength();
    if (count > 0) {
        jclass listKlass = env->FindClass("java/util/ArrayList");
        Util::CheckErrorAndLog(env, "ToJavaWifiP2pDeviceList", "NewObject: ArrayList line: %d", __LINE__);

        jmethodID m = env->GetMethodID(listKlass, "<init>", "()V");
        Util::CheckErrorAndLog(env, "ToJavaWifiP2pDeviceList", "GetMethodID: ArrayList line: %d", __LINE__);

        jobject jlist = env->NewObject(listKlass, m);
        Util::CheckErrorAndLog(env, "ToJavaWifiP2pDeviceList", "NewObject: ArrayList line: %d", __LINE__);

        jmethodID mAdd = env->GetMethodID(listKlass, "add", "(Ljava/lang/Object;)Z");
        Util::CheckErrorAndLog(env, "ToJavaWifiP2pDeviceList", "GetMethodID: add line: %d", __LINE__);

        for (Int32 i = 0; i < count; i++) {
            jobject jdev = ToJavaWifiP2pDevice(env, IWifiP2pDevice::Probe((*array)[i]));
            env->CallBooleanMethod(jlist, mAdd, jdev);
            Util::CheckErrorAndLog(env, "ToJavaWifiP2pDeviceList", "CallBooleanMethod: add line: %d", __LINE__);

            env->DeleteLocalRef(jdev);
        }

        m = env->GetMethodID(wpdlKlass, "<init>", "(Ljava/util/ArrayList;)V");
        Util::CheckErrorAndLog(env, "ToJavaWifiP2pDeviceList", "GetMethodID: WifiP2pDeviceList line: %d", __LINE__);

        jinfo = env->NewObject(wpdlKlass, m, jlist);
        Util::CheckErrorAndLog(env, "ToJavaWifiP2pDeviceList", "NewObject: WifiP2pDeviceList line: %d", __LINE__);

        env->DeleteLocalRef(jlist);
        env->DeleteLocalRef(listKlass);
    }
    else {
        jmethodID m = env->GetMethodID(wpdlKlass, "<init>", "()V");
        Util::CheckErrorAndLog(env, "ToJavaWifiP2pDeviceList", "GetMethodID: WifiP2pDeviceList line: %d", __LINE__);

        jinfo = env->NewObject(wpdlKlass, m);
        Util::CheckErrorAndLog(env, "ToJavaWifiP2pDeviceList", "NewObject: WifiP2pDeviceList line: %d", __LINE__);
    }

    env->DeleteLocalRef(wpdlKlass);
    return jinfo;
}

jobject Util::ToJavaWifiP2pGroup(
    /* [in] */ JNIEnv* env,
    /* [in] */ IWifiP2pGroup* group)
{
    if (env == NULL || group == NULL) {
        LOGGERE("ToJavaWifiP2pGroup", "Invalid arguments!");
        return NULL;
    }

    jclass wpgKlass = env->FindClass("android/net/wifi/p2p/WifiP2pGroup");
    Util::CheckErrorAndLog(env, "ToJavaWifiP2pGroup", "FindClass: WifiP2pGroup line: %d", __LINE__);

    jmethodID m = env->GetMethodID(wpgKlass, "<init>", "()V");
    Util::CheckErrorAndLog(env, "ToJavaWifiP2pGroup", "GetMethodID: WifiP2pGroup line: %d", __LINE__);

    jobject jgroup = env->NewObject(wpgKlass, m);
    Util::CheckErrorAndLog(env, "ToJavaWifiP2pGroup", "NewObject: WifiP2pGroup line: %d", __LINE__);

    String networkName;
    group->GetNetworkName(&networkName);
    SetJavaStringField(env, wpgKlass, jgroup, networkName, "mNetworkName", "ToJavaWifiP2pGroup");

    AutoPtr<IWifiP2pDevice> owner;
    group->GetOwner((IWifiP2pDevice**)&owner);
    if (owner != NULL) {
        jobject jowner = Util::ToJavaWifiP2pDevice(env, owner);

        jfieldID f = env->GetFieldID(wpgKlass, "mOwner", "Landroid/net/wifi/p2p/WifiP2pDevice;");
        Util::CheckErrorAndLog(env, "Fail GetFieldID: WifiP2pDevice : %d!\n", __LINE__);

        env->SetObjectField(jgroup, f, jowner);
        Util::CheckErrorAndLog(env, "Fail SetObjectField: WifiP2pDevice : %d!\n", __LINE__);
        env->DeleteLocalRef(jowner);
    }

    Boolean isGroupOwner;
    group->IsGroupOwner(&isGroupOwner);
    Util::SetJavaBoolField(env, wpgKlass, jgroup, isGroupOwner, "mIsGroupOwner", "ToJavaWifiP2pGroup");

    AutoPtr<ICollection> list;
    group->GetClientList((ICollection**)&list);
    AutoPtr<ArrayOf<IInterface*> > clients;
    list->ToArray((ArrayOf<IInterface*>**)&clients);
    if (clients != NULL) {
        jfieldID f = env->GetFieldID(wpgKlass, "mClients", "Ljava/util/List;");
        Util::CheckErrorAndLog(env, "ToJavaWifiP2pGroup", "GetFieldID: mClients line: %d", __LINE__);

        jobject jclients = env->GetObjectField(jgroup, f);
        Util::CheckErrorAndLog(env, "ToJavaWifiP2pGroup", "GetObjectField: mClients line: %d", __LINE__);

        jclass listKlass = env->FindClass("java/util/List");
        CheckErrorAndLog(env, "ToJavaWifiP2pGroup", "FindClass: ArrayList : %d!", __LINE__);

        jmethodID mAdd = env->GetMethodID(listKlass, "add", "(Ljava/lang/Object;)Z");
        CheckErrorAndLog(env, "ToJavaWifiP2pGroup", "GetMethodID: add : %d!", __LINE__);

        for (Int32 i = 0; i < clients->GetLength(); i++) {
            jobject jclient = Util::ToJavaWifiP2pDevice(env, IWifiP2pDevice::Probe((*clients)[i]));

            env->CallBooleanMethod(jclients, mAdd, jclient);
            Util::CheckErrorAndLog(env, "ToJavaWifiP2pGroup", "CallBooleanMethod: add : %d!\n", __LINE__);

            env->DeleteLocalRef(jclient);
        }

        env->SetObjectField(jgroup, f, jclients);
        Util::CheckErrorAndLog(env, "ToJavaWifiP2pGroup", "Fail SetObjectField: jclients %d", __LINE__);

        env->DeleteLocalRef(jclients);
        env->DeleteLocalRef(listKlass);
    }

    String passphrase;
    group->GetPassphrase(&passphrase);
    SetJavaStringField(env, wpgKlass, jgroup, passphrase, "mPassphrase", "ToJavaWifiP2pGroup");

    String intrface;
    group->GetInterface(&intrface);
    SetJavaStringField(env, wpgKlass, jgroup, intrface, "mInterface", "ToJavaWifiP2pGroup");

    Int32 netId = 0;
    group->GetNetworkId(&netId);
    Util::SetJavaIntField(env, wpgKlass, jgroup, netId, "mNetId", "ToJavaWifiP2pGroup");

    env->DeleteLocalRef(wpgKlass);
    return jgroup;
}

jobject Util::ToJavaWifiConfiguration(
    /* [in] */ JNIEnv* env,
    /* [in] */ IWifiConfiguration* config)
{
    if (env == NULL || config == NULL) {
        LOGGERE("ToJavaWifiConfiguration", "Invalid arguments!");
        return NULL;
    }

    jclass wcKlass = env->FindClass("android/net/wifi/WifiConfiguration");
    Util::CheckErrorAndLog(env, "ToJavaWifiConfiguration", "FindClass: WifiConfiguration line: %d", __LINE__);

    jmethodID m = env->GetMethodID(wcKlass, "<init>", "()V");
    Util::CheckErrorAndLog(env, "ToJavaWifiConfiguration", "GetMethodID: WifiConfiguration line: %d", __LINE__);

    jobject jconfig = env->NewObject(wcKlass, m);
    Util::CheckErrorAndLog(env, "ToJavaWifiConfiguration", "NewObject: WifiConfiguration line: %d", __LINE__);

    Int32 tempInt = 0;
    config->GetNetworkId(&tempInt);
    Util::SetJavaIntField(env, wcKlass, jconfig, tempInt, "networkId", "ToJavaWifiConfiguration");

    config->GetStatus(&tempInt);
    Util::SetJavaIntField(env, wcKlass, jconfig, tempInt, "status", "ToJavaWifiConfiguration");

    config->GetDisableReason(&tempInt);
    Util::SetJavaIntField(env, wcKlass, jconfig, tempInt, "disableReason", "ToJavaWifiConfiguration");

    String SSID;
    config->GetSSID(&SSID);
    SetJavaStringField(env, wcKlass, jconfig, SSID, "SSID", "ToJavaWifiConfiguration");

    String BSSID;
    config->GetBSSID(&BSSID);
    SetJavaStringField(env, wcKlass, jconfig, BSSID, "BSSID", "ToJavaWifiConfiguration");

    String autoJoinBSSID;
    config->GetAutoJoinBSSID(&autoJoinBSSID);
    SetJavaStringField(env, wcKlass, jconfig, autoJoinBSSID, "autoJoinBSSID", "ToJavaWifiConfiguration");

    String FQDN;
    config->GetFQDN(&FQDN);
    SetJavaStringField(env, wcKlass, jconfig, FQDN, "FQDN", "ToJavaWifiConfiguration");

    String naiRealm;
    config->GetNaiRealm(&naiRealm);
    SetJavaStringField(env, wcKlass, jconfig, naiRealm, "naiRealm", "ToJavaWifiConfiguration");

    String preSharedKey;
    config->GetPreSharedKey(&preSharedKey);
    SetJavaStringField(env, wcKlass, jconfig, preSharedKey, "preSharedKey", "ToJavaWifiConfiguration");

    AutoPtr<ArrayOf<String> > wepKeys;
    config->GetWepKeys((ArrayOf<String>**)&wepKeys);
    if (wepKeys != NULL) {
        jobjectArray jwepKeys = Util::ToJavaStringArray(env, wepKeys);

        jfieldID f = env->GetFieldID(wcKlass, "wepKeys", "[Ljava/lang/String;");
        Util::CheckErrorAndLog(env, "Fail GetFieldID: wepKeys : %d!\n", __LINE__);

        env->SetObjectField(jconfig, f, jwepKeys);
        Util::CheckErrorAndLog(env, "Fail SetObjectField: wepKeys : %d!\n", __LINE__);
        env->DeleteLocalRef(jwepKeys);
    }

    config->GetWepTxKeyIndex(&tempInt);
    Util::SetJavaIntField(env, wcKlass, jconfig, tempInt, "wepTxKeyIndex", "ToJavaWifiConfiguration");

    config->GetPriority(&tempInt);
    Util::SetJavaIntField(env, wcKlass, jconfig, tempInt, "priority", "ToJavaWifiConfiguration");

    Boolean tempBool;
    config->GetHiddenSSID(&tempBool);
    Util::SetJavaBoolField(env, wcKlass, jconfig, tempBool, "hiddenSSID", "ToJavaWifiConfiguration");

    // TODO: CM12
    // config->GetIsIBSS(&tempBool);
    // Util::SetJavaBoolField(env, wcKlass, jconfig, tempBool, "isIBSS", "ToJavaWifiConfiguration");

    // config->GetFrequency(&tempInt);
    // Util::SetJavaIntField(env, wcKlass, jconfig, tempInt, "frequency", "ToJavaWifiConfiguration");

    config->GetRequirePMF(&tempBool);
    Util::SetJavaBoolField(env, wcKlass, jconfig, tempBool, "requirePMF", "ToJavaWifiConfiguration");

    String updateIdentifier;
    config->GetUpdateIdentifier(&updateIdentifier);
    SetJavaStringField(env, wcKlass, jconfig, updateIdentifier, "updateIdentifier", "ToJavaWifiConfiguration");

    AutoPtr<IBitSet> allowedKeyManagement;
    config->GetAllowedKeyManagement((IBitSet**)&allowedKeyManagement);
    if (allowedKeyManagement != NULL) {
        jobject jallowedKeyManagement = Util::ToJavaBitSet(env, allowedKeyManagement);

        jfieldID f = env->GetFieldID(wcKlass, "allowedKeyManagement", "Ljava/util/BitSet;");
        Util::CheckErrorAndLog(env, "Fail GetFieldID: allowedKeyManagement : %d!\n", __LINE__);

        env->SetObjectField(jconfig, f, jallowedKeyManagement);
        Util::CheckErrorAndLog(env, "Fail SetObjectField: allowedKeyManagement : %d!\n", __LINE__);
        env->DeleteLocalRef(jallowedKeyManagement);
    }

    AutoPtr<IBitSet> allowedProtocols;
    config->GetAllowedProtocols((IBitSet**)&allowedProtocols);
    if (allowedProtocols != NULL) {
        jobject jallowedProtocols = Util::ToJavaBitSet(env, allowedProtocols);

        jfieldID f = env->GetFieldID(wcKlass, "allowedProtocols", "Ljava/util/BitSet;");
        Util::CheckErrorAndLog(env, "Fail GetFieldID: allowedProtocols : %d!\n", __LINE__);

        env->SetObjectField(jconfig, f, jallowedProtocols);
        Util::CheckErrorAndLog(env, "Fail SetObjectField: allowedProtocols : %d!\n", __LINE__);
        env->DeleteLocalRef(jallowedProtocols);
    }

    AutoPtr<IBitSet> allowedAuthAlgorithms;
    config->GetAllowedAuthAlgorithms((IBitSet**)&allowedAuthAlgorithms);
    if (allowedAuthAlgorithms != NULL) {
        jobject jallowedAuthAlgorithms = Util::ToJavaBitSet(env, allowedAuthAlgorithms);

        jfieldID f = env->GetFieldID(wcKlass, "allowedAuthAlgorithms", "Ljava/util/BitSet;");
        Util::CheckErrorAndLog(env, "Fail GetFieldID: allowedAuthAlgorithms : %d!\n", __LINE__);

        env->SetObjectField(jconfig, f, jallowedAuthAlgorithms);
        Util::CheckErrorAndLog(env, "Fail SetObjectField: allowedAuthAlgorithms : %d!\n", __LINE__);
        env->DeleteLocalRef(jallowedAuthAlgorithms);
    }

    AutoPtr<IBitSet> allowedPairwiseCiphers;
    config->GetAllowedPairwiseCiphers((IBitSet**)&allowedPairwiseCiphers);
    if (allowedPairwiseCiphers != NULL) {
        jobject jallowedPairwiseCiphers = Util::ToJavaBitSet(env, allowedPairwiseCiphers);

        jfieldID f = env->GetFieldID(wcKlass, "allowedPairwiseCiphers", "Ljava/util/BitSet;");
        Util::CheckErrorAndLog(env, "Fail GetFieldID: allowedPairwiseCiphers : %d!\n", __LINE__);

        env->SetObjectField(jconfig, f, jallowedPairwiseCiphers);
        Util::CheckErrorAndLog(env, "Fail SetObjectField: allowedPairwiseCiphers : %d!\n", __LINE__);
        env->DeleteLocalRef(jallowedPairwiseCiphers);
    }

    AutoPtr<IBitSet> allowedGroupCiphers;
    config->GetAllowedGroupCiphers((IBitSet**)&allowedGroupCiphers);
    if (allowedGroupCiphers != NULL) {
        jobject jallowedGroupCiphers = Util::ToJavaBitSet(env, allowedGroupCiphers);

        jfieldID f = env->GetFieldID(wcKlass, "allowedGroupCiphers", "Ljava/util/BitSet;");
        Util::CheckErrorAndLog(env, "Fail GetFieldID: allowedGroupCiphers : %d!\n", __LINE__);

        env->SetObjectField(jconfig, f, jallowedGroupCiphers);
        Util::CheckErrorAndLog(env, "Fail SetObjectField: allowedGroupCiphers : %d!\n", __LINE__);
        env->DeleteLocalRef(jallowedGroupCiphers);
    }

    AutoPtr<IWifiEnterpriseConfig> enterpriseConfig;
    config->GetEnterpriseConfig((IWifiEnterpriseConfig**)&enterpriseConfig);
    if (enterpriseConfig != NULL) {
        jobject jenterpriseConfig = Util::ToJavaWifiEnterpriseConfig(env, enterpriseConfig);

        jfieldID f = env->GetFieldID(wcKlass, "enterpriseConfig", "Landroid/net/wifi/WifiEnterpriseConfig;");
        Util::CheckErrorAndLog(env, "ToJavaWifiConfiguration", "Fail GetFieldID: enterpriseConfig : %d!\n", __LINE__);

        env->SetObjectField(jconfig, f, jenterpriseConfig);
        Util::CheckErrorAndLog(env, "ToJavaWifiConfiguration", "Fail SetObjectField: enterpriseConfig : %d!\n", __LINE__);

        env->DeleteLocalRef(jenterpriseConfig);
    }

    AutoPtr<IIpConfiguration> ipConfiguration;
    config->GetIpConfiguration((IIpConfiguration**)&ipConfiguration);
    if (ipConfiguration != NULL) {
        jobject jipConfiguration = Util::ToJavaIpConfiguration(env, ipConfiguration);

        jfieldID f = env->GetFieldID(wcKlass, "mIpConfiguration", "Landroid/net/IpConfiguration;");
        Util::CheckErrorAndLog(env, "ToJavaWifiConfiguration", "Fail GetFieldID: mIpConfiguration : %d!\n", __LINE__);

        env->SetObjectField(jconfig, f, jipConfiguration);
        Util::CheckErrorAndLog(env, "ToJavaWifiConfiguration", "Fail SetObjectField: mIpConfiguration : %d!\n", __LINE__);

        env->DeleteLocalRef(jipConfiguration);
    }

    String dhcpServer;
    config->GetDhcpServer(&dhcpServer);
    SetJavaStringField(env, wcKlass, jconfig, dhcpServer, "dhcpServer", "ToJavaWifiConfiguration");

    String defaultGwMacAddress;
    config->GetDefaultGwMacAddress(&defaultGwMacAddress);
    SetJavaStringField(env, wcKlass, jconfig, defaultGwMacAddress, "defaultGwMacAddress", "ToJavaWifiConfiguration");

    config->GetAutoJoinStatus(&tempInt);
    Util::SetJavaIntField(env, wcKlass, jconfig, tempInt, "autoJoinStatus", "ToJavaWifiConfiguration");

    config->GetSelfAdded(&tempBool);
    Util::SetJavaBoolField(env, wcKlass, jconfig, tempBool, "selfAdded", "ToJavaWifiConfiguration");

    config->GetDidSelfAdd(&tempBool);
    Util::SetJavaBoolField(env, wcKlass, jconfig, tempBool, "didSelfAdd", "ToJavaWifiConfiguration");

    config->GetNoInternetAccess(&tempBool);
    Util::SetJavaBoolField(env, wcKlass, jconfig, tempBool, "noInternetAccess", "ToJavaWifiConfiguration");

    config->GetCreatorUid(&tempInt);
    Util::SetJavaIntField(env, wcKlass, jconfig, tempInt, "creatorUid", "ToJavaWifiConfiguration");

    config->GetLastConnectUid(&tempInt);
    Util::SetJavaIntField(env, wcKlass, jconfig, tempInt, "lastConnectUid", "ToJavaWifiConfiguration");

    config->GetLastUpdateUid(&tempInt);
    Util::SetJavaIntField(env, wcKlass, jconfig, tempInt, "lastUpdateUid", "ToJavaWifiConfiguration");

    Int64 blackListTimestamp;
    config->GetBlackListTimestamp(&blackListTimestamp);
    Util::SetJavaIntField(env, wcKlass, jconfig, blackListTimestamp, "blackListTimestamp", "ToJavaWifiConfiguration");

    Int64 lastConnectionFailure;
    config->GetLastConnectionFailure(&lastConnectionFailure);
    Util::SetJavaIntField(env, wcKlass, jconfig, lastConnectionFailure, "lastConnectionFailure", "ToJavaWifiConfiguration");

    config->GetNumConnectionFailures(&tempInt);
    Util::SetJavaIntField(env, wcKlass, jconfig, tempInt, "numConnectionFailures", "ToJavaWifiConfiguration");

    config->GetNumIpConfigFailures(&tempInt);
    Util::SetJavaIntField(env, wcKlass, jconfig, tempInt, "numIpConfigFailures", "ToJavaWifiConfiguration");

    config->GetNumAuthFailures(&tempInt);
    Util::SetJavaIntField(env, wcKlass, jconfig, tempInt, "numAuthFailures", "ToJavaWifiConfiguration");

    config->GetNumScorerOverride(&tempInt);
    Util::SetJavaIntField(env, wcKlass, jconfig, tempInt, "numScorerOverride", "ToJavaWifiConfiguration");

    config->GetNumScorerOverrideAndSwitchedNetwork(&tempInt);
    Util::SetJavaIntField(env, wcKlass, jconfig, tempInt, "numScorerOverrideAndSwitchedNetwork", "ToJavaWifiConfiguration");

    config->GetNumAssociation(&tempInt);
    Util::SetJavaIntField(env, wcKlass, jconfig, tempInt, "numAssociation", "ToJavaWifiConfiguration");

    config->GetNumUserTriggeredWifiDisableLowRSSI(&tempInt);
    Util::SetJavaIntField(env, wcKlass, jconfig, tempInt, "numUserTriggeredWifiDisableLowRSSI", "ToJavaWifiConfiguration");

    config->GetNumUserTriggeredWifiDisableBadRSSI(&tempInt);
    Util::SetJavaIntField(env, wcKlass, jconfig, tempInt, "numUserTriggeredWifiDisableBadRSSI", "ToJavaWifiConfiguration");

    config->GetNumUserTriggeredWifiDisableNotHighRSSI(&tempInt);
    Util::SetJavaIntField(env, wcKlass, jconfig, tempInt, "numUserTriggeredWifiDisableNotHighRSSI", "ToJavaWifiConfiguration");

    config->GetNumTicksAtLowRSSI(&tempInt);
    Util::SetJavaIntField(env, wcKlass, jconfig, tempInt, "numTicksAtLowRSSI", "ToJavaWifiConfiguration");

    config->GetNumTicksAtBadRSSI(&tempInt);
    Util::SetJavaIntField(env, wcKlass, jconfig, tempInt, "numTicksAtBadRSSI", "ToJavaWifiConfiguration");

    config->GetNumTicksAtNotHighRSSI(&tempInt);
    Util::SetJavaIntField(env, wcKlass, jconfig, tempInt, "numTicksAtNotHighRSSI", "ToJavaWifiConfiguration");

    config->GetNumUserTriggeredJoinAttempts(&tempInt);
    Util::SetJavaIntField(env, wcKlass, jconfig, tempInt, "numUserTriggeredJoinAttempts", "ToJavaWifiConfiguration");

    config->GetAutoJoinUseAggressiveJoinAttemptThreshold(&tempInt);
    Util::SetJavaIntField(env, wcKlass, jconfig, tempInt, "autoJoinUseAggressiveJoinAttemptThreshold", "ToJavaWifiConfiguration");

    config->GetAutoJoinBailedDueToLowRssi(&tempBool);
    Util::SetJavaBoolField(env, wcKlass, jconfig, tempBool, "autoJoinBailedDueToLowRssi", "ToJavaWifiConfiguration");

    env->DeleteLocalRef(wcKlass);
    return jconfig;
}

jobject Util::ToJavaWifiEnterpriseConfig(
    /* [in] */ JNIEnv* env,
    /* [in] */ IWifiEnterpriseConfig* config)
{
    if (env == NULL || config == NULL) {
        LOGGERE("ToJavaWifiEnterpriseConfig", "Invalid arguments!");
        return NULL;
    }

    jclass parcelClass = env->FindClass("android/os/Parcel");
    Util::CheckErrorAndLog(env, "ToJavaWifiEnterpriseConfig", "FindClass: Parcel : %d", __LINE__);

    jmethodID m = env->GetStaticMethodID(parcelClass, "obtain", "()Landroid/os/Parcel;");
    Util::CheckErrorAndLog(env, "ToJavaWifiEnterpriseConfig", "GetStaticMethodID: obtain : %d", __LINE__);

    jobject jparcel = env->CallStaticObjectMethod(parcelClass, m);
    Util::CheckErrorAndLog(env, "ToJavaWifiEnterpriseConfig", "CallStaticObjectMethod: obtain : %d", __LINE__);

    jmethodID mWriteInt = env->GetMethodID(parcelClass, "writeInt", "(I)V");
    jmethodID mWriteString = env->GetMethodID(parcelClass, "writeString", "(Ljava/lang/String;)V");
    jmethodID mWriteByteArray = env->GetMethodID(parcelClass, "writeByteArray", "([B)V");

    AutoPtr<IParcel> source;
    CParcel::New((IParcel**)&source);
    IParcelable::Probe(config)->WriteToParcel(source);
    source->SetDataPosition(0);

    Int32 count;
    source->ReadInt32(&count);
    env->CallVoidMethod(jparcel, mWriteInt, count);
    CheckErrorAndLog(env, "ToJavaWifiEnterpriseConfig", "CallVoidMethod: writeInt", __LINE__);
    for (Int32 i = 0; i < count; i++) {
        String key, value;
        source->ReadString(&key);
        source->ReadString(&value);
        jstring jkey = ToJavaString(env, key);
        jstring jvalue = ToJavaString(env, value);
        env->CallVoidMethod(jparcel, mWriteString, jkey);
        CheckErrorAndLog(env, "ToJavaWifiEnterpriseConfig", "CallVoidMethod: writeString", __LINE__);
        env->CallVoidMethod(jparcel, mWriteString, jvalue);
        CheckErrorAndLog(env, "ToJavaWifiEnterpriseConfig", "CallVoidMethod: writeString", __LINE__);
        env->DeleteLocalRef(jkey);
        env->DeleteLocalRef(jvalue);
    }

    Int32 len;
    source->ReadInt32(&len);
    env->CallVoidMethod(jparcel, mWriteInt, len);
    CheckErrorAndLog(env, "ToJavaWifiEnterpriseConfig", "CallVoidMethod: writeInt", __LINE__);
    if (len > 0) {
        AutoPtr<ArrayOf<Byte> > byteArray;
        source->ReadArrayOf((Handle32*)&byteArray);
        jbyteArray jbytearray = ToJavaByteArray(env, byteArray);
        env->CallVoidMethod(jparcel, mWriteByteArray, jbytearray);
        CheckErrorAndLog(env, "ToJavaWifiEnterpriseConfig", "CallVoidMethod: writeByteArray", __LINE__);
        env->DeleteLocalRef(jbytearray);
    }

    source->ReadInt32(&len);
    env->CallVoidMethod(jparcel, mWriteInt, len);
    CheckErrorAndLog(env, "ToJavaWifiEnterpriseConfig", "CallVoidMethod: writeInt", __LINE__);
    if (len > 0) {
        AutoPtr<ArrayOf<Byte> > byteArray;
        source->ReadArrayOf((Handle32*)&byteArray);
        jbyteArray jbytearray = ToJavaByteArray(env, byteArray);
        env->CallVoidMethod(jparcel, mWriteByteArray, jbytearray);
        CheckErrorAndLog(env, "ToJavaWifiEnterpriseConfig", "CallVoidMethod: writeByteArray", __LINE__);
        env->DeleteLocalRef(jbytearray);
        String algorithm;
        source->ReadString(&algorithm);
        jstring jalgorithm = ToJavaString(env, algorithm);
        env->CallVoidMethod(jparcel, mWriteString, jalgorithm);
        CheckErrorAndLog(env, "ToJavaWifiEnterpriseConfig", "CallVoidMethod: writeString", __LINE__);
        env->DeleteLocalRef(jalgorithm);
    }

    source->ReadInt32(&len);
    env->CallVoidMethod(jparcel, mWriteInt, len);
    CheckErrorAndLog(env, "ToJavaWifiEnterpriseConfig", "CallVoidMethod: writeInt", __LINE__);
    if (len > 0) {
        AutoPtr<ArrayOf<Byte> > byteArray;
        source->ReadArrayOf((Handle32*)&byteArray);
        jbyteArray jbytearray = ToJavaByteArray(env, byteArray);
        env->CallVoidMethod(jparcel, mWriteByteArray, jbytearray);
        CheckErrorAndLog(env, "ToJavaWifiEnterpriseConfig", "CallVoidMethod: writeByteArray", __LINE__);
        env->DeleteLocalRef(jbytearray);
    }

    jmethodID mSetDataPosition = env->GetMethodID(parcelClass, "setDataPosition", "(I)V");
    env->CallVoidMethod(jparcel, mSetDataPosition, 0);
    CheckErrorAndLog(env, "ToJavaWifiEnterpriseConfig", "CallVoidMethod: setDataPosition", __LINE__);

    jclass wecKlass = env->FindClass("android/net/wifi/WifiEnterpriseConfig");
    Util::CheckErrorAndLog(env, "ToJavaWifiEnterpriseConfig", "FindClass: WifiEnterpriseConfig line: %d", __LINE__);

    jfieldID f = env->GetStaticFieldID(wecKlass, "CREATOR", "Landroid/os/Parcelable$Creator;");
    Util::CheckErrorAndLog(env, "ToJavaWifiEnterpriseConfig", "Fail GetStaticFieldID: SIMPLIFIED_CHINESE : %d", __LINE__);

    jobject jcreater = env->GetStaticObjectField(wecKlass, f);
    Util::CheckErrorAndLog(env, "ToJavaWifiEnterpriseConfig", "Fail GetStaticObjectField: %d", __LINE__);

    jclass cKlass = env->GetObjectClass(jcreater);
    Util::CheckErrorAndLog(env, "ToJavaWifiEnterpriseConfig", "GetObjectClass: Parcelable$Creator line: %d", __LINE__);

    m = env->GetMethodID(cKlass, "createFromParcel", "(Landroid/os/Parcel;)Landroid/net/wifi/WifiEnterpriseConfig;");
    Util::CheckErrorAndLog(env, "ToJavaWifiEnterpriseConfig", "GetMethodID: WifiEnterpriseConfig line: %d", __LINE__);

    jobject jconfig = env->CallObjectMethod(jcreater, m, jparcel);
    Util::CheckErrorAndLog(env, "ToJavaWifiEnterpriseConfig", "NewObject: WifiEnterpriseConfig line: %d", __LINE__);

    env->DeleteLocalRef(parcelClass);
    env->DeleteLocalRef(jparcel);
    env->DeleteLocalRef(wecKlass);
    env->DeleteLocalRef(jcreater);
    env->DeleteLocalRef(cKlass);
    return jconfig;
}

jobject Util::ToJavaIpConfiguration(
    /* [in] */ JNIEnv* env,
    /* [in] */ IIpConfiguration* config)
{
    if (env == NULL || config == NULL) {
        LOGGERE("ToJavaIpConfiguration", "Invalid arguments!");
        return NULL;
    }

    jclass ipaClass = env->FindClass("android/net/IpConfiguration$IpAssignment");
    Util::CheckErrorAndLog(env, "ToJavaWifiConfiguration", "Fail FindClass IpAssignment %d", __LINE__);

    Elastos::Droid::Net::IpConfigurationIpAssignment ipAssignment;
    config->GetIpAssignment(&ipAssignment);
    jobject jipAssignment = NULL;
    switch (ipAssignment) {
        case Elastos::Droid::Net::STATIC_IpAssignment: {
            jfieldID f = env->GetStaticFieldID(ipaClass, "STATIC", "Landroid/net/IpConfiguration$IpAssignment;");
            CheckErrorAndLog(env,"ToJavaIpConfiguration", "GetStaticFieldID: STATIC : %d!\n", __LINE__);

            jipAssignment = env->GetStaticObjectField(ipaClass, f);
            CheckErrorAndLog(env,"ToJavaIpConfiguration", "GetStaticObjectField: STATIC : %d!\n", __LINE__);
            break;
        }
        case Elastos::Droid::Net::DHCP_IpAssignment: {
            jfieldID f = env->GetStaticFieldID(ipaClass, "DHCP", "Landroid/net/IpConfiguration$IpAssignment;");
            CheckErrorAndLog(env,"ToJavaIpConfiguration", "GetStaticFieldID: DHCP : %d!\n", __LINE__);

            jipAssignment = env->GetStaticObjectField(ipaClass, f);
            CheckErrorAndLog(env,"ToJavaIpConfiguration", "GetStaticObjectField: DHCP : %d!\n", __LINE__);
            break;
        }
        default: {
            jfieldID f = env->GetStaticFieldID(ipaClass, "UNASSIGNED", "Landroid/net/IpConfiguration$IpAssignment;");
            CheckErrorAndLog(env,"ToJavaIpConfiguration", "GetStaticFieldID: UNASSIGNED : %d!\n", __LINE__);

            jipAssignment = env->GetStaticObjectField(ipaClass, f);
            CheckErrorAndLog(env,"ToJavaIpConfiguration", "GetStaticObjectField: UNASSIGNED : %d!\n", __LINE__);
            break;
        }
    }

    jclass psClass = env->FindClass("android/net/IpConfiguration$ProxySettings");
    Util::CheckErrorAndLog(env, "ToJavaIpConfiguration", "Fail FindClass ProxySettings %d", __LINE__);

    Elastos::Droid::Net::IpConfigurationProxySettings proxySettings;
    config->GetIpAssignment(&proxySettings);
    jobject jproxySettings = NULL;
    switch(proxySettings) {
        case Elastos::Droid::Net::NONE_ProxySettings: {
            jfieldID f = env->GetStaticFieldID(psClass, "NONE", "Landroid/net/IpConfiguration$ProxySettings;");
            CheckErrorAndLog(env,"ToJavaIpConfiguration", "GetStaticFieldID: NONE : %d!\n", __LINE__);

            jproxySettings = env->GetStaticObjectField(psClass, f);
            CheckErrorAndLog(env,"ToJavaIpConfiguration", "GetStaticObjectField: STATIC : %d!\n", __LINE__);
            break;
        }
        case Elastos::Droid::Net::STATIC_ProxySettings: {
            jfieldID f = env->GetStaticFieldID(psClass, "STATIC", "Landroid/net/IpConfiguration$ProxySettings;");
            CheckErrorAndLog(env,"ToJavaIpConfiguration", "GetStaticFieldID: STATIC : %d!\n", __LINE__);

            jproxySettings = env->GetStaticObjectField(psClass, f);
            CheckErrorAndLog(env,"ToJavaIpConfiguration", "GetStaticObjectField: STATIC : %d!\n", __LINE__);
            break;
        }
        case Elastos::Droid::Net::PAC_ProxySettings: {
            jfieldID f = env->GetStaticFieldID(psClass, "PAC", "Landroid/net/IpConfiguration$ProxySettings;");
            CheckErrorAndLog(env,"ToJavaIpConfiguration", "GetStaticFieldID: PAC : %d!\n", __LINE__);

            jproxySettings = env->GetStaticObjectField(psClass, f);
            CheckErrorAndLog(env,"ToJavaIpConfiguration", "GetStaticObjectField: PAC : %d!\n", __LINE__);
            break;
        }
        default: {
            jfieldID f = env->GetStaticFieldID(psClass, "UNASSIGNED", "Landroid/net/IpConfiguration$ProxySettings;");
            CheckErrorAndLog(env,"ToJavaIpConfiguration", "GetStaticFieldID: UNASSIGNED : %d!\n", __LINE__);

            jproxySettings = env->GetStaticObjectField(psClass, f);
            CheckErrorAndLog(env,"ToJavaIpConfiguration", "GetStaticObjectField: UNASSIGNED : %d!\n", __LINE__);
            break;
        }
    }

    AutoPtr<IStaticIpConfiguration> staticIpConfiguration;
    config->GetStaticIpConfiguration((IStaticIpConfiguration**)&staticIpConfiguration);
    jobject jstaticIpConfiguration = ToJavaStaticIpConfiguration(env, staticIpConfiguration);

    AutoPtr<IProxyInfo> httpProxy;
    config->GetHttpProxy((IProxyInfo**)&httpProxy);
    jobject jhttpProxy = Util::ToJavaProxyInfo(env, httpProxy);

    jclass icKlass = env->FindClass("android/net/IpConfiguration");
    Util::CheckErrorAndLog(env, "ToJavaIpConfiguration", "FindClass: IpConfiguration line: %d", __LINE__);

    jmethodID m = env->GetMethodID(icKlass, "<init>", "(Landroid/net/IpConfiguration$IpAssignment;"
        "Landroid/net/IpConfiguration$ProxySettings;Landroid/net/StaticIpConfiguration;Landroid/net/ProxyInfo)V");
    Util::CheckErrorAndLog(env, "ToJavaIpConfiguration", "GetMethodID: IpConfiguration line: %d", __LINE__);

    jobject jconfig = env->NewObject(icKlass, m, jipAssignment, jproxySettings, jstaticIpConfiguration, jhttpProxy);
    Util::CheckErrorAndLog(env, "ToJavaIpConfiguration", "NewObject: IpConfiguration line: %d", __LINE__);

    env->DeleteLocalRef(icKlass);
    env->DeleteLocalRef(ipaClass);
    env->DeleteLocalRef(psClass);
    env->DeleteLocalRef(jipAssignment);
    env->DeleteLocalRef(jproxySettings);
    env->DeleteLocalRef(jstaticIpConfiguration);
    env->DeleteLocalRef(jhttpProxy);

    return jconfig;
}

jobject Util::ToJavaStaticIpConfiguration(
    /* [in] */ JNIEnv* env,
    /* [in] */ IStaticIpConfiguration* config)
{
    if (env == NULL || config == NULL) {
        LOGGERE("ToJavaStaticIpConfiguration", "Invalid arguments!");
        return NULL;
    }

    jclass sicKlass = env->FindClass("android/net/StaticIpConfiguration");
    Util::CheckErrorAndLog(env, "ToJavaStaticIpConfiguration", "FindClass: StaticIpConfiguration line: %d", __LINE__);

    jmethodID m = env->GetMethodID(sicKlass, "<init>", "()V");
    Util::CheckErrorAndLog(env, "ToJavaStaticIpConfiguration", "GetMethodID: StaticIpConfiguration line: %d", __LINE__);

    jobject jconfig = env->NewObject(sicKlass, m);
    Util::CheckErrorAndLog(env, "ToJavaStaticIpConfiguration", "NewObject: StaticIpConfiguration line: %d", __LINE__);

    AutoPtr<ILinkAddress> ipAddress;
    config->GetIpAddress((ILinkAddress**)&ipAddress);

    jobject jipAddress = ToJavaLinkAddress(env, ipAddress);
    jfieldID f = env->GetFieldID(sicKlass, "ipAddress", "Landroid/net/LinkAddress;");
    Util::CheckErrorAndLog(env, "ToJavaStaticIpConfiguration", "Fail GetFieldID: ipAddress : %d!\n", __LINE__);

    env->SetObjectField(jconfig, f, jipAddress);
    Util::CheckErrorAndLog(env, "ToJavaStaticIpConfiguration", "Fail SetObjectField: ipAddress : %d!\n", __LINE__);

    AutoPtr<IInetAddress> gateway;
    config->GetGateway((IInetAddress**)&gateway);

    jobject jgateway = ToJavaInetAddress(env, gateway);
    f = env->GetFieldID(sicKlass, "gateway", "Ljava/net/InetAddress;");
    Util::CheckErrorAndLog(env, "ToJavaStaticIpConfiguration", "Fail GetFieldID: gateway : %d!\n", __LINE__);

    env->SetObjectField(jconfig, f, jgateway);
    Util::CheckErrorAndLog(env, "ToJavaStaticIpConfiguration", "Fail SetObjectField: gateway : %d!\n", __LINE__);

    AutoPtr<IArrayList> dnsServers;
    config->GetDnsServers((IArrayList**)&dnsServers);
    Int32 size = 0;
    if (dnsServers)
        dnsServers->GetSize(&size);
    if (size > 0) {
        jfieldID f = env->GetFieldID(sicKlass, "dnsServers", "Ljava/util/ArrayList;");
        Util::CheckErrorAndLog(env, "ToJavaStaticIpConfiguration", "GetFieldID: dnsServers line: %d", __LINE__);

        jobject jdnsServers = env->GetObjectField(jconfig, f);
        Util::CheckErrorAndLog(env, "ToJavaStaticIpConfiguration", "GetObjectField: dnsServers line: %d", __LINE__);

        jclass listKlass = env->FindClass("java/util/ArrayList");
        CheckErrorAndLog(env, "ToJavaStaticIpConfiguration", "FindClass: ArrayList : %d!", __LINE__);

        jmethodID mAdd = env->GetMethodID(listKlass, "add", "(Ljava/lang/Object;)Z");
        CheckErrorAndLog(env, "ToJavaStaticIpConfiguration", "GetMethodID: add : %d!", __LINE__);

        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> item;
            dnsServers->Get(i, (IInterface**)&item);
            jobject jitem = ToJavaInetAddress(env, IInetAddress::Probe(item));
            env->CallBooleanMethod(jdnsServers, mAdd, jitem);
            Util::CheckErrorAndLog(env, "ToJavaStaticIpConfiguration", "CallBooleanMethod: add : %d!\n", __LINE__);
            env->DeleteLocalRef(jitem);
        }
        env->DeleteLocalRef(jdnsServers);
        env->DeleteLocalRef(listKlass);
    }

    String domains;
    config->GetDomains(&domains);
    SetJavaStringField(env, sicKlass, jconfig, domains, "domains", "ToJavaStaticIpConfiguration");

    env->DeleteLocalRef(sicKlass);
    env->DeleteLocalRef(jipAddress);
    env->DeleteLocalRef(jgateway);

    return jconfig;
}

jobject Util::ToJavaWifiDisplayStatus(
    /* [in] */ JNIEnv* env,
    /* [in] */ IWifiDisplayStatus* status)
{
    if (env == NULL || status == NULL) {
        LOGGERE("ToJavaWifiDisplayStatus", "Invalid arguments!");
        return NULL;
    }

    Int32 featureState;
    status->GetFeatureState(&featureState);

    Int32 scanState;
    status->GetScanState(&scanState);

    Int32 activeDisplayState;
    status->GetActiveDisplayState(&activeDisplayState);

    AutoPtr<IWifiDisplay> activeDisplay;
    status->GetActiveDisplay((IWifiDisplay**)&activeDisplay);
    jobject jactiveDisplay = NULL;
    if (activeDisplay != NULL) {
        jactiveDisplay = ToJavaWifiDisplay(env, activeDisplay);
    }

    jclass wifiDisplayKlass = env->FindClass("android/hardware/display/WifiDisplay");
    Util::CheckErrorAndLog(env, "ToJavaWifiDisplayStatus", "FindClass: WifiDisplay %d", __LINE__);

    AutoPtr<ArrayOf<IWifiDisplay*> > displays;
    status->GetDisplays((ArrayOf<IWifiDisplay*>**)&displays);

    jobjectArray jdisplays = NULL;
    if (displays != NULL){
        int size = displays->GetLength();
        jdisplays = env->NewObjectArray((jsize)size, wifiDisplayKlass, 0);
        Util::CheckErrorAndLog(env, "ToJavaWifiDisplayStatus", "new object array: WifiDisplayStatus %d", __LINE__);

        for(Int32 i = 0; i < size; i++ ) {
            jobject jwifiDisplay = ToJavaWifiDisplay(env, (*displays)[i]);
            if (jwifiDisplay != NULL) {
                env->SetObjectArrayElement(jdisplays, i, jwifiDisplay);
                Util::CheckErrorAndLog(env, "ToJavaWifiDisplayStatus", "SetObjectArrayElement fail %d", __LINE__);
                env->DeleteLocalRef(jwifiDisplay);
            }
        }
    }

    AutoPtr<IWifiDisplaySessionInfo> sessionInfo;
    status->GetSessionInfo((IWifiDisplaySessionInfo**)&sessionInfo);

    jobject jsessionInfo = NULL;
    if (sessionInfo != NULL) {
        jsessionInfo = ToJavaWifiDisplaySessionInfo(env, sessionInfo);
    }

    env->DeleteLocalRef(wifiDisplayKlass);

    jclass wifiDisplayStatusKlass = env->FindClass("android/hardware/display/WifiDisplayStatus");
    Util::CheckErrorAndLog(env, "ToJavaWifiDisplayStatus", "FindClass: WifiDisplayStatus %d", __LINE__);

    jmethodID m = env->GetMethodID(wifiDisplayStatusKlass, "<init>", "(IIILandroid/hardware/display/WifiDisplay;"
        "[Landroid/hardware/display/WifiDisplay;Landroid/hardware/display/WifiDisplaySessionInfo;)V");
    Util::CheckErrorAndLog(env, "ToJavaWifiDisplayStatus", "GetMethodID: WifiDisplayStatus() line: %d", __LINE__);

    jobject jstatus = env->NewObject(wifiDisplayStatusKlass, m, featureState, scanState, activeDisplayState,
        jactiveDisplay, jdisplays, jsessionInfo);

    env->DeleteLocalRef(wifiDisplayStatusKlass);
    env->DeleteLocalRef(jactiveDisplay);
    env->DeleteLocalRef(jdisplays);
    env->DeleteLocalRef(jsessionInfo);

    return jstatus;
}

jobject Util::ToJavaWifiDisplaySessionInfo(
    /* [in] */ JNIEnv* env,
    /* [in] */ IWifiDisplaySessionInfo* info)
{
    if (env == NULL || info == NULL) {
        LOGGERE("ToJavaWifiDisplaySessionInfo", "Invalid arguments!");
        return NULL;
    }

    Boolean isClient;
    info->IsClient(&isClient);
    Int32 id;
    info->GetSessionId(&id);
    String gId;
    info->GetGroupId(&gId);
    jstring jgId = ToJavaString(env, gId);
    String pp;
    info->GetPassphrase(&pp);
    jstring jpp = ToJavaString(env, pp);
    String ip;
    info->GetIP(&ip);
    jstring jip = ToJavaString(env, ip);

    jclass infoKlass = env->FindClass("android/hardware/display/WifiDisplaySessionInfo");
    Util::CheckErrorAndLog(env, "ToJavaWifiDisplaySessionInfo", "FindClass: WifiDisplaySessionInfo %d", __LINE__);

    jmethodID m = env->GetMethodID(infoKlass, "<init>", "(ZILjava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
    Util::CheckErrorAndLog(env, "ToJavaWifiDisplaySessionInfo", "GetMethodID: WifiDisplaySessionInfo() line: %d", __LINE__);

    jobject jinfo = env->NewObject(infoKlass, m, isClient, id, jgId, jpp, jip);

    env->DeleteLocalRef(infoKlass);
    env->DeleteLocalRef(jgId);
    env->DeleteLocalRef(jpp);
    env->DeleteLocalRef(jip);

    return jinfo;
}

jobject Util::ToJavaWifiDisplay(
    /* [in] */ JNIEnv* env,
    /* [in] */ IWifiDisplay* display)
{
    if (env == NULL || display == NULL) {
        LOGGERE("ToJavaWifiDisplay", "Invalid arguments!");
        return NULL;
    }

    String deviceAddress;
    display->GetDeviceAddress(&deviceAddress);
    jstring jdeviceAddress = ToJavaString(env, deviceAddress);

    String deviceName;
    display->GetDeviceName(&deviceName);
    jstring jdeviceName = ToJavaString(env, deviceName);

    String deviceAlias;
    display->GetDeviceAlias(&deviceAlias);
    jstring jdeviceAlias = ToJavaString(env, deviceAlias);

    Boolean available, canConnect, remembered;
    display->IsAvailable(&available);
    display->CanConnect(&canConnect);
    display->IsRemembered(&remembered);

    jclass wifiDisplayKlass = env->FindClass("android/hardware/display/WifiDisplay");
    Util::CheckErrorAndLog(env, "ToJavaWifiDisplay", "FindClass: WifiDisplay %d", __LINE__);

    jmethodID m = env->GetMethodID(wifiDisplayKlass, "<init>", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;ZZZ)V");
    Util::CheckErrorAndLog(env, "ToJavaWifiDisplay", "GetMethodID: WifiDisplay() line: %d", __LINE__);

    jobject jdisplay = env->NewObject(wifiDisplayKlass, m, jdeviceAddress, jdeviceName, jdeviceAlias,
        available, canConnect, remembered);
    Util::CheckErrorAndLog(env, "ToJavaWifiDisplay", "NewObject: WifiDisplay() line: %d", __LINE__);

    env->DeleteLocalRef(wifiDisplayKlass);
    env->DeleteLocalRef(jdeviceAddress);
    env->DeleteLocalRef(jdeviceName);
    env->DeleteLocalRef(jdeviceAlias);
    return jdisplay;
}

jobject Util::ToJavaBitSet(
    /* [in] */ JNIEnv* env,
    /* [in] */ IBitSet* bitSet)
{
    if (env == NULL || bitSet == NULL) {
        LOGGERE("ToJavaBitSet", "Invalid arguments!");
        return NULL;
    }

    jobject jbitSet = NULL;

    jclass bsKlass = env->FindClass("java/util/BitSet");
    Util::CheckErrorAndLog(env, "ToJavaBitSet", "FindClass: BitSet line: %d", __LINE__);

    jmethodID m = env->GetMethodID(bsKlass, "<init>", "([J)V");
    Util::CheckErrorAndLog(env, "ToJavaBitSet", "GetMethodID: BitSet line: %d", __LINE__);

    AutoPtr<ArrayOf<Int64> > int64Arr;
    bitSet->ToArray((ArrayOf<Int64>**)&int64Arr);
    if (int64Arr != NULL) {
        Int32 count = int64Arr->GetLength();

        jlongArray jlongArr = env->NewLongArray(count);
        Util::CheckErrorAndLog(env, "ToJavaBitSet", "NewLongArray fail line: %d", __LINE__);

        jlong* longArrInd = (jlong*)int64Arr->GetPayload();

        env->SetLongArrayRegion(jlongArr, 0, count, longArrInd);
        Util::CheckErrorAndLog(env, "ToJavaBitSet", "SetLongArrayRegion fail line: %d", __LINE__);

        jbitSet = env->NewObject(bsKlass, m, jlongArr);
        Util::CheckErrorAndLog(env, "ToJavaBitSet", "NewObject BitSet fail line: %d", __LINE__);

        env->DeleteLocalRef(jlongArr);
    }

    env->DeleteLocalRef(bsKlass);
    return jbitSet;
}

jobject Util::ToJavaContainerEncryptionParams(
    /* [in] */ JNIEnv* env,
    /* [in] */ IContainerEncryptionParams* params)
{
    if (env == NULL || params == NULL) {
        LOGGERE("ToJavaContainerEncryptionParams", "Invalid arguments!");
        return NULL;
    }

    jobject jparams = NULL;

    jclass cepKlass = env->FindClass("android/content/pm/ContainerEncryptionParams");
    Util::CheckErrorAndLog(env, "ToJavaContainerEncryptionParams", "FindClass: ContainerEncryptionParams line: %d", __LINE__);

    jmethodID m = env->GetMethodID(cepKlass, "<init>", "(Ljava/lang/String;Ljava/security/spec/AlgorithmParameterSpec;Ljavax/crypto/SecretKey;Ljava/lang/String;Ljava/security/spec/AlgorithmParameterSpec;Ljavax/crypto/SecretKey;[BJJJ)V");
    Util::CheckErrorAndLog(env, "ToJavaContainerEncryptionParams", "GetMethodID: ContainerEncryptionParams line: %d", __LINE__);

    String encryptionAlgorithm;
    params->GetEncryptionAlgorithm(&encryptionAlgorithm);
    jstring jencryptionAlgorithm = Util::ToJavaString(env, encryptionAlgorithm);

    AutoPtr<Elastos::Security::Spec::IAlgorithmParameterSpec> encryptionSpec;
    params->GetEncryptionSpec((Elastos::Security::Spec::IAlgorithmParameterSpec**)&encryptionSpec);
    jobject jencryptionSpec = NULL;
    if (encryptionSpec != NULL) {
        AutoPtr<IIvParameterSpec> spec = IIvParameterSpec::Probe(encryptionSpec);
        if (spec != NULL) {
            jencryptionSpec = Util::ToJavaIvParameterSpec(env, spec);
        }
        else {
            LOGGERE("ToJavaContainerEncryptionParams", "What's this encryptionSpec?");
            assert(0);
        }
    }

    AutoPtr<Elastosx::Crypto::ISecretKey> encryptionKey;
    params->GetEncryptionKey((Elastosx::Crypto::ISecretKey**)&encryptionKey);
    jobject jencryptionKey = NULL;
    if (encryptionKey != NULL) {
        LOGGERE("ToJavaContainerEncryptionParams", "encryptionKey is NOT NULL!");
    }

    String macAlgorithm;
    params->GetMacAlgorithm(&macAlgorithm);
    jstring jmacAlgorithm = Util::ToJavaString(env, macAlgorithm);

    AutoPtr<Elastos::Security::Spec::IAlgorithmParameterSpec> macSpec;
    params->GetMacSpec((Elastos::Security::Spec::IAlgorithmParameterSpec**)&macSpec);
    jobject jmacSpec = NULL;
    if (macSpec != NULL) {
        LOGGERE("ToJavaContainerEncryptionParams", "macSpec is NOT NULL!");
    }

    AutoPtr<Elastosx::Crypto::ISecretKey> macKey;
    params->GetMacKey((Elastosx::Crypto::ISecretKey**)&macKey);
    jobject jmacKey = NULL;
    if (macKey != NULL) {
        LOGGERE("ToJavaContainerEncryptionParams", "encryptionKey is NOT NULL!");
    }

    AutoPtr<ArrayOf<Byte> > macTag;
    params->GetMacTag((ArrayOf<Byte>**)&macTag);
    jbyteArray jmacTag = NULL;
    if (macTag != NULL) {
        jmacTag = Util::ToJavaByteArray(env, macTag);
    }

    Int64 authenticatedDataStart = 0;
    params->GetAuthenticatedDataStart(&authenticatedDataStart);

    Int64 encryptedDataStart = 0;
    params->GetEncryptedDataStart(&encryptedDataStart);

    Int64 dataEnd = 0;
    params->GetDataEnd(&dataEnd);

    jparams = env->NewObject(cepKlass, m, jencryptionAlgorithm, jencryptionSpec, jencryptionKey, jmacAlgorithm,
            jmacSpec, jmacKey, jmacTag, (jlong)authenticatedDataStart, (jlong)encryptedDataStart, (jlong)dataEnd);
    Util::CheckErrorAndLog(env, "ToJavaContainerEncryptionParams", "NewObject ContainerEncryptionParams line: %d", __LINE__);

    env->DeleteLocalRef(cepKlass);
    env->DeleteLocalRef(jencryptionAlgorithm);
    env->DeleteLocalRef(jencryptionSpec);
    env->DeleteLocalRef(jencryptionKey);
    env->DeleteLocalRef(jmacAlgorithm);
    env->DeleteLocalRef(jmacSpec);
    env->DeleteLocalRef(jmacKey);
    env->DeleteLocalRef(jmacTag);
    return jparams;
}

jobject Util::ToJavaIvParameterSpec(
    /* [in] */ JNIEnv* env,
    /* [in] */ IIvParameterSpec* spec)
{
    if (env == NULL || spec == NULL) {
        LOGGERE("ToJavaIvParameterSpec", "Invalid arguments!");
        return NULL;
    }

    jclass ipsKlass = env->FindClass("javax/crypto/spec/IvParameterSpec");
    Util::CheckErrorAndLog(env, "ToJavaIvParameterSpec", "FindClass: IvParameterSpec line: %d", __LINE__);

    jmethodID m = env->GetMethodID(ipsKlass, "<init>", "([B)V");
    Util::CheckErrorAndLog(env, "ToJavaIvParameterSpec", "GetMethodID: IvParameterSpec line: %d", __LINE__);

    AutoPtr<ArrayOf<Byte> > iv;
    spec->GetIV((ArrayOf<Byte>**)&iv);
    jbyteArray jiv = NULL;
    if (iv != NULL) {
        jiv = Util::ToJavaByteArray(env, iv);
    }

    jobject jspec = env->NewObject(ipsKlass, m, jiv);
    Util::CheckErrorAndLog(env, "ToJavaIvParameterSpec", "NewObject IvParameterSpec line: %d", __LINE__);

    env->DeleteLocalRef(ipsKlass);
    env->DeleteLocalRef(jiv);
    return jspec;
}

jobject Util::ToJavaAudioRoutesInfo(
    /* [in] */ JNIEnv* env,
    /* [in] */ IAudioRoutesInfo* event)
{
    if (env == NULL || event == NULL) {
        ALOGE("Util::ToJavaAudioRoutesInfo() invalid param!");
        return NULL;
    }

    jclass klass = env->FindClass("android/media/AudioRoutesInfo");
    CheckErrorAndLog(env, "ToJavaAudioRoutesInfo Fail FindClass: AudioRoutesInfo : %d!\n", __LINE__);

    jmethodID m = env->GetMethodID(klass, "<init>", "()V");
    CheckErrorAndLog(env, "ToJavaAudioRoutesInfo Fail GetMethodID: AudioRoutesInfo() : %d!\n", __LINE__);

    jobject jaudioRoutesInfo = env->NewObject(klass, m);
    CheckErrorAndLog(env, "ToJavaAudioRoutesInfo Fail NewObject: AudioRoutesInfo : %d!\n", __LINE__);

    // Set mBluetoothName
    AutoPtr<ICharSequence> csBluetoothName;
    event->GetBluetoothName((ICharSequence**)&csBluetoothName);
    jstring jbluetoothName = NULL;
    if (csBluetoothName != NULL) {
        String blueToothName;
        csBluetoothName->ToString(&blueToothName);
        jbluetoothName = ToJavaString(env, blueToothName);
    }

    jfieldID f = env->GetFieldID(klass, "mBluetoothName", "Ljava/lang/CharSequence;");
    CheckErrorAndLog(env, "ToJavaAudioRoutesInfo, Fail GetFieldID: CharSequence:mBluetoothName %d", __LINE__);

    env->SetObjectField(jaudioRoutesInfo, f, jbluetoothName);
    CheckErrorAndLog(env, "ToJavaAudioRoutesInfo, Fail SetObjectField: mBluetoothName %d", __LINE__);
    env->DeleteLocalRef(jbluetoothName);

    // Set mMainType
    Int32 mainType;
    event->GetMainType(&mainType);
    f = env->GetFieldID(klass, "mMainType", "I");
    CheckErrorAndLog(env, "ToJavaAudioRoutesInfo, Fail GetFieldID: int:mMainType %d", __LINE__);

    env->SetIntField(jaudioRoutesInfo, f, (jint)mainType);
    CheckErrorAndLog(env, "ToJavaAudioRoutesInfo, Fail SetObjectField: mMainType %d", __LINE__);

    env->DeleteLocalRef(klass);
    return jaudioRoutesInfo;
}

Boolean Util::GetElClipData(
    /* [in] */ JNIEnv* env,
    /* [in] */ jobject jclip,
    /* [out] */ IClipData** clip)
{
    if (env == NULL || jclip == NULL || clip == NULL) {
        LOGGERE("GetElClipData", "Invalid argumenet!");
        *clip = NULL;
        return FALSE;
    }

    jclass clipKlass = env->FindClass("android/content/ClipData");
    CheckErrorAndLog(env, "GetElClipData", "FindClass: ClipData : %d!\n", __LINE__);

    if (NOERROR != CClipData::New(clip)) {
        LOGGERE("GetElClipData", "create CCorrectionInfo fail!");
        *clip = NULL;
        return FALSE;
    }

    AutoPtr<IParcel> source;
    CParcel::New((IParcel**)&source);

    jfieldID f = env->GetFieldID(clipKlass, "mClipDescription", "Landroid/content/ClipDescription;");
    CheckErrorAndLog(env, "GetElClipData", "GetFieldID mClipDescription : %d!\n", __LINE__);

    jobject jclipDescription = env->GetObjectField(jclip, f);
    CheckErrorAndLog(env, "GetElClipData", "GetObjectField mClipDescription : %d!\n", __LINE__);

    AutoPtr<IClipDescription> clipDescription;
    if (jclipDescription != NULL) {
        if (!Util::GetElClipDescription(env, jclipDescription, (IClipDescription**)&clipDescription)) {
            LOGGERE("GetElClipData", "ToElClipDescription fail!");
        }
    }

    source->WriteInterfacePtr(clipDescription);

    f = env->GetFieldID(clipKlass, "mIcon", "Landroid/graphics/Bitmap;");
    CheckErrorAndLog(env, "GetElClipData", "GetFieldID mIcon : %d!\n", __LINE__);

    jobject jicon= env->GetObjectField(jclip, f);
    CheckErrorAndLog(env, "GetElClipData", "GetObjectField mIcon : %d!\n", __LINE__);

    if (jicon != NULL) {
        AutoPtr<IBitmap> icon;
        if (Util::GetElBitmap(env, jicon, (IBitmap**)&icon)) {
            source->WriteInt32(1);
            source->WriteInterfacePtr(icon);
            env->DeleteLocalRef(jicon);
        }
        else {
            LOGGERE("GetElClipData", " GetElBitmap fail!");
            source->WriteInt32(0);
        }

    }
    else {
        source->WriteInt32(0);
    }

    f = env->GetFieldID(clipKlass, "mItems", "Ljava/util/ArrayList;");
    Util::CheckErrorAndLog(env, "GetElClipData", "GetFieldID: mItems %d", __LINE__);

    jobject jitems = env->GetObjectField(jclip, f);
    Util::CheckErrorAndLog(env, "GetElClipData", "GetObjectField: mItems %d", __LINE__);

    if (jitems != NULL) {
        jclass alistKlass = env->FindClass("java/util/ArrayList");
        CheckErrorAndLog(env, "GetElClipData", "FindClass: ArrayList : %d!\n", __LINE__);

        jmethodID m = env->GetMethodID(alistKlass, "size", "()I");
        CheckErrorAndLog(env, "GetElClipData", "GetMethodID: get : %d!\n", __LINE__);

        jint jsize = env->CallIntMethod(jitems, m);
        CheckErrorAndLog(env, "GetElClipData", "CallIntMethod: size : %d!\n", __LINE__);

        source->WriteInt32((Int32)jsize);

        jmethodID mGet = env->GetMethodID(alistKlass, "get", "(I)Ljava/lang/Object;");
        CheckErrorAndLog(env, "GetElClipData", "GetMethodID: get : %d!\n", __LINE__);
        if (jsize > 0) {
            for (jint i = 0; i < jsize; i++) {
                jobject jitem = env->CallObjectMethod(jitems, mGet, i);
                CheckErrorAndLog(env, "GetElClipData", "CallObjectMethod: get : %d!\n", __LINE__);

                AutoPtr<IClipDataItem> item;
                if (!Util::GetElClipDataItem(env, jitem, (IClipDataItem**)&item)) {
                    LOGGERE("GetElClipData", "ToElClipDataItem fail!");
                    continue;
                }

                AutoPtr<ICharSequence> text;
                item->GetText((ICharSequence**)&text);
                TextUtils::WriteToParcel(text, source);

                String htmlText;
                item->GetHtmlText(&htmlText);
                source->WriteString(htmlText);

                AutoPtr<IIntent> intent;
                item->GetIntent((IIntent**)&intent);
                if (intent != NULL) {
                    source->WriteInt32(1);
                    source->WriteInterfacePtr(intent);
                }
                else {
                    source->WriteInt32(0);
                }

                AutoPtr<IUri> uri;
                item->GetUri((IUri**)&uri);
                if (uri != NULL) {
                    source->WriteInt32(1);
                    source->WriteInterfacePtr(uri);
                }
                else {
                    source->WriteInt32(0);
                }

                env->DeleteLocalRef(jitem);
            }
        }
        env->DeleteLocalRef(alistKlass);
        env->DeleteLocalRef(jitems);
    }
    else {
        source->WriteInt32(0);
    }

    source->SetDataPosition(0);
    AutoPtr<IParcelable> clipParcel = IParcelable::Probe(*clip);
    if (clipParcel != NULL) {
        clipParcel->ReadFromParcel(source);
    }else {
        LOGGERE("GetElClipData", "Get IParcelable  from IClipData failed!");
        return FALSE;
    }

    env->DeleteLocalRef(clipKlass);
    return TRUE;
}

Boolean Util::GetElClipDataItem(
    /* [in] */ JNIEnv* env,
    /* [in] */ jobject jitem,
    /* [out] */ IClipDataItem** item)
{
    if (env == NULL || jitem == NULL || item == NULL) {
        LOGGERE("GetElClipDataItem", "Invalid argumenet!");
        *item = NULL;
        return FALSE;
    }

    jclass cdiKlass = env->FindClass("android/content/ClipData$Item");
    CheckErrorAndLog(env, "GetElClipDataItem", "FindClass: ClipData$Item : %d!\n", __LINE__);


    jfieldID f = env->GetFieldID(cdiKlass, "mText", "Ljava/lang/CharSequence;");
    CheckErrorAndLog(env, "GetElClipDataItem", "GetFieldID mText : %d!\n", __LINE__);

    jobject jtext = env->GetObjectField(jitem, f);
    CheckErrorAndLog(env, "GetElClipDataItem", "GetObjectField mText : %d!\n", __LINE__);

    AutoPtr<ICharSequence> text;
    if (jtext != NULL) {
        jclass csClass = env->FindClass("java/lang/CharSequence");
        CheckErrorAndLog(env, "GetElClipDataItem", "FindClass CharSequence : %d!\n", __LINE__);

        jmethodID m = env->GetMethodID(csClass, "toString", "()Ljava/lang/String;");
        CheckErrorAndLog(env, "GetElClipDataItem", "GetMethodID toString : %d!\n", __LINE__);

        jstring jstext = (jstring)env->CallObjectMethod(jtext, m);
        CheckErrorAndLog(env, "GetElClipDataItem", "CallObjectMethod toString : %d!\n", __LINE__);

        String stext = Util::GetElString(env, jstext);
        CString::New(stext, (ICharSequence**)&text);

        env->DeleteLocalRef(csClass);
        env->DeleteLocalRef(jstext);
        env->DeleteLocalRef(jtext);
    }

    String htmlText = Util::GetJavaStringField(env, cdiKlass, jitem, "mHtmlText", "ToElClipDataItem");

    f = env->GetFieldID(cdiKlass, "mIntent", "Landroid/content/Intent;");
    CheckErrorAndLog(env, "GetElClipDataItem", "GetFieldID mIntent : %d!\n", __LINE__);

    jobject jintent = env->GetObjectField(jitem, f);
    CheckErrorAndLog(env, "GetElClipDataItem", "GetObjectField mIntent : %d!\n", __LINE__);

    AutoPtr<IIntent> intent;
    if (jintent != NULL) {
        if (!Util::GetElIntent(env, jintent, (IIntent**)&intent)) {
            LOGGERE("GetElClipDataItem", "GetElIntent fail");
        }
    }

    f = env->GetFieldID(cdiKlass, "mUri", "Landroid/net/Uri;");
    CheckErrorAndLog(env, "GetElClipDataItem", "GetFieldID mUri : %d!\n", __LINE__);

    jobject juri = env->GetObjectField(jitem, f);
    CheckErrorAndLog(env, "GetElClipDataItem", "GetObjectField mUri : %d!\n", __LINE__);

    AutoPtr<IUri> uri;
    if (juri != NULL) {
        if (!Util::GetElUri(env, juri, (IUri**)&uri)) {
            LOGGERE("GetElClipDataItem", "GetElUri fail");
        }
    }

    if (NOERROR != CClipDataItem::New(text, htmlText, intent, uri, item)) {
        LOGGERE("GetElClipDataItem", "create CClipDataItem fail!");
        *item = NULL;
        return FALSE;
    }

    env->DeleteLocalRef(cdiKlass);
    return TRUE;
}

Boolean Util::GetElClipDescription(
    /* [in] */ JNIEnv* env,
    /* [in] */ jobject jclipDesc,
    /* [out] */ IClipDescription** clipDesc)
{
    if (env == NULL || jclipDesc == NULL || clipDesc == NULL) {
        LOGGERE("GetElClipDescription", "Invalid argumenet!");
        *clipDesc = NULL;
        return FALSE;
    }

    jclass cdKlass = env->FindClass("android/content/ClipDescription");
    CheckErrorAndLog(env, "GetElClipDescription", "FindClass: ClipDescription : %d!\n", __LINE__);

    jfieldID f = env->GetFieldID(cdKlass, "mLabel", "Ljava/lang/CharSequence;");
    CheckErrorAndLog(env, "GetElClipDescription", "GetFieldID mLabel : %d!\n", __LINE__);

    jobject jlabel = env->GetObjectField(jclipDesc, f);
    CheckErrorAndLog(env, "GetElClipDescription", "GetObjectField mLabel : %d!\n", __LINE__);

    AutoPtr<ICharSequence> label;
    if (jlabel != NULL) {
        jclass csClass = env->FindClass("java/lang/CharSequence");
        CheckErrorAndLog(env, "GetElClipDescription", "FindClass CharSequence : %d!\n", __LINE__);

        jmethodID m = env->GetMethodID(csClass, "toString", "()Ljava/lang/String;");
        CheckErrorAndLog(env, "GetElClipDescription", "GetMethodID toString : %d!\n", __LINE__);

        jstring jslabel = (jstring)env->CallObjectMethod(jlabel, m);
        CheckErrorAndLog(env, "GetElClipDescription", "CallObjectMethod toString : %d!\n", __LINE__);

        String slabel = Util::GetElString(env, jslabel);
        CString::New(slabel, (ICharSequence**)&label);

        env->DeleteLocalRef(csClass);
        env->DeleteLocalRef(jslabel);
        env->DeleteLocalRef(jlabel);
    }

    f = env->GetFieldID(cdKlass, "mMimeTypes", "[Ljava/lang/String;");
    CheckErrorAndLog(env, "GetElClipDescription", "GetFieldID mMimeTypes : %d!\n", __LINE__);

    jobjectArray jmimeTypes = (jobjectArray)env->GetObjectField(jclipDesc, f);
    CheckErrorAndLog(env, "GetElClipDescription", "GetObjectField mMimeTypes : %d!\n", __LINE__);

    AutoPtr<ArrayOf<String> > mimeTypes;
    if (jmimeTypes != NULL) {
        if (!Util::GetElStringArray(env, jmimeTypes, (ArrayOf<String>**)&mimeTypes)) {
            LOGGERE("GetElClipDescription", "GetElStringArray fail!");
        }
    }

    if (NOERROR != CClipDescription::New(label, mimeTypes, clipDesc)) {
        LOGGERE("GetElClipDescription", "create CClipDescription fail!");
        *clipDesc = NULL;
        return FALSE;
    }

    env->DeleteLocalRef(cdKlass);
    return TRUE;
}

jobject Util::ElByteArrayToJavaObject(
    /* [in] */ JNIEnv* env,
    /* [in] */ ISerializable* serializable)
{
    if (env == NULL || serializable == NULL) {
        LOGGERE("ElByteArrayToJavaObject", "Invalid argumenet!");
        return NULL;
    }

    CISerializableNative* snative = (CISerializableNative*)serializable;

    jbyteArray jbArray = Util::ToJavaByteArray(env, snative->mObject);
    if (jbArray == NULL) {
        LOGGERE("ElByteArrayToJavaObject", "ToJavaByteArray fail!");
        return NULL;
    }

    jclass baisKlass = env->FindClass("java/io/ByteArrayInputStream");
    CheckErrorAndLog(env, "ElByteArrayToJavaObject", "FindClass ByteArrayInputStream : %d!\n", __LINE__);

    jmethodID m = env->GetMethodID(baisKlass, "<init>", "([B)V");
    CheckErrorAndLog(env, "ElByteArrayToJavaObject", "GetMethodID ByteArrayInputStream : %d!\n", __LINE__);

    jobject jbais = env->NewObject(baisKlass, m, jbArray);
    CheckErrorAndLog(env, "ElByteArrayToJavaObject", "NewObject ByteArrayInputStream : %d!\n", __LINE__);

    jclass oisKlass = env->FindClass("java/io/ObjectInputStream");
    CheckErrorAndLog(env, "ElByteArrayToJavaObject", "FindClass ObjectInputStream : %d!\n", __LINE__);

    m = env->GetMethodID(oisKlass, "<init>", "(Ljava/io/InputStream;)V");
    CheckErrorAndLog(env, "ElByteArrayToJavaObject", "GetMethodID ObjectInputStream : %d!\n", __LINE__);

    jobject jois = env->NewObject(oisKlass, m, jbais);
    CheckErrorAndLog(env, "ElByteArrayToJavaObject", "GetMethodID ObjectInputStream : %d!\n", __LINE__);

    if (!snative->mPkgPath.IsNullOrEmpty())  {
        jstring jpath = Util::ToJavaString(env, snative->mPkgPath);
        jobject jloader;
        if (snative->mIsDexClassLoader) {
            // jclass dexKlass = env->FindClass("dalvik/system/DexClassLoader");
            // CheckErrorAndLog(env, "FindClass DexClassLoader : %d!\n", __LINE__);

            // jclass klass = env->FindClass("java/lang/Class");
            // CheckErrorAndLog(env, "FindClass Class : %d!\n", __LINE__);

            // m = env->GetMethodID(klass, "getClassLoader", "()Ljava/lang/ClassLoader;");
            // CheckErrorAndLog(env, "GetMethodID getClassLoader : %d!\n", __LINE__);

            // jobject jparent = env->CallObjectMethod(dexKlass, m);
            // CheckErrorAndLog(env, "CallObjectMethod getClassLoader : %d!\n", __LINE__);

            // m = env->GetMethodID(dexKlass, "<init>", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/ClassLoader;)V");
            // CheckErrorAndLog(env, "GetMethodID DexClassLoader : %d!\n", __LINE__);

            // jstring jdir = Util::ToJavaString(env, snative->mOptimizedDirectory);

            // jloader = env->NewObject(dexKlass, m, jpath, jdir, NULL, jparent);
            // CheckErrorAndLog(env, "NewObject DexClassLoader : %d!\n", __LINE__);

            // env->DeleteLocalRef(dexKlass);
            // env->DeleteLocalRef(jdir);

            jloader = (jobject)StringUtils::ParseInt32(snative->mOptimizedDirectory);
            snative->mOptimizedDirectory = NULL;
        }
        else {
            jclass alsKlass = env->FindClass("android/app/ApplicationLoaders");
            CheckErrorAndLog(env, "FindClass ApplicationLoaders : %d!\n", __LINE__);

            m = env->GetStaticMethodID(alsKlass, "getDefault", "()Landroid/app/ApplicationLoaders;");
            CheckErrorAndLog(env, "GetStaticMethodID getDefault : %d!\n", __LINE__);

            jobject jloaders = env->CallStaticObjectMethod(alsKlass, m);
            CheckErrorAndLog(env, "GetMethodID getDefault : %d!\n", __LINE__);

            m = env->GetMethodID(alsKlass, "getClassLoader", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/ClassLoader;)Ljava/lang/ClassLoader;");
            CheckErrorAndLog(env, "GetMethodID getClassLoader : %d!\n", __LINE__);

            jloader = env->CallObjectMethod(jloaders, m, jpath, NULL, NULL);
            CheckErrorAndLog(env, "CallObjectMethod getClassLoader : %d!\n", __LINE__);

            env->DeleteLocalRef(alsKlass);
            env->DeleteLocalRef(jloaders);
        }

        if (jloader != NULL) {
            jfieldID f = env->GetFieldID(oisKlass, "callerClassLoader", "Ljava/lang/ClassLoader;");
            Util::CheckErrorAndLog(env, "GetFieldID: callerClassLoader %d", __LINE__);

            env->SetObjectField(jois, f, jloader);
            Util::CheckErrorAndLog(env, "SetObjectField: callerClassLoader %d", __LINE__);

            Util::SetJavaIntField(env, oisKlass, jois, 1, "nestedLevels", "ElByteArrayToJavaObject");

            if (snative->mIsDexClassLoader)
                env->DeleteGlobalRef(jloader);
            else
                env->DeleteLocalRef(jloader);
        }

        env->DeleteLocalRef(jpath);
    }

    m = env->GetMethodID(oisKlass, "readObject", "()Ljava/lang/Object;");
    CheckErrorAndLog(env, "ElByteArrayToJavaObject", "GetMethodID readObject : %d!\n", __LINE__);

    jobject jobj = env->CallObjectMethod(jois, m);
    CheckErrorAndLog(env, "ElByteArrayToJavaObject", "CallObjectMethod readObject : %d!\n", __LINE__);

    env->DeleteLocalRef(baisKlass);
    env->DeleteLocalRef(oisKlass);
    env->DeleteLocalRef(jbArray);
    env->DeleteLocalRef(jbais);
    env->DeleteLocalRef(jois);
    return jobj;
}

jobject Util::ToJavaParcelable(
    /* [in] */ JNIEnv* env,
    /* [in] */ IParcelable* parcelable)
{
    if (env == NULL || parcelable == NULL) {
        LOGGERE("ToJavaParcelable", "Invalid argumenet!");
        return NULL;
    }

    jclass parcelClass = env->FindClass("android/os/Parcel");
    Util::CheckErrorAndLog(env, "ToJavaParcelable", "FindClass: Parcel : %d", __LINE__);

    jmethodID m = env->GetStaticMethodID(parcelClass, "obtain", "()Landroid/os/Parcel;");
    Util::CheckErrorAndLog(env, "ToJavaParcelable", "GetStaticMethodID: obtain : %d", __LINE__);

    jobject jparcel = env->CallStaticObjectMethod(parcelClass, m);
    Util::CheckErrorAndLog(env, "ToJavaParcelable", "GetStaticMethodID: obtain : %d", __LINE__);

    CIParcelableNative* parcelNative = (CIParcelableNative*)parcelable;
    AutoPtr<ArrayOf<Byte> > parcelObj;
    parcelNative->GetObject((ArrayOf<Byte>**)&parcelObj);
    if (parcelObj != NULL) {
        Int64 nativePtr = Util::GetJavalongField(env, jparcel, "mNativePtr", "ToJavaParcelable");
        android::Parcel* parcel =  reinterpret_cast< android::Parcel*>(nativePtr);

        if (parcel == NULL) {
            LOGGERE("ToJavaParcelable", "ToJavaParcelable() IParcelable parcel is NULL!");
           return NULL;
        }

        Int32 length = parcelObj->GetLength() * sizeof(Byte);
        parcel->setDataSize(length);
        parcel->setDataPosition(0);

        void* raw = parcel->writeInplace(length);
        memcpy(raw, parcelObj->GetPayload(), length);
        parcel->setDataPosition(0);
    }
    else {
        LOGGERE("ToJavaParcelable", "ToJavaParcelable() IParcelable parcelObj is NULL!");
        env->DeleteLocalRef(parcelClass);
        env->DeleteLocalRef(jparcel);
        return NULL;
    }

    String pkgPath = parcelNative->GetPackagePath();

    jobject jloader = NULL;
    if (!pkgPath.IsNullOrEmpty())  {
        jclass alsKlass = env->FindClass("android/app/ApplicationLoaders");
        CheckErrorAndLog(env, "FindClass ApplicationLoaders : %d!\n", __LINE__);

        m = env->GetStaticMethodID(alsKlass, "getDefault", "()Landroid/app/ApplicationLoaders;");
        CheckErrorAndLog(env, "GetStaticMethodID getDefault : %d!\n", __LINE__);

        jobject jloaders = env->CallStaticObjectMethod(alsKlass, m);
        CheckErrorAndLog(env, "GetMethodID getDefault : %d!\n", __LINE__);

        m = env->GetMethodID(alsKlass, "getClassLoader", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/ClassLoader;)Ljava/lang/ClassLoader;");
        CheckErrorAndLog(env, "GetMethodID getClassLoader : %d!\n", __LINE__);

        jstring jpkgPath = Util::ToJavaString(env, pkgPath);

        jloader = env->CallObjectMethod(jloaders, m, jpkgPath, NULL, NULL);
        CheckErrorAndLog(env, "CallObjectMethod getClassLoader : %d!\n", __LINE__);

        env->DeleteLocalRef(alsKlass);
        env->DeleteLocalRef(jloaders);
        env->DeleteLocalRef(jpkgPath);
    }
    else {
        jclass clKlass = env->FindClass("java/lang/ClassLoader");
        CheckErrorAndLog(env, "FindClass ClassLoader : %d!\n", __LINE__);

        m = env->GetStaticMethodID(clKlass, "getSystemClassLoader", "()Ljava/lang/ClassLoader;");
        CheckErrorAndLog(env, "GetStaticMethodID getSystemClassLoader : %d!\n", __LINE__);

        jloader = env->CallStaticObjectMethod(clKlass, m);
        CheckErrorAndLog(env, "GetMethodID getSystemClassLoader : %d!\n", __LINE__);

        env->DeleteLocalRef(clKlass);
    }

    m = env->GetMethodID(parcelClass, "readParcelable", "(Ljava/lang/ClassLoader;)Landroid/os/Parcelable;");
    CheckErrorAndLog(env, "GetMethodID readParcelable : %d!\n", __LINE__);

    jobject jparcelable = env->CallObjectMethod(jparcel, m, jloader);
    CheckErrorAndLog(env, "CallObjectMethod readParcelable : %d!\n", __LINE__);

    env->DeleteLocalRef(parcelClass);
    env->DeleteLocalRef(jparcel);
    env->DeleteLocalRef(jloader);
    return jparcelable;
}

jobject Util::ToJavaContentProviderOperation(
    /* [in] */ JNIEnv* env,
    /* [in] */ IContentProviderOperation* operation)
{
    if (env == NULL || operation == NULL) {
        ALOGE("Util::ToJavaContentProviderOperation() invalid param!");
        return NULL;
    }

    jclass cpobklass = env->FindClass("android/content/ContentProviderOperation$Builder");
    CheckErrorAndLog(env, "ToJavaContentProviderOperation FindClass: ContentProviderOperation$Builder : %d!\n", __LINE__);

    jmethodID m = env->GetMethodID(cpobklass, "<init>", "(ILandroid/net/Uri;)V");
    CheckErrorAndLog(env, "ToJavaContentProviderOperation GetMethodID: ContentProviderOperation$Builder() : %d!\n", __LINE__);


    AutoPtr<IParcel> parcel;
    Elastos::Droid::Os::CParcel::New((IParcel**)&parcel);
    IParcelable::Probe(operation)->WriteToParcel(parcel);
    parcel->SetDataPosition(0);

    Int32 type = 0;
    parcel->ReadInt32(&type);

    AutoPtr<IUri> uri;
    Uri::ReadFromParcel(parcel, (IUri**)&uri);

    jobject juri = NULL;
    if (uri !=  NULL) {
        juri = Util::ToJavaUri(env, uri);
    }

    jobject jbuilder = env->NewObject(cpobklass, m, (jint)type, juri);
    CheckErrorAndLog(env, "ToJavaContentProviderOperation NewObject: ContentProviderOperation$Builder : %d!\n", __LINE__);

    Int32 tempInt = 0;
    if ((parcel->ReadInt32(&tempInt), tempInt) != 0) {
        AutoPtr<IInterface> contentValues;
        parcel->ReadInterfacePtr((Handle32*)(IInterface**)&contentValues);
        AutoPtr<IContentValues> values = IContentValues::Probe(contentValues);

        jobject jvalues = Util::ToJavaContentValues(env, values);

        jfieldID f = env->GetFieldID(cpobklass, "mValues", "Landroid/content/ContentValues;");
        Util::CheckErrorAndLog(env, "ToJavaContentProviderOperation", "GetFieldID: mValues %d", __LINE__);

        env->SetObjectField(jbuilder, f, jvalues);
        Util::CheckErrorAndLog(env, "ToJavaContentProviderOperation", "SetObjectField: mValues %d", __LINE__);

        env->DeleteLocalRef(jvalues);
    }

    if ((parcel->ReadInt32(&tempInt), tempInt) != 0) {
        String selection;
        parcel->ReadString(&selection);
        SetJavaStringField(env, cpobklass, jbuilder, selection, "mSelection", "ToJavaContentProviderOperation");
    }

    if ((parcel->ReadInt32(&tempInt), tempInt) != 0) {
        AutoPtr<ArrayOf<String> > selectionArgs;
        parcel->ReadArrayOfString((ArrayOf<String>**)&selectionArgs);
        jobjectArray jselectionArgs = Util::ToJavaStringArray(env, selectionArgs);

        jfieldID f = env->GetFieldID(cpobklass, "mSelectionArgs", "[Ljava/lang/String;");
        Util::CheckErrorAndLog(env, "ToJavaContentProviderOperation", "GetFieldID: mSelectionArgs %d", __LINE__);

        env->SetObjectField(jbuilder, f, jselectionArgs);
        Util::CheckErrorAndLog(env, "ToJavaContentProviderOperation", "SetObjectField: mSelectionArgs %d", __LINE__);
    }

    if ((parcel->ReadInt32(&tempInt), tempInt) != 0) {
        parcel->ReadInt32(&tempInt);
        Util::SetJavaIntegerField(env, cpobklass, jbuilder, tempInt, "mExpectedCount", "ToJavaContentProviderOperation");
    }

    if ((parcel->ReadInt32(&tempInt), tempInt) != 0) {
        AutoPtr<IContentValues> valuesBackReferences;
        CContentValues::New((IContentValues**)&valuesBackReferences);
        IParcelable::Probe(valuesBackReferences)->ReadFromParcel(parcel);

        jobject jvaluesBackReferences = Util::ToJavaContentValues(env, valuesBackReferences);

        jfieldID f = env->GetFieldID(cpobklass, "mValuesBackReferences", "Landroid/content/ContentValues;");
        Util::CheckErrorAndLog(env, "ToJavaContentProviderOperation", "GetFieldID: mValuesBackReferences %d", __LINE__);

        env->SetObjectField(jbuilder, f, jvaluesBackReferences);
        Util::CheckErrorAndLog(env, "ToJavaContentProviderOperation", "SetObjectField: mValuesBackReferences %d", __LINE__);

        env->DeleteLocalRef(jvaluesBackReferences);
    }

    if ((parcel->ReadInt32(&tempInt), tempInt) != 0) {
        jclass hmKlass = env->FindClass("java/util/HashMap");
        Util::CheckErrorAndLog(env, "ToJavaContentProviderOperation", "FindClass: HashMap line: %d", __LINE__);

        jmethodID m = env->GetMethodID(hmKlass, "<init>", "()V");
        Util::CheckErrorAndLog(env, "ToJavaContentProviderOperation", "GetMethodID: HashMap line: %d", __LINE__);

        jobject jselectionArgsBackReferences = env->NewObject(hmKlass, m);
        Util::CheckErrorAndLog(env, "ToJavaContentProviderOperation", "NewObject: HashMap line: %d", __LINE__);

        m = env->GetMethodID(hmKlass, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
        Util::CheckErrorAndLog(env, "ToJavaContentProviderOperation", "GetMethodID: put line: %d", __LINE__);

        Int32 size = 0;
        parcel->ReadInt32(&size);
        Int32 key = 0;
        Int32 value = 0;
        for (Int32 i = 0; i < size; i++) {
            parcel->ReadInt32(&key);
            parcel->ReadInt32(&value);

            jclass jintegerCls = env->FindClass("java/lang/Integer");
            Util::CheckErrorAndLog(env, "ToJavaContentProviderOperation", "FindClass: Integer line: %d", __LINE__);

            jmethodID integerInit = env->GetMethodID(jintegerCls, "<init>", "(I)V");
            Util::CheckErrorAndLog(env, "ToJavaContentProviderOperation", "GetMethodID: Integer line: %d", __LINE__);

            jobject jintegerK = env->NewObject(jintegerCls, integerInit, key);
            Util::CheckErrorAndLog(env, "ToJavaContentProviderOperation", "NewObject: Integer K line: %d", __LINE__);
            jobject jintegerV = env->NewObject(jintegerCls, integerInit, value);
            Util::CheckErrorAndLog(env, "ToJavaContentProviderOperation", "NewObject: Integer V line: %d", __LINE__);

            jobject jvalue = env->CallObjectMethod(jselectionArgsBackReferences, m, jintegerK, jintegerV);
            Util::CheckErrorAndLog(env, "ToJavaHashMap", "CallObjectMethod: put line: %d", __LINE__);

            env->DeleteLocalRef(jvalue);
            env->DeleteLocalRef(jintegerV);
            env->DeleteLocalRef(jintegerK);
            env->DeleteLocalRef(jintegerCls);
            env->DeleteLocalRef(jvalue);
        }

        jfieldID f = env->GetFieldID(cpobklass, "mSelectionArgsBackReferences", "Ljava/util/Map;");
        Util::CheckErrorAndLog(env, "ToJavaContentProviderOperation", "GetFieldID: mSelectionArgsBackReferences %d", __LINE__);

        env->SetObjectField(jbuilder, f, jselectionArgsBackReferences);
        Util::CheckErrorAndLog(env, "ToJavaContentProviderOperation", "SetObjectField: mSelectionArgsBackReferences %d", __LINE__);

        env->DeleteLocalRef(hmKlass);
        env->DeleteLocalRef(jselectionArgsBackReferences);
    }

    parcel->ReadInt32(&tempInt),
    Util::SetJavaBoolField(env, cpobklass, jbuilder, tempInt != 0, "mYieldAllowed", "ToJavaContentProviderOperation");

    jclass cpoklass = env->FindClass("android/content/ContentProviderOperation");
    CheckErrorAndLog(env, "ToJavaContentProviderOperation FindClass: ContentProviderOperation : %d!\n", __LINE__);

    m = env->GetMethodID(cpoklass, "<init>", "(Landroid/content/ContentProviderOperation$Builder;)V");
    CheckErrorAndLog(env, "ToJavaContentProviderOperation GetMethodID: ContentProviderOperation() : %d!\n", __LINE__);

    jobject joperation = env->NewObject(cpoklass, m, jbuilder);
    CheckErrorAndLog(env, "ToJavaContentProviderOperation NewObject: ContentProviderOperation : %d!\n", __LINE__);

    env->DeleteLocalRef(cpoklass);
    env->DeleteLocalRef(cpobklass);
    env->DeleteLocalRef(jbuilder);
    return joperation;
}

Boolean Util::GetElContentProviderResult(
    /* [in] */ JNIEnv* env,
    /* [in] */ jobject jresult,
    /* [out] */ IContentProviderResult** result)
{
    if (env == NULL || jresult == NULL || result == NULL) {
        LOGGERE("GetElContentProviderResult", "Util::GetElContentProviderResult() invalid param!");
        return FALSE;
    }

    jclass cprKlass = env->FindClass("android/content/ContentProviderResult");
    Util::CheckErrorAndLog(env, "GetElContentProviderResult", "FindClass: ContentProviderResult : %d!\n", __LINE__);

    jfieldID f = env->GetFieldID(cprKlass, "uri", "Landroid/net/Uri;");
    Util::CheckErrorAndLog(env, "GetElContentProviderResult", "GetFieldID: uri %d", __LINE__);

    jobject juri = env->GetObjectField(jresult, f);
    Util::CheckErrorAndLog(env,"GetElContentProviderResult", "GetObjectField: uri %d", __LINE__);

    AutoPtr<IUri> uri;
    if (juri != NULL)  {
        if (!Util::GetElUri(env, juri, (IUri**)&uri)) {
            LOGGERE("GetElContentProviderResult", "GetElContentProviderResult() GetElIntent fail %d", __LINE__);
        }

        if (NOERROR != CContentProviderResult::New(uri, result)) {
            LOGGERE("GetElContentProviderResult", "GetElContentProviderResult new CContentProviderResult fail!");
            env->DeleteLocalRef(cprKlass);
            return FALSE;
        }

        env->DeleteLocalRef(juri);
    }
    else {
        Int32 count = Util::GetJavaIntegerField(env, cprKlass, jresult, "count", 0, "GetElContentProviderResult");

        if (NOERROR != CContentProviderResult::New(count, result)) {
            LOGGERE("GetElContentProviderResult", "GetElContentProviderResult new CContentProviderResult fail!");
            env->DeleteLocalRef(cprKlass);
            return FALSE;
        }
    }

    env->DeleteLocalRef(cprKlass);
    return TRUE;
}

jobject Util::ToJavaSuggestionsInfo(
    /* [in] */ JNIEnv* env,
    /* [in] */ ISuggestionsInfo* info)
{
    if (env == NULL || info == NULL) {
        LOGGERE("ToJavaSuggestionsInfo", "Invalid arguments!");
        return NULL;
    }

    Int32 suggestionsAttributes = 0;
    info->GetSuggestionsAttributes(&suggestionsAttributes);

    Int32 count = 0;
    info->GetSuggestionsCount(&count);

    jobjectArray jsuggestions = NULL;
    if (count > 0) {
        jclass stringKlass = env->FindClass("java/lang/String");
        Util::CheckErrorAndLog(env, "ToJavaSuggestionsInfo", "FindClass: String %d", __LINE__);

        jsuggestions = env->NewObjectArray((jsize)count, stringKlass, 0);
        Util::CheckErrorAndLog(env, "ToJavaSuggestionsInfo", "FindClass: String %d", __LINE__);

        for(Int32 i = 0; i < count; i++ ) {
            String suggestion;
            info->GetSuggestionAt(i, &suggestion);
            jstring jsuggestion = ToJavaString(env, suggestion);
            if (jsuggestion != NULL) {
                env->SetObjectArrayElement(jsuggestions, i, jsuggestion);
                Util::CheckErrorAndLog(env, "ToJavaSuggestionsInfo", "SetObjectArrayElement fail %d", __LINE__);
                env->DeleteLocalRef(jsuggestion);
            }
        }

        env->DeleteLocalRef(stringKlass);
    }

    Int32 cookie = 0;
    info->GetCookie(&cookie);

    Int32 sequence = 0;
    info->GetSequence(&sequence);

    jclass siKlass = env->FindClass("android/view/textservice/SuggestionsInfo");
    Util::CheckErrorAndLog(env, "ToJavaSuggestionsInfo", "FindClass: SuggestionsInfo line: %d", __LINE__);

    jmethodID m = env->GetMethodID(siKlass, "<init>", "(I[Ljava/lang/String;II)V");
    Util::CheckErrorAndLog(env, "ToJavaSuggestionsInfo", "GetMethodID: SuggestionsInfo line: %d", __LINE__);

    jobject jinfo = env->NewObject(siKlass, m, (jint)suggestionsAttributes, jsuggestions, (jint)cookie, (jint)sequence);
    Util::CheckErrorAndLog(env, "ToJavaIvParameterSpec", "NewObject SuggestionsInfo line: %d", __LINE__);

    env->DeleteLocalRef(siKlass);
    return jinfo;
}

jobject Util::ToJavaSentenceSuggestionsInfo(
    /* [in] */ JNIEnv* env,
    /* [in] */ ISentenceSuggestionsInfo* info)
{
    if (env == NULL || info == NULL) {
        LOGGERE("ToJavaSentenceSuggestionsInfo", "Invalid arguments!");
        return NULL;
    }

    jobjectArray jsuggestionsInfos = NULL;
    jintArray joffsets = NULL;
    jintArray jlengths = NULL;

    Int32 count = 0;
    info->GetSuggestionsCount(&count);
    if (count > 0) {
        jclass siKlass = env->FindClass("android/view/textservice/SuggestionsInfo");
        Util::CheckErrorAndLog(env, "ToJavaSentenceSuggestionsInfo", "FindClass: String %d", __LINE__);

        jsuggestionsInfos = env->NewObjectArray((jsize)count, siKlass, 0);
        Util::CheckErrorAndLog(env, "ToJavaSentenceSuggestionsInfo", "NewObjectArray: SuggestionsInfo %d", __LINE__);

        joffsets = env->NewIntArray((jsize)count);
        Util::CheckErrorAndLog(env, "ToJavaSentenceSuggestionsInfo", "NewIntArray %d", __LINE__);

        jlengths = env->NewIntArray((jsize)count);
        Util::CheckErrorAndLog(env, "ToJavaSentenceSuggestionsInfo", "NewIntArray %d", __LINE__);

        for(Int32 i = 0; i < count; i++ ) {
            AutoPtr<ISuggestionsInfo> suggestion;
            info->GetSuggestionsInfoAt(i, (ISuggestionsInfo**)&suggestion);
            jobject jsuggestion = ToJavaSuggestionsInfo(env, suggestion);

            Int32 offset = 0;
            info->GetOffsetAt(i, &offset);

            Int32 length = 0;
            info->GetLengthAt(i, &length);

            if (jsuggestion != NULL) {
                env->SetObjectArrayElement(jsuggestionsInfos, i, jsuggestion);
                Util::CheckErrorAndLog(env, "ToJavaSentenceSuggestionsInfo", "SetObjectArrayElement fail %d", __LINE__);

                env->SetIntArrayRegion(joffsets, 0, 1, &offset);
                Util::CheckErrorAndLog(env, "ToJavaSentenceSuggestionsInfo", "SetIntArrayRegion fail %d", __LINE__);

                env->SetIntArrayRegion(jlengths, 0, 1, &length);
                Util::CheckErrorAndLog(env, "ToJavaSentenceSuggestionsInfo", "SetIntArrayRegion fail %d", __LINE__);

                env->DeleteLocalRef(jsuggestion);
            }
        }

        env->DeleteLocalRef(siKlass);
    }

    jclass ssiKlass = env->FindClass("android/view/textservice/SentenceSuggestionsInfo");
    Util::CheckErrorAndLog(env, "ToJavaSuggestionsInfo", "FindClass: SentenceSuggestionsInfo line: %d", __LINE__);

    jmethodID m = env->GetMethodID(ssiKlass, "<init>", "([Landroid/view/textservice/SuggestionsInfo;[I[I)V");
    Util::CheckErrorAndLog(env, "ToJavaSuggestionsInfo", "GetMethodID: SentenceSuggestionsInfo line: %d", __LINE__);

    jobject jinfo = env->NewObject(ssiKlass, m, jsuggestionsInfos, joffsets, jlengths);
    Util::CheckErrorAndLog(env, "ToJavaIvParameterSpec", "NewObject SentenceSuggestionsInfo line: %d", __LINE__);

    env->DeleteLocalRef(ssiKlass);
    return jinfo;
}

jobject Util::ToJavaMessenger(
    /* [in] */ JNIEnv* env,
    /* [in] */ IMessenger* msgr)
{
    if (env == NULL || msgr == NULL) {
        LOGGERE("ToJavaMessenger", "Invalid arguments!");
        return NULL;
    }

    jobject jmsgr = NULL;

    jclass msgerKlass = env->FindClass("android/os/Messenger");
    Util::CheckErrorAndLog(env, "ToJavaMessenger", "NewObject Messenger line: %d", __LINE__);

    AutoPtr<IIMessenger> mgr;
    msgr->GetIMessenger((IIMessenger**)&mgr);

    jclass elmsgerKlass = env->FindClass("android/os/ElMessengerProxy");
    Util::CheckErrorAndLog(env, "ToJavaMessenger", "FindClass ElMessengerProxy line: %d", __LINE__);

    jmethodID m = env->GetMethodID(elmsgerKlass, "<init>", "(J)V");
    Util::CheckErrorAndLog(env, "ToJavaMessenger", "GetMethodID ElMessengerProxy line: %d", __LINE__);

    jobject jelMgr = env->NewObject(elmsgerKlass, m, (jlong)mgr.Get());
    Util::CheckErrorAndLog(env, "ToJavaMessenger", "NewObject ElMessengerProxy line: %d", __LINE__);
    mgr->AddRef();

    m = env->GetMethodID(msgerKlass, "<init>", "(Landroid/os/IBinder;)V");
    Util::CheckErrorAndLog(env, "ToJavaMessenger", "GetMethodID Messenger line: %d", __LINE__);

    jmsgr = env->NewObject(msgerKlass, m, jelMgr);
    Util::CheckErrorAndLog(env, "ToJavaMessenger", "NewObject Messenger line: %d", __LINE__);

    env->DeleteLocalRef(jelMgr);
    env->DeleteLocalRef(msgerKlass);
    env->DeleteLocalRef(elmsgerKlass);
    return jmsgr;
}

Boolean Util::GetElVerifierInfo(
    /* [in] */ JNIEnv* env,
    /* [in] */ jobject jinfo,
    /* [out] */ IVerifierInfo** info)
{
    if (env == NULL || jinfo == NULL || info == NULL) {
        ALOGE("Util::GetElVerifierInfo() invalid param!");
        return FALSE;
    }

    jclass vfKlass = env->FindClass("android/content/pm/VerifierInfo");
    Util::CheckErrorAndLog(env, "GetElVerifierInfo", "FindClass: VerifierInfo : %d!\n", __LINE__);

    String packageName = Util::GetJavaStringField(env, vfKlass, jinfo, "packageName", "GetElVerifierInfo");

    jfieldID f = env->GetFieldID(vfKlass, "publicKey", "Ljava/security/PublicKey;");
    Util::CheckErrorAndLog(env, "GetElVerifierInfo", "GetFieldID: publicKey %d", __LINE__);

    jobject jpublicKey = env->GetObjectField(jinfo, f);
    Util::CheckErrorAndLog(env,"GetElVerifierInfo", "GetObjectField: publicKey %d", __LINE__);

    AutoPtr<IPublicKey> publicKey;
    if (jpublicKey != NULL) {
        if (!Util::GetElPublicKey(env, jpublicKey, (IPublicKey**)&publicKey)) {
            ALOGE("GetElVerifierInfo() GetElPublicKey fail!");
        }
        env->DeleteLocalRef(jpublicKey);
    }

    if (NOERROR != CVerifierInfo::New(packageName, publicKey, info)) {
        LOGGERE("GetElVerifierInfo", "new CVerifierInfo fail!");
        env->DeleteLocalRef(vfKlass);
        return FALSE;
    }

    env->DeleteLocalRef(vfKlass);
    return TRUE;
}

Boolean Util::GetElPublicKey(
    /* [in] */ JNIEnv* env,
    /* [in] */ jobject jkey,
    /* [out] */ IPublicKey** key)
{
    if (env == NULL || jkey == NULL || key == NULL) {
        ALOGE("Util::GetElPublicKey() invalid param!");
        return FALSE;
    }

    // TODO:
    ALOGE("Util::GetElPublicKey() E_NOT_IMPLEMENTED");
    return FALSE;
}

jobject Util::ToJavaRestoreSet(
    /* [in] */ JNIEnv* env,
    /* [in] */ IRestoreSet* rs)
{
    if (env == NULL || rs == NULL) {
        LOGGERE("ToJavaRestoreSet", "Invalid arguments!");
        return NULL;
    }

    String name;
    rs->GetName(&name);
    jstring jname = Util::ToJavaString(env, name);

    String device;
    rs->GetDevice(&device);
    jstring jdevice = Util::ToJavaString(env, device);

    Int64 token = 0;
    rs->GetToken(&token);

    jclass rsKlass = env->FindClass("android/app/backup/RestoreSet");
    Util::CheckErrorAndLog(env, "ToJavaRestoreSet", "NewObject RestoreSet line: %d", __LINE__);

    jmethodID m = env->GetMethodID(rsKlass, "<init>", "(Ljava/lang/String;Ljava/lang/String;J)V");
    Util::CheckErrorAndLog(env, "ToJavaRestoreSet", "GetMethodID RestoreSet line: %d", __LINE__);

    jobject jrs = env->NewObject(rsKlass, m, jname, jdevice, (jlong)token);
    Util::CheckErrorAndLog(env, "ToJavaRestoreSet", "NewObject RestoreSet line: %d", __LINE__);

    env->DeleteLocalRef(rsKlass);
    env->DeleteLocalRef(jname);
    env->DeleteLocalRef(jdevice);
    return jrs;
}

jobject Util::ToJavaCountry(
    /* [in] */ JNIEnv* env,
    /* [in] */ ICountry* country)
{
    if (env == NULL || country == NULL) {
        LOGGERE("ToJavaCountry", "Invalid arguments!");
        return NULL;
    }

    jclass cKlass = env->FindClass("android/location/Country");
    Util::CheckErrorAndLog(env, "ToJavaCountry", "NewObject Country line: %d", __LINE__);

    jmethodID m = env->GetMethodID(cKlass, "<init>", "(Ljava/lang/String;IJ)V");
    Util::CheckErrorAndLog(env, "ToJavaCountry", "GetMethodID Country line: %d", __LINE__);

    String countryIso;
    country->GetCountryIso(&countryIso);
    jstring jcountryIso = Util::ToJavaString(env, countryIso);

    Int32 source = 0;
    country->GetSource(&source);

    Int64 timestamp = 0;
    country->GetTimestamp(&timestamp);

    jobject jcountry = env->NewObject(cKlass, m, jcountryIso, (jint)source, (jlong)timestamp);
    Util::CheckErrorAndLog(env, "ToJavaCountry", "NewObject Country line: %d", __LINE__);

    env->DeleteLocalRef(cKlass);
    env->DeleteLocalRef(jcountryIso);
    return jcountry;
}

jobject Util::ToJavaPersistableBundle(
    /* [in] */ JNIEnv* env,
    /* [in] */ IPersistableBundle* persistableBundle)
{
    if (env == NULL || persistableBundle == NULL) {
        LOGGERE("ToJavaPersistableBundle", "Invalid arguments!");
        return NULL;
    }

    jclass bundleKlass = env->FindClass("android/os/PersistableBundle");
    Util::CheckErrorAndLog(env, "ToJavaPersistableBundle", "Fail FindClass: PersistableBundle %d", __LINE__);

    jmethodID m = env->GetMethodID(bundleKlass, "<init>", "()V");
    Util::CheckErrorAndLog(env, "ToJavaPersistableBundle", "Fail GetMethodID: bundleKlass %d", __LINE__);

    jobject jBundle = env->NewObject(bundleKlass, m);
    Util::CheckErrorAndLog(env, "ToJavaPersistableBundle", "Fail NewObject: bundleKlass %d", __LINE__);

    if (!SetJavaBaseBundle(env, IBaseBundle::Probe(persistableBundle), bundleKlass, jBundle)) {
        LOGGERE(TAG, "ToJavaPersistableBundle SetJavaBaseBundle failed!");
    }
    env->DeleteLocalRef(bundleKlass);
    return jBundle;
}

jobject Util::ToJavaProfilerInfo(
    /* [in] */ JNIEnv* env,
    /* [in] */ IProfilerInfo* profilerInfo)
{
    if (env == NULL || profilerInfo == NULL) {
        LOGGERE("ToJavaProfilerInfo", "Invalid arguments!");
        return NULL;
    }

    String profileFile;
    profilerInfo->GetProfileFile(&profileFile);
    jstring jprofileFile = ToJavaString(env, profileFile);

    AutoPtr<IParcelFileDescriptor> profileFd;
    profilerInfo->GetProfileFd((IParcelFileDescriptor**)&profileFd);
    jobject jprofileFd = ToJavaParcelFileDescriptor(env, profileFd);

    Int32 samplingInterval;
    profilerInfo->GetSamplingInterval(&samplingInterval);

    Boolean autoStopProfiler;
    profilerInfo->IsAutoStopProfiler(&autoStopProfiler);

    jclass piKlass = env->FindClass("android/app/ProfilerInfo");
    Util::CheckErrorAndLog(env, "ToJavaProfilerInfo", "Fail FindClass: ProfilerInfo %d", __LINE__);

    jmethodID m = env->GetMethodID(piKlass, "<init>", "(Ljava/lang/String;Landroid/os/ParcelFileDescriptor;IZ)V");
    Util::CheckErrorAndLog(env, "ToJavaProfilerInfo", "Fail GetMethodID: piKlass %d", __LINE__);

    jobject jprofilerInfo = env->NewObject(piKlass, m, jprofileFile, jprofileFd, samplingInterval, autoStopProfiler);
    Util::CheckErrorAndLog(env, "ToJavaProfilerInfo", "Fail NewObject: piKlass %d", __LINE__);

    env->DeleteLocalRef(jprofileFile);
    env->DeleteLocalRef(jprofileFd);
    env->DeleteLocalRef(piKlass);

    return jprofilerInfo;
}

jobject Util::ToJavaResultInfo(
    /* [in] */ JNIEnv* env,
    /* [in] */ IResultInfo* resultInfo)
{
    if (env == NULL || resultInfo == NULL) {
        LOGGERE("ToJavaResultInfo", "Invalid arguments!");
        return NULL;
    }

    String resultWho;
    resultInfo->GetResultWho(&resultWho);
    jstring jresultWho = ToJavaString(env, resultWho);

    Int32 requestCode;
    resultInfo->GetRequestCode(&requestCode);

    Int32 resultCode;
    resultInfo->GetResultCode(&resultCode);

    AutoPtr<IIntent> data;
    resultInfo->GetData((IIntent**)&data);
    jobject jdata = ToJavaIntent(env, data);

    jclass riKlass = env->FindClass("android/app/ResultInfo");
    Util::CheckErrorAndLog(env, "ToJavaResultInfo", "Fail FindClass: ResultInfo %d", __LINE__);

    jmethodID m = env->GetMethodID(riKlass, "<init>", "(Ljava/lang/String;IILandroid/content/Intent;)V");
    Util::CheckErrorAndLog(env, "ToJavaResultInfo", "Fail GetMethodID: ResultInfo %d", __LINE__);

    jobject jresultInfo = env->NewObject(riKlass, m, jresultWho, requestCode, resultCode, jdata);
    Util::CheckErrorAndLog(env, "ToJavaResultInfo", "Fail NewObject: ResultInfo %d", __LINE__);

    env->DeleteLocalRef(jresultWho);
    env->DeleteLocalRef(jdata);
    env->DeleteLocalRef(riKlass);
    return jresultInfo;
}

jobject Util::ToJavaRating(
    /* [in] */ JNIEnv* env,
    /* [in] */ IRating* rating)
{
    if (env == NULL || rating == NULL) {
        LOGGERE("ToJavaRating", "Invalid arguments!");
        return NULL;
    }

    AutoPtr<IParcel> parcel;
    Elastos::Droid::Os::CParcel::New((IParcel**)&parcel);
    IParcelable::Probe(rating)->WriteToParcel(parcel);
    parcel->SetDataPosition(0);

    Int32 ratingStyle;
    parcel->ReadInt32(&ratingStyle);

    Float ratingValue;
    parcel->ReadFloat(&ratingValue);

    jclass rKlass = env->FindClass("android/meida/Rating");
    Util::CheckErrorAndLog(env, "ToJavaRating", "Fail FindClass: Rating %d", __LINE__);

    jmethodID m = env->GetMethodID(rKlass, "<init>", "(IF)V");
    Util::CheckErrorAndLog(env, "ToJavaRating", "Fail GetMethodID: Rating %d", __LINE__);

    jobject jrating = env->NewObject(rKlass, m, ratingStyle, ratingValue);
    Util::CheckErrorAndLog(env, "ToJavaRating", "Fail NewObject: Rating %d", __LINE__);

    env->DeleteLocalRef(rKlass);
    return jrating;
}

jobject Util::ToJavaAudioAttributes(
    /* [in] */ JNIEnv* env,
    /* [in] */ IAudioAttributes* aa)
{
    if (env == NULL || aa == NULL) {
        LOGGERE("ToJavaAudioAttributes", "Invalid arguments!");
        return NULL;
    }

    jclass aaKlass = env->FindClass("android/meida/AudioAttributes");
    Util::CheckErrorAndLog(env, "ToJavaAudioAttributes", "Fail FindClass: AudioAttributes %d", __LINE__);

    jmethodID m = env->GetMethodID(aaKlass, "<init>", "()V");
    Util::CheckErrorAndLog(env, "ToJavaAudioAttributes", "Fail GetMethodID: AudioAttributes %d", __LINE__);

    jobject jaa = env->NewObject(aaKlass, m);
    Util::CheckErrorAndLog(env, "ToJavaAudioAttributes", "Fail NewObject: AudioAttributes %d", __LINE__);

    AutoPtr<IParcel> parcel;
    Elastos::Droid::Os::CParcel::New((IParcel**)&parcel);
    IParcelable::Probe(aa)->WriteToParcel(parcel);
    parcel->SetDataPosition(0);

    Int32 tempInt;
    parcel->ReadInt32(&tempInt);
    Util::SetJavaIntField(env, aaKlass, jaa, tempInt, "mUsage", "ToJavaAudioAttributes");

    parcel->ReadInt32(&tempInt);
    Util::SetJavaIntField(env, aaKlass, jaa, tempInt, "mContentType", "ToJavaAudioAttributes");

    parcel->ReadInt32(&tempInt);
    Util::SetJavaIntField(env, aaKlass, jaa, tempInt, "mSource", "ToJavaAudioAttributes");

    parcel->ReadInt32(&tempInt);
    Util::SetJavaIntField(env, aaKlass, jaa, tempInt, "mFlags", "ToJavaAudioAttributes");

    String formattedTags;
    parcel->ReadString(&formattedTags);
    Util::SetJavaStringField(env, aaKlass, jaa, formattedTags, "mFormattedTags", "ToJavaAudioAttributes");

    AutoPtr<ISet> set;
    aa->GetTags((ISet**)&set);
    if (set != NULL) {
        AutoPtr<ArrayOf<IInterface*> > tags;
        set->ToArray((ArrayOf<IInterface*>**)&tags);

        jclass hsKlass = env->FindClass("java/util/HashSet");
        Util::CheckErrorAndLog(env, "ToJavaAudioAttributes", "FindClass: HashSet line: %d", __LINE__);

        jmethodID m = env->GetMethodID(hsKlass, "<init>", "()V");
        Util::CheckErrorAndLog(env, "ToJavaAudioAttributes", "GetMethodID: HashSet line: %d", __LINE__);

        jobject jset = env->NewObject(hsKlass, m);
        Util::CheckErrorAndLog(env, "ToJavaAudioAttributes", "NewObject: HashSet line: %d", __LINE__);

        m = env->GetMethodID(hsKlass, "add", "(Ljava/lang/Object;)Z");
        Util::CheckErrorAndLog(env, "ToJavaAudioAttributes", "GetMethodID: add line: %d", __LINE__);

        for (Int32 i = 0; i < tags->GetLength(); i++) {
            AutoPtr<IInterface> item = (*tags)[i];
            String tag;
            ICharSequence::Probe(item)->ToString(&tag);
            jstring jtag = Util::ToJavaString(env, tag);
            env->CallBooleanMethod(jset, m, jtag);
            env->DeleteLocalRef(jtag);
        }
        jfieldID f = env->GetFieldID(aaKlass, "mTags", "Ljava/util/HashSet;");
        Util::CheckErrorAndLog(env, "ToJavaAudioAttributes", "GetFieldID: mTags %d", __LINE__);

        env->SetObjectField(jaa, f, jset);
        Util::CheckErrorAndLog(env, "ToJavaAudioAttributes", "SetObjectField: jdata %d", __LINE__);
        env->DeleteLocalRef(hsKlass);
        env->DeleteLocalRef(jset);
    }

    env->DeleteLocalRef(aaKlass);
    return jaa;
}

} // JavaProxy
} // Droid
} // Elastos
