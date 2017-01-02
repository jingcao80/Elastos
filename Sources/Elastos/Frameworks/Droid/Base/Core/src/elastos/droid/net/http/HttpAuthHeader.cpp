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

#include "elastos/droid/net/http/HttpAuthHeader.h"
#include "elastos/droid/net/Proxy.h"
#include "elastos/droid/net/http/HttpLog.h"

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

CAR_INTERFACE_IMPL(HttpAuthHeader, Object, IHttpAuthHeader)

const String HttpAuthHeader::REALM_TOKEN("realm");
const String HttpAuthHeader::NONCE_TOKEN("nonce");
const String HttpAuthHeader::STALE_TOKEN("stale");
const String HttpAuthHeader::OPAQUE_TOKEN("opaque");
const String HttpAuthHeader::QOP_TOKEN("qop");
const String HttpAuthHeader::ALGORITHM_TOKEN("algorithm");

HttpAuthHeader::HttpAuthHeader()
    : mScheme(0)
    , mStale(FALSE)
    , mIsProxy(FALSE)
{}

ECode HttpAuthHeader::constructor(
    /* [in] */ const String& header)
{
    if (header != NULL) {
        ParseHeader(header);
    }
    return NOERROR;
}

ECode HttpAuthHeader::IsProxy(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mIsProxy;
    return NOERROR;
}

ECode HttpAuthHeader::SetProxy()
{
    mIsProxy = TRUE;
    return NOERROR;
}

ECode HttpAuthHeader::GetUsername(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mUsername;
    return NOERROR;
}

ECode HttpAuthHeader::SetUsername(
    /* [in] */ const String& username)
{
    mUsername = username;
    return NOERROR;
}

ECode HttpAuthHeader::GetPassword(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mPassword;
    return NOERROR;
}

ECode HttpAuthHeader::SetPassword(
    /* [in] */ const String& password)
{
    mPassword = password;
    return NOERROR;
}

ECode HttpAuthHeader::IsBasic(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mScheme == BASIC;
    return NOERROR;
}

ECode HttpAuthHeader::IsDigest(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mScheme == DIGEST;
    return NOERROR;
}

ECode HttpAuthHeader::GetScheme(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mScheme;
    return NOERROR;
}

ECode HttpAuthHeader::GetStale(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mStale;
    return NOERROR;
}

ECode HttpAuthHeader::GetRealm(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mRealm;
    return NOERROR;
}

ECode HttpAuthHeader::GetNonce(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mNonce;
    return NOERROR;
}

ECode HttpAuthHeader::GetOpaque(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mOpaque;
    return NOERROR;
}

ECode HttpAuthHeader::GetQop(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mQop;
    return NOERROR;
}

ECode HttpAuthHeader::GetAlgorithm(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mAlgorithm;
    return NOERROR;
}

ECode HttpAuthHeader::IsSupportedScheme(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    // it is a good idea to enforce non-null realms!
    if (mRealm != NULL) {
        if (mScheme == BASIC) {
            *result = TRUE;
        } else {
            if (mScheme == DIGEST) {
                *result = mAlgorithm.Equals("md5") &&
                    (mQop == NULL || mQop.Equals("auth"));
            }
        }
    }

    *result = FALSE;
    return NOERROR;
}

ECode HttpAuthHeader::ParseHeader(
    /* [in] */ const String& header)
{
    if (HttpLog::LOGV) {
        HttpLog::V("HttpAuthHeader.parseHeader(): header: %s", header.string());
    }

    if (header != NULL) {
        String parameters;
        parameters = ParseScheme(header);
        if (parameters != NULL) {
            // if we have a supported scheme
            if (mScheme != UNKNOWN) {
                ParseParameters(parameters);
            }
        }
    }
    return NOERROR;
}

String HttpAuthHeader::ParseScheme(
    /* [in] */ const String& header)
{
    if (header != NULL) {
        Int32 i = header.IndexOf(' ');
        if (i >= 0) {
            String localScheme = header.Substring(0, i).Trim();
            if (localScheme.EqualsIgnoreCase(DIGEST_TOKEN)) {
                mScheme = DIGEST;

                // md5 is the default algorithm!!!
                mAlgorithm = String("md5");
            } else {
                if (localScheme.EqualsIgnoreCase(BASIC_TOKEN)) {
                    mScheme = BASIC;
                }
            }

            return header.Substring(i + 1);
        }
    }

    return String(NULL);
}

ECode HttpAuthHeader::ParseParameters(
    /* [in] */ const String& parameters)
{
    if (HttpLog::LOGV) {
        HttpLog::V("HttpAuthHeader.parseParameters(): parameters: %s", parameters.string());
    }

    if (parameters != NULL) {
        Int32 i;
        String loParameters = parameters;
        do {
            i = loParameters.IndexOf(',');
            if (i < 0) {
                // have only one parameter
                ParseParameter(loParameters);
            } else {
                ParseParameter(loParameters.Substring(0, i));
                loParameters = loParameters.Substring(i + 1);
            }
        } while (i >= 0);
    }
    return NOERROR;
}

ECode HttpAuthHeader::ParseParameter(
    /* [in] */ const String& parameter)
{
    if (parameter != NULL) {
        // here, we are looking for the 1st occurence of '=' only!!!
        Int32 i = parameter.IndexOf('=');
        if (i >= 0) {
            String token = parameter.Substring(0, i).Trim();
            String value =
                TrimDoubleQuotesIfAny(parameter.Substring(i + 1).Trim());

            if (HttpLog::LOGV) {
                HttpLog::V("HttpAuthHeader.parseParameter(): token: %s value: %s",
                    token.string(), value.string());
            }

            if (token.EqualsIgnoreCase(REALM_TOKEN)) {
                mRealm = value;
            } else {
                if (mScheme == DIGEST) {
                    ParseParameter(token, value);
                }
            }
        }
    }
    return NOERROR;
}

ECode HttpAuthHeader::ParseParameter(
    /* [in] */ const String& token,
    /* [in] */ const String& value)
{
    if (token != NULL && value != NULL) {
        if (token.EqualsIgnoreCase(NONCE_TOKEN)) {
            mNonce = value;
            return NOERROR;
        }

        if (token.EqualsIgnoreCase(STALE_TOKEN)) {
            ParseStale(value);
            return NOERROR;
        }

        if (token.EqualsIgnoreCase(OPAQUE_TOKEN)) {
            mOpaque = value;
            return NOERROR;
        }

        if (token.EqualsIgnoreCase(QOP_TOKEN)) {
            mQop = value.ToLowerCase();
            return NOERROR;
        }

        if (token.EqualsIgnoreCase(ALGORITHM_TOKEN)) {
            mAlgorithm = value.ToLowerCase();
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode HttpAuthHeader::ParseStale(
    /* [in] */ const String& value)
{
    if (value != NULL) {
        if (value.EqualsIgnoreCase("true")) {
            mStale = TRUE;
        }
    }
    return NOERROR;
}

String HttpAuthHeader::TrimDoubleQuotesIfAny(
    /* [in] */ const String& value)
{
    if (value != NULL) {
        Int32 len = value.GetLength();
        if (len > 2
                && value.GetChar(0) == '\"'
                && value.GetChar(len -1) == '\"') {
            return value.Substring(1, len - 1);
        }
    }

    return value;
}

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos
