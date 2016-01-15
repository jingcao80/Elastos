
#include "FilterWriter.h"
#include "AutoLock.h"

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL(FilterWriter, Writer, IFilterWriter)

FilterWriter::FilterWriter()
{
}

FilterWriter::~FilterWriter()
{
}

ECode FilterWriter::constructor(
    /* [in] */ IWriter* out)
{
    FAIL_RETURN(Writer::constructor(ISynchronize::Probe(out)))

    mOut = out;

    return NOERROR;
}

ECode FilterWriter::Close()
{
    assert(mLock != NULL);
    AutoLock lock(mLock);

    return ICloseable::Probe(mOut)->Close();
}

ECode FilterWriter::Flush()
{
    assert(mLock != NULL);
    AutoLock lock(mLock);

    return IFlushable::Probe(mOut)->Flush();
}

ECode FilterWriter::Write(
    /* [in] */ ArrayOf<Char32>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count)
{
    assert(mLock != NULL);
    AutoLock lock(mLock);

    return mOut->Write(buffer, offset, count);
}

ECode FilterWriter::Write(
    /* [in] */ Int32 oneChar32)
{
    assert(mLock != NULL);
    AutoLock lock(mLock);

    return mOut->Write(oneChar32);
}

ECode FilterWriter::Write(
    /* [in] */ const String& str,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count)
{
    assert(mLock != NULL);
    AutoLock lock(mLock);

    return mOut->Write(str, offset, count);
}

} // namespace IO
} // namespace Elastos
