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

#ifndef __ORG_APACHE_HTTP_MESSAGE_CHEADERGROUP_H_
#define __ORG_APACHE_HTTP_MESSAGE_CHEADERGROUP_H_

#include "Elastos.CoreLibrary.Utility.h"
#include "_Org_Apache_Http_Message_CHeaderGroup.h"
#include "org/apache/http/message/BasicLineFormatter.h"

using Elastos::Core::ICloneable;
using Elastos::Utility::IList;
using Org::Apache::Http::IHeaderIterator;
using Org::Apache::Http::IHeader;

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

/**
 * A class for combining a set of headers.
 * This class allows for multiple headers with the same name and
 * keeps track of the order in which headers were added.
 *
 * @author Michael Becke
 *
 * @since 4.0
 */
CarClass(CHeaderGroup)
    , public Object
    , public IHeaderGroup
    , public ICloneable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Removes any contained headers.
     */
    CARAPI Clear();

    /**
     * Adds the given header to the group.  The order in which this header was
     * added is preserved.
     *
     * @param header the header to add
     */
    CARAPI AddHeader(
        /* [in] */ IHeader* header);

    /**
     * Removes the given header.
     *
     * @param header the header to remove
     */
    CARAPI RemoveHeader(
        /* [in] */ IHeader* header);

    /**
     * Replaces the first occurence of the header with the same name. If no header with
     * the same name is found the given header is added to the end of the list.
     *
     * @param header the new header that should replace the first header with the same
     * name if present in the list.
     */
    CARAPI UpdateHeader(
        /* [in] */ IHeader* header);

    /**
     * Sets all of the headers contained within this group overriding any
     * existing headers. The headers are added in the order in which they appear
     * in the array.
     *
     * @param headers the headers to set
     */
    CARAPI SetHeaders(
        /* [in] */ ArrayOf<IHeader*>* headers);

    /**
     * Gets a header representing all of the header values with the given name.
     * If more that one header with the given name exists the values will be
     * combined with a "," as per RFC 2616.
     *
     * <p>Header name comparison is case insensitive.
     *
     * @param name the name of the header(s) to get
     * @return a header with a condensed value or <code>null</code> if no
     * headers by the given name are present
     */
    CARAPI GetCondensedHeader(
        /* [in] */ const String& name,
        /* [out] */ IHeader** condensedHeader);

    /**
     * Gets all of the headers with the given name.  The returned array
     * maintains the relative order in which the headers were added.
     *
     * <p>Header name comparison is case insensitive.
     *
     * @param name the name of the header(s) to get
     *
     * @return an array of length >= 0
     */
    CARAPI GetHeaders(
        /* [in] */ const String& name,
        /* [out, callee] */ ArrayOf<IHeader*>** headers);

    /**
     * Gets the first header with the given name.
     *
     * <p>Header name comparison is case insensitive.
     *
     * @param name the name of the header to get
     * @return the first header or <code>null</code>
     */
    CARAPI GetFirstHeader(
        /* [in] */ const String& name,
        /* [out] */ IHeader** firstHeader);

    /**
     * Gets the last header with the given name.
     *
     * <p>Header name comparison is case insensitive.
     *
     * @param name the name of the header to get
     * @return the last header or <code>null</code>
     */
    CARAPI GetLastHeader(
        /* [in] */ const String& name,
        /* [out] */ IHeader** lastHeader);

    /**
     * Gets all of the headers contained within this group.
     *
     * @return an array of length >= 0
     */
    CARAPI GetAllHeaders(
        /* [out, callee] */ ArrayOf<IHeader*>** allHeaders);

    /**
     * Tests if headers with the given name are contained within this group.
     *
     * <p>Header name comparison is case insensitive.
     *
     * @param name the header name to test for
     * @return <code>true</code> if at least one header with the name is
     * contained, <code>false</code> otherwise
     */
    CARAPI ContainsHeader(
        /* [in] */ const String& name,
        /* [out] */ Boolean* containsHeader);

    /**
     * Returns an iterator over this group of headers.
     *
     * @return iterator over this group of headers.
     *
     * @since 4.0
     */
    CARAPI GetIterator(
        /* [out] */ IHeaderIterator** headerIterator);

    /**
     * Returns an iterator over the headers with a given name in this group.
     *
     * @param name      the name of the headers over which to iterate, or
     *                  <code>null</code> for all headers
     *
     * @return iterator over some headers in this group.
     *
     * @since 4.0
     */
    CARAPI GetIterator(
        /* [in] */ const String& name,
        /* [out] */ IHeaderIterator** headerIterator);

    /**
     * Returns a copy of this object
     *
     * @return copy of this object
     */
    CARAPI Copy(
        /* [out] */ IHeaderGroup** headerGroup);

    CARAPI Clone(
        /* [out] */ IInterface** obj);

    CARAPI constructor();

private:
    /** The list of headers for this group, in the order in which they were added */
    AutoPtr<IList> mHeaders;
};

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_MESSAGE_CHEADERGROUP_H_
