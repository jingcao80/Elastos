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

#include "elastos/droid/javaproxy/CCursorNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include "Elastos.Droid.Content.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Database::EIID_ICursor;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::IO::EIID_ICloseable;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CCursorNative::TAG("CCursorNative");

HashMap<IContentObserver*, jobject> CCursorNative::mContentObserverMap;
HashMap<IDataSetObserver*, jobject> CCursorNative::mDataSetObserverMap;

CAR_INTERFACE_IMPL_3(CCursorNative, Object, ICursor, ICloseable, IBinder)

CAR_OBJECT_IMPL(CCursorNative)

CCursorNative::~CCursorNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CCursorNative::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CCursorNative::GetCount(
    /* [out] */ Int32* count)
{
    // LOGGERD(TAG, "+ CCursorNative::GetCount()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "getCount", "()I");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: getCount %d", __LINE__);

    *count = env->CallIntMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallIntMethod: getCount %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CCursorNative::GetCount()");
    return NOERROR;
}

ECode CCursorNative::GetColumnIndexOrThrow(
    /* [in] */ const String& columnName,
    /* [out] */ Int32* columnIndex)
{
    // LOGGERD(TAG, "+ CCursorNative::GetColumnIndexOrThrow()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    *columnIndex = -1;

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jobject jcolumnName = Util::ToJavaString(env, columnName);

    jmethodID m = env->GetMethodID(c, "getColumnIndexOrThrow", "(Ljava/lang/String;)I");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: GetColumnIndexOrThrow %d", __LINE__);

    *columnIndex = env->CallIntMethod(mJInstance, m, jcolumnName);
    ECode ec = Util::CheckErrorAndLog(env, TAG, "Call GetColumnIndexOrThrow %d", __LINE__);
    if (FAILED(ec)) *columnIndex = -1;

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jcolumnName);

    // LOGGERD(TAG, "- CCursorNative::GetColumnIndexOrThrow()");
    return ec;
}

ECode CCursorNative::GetColumnIndex(
    /* [in] */ const String& columnName,
    /* [out] */ Int32* columnIndex)
{
    // LOGGERD(TAG, "+ CCursorNative::GetColumnIndex()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    *columnIndex = -1;

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jobject jcolumnName = Util::ToJavaString(env, columnName);

    jmethodID m = env->GetMethodID(c, "getColumnIndex", "(Ljava/lang/String;)I");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: GetColumnIndex %d", __LINE__);

    *columnIndex = env->CallIntMethod(mJInstance, m, jcolumnName);
    Util::CheckErrorAndLog(env, TAG, "CallIntMethod: GetColumnIndex %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jcolumnName);

    // LOGGERD(TAG, "- CCursorNative::GetColumnIndex()");
    return NOERROR;
}

ECode CCursorNative::MoveToPosition(
    /* [in] */ Int32 position,
    /* [out] */ Boolean* succeeded)
{
    // LOGGERD(TAG, "+ CCursorNative::MoveToPosition()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    *succeeded = FALSE;

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "moveToPosition", "(I)Z");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: moveToPosition %d", __LINE__);

    *succeeded = env->CallBooleanMethod(mJInstance, m, (jint)position);
    Util::CheckErrorAndLog(env, TAG, "CallBooleanMethod: moveToPosition %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CCursorNative::MoveToPosition()");
    return NOERROR;
}

ECode CCursorNative::GetInt64(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Int64* columnValue)
{
    // LOGGERD(TAG, "+ CCursorNative::GetInt64()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    *columnValue = 0;

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "getLong", "(I)J");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: moveToPosition %d", __LINE__);

    *columnValue = env->CallLongMethod(mJInstance, m, (jint)columnIndex);
    Util::CheckErrorAndLog(env, TAG, "CallLongMethod: getLong %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CCursorNative::GetInt64()");
    return NOERROR;
}

