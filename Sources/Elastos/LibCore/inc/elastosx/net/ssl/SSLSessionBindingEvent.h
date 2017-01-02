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

#ifndef __ELASTOSX_NET_SSL_SSLSESSIONBINDINGEVENT_H__
#define __ELASTOSX_NET_SSL_SSLSESSIONBINDINGEVENT_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include "EventObject.h"

using Elastos::Utility::EventObject;
using Elastosx::Net::Ssl::ISSLSession;

namespace Elastosx {
namespace Net {
namespace Ssl {

/**
 * The event sent to an {@code SSLSessionBindingListener} when the listener
 * object is bound ({@link SSLSession#putValue(String, Object)}) or unbound
 * ({@link SSLSession#removeValue(String)}) to an {@code SSLSession}.
 */
class SSLSessionBindingEvent
    : public EventObject
    , public ISSLSessionBindingEvent
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Creates a new {@code SSLSessionBindingEvent} for the specified session
     * indicating a binding event for the specified name.
     *
     * @param session
     *            the session for which the event occurs.
     * @param name
     *            the name of the object being (un)bound.
     */
    CARAPI constructor(
        /* [in] */ ISSLSession* session,
        /* [in] */ const String& name);

    /**
     * Returns the name of the binding being added or removed.
     *
     * @return the name of the binding.
     */
    CARAPI GetName(
        /* [out] */ String* name);

    /**
     * Returns the session to which the binding is added or from which it is
     * removed.
     *
     * @return the session to which the binding is added or from which it is
     *         removed.
     */
    CARAPI GetSession(
        /* [out] */ ISSLSession** session);

private:
    /*const*/ String mName;
};

} // namespace Ssl
} // namespace Net
} // namespace Elastosx

#endif // __ELASTOSX_NET_SSL_SSLSESSIONBINDINGEVENT_H__
