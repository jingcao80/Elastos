
#include "CCursorNative.h"
#include <elastos/utility/logging/Logger.h>
#include "Util.h"

using Elastos::Droid::JavaProxy::Util;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CCursorNative::TAG("CCursorNative");

HashMap<IContentObserver*, jobject> CCursorNative::mContentObserverMap;
HashMap<IDataSetObserver*, jobject> CCursorNative::mDataSetObserverMap;

CCursorNative::~CCursorNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CCursorNative::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CCursorNative::GetCount(
    /* [out] */ Int32* count)
{
    // LOGGERD(TAG, String("+ CCursorNative::GetCount()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "getCount", "()I");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: getCount %d", __LINE__);

    *count = env->CallIntMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallIntMethod: getCount %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CCursorNative::GetCount()"));
    return NOERROR;
}

ECode CCursorNative::GetColumnIndexOrThrow(
    /* [in] */ const String& columnName,
    /* [out] */ Int32* columnIndex)
{
    // LOGGERD(TAG, String("+ CCursorNative::GetColumnIndexOrThrow()"));

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

    // LOGGERD(TAG, String("- CCursorNative::GetColumnIndexOrThrow()"));
    return ec;
}

ECode CCursorNative::GetColumnIndex(
    /* [in] */ const String& columnName,
    /* [out] */ Int32* columnIndex)
{
    // LOGGERD(TAG, String("+ CCursorNative::GetColumnIndex()"));

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

    // LOGGERD(TAG, String("- CCursorNative::GetColumnIndex()"));
    return NOERROR;
}

ECode CCursorNative::MoveToPosition(
    /* [in] */ Int32 position,
    /* [out] */ Boolean* succeeded)
{
    // LOGGERD(TAG, String("+ CCursorNative::MoveToPosition()"));

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

    // LOGGERD(TAG, String("- CCursorNative::MoveToPosition()"));
    return NOERROR;
}

ECode CCursorNative::GetInt64(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Int64* columnValue)
{
    // LOGGERD(TAG, String("+ CCursorNative::GetInt64()"));

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

    // LOGGERD(TAG, String("- CCursorNative::GetInt64()"));
    return NOERROR;
}

ECode CCursorNative::IsClosed(
    /* [out] */ Boolean* closed)
{
    // LOGGERD(TAG, String("+ CCursorNative::IsClosed()"));

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

    // LOGGERD(TAG, String("- CCursorNative::IsClosed()"));
    return NOERROR;
}

ECode CCursorNative::Close()
{
    // LOGGERD(TAG, String("+ CCursorNative::Close()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "close", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: Close %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: Close %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CCursorNative::Close()"));
    return NOERROR;
}

ECode CCursorNative::Move(
    /* [in] */ Int32 offset,
    /* [out] */ Boolean* succeeded)
{
    // LOGGERD(TAG, String("+ CCursorNative::Move()"));

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

    // LOGGERD(TAG, String("- CCursorNative::Move()"));
    return NOERROR;
}

ECode CCursorNative::GetPosition(
    /* [out] */ Int32* position)
{
    // LOGGERD(TAG, String("+ CCursorNative::GetPosition()"));

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

    // LOGGERD(TAG, String("- CCursorNative::GetPosition()"));
    return NOERROR;
}

ECode CCursorNative::MoveToFirst(
    /* [out] */ Boolean* succeeded)
{
    // LOGGERD(TAG, String("+ CCursorNative::MoveToFirst()"));

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

    // LOGGERD(TAG, String("- CCursorNative::MoveToFirst()"));
    return NOERROR;
}

ECode CCursorNative::MoveToLast(
    /* [out] */ Boolean* succeeded)
{
    // LOGGERD(TAG, String("+ CCursorNative::MoveToLast()"));

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

    // LOGGERD(TAG, String("- CCursorNative::MoveToFirst()"));
    return NOERROR;
}

ECode CCursorNative::MoveToNext(
    /* [out] */ Boolean* succeeded)
{
    // LOGGERD(TAG, String("+ CCursorNative::MoveToNext()"));

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

    // LOGGERD(TAG, String("- CCursorNative::MoveToNext()"));
    return NOERROR;
}

ECode CCursorNative::MoveToPrevious(
    /* [out] */ Boolean* succeeded)
{
    // LOGGERD(TAG, String("+ CCursorNative::MoveToPrevious()"));

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

    // LOGGERD(TAG, String("- CCursorNative::MoveToFirst()"));
    return NOERROR;
}

