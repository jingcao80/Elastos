/*
 * Copyright (C) 2006 The Android Open Source Project
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

package com.android.internal.telephony;

using Elastos::Droid::Content::IContentProvider;
using Elastos::Droid::Content::IUriMatcher;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Database::IMergeCursor;
using Elastos::Droid::Database::IMatrixCursor;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Telephony::ISubInfoRecord;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Telephony::IRlog;

using Elastos::Utility::IList;

using Elastos::Droid::Internal::Telephony::IIIccPhoneBook;
using Elastos::Droid::Internal::Telephony::Uicc::IAdnRecord;
using Elastos::Droid::Internal::Telephony::Uicc::IIccConstants;


/**
 * {@hide}
 */
public class IccProvider extends ContentProvider {
    private static const String TAG = "IccProvider";
    private static const Boolean DBG = FALSE;


    private static const String[] ADDRESS_BOOK_COLUMN_NAMES = new String[] {
        "name",
        "number",
        "emails",
        "anrs",
        "_id"
    };

    protected static const Int32 ADN = 1;
    protected static const Int32 ADN_SUB = 2;
    protected static const Int32 FDN = 3;
    protected static const Int32 FDN_SUB = 4;
    protected static const Int32 SDN = 5;
    protected static const Int32 SDN_SUB = 6;
    protected static const Int32 ADN_ALL = 7;

    public static const String STR_TAG = "tag";
    public static const String STR_NUMBER = "number";
    public static const String STR_EMAILS = "emails";
    public static const String STR_ANRS = "anrs";
    public static const String STR_NEW_TAG = "newTag";
    public static const String STR_NEW_NUMBER = "newNumber";
    public static const String STR_NEW_EMAILS = "newEmails";
    public static const String STR_NEW_ANRS = "newAnrs";
    public static const String STR_PIN2 = "pin2";

    private static const UriMatcher URL_MATCHER =
                            new UriMatcher(UriMatcher.NO_MATCH);

    static {
        URL_MATCHER->AddURI("icc", "adn", ADN);
        URL_MATCHER->AddURI("icc", "adn/subId/#", ADN_SUB);
        URL_MATCHER->AddURI("icc", "fdn", FDN);
        URL_MATCHER->AddURI("icc", "fdn/subId/#", FDN_SUB);
        URL_MATCHER->AddURI("icc", "sdn", SDN);
        URL_MATCHER->AddURI("icc", "sdn/subId/#", SDN_SUB);
        URL_MATCHER->AddURI("icc", "adn/adn_all", ADN_ALL);
    }

    //@Override
    public Boolean OnCreate() {
        return TRUE;
    }

    //@Override
    public Cursor Query(Uri url, String[] projection, String selection,
            String[] selectionArgs, String sort) {
        If (DBG) Log("query");

        Switch (URL_MATCHER->Match(url)) {
            case ADN:
                return LoadFromEf(IccConstants.EF_ADN, SubscriptionManager->GetDefaultSubId());

            case ADN_SUB:
                return LoadFromEf(IccConstants.EF_ADN, GetRequestSubId(url));

            case FDN:
                return LoadFromEf(IccConstants.EF_FDN, SubscriptionManager->GetDefaultSubId());

            case FDN_SUB:
                return LoadFromEf(IccConstants.EF_FDN, GetRequestSubId(url));

            case SDN:
                return LoadFromEf(IccConstants.EF_SDN, SubscriptionManager->GetDefaultSubId());

            case SDN_SUB:
                return LoadFromEf(IccConstants.EF_SDN, GetRequestSubId(url));

            case ADN_ALL:
                return LoadAllSimContacts(IccConstants.EF_ADN);

            default:
                throw new IllegalArgumentException("Unknown URL " + url);
        }
    }

