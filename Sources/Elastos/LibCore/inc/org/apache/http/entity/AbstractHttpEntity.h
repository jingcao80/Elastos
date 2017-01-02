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

#ifndef __ORG_APACHE_HTTP_ENTITY_ABSTRACTHTTPENTITY_H__
#define __ORG_APACHE_HTTP_ENTITY_ABSTRACTHTTPENTITY_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Org::Apache::Http::IHeader;

namespace Org {
namespace Apache {
namespace Http {
namespace Entity {

/**
 * Abstract base class for entities.
 * Provides the commonly used attributes for streamed and self-contained
 * implementations of {@link HttpEntity HttpEntity}.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 496070 $
 *
 * @since 4.0
 */
class ECO_PUBLIC AbstractHttpEntity
    : public Object
    , public IAbstractHttpEntity
    , public IHttpEntity
{
public:
    /**
     * Protected default constructor.
     * The attributes of the created object remain
     * <code>null</code> and <code>false</code>, respectively.
     */
    AbstractHttpEntity();

    virtual ~AbstractHttpEntity() {}

    CAR_INTERFACE_DECL()

    /**
     * Obtains the Content-Type header.
     * The default implementation returns the value of the
     * {@link #contentType contentType} attribute.
     *
     * @return  the Content-Type header, or <code>null</code>
     */
    CARAPI GetContentType(
        /* [out] */ IHeader** type);

    /**
     * Obtains the Content-Encoding header.
     * The default implementation returns the value of the
     * {@link #contentEncoding contentEncoding} attribute.
     *
     * @return  the Content-Encoding header, or <code>null</code>
     */
    CARAPI GetContentEncoding(
        /* [out] */ IHeader** encoding);

    /**
     * Obtains the 'chunked' flag.
     * The default implementation returns the value of the
     * {@link #chunked chunked} attribute.
     *
     * @return  the 'chunked' flag
     */
    CARAPI IsChunked(
        /* [out] */ Boolean* isChunked);

    /**
     * Specifies the Content-Type header.
     * The default implementation sets the value of the
     * {@link #contentType contentType} attribute.
     *
     * @param contentType       the new Content-Encoding header, or
     *                          <code>null</code> to unset
     */
    CARAPI SetContentType(
        /* [in] */ IHeader* contentType);

    /**
     * Specifies the Content-Type header, as a string.
     * The default implementation calls
     * {@link #setContentType(Header) setContentType(Header)}.
     *
     * @param ctString     the new Content-Type header, or
     *                     <code>null</code> to unset
     */
    CARAPI SetContentType(
        /* [in] */ const String& ctString);

    /**
     * Specifies the Content-Encoding header.
     * The default implementation sets the value of the
     * {@link #contentEncoding contentEncoding} attribute.
     *
     * @param contentEncoding   the new Content-Encoding header, or
     *                          <code>null</code> to unset
     */
    CARAPI SetContentEncoding(
        /* [in] */ IHeader* contentEncoding);

    /**
     * Specifies the Content-Encoding header, as a string.
     * The default implementation calls
     * {@link #setContentEncoding(Header) setContentEncoding(Header)}.
     *
     * @param ceString     the new Content-Encoding header, or
     *                     <code>null</code> to unset
     */
    CARAPI SetContentEncoding(
        /* [in] */ const String& ceString);

    /**
     * Specifies the 'chunked' flag.
     * The default implementation sets the value of the
     * {@link #chunked chunked} attribute.
     *
     * @param b         the new 'chunked' flag
     */
    CARAPI SetChunked(
        /* [in] */ Boolean b);

    /**
     * Does not consume anything.
     * The default implementation does nothing if
     * {@link HttpEntity#isStreaming isStreaming}
     * returns <code>false</code>, and throws an exception
     * if it returns <code>true</code>.
     * This removes the buAbstractHttpEntity.hrden of implementing
     * an empty method for non-streaming entities.
     *
     * @throws IOException      in case of an I/O problem
     * @throws UnsupportedOperationException
     *          if a streaming subclass does not override this method
     */
    CARAPI ConsumeContent();

protected:
    /**
     * The Content-Type header.
     * Returned by {@link #getContentType getContentType},
     * unless that method is overridden.
     */
    AutoPtr<IHeader> mContentType;

    /**
     * The Content-Encoding header.
     * Returned by {@link #getContentEncoding getContentEncoding},
     * unless that method is overridden.
     */
    AutoPtr<IHeader> mContentEncoding;

    /**
     * The 'chunked' flag.
     * Returned by {@link #isChunked isChunked},
     * unless that method is overridden.
     */
    Boolean mChunked;
};

} // namespace Entity
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_ENTITY_ABSTRACTHTTPENTITY_H__
