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

#ifndef __ORG_APACHE_HTTP_MESSAGE_BASICHEADERELEMENT_H_
#define __ORG_APACHE_HTTP_MESSAGE_BASICHEADERELEMENT_H_

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Elastos::Core::IArrayOf;
using Elastos::Core::ICloneable;
using Org::Apache::Http::IHeaderElement;
using Org::Apache::Http::INameValuePair;

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

/**
 * One element of an HTTP header's value.
 * <p>
 * Some HTTP headers (such as the set-cookie header) have values that
 * can be decomposed into multiple elements.  Such headers must be in the
 * following form:
 * </p>
 * <pre>
 * header  = [ element ] *( "," [ element ] )
 * element = name [ "=" [ value ] ] *( ";" [ param ] )
 * param   = name [ "=" [ value ] ]
 *
 * name    = token
 * value   = ( token | quoted-string )
 *
 * token         = 1*&lt;any char except "=", ",", ";", &lt;"&gt; and
 *                       white space&gt;
 * quoted-string = &lt;"&gt; *( text | quoted-char ) &lt;"&gt;
 * text          = any char except &lt;"&gt;
 * quoted-char   = "\" char
 * </pre>
 * <p>
 * Any amount of white space is allowed between any part of the
 * header, element or param and is ignored. A missing value in any
 * element or param will be stored as the empty {@link String};
 * if the "=" is also missing <var>null</var> will be stored instead.
 * </p>
 * <p>
 * This class represents an individual header element, containing
 * both a name/value pair (value may be <tt>null</tt>) and optionally
 * a set of additional parameters.
 * </p>
 *
 * @author <a href="mailto:bcholmes@interlog.com">B.C. Holmes</a>
 * @author <a href="mailto:jericho@thinkfree.com">Park, Sung-Gu</a>
 * @author <a href="mailto:mbowler@GargoyleSoftware.com">Mike Bowler</a>
 * @author <a href="mailto:oleg at ural.com">Oleg Kalnichevski</a>
 *
 *
 * <!-- empty lines above to avoid 'svn diff' context problems -->
 * @version $Revision: 604625 $ $Date: 2007-12-16 06:11:11 -0800 (Sun, 16 Dec 2007) $
 *
 * @since 4.0
 */
class BasicHeaderElement
    : public Object
    , public IHeaderElement
    , public ICloneable
{
public:
    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ const String& value,
        /* [in] */ IArrayOf* parameters);/*INameValuePair*/

    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ const String& value);

    CARAPI constructor();

    CAR_INTERFACE_DECL()

    virtual ~BasicHeaderElement() {}

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
     * Get parameters, if any.
     * The returned array is created for each invocation and can
     * be modified by the caller without affecting this header element.
     *
     * @return parameters as an array of {@link NameValuePair}s
     */
    CARAPI GetParameters(
        /* [out, callee] */ ArrayOf<INameValuePair*>** parameters);

    /**
     * Obtains the number of parameters.
     *
     * @return  the number of parameters
     */
    CARAPI GetParameterCount(
        /* [out] */ Int32* count);

    /**
     * Obtains the parameter with the given index.
     *
     * @param index     the index of the parameter, 0-based
     *
     * @return  the parameter with the given index
     */
    CARAPI GetParameter(
        /* [in] */ Int32 index,
        /* [out] */ INameValuePair** parameter);

    /**
     * Returns parameter with the given name, if found. Otherwise null
     * is returned
     *
     * @param name The name to search by.
     * @return NameValuePair parameter with the given name
     */
    CARAPI GetParameterByName(
        /* [in] */ const String& name,
        /* [out] */ INameValuePair** parameter);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* equals);

    CARAPI ToString(
        /* [out] */ String* string);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

protected:
    CARAPI_(void) CloneImpl(
        /* [in] */ BasicHeaderElement* obj);

private:
    String mName;
    String mValue;
    AutoPtr<IArrayOf> mParameters;
};

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_MESSAGE_BASICHEADERELEMENT_H_
