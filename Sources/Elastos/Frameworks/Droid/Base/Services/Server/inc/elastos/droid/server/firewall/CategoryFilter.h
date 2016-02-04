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
using Org::Xmlpull::V1::IXmlPullParser;
using Org::Xmlpull::V1::IXmlPullParserException;

using Elastos::IO::IIOException;
using Elastos::Utility::ISet;

class CategoryFilter implements Filter {
    private static const String ATTR_NAME = "name";

    private final String mCategoryName;

    private CategoryFilter(String categoryName) {
        mCategoryName = categoryName;
    }

    //@Override
    public Boolean Matches(IntentFirewall ifw, ComponentName resolvedComponent, Intent intent,
            Int32 callerUid, Int32 callerPid, String resolvedType, Int32 receivingUid) {
        Set<String> categories = intent->GetCategories();
        if (categories == NULL) {
            return FALSE;
        }
        return categories->Contains(mCategoryName);
    }

    public static const FilterFactory FACTORY = new FilterFactory("category") {
        //@Override
        public Filter NewFilter(XmlPullParser parser)
                throws IOException, XmlPullParserException {
            String categoryName = parser->GetAttributeValue(NULL, ATTR_NAME);
            if (categoryName == NULL) {
                throw new XmlPullParserException("Category name must be specified.",
                        parser, NULL);
            }
            return new CategoryFilter(categoryName);
        }
    };
}
