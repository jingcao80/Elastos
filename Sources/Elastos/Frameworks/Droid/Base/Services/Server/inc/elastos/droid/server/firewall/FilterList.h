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

using Elastos::Droid::internal.util.XmlUtils;
using Org::Xmlpull::V1::IXmlPullParser;
using Org::Xmlpull::V1::IXmlPullParserException;

using Elastos::IO::IIOException;
using Elastos::Utility::IArrayList;

abstract class FilterList implements Filter {
    protected final ArrayList<Filter> children = new ArrayList<Filter>();

    public FilterList ReadFromXml(XmlPullParser parser) throws IOException, XmlPullParserException {
        Int32 outerDepth = parser->GetDepth();
        while (XmlUtils->NextElementWithin(parser, outerDepth)) {
            ReadChild(parser);
        }
        return this;
    }

    protected void ReadChild(XmlPullParser parser) throws IOException, XmlPullParserException {
        Filter filter = IntentFirewall->ParseFilter(parser);
        children->Add(filter);
    }
}
