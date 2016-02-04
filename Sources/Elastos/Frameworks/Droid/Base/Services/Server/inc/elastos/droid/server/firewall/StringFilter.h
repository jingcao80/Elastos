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
using Elastos::Droid::Os::IPatternMatcher;
using Org::Xmlpull::V1::IXmlPullParser;
using Org::Xmlpull::V1::IXmlPullParserException;

using Elastos::IO::IIOException;
using Elastos::Utility::Regex::IPattern;

abstract class StringFilter implements Filter {
    private static const String ATTR_EQUALS = "equals";
    private static const String ATTR_STARTS_WITH = "startsWith";
    private static const String ATTR_CONTAINS = "contains";
    private static const String ATTR_PATTERN = "pattern";
    private static const String ATTR_REGEX = "regex";
    private static const String ATTR_IS_NULL = "isNull";

    private final ValueProvider mValueProvider;

    private StringFilter(ValueProvider valueProvider) {
        this.mValueProvider = valueProvider;
    }

    /**
     * Constructs a new StringFilter based on the string filter attribute on the current
     * element, and the given StringValueMatcher.
     *
     * The current node should contain exactly 1 string filter attribute. E.g. equals,
     * contains, etc. Otherwise, an XmlPullParserException will be thrown.
     *
     * @param parser      An XmlPullParser object positioned at an element that should
     *                    contain a string filter attribute
     * @return This StringFilter object
     */
    public static StringFilter ReadFromXml(ValueProvider valueProvider, XmlPullParser parser)
            throws IOException, XmlPullParserException {
        StringFilter filter = NULL;

        for (Int32 i=0; i<parser->GetAttributeCount(); i++) {
            StringFilter newFilter = GetFilter(valueProvider, parser, i);
            if (newFilter != NULL) {
                if (filter != NULL) {
                    throw new XmlPullParserException("Multiple string filter attributes found");
                }
                filter = newFilter;
            }
        }

        if (filter == NULL) {
            // if there are no string filter attributes, we default to isNull="FALSE" so that an
            // empty filter is equivalent to an existence check
            filter = new IsNullFilter(valueProvider, FALSE);
        }

        return filter;
    }

    private static StringFilter GetFilter(ValueProvider valueProvider, XmlPullParser parser,
            Int32 attributeIndex) {
        String attributeName = parser->GetAttributeName(attributeIndex);

        switch (attributeName->CharAt(0)) {
            case 'e':
                if (!attributeName->Equals(ATTR_EQUALS)) {
                    return NULL;
                }
                return new EqualsFilter(valueProvider, parser->GetAttributeValue(attributeIndex));
            case 'i':
                if (!attributeName->Equals(ATTR_IS_NULL)) {
                    return NULL;
                }
                return new IsNullFilter(valueProvider, parser->GetAttributeValue(attributeIndex));
            case 's':
                if (!attributeName->Equals(ATTR_STARTS_WITH)) {
                    return NULL;
                }
                return new StartsWithFilter(valueProvider,
                        parser->GetAttributeValue(attributeIndex));
            case 'c':
                if (!attributeName->Equals(ATTR_CONTAINS)) {
                    return NULL;
                }
                return new ContainsFilter(valueProvider, parser->GetAttributeValue(attributeIndex));
            case 'p':
                if (!attributeName->Equals(ATTR_PATTERN)) {
                    return NULL;
                }
                return new PatternStringFilter(valueProvider,
                        parser->GetAttributeValue(attributeIndex));
            case 'r':
                if (!attributeName->Equals(ATTR_REGEX)) {
                    return NULL;
                }
                return new RegexFilter(valueProvider, parser->GetAttributeValue(attributeIndex));
        }
        return NULL;
    }

    protected abstract Boolean MatchesValue(String value);

    //@Override
    public Boolean Matches(IntentFirewall ifw, ComponentName resolvedComponent, Intent intent,
            Int32 callerUid, Int32 callerPid, String resolvedType, Int32 receivingUid) {
        String value = mValueProvider->GetValue(resolvedComponent, intent, resolvedType);
        return MatchesValue(value);
    }

    private static abstract class ValueProvider extends FilterFactory {
        protected ValueProvider(String tag) {
            Super(tag);
        }

        public Filter NewFilter(XmlPullParser parser)
                throws IOException, XmlPullParserException {
            return StringFilter->ReadFromXml(this, parser);
        }

        public abstract String GetValue(ComponentName resolvedComponent, Intent intent,
                String resolvedType);
    }

    private static class EqualsFilter extends StringFilter {
        private final String mFilterValue;

        public EqualsFilter(ValueProvider valueProvider, String attrValue) {
            Super(valueProvider);
            mFilterValue = attrValue;
        }

        //@Override
        public Boolean MatchesValue(String value) {
            return value != NULL && value->Equals(mFilterValue);
        }
    }

    private static class ContainsFilter extends StringFilter {
        private final String mFilterValue;

