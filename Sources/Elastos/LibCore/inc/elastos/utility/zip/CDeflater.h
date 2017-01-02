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

#ifndef __ELASTOS_UTILITY_ZIP_ELASTOS_UTILITY_CDEFLATER_H__
#define __ELASTOS_UTILITY_ZIP_ELASTOS_UTILITY_CDEFLATER_H__

#include "_Elastos_Utility_Zip_CDeflater.h"
#include "NativeZipStream.h"
#include "Object.h"

using Elastos::Core::ICloseGuard;

namespace Elastos {
namespace Utility {
namespace Zip {

CarClass(CDeflater)
    , public Object
    , public IDeflater
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CDeflater();

    ~CDeflater();

    /**
     * Constructs a new {@code Deflater} instance with default compression
     * level. The strategy can be specified with {@link #setStrategy}, only. A
     * header is added to the output by default; use constructor {@code
     * Deflater(level, boolean)} if you need to omit the header.
     */
    CARAPI constructor();

    /**
     * Constructs a new {@code Deflater} instance with a specific compression
     * level. The strategy can be specified with {@code setStrategy}, only. A
     * header is added to the output by default; use
     * {@code Deflater(level, boolean)} if you need to omit the header.
     *
     * @param level
     *            the compression level in the range between 0 and 9.
     */
    CARAPI constructor(
        /* [in] */ Int32 level);

    /**
     * Constructs a new {@code Deflater} instance with a specific compression
     * level. If noHeader is passed as true no ZLib header is added to the
     * output. In a ZIP archive every entry (compressed file) comes with such a
     * header. The strategy can be specified with the setStrategy method, only.
     *
     * @param level
     *            the compression level in the range between 0 and 9.
     * @param noHeader
     *            {@code true} indicates that no ZLIB header should be written.
     */
    CARAPI constructor(
        /* [in] */ Int32 level,
        /* [in] */ Boolean noHeader);


    /**
     * Deflates the data (previously passed to {@code setInput}) into the
     * supplied buffer.
     *
     * @param buf
     *            buffer to write compressed data to.
     * @return number of bytes of compressed data written to {@code buf}.
     * @see #deflate(byte[], int, int)
     */
    CARAPI Deflate(
        /* [out] */ ArrayOf<Byte>* buf,
        /* [out] */ Int32* number);

    /**
     * Deflates data (previously passed to {@code setInput}) into a specific
     * region within the supplied buffer.
     *
     * @param buf
     *            the buffer to write compressed data to.
     * @param off
     *            the offset within {@code buf} at which to start writing to.
     * @param nbytes
     *            maximum number of bytes of compressed data to be written.
     * @return the number of bytes of compressed data written to {@code buf}.
     */
    // synchronized
    CARAPI Deflate(
        /* [out] */ ArrayOf<Byte>* buf,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 byteCount,
        /* [out] */ Int32* number);

    /**
     * Deflates data (previously passed to {@code setInput}) into a specific
     * region within the supplied buffer, optionally flushing the input buffer.
     *
     * @param buf the buffer to write compressed data to.
     * @param off the offset within {@code buf} at which to start writing to.
     * @param nbytes maximum number of bytes of compressed data to be written.
     * @param flush one of {@link #NO_FLUSH}, {@link #SYNC_FLUSH} or
     *      {@link #FULL_FLUSH}.
     * @return the number of compressed bytes written to {@code buf}. If this
     *      equals {@code nbytes}, the number of bytes of input to be flushed
     *      may have exceeded the output buffer's capacity. In this case,
     *      finishing a flush will require the output buffer to be drained
     *      and additional calls to {@link #deflate} to be made.
     * @hide
     * @since 1.7
     */
    // synchronized
    CARAPI Deflate(
        /* [out] */ ArrayOf<Byte>* buf,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 byteCount,
        /* [in] */ Int32 flush,
        /* [out] */ Int32* number);

    /**
     * Frees all resources held onto by this deflating algorithm. Any unused
     * input or output is discarded. While this method is used by {@code
     * finalize()}, it can be called explicitly in order to free native
     * resources before the next GC cycle. After {@code end()} was called other
     * methods will typically throw an {@code IllegalStateException}.
     */
    // synchronized
    CARAPI End();

