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

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::internal.util.XmlUtils;
using Org::Xmlpull::V1::IXmlPullParser;
using Org::Xmlpull::V1::IXmlPullParserException;

using Elastos::IO::IIOException;

class NotFilter implements Filter {
    private final Filter mChild;

    private NotFilter(Filter child) {
        mChild = child;
    }

    //@Override
    public Boolean Matches(IntentFirewall ifw, ComponentName resolvedComponent, Intent intent,
            Int32 callerUid, Int32 callerPid, String resolvedType, Int32 receivingUid) {
        return !mChild->Matches(ifw, resolvedComponent, intent, callerUid, callerPid, resolvedType,
                receivingUid);
    }

    public static const FilterFactory FACTORY = new FilterFactory("not") {
        //@Override
        public Filter NewFilter(XmlPullParser parser)
                throws IOException, XmlPullParserException {
            Filter child = NULL;
            Int32 outerDepth = parser->GetDepth();
            while (XmlUtils->NextElementWithin(parser, outerDepth)) {
                Filter filter = IntentFirewall->ParseFilter(parser);
                if (child == NULL) {
                    child = filter;
                } else {
                    throw new XmlPullParserException(
                            "<not> tag can only contain a single child filter.", parser, NULL);
                }
            }
            return new NotFilter(child);
        }
    };
}
