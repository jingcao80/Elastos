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

#ifndef __ORG_CONSCRYPT_SERVERSESSIONCONTEXT_H__
#define __ORG_CONSCRYPT_SERVERSESSIONCONTEXT_H__

#include "_Org.Conscrypt.h"
#include "AbstractSessionContext.h"

using Elastosx::Net::Ssl::ISSLSession;

namespace Org {
namespace Conscrypt {

/**
 * Caches server sessions. Indexes by session ID. Users typically look up
 * sessions using the ID provided by an SSL client.
 */
class ServerSessionContext
    : public AbstractSessionContext
    , public IServerSessionContext
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI SetPersistentCache(
        /* [in] */ ISSLServerSessionCache* persistentCache);

    CARAPI SessionRemoved(
        /* [in] */ ISSLSession* session);

    CARAPI GetSession(
        /* [in] */ ArrayOf<Byte>* sessionId,
        /* [out] */ ISSLSession** result);

    CARAPI PutSession(
        /* [in] */ ISSLSession* session);

private:
    AutoPtr<ISSLServerSessionCache> mPersistentCache;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_SERVERSESSIONCONTEXT_H__