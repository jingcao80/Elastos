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

#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/server/firewall/StringFilter.h"
#include <elastos/core/Object.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Net::IUri;
using Elastos::Core::StringUtils;
using Elastos::Utility::Regex::IMatcher;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Firewall {

//------------------------------------------------------------------------------
// ValueProvider
//------------------------------------------------------------------------------

ValueProvider::ValueProvider(
    /* [in] */ const String& tag)
{
    FilterFactory::constructor(tag);
}

ECode ValueProvider::NewFilter(
    /* [in] */ IXmlPullParser* parser,
    /* [out] */ IFilter** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<StringFilter> stringFilter = StringFilter::ReadFromXml(this, parser);
    *result = IFilter::Probe(stringFilter);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

//------------------------------------------------------------------------------
// EqualsFilter
//------------------------------------------------------------------------------

EqualsFilter::EqualsFilter(
    /* [in] */ ValueProvider* valueProvider,
    /* [in] */ const String& attrValue)
    : StringFilter(valueProvider), mFilterValue(attrValue)
{}

Boolean EqualsFilter::MatchesValue(
    /* [in] */ const String& value)
{
    return !value.IsNull() && value.Equals(mFilterValue);
}

//------------------------------------------------------------------------------
// ContainsFilter
//------------------------------------------------------------------------------

ContainsFilter::ContainsFilter(
    /* [in] */ ValueProvider* valueProvider,
    /* [in] */ const String& attrValue)
    : StringFilter(valueProvider), mFilterValue(attrValue)
{}

Boolean ContainsFilter::MatchesValue(
    /* [in] */ const String& value)
{
    return !value.IsNull() && value.Contains(mFilterValue);
}

//------------------------------------------------------------------------------
// StartsWithFilter
//------------------------------------------------------------------------------

StartsWithFilter::StartsWithFilter(
    /* [in] */ ValueProvider* valueProvider,
    /* [in] */ const String& attrValue)
    : StringFilter(valueProvider), mFilterValue(attrValue)
{}

Boolean StartsWithFilter::MatchesValue(
    /* [in] */ const String& value)
{
    return !value.IsNull() && value.StartWith(mFilterValue);
}

//------------------------------------------------------------------------------
// PatternStringFilter
//------------------------------------------------------------------------------

PatternStringFilter::PatternStringFilter(
    /* [in] */ ValueProvider* valueProvider,
    /* [in] */ const String& attrValue)
    : StringFilter(valueProvider)
{
    CPatternMatcher::New(attrValue, IPatternMatcher::PATTERN_SIMPLE_GLOB, (IPatternMatcher**)&mPattern);
}

Boolean PatternStringFilter::MatchesValue(
    /* [in] */ const String& value)
{
    Boolean b = FALSE;
    mPattern->Match(value, &b);
    return !value.IsNull() && b;
}

//------------------------------------------------------------------------------
// PatternStringFilter
//------------------------------------------------------------------------------

RegexFilter::RegexFilter(
    /* [in] */ ValueProvider* valueProvider,
    /* [in] */ const String& attrValue)
    : StringFilter(valueProvider)
{
    Pattern::Compile(attrValue, (IPattern**)&mPattern);
}

Boolean RegexFilter::MatchesValue(
    /* [in] */ const String& value)
{
    AutoPtr<IMatcher> matcher;
    Boolean matched;

    mPattern->Matcher(value, (IMatcher**)&matcher);
    matcher->Matches(&matched);
    return !value.IsNull() && matched;
}

//------------------------------------------------------------------------------
// PatternStringFilter
//------------------------------------------------------------------------------

IsNullFilter::IsNullFilter(
    /* [in] */ ValueProvider* valueProvider,
    /* [in] */ const String& attrValue)
    : StringFilter(valueProvider)
{
    mIsNull = StringUtils::ParseBoolean(attrValue);
}

IsNullFilter::IsNullFilter(
    /* [in] */ ValueProvider* valueProvider,
    /* [in] */ Boolean isNull)
    : StringFilter(valueProvider), mIsNull(isNull)
{}


Boolean IsNullFilter::MatchesValue(
    /* [in] */ const String& value)
{
    return (value.IsNull()) == mIsNull;
}

//------------------------------------------------------------------------------
// COMPONENT_ValueProvider
//------------------------------------------------------------------------------

String COMPONENT_ValueProvider::GetValue(
    /* in */ IComponentName* resolvedComponent,
    /* in */ IIntent* intent,
    /* in */ const String& resolvedType)
{
    if (resolvedComponent != NULL) {
        String str;
        resolvedComponent->FlattenToString(&str);
        return str;
    }
    return String(NULL);
}

//------------------------------------------------------------------------------
// COMPONENT_NAME_ValueProvider
//------------------------------------------------------------------------------

String COMPONENT_NAME_ValueProvider::GetValue(
    /* in */ IComponentName* resolvedComponent,
    /* in */ IIntent* intent,
    /* in */ const String& resolvedType)
{
    if (resolvedComponent != NULL) {
        String str;
        resolvedComponent->GetClassName(&str);
        return str;
    }
    return String(NULL);
}

//------------------------------------------------------------------------------
// COMPONENT_PACKAGE_ValueProvider
//------------------------------------------------------------------------------

String COMPONENT_PACKAGE_ValueProvider::GetValue(
    /* in */ IComponentName* resolvedComponent,
    /* in */ IIntent* intent,
    /* in */ const String& resolvedType)
{
    if (resolvedComponent != NULL) {
        String str;
        resolvedComponent->GetPackageName(&str);
        return str;
    }
    return String(NULL);
}

//------------------------------------------------------------------------------
// ACTION_ValueProvider
//------------------------------------------------------------------------------

String ACTION_ValueProvider::GetValue(
    /* in */ IComponentName* resolvedComponent,
    /* in */ IIntent* intent,
    /* in */ const String& resolvedType)
{
    String str;
    intent->GetAction(&str);
    return str;
}

//------------------------------------------------------------------------------
// DATA_ValueProvider
//------------------------------------------------------------------------------

String DATA_ValueProvider::GetValue(
    /* in */ IComponentName* resolvedComponent,
    /* in */ IIntent* intent,
    /* in */ const String& resolvedType)
{
    AutoPtr<IUri> data;
    intent->GetData((IUri**)&data);
    if (data != NULL) {
        return TO_STR(data);
    }
    return String(NULL);
}

//------------------------------------------------------------------------------
// MIME_TYPE_ValueProvider
//------------------------------------------------------------------------------

String MIME_TYPE_ValueProvider::GetValue(
    /* in */ IComponentName* resolvedComponent,
    /* in */ IIntent* intent,
    /* in */ const String& resolvedType)
{
    return resolvedType;
}

//------------------------------------------------------------------------------
// SCHEME_ValueProvider
//------------------------------------------------------------------------------

String SCHEME_ValueProvider::GetValue(
    /* in */ IComponentName* resolvedComponent,
    /* in */ IIntent* intent,
    /* in */ const String& resolvedType)
{
    AutoPtr<IUri> data;
    intent->GetData((IUri**)&data);
    if (data != NULL) {
        String str;
        data->GetScheme(&str);
        return str;
    }
    return String(NULL);
}

//------------------------------------------------------------------------------
// SSP_ValueProvider
//------------------------------------------------------------------------------

String SSP_ValueProvider::GetValue(
    /* in */ IComponentName* resolvedComponent,
    /* in */ IIntent* intent,
    /* in */ const String& resolvedType)
{
    AutoPtr<IUri> data;
    intent->GetData((IUri**)&data);
    if (data != NULL) {
        String str;
        data->GetSchemeSpecificPart(&str);
        return str;
    }
    return String(NULL);
}

//------------------------------------------------------------------------------
// HOST_ValueProvider
//------------------------------------------------------------------------------

String HOST_ValueProvider::GetValue(
    /* in */ IComponentName* resolvedComponent,
    /* in */ IIntent* intent,
    /* in */ const String& resolvedType)
{
    AutoPtr<IUri> data;
    intent->GetData((IUri**)&data);
    if (data != NULL) {
        String str;
        data->GetHost(&str);
        return str;
    }
    return String(NULL);
}

//------------------------------------------------------------------------------
// PATH_ValueProvider
//------------------------------------------------------------------------------

String PATH_ValueProvider::GetValue(
    /* in */ IComponentName* resolvedComponent,
    /* in */ IIntent* intent,
    /* in */ const String& resolvedType)
{
    AutoPtr<IUri> data;
    intent->GetData((IUri**)&data);
    if (data != NULL) {
        String str;
        data->GetPath(&str);
        return str;
    }
    return String(NULL);
}

//=======================================================================================
// StringFilter
//=======================================================================================

const String StringFilter::ATTR_EQUALS("equals");
const String StringFilter::ATTR_STARTS_WITH("startsWith");
const String StringFilter::ATTR_CONTAINS("contains");
const String StringFilter::ATTR_PATTERN("pattern");
const String StringFilter::ATTR_REGEX("regex");
const String StringFilter::ATTR_IS_NULL("isNull");

INIT_PROI_3 AutoPtr<COMPONENT_ValueProvider> StringFilter::COMPONENT = new COMPONENT_ValueProvider(String("component"));
INIT_PROI_3 AutoPtr<COMPONENT_NAME_ValueProvider> StringFilter::COMPONENT_NAME = new COMPONENT_NAME_ValueProvider(String("component-name"));
INIT_PROI_3 AutoPtr<COMPONENT_PACKAGE_ValueProvider> StringFilter::COMPONENT_PACKAGE = new COMPONENT_PACKAGE_ValueProvider(String("component-package"));
INIT_PROI_3 AutoPtr<ACTION_ValueProvider> StringFilter::ACTION = new ACTION_ValueProvider(String("action"));
INIT_PROI_3 AutoPtr<DATA_ValueProvider> StringFilter::DATA = new DATA_ValueProvider(String("data"));
INIT_PROI_3 AutoPtr<MIME_TYPE_ValueProvider> StringFilter::MIME_TYPE = new MIME_TYPE_ValueProvider(String("mime-type"));
INIT_PROI_3 AutoPtr<SCHEME_ValueProvider> StringFilter::SCHEME = new SCHEME_ValueProvider(String("scheme"));
INIT_PROI_3 AutoPtr<SSP_ValueProvider> StringFilter::SSP = new SSP_ValueProvider(String("scheme-specific-part"));
INIT_PROI_3 AutoPtr<HOST_ValueProvider> StringFilter::HOST = new HOST_ValueProvider(String("host"));
INIT_PROI_3 AutoPtr<PATH_ValueProvider> StringFilter::PATH = new PATH_ValueProvider(String("path"));

CAR_INTERFACE_IMPL(StringFilter, Object, IFilter);

StringFilter::StringFilter(
    /* [in] */ ValueProvider* valueProvider)
    : mValueProvider(valueProvider)
{}

AutoPtr<StringFilter> StringFilter::ReadFromXml(
    /* [in] */ ValueProvider* valueProvider,
    /* [in] */ IXmlPullParser* parser)
{
    AutoPtr<StringFilter> filter;
    Int32 count;

    parser->GetAttributeCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<StringFilter> newFilter = GetFilter(valueProvider, parser, i);
        if (newFilter != NULL) {
            if (filter != NULL) {
                //throw new XmlPullParserException("Multiple string filter attributes found");
                return NULL;
            }
            filter = newFilter;
        }
    }

    if (filter == NULL) {
        // if there are no string filter attributes, we default to isNull="false" so that an
        // empty filter is equivalent to an existence check
        filter = new IsNullFilter(valueProvider, FALSE);
    }

    return filter;
}

AutoPtr<StringFilter> StringFilter::GetFilter(
    /* [in] */ ValueProvider* valueProvider,
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ Int32 attributeIndex)
{
    String attributeName;
    String attriValue;
    parser->GetAttributeName(attributeIndex, &attributeName);
    parser->GetAttributeValue(attributeIndex, &attriValue);

    switch (attributeName.GetChar(0)) {
        case 'e':
            if (!attributeName.Equals(ATTR_EQUALS)) {
                return NULL;
            }
            return new EqualsFilter(valueProvider, attriValue);
        case 'i':
            if (!attributeName.Equals(ATTR_IS_NULL)) {
                return NULL;
            }
            return new IsNullFilter(valueProvider, attriValue);
        case 's':
            if (!attributeName.Equals(ATTR_STARTS_WITH)) {
                return NULL;
            }
            return new StartsWithFilter(valueProvider, attriValue);
        case 'c':
            if (!attributeName.Equals(ATTR_CONTAINS)) {
                return NULL;
            }
            return new ContainsFilter(valueProvider, attriValue);
        case 'p':
            if (!attributeName.Equals(ATTR_PATTERN)) {
                return NULL;
            }
            return new PatternStringFilter(valueProvider, attriValue);
        case 'r':
            if (!attributeName.Equals(ATTR_REGEX)) {
                return NULL;
            }
            return new RegexFilter(valueProvider, attriValue);
    }
    return NULL;
}

ECode StringFilter::Matches(
    /* [in] */ IIntentFirewall* ifw,
    /* [in] */ IComponentName* resolvedComponent,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 callerUid,
    /* [in] */ Int32 callerPid,
    /* [in] */ const String& resolvedType,
    /* [in] */ Int32 receivingUid,
    /* [out] */ Boolean *ret)
{
    VALIDATE_NOT_NULL(ret)
    String value = mValueProvider->GetValue(resolvedComponent, intent, resolvedType);
    *ret = MatchesValue(value);
    return NOERROR;
}

} // Firewall
} // Server
} // Droid
} // Elastos
