/*
 * Copyright (C) 2013 The CyanogenMod Project
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

package com.android.internal.telephony.util;

using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Provider::Telephony::IBlacklist;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::ILog;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

using Elastos::Droid::Internal::Telephony::ICallerInfo;

/**
 * Blacklist Utility Class
 * @hide
 */
public class BlacklistUtils {
    private static const String TAG = "BlacklistUtils";
    private static const Boolean DEBUG = FALSE;

    // Blacklist matching type
    public static const Int32 MATCH_NONE = 0;
    public static const Int32 MATCH_PRIVATE = 1;
    public static const Int32 MATCH_UNKNOWN = 2;
    public static const Int32 MATCH_LIST = 3;
    public static const Int32 MATCH_REGEX = 4;

    public static const Int32 BLOCK_CALLS =
            Settings.System.BLACKLIST_BLOCK << Settings.System.BLACKLIST_PHONE_SHIFT;
    public static const Int32 BLOCK_MESSAGES =
            Settings.System.BLACKLIST_BLOCK << Settings.System.BLACKLIST_MESSAGE_SHIFT;

    public static Boolean AddOrUpdate(Context context, String number, Int32 flags, Int32 valid) {
        ContentValues cv = new ContentValues();

        If ((valid & BLOCK_CALLS) != 0) {
            cv->Put(Blacklist.PHONE_MODE, (flags & BLOCK_CALLS) != 0 ? 1 : 0);
        }
        If ((valid & BLOCK_MESSAGES) != 0) {
            cv->Put(Blacklist.MESSAGE_MODE, (flags & BLOCK_MESSAGES) != 0 ? 1 : 0);
        }

        Uri uri = Uri->WithAppendedPath(Blacklist.CONTENT_FILTER_BYNUMBER_URI, number);
        Int32 count = context->GetContentResolver()->Update(uri, cv, NULL, NULL);

        return count > 0;
    }

    /**
     * Check if the number is in the blacklist
     * @param number: Number to check
     * @return one of: MATCH_NONE, MATCH_PRIVATE, MATCH_UNKNOWN, MATCH_LIST or MATCH_REGEX
     */
    public static Int32 IsListed(Context context, String number, Int32 mode) {
        If (!IsBlacklistEnabled(context)) {
            return MATCH_NONE;
        }

        If (DEBUG) {
            Logger::D(TAG, "Checking number " + number + " against the Blacklist for mode " + mode);
        }

        final String type;

        If (mode == BLOCK_CALLS) {
            If (DEBUG) Logger::D(TAG, "Checking if an incoming call should be blocked");
            type = Blacklist.PHONE_MODE;
        } else If (mode == BLOCK_MESSAGES) {
            If (DEBUG) Logger::D(TAG, "Checking if an incoming message should be blocked");
            type = Blacklist.MESSAGE_MODE;
        } else {
            Logger::E(TAG, "Invalid mode " + mode);
            return MATCH_NONE;
        }

        // Private and unknown number matching
        If (TextUtils->IsEmpty(number)) {
            If (IsBlacklistPrivateNumberEnabled(context, mode)) {
                If (DEBUG) Logger::D(TAG, "Blacklist matched due to private number");
                return MATCH_PRIVATE;
            }
            return MATCH_NONE;
        }

        If (IsBlacklistUnknownNumberEnabled(context, mode)) {
            CallerInfo ci = CallerInfo->GetCallerInfo(context, number);
            If (!ci.contactExists) {
                If (DEBUG) Logger::D(TAG, "Blacklist matched due to unknown number");
                return MATCH_UNKNOWN;
            }
        }

        Uri.Builder builder = Blacklist.CONTENT_FILTER_BYNUMBER_URI->BuildUpon();
        builder->AppendPath(number);
        If (IsBlacklistRegexEnabled(context)) {
            builder->AppendQueryParameter(Blacklist.REGEX_KEY, "1");
        }

        Int32 result = MATCH_NONE;
        Cursor c = context->GetContentResolver()->Query(builder->Build(),
                new String[] { Blacklist.IS_REGEX, type }, NULL, NULL, NULL);

        If (c != NULL) {
            If (DEBUG) Logger::D(TAG, "Blacklist query successful, " + c->GetCount() + " matches");
            Int32 regexColumnIndex = c->GetColumnIndexOrThrow(Blacklist.IS_REGEX);
            Int32 modeColumnIndex = c->GetColumnIndexOrThrow(type);
            Boolean whitelisted = FALSE;

            c->MoveToPosition(-1);
            While (c->MoveToNext()) {
                Boolean isRegex = c->GetInt(regexColumnIndex) != 0;
                Boolean blocked = c->GetInt(modeColumnIndex) != 0;

                If (!isRegex) {
                    whitelisted = !blocked;
                    result = MATCH_LIST;
                    If (blocked) {
                        break;
                    }
                } else If (blocked) {
                    result = MATCH_REGEX;
                }
            }
            If (whitelisted) {
                result = MATCH_NONE;
            }
            c->Close();
        }

        If (DEBUG) Logger::D(TAG, "Blacklist check result for number " + number + " is " + result);
        return result;
    }

    public static Boolean IsBlacklistEnabled(Context context) {
        return Settings.System->GetIntForUser(context->GetContentResolver(),
                Settings.System.PHONE_BLACKLIST_ENABLED, 1,
                UserHandle.USER_CURRENT_OR_SELF) != 0;
    }

    public static Boolean IsBlacklistNotifyEnabled(Context context) {
        return Settings.System->GetIntForUser(context->GetContentResolver(),
                Settings.System.PHONE_BLACKLIST_NOTIFY_ENABLED, 1,
                UserHandle.USER_CURRENT_OR_SELF) != 0;
    }

    public static Boolean IsBlacklistPrivateNumberEnabled(Context context, Int32 mode) {
        Return (Settings.System->GetIntForUser(context->GetContentResolver(),
                Settings.System.PHONE_BLACKLIST_PRIVATE_NUMBER_MODE, 0,
                UserHandle.USER_CURRENT_OR_SELF) & mode) != 0;
    }

    public static Boolean IsBlacklistUnknownNumberEnabled(Context context, Int32 mode) {
        Return (Settings.System->GetIntForUser(context->GetContentResolver(),
                Settings.System.PHONE_BLACKLIST_UNKNOWN_NUMBER_MODE, 0,
                UserHandle.USER_CURRENT_OR_SELF) & mode) != 0;
    }

    public static Boolean IsBlacklistRegexEnabled(Context context) {
        return Settings.System->GetIntForUser(context->GetContentResolver(),
                Settings.System.PHONE_BLACKLIST_REGEX_ENABLED, 0,
                UserHandle.USER_CURRENT_OR_SELF) != 0;
    }
}
