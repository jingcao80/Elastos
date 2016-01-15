
#include "CCharBufferHelper.h"
#include "BufferFactory.h"
#include "CharBuffer.h"

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL(CCharBufferHelper, Singleton, ICharBufferHelper)

CAR_SINGLETON_IMPL(CCharBufferHelper)

ECode CCharBufferHelper::Allocate(
    /* [in] */ Int32 capacity,
    /* [out] */ ICharBuffer** charBuf)
{
    return CharBuffer::Allocate(capacity, charBuf);
}

ECode CCharBufferHelper::Wrap(
    /* [in] */ ArrayOf<Char32>* array,
    /* [out] */ ICharBuffer** charBuf)
{
    return CharBuffer::Wrap(array, charBuf);
}

ECode CCharBufferHelper::Wrap(
    /* [in] */ ArrayOf<Char32>* array,
    /* [in] */ Int32 start,
    /* [in] */ Int32 charCount,
    /* [out] */ ICharBuffer** charBuf)
{
    return CharBuffer::Wrap(array, start, charCount, charBuf);
}

ECode CCharBufferHelper::Wrap(
    /* [in] */ ICharSequence* charSequence,
    /* [out] */ ICharBuffer**  charBuf)
{
    return CharBuffer::Wrap(charSequence, charBuf);
}

ECode CCharBufferHelper::Wrap(
    /* [in] */ ICharSequence* cs,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ ICharBuffer** charBuf)
{
    return CharBuffer::Wrap(cs, start, end, charBuf);
}

} // namespace IO
} // namespace Elastos
