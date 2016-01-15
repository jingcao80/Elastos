#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_PROCFILEREADER_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_PROCFILEREADER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>

using Elastos::IO::ICloseable;
using Elastos::IO::IInputStream;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

class ProcFileReader
    : public Object
    , public IProcFileReader
    , public ICloseable
{
public:
    CAR_INTERFACE_DECL()

    ProcFileReader();

    CARAPI constructor(
        /* [in] */ IInputStream* stream);

    CARAPI constructor(
        /* [in] */ IInputStream* stream,
        /* [in] */ Int32 bufferSize);

    /**
     * Check if stream has more data to be parsed.
     */
    CARAPI HasMoreData(
        /* [out] */ Boolean* result);

    /**
     * Finish current line, skipping any remaining data.
     */
    CARAPI FinishLine();

    /**
     * Parse and return next token as {@link String}.
     */
    CARAPI NextString(
        /* [out] */ String* str);

    /**
     * Parse and return next token as base-10 encoded {@code Int64}.
     */
    CARAPI NextInt64(
        /* [out] */ Int64* result);

    /**
     * Parse and return next token as base-10 encoded {@code Int64}, or return
     * the given default value if no remaining tokens on current line.
     */
    CARAPI NextOptionalInt64(
        /* [in] */ Int64 def,
        /* [out] */ Int64* result);

    /**
     * Parse and return next token as base-10 encoded {@code Int32}.
     */
    CARAPI NextInt32(
        /* [out] */ Int32* result);

    // @Override
    CARAPI Close();

private:
    /**
     * Read more data from {@link #mStream} into internal buffer.
     */
    CARAPI FillBuf(
        /* [out] */ Int32* num);

    /**
     * Consume number of bytes from beginning of internal buffer. If consuming
     * all remaining bytes, will attempt to {@link #FillBuf()}.
     */
    CARAPI ConsumeBuf(
        /* [in] */ Int32 count);

    /**
     * Find buffer index of next token delimiter, usually space or newline.
     * Fills buffer as needed.
     *
     * @return Index of next delimeter, otherwise -1 if no tokens remain on
     *         current line.
     */
    CARAPI NextTokenIndex(
        /* [out] */ Int32* index);

    CARAPI ParseAndConsumeString(
        /* [in] */ Int32 tokenIndex,
        /* [out] */ String* result);

    CARAPI ParseAndConsumeInt64(
        /* [in] */ Int32 tokenIndex,
        /* [out] */ Int64* res);

    CARAPI InvalidInt64(
        /* [in] */ Int32 tokenIndex);

private:
    AutoPtr<IInputStream> mStream;
    AutoPtr<ArrayOf<Byte> > mBuffer;

    /** Write pointer in {@link #mBuffer}. */
    Int32 mTail;
    /** Flag when last read token finished current line. */
    Boolean mLineFinished;
};

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_PROCFILEREADER_H__
