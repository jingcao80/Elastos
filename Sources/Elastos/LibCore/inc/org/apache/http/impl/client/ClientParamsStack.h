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

#ifndef __ORG_APACHE_HTTP_IMPL_CLIENT_CLIENTPARAMSSTACK_H__
#define __ORG_APACHE_HTTP_IMPL_CLIENT_CLIENTPARAMSSTACK_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "org/apache/http/params/AbstractHttpParams.h"

using Org::Apache::Http::Params::AbstractHttpParams;
using Org::Apache::Http::Params::IHttpParams;
using Org::Apache::Http::Client::IResponseHandler;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Client {

/**
 * Represents a stack of parameter collections.
 * When retrieving a parameter, the stack is searched in a fixed order
 * and the first match returned. Setting parameters via the stack is
 * not supported. To minimize overhead, the stack has a fixed size and
 * does not maintain an internal array.
 * The supported stack entries, sorted by increasing priority, are:
 * <ol>
 * <li>Application parameters:
 *     expected to be the same for all clients used by an application.
 *     These provide "global", that is application-wide, defaults.
 *     </li>
 * <li>Client parameters:
 *     specific to an instance of
 *     {@link org.apache.http.client.HttpClient HttpClient}.
 *     These provide client specific defaults.
 *     </li>
 * <li>Request parameters:
 *     specific to a single request execution.
 *     For overriding client and global defaults.
 *     </li>
 * <li>Override parameters:
 *     specific to an instance of
 *     {@link org.apache.http.client.HttpClient HttpClient}.
 *     These can be used to set parameters that cannot be overridden
 *     on a per-request basis.
 *     </li>
 * </ol>
 * Each stack entry may be <code>null</code>. That is preferable over
 * an empty params collection, since it avoids searching the empty collection
 * when looking up parameters.
 *
 * @author <a href="mailto:rolandw at apache.org">Roland Weber</a>
 *
 *
 * @version $Revision: 673450 $
 */
class ClientParamsStack : public AbstractHttpParams
{
public:
    /**
     * Creates a new parameter stack from elements.
     * The arguments will be stored as-is, there is no copying to
     * prevent modification.
     *
     * @param aparams   application parameters, or <code>null</code>
     * @param cparams   client parameters, or <code>null</code>
     * @param rparams   request parameters, or <code>null</code>
     * @param oparams   override parameters, or <code>null</code>
     */
    ClientParamsStack(
        /* [in] */ IHttpParams* aparams,
        /* [in] */ IHttpParams* cparams,
        /* [in] */ IHttpParams* rparams,
        /* [in] */ IHttpParams* oparams);

    /**
     * Creates a copy of a parameter stack.
     * The new stack will have the exact same entries as the argument stack.
     * There is no copying of parameters.
     *
     * @param stack     the stack to copy
     */
    ClientParamsStack(
        /* [in] */ ClientParamsStack* stack);

    /**
     * Creates a modified copy of a parameter stack.
     * The new stack will contain the explicitly passed elements.
     * For elements where the explicit argument is <code>null</code>,
     * the corresponding element from the argument stack is used.
     * There is no copying of parameters.
     *
     * @param stack     the stack to modify
     * @param aparams   application parameters, or <code>null</code>
     * @param cparams   client parameters, or <code>null</code>
     * @param rparams   request parameters, or <code>null</code>
     * @param oparams   override parameters, or <code>null</code>
     */
    ClientParamsStack(
        /* [in] */ ClientParamsStack* stack,
        /* [in] */ IHttpParams* aparams,
        /* [in] */ IHttpParams* cparams,
        /* [in] */ IHttpParams* rparams,
        /* [in] */ IHttpParams* oparams);

    /**
     * Obtains the application parameters of this stack.
     *
     * @return  the application parameters, or <code>null</code>
     */
    CARAPI_(AutoPtr<IHttpParams>) GetApplicationParams();

    /**
     * Obtains the client parameters of this stack.
     *
     * @return  the client parameters, or <code>null</code>
     */
    CARAPI_(AutoPtr<IHttpParams>) GetClientParams();

    /**
     * Obtains the request parameters of this stack.
     *
     * @return  the request parameters, or <code>null</code>
     */
    CARAPI_(AutoPtr<IHttpParams>) GetRequestParams();

    /**
     * Obtains the override parameters of this stack.
     *
     * @return  the override parameters, or <code>null</code>
     */
    CARAPI_(AutoPtr<IHttpParams>) GetOverrideParams();

    /**
     * Obtains a parameter from this stack.
     * See class comment for search order.
     *
     * @param name      the name of the parameter to obtain
     *
     * @return  the highest-priority value for that parameter, or
     *          <code>null</code> if it is not set anywhere in this stack
     */
    CARAPI GetParameter(
        /* [in] */ const String& name,
        /* [out] */ IInterface** parameter);

    /**
     * Does <i>not</i> set a parameter.
     * Parameter stacks are read-only. It is possible, though discouraged,
     * to access and modify specific stack entries.
     * Derived classes may change this behavior.
     *
     * @param name      ignored
     * @param value     ignored
     *
     * @return  nothing
     *
     * @throws UnsupportedOperationException    always
     */
    CARAPI SetParameter(
        /* [in] */ const String& name,
        /* [in] */ IInterface* value,
        /* [out] */ IHttpParams** httpParams);

    /**
     * Does <i>not</i> remove a parameter.
     * Parameter stacks are read-only. It is possible, though discouraged,
     * to access and modify specific stack entries.
     * Derived classes may change this behavior.
     *
     * @param name      ignored
     *
     * @return  nothing
     *
     * @throws UnsupportedOperationException    always
     */
    CARAPI RemoveParameter(
        /* [in] */ const String& name,
        /* [out] */ Boolean* remove);

    /**
     * Does <i>not</i> copy parameters.
     * Parameter stacks are lightweight objects, expected to be instantiated
     * as needed and to be used only in a very specific context. On top of
     * that, they are read-only. The typical copy operation to prevent
     * accidental modification of parameters passed by the application to
     * a framework object is therefore pointless and disabled.
     * Create a new stack if you really need a copy.
     * <br/>
     * Derived classes may change this behavior.
     *
     * @return <code>this</code> parameter stack
     */
    CARAPI Copy(
        /* [out] */ IHttpParams** httpParams);

private:
    CARAPI_(void) Init(
        /* [in] */ IHttpParams* aparams,
        /* [in] */ IHttpParams* cparams,
        /* [in] */ IHttpParams* rparams,
        /* [in] */ IHttpParams* oparams);

protected:
    /** The application parameter collection, or <code>null</code>. */
    AutoPtr<IHttpParams> mApplicationParams;

    /** The client parameter collection, or <code>null</code>. */
    AutoPtr<IHttpParams> mClientParams;

    /** The request parameter collection, or <code>null</code>. */
    AutoPtr<IHttpParams> mRequestParams;

    /** The override parameter collection, or <code>null</code>. */
    AutoPtr<IHttpParams> mOverrideParams;
};

} // namespace Client
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_CLIENT_CLIENTPARAMSSTACK_H__