    private Cursor LoadAllSimContacts(Int32 efType) {
        Cursor [] result;
        List<SubInfoRecord> subInfoList = SubscriptionManager->GetActiveSubInfoList();

        If ((subInfoList == NULL) || (subInfoList->Size() == 0)) {
            result = new Cursor[0];
        } else {
            Int32 subIdCount = subInfoList->Size();
            result = new Cursor[subIdCount];
            Int64 subId;

            For (Int32 i = 0; i < subIdCount; i++) {
                subId = subInfoList->Get(i).subId;
                result[i] = LoadFromEf(efType, subId);
                Rlog->I(TAG,"ADN Records loaded for Subscription ::" + subId);
            }
        }

        return new MergeCursor(result);
    }

    //@Override
    public String GetType(Uri url) {
        Switch (URL_MATCHER->Match(url)) {
            case ADN:
            case ADN_SUB:
            case FDN:
            case FDN_SUB:
            case SDN:
            case SDN_SUB:
            case ADN_ALL:
                return "vnd.android.cursor.dir/sim-contact";

            default:
                throw new IllegalArgumentException("Unknown URL " + url);
        }
    }

    //@Override
    public Uri Insert(Uri url, ContentValues initialValues) {
        Uri resultUri;
        Int32 efType;
        String pin2 = NULL;
        Int64 subId;

        If (DBG) Log("insert");

        Int32 match = URL_MATCHER->Match(url);
        Switch (match) {
            case ADN:
                efType = IccConstants.EF_ADN;
                subId = SubscriptionManager->GetDefaultSubId();
                break;

            case ADN_SUB:
                efType = IccConstants.EF_ADN;
                subId = GetRequestSubId(url);
                break;

            case FDN:
                efType = IccConstants.EF_FDN;
                subId = SubscriptionManager->GetDefaultSubId();
                pin2 = initialValues->GetAsString("pin2");
                break;

            case FDN_SUB:
                efType = IccConstants.EF_FDN;
                subId = GetRequestSubId(url);
                pin2 = initialValues->GetAsString("pin2");
                break;

            default:
                throw new UnsupportedOperationException(
                        "Cannot insert into URL: " + url);
        }

        String tag = initialValues->GetAsString("tag");
        String number = initialValues->GetAsString("number");
        String emails = initialValues->GetAsString("emails");
        String anrs = initialValues->GetAsString("anrs");

        // TODO(): Read email instead of sending NULL.
        ContentValues mValues = new ContentValues();
        mValues->Put(STR_TAG,"");
        mValues->Put(STR_NUMBER,"");
        mValues->Put(STR_EMAILS,"");
        mValues->Put(STR_ANRS,"");
        mValues->Put(STR_NEW_TAG,tag);
        mValues->Put(STR_NEW_NUMBER,number);
        mValues->Put(STR_NEW_EMAILS,emails);
        mValues->Put(STR_NEW_ANRS,anrs);
        Boolean success = UpdateIccRecordInEf(efType, mValues, pin2, subId);

        If (!success) {
            return NULL;
        }

        StringBuilder buf = new StringBuilder("content://icc/");
        Switch (match) {
            case ADN:
                buf->Append("adn/");
                break;

            case ADN_SUB:
                buf->Append("adn/subId/");
                break;

            case FDN:
                buf->Append("fdn/");
                break;

            case FDN_SUB:
                buf->Append("fdn/subId/");
                break;
        }

        // TODO: we need to find out the rowId for the newly added record
        buf->Append(0);

        resultUri = Uri->Parse(buf->ToString());

        GetContext()->GetContentResolver().NotifyChange(url, NULL);
        /*
        // notify interested parties that an insertion happened
        GetContext()->GetContentResolver().NotifyInsert(
                resultUri, rowID, NULL);
        */

        return resultUri;
    }

    private String NormalizeValue(String inVal) {
        Int32 len = inVal->Length();
        // If name is empty in contact return NULL to avoid crash.
        If (len == 0) {
            If (DBG) Log("len of input String is 0");
            return inVal;
        }
        String retVal = inVal;

        If (inVal->CharAt(0) == '\'' && inVal->CharAt(len-1) == '\'') {
            retVal = inVal->Substring(1, len-1);
        }

        return retVal;
    }