ECode CCursorNative::IsClosed(
    /* [out] */ Boolean* closed)
{
    // LOGGERD(TAG, "+ CCursorNative::IsClosed()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    *closed = FALSE;

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "isClosed", "()Z");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: moveToPosition %d", __LINE__);

    *closed = env->CallBooleanMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallBooleanMethod: isClosed %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CCursorNative::IsClosed()");
    return NOERROR;
}

ECode CCursorNative::Close()
{
    // LOGGERD(TAG, "+ CCursorNative::Close()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "close", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: Close %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: Close %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CCursorNative::Close()");
    return NOERROR;
}

ECode CCursorNative::Move(
    /* [in] */ Int32 offset,
    /* [out] */ Boolean* succeeded)
{
    // LOGGERD(TAG, "+ CCursorNative::Move()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    *succeeded = FALSE;

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "move", "(I)Z");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: move %d", __LINE__);

    *succeeded = env->CallBooleanMethod(mJInstance, m, (jint)offset);
    Util::CheckErrorAndLog(env, TAG, "CallBooleanMethod: move %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CCursorNative::Move()");
    return NOERROR;
}

ECode CCursorNative::GetPosition(
    /* [out] */ Int32* position)
{
    // LOGGERD(TAG, "+ CCursorNative::GetPosition()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    *position = -1;

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "getPosition", "()I");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: getPosition %d", __LINE__);

    *position = env->CallIntMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallIntMethod: getPosition %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CCursorNative::GetPosition()");
    return NOERROR;
}

ECode CCursorNative::MoveToFirst(
    /* [out] */ Boolean* succeeded)
{
    // LOGGERD(TAG, "+ CCursorNative::MoveToFirst()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    *succeeded = FALSE;

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "moveToFirst", "()Z");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: MoveToFirst %d", __LINE__);

    *succeeded = env->CallBooleanMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallBooleanMethod: MoveToFirst %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CCursorNative::MoveToFirst()");
    return NOERROR;
}

ECode CCursorNative::MoveToLast(
    /* [out] */ Boolean* succeeded)
{
    // LOGGERD(TAG, "+ CCursorNative::MoveToLast()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    *succeeded = FALSE;

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "moveToLast", "()Z");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: MoveToNext %d", __LINE__);

    *succeeded = env->CallBooleanMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallBooleanMethod: MoveToNext %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CCursorNative::MoveToFirst()");
    return NOERROR;
}

ECode CCursorNative::MoveToNext(
    /* [out] */ Boolean* succeeded)
{
    // LOGGERD(TAG, "+ CCursorNative::MoveToNext()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    *succeeded = FALSE;

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "moveToNext", "()Z");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: MoveToNext %d", __LINE__);

    *succeeded = env->CallBooleanMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallBooleanMethod: MoveToNext %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CCursorNative::MoveToNext()");
    return NOERROR;
}

ECode CCursorNative::MoveToPrevious(
    /* [out] */ Boolean* succeeded)
{
    // LOGGERD(TAG, "+ CCursorNative::MoveToPrevious()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    *succeeded = FALSE;

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "moveToPrevious", "()Z");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: MoveToPrevious %d", __LINE__);

    *succeeded = env->CallBooleanMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallBooleanMethod: MoveToPrevious %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CCursorNative::MoveToFirst()");
    return NOERROR;
}

ECode CCursorNative::IsFirst(
    /* [out] */ Boolean* isFirst)
{
    // LOGGERD(TAG, "+ CCursorNative::IsFirst()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    *isFirst = FALSE;

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "isFirst", "()Z");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: IsFirst %d", __LINE__);

    *isFirst = env->CallBooleanMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallBooleanMethod: IsFirst %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CCursorNative::IsFirst()");
    return NOERROR;
}

