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
using Elastos::Droid::Net::IUri;
using Org::Xmlpull::V1::IXmlPullParser;
using Org::Xmlpull::V1::IXmlPullParserException;

using Elastos::IO::IIOException;

class PortFilter implements Filter {
    private static const String ATTR_EQUALS = "equals";
    private static const String ATTR_MIN = "min";
    private static const String ATTR_MAX = "max";

    private static const Int32 NO_BOUND = -1;

    // both bounds are inclusive
    private final Int32 mLowerBound;
    private final Int32 mUpperBound;

    private PortFilter(Int32 lowerBound, Int32 upperBound) {
        mLowerBound = lowerBound;
        mUpperBound = upperBound;
    }

    //@Override
    public Boolean Matches(IntentFirewall ifw, ComponentName resolvedComponent, Intent intent,
            Int32 callerUid, Int32 callerPid, String resolvedType, Int32 receivingUid) {
        Int32 port = -1;
        Uri uri = intent->GetData();
        if (uri != NULL) {
            port = uri->GetPort();
        }
        return port != -1 &&
                (mLowerBound == NO_BOUND || mLowerBound <= port) &&
                (mUpperBound == NO_BOUND || mUpperBound >= port);
    }

    public static const FilterFactory FACTORY = new FilterFactory("port") {
        //@Override
        public Filter NewFilter(XmlPullParser parser)
                throws IOException, XmlPullParserException {
            Int32 lowerBound = NO_BOUND;
            Int32 upperBound = NO_BOUND;

            String equalsValue = parser->GetAttributeValue(NULL, ATTR_EQUALS);
            if (equalsValue != NULL) {
                Int32 value;
                try {
                    value = Integer->ParseInt(equalsValue);
                } catch (NumberFormatException ex) {
                    throw new XmlPullParserException("Invalid port value: " + equalsValue,
                            parser, NULL);
                }
                lowerBound = value;
                upperBound = value;
            }

            String lowerBoundString = parser->GetAttributeValue(NULL, ATTR_MIN);
            String upperBoundString = parser->GetAttributeValue(NULL, ATTR_MAX);
            if (lowerBoundString != NULL || upperBoundString != NULL) {
                if (equalsValue != NULL) {
                    throw new XmlPullParserException(
                            "Port filter cannot use both equals and range filtering",
                            parser, NULL);
                }

                if (lowerBoundString != NULL) {
                    try {
                        lowerBound = Integer->ParseInt(lowerBoundString);
                    } catch (NumberFormatException ex) {
                        throw new XmlPullParserException(
                                "Invalid minimum port value: " + lowerBoundString,
                                parser, NULL);
                    }
                }

                if (upperBoundString != NULL) {
                    try {
                        upperBound = Integer->ParseInt(upperBoundString);
                    } catch (NumberFormatException ex) {
                        throw new XmlPullParserException(
                                "Invalid maximum port value: " + upperBoundString,
                                parser, NULL);
                    }
                }
            }

            // an empty port filter is explicitly allowed, and checks for the existence of a port
            return new PortFilter(lowerBound, upperBound);
        }
    };
}
