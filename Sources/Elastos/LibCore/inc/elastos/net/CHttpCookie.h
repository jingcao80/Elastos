
#ifndef __ELASTOS_NET_CHTTPCOOKIE_H__
#define __ELASTOS_NET_CHTTPCOOKIE_H__

#include "_Elastos_Net_CHttpCookie.h"
#include "InetAddress.h"
#include "StringBuilder.h"
#include <elastos/utility/etl/List.h>
#include <elastos/utility/etl/HashSet.h>

using Elastos::Core::Object;
using Elastos::Core::ICloneable;
using Elastos::Core::StringBuilder;
using Elastos::Utility::IDate;
using Elastos::Utility::IList;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::HashSet;

namespace Elastos {
namespace Net {

CarClass(CHttpCookie)
    , public Object
    , public IHttpCookie
    , public ICloneable
{
public:
    class CookieParser : public ElRefBase
    {
    public:
        CookieParser(
            /* [in] */ const String& input);

        CARAPI Parse(
            /* [out] */ IList** httpCookies);

    private:
        CARAPI SetAttribute(
            /* [in] */ CHttpCookie* cookie,
            /* [in] */ const String& name,
            /* [in] */ const String& value);

        CARAPI_(String) ReadAttributeName(
            /* [in] */ Boolean returnLowerCase);

        CARAPI_(Boolean) ReadEqualsSign();

        CARAPI ReadAttributeValue(
            /* [in] */ const String& terminators,
            /* [out] */ String* value);

        CARAPI_(Int32) Find(
            /* [in] */ const String& chars);

        CARAPI_(void) SkipWhitespace();

    public:
        Boolean mHasExpires;
        Boolean mHasMaxAge;
        Boolean mHasVersion;

    private:
        static const String ATTRIBUTE_NAME_TERMINATORS;
        static const String WHITESPACE;
        String mInput;
        String mInputLowerCase;
        Int32 mPos;
    };

public:
    static CARAPI StaticInit();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CHttpCookie();

    static CARAPI_(Boolean) IsDomainMatches(
        /* [in] */ const String& domainPattern,
        /* [in] */ const String& host);

    static CARAPI_(Boolean) PathMatches(
        /* [in] */ IHttpCookie* cookie,
        /* [in] */ IURI* uri);

    static CARAPI_(Boolean) SecureMatches(
        /* [in] */ IHttpCookie* cookie,
        /* [in] */ IURI* uri);

    static CARAPI_(Boolean) PortMatches(
        /* [in] */ IHttpCookie* cookie,
        /* [in] */ IURI* uri);

    static CARAPI Parse(
        /* [in] */ const String& header,
        /* [out] */ IList** httpCookies);

    CARAPI GetComment(
        /* [out] */ String* comment);

    CARAPI GetCommentURL(
        /* [out] */ String* url);

    CARAPI GetDiscard(
        /* [out] */ Boolean* discard);

    CARAPI GetDomainAttr(
        /* [out] */ String* theDomain);

    CARAPI GetMaxAge(
        /* [out] */ Int64* maxAge);

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI GetPath(
        /* [out] */ String* path);

    CARAPI GetPortList(
        /* [out] */ String* portList);

    CARAPI GetSecure(
        /* [out] */ Boolean* secure);

    CARAPI GetValue(
        /* [out] */ String* value);

    CARAPI GetVersion(
        /* [out] */ Int32* theVersion);

    CARAPI HasExpired(
        /* [out] */ Boolean* expired);

    CARAPI SetComment(
        /* [in] */ const String& comment);

    CARAPI SetCommentURL(
        /* [in] */ const String& commentURL);

    CARAPI SetDiscard(
        /* [in] */ Boolean discard);

    CARAPI SetDomain(
        /* [in] */ const String& pattern);

    CARAPI SetMaxAge(
        /* [in] */ Int64 deltaSeconds);

    CARAPI SetPath(
        /* [in] */ const String& path);

    CARAPI SetPortList(
        /* [in] */ const String& portList);

    CARAPI SetSecure(
        /* [in] */ Boolean secure);

    CARAPI SetValue(
        /* [in] */ const String& value);

    CARAPI SetVersion(
        /* [in] */ Int32 v);

    CARAPI_(String) ToString();

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI Clone(
        /* [out] */ IInterface** result);

    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ const String& value);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */Boolean* result);
private:
    static CARAPI_(String) MatchablePath(
        /* [in] */ const String& path);

    static CARAPI_(Boolean) IsFullyQualifiedDomainName(
        /* [in] */ const String& s,
        /* [in] */ Int32 firstCharacter);

    CARAPI_(Boolean) IsValidName(
        /* [in] */ const String& n);

    CARAPI_(void) SetExpires(
        /* [in] */ IDate* expires);

    CARAPI_(void) AppendAttribute(
        /* [in] */ StringBuilder& builder,
        /* [in] */ const String& name,
        /* [in] */ const String& value);

private:

    static AutoPtr< HashSet<String> > RESERVED_NAMES;

    String mComment;
    String mCommentURL;
    Boolean mDiscard;
    String mDomain;
    Int64 mMaxAge;
    String mName;
    String mPath;
    String mPortList;
    Boolean mSecure;
    Boolean mHttpOnly;
    String mValue;
    Int32 mVersion;
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_CHTTPCOOKIE_H__