ECode CCursorNative::IsFirst(
    /* [out] */ Boolean* isFirst)
{
    // LOGGERD(TAG, String("+ CCursorNative::IsFirst()"));

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

    // LOGGERD(TAG, String("- CCursorNative::IsFirst()"));
    return NOERROR;
}

ECode CCursorNative::IsLast(
    /* [out] */ Boolean* isLast)
{
    // LOGGERD(TAG, String("+ CCursorNative::IsLast()"));

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

    // LOGGERD(TAG, String("- CCursorNative::IsLast()"));
    return NOERROR;
}

ECode CCursorNative::IsBeforeFirst(
    /* [out] */ Boolean* isBeforeFirst)
{
    // LOGGERD(TAG, String("+ CCursorNative::IsBeforeFirst()"));

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

    // LOGGERD(TAG, String("- CCursorNative::IsBeforeFirst()"));
    return NOERROR;
}

ECode CCursorNative::IsAfterLast(
    /* [out] */ Boolean* isAfterLast)
{
    // LOGGERD(TAG, String("+ CCursorNative::IsAfterLast()"));

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

    // LOGGERD(TAG, String("- CCursorNative::IsAfterLast()"));
    return NOERROR;
}

ECode CCursorNative::GetColumnName(
    /* [in] */ Int32 columnIndex,
    /* [out] */ String* columnName)
{
    // LOGGERD(TAG, String("+ CCursorNative::GetColumnName()"));

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

    // LOGGERD(TAG, String("- CCursorNative::GetColumnName()"));
    return NOERROR;
}

ECode CCursorNative::GetColumnNames(
    /* [out, callee] */ ArrayOf<String>** columnNames)
{
    // LOGGERD(TAG, String("+ CCursorNative::GetColumnNames()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "getColumnNames", "()[Ljava/lang/String;");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: GetColumnNames %d", __LINE__);

    jobjectArray jcolumnNames = (jobjectArray)env->CallObjectMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallObjectMethod: GetColumnNames %d", __LINE__);

    *columnNames = NULL;
    if (jcolumnNames != NULL) {
        jint jcount = env->GetArrayLength(jcolumnNames);
        if (jcount > 0) {
            *columnNames = ArrayOf<String>::Alloc((Int32)jcount);
            if (*columnNames != NULL) {
                for (Int32 i = 0; i < jcount; i++) {
                    jstring jcolumnName = (jstring)env->GetObjectArrayElement(jcolumnNames, i);
                    Util::CheckErrorAndLog(env, "GetColumnNames", "Fail GetObjectArrayElement: jcolumnNames %d", __LINE__);
                    String columnName = Util::GetElString(env, jcolumnName);
                    (*columnNames)->Set(i, columnName);
                    env->DeleteLocalRef(jcolumnName);
                }
            }
        }
        env->DeleteLocalRef(jcolumnNames);
    }

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CCursorNative::GetColumnName()"));
    return NOERROR;
}

ECode CCursorNative::GetColumnCount(
    /* [out] */ Int32* columnCount)
{
    // LOGGERD(TAG, String("+ CCursorNative::GetColumnCount()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "getColumnCount", "()I");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: GetColumnCount %d", __LINE__);

    *columnCount = env->CallIntMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallIntMethod: GetColumnCount %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CCursorNative::GetColumnCount()"));
    return NOERROR;
}

ECode CCursorNative::GetBlob(
    /* [in] */ Int32 columnIndex,
    /* [out, callee] */ ArrayOf<Byte>** blob)
{
    // LOGGERD(TAG, String("+ CCursorNative::GetBlob()"));

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
            *blob = ArrayOf<Byte>::Alloc((Byte*)jblob, (Int32)jcount);
        }
        env->DeleteLocalRef(jblobs);
    }

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CCursorNative::GetBlob()"));
    return NOERROR;
}

ECode CCursorNative::GetString(
    /* [in] */ Int32 columnIndex,
    /* [out] */ String* columnValue)
{
    // LOGGERD(TAG, String("+ CCursorNative::GetString()"));

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

    // LOGGERD(TAG, String("- CCursorNative::GetString()"));
    return NOERROR;
}

