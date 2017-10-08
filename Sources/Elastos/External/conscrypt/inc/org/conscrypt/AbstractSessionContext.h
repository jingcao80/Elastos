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

#ifndef __ORG_CONSCRYPT_ABSTRACTSESSIONCONTEXT_H__
#define __ORG_CONSCRYPT_ABSTRACTSESSIONCONTEXT_H__

#include <Elastos.CoreLibrary.Extensions.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Security.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/coredef.h>
#include <elastos/core/Object.h>
#include <elastos/utility/LinkedHashMap.h>
#include "_Org.Conscrypt.h"

using Elastos::Utility::IEnumeration;
using Elastos::Utility::IIterator;
using Elastos::Utility::IMap;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::LinkedHashMap;
using Elastosx::Net::Ssl::ISSLSession;
using Elastosx::Net::Ssl::ISSLSessionContext;

namespace Org {
namespace Conscrypt {

class AbstractSessionContext
    : public ISSLSessionContext
    , public IAbstractSessionContext
    , public Object
{
private:
    class _Enumeration
        : public Object
        , public IEnumeration
    {
    public:
        _Enumeration(
            /* [in] */ IIterator* i,
            /* [in] */ AbstractSessionContext* host);

        CAR_INTERFACE_DECL()

        CARAPI HasMoreElements(
            /* [out] */ Boolean* result);

        CARAPI GetNextElement(
            /* [out] */ IInterface** element);

    private:
        AbstractSessionContext* mHost;
        AutoPtr<ISSLSession> mNext;
        AutoPtr<IIterator> mI;
    };

    class CustomLinkedHashMap
        : public LinkedHashMap
    {
    public:
        CustomLinkedHashMap(
            /* [in] */ AbstractSessionContext* host);

    protected:
        CARAPI_(Boolean) RemoveEldestEntry(
            /* [in] */ IMapEntry* eldest);

    private:
        AbstractSessionContext* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    AbstractSessionContext();

    virtual ~AbstractSessionContext();

    /**
     * Constructs a new session context.
     *
     * @param maximumSize of cache
     * @param timeout for cache entries
     */
    CARAPI constructor(
        /* [in] */ Int32 maximumSize,
        /* [in] */ Int32 timeout);

    // @Override
    virtual CARAPI GetIds(
        /* [out] */ IEnumeration** ids);

    // @Override
    virtual CARAPI GetSessionCacheSize(
        /* [out] */ Int32* result);

    // @Override
    virtual CARAPI GetSessionTimeout(
        /* [out] */ Int32* result);

    // @Override
    virtual CARAPI SetSessionTimeout(
        /* [in] */ Int32 seconds);

    // @Override
    virtual CARAPI SetSessionCacheSize(
        /* [in] */ Int32 size);

    /**
     * Converts the given session to bytes.
     *
     * @return session data as bytes or null if the session can't be converted
     */
    virtual CARAPI ToBytes(
        /* [in] */ ISSLSession* session,
        /* [out] */ ArrayOf<Byte>** result);

    /**
     * Creates a session from the given bytes.
     *
     * @return a session or null if the session can't be converted
     */
    virtual CARAPI ToSession(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [out] */ ISSLSession** result);

    // @Override
    virtual CARAPI GetSession(
        /* [in] */ ArrayOf<Byte>* sessionId,
        /* [out] */ ISSLSession** result);

    virtual CARAPI PutSession(
        /* [in] */ ISSLSession* session);

protected:
    /**
     * Makes sure cache size is < maximumSize.
     */
    CARAPI_(void) TrimToSize();

    /**
     * Called when a session is removed. Used by ClientSessionContext
     * to update its host-and-port based cache.
     */
    CARAPI_(void) SessionRemoved(
        /* [in] */ ISSLSession* session);

private:
    /**
     * Returns the collection of sessions ordered from oldest to newest
     */
    CARAPI_(AutoPtr<IIterator>) SessionIterator();

public:
    /*volatile*/ Int32 mMaximumSize;
    /*volatile*/ Int32 mTimeout;

    Int64 mSslCtxNativePointer;  // = NativeCrypto.SSL_CTX_new();

private:
    AutoPtr<IMap/*<ByteArray, SSLSession>*/> mSessions;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_ABSTRACTSESSIONCONTEXT_H__