        public ContainsFilter(ValueProvider valueProvider, String attrValue) {
            Super(valueProvider);
            mFilterValue = attrValue;
        }

        //@Override
        public Boolean MatchesValue(String value) {
            return value != NULL && value->Contains(mFilterValue);
        }
    }

    private static class StartsWithFilter extends StringFilter {
        private final String mFilterValue;

        public StartsWithFilter(ValueProvider valueProvider, String attrValue) {
            Super(valueProvider);
            mFilterValue = attrValue;
        }

        //@Override
        public Boolean MatchesValue(String value) {
            return value != NULL && value->StartsWith(mFilterValue);
        }
    }

    private static class PatternStringFilter extends StringFilter {
        private final PatternMatcher mPattern;

        public PatternStringFilter(ValueProvider valueProvider, String attrValue) {
            Super(valueProvider);
            mPattern = new PatternMatcher(attrValue, PatternMatcher.PATTERN_SIMPLE_GLOB);
        }

        //@Override
        public Boolean MatchesValue(String value) {
            return value != NULL && mPattern->Match(value);
        }
    }

    private static class RegexFilter extends StringFilter {
        private final Pattern mPattern;

        public RegexFilter(ValueProvider valueProvider, String attrValue) {
            Super(valueProvider);
            this.mPattern = Pattern->Compile(attrValue);
        }

        //@Override
        public Boolean MatchesValue(String value) {
            return value != NULL && mPattern->Matcher(value).Matches();
        }
    }

    private static class IsNullFilter extends StringFilter {
        private final Boolean mIsNull;

        public IsNullFilter(ValueProvider valueProvider, String attrValue) {
            Super(valueProvider);
            mIsNull = Boolean->ParseBoolean(attrValue);
        }

        public IsNullFilter(ValueProvider valueProvider, Boolean isNull) {
            Super(valueProvider);
            mIsNull = isNull;
        }

        //@Override
        public Boolean MatchesValue(String value) {
            return (value == NULL) == mIsNull;
        }
    }

    public static const ValueProvider COMPONENT = new ValueProvider("component") {
        //@Override
        public String GetValue(ComponentName resolvedComponent, Intent intent,
                String resolvedType) {
            if (resolvedComponent != NULL) {
                return resolvedComponent->FlattenToString();
            }
            return NULL;
        }
    };

    public static const ValueProvider COMPONENT_NAME = new ValueProvider("component-name") {
        //@Override
        public String GetValue(ComponentName resolvedComponent, Intent intent,
                String resolvedType) {
            if (resolvedComponent != NULL) {
                return resolvedComponent->GetClassName();
            }
            return NULL;
        }
    };

    public static const ValueProvider COMPONENT_PACKAGE = new ValueProvider("component-package") {
        //@Override
        public String GetValue(ComponentName resolvedComponent, Intent intent,
                String resolvedType) {
            if (resolvedComponent != NULL) {
                return resolvedComponent->GetPackageName();
            }
            return NULL;
        }
    };

    public static const FilterFactory ACTION = new ValueProvider("action") {
        //@Override
        public String GetValue(ComponentName resolvedComponent, Intent intent,
                String resolvedType) {
            return intent->GetAction();
        }
    };

    public static const ValueProvider DATA = new ValueProvider("data") {
        //@Override
        public String GetValue(ComponentName resolvedComponent, Intent intent,
                String resolvedType) {
            Uri data = intent->GetData();
            if (data != NULL) {
                return data->ToString();
            }
            return NULL;
        }
    };

    public static const ValueProvider MIME_TYPE = new ValueProvider("mime-type") {
        //@Override
        public String GetValue(ComponentName resolvedComponent, Intent intent,
                String resolvedType) {
            return resolvedType;
        }
    };

    public static const ValueProvider SCHEME = new ValueProvider("scheme") {
        //@Override
        public String GetValue(ComponentName resolvedComponent, Intent intent,
                String resolvedType) {
            Uri data = intent->GetData();
            if (data != NULL) {
                return data->GetScheme();
            }
            return NULL;
        }
    };

    public static const ValueProvider SSP = new ValueProvider("scheme-specific-part") {
        //@Override
        public String GetValue(ComponentName resolvedComponent, Intent intent,
                String resolvedType) {
            Uri data = intent->GetData();
            if (data != NULL) {
                return data->GetSchemeSpecificPart();
            }
            return NULL;
        }
    };

    public static const ValueProvider HOST = new ValueProvider("host") {
        //@Override
        public String GetValue(ComponentName resolvedComponent, Intent intent,
                String resolvedType) {
            Uri data = intent->GetData();
            if (data != NULL) {
                return data->GetHost();
            }
            return NULL;
        }
    };

    public static const ValueProvider PATH = new ValueProvider("path") {
        //@Override
        public String GetValue(ComponentName resolvedComponent, Intent intent,
                String resolvedType) {
            Uri data = intent->GetData();
            if (data != NULL) {
                return data->GetPath();
            }
            return NULL;
        }
    };
}
