#include "elastos/droid/server/firewall/StringFilter.h"
#include <elastos/core/Object.h>
#include <elastos/core/StringUtils.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"

using Elastos::Core::StringUtils;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Net::IUri;
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
    /* in */ const String& tag)
{
    FilterFactory::constructor(tag);
}

IFilter* ValueProvider::NewFilter(
    /* in */ IXmlPullParser* parser)
{
    AutoPtr<StringFilter> stringFilter = StringFilter::ReadFromXml(this, parser);
    REFCOUNT_ADD(stringFilter);

    return (IFilter*)stringFilter;
}

//------------------------------------------------------------------------------
// EqualsFilter
//------------------------------------------------------------------------------

EqualsFilter::EqualsFilter(
    /* in */ ValueProvider* valueProvider,
    /* in */ const String& attrValue)
    : StringFilter(valueProvider), mFilterValue(attrValue)
{}

Boolean EqualsFilter::MatchesValue(
    /* in */ const String& value)
{
    return value != NULL && value.Equals(mFilterValue);
}

//------------------------------------------------------------------------------
// ContainsFilter
//------------------------------------------------------------------------------

ContainsFilter::ContainsFilter(
    /* in */ ValueProvider* valueProvider,
    /* in */ const String& attrValue)
    : StringFilter(valueProvider), mFilterValue(attrValue)
{}

Boolean ContainsFilter::MatchesValue(
    /* in */ const String& value)
{
    return value != NULL && value.Contains(mFilterValue);
}

//------------------------------------------------------------------------------
// StartsWithFilter
//------------------------------------------------------------------------------

StartsWithFilter::StartsWithFilter(
    /* in */ ValueProvider* valueProvider,
    /* in */ const String& attrValue)
    : StringFilter(valueProvider), mFilterValue(attrValue)
{}

Boolean StartsWithFilter::MatchesValue(
    /* in */ const String& value)
{
    return value != NULL && value.StartWith(mFilterValue);
}

//------------------------------------------------------------------------------
// PatternStringFilter
//------------------------------------------------------------------------------

PatternStringFilter::PatternStringFilter(
    /* in */ ValueProvider* valueProvider,
    /* in */ const String& attrValue)
    : StringFilter(valueProvider)
{
    CPatternMatcher::New(attrValue, IPatternMatcher::PATTERN_SIMPLE_GLOB, (IPatternMatcher**)&mPattern);
}

Boolean PatternStringFilter::MatchesValue(
    /* in */ const String& value)
{
    Boolean b;

    mPattern->Match(value, &b);
    return value != NULL && b;
}

//------------------------------------------------------------------------------
// PatternStringFilter
//------------------------------------------------------------------------------

RegexFilter::RegexFilter(
    /* in */ ValueProvider* valueProvider,
    /* in */ const String& attrValue)
    : StringFilter(valueProvider)
{
    Pattern::Compile(attrValue, (IPattern**)&mPattern);
}

Boolean RegexFilter::MatchesValue(
    /* in */ const String& value)
{
    AutoPtr<IMatcher> matcher;
    Boolean matched;

    mPattern->Matcher(value, (IMatcher**)&matcher);
    matcher->Matches(&matched);
    return value != NULL && matched;
}

//------------------------------------------------------------------------------
// PatternStringFilter
//------------------------------------------------------------------------------

IsNullFilter::IsNullFilter(
    /* in */ ValueProvider* valueProvider,
    /* in */ const String& attrValue)
    : StringFilter(valueProvider)
{
     mIsNull = StringUtils::ParseBoolean(attrValue);
}

IsNullFilter::IsNullFilter(
    /* in */ ValueProvider* valueProvider,
    /* in */ Boolean isNull)
    : StringFilter(valueProvider), mIsNull(isNull)
{}


Boolean IsNullFilter::MatchesValue(
    /* in */ const String& value)
{
    return (value == NULL) == mIsNull;
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

CAR_INTERFACE_IMPL(StringFilter, Object, IFilter);

StringFilter::StringFilter(
    /* [in] */ ValueProvider* valueProvider)
    : mValueProvider(valueProvider)
{}


StringFilter* StringFilter::ReadFromXml(
    /* [in] */ ValueProvider* valueProvider,
    /* [in] */ IXmlPullParser* parser)
{
    AutoPtr<StringFilter> filter;
    Int32 count;

    parser->GetAttributeCount(&count);
    for (Int32 i = 0;  i < count;  i++) {
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
    REFCOUNT_ADD(filter);

    return filter;
}

StringFilter* StringFilter::GetFilter(
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
    String value = mValueProvider->GetValue(resolvedComponent, intent, resolvedType);
    *ret = MatchesValue(value);

    return NOERROR;
}

} // Firewall
} // Server
} // Droid
} // Elastos
