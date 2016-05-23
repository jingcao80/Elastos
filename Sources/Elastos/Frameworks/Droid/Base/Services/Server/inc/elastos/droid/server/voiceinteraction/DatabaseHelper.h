/**
 * Copyright (C) 2014 The Android Open Source Project
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

package com.android.server.voiceinteraction;

using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Database::Sqlite::ISQLiteDatabase;
using Elastos::Droid::Database::Sqlite::ISQLiteOpenHelper;
using Elastos::Droid::Hardware::soundtrigger::ISoundTrigger;
using Elastos::Droid::Hardware::soundtrigger::ISoundTrigger.Keyphrase;
using Elastos::Droid::Hardware::soundtrigger::ISoundTrigger.KeyphraseSoundModel;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::ISlog;

using Elastos::Utility::ILocale;
using Elastos::Utility::IUUID;

/**
 * Helper to manage the database of the sound models that have been registered on the device.
 *
 * @hide
 */
public class DatabaseHelper extends SQLiteOpenHelper {
    static const String TAG = "SoundModelDBHelper";
    static const Boolean DBG = FALSE;

    private static const String NAME = "sound_model.db";
    private static const Int32 VERSION = 4;

    public static interface SoundModelContract {
        public static const String TABLE = "sound_model";
        public static const String KEY_MODEL_UUID = "model_uuid";
        public static const String KEY_KEYPHRASE_ID = "keyphrase_id";
        public static const String KEY_TYPE = "type";
        public static const String KEY_DATA = "data";
        public static const String KEY_RECOGNITION_MODES = "recognition_modes";
        public static const String KEY_LOCALE = "locale";
        public static const String KEY_HINT_TEXT = "hint_text";
        public static const String KEY_USERS = "users";
    }

    // Table Create Statement
    private static const String CREATE_TABLE_SOUND_MODEL = "CREATE TABLE "
            + SoundModelContract.TABLE + "("
            + SoundModelContract.KEY_MODEL_UUID + " TEXT PRIMARY KEY,"
            + SoundModelContract.KEY_KEYPHRASE_ID + " INTEGER,"
            + SoundModelContract.KEY_TYPE + " INTEGER,"
            + SoundModelContract.KEY_DATA + " BLOB,"
            + SoundModelContract.KEY_RECOGNITION_MODES + " INTEGER,"
            + SoundModelContract.KEY_LOCALE + " TEXT,"
            + SoundModelContract.KEY_HINT_TEXT + " TEXT,"
            + SoundModelContract.KEY_USERS + " TEXT" + ")";

    public DatabaseHelper(Context context) {
        Super(context, NAME, NULL, VERSION);
    }

    //@Override
    CARAPI OnCreate(SQLiteDatabase db) {
        // creating required tables
        db->ExecSQL(CREATE_TABLE_SOUND_MODEL);
    }

    //@Override
    CARAPI OnUpgrade(SQLiteDatabase db, Int32 oldVersion, Int32 newVersion) {
        // TODO: For now, drop older tables and recreate new ones.
        db->ExecSQL("DROP TABLE IF EXISTS " + SoundModelContract.TABLE);
        OnCreate(db);
    }

    /**
     * Updates the given keyphrase model, adds it, if it doesn't already exist.
     *
     * TODO: We only support one keyphrase currently.
     */
    public Boolean UpdateKeyphraseSoundModel(KeyphraseSoundModel soundModel) {
        {    AutoLock syncLock(this);
            SQLiteDatabase db = GetWritableDatabase();
            ContentValues values = new ContentValues();
            values->Put(SoundModelContract.KEY_MODEL_UUID, soundModel.uuid->ToString());
            values->Put(SoundModelContract.KEY_TYPE, SoundTrigger.SoundModel.TYPE_KEYPHRASE);
            values->Put(SoundModelContract.KEY_DATA, soundModel.data);

            if (soundModel.keyphrases != NULL && soundModel.keyphrases.length == 1) {
                values->Put(SoundModelContract.KEY_KEYPHRASE_ID, soundModel.keyphrases[0].id);
                values->Put(SoundModelContract.KEY_RECOGNITION_MODES,
                        soundModel.keyphrases[0].recognitionModes);
                values->Put(SoundModelContract.KEY_USERS,
                        GetCommaSeparatedString(soundModel.keyphrases[0].users));
                values->Put(SoundModelContract.KEY_LOCALE, soundModel.keyphrases[0].locale);
                values->Put(SoundModelContract.KEY_HINT_TEXT, soundModel.keyphrases[0].text);
                try {
                    return db->InsertWithOnConflict(SoundModelContract.TABLE, NULL, values,
                            SQLiteDatabase.CONFLICT_REPLACE) != -1;
                } finally {
                    db->Close();
                }
            }
            return FALSE;
        }
    }