ECode CCursorNative::IsLast(
    /* [out] */ Boolean* isLast)
{
    // LOGGERD(TAG, "+ CCursorNative::IsLast()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    *isLast = FALSE;

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "isLast", "()Z");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: IsLast %d", __LINE__);

    *isLast = env->CallBooleanMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallBooleanMethod: IsLast %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CCursorNative::IsLast()");
    return NOERROR;
}

ECode CCursorNative::IsBeforeFirst(
    /* [out] */ Boolean* isBeforeFirst)
{
    // LOGGERD(TAG, "+ CCursorNative::IsBeforeFirst()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    *isBeforeFirst = FALSE;

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "isBeforeFirst", "()Z");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: IsBeforeFirst %d", __LINE__);

    *isBeforeFirst = env->CallBooleanMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallBooleanMethod: IsBeforeFirst %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CCursorNative::IsBeforeFirst()");
    return NOERROR;
}

ECode CCursorNative::IsAfterLast(
    /* [out] */ Boolean* isAfterLast)
{
    // LOGGERD(TAG, "+ CCursorNative::IsAfterLast()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    *isAfterLast = FALSE;

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "isAfterLast", "()Z");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: IsAfterLast %d", __LINE__);

    *isAfterLast = env->CallBooleanMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallBooleanMethod: IsAfterLast %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CCursorNative::IsAfterLast()");
    return NOERROR;
}

ECode CCursorNative::GetColumnName(
    /* [in] */ Int32 columnIndex,
    /* [out] */ String* columnName)
{
    // LOGGERD(TAG, "+ CCursorNative::GetColumnName()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "getColumnName", "(I)Ljava/lang/String;");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: GetColumnName %d", __LINE__);

    jstring jcolumnName = (jstring)env->CallObjectMethod(mJInstance, m, (jint)columnIndex);
    Util::CheckErrorAndLog(env, TAG, "CallObjectMethod: GetColumnName %d", __LINE__);

    *columnName = Util::GetElString(env, jcolumnName);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jcolumnName);

    // LOGGERD(TAG, "- CCursorNative::GetColumnName()");
    return NOERROR;
}

ECode CCursorNative::GetColumnNames(
    /* [out, callee] */ ArrayOf<String>** columnNames)
{
    VALIDATE_NOT_NULL(columnNames)
    *columnNames = NULL;
    // LOGGERD(TAG, "+ CCursorNative::GetColumnNames()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "getColumnNames", "()[Ljava/lang/String;");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: GetColumnNames %d", __LINE__);

    jobjectArray jcolumnNames = (jobjectArray)env->CallObjectMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallObjectMethod: GetColumnNames %d", __LINE__);

    if (jcolumnNames != NULL) {
        jint jcount = env->GetArrayLength(jcolumnNames);
        *columnNames = ArrayOf<String>::Alloc((Int32)jcount);
        REFCOUNT_ADD(*columnNames)
        if (*columnNames != NULL) {
            for (Int32 i = 0; i < jcount; i++) {
                jstring jcolumnName = (jstring)env->GetObjectArrayElement(jcolumnNames, i);
                Util::CheckErrorAndLog(env, "GetColumnNames", "Fail GetObjectArrayElement: jcolumnNames %d", __LINE__);
                String columnName = Util::GetElString(env, jcolumnName);
                (*columnNames)->Set(i, columnName);
                env->DeleteLocalRef(jcolumnName);
            }
        }
        env->DeleteLocalRef(jcolumnNames);
    }

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CCursorNative::GetColumnName()");
    return NOERROR;
}

ECode CCursorNative::GetColumnCount(
    /* [out] */ Int32* columnCount)
{
    // LOGGERD(TAG, "+ CCursorNative::GetColumnCount()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "getColumnCount", "()I");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: GetColumnCount %d", __LINE__);

    *columnCount = env->CallIntMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallIntMethod: GetColumnCount %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CCursorNative::GetColumnCount()");
    return NOERROR;
}

