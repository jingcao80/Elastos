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

#ifndef __ELASTOS_NET_URI_H__
#define __ELASTOS_NET_URI_H__

#include "Elastos.CoreLibrary.Net.h"
#include "elastos/core/Object.h"
#include "elastos/core/Character.h"
#include "libcore/net/UriCodec.h"

using Elastos::Core::Character;
using Elastos::Core::IComparable;
using Elastos::IO::ISerializable;
using Libcore::Net::UriCodec;

namespace Elastos {
namespace Net {

class ECO_PUBLIC URI
    : public Object
    , public IURI
    , public ISerializable
    , public IComparable
{
public:
    CAR_INTERFACE_DECL()

    URI();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& uri);

    CARAPI constructor(
        /* [in] */ const String& scheme,
        /* [in] */ const String& ssp,
        /* [in] */ const String& frag);

    CARAPI constructor(
        /* [in] */ const String& scheme,
        /* [in] */ const String& userInfo,
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [in] */ const String& path,
        /* [in] */ const String& query,
        /* [in] */ const String& fragment);

    CARAPI constructor(
        /* [in] */ const String& scheme,
        /* [in] */ const String& host,
        /* [in] */ const String& path,
        /* [in] */ const String& fragment);

    CARAPI constructor(
        /* [in] */ const String& scheme,
        /* [in] */ const String& authority,
        /* [in] */ const String& path,
        /* [in] */ const String& query,
        /* [in] */ const String& fragment);

    CARAPI CompareTo(
        /* [in] */ IInterface* uri,
        /* [out] */ Int32* result);

    static CARAPI Create(
        /* [in] */ const String& uri,
        /* [out] */ IURI** obj);

    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

    CARAPI GetScheme(
        /* [out] */ String* scheme);

    CARAPI GetSchemeSpecificPart(
        /* [out] */ String* schemeSpecific);

    CARAPI GetRawSchemeSpecificPart(
        /* [out] */ String* schemeSpecific);

    CARAPI GetAuthority(
        /* [out] */ String* authority);

    CARAPI GetRawAuthority(
        /* [out] */ String* authority);

    CARAPI GetUserInfo(
        /* [out] */ String* userInfo);

    CARAPI GetRawUserInfo(
        /* [out] */ String* userInfo);

    CARAPI GetHost(
        /* [out] */ String* host);

    CARAPI GetPort(
        /* [out] */ Int32* port);

    /** @hide */
    CARAPI GetEffectivePort(
        /* [out] */ Int32* port);

    static CARAPI_(Int32) GetEffectivePort(
        /* [in] */ const String& scheme,
        /* [in] */ Int32 specifiedPort);

    CARAPI GetPath(
        /* [out] */ String* path);

    CARAPI GetRawPath(
        /* [out] */ String* path);


    CARAPI GetQuery(
        /* [out] */ String* query);

    CARAPI GetRawQuery(
        /* [out] */ String* query);

    CARAPI GetFragment(
        /* [out] */ String* fragment);

    CARAPI GetRawFragment(
        /* [out] */ String* fragment);

    CARAPI GetHashCode(
        /* [out] */ Int32* hash);

    CARAPI IsAbsolute(
        /* [out] */ Boolean* isAbsolute);

    CARAPI IsOpaque(
        /* [out] */ Boolean* isOpaque);

    CARAPI Normalize(
        /* [out] */ IURI** uri);

    CARAPI ParseServerAuthority(
        /* [out] */ IURI** uri);

    CARAPI Relativize(
        /* [in] */ IURI* relative,
        /* [out] */ IURI** uri);

    CARAPI Resolve(
        /* [in] */ IURI* relative,
        /* [out] */ IURI** uri);

    CARAPI Resolve(
        /* [in] */ const String& relative,
        /* [out] */ IURI** uri);

    CARAPI ToASCIIString(
        /* [out] */ String* str);

    CARAPI ToString(
        /* [out] */ String* s);

