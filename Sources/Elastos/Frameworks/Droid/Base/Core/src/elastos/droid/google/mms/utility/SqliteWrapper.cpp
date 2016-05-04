/*
 * Copyright (C) 2008 Esmertec AG.
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.google.android.mms.util;

using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Widget::IToast;

public class SqliteWrapper {
    private static const String TAG = "SqliteWrapper";
    private static const String SQLITE_EXCEPTION_DETAIL_MESSAGE
                = "unable to open database file";

    private SqliteWrapper() {
        // Forbidden being instantiated.
    }

    // FIXME: It looks like outInfo.lowMemory does not work well as we expected.
    // after run command: adb shell fillup -p 100, outInfo.lowMemory is still FALSE.
    private static Boolean IsLowMemory(Context context) {
        If (NULL == context) {
            return FALSE;
        }

        ActivityManager am = (ActivityManager)
                        context->GetSystemService(Context.ACTIVITY_SERVICE);
        ActivityManager.MemoryInfo outInfo = new ActivityManager->MemoryInfo();
        am->GetMemoryInfo(outInfo);

        return outInfo.lowMemory;
    }

    // FIXME: need to optimize this method.
    private static Boolean IsLowMemory(SQLiteException e) {
        return e->GetMessage()->Equals(SQLITE_EXCEPTION_DETAIL_MESSAGE);
    }

    public static void CheckSQLiteException(Context context, SQLiteException e) {
        If (IsLowMemory(e)) {
            Toast->MakeText(context, R::string::low_memory,
                    Toast.LENGTH_SHORT).Show();
        } else {
            throw e;
        }
    }

    public static Cursor Query(Context context, ContentResolver resolver, Uri uri,
            String[] projection, String selection, String[] selectionArgs, String sortOrder) {
        try {
            return resolver->Query(uri, projection, selection, selectionArgs, sortOrder);
        } Catch (SQLiteException e) {
            Logger::E(TAG, "Catch a SQLiteException when query: ", e);
            CheckSQLiteException(context, e);
            return NULL;
        }
    }

    public static Boolean Requery(Context context, Cursor cursor) {
        try {
            return cursor->Requery();
        } Catch (SQLiteException e) {
            Logger::E(TAG, "Catch a SQLiteException when requery: ", e);
            CheckSQLiteException(context, e);
            return FALSE;
        }
    }
    public static Int32 Update(Context context, ContentResolver resolver, Uri uri,
            ContentValues values, String where, String[] selectionArgs) {
        try {
            return resolver->Update(uri, values, where, selectionArgs);
        } Catch (SQLiteException e) {
            Logger::E(TAG, "Catch a SQLiteException when update: ", e);
            CheckSQLiteException(context, e);
            return -1;
        }
    }

    public static Int32 Delete(Context context, ContentResolver resolver, Uri uri,
            String where, String[] selectionArgs) {
        try {
            return resolver->Delete(uri, where, selectionArgs);
        } Catch (SQLiteException e) {
            Logger::E(TAG, "Catch a SQLiteException when delete: ", e);
            CheckSQLiteException(context, e);
            return -1;
        }
    }

    public static Uri Insert(Context context, ContentResolver resolver,
            Uri uri, ContentValues values) {
        try {
            return resolver->Insert(uri, values);
        } Catch (SQLiteException e) {
            Logger::E(TAG, "Catch a SQLiteException when insert: ", e);
            CheckSQLiteException(context, e);
            return NULL;
        }
    }
}