ECode CCursorNative::GetBlob(
    /* [in] */ Int32 columnIndex,
    /* [out, callee] */ ArrayOf<Byte>** blob)
{
    // LOGGERD(TAG, "+ CCursorNative::GetBlob()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "getBlob", "(I)[B");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: GetBlob %d", __LINE__);

    jbyteArray jblobs = (jbyteArray)env->CallObjectMethod(mJInstance, m, (jint)columnIndex);
    Util::CheckErrorAndLog(env, TAG, "CallObjectMethod: GetBlob %d", __LINE__);

    *blob = NULL;
    if (jblobs != NULL) {
        jint jcount = env->GetArrayLength(jblobs);
        if (jcount > 0) {
            jboolean isCopy;
            jbyte* jblob = env->GetByteArrayElements(jblobs, &isCopy);
            Util::CheckErrorAndLog(env, "GetBlob", "Fail GetByteArrayElements %d", __LINE__);
            *blob = ArrayOf<Byte>::Alloc((Int32)jcount);
            REFCOUNT_ADD(*blob)
            (*blob)->Copy((Byte*)jblob, (Int32)jcount);
        }
        env->DeleteLocalRef(jblobs);
    }

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CCursorNative::GetBlob()");
    return NOERROR;
}

ECode CCursorNative::GetString(
    /* [in] */ Int32 columnIndex,
    /* [out] */ String* columnValue)
{
    // LOGGERD(TAG, "+ CCursorNative::GetString()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "getString", "(I)Ljava/lang/String;");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: GetString %d", __LINE__);

    jstring jcolumnValue = (jstring)env->CallObjectMethod(mJInstance, m, (jint)columnIndex);
    ECode ec = Util::CheckErrorAndLog(env, TAG, "CallObjectMethod: GetString %d", __LINE__);

    if (SUCCEEDED(ec)) {
        *columnValue = Util::GetElString(env, jcolumnValue);
    }
    else *columnValue = NULL;

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jcolumnValue);

    // LOGGERD(TAG, "- CCursorNative::GetString()");
    return NOERROR;
}

ECode CCursorNative::GetInt32(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Int32* columnValue)
{
    // LOGGERD(TAG, "+ CCursorNative::GetInt32()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "getInt", "(I)I");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: GetInt32 %d", __LINE__);

    *columnValue = env->CallIntMethod(mJInstance, m, (jint)columnIndex);
    Util::CheckErrorAndLog(env, TAG, "CallIntMethod: GetInt32 %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CCursorNative::GetInt32()");
    return NOERROR;
}

ECode CCursorNative::RegisterContentObserver(
    /* [in] */ IContentObserver* observer)
{
    // LOGGERD(TAG, "+ CCursorNative::RegisterContentObserver()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jobserver = NULL;
    if (observer != NULL) {
        jclass c = env->FindClass("android/database/ElContentObserverProxy");
        Util::CheckErrorAndLog(env, TAG, "FindClass: ElContentObserverProxy %d", __LINE__);

        jmethodID m = env->GetMethodID(c, "<init>", "(J)V");
        Util::CheckErrorAndLog(env, TAG, "GetMethodID: ElContentObserverProxy %d", __LINE__);

        jobserver = env->NewObject(c, m, (jlong)observer);
        Util::CheckErrorAndLog(env, TAG, "NewObject: ElContentObserverProxy %d", __LINE__);
        observer->AddRef(); // add ref for jobserver

        jobject gobserver = env->NewGlobalRef(jobserver);
        mContentObserverMap[observer] = gobserver;
        observer->AddRef(); // add ref for mContentObserverMap

        env->DeleteLocalRef(c);
    }

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "registerContentObserver", "(Landroid/database/ContentObserver;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: registerContentObserver %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jobserver);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: registerContentObserver %d", __LINE__);
    env->DeleteLocalRef(jobserver);

    // LOGGERD(TAG, "- CCursorNative::RegisterContentObserver()");
    return NOERROR;
}