ECode CCursorNative::GetInt32(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Int32* columnValue)
{
    // LOGGERD(TAG, String("+ CCursorNative::GetInt32()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "getInt", "(I)I");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: GetInt32 %d", __LINE__);

    *columnValue = env->CallIntMethod(mJInstance, m, (jint)columnIndex);
    Util::CheckErrorAndLog(env, TAG, "CallIntMethod: GetInt32 %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CCursorNative::GetInt32()"));
    return NOERROR;
}

ECode CCursorNative::RegisterContentObserver(
    /* [in] */ IContentObserver* observer)
{
    // LOGGERD(TAG, String("+ CCursorNative::RegisterContentObserver()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jobserver = NULL;
    if (observer != NULL) {
        jclass c = env->FindClass("android/database/ElContentObserverProxy");
        Util::CheckErrorAndLog(env, TAG, "FindClass: ElContentObserverProxy %d", __LINE__);

        jmethodID m = env->GetMethodID(c, "<init>", "(I)V");
        Util::CheckErrorAndLog(env, TAG, "GetMethodID: ElContentObserverProxy %d", __LINE__);

        jobserver = env->NewObject(c, m, (jint)observer);
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

    // LOGGERD(TAG, String("- CCursorNative::RegisterContentObserver()"));
    return NOERROR;
}

ECode CCursorNative::UnregisterContentObserver(
    /* [in] */ IContentObserver* observer)
{
    // LOGGERD(TAG, String("+ CCursorNative::UnregisterContentObserver()"));

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

    // LOGGERD(TAG, String("- CCursorNative::UnregisterContentObserver()"));
    return NOERROR;
}

ECode CCursorNative::Requery(
    /* [out] */ Boolean* succeeded)
{
    // LOGGERD(TAG, String("+ CCursorNative::Requery()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "requery", "()Z");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: requery %d", __LINE__);

    *succeeded = (Boolean)env->CallBooleanMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallBooleanMethod: requery %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CCursorNative::Requery()"));
    return NOERROR;
}

ECode CCursorNative::IsNull(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Boolean* isNull)
{
    // LOGGERD(TAG, String("+ CCursorNative::IsNull()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "isNull", "(I)Z");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: isNull %d", __LINE__);

    *isNull = (Boolean)env->CallBooleanMethod(mJInstance, m, (jint)columnIndex);
    Util::CheckErrorAndLog(env, TAG, "CallBooleanMethod: isNull %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CCursorNative::IsNull()"));
    return NOERROR;
}

ECode CCursorNative::CopyStringToBuffer(
    /* [in] */ Int32 columnIndex,
    /* [in] */ ICharArrayBuffer* buf)
{
    LOGGERD(TAG, String("+ CCursorNative::CopyStringToBuffer()"));

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

    LOGGERD(TAG, String("- CCursorNative::CopyStringToBuffer()"));
    return NOERROR;
}

