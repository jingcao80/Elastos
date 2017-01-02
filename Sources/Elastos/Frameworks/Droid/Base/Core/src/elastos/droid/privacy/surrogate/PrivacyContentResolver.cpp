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

#include "PrivacyContentResolver.h"
#include "PrivacyCursor.h"
#include <os/Binder.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Privacy::IPrivacySettings;
using Elastos::Droid::Provider::ICalendarContract;
using Elastos::Droid::Provider::ICallLog;
using Elastos::Droid::Provider::IContacts;
using Elastos::Droid::Provider::IContactsContract;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Privacy {
namespace Surrogate {

const String PrivacyContentResolver::TAG("PrivacyContentResolver");
const String PrivacyContentResolver::SMS_CONTENT_URI_AUTHORITY("sms");
const String PrivacyContentResolver::MMS_CONTENT_URI_AUTHORITY("mms");
const String PrivacyContentResolver::MMS_SMS_CONTENT_URI_AUTHORITY("mms-sms");
AutoPtr<IPrivacySettingsManager> PrivacyContentResolver::mPrivacySetMan(NULL);

UInt32 PrivacyContentResolver::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 PrivacyContentResolver::Release()
{
    return ElRefBase::Release();
}

ECode PrivacyContentResolver::EnforcePrivacyPermission(
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ IContext* context,
    /* [in] */ ICursor* realCursor,
    /* [out] */ ICursor** outCusor)
{
    VALIDATE_NOT_NULL(outCusor)

    if (uri != NULL) {
        if (mPrivacySetMan == NULL) {
            AutoPtr<IInterface> obj;
            context->GetSystemService(String("privacy"), (IInterface**)&obj);
            mPrivacySetMan = IPrivacySettingsManager::Probe(obj);
        }

        String packageName;
        AutoPtr<IPrivacySettings> pSet;
        context->GetPackageName(&packageName);
        Int32 uid = Binder::GetCallingUid();
        mPrivacySetMan->GetSettings(packageName, uid, (IPrivacySettings**)&pSet);
        String auth;
        uri->GetAuthority(&auth);
        String output_label("[real]");
        AutoPtr<ICursor> output = realCursor;

        if (!auth.IsEmpty()) {
            if (auth.Equals(IContacts::AUTHORITY) || auth.Equals(IContactsContract::AUTHORITY)) {
                if (pSet != NULL) {
                    Byte contactsSetting = 0;
                    pSet->GetContactsSetting(&contactsSetting);
                    String uriStr;
                    uri->ToString(&uriStr);

                    if (contactsSetting == IPrivacySettings::EMPTY) {
                        output_label = String("[empty]");
                        output = new PrivacyCursor();
                        mPrivacySetMan->Notification(packageName, uid, IPrivacySettings::EMPTY, IPrivacySettings::DATA_CONTACTS, String(NULL), pSet.Get());
                    } else if (contactsSetting == IPrivacySettings::CUSTOM
                            && uriStr.Contains(GetContactsContractContactsContentUriString())) {
                        Boolean idFound = FALSE;
                        if (projection != NULL) {
                            Int32 size = projection->GetLength();
                            for (Int32 i = 0; i < size; i++) {
                                String p = (*projection)[i];
                                if (p.Equals("_id")) {
                                    idFound = TRUE;
                                    break;
                                }
                            }

                        // if (!idFound) { // add ID to projection
                        // String[] newProjection = new
                        // String[projection.length + 1];
                        // System.arraycopy(projection, 0,
                        // newProjection, 0, projection.length);
                        // newProjection[projection.length] =
                        // ContactsContract.Contacts._ID;
                        // projection = newProjection;
                            // }
                        }
                        if (!idFound) {
                            output = new PrivacyCursor();
                        } else {
                            AutoPtr<ArrayOf<Int32> > allowedContacts;
                            pSet->GetAllowedContacts((ArrayOf<Int32>**)&allowedContacts);
                            output = new PrivacyCursor(output, allowedContacts);
                        }
                        mPrivacySetMan->Notification(packageName, uid, IPrivacySettings::CUSTOM, IPrivacySettings::DATA_CONTACTS, String(NULL), pSet);
                    } else { // REAL
                        mPrivacySetMan->Notification(packageName, uid, IPrivacySettings::REAL, IPrivacySettings::DATA_CONTACTS, String(NULL), pSet);
                    }
                }
            } else if (auth.Equals(ICalendarContract::AUTHORITY)) {
                Byte calendarSetting = 0;
                if(pSet != NULL) {
                    pSet->GetCalendarSetting(&calendarSetting);
                }
                if (pSet != NULL && calendarSetting == IPrivacySettings::EMPTY) {
                    output_label = String("[empty]");
                    output = new PrivacyCursor();
                    mPrivacySetMan->Notification(packageName, uid, IPrivacySettings::EMPTY, IPrivacySettings::DATA_CALENDAR, String(NULL), pSet);
                } else {
                    mPrivacySetMan->Notification(packageName, uid, IPrivacySettings::REAL, IPrivacySettings::DATA_CALENDAR, String(NULL), pSet);
                }
            } else if (auth.Equals(MMS_CONTENT_URI_AUTHORITY)) {
                Byte mmsSetting = 0;
                if(pSet != NULL) {
                    pSet->GetMmsSetting(&mmsSetting);
                }
                if (pSet != NULL && mmsSetting == IPrivacySettings::EMPTY) {
                    output_label = String("[empty]");
                    output = new PrivacyCursor();
                    mPrivacySetMan->Notification(packageName, uid, IPrivacySettings::EMPTY, IPrivacySettings::DATA_MMS, String(NULL), pSet);
                } else {
                    mPrivacySetMan->Notification(packageName, uid, IPrivacySettings::REAL, IPrivacySettings::DATA_MMS, String(NULL), pSet);
                }

            } else if (auth.Equals(SMS_CONTENT_URI_AUTHORITY)) {
                Byte smsSetting = 0;
                if(pSet != NULL) {
                    pSet->GetSmsSetting(&smsSetting);
                }
                if (pSet != NULL && smsSetting == IPrivacySettings::EMPTY) {
                    output_label = String("[empty]");
                    output = new PrivacyCursor();
                    mPrivacySetMan->Notification(packageName, uid, IPrivacySettings::EMPTY, IPrivacySettings::DATA_SMS, String(NULL), pSet);
                } else {
                    mPrivacySetMan->Notification(packageName, uid, IPrivacySettings::REAL, IPrivacySettings::DATA_SMS, String(NULL), pSet);
                }
                // all messages, sms and mms
            } else if (auth.Equals(MMS_SMS_CONTENT_URI_AUTHORITY)
                    || auth.Equals("mms-sms-v2") /** htc specific accessed by system messages application */) {
                // deny access if access to either sms, mms or both is
                // restricted by privacy settings
                Byte smsSetting = 0;
                Byte mmsSetting = 0;
                if(pSet != NULL) {
                    pSet->GetSmsSetting(&smsSetting);
                    pSet->GetMmsSetting(&mmsSetting);
                }
                if (pSet != NULL && (mmsSetting == IPrivacySettings::EMPTY || smsSetting == IPrivacySettings::EMPTY)) {
                    output_label = String("[empty]");
                    output = new PrivacyCursor();
                    mPrivacySetMan->Notification(packageName, uid, IPrivacySettings::EMPTY, IPrivacySettings::DATA_MMS_SMS, String(NULL), pSet);
                } else {
                    mPrivacySetMan->Notification(packageName, uid, IPrivacySettings::REAL, IPrivacySettings::DATA_MMS_SMS, String(NULL), pSet);
                }

            } else if (auth.Equals(ICallLog::AUTHORITY)) {
                Byte callSetting = 0;
                if(pSet != NULL) {
                    pSet->GetCallLogSetting(&callSetting);
                }
                if (pSet != NULL && callSetting == IPrivacySettings::EMPTY) {
                    output_label = String("[empty]");
                    output = new PrivacyCursor();
                    mPrivacySetMan->Notification(packageName, uid, IPrivacySettings::EMPTY, IPrivacySettings::DATA_CALL_LOG, String(NULL), pSet);
                } else {
                    mPrivacySetMan->Notification(packageName, uid, IPrivacySettings::REAL, IPrivacySettings::DATA_CALL_LOG, String(NULL), pSet);
                }

            } else if (auth.Equals(GetBrowserBookmarksUriAuthority())) {
                Byte bookmarksSetting = 0;
                if(pSet != NULL) {
                    pSet->GetBookmarksSetting(&bookmarksSetting);
                }

                if (pSet != NULL && bookmarksSetting == IPrivacySettings::EMPTY) {
                    output_label = "[empty]";
                    output = new PrivacyCursor();
                    mPrivacySetMan->Notification(packageName, uid, IPrivacySettings::EMPTY, IPrivacySettings::DATA_BOOKMARKS, String(NULL), pSet);
                } else {
                    mPrivacySetMan->Notification(packageName, uid, IPrivacySettings::REAL, IPrivacySettings::DATA_BOOKMARKS, String(NULL), pSet);
                }
            }
        }
        // Log.d(TAG, "query - " + packageName + " (" + uid + ") auth: " + auth + " output: " + output_label);
        *outCusor = output.Get();
        REFCOUNT_ADD(*outCusor);
        return NOERROR;
    }
    *outCusor = realCursor;
    REFCOUNT_ADD(*outCusor);
    return NOERROR;
}

ECode PrivacyContentResolver::EnforcePrivacyPermission(
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ IContext* context,
    /* [in] */ ICursor* realCursor,
    /* [in] */ Boolean google_access,
    /* [out] */ ICursor** outCusor)
{
    VALIDATE_NOT_NULL(outCusor)

    if (uri != NULL) {
        if (mPrivacySetMan == NULL) {
            AutoPtr<IInterface> obj;
            context->GetSystemService(String("privacy"), (IInterface**)&obj);
            mPrivacySetMan = IPrivacySettingsManager::Probe(obj);
        }

        String packageName;
        AutoPtr<IPrivacySettings> pSet;
        String auth;
        context->GetPackageName(&packageName);
        Int32 uid = Binder::GetCallingUid();
        mPrivacySetMan->GetSettings(packageName, uid, (IPrivacySettings**)&pSet);
        uri->GetAuthority(&auth);
        String output_label("[real]");
        AutoPtr<ICursor> output = realCursor;
        if (auth != NULL && auth.Equals("com.google.android.gsf.gservices")) {

            Byte simInfoSetting = 0;
            if(pSet != NULL){
                pSet->GetSimInfoSetting(&simInfoSetting);
            }

            if (pSet != NULL && simInfoSetting != IPrivacySettings::REAL) {
                Int32 actual_pos = 0;
                Int32 forbidden_position = -1;
                realCursor->GetPosition(&actual_pos);

                Int32 count = 0;
                realCursor->GetCount(&count);

                ECode ec = E_FAIL;

                Boolean moveResult = FALSE;
                for (Int32 i = 0; i < count; i++) {
                    String androidId("");
                    realCursor->MoveToNext(&moveResult);
                    ec = realCursor->GetString(0, &androidId);

                    if(FAILED(ec)){
                        Logger::E(TAG, "something went wrong while getting blocked permission for android id");
                        break;
                    }

                    if (androidId.Equals("android_id")) {
                        ec = realCursor->GetPosition(&forbidden_position);

                        if(FAILED(ec)){
                            Logger::E(TAG, "something went wrong while getting blocked permission for android id");
                       }
                        break;
                    }
                }

                realCursor->MoveToPosition(actual_pos, &moveResult);

                if(forbidden_position == -1) {
                    Logger::I(TAG,"now we return real cursor, because forbidden_pos is -1");
                    *outCusor = output.Get();
                    REFCOUNT_ADD(*outCusor)
                    return NOERROR;
                } //give realcursor, because there is no android_id to block

                Logger::I(TAG, "now blocking google access to android id and give fake cursor. forbidden_position: " + forbidden_position);
                output_label = String("[fake]");
//                output = new PrivacyCursor(realCursor, forbidden_position);
                mPrivacySetMan->Notification(packageName, uid, IPrivacySettings::EMPTY, IPrivacySettings::DATA_NETWORK_INFO_SIM, String(NULL), pSet.Get());
            }
            else {
                Logger::I(TAG, "google is allowed to get real cursor");
                mPrivacySetMan->Notification(packageName, uid, IPrivacySettings::REAL, IPrivacySettings::DATA_NETWORK_INFO_SIM, String(NULL), pSet.Get());
            }
        }

        *outCusor = output.Get();
        REFCOUNT_ADD(*outCusor);
        return NOERROR;
    }

    *outCusor = realCursor;
    REFCOUNT_ADD(*outCusor);
    return NOERROR;
}

String PrivacyContentResolver::ArrayToString(
    /* [in] */ ArrayOf<String>* array)
{
    StringBuilder sb("");

    if (array != NULL){
        Int32 size = array->GetLength();

        for (Int32 i = 0; i < size; i++ ) {
            sb += "[";
            sb += (*array)[i];
            sb += "]";
        }
    }

    String result(NULL);
    sb.ToString(&result);
    return result;
}

// TODO: Update while provider module finished
String PrivacyContentResolver::GetContactsContractContactsContentUriString()
{
    return String("");
}

// TODO: Update while provider module finished
String PrivacyContentResolver::GetBrowserBookmarksUriAuthority()
{
    // "content://browser/bookmarks"
    return String("browser");
}

} // namespace Surrogate
} // namespace Privacy
} // namespace Droid
} // namespace Elastos