    //@Override
    public Int32 Delete(Uri url, String where, String[] whereArgs) {
        Int32 efType;
        Int64 subId;

        If (DBG) Log("delete");
        Int32 match = URL_MATCHER->Match(url);
        Switch (match) {
            case ADN:
                efType = IccConstants.EF_ADN;
                subId = SubscriptionManager->GetDefaultSubId();
                break;

            case ADN_SUB:
                efType = IccConstants.EF_ADN;
                subId = GetRequestSubId(url);
                break;

            case FDN:
                efType = IccConstants.EF_FDN;
                subId = SubscriptionManager->GetDefaultSubId();
                break;

            case FDN_SUB:
                efType = IccConstants.EF_FDN;
                subId = GetRequestSubId(url);
                break;

            default:
                throw new UnsupportedOperationException(
                        "Cannot insert into URL: " + url);
        }

        // parse where clause
        String tag = NULL;
        String number = NULL;
        String emails = NULL;
        String anrs = NULL;
        String pin2 = NULL;

        String[] tokens = where->Split("AND");
        Int32 n = tokens.length;

        While (--n >= 0) {
            String param = tokens[n];
            If (DBG) Log("parsing '" + param + "'");

            String[] pair = param->Split("=", 2);

            If (pair.length != 2) {
                Rlog->E(TAG, "resolve: bad whereClause parameter: " + param);
                continue;
            }
            String key = pair[0].Trim();
            String val = pair[1].Trim();

            If (STR_TAG->Equals(key)) {
                tag = NormalizeValue(val);
            } else If (STR_NUMBER->Equals(key)) {
                number = NormalizeValue(val);
            } else If (STR_EMAILS->Equals(key)) {
                emails = NormalizeValue(val);
            } else If (STR_ANRS->Equals(key)) {
                anrs = NormalizeValue(val);
            } else If (STR_PIN2->Equals(key)) {
                pin2 = NormalizeValue(val);
            }
        }

        ContentValues mValues = new ContentValues();
        mValues->Put(STR_TAG,tag);
        mValues->Put(STR_NUMBER,number);
        mValues->Put(STR_EMAILS,emails);
        mValues->Put(STR_ANRS,anrs);
        mValues->Put(STR_NEW_TAG,"");
        mValues->Put(STR_NEW_NUMBER,"");
        mValues->Put(STR_NEW_EMAILS,"");
        mValues->Put(STR_NEW_ANRS,"");
        If ((efType == FDN) && TextUtils->IsEmpty(pin2)) {
            return 0;
        }

        If (DBG) Log("delete mvalues= " + mValues);
        Boolean success = UpdateIccRecordInEf(efType, mValues, pin2, subId);
        If (!success) {
            return 0;
        }

        GetContext()->GetContentResolver().NotifyChange(url, NULL);
        return 1;
    }

    //@Override
    public Int32 Update(Uri url, ContentValues values, String where, String[] whereArgs) {
        String pin2 = NULL;
        Int32 efType;
        Int64 subId;

        If (DBG) Log("update");

        Int32 match = URL_MATCHER->Match(url);
        Switch (match) {
            case ADN:
                efType = IccConstants.EF_ADN;
                subId = SubscriptionManager->GetDefaultSubId();
                break;

            case ADN_SUB:
                efType = IccConstants.EF_ADN;
                subId = GetRequestSubId(url);
                break;

            case FDN:
                efType = IccConstants.EF_FDN;
                subId = SubscriptionManager->GetDefaultSubId();
                pin2 = values->GetAsString("pin2");
                break;

            case FDN_SUB:
                efType = IccConstants.EF_FDN;
                subId = GetRequestSubId(url);
                pin2 = values->GetAsString("pin2");
                break;

            default:
                throw new UnsupportedOperationException(
                        "Cannot insert into URL: " + url);
        }

        String tag = values->GetAsString("tag");
        String number = values->GetAsString("number");
        String[] emails = NULL;
        String newTag = values->GetAsString("newTag");
        String newNumber = values->GetAsString("newNumber");
        String[] newEmails = NULL;
        // TODO(): Update for email.
        Boolean success = UpdateIccRecordInEf(efType, values, pin2, subId);

        If (!success) {
            return 0;
        }

        GetContext()->GetContentResolver().NotifyChange(url, NULL);
        return 1;
    }

