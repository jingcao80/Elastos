#ifndef __ELASTOS_DROID_Server_Firewall_StringFilter_H__
#define __ELASTOS_DROID_Server_Firewall_StringFilter_H__

#include "_Elastos.Droid.Server.h"
#include <Elastos.Droid.Os.h>
#include "elastos/core/Object.h"
#include "elastos/utility/regex/Pattern.h"
#include "elastos/droid/internal/utility/XmlUtils.h"
#include "elastos/droid/server/firewall/FilterFactory.h"

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Utility::Regex::Pattern;
using Elastos::Utility::Regex::IPattern;
using Org::Xmlpull::V1::IXmlPullParser;
using Elastos::Utility::IArrayList;
using Elastos::Droid::Os::CPatternMatcher;
using Elastos::Droid::Os::IPatternMatcher;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Firewall {

class ValueProvider;
class EqualsFilter;
class ContainsFilter;
class StartsWithFilter;
class PatternStringFilter;
class RegexFilter;
class IsNullFilter;
class COMPONENT_ValueProvider;
class COMPONENT_NAME_ValueProvider;
class COMPONENT_PACKAGE_ValueProvider;
class ACTION_ValueProvider;
class DATA_ValueProvider;
class MIME_TYPE_ValueProvider;
class SCHEME_ValueProvider;
class SSP_ValueProvider;
class HOST_ValueProvider;
class PATH_ValueProvider;

class StringFilter
    : public Object
    , public IFilter
{
public:
    CAR_INTERFACE_DECL()

    StringFilter(
        /* [in] */ ValueProvider* valueProvider);

    //@Override
    CARAPI Matches(
        /* [in] */ IIntentFirewall* ifw,
        /* [in] */ IComponentName* resolvedComponent,
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 callerUid,
        /* [in] */ Int32 callerPid,
        /* [in] */ const String& resolvedType,
        /* [in] */ Int32 receivingUid,
        /* [out] */ Boolean *ret);

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
    static CARAPI_(AutoPtr<StringFilter>) ReadFromXml(
        /* [in] */ ValueProvider* valueProvider,
        /* [in] */ IXmlPullParser* parser);

    static CARAPI_(AutoPtr<StringFilter>) GetFilter(
        /* [in] */ ValueProvider* valueProvider,
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ Int32 attributeIndex);

    virtual CARAPI_(Boolean) MatchesValue(
        /* in */ const String& value) = 0;

public:
    static AutoPtr<COMPONENT_ValueProvider> COMPONENT;
    static AutoPtr<COMPONENT_NAME_ValueProvider> COMPONENT_NAME;
    static AutoPtr<COMPONENT_PACKAGE_ValueProvider> COMPONENT_PACKAGE;
    static AutoPtr<ACTION_ValueProvider> ACTION;
    static AutoPtr<DATA_ValueProvider> DATA;
    static AutoPtr<MIME_TYPE_ValueProvider> MIME_TYPE;
    static AutoPtr<SCHEME_ValueProvider> SCHEME;
    static AutoPtr<SSP_ValueProvider> SSP;
    static AutoPtr<HOST_ValueProvider> HOST;
    static AutoPtr<PATH_ValueProvider> PATH;

private:
    static const String ATTR_EQUALS;
    static const String ATTR_STARTS_WITH;
    static const String ATTR_CONTAINS;
    static const String ATTR_PATTERN;
    static const String ATTR_REGEX;
    static const String ATTR_IS_NULL;

    AutoPtr<ValueProvider> mValueProvider;
};

class ValueProvider
    : public FilterFactory
{
public:
    CARAPI_(AutoPtr<IFilter>) NewFilter(
        /* in */ IXmlPullParser* parser);

    virtual CARAPI_(String) GetValue(
        /* in */ IComponentName* resolvedComponent,
        /* in */ IIntent* intent,
        /* in */ const String& resolvedType) = 0;

    ValueProvider(
        /* in */ const String& tag);
};

class EqualsFilter
    : public StringFilter
{
public:
    EqualsFilter(
        /* in */ ValueProvider* valueProvider,
        /* in */ const String& attrValue);

    //@Override
    CARAPI_(Boolean) MatchesValue(
        /* in */ const String& value);

private:
    String mFilterValue;
};

class ContainsFilter
    : public StringFilter
{
public:
    ContainsFilter(
        /* in */ ValueProvider* valueProvider,
        /* in */ const String& attrValue);

    //@Override
    CARAPI_(Boolean) MatchesValue(
        /* in */ const String& value);

private:
    String mFilterValue;
};

class StartsWithFilter
    : public StringFilter
{
public:
    StartsWithFilter(
        /* in */ ValueProvider* valueProvider,
        /* in */ const String& attrValue);

    //@Override
    CARAPI_(Boolean) MatchesValue(
        /* in */ const String& value);

private:
    String mFilterValue;
};

class PatternStringFilter
    : public StringFilter
{
public:
    PatternStringFilter(
        /* in */ ValueProvider* valueProvider,
        /* in */ const String& attrValue);

    //@Override
    CARAPI_(Boolean) MatchesValue(
        /* in */ const String& value);

private:
     AutoPtr<IPatternMatcher> mPattern;
};

class RegexFilter
    : public StringFilter
{
public:
    RegexFilter(
        /* in */ ValueProvider* valueProvider,
        /* in */ const String& attrValue);

    //@Override
    CARAPI_(Boolean) MatchesValue(
        /* in */ const String& value);

private:
    AutoPtr<IPattern> mPattern;
};

class IsNullFilter
    : public StringFilter
{
public:
    IsNullFilter(
        /* in */ ValueProvider* valueProvider,
        /* in */ const String& attrValue);

    IsNullFilter(
        /* in */ ValueProvider* valueProvider,
        /* in */ Boolean isNull);

    //@Override
    CARAPI_(Boolean) MatchesValue(
        /* in */ const String& value);

private:
    Boolean mIsNull;
};

class COMPONENT_ValueProvider
    : public ValueProvider
{
public:
    COMPONENT_ValueProvider(
        /* in */ const String& tag)
        : ValueProvider(tag) {}

    CARAPI_(String) GetValue(
        /* in */ IComponentName* resolvedComponent,
        /* in */ IIntent* intent,
        /* in */ const String& resolvedType);
};

class COMPONENT_NAME_ValueProvider
    : public ValueProvider
{
public:
    COMPONENT_NAME_ValueProvider(
        /* in */ const String& tag)
        : ValueProvider(tag) {}

    CARAPI_(String) GetValue(
        /* in */ IComponentName* resolvedComponent,
        /* in */ IIntent* intent,
        /* in */ const String& resolvedType);
};

class COMPONENT_PACKAGE_ValueProvider
    : public ValueProvider
{
public:
    COMPONENT_PACKAGE_ValueProvider(
        /* in */ const String& tag)
        : ValueProvider(tag) {}

    CARAPI_(String) GetValue(
        /* in */ IComponentName* resolvedComponent,
        /* in */ IIntent* intent,
        /* in */ const String& resolvedType);
};

class ACTION_ValueProvider
    : public ValueProvider
{
public:
    ACTION_ValueProvider(
        /* in */ const String& tag)
        : ValueProvider(tag) {}

    CARAPI_(String) GetValue(
        /* in */ IComponentName* resolvedComponent,
        /* in */ IIntent* intent,
        /* in */ const String& resolvedType);
};

class DATA_ValueProvider
    : public ValueProvider
{
public:
    DATA_ValueProvider(
        /* in */ const String& tag)
        : ValueProvider(tag) {}

    CARAPI_(String) GetValue(
        /* in */ IComponentName* resolvedComponent,
        /* in */ IIntent* intent,
        /* in */ const String& resolvedType);
};

class MIME_TYPE_ValueProvider
    : public ValueProvider
{
public:
    MIME_TYPE_ValueProvider(
        /* in */ const String& tag)
        : ValueProvider(tag) {}

    CARAPI_(String) GetValue(
        /* in */ IComponentName* resolvedComponent,
        /* in */ IIntent* intent,
        /* in */ const String& resolvedType);
};

class SCHEME_ValueProvider
    : public ValueProvider
{
public:
    SCHEME_ValueProvider(
        /* in */ const String& tag)
        : ValueProvider(tag) {}

    CARAPI_(String) GetValue(
        /* in */ IComponentName* resolvedComponent,
        /* in */ IIntent* intent,
        /* in */ const String& resolvedType);
};

class SSP_ValueProvider
    : public ValueProvider
{
public:
    SSP_ValueProvider(
        /* in */ const String& tag)
        : ValueProvider(tag) {}

    CARAPI_(String) GetValue(
        /* in */ IComponentName* resolvedComponent,
        /* in */ IIntent* intent,
        /* in */ const String& resolvedType);
};

class HOST_ValueProvider
    : public ValueProvider
{
public:
    HOST_ValueProvider(
        /* in */ const String& tag)
        : ValueProvider(tag) {}

    CARAPI_(String) GetValue(
        /* in */ IComponentName* resolvedComponent,
        /* in */ IIntent* intent,
        /* in */ const String& resolvedType);
};

class PATH_ValueProvider
    : public ValueProvider
{
public:
    PATH_ValueProvider(
        /* in */ const String& tag)
        : ValueProvider(tag) {}

    CARAPI_(String) GetValue(
        /* in */ IComponentName* resolvedComponent,
        /* in */ IIntent* intent,
        /* in */ const String& resolvedType);
};

} // Firewall
} // Server
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_Server_Firewall_StringFilter_H__
