#ifndef __ELASTOS_DROID_Server_Firewall_StringFilter_H__
#define __ELASTOS_DROID_Server_Firewall_StringFilter_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/core/Object.h"
#include "elastos/droid/os/PatternMatcher.h"
#include "elastos/utility/regex/Pattern.h"

using Elastos::Droid::internal.util.XmlUtils;
using Elastos::Droid::Os::PatternMatcher;
using Elastos::Utility::Regex::Pattern;
using Org::Xmlpull::V1::IXmlPullParser;
using Org::Xmlpull::V1::IXmlPullParserException;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Firewall {

class StringFilter
    : public Object
    , public IFilter;
{
public:
    class ValueProvider
        : public FilterFactory
    {
    public:
        CARAPI_(IFilter*) NewFilter(
            /* in */ IXmlPullParser* parser);

        virtual String GetValue(
            /* in */ IComponentName* resolvedComponent,
            /* in */ IIntent* intent,
            /* in */ const String& resolvedType);

    protected:
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
         AutoPtr<PatternMatcher> mPattern;
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
        AutoPtr<Pattern> mPattern;
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
        CARAPI_(String) GetValue(
            /* in */ IComponentName* resolvedComponent,
            /* in */ IIntent* intent,
            /* in */ const String& resolvedType);
    };


    class COMPONENT_NAME_ValueProvider
        : public ValueProvider
    {
    public:
        CARAPI_(String) GetValue(
            /* in */ IComponentName* resolvedComponent,
            /* in */ IIntent* intent,
            /* in */ const String& resolvedType);
    };

    class COMPONENT_PACKAGE_ValueProvider
        : public ValueProvider
    {
    public:
        CARAPI_(String) GetValue(
            /* in */ IComponentName* resolvedComponent,
            /* in */ IIntent* intent,
            /* in */ const String& resolvedType);
    };

    class ACTION_ValueProvider
        : public ValueProvider
    {
    public:
        CARAPI_(String) GetValue(
            /* in */ IComponentName* resolvedComponent,
            /* in */ IIntent* intent,
            /* in */ const String& resolvedType);
    };

    class DATA_ValueProvider
        : public ValueProvider
    {
    public:
        CARAPI_(String) GetValue(
            /* in */ IComponentName* resolvedComponent,
            /* in */ IIntent* intent,
            /* in */ const String& resolvedType);
    };

    class MIME_TYPE_ValueProvider
        : public ValueProvider
    {
    public:
        CARAPI_(String) GetValue(
            /* in */ IComponentName* resolvedComponent,
            /* in */ IIntent* intent,
            /* in */ const String& resolvedType);
    };

    class SCHEME_ValueProvider
        : public ValueProvider
    {
    public:
        CARAPI_(String) GetValue(
            /* in */ IComponentName* resolvedComponent,
            /* in */ IIntent* intent,
            /* in */ const String& resolvedType);
    };

    class SSP_ValueProvider
        : public ValueProvider
    {
    public:
        CARAPI_(String) GetValue(
            /* in */ IComponentName* resolvedComponent,
            /* in */ IIntent* intent,
            /* in */ const String& resolvedType);
    };

    class HOST_ValueProvider
        : public ValueProvider
    {
    public:
        CARAPI_(String) GetValue(
            /* in */ IComponentName* resolvedComponent,
            /* in */ IIntent* intent,
            /* in */ const String& resolvedType);
    };

    class PATH_ValueProvider
        : public ValueProvider
    {
    public:
        CARAPI_(String) GetValue(
            /* in */ IComponentName* resolvedComponent,
            /* in */ IIntent* intent,
            /* in */ const String& resolvedType);
    };

public:
    //@Override
    CARAPI Matches(
        /* [in] */ IIntentFirewall* ifw,
        /* [in] */ IComponentName* resolvedComponent,
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 callerUid,
        /* [in] */ Int32 callerPid,
        /* [in] */ const String& resolvedType,
        /* [in] */ Int32 receivingUid
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
    static StringFilter* ReadFromXml(
        /* [in] */ ValueProvider* valueProvider,
        /* [in] */ IXmlPullParser* parser);

    static StringFilter* GetFilter(
        /* [in] */ ValueProvider* valueProvider,
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ Int32 attributeIndex);

private:
    StringFilter(
        /* [in] */ ValueProvider* valueProvider);

private:
    static const String ATTR_EQUALS;        // = "equals";
    static const String ATTR_STARTS_WITH;   // = "startsWith";
    static const String ATTR_CONTAINS;      // = "contains";
    static const String ATTR_PATTERN;       // = "pattern";
    static const String ATTR_REGEX;         // = "regex";
    static const String ATTR_IS_NULL;       // = "isNull";

    AutoPtr<ValueProvider> mValueProvider;
};

} // Firewall
} // Server
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_Server_Firewall_StringFilter_H__