ECode CCursorNative::GetInt16(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Int16* columnValue)
{
    LOGGERD(TAG, String("+ CCursorNative::GetInt16()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "getShort", "(I)S");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: getShort %d", __LINE__);

    *columnValue = (Int16)env->CallShortMethod(mJInstance, m, (jint)columnIndex);
    Util::CheckErrorAndLog(env, TAG, "CallShortMethod: getShort %d", __LINE__);

    env->DeleteLocalRef(c);

    LOGGERD(TAG, String("- CCursorNative::GetInt16()"));
    return NOERROR;
}

ECode CCursorNative::GetFloat(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Float* columnValue)
{
    LOGGERD(TAG, String("+ CCursorNative::GetFloat()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "getFloat", "(I)F");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: getFloat %d", __LINE__);

    *columnValue = (Float)env->CallFloatMethod(mJInstance, m, (jint)columnIndex);
    Util::CheckErrorAndLog(env, TAG, "CallFloatMethod: getFloat %d", __LINE__);

    env->DeleteLocalRef(c);

    LOGGERD(TAG, String("- CCursorNative::GetFloat()"));
    return NOERROR;
}

ECode CCursorNative::GetDouble(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Double* columnValue)
{
    // LOGGERD(TAG, String("+ CCursorNative::GetDouble()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "getDouble", "(I)D");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: getDouble %d", __LINE__);

    *columnValue = (Double)env->CallDoubleMethod(mJInstance, m, (jint)columnIndex);
    Util::CheckErrorAndLog(env, TAG, "CallDoubleMethod: getDouble %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CCursorNative::GetDouble()"));
    return NOERROR;
}

ECode CCursorNative::GetType(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Int32* index)
{
    LOGGERD(TAG, String("+ CCursorNative::GetType()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "getType", "(I)I");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: getType %d", __LINE__);

    *index = (Int32)env->CallIntMethod(mJInstance, m, (jint)columnIndex);
    Util::CheckErrorAndLog(env, TAG, "CallIntMethod: getType %d", __LINE__);

    env->DeleteLocalRef(c);

    LOGGERD(TAG, String("- CCursorNative::GetType()"));
    return NOERROR;
}

ECode CCursorNative::Deactivate()
{
    LOGGERD(TAG, String("+ CCursorNative::Deactivate()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "deactivate", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: deactivate %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: deactivate %d", __LINE__);

    env->DeleteLocalRef(c);

    LOGGERD(TAG, String("- CCursorNative::Deactivate()"));
    return NOERROR;
}

ECode CCursorNative::RegisterDataSetObserver(
    /* [in] */ IDataSetObserver* observer)
{
    LOGGERD(TAG, String("+ CCursorNative::RegisterDataSetObserver()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jobserver = NULL;
    if (observer != NULL) {
        jclass c = env->FindClass("android/database/ElDataSetObserverProxy");
        Util::CheckErrorAndLog(env, TAG, "FindClass: ElDataSetObserverProxy %d", __LINE__);

        jmethodID m = env->GetMethodID(c, "<init>", "(I)V");
        Util::CheckErrorAndLog(env, TAG, "GetMethodID: ElDataSetObserverProxy %d", __LINE__);

        jobserver = env->NewObject(c, m, (jint)observer);
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

    LOGGERD(TAG, String("- CCursorNative::RegisterDataSetObserver()"));
    return NOERROR;
}

ECode CCursorNative::UnregisterDataSetObserver(
    /* [in] */ IDataSetObserver* observer)
{
    LOGGERD(TAG, String("+ CCursorNative::UnregisterDataSetObserver()"));

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

    LOGGERD(TAG, String("- CCursorNative::UnregisterDataSetObserver()"));
    return NOERROR;
}

ECode CCursorNative::SetNotificationUri(
    /* [in] */ IContentResolver* cr,
    /* [in] */ IUri* uri)
{
    LOGGERD(TAG, String("+ CCursorNative::SetNotificationUri()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jcr = NULL;
    if (cr != NULL) {
        jclass c = env->FindClass("android/content/ElContentResolverProxy");
        Util::CheckErrorAndLog(env, TAG, "FindClass: ElContentResolverProxy %d", __LINE__);

        jmethodID m = env->GetMethodID(c, "<init>", "(I)V");
        Util::CheckErrorAndLog(env, TAG, "GetMethodID: ElContentResolverProxy %d", __LINE__);

        jcr = env->NewObject(c, m, (jint)cr);
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

    LOGGERD(TAG, String("- CCursorNative::SetNotificationUri()"));
    return NOERROR;
}

ECode CCursorNative::GetWantsAllOnMoveCalls(
    /* [out] */ Boolean* result)
{
    LOGGERD(TAG, String("+ CCursorNative::GetWantsAllOnMoveCalls()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/database/Cursor");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Cursor %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "getWantsAllOnMoveCalls", "()Z");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: getWantsAllOnMoveCalls %d", __LINE__);

    *result = (Boolean)env->CallBooleanMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallBooleanMethod: getWantsAllOnMoveCalls %d", __LINE__);

    env->DeleteLocalRef(c);

    LOGGERD(TAG, String("- CCursorNative::GetWantsAllOnMoveCalls()"));
    return NOERROR;
}

ECode CCursorNative::GetExtras(
    /* [out] */ IBundle** extras)
{
    LOGGERD(TAG, String("+ CCursorNative::GetExtras()"));

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

    LOGGERD(TAG, String("- CCursorNative::GetExtras()"));
    return NOERROR;
}

ECode CCursorNative::Respond(
    /* [in] */ IBundle* extras,
    /* [out] */ IBundle** result)
{
    LOGGERD(TAG, String("+ CCursorNative::Respond()"));

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

    LOGGERD(TAG, String("- CCursorNative::Respond()"));
    return NOERROR;
}

}
}
}