    /**
     * Indicates to the {@code Deflater} that all uncompressed input has been provided
     * to it.
     *
     * @see #finished
     */
    // synchronized
    CARAPI Finish();

    /**
     * Returns whether or not all provided data has been successfully
     * compressed.
     *
     * @return true if all data has been compressed, false otherwise.
     */
    // synchronized
    CARAPI Finished(
        /* [out] */ Boolean* finished);

    /**
     * Returns the Adler32 checksum of uncompressed data currently read. If a
     * preset dictionary is used getAdler() will return the Adler32 checksum of
     * the dictionary used.
     *
     * @return the Adler32 checksum of uncompressed data or preset dictionary if
     *         used.
     * @see #setDictionary(byte[])
     * @see #setDictionary(byte[], int, int)
     */
    // synchronized
    CARAPI GetAdler(
        /* [out] */ Int32* checksum);

    /**
     * Returns the total number of bytes of input consumed by the {@code Deflater}.
     *
     * @return number of bytes of input read.
     */
    // synchronized
    CARAPI GetTotalIn(
        /* [out] */ Int32* number);

    /**
     * Returns the total number of compressed bytes output by this {@code Deflater}.
     *
     * @return number of compressed bytes output.
     */
    // synchronized
    CARAPI GetTotalOut(
        /* [out] */ Int32* number);

    /**
     * Counterpart to setInput(). Indicates whether or not all bytes of
     * uncompressed input have been consumed by the {@code Deflater}. If needsInput()
     * returns true setInput() must be called before deflation can continue. If
     * all bytes of uncompressed data have been provided to the {@code Deflater}
     * finish() must be called to ensure the compressed data is output.
     *
     * @return {@code true} if input is required for deflation to continue,
     *         {@code false} otherwise.
     * @see #finished()
     * @see #setInput(byte[])
     * @see #setInput(byte[], int, int)
     */
    // synchronized
    CARAPI NeedsInput(
        /* [out] */ Boolean* result);

    /**
     * Resets the {@code Deflater} to accept new input without affecting any
     * previously made settings for the compression strategy or level. This
     * operation <i>must</i> be called after {@code finished()} returns
     * {@code true} if the {@code Deflater} is to be reused.
     *
     * @see #finished
     */
    // synchronized
    CARAPI Reset();

    /**
     * Sets the dictionary to be used for compression by this {@code Deflater}.
     * setDictionary() can only be called if this {@code Deflater} supports the writing
     * of ZLIB headers. This is the default behaviour but can be overridden
     * using {@code Deflater(int, boolean)}.
     *
     * @param buf
     *            the buffer containing the dictionary data bytes.
     * @see Deflater#Deflater(int, boolean)
     */
    // synchronized
    CARAPI SetDictionary(
        /* [in] */ ArrayOf<Byte>* buf);

    /**
     * Sets the dictionary to be used for compression by this {@code Deflater}.
     * setDictionary() can only be called if this {@code Deflater} supports the writing
     * of ZLIB headers. This is the default behaviour but can be overridden
     * using {@code Deflater(int, boolean)}.
     *
     * @param buf
     *            the buffer containing the dictionary data bytes.
     * @param off
     *            the offset of the data.
     * @param nbytes
     *            the length of the data.
     * @see Deflater#Deflater(int, boolean)
     */
    // synchronized
    CARAPI SetDictionary(
        /* [in] */ ArrayOf<Byte>* buf,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 byteCount);

    /**
     * Sets the input buffer the {@code Deflater} will use to extract uncompressed bytes
     * for later compression.
     *
     * @param buf
     *            the buffer.
     */
    CARAPI SetInput(
        /* [in] */ ArrayOf<Byte>* buf);

    /* Sets the input buffer the {@code Deflater} will use to extract uncompressed bytes
     * for later compression. Input will be taken from the buffer region
     * starting at off and ending at nbytes - 1.
     *
     * @param buf
     *            the buffer containing the input data bytes.
     * @param off
     *            the offset of the data.
     * @param nbytes
     *            the length of the data.
     */
    // synchronized
    CARAPI SetInput(
        /* [in] */ ArrayOf<Byte>* buf,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 byteCount);

