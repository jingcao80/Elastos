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

#ifndef __ORG_APACHE_HTTP_IMPL_DEFAULTHTTPRESPONSEFACTORY_H__
#define __ORG_APACHE_HTTP_IMPL_DEFAULTHTTPRESPONSEFACTORY_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Elastos::Utility::ILocale;
using Org::Apache::Http::IReasonPhraseCatalog;
using Org::Apache::Http::IHttpResponseFactory;
using Org::Apache::Http::IHttpResponse;
using Org::Apache::Http::IProtocolVersion;
using Org::Apache::Http::IRequestLine;
using Org::Apache::Http::Protocol::IHttpContext;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {

/**
 * Default implementation of a factory for creating response objects.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 618367 $
 *
 * @since 4.0
 */
class DefaultHttpResponseFactory
    : public Object
    , public IHttpResponseFactory
{
public:
    /**
     * Creates a new response factory with the given catalog.
     *
     * @param catalog   the catalog of reason phrases
     */
    DefaultHttpResponseFactory(
        /* [in] */ IReasonPhraseCatalog* catalog);

    /**
     * Creates a new response factory with the default catalog.
     * The default catalog is
     * {@link EnglishReasonPhraseCatalog EnglishReasonPhraseCatalog}.
     */
    DefaultHttpResponseFactory();

    CAR_INTERFACE_DECL()

    CARAPI NewHttpResponse(
        /* [in] */ IProtocolVersion* ver,
        /* [in] */ Int32 status,
        /* [in] */ IHttpContext* context,
        /* [out] */ IHttpResponse** response);

    /**
     * Creates a new response from a status line.
     *
     * @param statusline the status line
     * @param context    the context from which to determine the locale
     *                   for looking up a reason phrase if the status code
     *                   is updated, or
     *                   <code>null</code> to use the default locale
     *
     * @return  the new response with the argument status line
     */
    CARAPI NewHttpResponse(
        /* [in] */ IStatusLine* statusline,
        /* [in] */ IHttpContext* context,
        /* [out] */ IHttpResponse** response);

protected:
    /**
     * Determines the locale of the response.
     * The implementation in this class always returns the default locale.
     *
     * @param context   the context from which to determine the locale, or
     *                  <code>null</code> to use the default locale
     *
     * @return  the locale for the response, never <code>null</code>
     */
    CARAPI DetermineLocale(
        /* [in] */ IHttpContext* context,
        /* [out] */ ILocale** locale);

private:
    CARAPI Init(
        /* [in] */ IReasonPhraseCatalog* catalog);

protected:
    /** The catalog for looking up reason phrases. */
    AutoPtr<IReasonPhraseCatalog> mReasonCatalog;
};

} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_DEFAULTHTTPRESPONSEFACTORY_H__
