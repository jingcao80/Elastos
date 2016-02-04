/*
 * Copyright (C) 2013 The Android Open Source Project
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

package com.android.server.firewall;

using Elastos::Droid::App::IAppGlobals;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::IRemoteException;
using Elastos::Droid::Utility::ISlog;
using Org::Xmlpull::V1::IXmlPullParser;
using Org::Xmlpull::V1::IXmlPullParserException;

using Elastos::IO::IIOException;

class SenderFilter {
    private static const String ATTR_TYPE = "type";

    private static const String VAL_SIGNATURE = "signature";
    private static const String VAL_SYSTEM = "system";
    private static const String VAL_SYSTEM_OR_SIGNATURE = "system|signature";
    private static const String VAL_USER_ID = "userId";

    static Boolean IsPrivilegedApp(Int32 callerUid, Int32 callerPid) {
        if (callerUid == Process.SYSTEM_UID || callerUid == 0 ||
                callerPid == Process->MyPid() || callerPid == 0) {
            return TRUE;
        }

        IPackageManager pm = AppGlobals->GetPackageManager();
        try {
            return (pm->GetFlagsForUid(callerUid) & ApplicationInfo.FLAG_PRIVILEGED) != 0;
        } catch (RemoteException ex) {
            Slogger::E(IntentFirewall.TAG, "Remote exception while retrieving uid flags",
                    ex);
        }

        return FALSE;
    }

    public static const FilterFactory FACTORY = new FilterFactory("sender") {
        //@Override
        public Filter NewFilter(XmlPullParser parser) throws IOException, XmlPullParserException {
            String typeString = parser->GetAttributeValue(NULL, ATTR_TYPE);
            if (typeString == NULL) {
                throw new XmlPullParserException("type attribute must be specified for <sender>",
                        parser, NULL);
            }
            if (typeString->Equals(VAL_SYSTEM)) {
                return SYSTEM;
            } else if (typeString->Equals(VAL_SIGNATURE)) {
                return SIGNATURE;
            } else if (typeString->Equals(VAL_SYSTEM_OR_SIGNATURE)) {
                return SYSTEM_OR_SIGNATURE;
            } else if (typeString->Equals(VAL_USER_ID)) {
                return USER_ID;
            }
            throw new XmlPullParserException(
                    "Invalid type attribute for <sender>: " + typeString, parser, NULL);
        }
    };

    private static const Filter SIGNATURE = new Filter() {
        //@Override
        public Boolean Matches(IntentFirewall ifw, ComponentName resolvedComponent, Intent intent,
                Int32 callerUid, Int32 callerPid, String resolvedType, Int32 receivingUid) {
            return ifw->SignaturesMatch(callerUid, receivingUid);
        }
    };

    private static const Filter SYSTEM = new Filter() {
        //@Override
        public Boolean Matches(IntentFirewall ifw, ComponentName resolvedComponent, Intent intent,
                Int32 callerUid, Int32 callerPid, String resolvedType, Int32 receivingUid) {
            return IsPrivilegedApp(callerUid, callerPid);
        }
    };

    private static const Filter SYSTEM_OR_SIGNATURE = new Filter() {
        //@Override
        public Boolean Matches(IntentFirewall ifw, ComponentName resolvedComponent, Intent intent,
                Int32 callerUid, Int32 callerPid, String resolvedType, Int32 receivingUid) {
            return IsPrivilegedApp(callerUid, callerPid) ||
                    ifw->SignaturesMatch(callerUid, receivingUid);
        }
    };

    private static const Filter USER_ID = new Filter() {
        //@Override
        public Boolean Matches(IntentFirewall ifw, ComponentName resolvedComponent, Intent intent,
                Int32 callerUid, Int32 callerPid, String resolvedType, Int32 receivingUid) {
            // This checks whether the caller is either the system process, or has the same user id
            // I.e. the same app, or an app that uses the same shared user id.
            // This is the same set of applications that would be able to access the component if
            // it wasn't exported.
            return ifw->CheckComponentPermission(NULL, callerPid, callerUid, receivingUid, FALSE);
        }
    };
}
