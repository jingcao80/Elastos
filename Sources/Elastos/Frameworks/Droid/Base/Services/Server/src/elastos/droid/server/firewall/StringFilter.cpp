#include "elastos/droid/server/firewall/StringFilter.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Firewall {

//------------------------------------------------------------------------------
// StringFilter::ValueProvider
//------------------------------------------------------------------------------

StringFilter::ValueProvider::ValueProvider(
    /* in */ const String& tag)
    : FilterFactory(tag)
{}

IFilter* StringFilter::ValueProvider::NewFilter(
    /* in */ IXmlPullParser* parser)
{
    AutoPtr<StringFilter> stringFilter = StringFilter::ReadFromXml(this, parser);
    REFCOUNT_ADD(stringFilter);

    return (IFilter*)StringFilter;
}

//------------------------------------------------------------------------------
// StringFilter::EqualsFilter
//------------------------------------------------------------------------------

StringFilter::EqualsFilter::EqualsFilter(
    /* in */ ValueProvider* valueProvider,
    /* in */ const String& attrValue)
    : StringFilter(valueProvider), mFilterValue(attrValue)
{}

Boolean StringFilter::EqualsFilter::MatchesValue(
    /* in */ const String& value)
{
    return value != NULL && value.Equals(mFilterValue);
}

//------------------------------------------------------------------------------
// StringFilter::ContainsFilter
//------------------------------------------------------------------------------

StringFilter::ContainsFilter::ContainsFilter(
    /* in */ ValueProvider* valueProvider,
    /* in */ const String& attrValue)
    : StringFilter(valueProvider), mFilterValue(attrValue)
{}

Boolean StringFilter::ContainsFilter::MatchesValue(
    /* in */ const String& value)
{
    return value != NULL && value.Contains(mFilterValue);
}

//------------------------------------------------------------------------------
// StringFilter::StartsWithFilter
//------------------------------------------------------------------------------

StringFilter::StartsWithFilter::StartsWithFilter(
    /* in */ ValueProvider* valueProvider,
    /* in */ const String& attrValue)
    : StringFilter(valueProvider), mFilterValue(attrValue)
{}

Boolean StringFilter::StartsWithFilter::MatchesValue(
    /* in */ const String& value)
{
    return value != NULL && value.StartsWith(mFilterValue);
}

//------------------------------------------------------------------------------
// StringFilter::PatternStringFilter
//------------------------------------------------------------------------------

StringFilter::PatternStringFilter::PatternStringFilter(
    /* in */ ValueProvider* valueProvider,
    /* in */ const String& attrValue)
    : StringFilter(valueProvider)
{
    mPattern = new PatternMatcher(attrValue, PatternMatcher::PATTERN_SIMPLE_GLOB);
}

Boolean StringFilter::PatternStringFilter::MatchesValue(
    /* in */ const String& value)
{
    return value != NULL && value.StartsWith(mFilterValue);
}

//------------------------------------------------------------------------------
// StringFilter::PatternStringFilter
//------------------------------------------------------------------------------

StringFilter::RegexFilter::RegexFilter(
    /* in */ ValueProvider* valueProvider,
    /* in */ const String& attrValue)
    : StringFilter(valueProvider)
{
    mPattern = Pattern::Compile(attrValue);
}

Boolean StringFilter::RegexFilter::MatchesValue(
    /* in */ const String& value)
{
    return value != NULL && mPattern->Matcher(value)->Matches();
}

//------------------------------------------------------------------------------
// StringFilter::PatternStringFilter
//------------------------------------------------------------------------------

StringFilter::IsNullFilter::IsNullFilter(
    /* in */ ValueProvider* valueProvider,
    /* in */ const String& attrValue)
    : StringFilter(valueProvider)
{
     mIsNull = StringUtils::ParseBoolean(attrValue);
}

StringFilter::IsNullFilter::IsNullFilter(
    /* in */ ValueProvider* valueProvider,
    /* in */ Boolean isNull)
    : StringFilter(valueProvider), mIsNull(isNull)
{}


Boolean StringFilter::IsNullFilter::MatchesValue(
    /* in */ const String& value)
{
    return (value == NULL) == mIsNull;
}

//------------------------------------------------------------------------------
// StringFilter::COMPONENT_ValueProvider
//------------------------------------------------------------------------------

String StringFilter::COMPONENT_ValueProvider::GetValue(
    /* in */ IComponentName* resolvedComponent,
    /* in */ IIntent* intent,
    /* in */ const String& resolvedType)
{
    if (resolvedComponent != NULL) {
        String str;
        resolvedComponent.FlattenToString(&str);
        return str;
    }
    return NULL;
}

//------------------------------------------------------------------------------
// StringFilter::COMPONENT_NAME_ValueProvider
//------------------------------------------------------------------------------

String StringFilter::COMPONENT_NAME_ValueProvider::GetValue(
    /* in */ IComponentName* resolvedComponent,
    /* in */ IIntent* intent,
    /* in */ const String& resolvedType)
{
    if (resolvedComponent != NULL) {
        String str;
        resolvedComponent.GetClassName(&str);
        return str;
    }
    return NULL;
}

//------------------------------------------------------------------------------
// StringFilter::COMPONENT_PACKAGE_ValueProvider
//------------------------------------------------------------------------------