ECode CCursorNative::UnregisterContentObserver(
    /* [in] */ IContentObserver* observer)
{
    // LOGGERD(TAG, "+ CCursorNative::UnregisterContentObserver()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jobserver = NULL;
    if (observer != NULL) {
        if (mContentObserverMap.Find(observer) != mContentObserverMap.End()) {
            jobserver = mContentObserverMap[observer];
            mContentObserverMap.Erase(observer);
            observer->Release();
        }
        else {
            LOGGERE(TAG, String("UnregisterContentObserver() cannot find observer!"));
        }
    }

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "unregisterContentObserver", "(Landroid/database/ContentObserver;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: unregisterContentObserver %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jobserver);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: unregisterContentObserver %d", __LINE__);

    env->DeleteLocalRef(c);
    if(jobserver){
        env->DeleteGlobalRef(jobserver);
    }

    // LOGGERD(TAG, "- CCursorNative::UnregisterContentObserver()");
    return NOERROR;
}

ECode CCursorNative::Requery(
    /* [out] */ Boolean* succeeded)
{
    // LOGGERD(TAG, "+ CCursorNative::Requery()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "requery", "()Z");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: requery %d", __LINE__);

    *succeeded = (Boolean)env->CallBooleanMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallBooleanMethod: requery %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CCursorNative::Requery()");
    return NOERROR;
}

ECode CCursorNative::IsNull(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Boolean* isNull)
{
    // LOGGERD(TAG, "+ CCursorNative::IsNull()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "isNull", "(I)Z");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: isNull %d", __LINE__);

    *isNull = (Boolean)env->CallBooleanMethod(mJInstance, m, (jint)columnIndex);
    Util::CheckErrorAndLog(env, TAG, "CallBooleanMethod: isNull %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CCursorNative::IsNull()");
    return NOERROR;
}

ECode CCursorNative::CopyStringToBuffer(
    /* [in] */ Int32 columnIndex,
    /* [in] */ ICharArrayBuffer* buf)
{
    // LOGGERD(TAG, "+ CCursorNative::CopyStringToBuffer()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jbuf = NULL;
    if (buf != NULL) {
        jbuf = Util::ToJavaCharArrayBuffer(env, buf);
    }

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "copyStringToBuffer", "(ILandroid/database/CharArrayBuffer;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: copyStringToBuffer %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)columnIndex, jbuf);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: copyStringToBuffer %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jbuf);

    // LOGGERD(TAG, "- CCursorNative::CopyStringToBuffer()");
    return NOERROR;
}

ECode CCursorNative::GetInt16(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Int16* columnValue)
{
    // LOGGERD(TAG, "+ CCursorNative::GetInt16()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "getShort", "(I)S");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: getShort %d", __LINE__);

    *columnValue = (Int16)env->CallShortMethod(mJInstance, m, (jint)columnIndex);
    Util::CheckErrorAndLog(env, TAG, "CallShortMethod: getShort %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CCursorNative::GetInt16()");
    return NOERROR;
}

ECode CCursorNative::GetFloat(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Float* columnValue)
{
    // LOGGERD(TAG, "+ CCursorNative::GetFloat()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "getFloat", "(I)F");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: getFloat %d", __LINE__);

    *columnValue = (Float)env->CallFloatMethod(mJInstance, m, (jint)columnIndex);
    Util::CheckErrorAndLog(env, TAG, "CallFloatMethod: getFloat %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CCursorNative::GetFloat()");
    return NOERROR;
}

ECode CCursorNative::GetDouble(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Double* columnValue)
{
    // LOGGERD(TAG, "+ CCursorNative::GetDouble()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "getDouble", "(I)D");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: getDouble %d", __LINE__);

    *columnValue = (Double)env->CallDoubleMethod(mJInstance, m, (jint)columnIndex);
    Util::CheckErrorAndLog(env, TAG, "CallDoubleMethod: getDouble %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CCursorNative::GetDouble()");
    return NOERROR;
}

