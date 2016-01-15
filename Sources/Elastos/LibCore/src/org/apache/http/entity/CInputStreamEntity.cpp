
#include "org/apache/http/entity/CInputStreamEntity.h"
#include "elastos/core/Math.h"
#include "elastos/io/CByteArrayInputStream.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::Math;
using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::CByteArrayInputStream;
using Elastos::IO::ICloseable;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Entity {

const Int32 CInputStreamEntity::BUFFER_SIZE;

CInputStreamEntity::CInputStreamEntity()
    : mLength(0)
    , mConsumed(FALSE)
{}

CAR_OBJECT_IMPL(CInputStreamEntity)

ECode CInputStreamEntity::IsRepeatable(
    /* [out] */ Boolean* isRepeatable)
{
    VALIDATE_NOT_NULL(isRepeatable)
    *isRepeatable = FALSE;
    return NOERROR;
}

ECode CInputStreamEntity::GetContentLength(
    /* [out] */ Int64* length)
{
    VALIDATE_NOT_NULL(length)
    *length = mLength;
    return NOERROR;
}

ECode CInputStreamEntity::GetContent(
    /* [out] */ IInputStream** inputStream)
{
    VALIDATE_NOT_NULL(inputStream)
    *inputStream = mContent;
    REFCOUNT_ADD(*inputStream)
    return NOERROR;
}

ECode CInputStreamEntity::WriteTo(
    /* [in] */ IOutputStream* outstream)
{
    if (outstream == NULL) {
        Logger::E("CInputStreamEntity", "Output stream may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IInputStream> instream = mContent;
    AutoPtr< ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(BUFFER_SIZE);
    Int32 l;
    if (mLength < 0) {
        // consume until EOF
        while (instream->Read(buffer, &l), l != -1) {
            outstream->Write(buffer, 0, l);
        }
    }
    else {
        // consume no more than length
        Int64 remaining = mLength;
        while (remaining > 0) {
            instream->Read(buffer, 0, (Int32)Elastos::Core::Math::Min((Int64)BUFFER_SIZE, remaining), &l);
            if (l == -1) {
                break;
            }
            outstream->Write(buffer, 0, l);
            remaining -= l;
        }
    }
    mConsumed = TRUE;
    return NOERROR;
}

ECode CInputStreamEntity::IsStreaming(
    /* [out] */ Boolean* isStreaming)
{
    VALIDATE_NOT_NULL(isStreaming)
    *isStreaming = !mConsumed;
    return NOERROR;
}

ECode CInputStreamEntity::ConsumeContent()
{
    mConsumed = TRUE;
    // If the input stream is from a connection, closing it will read to
    // the end of the content. Otherwise, we don't care what it does.
    ICloseable::Probe(mContent)->Close();
    return NOERROR;
}

ECode CInputStreamEntity::constructor(
    /* [in] */ IInputStream* instream,
    /* [in] */ Int64 length)
{
    if (instream == NULL) {
        Logger::E("CInputStreamEntity", "Source input stream may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mContent = instream;
    mLength = length;
    return NOERROR;
}

} // namespace Entity
} // namespace Http
} // namespace Apache
} // namespace Org