String StringFilter::COMPONENT_PACKAGE_ValueProvider::GetValue(
    /* in */ IComponentName* resolvedComponent,
    /* in */ IIntent* intent,
    /* in */ const String& resolvedType)
{
    if (resolvedComponent != NULL) {
        String str;
        resolvedComponent.GetPackageName(&str);
        return str;
    }
    return NULL;
}

//------------------------------------------------------------------------------
// StringFilter::ACTION_ValueProvider
//------------------------------------------------------------------------------

String StringFilter::ACTION_ValueProvider::GetValue(
    /* in */ IComponentName* resolvedComponent,
    /* in */ IIntent* intent,
    /* in */ const String& resolvedType)
{
    String str;
    intent->GetAction(&str);
    return str;
}

//------------------------------------------------------------------------------
// StringFilter::DATA_ValueProvider
//------------------------------------------------------------------------------

String StringFilter::DATA_ValueProvider::GetValue(
    /* in */ IComponentName* resolvedComponent,
    /* in */ IIntent* intent,
    /* in */ const String& resolvedType)
{
    AutoPtr<IUri> data;
    intent->GetData(&data);
    if (data != NULL) {
        return TO_STR(data);
    }
    return NULL;
}

//------------------------------------------------------------------------------
// StringFilter::MIME_TYPE_ValueProvider
//------------------------------------------------------------------------------

String StringFilter::MIME_TYPE_ValueProvider::GetValue(
    /* in */ IComponentName* resolvedComponent,
    /* in */ IIntent* intent,
    /* in */ const String& resolvedType)
{
    return resolvedType;
}

//------------------------------------------------------------------------------
// StringFilter::SCHEME_ValueProvider
//------------------------------------------------------------------------------

String StringFilter::SCHEME_ValueProvider::GetValue(
    /* in */ IComponentName* resolvedComponent,
    /* in */ IIntent* intent,
    /* in */ const String& resolvedType)
{
    AutoPtr<IUri> data;
    intent->GetData(&data);
    if (data != NULL) {
        String str;
        data->GetScheme(&str);
        return str;
    }
    return NULL;
}

//------------------------------------------------------------------------------
// StringFilter::SSP_ValueProvider
//------------------------------------------------------------------------------

String StringFilter::SSP_ValueProvider::GetValue(
    /* in */ IComponentName* resolvedComponent,
    /* in */ IIntent* intent,
    /* in */ const String& resolvedType)
{
    AutoPtr<IUri> data;
    intent->GetData(&data);
    if (data != NULL) {
        String str;
        data->GetSchemeSpecificPart(&str);
        return str;
    }
    return NULL;
}

//------------------------------------------------------------------------------
// StringFilter::HOST_ValueProvider
//------------------------------------------------------------------------------

String StringFilter::HOST_ValueProvider::GetValue(
    /* in */ IComponentName* resolvedComponent,
    /* in */ IIntent* intent,
    /* in */ const String& resolvedType)
{
    AutoPtr<IUri> data;
    intent->GetData(&data);
    if (data != NULL) {
        String str;
        data->GetHost(&str);
        return str;
    }
    return NULL;
}

//------------------------------------------------------------------------------
// StringFilter::PATH_ValueProvider
//------------------------------------------------------------------------------

String StringFilter::PATH_ValueProvider::GetValue(
    /* in */ IComponentName* resolvedComponent,
    /* in */ IIntent* intent,
    /* in */ const String& resolvedType)
{
    AutoPtr<IUri> data;
    intent->GetData(&data);
    if (data != NULL) {
        String str;
        data->GetPath(&str);
        return str;
    }
    return NULL;
}

//=======================================================================================
// StringFilter
//=======================================================================================

String StringFilter::ATTR_EQUALS = "equals";
String StringFilter::ATTR_STARTS_WITH = "startsWith";
String StringFilter::ATTR_CONTAINS = "contains";
String StringFilter::ATTR_PATTERN = "pattern";
String StringFilter::ATTR_REGEX = "regex";
String StringFilter::ATTR_IS_NULL = "isNull";

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
    parser->GetAttributeName(attributeIndex, &attributeName);

    switch (attributeName.GetChar(0)) {
        case 'e':
            if (!attributeName.Equals(ATTR_EQUALS)) {
                return NULL;
            }
            return new EqualsFilter(valueProvider, parser.getAttributeValue(attributeIndex));
        case 'i':
            if (!attributeName.Equals(ATTR_IS_NULL)) {
                return NULL;
            }
            return new IsNullFilter(valueProvider, parser.getAttributeValue(attributeIndex));
        case 's':
            if (!attributeName.Equals(ATTR_STARTS_WITH)) {
                return NULL;
            }
            return new StartsWithFilter(valueProvider,
                    parser.getAttributeValue(attributeIndex));
        case 'c':
            if (!attributeName.Equals(ATTR_CONTAINS)) {
                return NULL;
            }
            return new ContainsFilter(valueProvider, parser.getAttributeValue(attributeIndex));
        case 'p':
            if (!attributeName.Equals(ATTR_PATTERN)) {
                return NULL;
            }
            return new PatternStringFilter(valueProvider,
                    parser.getAttributeValue(attributeIndex));
        case 'r':
            if (!attributeName.Equals(ATTR_REGEX)) {
                return NULL;
            }
            return new RegexFilter(valueProvider, parser.getAttributeValue(attributeIndex));
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
    /* [in] */ Int32 receivingUid
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