ECode CCursorNative::GetType(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Int32* index)
{
    // LOGGERD(TAG, "+ CCursorNative::GetType()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "getType", "(I)I");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: getType %d", __LINE__);

    *index = (Int32)env->CallIntMethod(mJInstance, m, (jint)columnIndex);
    Util::CheckErrorAndLog(env, TAG, "CallIntMethod: getType %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CCursorNative::GetType()");
    return NOERROR;
}

ECode CCursorNative::Deactivate()
{
    // LOGGERD(TAG, "+ CCursorNative::Deactivate()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "deactivate", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: deactivate %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: deactivate %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CCursorNative::Deactivate()");
    return NOERROR;
}

ECode CCursorNative::RegisterDataSetObserver(
    /* [in] */ IDataSetObserver* observer)
{
    // LOGGERD(TAG, "+ CCursorNative::RegisterDataSetObserver()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jobserver = NULL;
    if (observer != NULL) {
        jclass c = env->FindClass("android/database/ElDataSetObserverProxy");
        Util::CheckErrorAndLog(env, TAG, "FindClass: ElDataSetObserverProxy %d", __LINE__);

        jmethodID m = env->GetMethodID(c, "<init>", "(J)V");
        Util::CheckErrorAndLog(env, TAG, "GetMethodID: ElDataSetObserverProxy %d", __LINE__);

        jobserver = env->NewObject(c, m, (jlong)observer);
        Util::CheckErrorAndLog(env, TAG, "NewObject: ElDataSetObserverProxy %d", __LINE__);
        observer->AddRef(); // add ref for jobserver

        jobject gobserver = env->NewGlobalRef(jobserver);
        mDataSetObserverMap[observer] = gobserver;
        observer->AddRef(); // add ref for mDataSetObserverMap

        env->DeleteLocalRef(c);
    }

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "registerDataSetObserver", "(Landroid/database/DataSetObserver;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: registerDataSetObserver %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jobserver);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: registerDataSetObserver %d", __LINE__);

    // LOGGERD(TAG, "- CCursorNative::RegisterDataSetObserver()");
    return NOERROR;
}

ECode CCursorNative::UnregisterDataSetObserver(
    /* [in] */ IDataSetObserver* observer)
{
    // LOGGERD(TAG, "+ CCursorNative::UnregisterDataSetObserver()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jobserver = NULL;
    if (observer != NULL) {
        if (mDataSetObserverMap.Find(observer) != mDataSetObserverMap.End()) {
            jobserver = mDataSetObserverMap[observer];
            mDataSetObserverMap.Erase(observer);
            observer->Release();
        }
        else {
            LOGGERE(TAG, String("UnregisterDataSetObserver() cannot find observer!"));
        }
    }

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "unregisterDataSetObserver", "(Landroid/database/DataSetObserver;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: unregisterDataSetObserver %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jobserver);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: unregisterDataSetObserver %d", __LINE__);

    env->DeleteLocalRef(c);
    if(jobserver){
        env->DeleteGlobalRef(jobserver);
    }

    // LOGGERD(TAG, "- CCursorNative::UnregisterDataSetObserver()");
    return NOERROR;
}