    /**
     * Deletes the sound model and associated keyphrases.
     */
    public Boolean DeleteKeyphraseSoundModel(Int32 keyphraseId, Int32 userHandle, String bcp47Locale) {
        // Sanitize the locale to guard against SQL injection.
        bcp47Locale = Locale->ForLanguageTag(bcp47Locale).ToLanguageTag();
        {    AutoLock syncLock(this);
            KeyphraseSoundModel soundModel = GetKeyphraseSoundModel(keyphraseId, userHandle,
                    bcp47Locale);
            if (soundModel == NULL) {
                return FALSE;
            }

            // Delete all sound models for the given keyphrase and specified user.
            SQLiteDatabase db = GetWritableDatabase();
            String soundModelClause = SoundModelContract.KEY_MODEL_UUID
                    + "='" + soundModel.uuid->ToString() + "'";
            try {
                return db->Delete(SoundModelContract.TABLE, soundModelClause, NULL) != 0;
            } finally {
                db->Close();
            }
        }
    }

    /**
     * Returns a matching {@link KeyphraseSoundModel} for the keyphrase ID.
     * Returns NULL if a match isn't found.
     *
     * TODO: We only support one keyphrase currently.
     */
    public KeyphraseSoundModel GetKeyphraseSoundModel(Int32 keyphraseId, Int32 userHandle,
            String bcp47Locale) {
        // Sanitize the locale to guard against SQL injection.
        bcp47Locale = Locale->ForLanguageTag(bcp47Locale).ToLanguageTag();
        {    AutoLock syncLock(this);
            // Find the corresponding sound model ID for the keyphrase.
            String selectQuery = "SELECT  * FROM " + SoundModelContract.TABLE
                    + " WHERE " + SoundModelContract.KEY_KEYPHRASE_ID + "= '" + keyphraseId
                    + "' AND " + SoundModelContract.KEY_LOCALE + "='" + bcp47Locale + "'";
            SQLiteDatabase db = GetReadableDatabase();
            Cursor c = db->RawQuery(selectQuery, NULL);

            try {
                if (c->MoveToFirst()) {
                    do {
                        Int32 type = c->GetInt(c->GetColumnIndex(SoundModelContract.KEY_TYPE));
                        if (type != SoundTrigger.SoundModel.TYPE_KEYPHRASE) {
                            if (DBG) {
                                Slogger::W(TAG, "Ignoring SoundModel since it's type is incorrect");
                            }
                            continue;
                        }

                        String modelUuid = c->GetString(
                                c->GetColumnIndex(SoundModelContract.KEY_MODEL_UUID));
                        if (modelUuid == NULL) {
                            Slogger::W(TAG, "Ignoring SoundModel since it doesn't specify an ID");
                            continue;
                        }

                        Byte[] data = c->GetBlob(c->GetColumnIndex(SoundModelContract.KEY_DATA));
                        Int32 recognitionModes = c->GetInt(
                                c->GetColumnIndex(SoundModelContract.KEY_RECOGNITION_MODES));
                        Int32[] users = GetArrayForCommaSeparatedString(
                                c->GetString(c->GetColumnIndex(SoundModelContract.KEY_USERS)));
                        String modelLocale = c->GetString(
                                c->GetColumnIndex(SoundModelContract.KEY_LOCALE));
                        String text = c->GetString(
                                c->GetColumnIndex(SoundModelContract.KEY_HINT_TEXT));

                        // Only add keyphrases meant for the current user.
                        if (users == NULL) {
                            // No users present in the keyphrase.
                            Slogger::W(TAG, "Ignoring SoundModel since it doesn't specify users");
                            continue;
                        }

                        Boolean isAvailableForCurrentUser = FALSE;
                        for (Int32 user : users) {
                            if (userHandle == user) {
                                isAvailableForCurrentUser = TRUE;
                                break;
                            }
                        }
                        if (!isAvailableForCurrentUser) {
                            if (DBG) {
                                Slogger::W(TAG, "Ignoring SoundModel since user handles don't match");
                            }
                            continue;
                        } else {
                            if (DBG) Slogger::D(TAG, "Found a SoundModel for user: " + userHandle);
                        }

                        Keyphrase[] keyphrases = new Keyphrase[1];
                        keyphrases[0] = new Keyphrase(
                                keyphraseId, recognitionModes, modelLocale, text, users);
                        KeyphraseSoundModel model = new KeyphraseSoundModel(
                                UUID->FromString(modelUuid),
                                NULL /* FIXME use vendor UUID */, data, keyphrases);
                        if (DBG) {
                            Slogger::D(TAG, "Found SoundModel for the given keyphrase/locale/user: "
                                    + model);
                        }
                        return model;
                    } while (c->MoveToNext());
                }
                Slogger::W(TAG, "No SoundModel available for the given keyphrase");
            } finally {
                c->Close();
                db->Close();
            }
            return NULL;
        }
    }

    private static String GetCommaSeparatedString(Int32[] users) {
        if (users == NULL) {
            return "";
        }
        StringBuilder sb = new StringBuilder();
        for (Int32 i = 0; i < users.length; i++) {
            if (i != 0) {
                sb->AppendChar32(',');
            }
            sb->Append(users[i]);
        }
        return sb->ToString();
    }

    private static Int32[] GetArrayForCommaSeparatedString(String text) {
        if (TextUtils->IsEmpty(text)) {
            return NULL;
        }
        String[] usersStr = text->Split(",");
        Int32[] users = new Int32[usersStr.length];
        for (Int32 i = 0; i < usersStr.length; i++) {
            users[i] = Integer->ValueOf(usersStr[i]);
        }
        return users;
    }
}
