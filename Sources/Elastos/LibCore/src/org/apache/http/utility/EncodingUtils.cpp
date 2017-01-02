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

#include "org/apache/http/utility/EncodingUtils.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Utility {

ECode EncodingUtils::GetString(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [in] */ const String& charset,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String(NULL);
    if (data == NULL) {
        Logger::E("EncodingUtils", "Parameter may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (charset.IsNull() || charset.GetLength() == 0) {
        Logger::E("EncodingUtils", "charset may not be null or empty");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // try {
    *str = String(*data, offset, length);// new String(data, offset, length, charset);
    return NOERROR;
    // } catch (UnsupportedEncodingException e) {
    //     return new String(data, offset, length);
    // }
}

ECode EncodingUtils::GetString(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ const String& charset,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String(NULL);
    if (data == NULL) {
        Logger::E("EncodingUtils", "Parameter may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return GetString(data, 0, data->GetLength(), charset, str);
}

ECode EncodingUtils::GetBytes(
    /* [in] */ const String& data,
    /* [in] */ const String& charset,
    /* [out] */ ArrayOf<Byte>** bytes)
{
    VALIDATE_NOT_NULL(bytes)
    *bytes = NULL;
    if (data == NULL) {
        Logger::E("EncodingUtils", "data may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (charset.IsNull() || charset.GetLength() == 0) {
        Logger::E("EncodingUtils", "charset may not be null or empty");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // try {
    *bytes = data.GetBytes(); // return data.getBytes(charset);
    REFCOUNT_ADD(*bytes)
    return NOERROR;
    // } catch (UnsupportedEncodingException e) {
    //     return data.getBytes();
    // }
}

ECode EncodingUtils::GetAsciiBytes(
    /* [in] */ const String& data,
    /* [out] */ ArrayOf<Byte>** bytes)
{
    VALIDATE_NOT_NULL(bytes)
    *bytes = NULL;
     if (data.IsNull()) {
        Logger::E("EncodingUtils", "Parameter may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // try {
    AutoPtr< ArrayOf<Byte> > bs = data.GetBytes(); // return data.getBytes(HTTP.US_ASCII);
    *bytes = bs;
    REFCOUNT_ADD(*bytes)
    return NOERROR;
    // } catch (UnsupportedEncodingException e) {
    //     throw new Error("HttpClient requires ASCII support");
    // }
}

ECode EncodingUtils::GetAsciiString(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String(NULL);
    if (data == NULL) {
        Logger::E("EncodingUtils", "Parameter may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // try {
    *str = String(*data, offset, length); // return new String(data, offset, length, HTTP.US_ASCII);
    // } catch (UnsupportedEncodingException e) {
    //     throw new Error("HttpClient requires ASCII support");
    // }
    return NOERROR;
}

ECode EncodingUtils::GetAsciiString(
    /* [in] */ ArrayOf<Byte>* data,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    if (data == NULL) {
        Logger::E("EncodingUtils", "Parameter may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return GetAsciiString(data, 0, data->GetLength(), str);
}

} // namespace Utility
} // namespace Http
} // namespace Apache
} // namespace Org
