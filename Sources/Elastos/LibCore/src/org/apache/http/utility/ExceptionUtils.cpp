
#include "org/apache/http/utility/EntityUtils.h"
#include "org/apache/http/utility/CByteArrayBuffer.h"
#include "org/apache/http/utility/CCharArrayBuffer.h"
#include "elastos/core/Math.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::Math;
using Elastos::IO::IInputStream;
using Elastos::IO::IReader;
using Elastos::IO::IInputStreamReader;
using Elastos::IO::CInputStreamReader;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IHeader;
using Org::Apache::Http::IHeaderElement;
using Org::Apache::Http::INameValuePair;
using Org::Apache::Http::Protocol::IHTTP;

namespace Org {
namespace Apache {
namespace Http {
namespace Utility {

ECode EntityUtils::ToByteArray(
    /* [in] */ IHttpEntity* entity,
    /* [out] */ ArrayOf<Byte>** bytes)
{
    VALIDATE_NOT_NULL(bytes)
    *bytes = NULL;

    if (entity == NULL) {
        Logger::E("EntityUtils", "HTTP entity may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IInputStream> instream;
    entity->GetContent((IInputStream**)&instream);
    if (instream == NULL) {
        *bytes = ArrayOf<Byte>::Alloc(0);
        REFCOUNT_ADD(*bytes);
        return NOERROR;
    }
    Int64 len;
    if (entity->GetContentLength(&len), (Int32)len > Math::INT32_MAX_VALUE) {
        Logger::E("EntityUtils", "HTTP entity too large to be buffered in memory");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 i = (Int32)len;
    if (i < 0) {
        i = 4096;
    }
    AutoPtr<IByteArrayBuffer> buffer;
    CByteArrayBuffer::New(i, (IByteArrayBuffer**)&buffer);
    // try {
    AutoPtr< ArrayOf<Byte> > tmp = ArrayOf<Byte>::Alloc(4096);
    Int32 l;
    while(instream->Read(tmp, &l), l != -1) {
        buffer->Append(tmp, 0, l);
    }
    // } finally {
    //     instream.close();
    // }
    instream->Close();
    return buffer->ToByteArray(bytes);
}

ECode EntityUtils::GetContentCharSet(
    /* [in] */ IHttpEntity* entity,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String(NULL);
    if (entity == NULL) {
        Logger::E("EntityUtils", "HTTP entity may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    String charset = String(NULL);
    AutoPtr<IHeader> type;
    if (entity->GetContentType((IHeader**)&type), type != NULL) {
        AutoPtr< ArrayOf<IHeaderElement*> > values;
        type->GetElements((ArrayOf<IHeaderElement*>**)&values);
        if (values->GetLength() > 0) {
            AutoPtr<INameValuePair> param;
            (*values)[0]->GetParameterByName(String("charset"), (INameValuePair**)&param);
            if (param != NULL) {
                param->GetValue(&charset);
            }
        }
    }
    *str = charset;
    return NOERROR;
}

ECode EntityUtils::ToString(
    /* [in] */ IHttpEntity* entity,
    /* [in] */ const String& defaultCharset,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String(NULL);
    if (entity == NULL) {
        Logger::E("EntityUtils", "HTTP entity may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IInputStream> instream;
    entity->GetContent((IInputStream**)&instream);
    if (instream == NULL) {
        *str = String("");
        return NOERROR;
    }
    Int64 len;
    if (entity->GetContentLength(&len), (Int32)len > Math::INT32_MAX_VALUE) {
        Logger::E("EntityUtils", "HTTP entity too large to be buffered in memory");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 i = (Int32)len;
    if (i < 0) {
        i = 4096;
    }
    String charset;
    GetContentCharSet(entity, &charset);
    if (charset.IsNull()) {
        charset = defaultCharset;
    }
    if (charset.IsNull()) {
        charset = IHTTP::DEFAULT_CONTENT_CHARSET;
    }
    AutoPtr<IInputStreamReader> inputReader;
    CInputStreamReader::New(instream, charset, (InputStreamReader**)&inputReader);
    AutoPtr<IReader> reader = IReader::Probe(inputReader);
    AutoPtr<ICharArrayBuffer> buffer;
    CCharArrayBuffer::New(i, (ICharArrayBuffer**)&buffer);
    // try {
    AutoPtr< ArrayOf<Char32> > tmp = ArrayOf<Char32>::Alloc(1024);
    Int32 l;
    while(reader->Read(tmp, &l), l != -1) {
        buffer->Append(tmp, 0, l);
    }
    // } finally {
    //     reader.close();
    // }
    reader->Close();
    return buffer->ToString(str);
}

ECode EntityUtils::ToString(
    /* [in] */ IHttpEntity* entity,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    return ToString(entity, String(NULL), str);
}

} // namespace Utility
} // namespace Http
} // namespace Apache
} // namespace Org
