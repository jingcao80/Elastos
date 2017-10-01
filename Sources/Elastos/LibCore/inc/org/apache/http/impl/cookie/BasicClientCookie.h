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

#ifndef __ORG_APACHE_HTTP_IMPL_COOKIE_BASICCLIENTCOOKIE_H__
#define __ORG_APACHE_HTTP_IMPL_COOKIE_BASICCLIENTCOOKIE_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Elastos::Core::ICloneable;
using Elastos::Utility::IMap;
using Elastos::Utility::IDate;
using Org::Apache::Http::Cookie::ISetCookie;
using Org::Apache::Http::Cookie::IClientCookie;
using Org::Apache::Http::Cookie::ICookie;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Cookie {

/**
 * HTTP "magic-cookie" represents a piece of state information
 * that the HTTP agent and the target server can exchange to maintain
 * a session.
 *
 * @author B.C. Holmes
 * @author <a href="mailto:jericho@thinkfree.com">Park, Sung-Gu</a>
 * @author <a href="mailto:dsale@us.britannica.com">Doug Sale</a>
 * @author Rod Waldhoff
 * @author dIon Gillard
 * @author Sean C. Sullivan
 * @author <a href="mailto:JEvans@Cyveillance.com">John Evans</a>
 * @author Marc A. Saegesser
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 * @author <a href="mailto:mbowler@GargoyleSoftware.com">Mike Bowler</a>
 *
 * @version $Revision: 659191 $
 */
class BasicClientCookie
    : public Object
    , public ISetCookie
    , public IClientCookie
    , public ICookie
    , public ICloneable
{
public:
    BasicClientCookie();

    /**
     * Default Constructor taking a name and a value. The value may be null.
     *
     * @param name The name.
     * @param value The value.
     */
    BasicClientCookie(
        /* [in] */ const String& name,
        /* [in] */ const String& value);

    virtual ~BasicClientCookie() {}

    CAR_INTERFACE_DECL()

    /**
     * Returns the name.
     *
     * @return String name The name
     */
    CARAPI GetName(
        /* [out] */ String* name);

    /**
     * Returns the value.
     *
     * @return String value The current value.
     */
    CARAPI GetValue(
        /* [out] */ String* value);

    /**
     * Sets the value
     *
     * @param value
     */
    CARAPI SetValue(
        /* [in] */ const String& value);

    /**
     * Returns the comment describing the purpose of this cookie, or
     * <tt>null</tt> if no such comment has been defined.
     *
     * @return comment
     */
    CARAPI GetComment(
        /* [out] */ String* comment);

    /**
     * If a user agent (web browser) presents this cookie to a user, the
     * cookie's purpose will be described using this comment.
     *
     * @param comment
     *
     * @see #getComment()
     */
    CARAPI SetComment(
        /* [in] */ const String& comment);

    /**
     * Returns null. Cookies prior to RFC2965 do not set this attribute
     */
    CARAPI GetCommentURL(
        /* [out] */ String* commentURL);

    /**
     * Returns the expiration {@link Date} of the cookie, or <tt>null</tt>
     * if none exists.
     * <p><strong>Note:</strong> the object returned by this method is
     * considered immutable. Changing it (e.g. using setTime()) could result
     * in undefined behaviour. Do so at your peril. </p>
     * @return Expiration {@link Date}, or <tt>null</tt>.
     */
    CARAPI GetExpiryDate(
        /* [out] */ IDate** date);

    /**
     * Sets expiration date.
     * <p><strong>Note:</strong> the object returned by this method is considered
     * immutable. Changing it (e.g. using setTime()) could result in undefined
     * behaviour. Do so at your peril.</p>
     *
     * @param expiryDate the {@link Date} after which this cookie is no longer valid.
     *
     * @see Cookie#getExpiryDate
     *
     */
    CARAPI SetExpiryDate(
        /* [in] */ IDate* expiryDate);

    /**
     * Returns <tt>false</tt> if the cookie should be discarded at the end
     * of the "session"; <tt>true</tt> otherwise.
     *
     * @return <tt>false</tt> if the cookie should be discarded at the end
     *         of the "session"; <tt>true</tt> otherwise
     */
    CARAPI IsPersistent(
        /* [out] */ Boolean* isPersistent);

    using Object::GetDomain;

    /**
     * Returns domain attribute of the cookie.
     *
     * @return the value of the domain attribute
     *
     * @see #setDomain(java.lang.String)
     */
    CARAPI GetDomain(
        /* [out] */ String* domain);

    /**
     * Sets the domain attribute.
     *
     * @param domain The value of the domain attribute
     *
     * @see Cookie#getDomain
     */
    CARAPI SetDomain(
        /* [in] */ const String& domain);

    /**
     * Returns the path attribute of the cookie
     *
     * @return The value of the path attribute.
     *
     * @see #setPath(java.lang.String)
     */
    CARAPI GetPath(
        /* [out] */ String* path);

    /**
     * Sets the path attribute.
     *
     * @param path The value of the path attribute
     *
     * @see Cookie#getPath
     *
     */
    CARAPI SetPath(
        /* [in] */ const String& path);

    /**
     * @return <code>true</code> if this cookie should only be sent over secure connections.
     * @see #setSecure(boolean)
     */
    CARAPI IsSecure(
        /* [out] */ Boolean* isSecure);

    /**
     * Sets the secure attribute of the cookie.
     * <p>
     * When <tt>true</tt> the cookie should only be sent
     * using a secure protocol (https).  This should only be set when
     * the cookie's originating server used a secure protocol to set the
     * cookie's value.
     *
     * @param secure The value of the secure attribute
     *
     * @see #isSecure()
     */
    CARAPI SetSecure(
        /* [in] */ Boolean secure);

    /**
     * Returns null. Cookies prior to RFC2965 do not set this attribute
     */
    CARAPI GetPorts(
        /* [out, callee] */ ArrayOf<Int32>** ports);

    /**
     * Returns the version of the cookie specification to which this
     * cookie conforms.
     *
     * @return the version of the cookie.
     *
     * @see #setVersion(int)
     *
     */
    CARAPI GetVersion(
        /* [out] */ Int32* version);

    /**
     * Sets the version of the cookie specification to which this
     * cookie conforms.
     *
     * @param version the version of the cookie.
     *
     * @see Cookie#getVersion
     */
    CARAPI SetVersion(
        /* [in] */ Int32 version);

    /**
     * Returns true if this cookie has expired.
     * @param date Current time
     *
     * @return <tt>true</tt> if the cookie has expired.
     */
    CARAPI IsExpired(
        /* [in] */ IDate* date,
        /* [out] */ Boolean* isExpired);

    CARAPI SetAttribute(
        /* [in] */ const String& name,
        /* [in] */ const String& value);

    CARAPI GetAttribute(
        /* [in] */ const String& name,
        /* [out] */ String* attr);

    CARAPI ContainsAttribute(
        /* [in] */ const String& name,
        /* [out] */ Boolean* result);

    CARAPI Clone(
        /* [out] */ IInterface** object);

    CARAPI ToString(
        /* [out] */ String* str);

protected:
    CARAPI CloneImpl(
        /* [in] */ BasicClientCookie* cookie);

private:
    /** Cookie name */
    String mName;

    /** Cookie attributes as specified by the origin server */
    AutoPtr<IMap> mAttribs;

    /** Cookie value */
    String mValue;

    /** Comment attribute. */
    String mCookieComment;

    /** Domain attribute. */
    String mCookieDomain;

    /** Expiration {@link Date}. */
    AutoPtr<IDate> mCookieExpiryDate;

    /** Path attribute. */
    String mCookiePath;

    /** My secure flag. */
    Boolean mIsSecure;

    /** The version of the cookie specification I was created from. */
    Int32 mCookieVersion;
};

} // namespace Cookie
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_COOKIE_BASICCLIENTCOOKIE_H__