    private MatrixCursor LoadFromEf(Int32 efType, Int64 subId) {
        If (DBG) Log("loadFromEf: efType=" + efType + ", subscription=" + subId);

        List<AdnRecord> adnRecords = NULL;
        try {
            IIccPhoneBook iccIpb = IIccPhoneBook.Stub->AsInterface(
                    ServiceManager->GetService("simphonebook"));
            If (iccIpb != NULL) {
                adnRecords = iccIpb->GetAdnRecordsInEfForSubscriber(subId, efType);
            }
        } Catch (RemoteException ex) {
            // ignore it
        } Catch (SecurityException ex) {
            If (DBG) Log(ex->ToString());
        }

        If (adnRecords != NULL) {
            // Load the results
            final Int32 N = adnRecords->Size();
            final MatrixCursor cursor = new MatrixCursor(ADDRESS_BOOK_COLUMN_NAMES, N);
            Log("adnRecords.size=" + N);
            For (Int32 i = 0; i < N ; i++) {
                LoadRecord(adnRecords->Get(i), cursor, i);
            }
            return cursor;
        } else {
            // No results to load
            Rlog->W(TAG, "Cannot load ADN records");
            return new MatrixCursor(ADDRESS_BOOK_COLUMN_NAMES);
        }
    }

    private Boolean
    UpdateIccRecordInEf(Int32 efType, ContentValues values, String pin2, Int64 subId) {
        Boolean success = FALSE;

        If (DBG) Log("updateIccRecordInEf: efType=" + efType +
                    ", values: [ "+ values + " ], subId:" + subId);

        try {
            IIccPhoneBook iccIpb = IIccPhoneBook.Stub->AsInterface(
                    ServiceManager->GetService("simphonebook"));
            If (iccIpb != NULL) {
                success = iccIpb
                        .UpdateAdnRecordsWithContentValuesInEfBySearchUsingSubId(
                            subId, efType, values, pin2);
            }
        } Catch (RemoteException ex) {
            // ignore it
        } Catch (SecurityException ex) {
            If (DBG) Log(ex->ToString());
        }
        If (DBG) Log("updateIccRecordInEf: " + success);
        return success;
    }

    /**
     * Loads an AdnRecord into a MatrixCursor. Must be called with mLock held.
     *
     * @param record the ADN record to load from
     * @param cursor the cursor to receive the results
     */
    private void LoadRecord(AdnRecord record, MatrixCursor cursor, Int32 id) {
        If (!record->IsEmpty()) {
            Object[] contact = new Object[5];
            String alphaTag = record->GetAlphaTag();
            String number = record->GetNumber();
            String[] anrs =record->GetAdditionalNumbers();
            If (DBG) Log("loadRecord: " + alphaTag + ", " + number + ",");
            contact[0] = alphaTag;
            contact[1] = number;

            String[] emails = record->GetEmails();
            If (emails != NULL) {
                StringBuilder emailString = new StringBuilder();
                For (String email: emails) {
                    If (DBG) Log("Adding email:" + email);
                    emailString->Append(email);
                    emailString->Append(",");
                }
                contact[2] = emailString->ToString();
            }

            If (anrs != NULL) {
                StringBuilder anrString = new StringBuilder();
                For (String anr : anrs) {
                    If (DBG) Log("Adding anr:" + anr);
                    anrString->Append(anr);
                    anrString->Append(",");
                }
                contact[3] = anrString->ToString();
            }

            contact[4] = id;
            cursor->AddRow(contact);
        }
    }

    private void Log(String msg) {
        Rlog->D(TAG, "[IccProvider] " + msg);
    }

    private Int64 GetRequestSubId(Uri url) {
        If (DBG) Log("getRequestSubId url: " + url);

        try {
            return Long->ParseLong(url->GetLastPathSegment());
        } Catch (NumberFormatException ex) {
            throw new IllegalArgumentException("Unknown URL " + url);
        }
    }
}
