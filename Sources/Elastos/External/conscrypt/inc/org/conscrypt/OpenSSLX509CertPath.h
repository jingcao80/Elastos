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

#ifndef __ORG_CONSCRYPT_OPENSSLX509CERTPATH_H__
#define __ORG_CONSCRYPT_OPENSSLX509CERTPATH_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "_Org.Conscrypt.h"
#include <elastos/core/Object.h>

using Elastos::IO::IInputStream;
using Elastos::Security::Cert::ICertPath;
using Elastos::Utility::IIterator;
using Elastos::Utility::IList;

namespace Org {
namespace Conscrypt {

class OpenSSLX509CertPath
    : public Object // public CertPath
    , public IOpenSSLX509CertPath
{
public:
    /**
     * Supported encoding types for CerthPath. Used by the various APIs that
     * encode this into bytes such as {@link #getEncoded()}.
     */
    enum Encoding
    {
        PKI_PATH,   // = "PkiPath",
        PKCS7,   // = "PKCS7",
    };

public:
    CAR_INTERFACE_DECL()

    static CARAPI GetEncodingsIterator(
        /* [out] */ IIterator** result);

    CARAPI constructor(
        /* [in] */ IList* certificates);

    CARAPI GetCertificates(
        /* [out] */ IList** result);

    CARAPI GetEncoded(
        /* [out] */ ArrayOf<Byte>** result);

    CARAPI GetEncoded(
        /* [in] */ String encoding,
        /* [out] */ ArrayOf<Byte>** result);

    CARAPI GetEncodings(
        /* [out] */ IIterator** result);

    static CARAPI FromEncoding(
        /* [in] */ IInputStream* inStream,
        /* [in] */ String encoding,
        /* [out] */ ICertPath** result);

    static CARAPI FromEncoding(
        /* [in] */ IInputStream* inStream,
        /* [out] */ ICertPath** result);

private:
    AutoPtr<ArrayOf<Byte> > GetEncoded(
        /* [in] */ Encoding encoding);

    static AutoPtr<ICertPath> FromPkiPathEncoding(
        /* [in] */ IInputStream* inStream);

    static AutoPtr<ICertPath> FromPkcs7Encoding(
        /* [in] */ IInputStream* inStream);

    static AutoPtr<ICertPath> FromEncoding(
        /* [in] */ IInputStream* inStream,
        /* [in] */ Encoding encoding);

private:
    static AutoPtr<ArrayOf<Byte> > PKCS7_MARKER;

    static Int32 PUSHBACK_SIZE;

    /** Unmodifiable list of encodings for the API. */
    static AutoPtr<IList> ALL_ENCODINGS;

    static Encoding DEFAULT_ENCODING;

    AutoPtr<IList> mCertificates;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLX509CERTPATH_H__