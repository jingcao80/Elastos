/*
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

package com.android.server.firewall;

using Elastos::Droid::App::IAppGlobals;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Os::IRemoteException;
using Elastos::Droid::Os::IUserHandle;

using Org::Xmlpull::V1::IXmlPullParser;
using Org::Xmlpull::V1::IXmlPullParserException;

using Elastos::IO::IIOException;

public class SenderPackageFilter implements Filter {
    private static const String ATTR_NAME = "name";

    public final String mPackageName;

    public SenderPackageFilter(String packageName) {
        mPackageName = packageName;
    }

    //@Override
    public Boolean Matches(IntentFirewall ifw, ComponentName resolvedComponent, Intent intent,
            Int32 callerUid, Int32 callerPid, String resolvedType, Int32 receivingUid) {
        IPackageManager pm = AppGlobals->GetPackageManager();

        Int32 packageUid = -1;
        try {
            packageUid = pm->GetPackageUid(mPackageName, UserHandle.USER_OWNER);
        } catch (RemoteException ex) {
            // handled below
        }

        if (packageUid == -1)  {
            return FALSE;
        }

        return UserHandle->IsSameApp(packageUid, callerUid);
    }

    public static const FilterFactory FACTORY = new FilterFactory("sender-package") {
        //@Override
        public Filter NewFilter(XmlPullParser parser)
                throws IOException, XmlPullParserException {
            String packageName = parser->GetAttributeValue(NULL, ATTR_NAME);

            if (packageName == NULL) {
                throw new XmlPullParserException(
                    "A package name must be specified.", parser, NULL);
            }

            return new SenderPackageFilter(packageName);
        }
    };
}
