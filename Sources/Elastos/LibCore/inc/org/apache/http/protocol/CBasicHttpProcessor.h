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

#ifndef __ORG_APACHE_HTTP_PROTOCOL_CBasicHttpProcessor_H_
#define __ORG_APACHE_HTTP_PROTOCOL_CBasicHttpProcessor_H_

#include "Elastos.CoreLibrary.Utility.h"
#include "_Org_Apache_Http_Protocol_CBasicHttpProcessor.h"
#include "elastos/core/Object.h"

using Elastos::Core::ICloneable;
using Elastos::Utility::IList;
using Org::Apache::Http::IHttpRequestInterceptor;
using Org::Apache::Http::IHttpResponseInterceptor;
using Org::Apache::Http::IHttpResponse;
using Org::Apache::Http::IHttpRequest;

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {

/**
 * Keeps lists of interceptors for processing requests and responses.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 * @author Andrea Selva
 *
 * @version $Revision: 613298 $
 *
 * @since 4.0
 */
CarClass(CBasicHttpProcessor)
    , public Object
    , public IBasicHttpProcessor
    , public IHttpRequestInterceptor
    , public IHttpResponseInterceptor
    , public IHttpRequestInterceptorList
    , public IHttpResponseInterceptorList
    , public ICloneable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Appends a request interceptor to this list.
     *
     * @param itcp      the request interceptor to add
     */
    CARAPI AddRequestInterceptor(
        /* [in] */ IHttpRequestInterceptor* itcp);

    /**
     * Inserts a request interceptor at the specified index.
     *
     * @param itcp      the request interceptor to add
     * @param index     the index to insert the interceptor at
     */
    CARAPI AddRequestInterceptor(
        /* [in] */ IHttpRequestInterceptor* itcp,
        /* [in] */ Int32 index);

    /**
     * Inserts a Response interceptor at the specified index.
     *
     * @param itcp      the Response interceptor to add
     * @param index     the index to insert the interceptor at
     */
    CARAPI AddResponseInterceptor(
        /* [in] */ IHttpResponseInterceptor* itcp,
        /* [in] */ Int32 index);

    /**
     * Removes all request interceptor of the specified class
     *
     * @param clazz  the class of the instances to be removed.
     */
    CARAPI RemoveRequestInterceptorByClass(
        /* [in] */ IInterface* clazz /*Class clazz*/);

    /**
     * Removes all Response interceptor of the specified class
     *
     * @param clazz  the class of the instances to be removed.
     */
    CARAPI RemoveResponseInterceptorByClass(
        /* [in] */ IInterface* clazz /*Class clazz*/);

    /**
     * Same as {@link #addRequestInterceptor(HttpRequestInterceptor) addRequestInterceptor}.
     *
     * @param interceptor       the interceptor to add
     */
    CARAPI AddInterceptor(
        /* [in] */ IHttpRequestInterceptor* interceptor);

    CARAPI AddInterceptor(
        /* [in] */ IHttpRequestInterceptor* interceptor,
        /* [in] */ Int32 index);

    /**
     * Obtains the current size of this list.
     *
     * @return  the number of request interceptors in this list
     */
    CARAPI GetRequestInterceptorCount(
        /* [out] */ Int32* count);

    /**
     * Obtains a request interceptor from this list.
     *
     * @param index     the index of the interceptor to obtain,
     *                  0 for first
     *
     * @return  the interceptor at the given index, or
     *          <code>null</code> if the index is out of range
     */
    CARAPI GetRequestInterceptor(
        /* [in] */ Int32 index,
        /* [out] */ IHttpRequestInterceptor** interceptor);

    /**
     * Removes all request interceptors from this list.
     */
    CARAPI ClearRequestInterceptors();

    /**
     * Appends a Response interceptor to this list.
     *
     * @param itcp      the Response interceptor to add
     */
    CARAPI AddResponseInterceptor(
        /* [in] */ IHttpResponseInterceptor* itcp);

    /**
     * Same as {@link #addResponseInterceptor(HttpResponseInterceptor) addResponseInterceptor}.
     *
     * @param interceptor       the interceptor to add
     */
    CARAPI AddInterceptor(
        /* [in] */ IHttpResponseInterceptor* interceptor);

    CARAPI AddInterceptor(
        /* [in] */ IHttpResponseInterceptor* interceptor,
        /* [in] */ Int32 index);

    /**
     * Obtains the current size of this list.
     *
     * @return  the number of Response interceptors in this list
     */
    CARAPI GetResponseInterceptorCount(
        /* [out] */ Int32* count);

    /**
     * Obtains a Response interceptor from this list.
     *
     * @param index     the index of the interceptor to obtain,
     *                  0 for first
     *
     * @return  the interceptor at the given index, or
     *          <code>null</code> if the index is out of range
     */
    CARAPI GetResponseInterceptor(
        /* [in] */ Int32 index,
        /* [out] */ IHttpResponseInterceptor** interceptor);

    /**
     * Removes all Response interceptors from this list.
     */
    CARAPI ClearResponseInterceptors();

    /**
     * Sets the interceptor lists.
     * First, both interceptor lists maintained by this processor
     * will be cleared.
     * Subsequently,
     * elements of the argument list that are request interceptors will be
     * added to the request interceptor list.
     * Elements that are response interceptors will be
     * added to the response interceptor list.
     * Elements that are both request and response interceptor will be
     * added to both lists.
     * Elements that are neither request nor response interceptor
     * will be ignored.
     *
     * @param list      the list of request and response interceptors
     *                  from which to initialize
     */
    CARAPI SetInterceptors(
        /* [in] */ IList* itcps);

    /**
     * Clears both interceptor lists maintained by this processor.
     */
    CARAPI ClearInterceptors();

    /**
     * Processes a request.
     * On the client side, this step is performed before the request is
     * sent to the server. On the server side, this step is performed
     * on incoming messages before the message body is evaluated.
     *
     * @param request   the request to preprocess
     * @param context   the context for the request
     *
     * @throws IOException      in case of an IO problem
     * @throws HttpException    in case of a protocol or other problem
     */
    CARAPI Process(
        /* [in] */ IHttpRequest* request,
        /* [in] */ IHttpContext* context);

    /**
     * Processes a response.
     * On the server side, this step is performed before the response is
     * sent to the client. On the client side, this step is performed
     * on incoming messages before the message body is evaluated.
     *
     * @param response  the response to postprocess
     * @param context   the context for the request
     *
     * @throws IOException      in case of an IO problem
     * @throws HttpException    in case of a protocol or other problem
     */
    CARAPI Process(
        /* [in] */ IHttpResponse* request,
        /* [in] */ IHttpContext* context);

    /**
     * Creates a copy of this instance
     *
     * @return new instance of the BasicHttpProcessor
     */
    CARAPI Copy(
        /* [out] */ IBasicHttpProcessor** processor);

    CARAPI Clone(
        /* [out] */ IInterface** obj);

    CARAPI constructor();

protected:
    CARAPI_(void) CopyInterceptors(
        /* [in] */ IBasicHttpProcessor* target);

protected:
    AutoPtr<IList> mRequestInterceptors;
    AutoPtr<IList> mResponseInterceptors;
};

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_PROTOCOL_CBasicHttpProcessor_H_