    CARAPI ToURL(
        /* [out] */ IURL** url);

private:
    /**
     * Breaks uri into its component parts. This first splits URI into scheme,
     * scheme-specific part and fragment:
     *   [scheme:][scheme-specific part][#fragment]
     *
     * Then it breaks the scheme-specific part into authority, path and query:
     *   [//authority][path][?query]
     *
     * Finally it delegates to parseAuthority to break the authority into user
     * info, host and port:
     *   [user-info@][host][:port]
     */
    ECO_LOCAL CARAPI ParseURI(
        /* [in] */ const String& uri,
        /* [in] */ Boolean forceServer);

    ECO_LOCAL CARAPI ValidateScheme(
        /* [in] */ const String& uri,
        /* [in] */ Int32 end,
        /* [out] */ String* result);

    ECO_LOCAL CARAPI ParseAuthority(
        /* [in] */ Boolean forceServer);

    ECO_LOCAL CARAPI ValidateUserInfo(
        /* [in] */ const String& uri,
        /* [in] */ const String& userInfo,
        /* [in] */ Int32 index);

    ECO_LOCAL CARAPI IsValidHost(
        /* [in] */ Boolean forceServer,
        /* [in] */ const String& host,
        /* [out] */ Boolean* isValid);

    ECO_LOCAL CARAPI_(Boolean) IsValidDomainName(
        /* [in] */ const String& host);

    ECO_LOCAL CARAPI_(AutoPtr<IURI>) Duplicate();

    ECO_LOCAL CARAPI_(String) ConvertHexToLowerCase(
        /* [in] */ const String& s);

    ECO_LOCAL CARAPI_(Boolean) EscapedEquals(
        /* [in] */ const String& first,
        /* [in] */ const String& second);

    ECO_LOCAL CARAPI_(String) Normalize(
        /* [in] */ const String& path,
        /* [in] */ Boolean discardRelativePrefix);

    ECO_LOCAL CARAPI_(void) SetSchemeSpecificPart();

    ECO_LOCAL CARAPI Decode(
        /* [in] */ const String& s,
        /* [out] */ String* decodeS);

    ECO_LOCAL CARAPI_(String) GetHashString();

//    void readObject(ObjectInputStream in);
//    void writeObject(ObjectOutputStream out);

private:
    class ECO_LOCAL PartEncoder : public UriCodec
    {
    private:
        String mExtraLegalCharacters;

    public:
        PartEncoder(
            /* [in] */ const String& extraLegalCharacters)
        {
            mExtraLegalCharacters = extraLegalCharacters;
        }
    protected:
        Boolean IsRetained(
            /* [in] */ Char32 c)
        {
            return URI::UNRESERVED.IndexOf(c) != -1
                    || PUNCTUATION.IndexOf(c) != -1
                    || mExtraLegalCharacters.IndexOf(c) != -1
                    || (c > 127 && !Character::IsSpaceChar(c) && !Character::IsISOControl(c));
        }
    };

    class ECO_LOCAL ASCIIEncoder : public UriCodec
    {
    protected:
        Boolean IsRetained(
            /* [in] */ Char32 c)
        {
            return c < 127;
        }
    };

public:
    static const String UNRESERVED;
    static const String PUNCTUATION;
    static const AutoPtr<UriCodec> USER_INFO_ENCODER;
    static const AutoPtr<UriCodec> PATH_ENCODER;
    static const AutoPtr<UriCodec> AUTHORITY_ENCODER;
    static const AutoPtr<UriCodec> FILE_AND_QUERY_ENCODER;
    static const AutoPtr<UriCodec> ALL_LEGAL_ENCODER;
    static const AutoPtr<UriCodec> ASCII_ONLY;

private:
    String mString;
    String mScheme;
    String mSchemeSpecificPart;
    String mAuthority;
    String mUserInfo;
    String mHost;
    Int32 mPort;
    String mPath;
    String mQuery;
    String mFragment;
    Boolean mOpaque;
    Boolean mAbsolute;
    Boolean mServerAuthority;
    Int32 mHash;
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_URI_H__
