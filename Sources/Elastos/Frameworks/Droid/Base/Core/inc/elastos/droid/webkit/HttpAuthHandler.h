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

#ifndef __ELASTOS_DROID_WEBKIT_HTTPAUTHHANDLER_H__
#define __ELASTOS_DROID_WEBKIT_HTTPAUTHHANDLER_H__

#include "Elastos.Droid.Webkit.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Handler.h"

using Elastos::Droid::Os::Handler;

namespace Elastos {
namespace Droid {
namespace Webkit {

/**
 * Represents a request for HTTP authentication. Instances of this class are
 * created by the WebView and passed to
 * {@link WebViewClient#onReceivedHttpAuthRequest}. The host application must
 * call either {@link #proceed} or {@link #cancel} to set the WebView's
 * response to the request.
 */
class ECO_PUBLIC HttpAuthHandler
    : public Handler
    , public IHttpAuthHandler
{
public:
    CAR_INTERFACE_DECL()

    /**
     * @hide Only for use by WebViewProvider implementations.
     */
    HttpAuthHandler();

    /**
     * Gets whether the credentials stored for the current host (i.e. the host
     * for which {@link WebViewClient#onReceivedHttpAuthRequest} was called)
     * are suitable for use. Credentials are not suitable if they have
     * previously been rejected by the server for the current request.
     *
     * @return whether the credentials are suitable for use
     * @see Webview#getHttpAuthUsernamePassword
     */
    virtual CARAPI UseHttpAuthUsernamePassword(
        /* [out] */ Boolean* result);

    /**
     * Instructs the WebView to cancel the authentication request.
     */
    virtual CARAPI Cancel();

    /**
     * Instructs the WebView to proceed with the authentication with the given
     * credentials. Credentials for use with this method can be retrieved from
     * the WebView's store using {@link WebView#getHttpAuthUsernamePassword}.
     */
    virtual CARAPI Proceed(
        /* [in] */ const String& username,
        /* [in] */ const String& password);

    /**
     * Gets whether the prompt dialog should be suppressed.
     *
     * @return whether the prompt dialog should be suppressed
     * @hide
     */
    virtual CARAPI SuppressDialog(
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* info);
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WEBKIT_HTTPAUTHHANDLER_H__