ECode CCursorNative::SetNotificationUri(
    /* [in] */ IContentResolver* cr,
    /* [in] */ IUri* uri)
{
    // LOGGERD(TAG, "+ CCursorNative::SetNotificationUri()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jcr = NULL;
    if (cr != NULL) {
        jclass c = env->FindClass("android/content/ElContentResolverProxy");
        Util::CheckErrorAndLog(env, TAG, "FindClass: ElContentResolverProxy %d", __LINE__);

        jmethodID m = env->GetMethodID(c, "<init>", "(J)V");
        Util::CheckErrorAndLog(env, TAG, "GetMethodID: ElContentResolverProxy %d", __LINE__);

        jcr = env->NewObject(c, m, (jlong)cr);
        Util::CheckErrorAndLog(env, TAG, "NewObject: ElContentResolverProxy %d", __LINE__);
        cr->AddRef();

        env->DeleteLocalRef(c);
    }

    jobject juri = NULL;
    if (uri != NULL) {
        juri = Util::ToJavaUri(env, uri);
    }

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "setNotificationUri", "(Landroid/content/ContentResolver;Landroid/net/Uri;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: setNotificationUri %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jcr, juri);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: setNotificationUri %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jcr);
    env->DeleteLocalRef(juri);

    // LOGGERD(TAG, "- CCursorNative::SetNotificationUri()");
    return NOERROR;
}

ECode CCursorNative::GetNotificationUri(
    /* [out] */ IUri** uri)
{
    // LOGGERD(TAG, "+ CCursorNative::GetNotificationUri()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "getNotificationUri", "(V)Landroid/net/Uri;");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: getNotificationUri %d", __LINE__);

    jobject juri = env->CallObjectMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallObjectMethod: getNotificationUri %d", __LINE__);

    *uri = NULL;
    if (juri != NULL) {
        if (!Util::GetElUri(env, juri, uri)) {
            LOGGERE(TAG, "GetNotificationUri() GetElUri fail!");
        }
        env->DeleteLocalRef(juri);
    }

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CCursorNative::GetNotificationUri()");
    return NOERROR;
}

ECode CCursorNative::GetWantsAllOnMoveCalls(
    /* [out] */ Boolean* result)
{
    // LOGGERD(TAG, "+ CCursorNative::GetWantsAllOnMoveCalls()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "getWantsAllOnMoveCalls", "()Z");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: getWantsAllOnMoveCalls %d", __LINE__);

    *result = (Boolean)env->CallBooleanMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallBooleanMethod: getWantsAllOnMoveCalls %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CCursorNative::GetWantsAllOnMoveCalls()");
    return NOERROR;
}

ECode CCursorNative::GetExtras(
    /* [out] */ IBundle** extras)
{
    // LOGGERD(TAG, "+ CCursorNative::GetExtras()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "getExtras", "()Landroid/os/Bundle;");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: getExtras %d", __LINE__);

    jobject jextras = env->CallObjectMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallObjectMethod: getExtras %d", __LINE__);

    *extras = NULL;
    if (jextras != NULL) {
        if (!Util::GetElBundle(env, jextras, extras)) {
            LOGGERE(TAG, "GetExtras() GetElBundle fail!");
        }
        env->DeleteLocalRef(jextras);
    }

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CCursorNative::GetExtras()");
    return NOERROR;
}

ECode CCursorNative::Respond(
    /* [in] */ IBundle* extras,
    /* [out] */ IBundle** result)
{
    // LOGGERD(TAG, "+ CCursorNative::Respond()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jextras = NULL;
    if (extras != NULL) {
        jextras = Util::ToJavaBundle(env, extras);
    }

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "respond", "(Landroid/os/Bundle;)Landroid/os/Bundle;");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: respond %d", __LINE__);

    jobject jresult = env->CallObjectMethod(mJInstance, m, jextras);
    Util::CheckErrorAndLog(env, TAG, "CallObjectMethod: respond %d", __LINE__);

    *result = NULL;
    if (jresult != NULL) {
        if (!Util::GetElBundle(env, jresult, result)) {
            LOGGERE(TAG, "Respond() GetElBundle fail!");
        }
        env->DeleteLocalRef(jresult);
    }

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jextras);

    // LOGGERD(TAG, "- CCursorNative::Respond()");
    return NOERROR;
}

ECode CCursorNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CCursorNative::ToString()");
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

    // LOGGERD(TAG, "- CCursorNative::ToString()");
    return NOERROR;
}

}
}
}