    /**
     * Sets the compression level to be used when compressing data. The
     * compression level must be a value between 0 and 9. This value must be set
     * prior to calling setInput().
     *
     * @param level
     *            compression level to use
     * @exception IllegalArgumentException
     *                If the compression level is invalid.
     */
    // synchronized
    CARAPI SetLevel(
        /* [in] */ Int32 level);

    /**
     * Sets the compression strategy to be used. The strategy must be one of
     * FILTERED, HUFFMAN_ONLY or DEFAULT_STRATEGY.This value must be set prior
     * to calling setInput().
     *
     * @param strategy
     *            compression strategy to use
     * @exception IllegalArgumentException
     *                If the strategy specified is not one of FILTERED,
     *                HUFFMAN_ONLY or DEFAULT_STRATEGY.
     */
    // synchronized
    CARAPI SetStrategy(
        /* [in] */ Int32 strategy);

    /**
     * Returns a long int of total number of bytes read by the {@code Deflater}. This
     * method performs the same as {@code getTotalIn} except it returns a long value
     * instead of an integer
     *
     * @see #getTotalIn()
     * @return total number of bytes read by {@code Deflater}.
     */
    // synchronized
    CARAPI GetBytesRead(
        /* [out] */ Int64* number);

    /**
     * Returns a long int of total number of bytes of read by the {@code Deflater}. This
     * method performs the same as {@code getTotalOut} except it returns a long
     * value instead of an integer
     *
     * @see #getTotalOut()
     * @return bytes exactly write by {@code Deflater}
     */
    // synchronized
    CARAPI GetBytesWritten(
        /* [out] */ Int64* number);

private:
    CARAPI DeflateImplLocked(
        /* [in] */ Int32 offset,
        /* [in] */ Int32 byteCount,
        /* [in] */ Int32 flush,
        /* [out] */ ArrayOf<Byte>* buf,
        /* [out] */ Int32* number);

    CARAPI_(Int32) DeflateImplLocked(
        /* [in] */ Int32 offset,
        /* [in] */ Int32 byteCount,
        /* [in] */ Int32 flushParm,
        /* [in] */ NativeZipStream* stream,
        /* [out] */ ArrayOf<Byte>* buf);

    CARAPI_(void) EndImplLocked(
        /* [in] */ NativeZipStream* stream);

    CARAPI_(void) EndImplLocked();

    CARAPI_(Int32) GetAdlerImplLocked(
        /* [in] */ NativeZipStream* stream);

    CARAPI_(Int64) GetTotalInImplLocked(
        /* [in] */ NativeZipStream* stream);

    CARAPI_(Int64) GetTotalOutImplLocked(
        /* [in] */ NativeZipStream* stream);

    CARAPI ResetImplLocked(
        /* [in] */ NativeZipStream* stream);

    CARAPI_(void) SetDictionaryImplLocked(
        /* [in] */ ArrayOf<Byte>* buf,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 byteCount,
        /* [in] */ NativeZipStream* stream);

    CARAPI SetLevelsImplLocked(
        /* [in] */ Int32 level,
        /* [in] */ Int32 strategy,
        /* [in] */ NativeZipStream* stream);

    CARAPI_(void) SetInputImplLocked(
        /* [in] */ ArrayOf<Byte>* buf,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 byteCount,
        /* [in] */ NativeZipStream* stream);

    CARAPI CreateStream(
        /* [in] */ Int32 level,
        /* [in] */ Int32 strategy,
        /* [in] */ Boolean noHeader,
        /* [out] */ NativeZipStream** stream);

    CARAPI CheckOpen();

private:
    /**
     * Flush buffers and mark the end of the datastream.
     */
    static const Int32 FINISH = 4;

    Int32 mFlushParm;

    Boolean mFinished;

    Int32 mCompressLevel;

    Int32 mStrategy;

    AutoPtr<NativeZipStream> mStreamHandle;

    AutoPtr<ArrayOf<Byte> > mInputBuffer;

    Int32 mInRead;

    Int32 mInLength;

    AutoPtr<ICloseGuard> mGuard;
};

} // namespace Zip
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_ZIP_ELASTOS_UTILITY_CDEFLATER_H__
