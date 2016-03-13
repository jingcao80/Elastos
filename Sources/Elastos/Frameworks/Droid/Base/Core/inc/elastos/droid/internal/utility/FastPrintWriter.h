#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_FASTPRINTWRITER_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_FASTPRINTWRITER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Utility.h"
#include <elastos/core/Object.h>
#include <elastos/io/PrintWriter.h>

using Elastos::Droid::Utility::IPrinter;
using Elastos::IO::Charset::ICharsetEncoder;
using Elastos::IO::IByteBuffer;
using Elastos::IO::IOutputStream;
using Elastos::IO::IWriter;
using Elastos::IO::PrintWriter;
using Elastos::IO::Writer;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

class FastPrintWriter
    : public PrintWriter
    , public IFastPrintWriter
{
private:
    class DummyWriter : public Writer
    {
    public:
        // Override
        CARAPI Close();

        // Override
        CARAPI Flush();

        // Override
        CARAPI Write(
            /* [in] */ ArrayOf<Char32>* buffer,
            /* [in] */ Int32 offset,
            /* [in] */ Int32 count);
    };

public:
    FastPrintWriter();

    CAR_INTERFACE_DECL()

    /**
     * Constructs a new {@code PrintWriter} with {@code out} as its target
     * stream. By default, the new Print writer does not automatically flush its
     * contents to the target stream when a newline is encountered.
     *
     * @param out
     *            the target output stream.
     * @throws NullPointerException
     *             if {@code out} is {@code NULL}.
     */
    CARAPI constructor(
        /* [in] */ IOutputStream* out);

    /**
     * Constructs a new {@code PrintWriter} with {@code out} as its target
     * stream. The parameter {@code autoFlush} determines if the Print writer
     * automatically flushes its contents to the target stream when a newline is
     * encountered.
     *
     * @param out
     *            the target output stream.
     * @param autoFlush
     *            indicates whether contents are flushed upon encountering a
     *            newline sequence.
     * @throws NullPointerException
     *             if {@code out} is {@code NULL}.
     */
    CARAPI constructor(
        /* [in] */ IOutputStream* out,
        /* [in] */ Boolean autoFlush);

    /**
     * Constructs a new {@code PrintWriter} with {@code out} as its target
     * stream and a custom buffer size. The parameter {@code autoFlush} determines
     * if the Print writer automatically flushes its contents to the target stream
     * when a newline is encountered.
     *
     * @param out
     *            the target output stream.
     * @param autoFlush
     *            indicates whether contents are flushed upon encountering a
     *            newline sequence.
     * @param bufferLen
     *            specifies the size of the FastPrintWriter's internal buffer; the
     *            default is 8192.
     * @throws NullPointerException
     *             if {@code out} is {@code NULL}.
     */
    CARAPI constructor(
        /* [in] */ IOutputStream* out,
        /* [in] */ Boolean autoFlush,
        /* [in] */ Int32 bufferLen);

    /**
     * Constructs a new {@code PrintWriter} with {@code wr} as its target
     * writer. By default, the new Print writer does not automatically flush its
     * contents to the target writer when a newline is encountered.
     *
     * <p>NOTE: Unlike PrintWriter, this version will still do buffering inside of
     * FastPrintWriter before sending data to the Writer.  This means you must call
     * flush() before retrieving any data from the Writer.</p>
     *
     * @param wr
     *            the target writer.
     * @throws NullPointerException
     *             if {@code wr} is {@code NULL}.
     */
    CARAPI constructor(
        /* [in] */ IWriter* wr);

    /**
     * Constructs a new {@code PrintWriter} with {@code wr} as its target
     * writer. The parameter {@code autoFlush} determines if the Print writer
     * automatically flushes its contents to the target writer when a newline is
     * encountered.
     *
     * @param wr
     *            the target writer.
     * @param autoFlush
     *            indicates whether to flush contents upon encountering a
     *            newline sequence.
     * @throws NullPointerException
     *             if {@code out} is {@code NULL}.
     */
    CARAPI constructor(
        /* [in] */ IWriter* wr,
        /* [in] */ Boolean autoFlush);

    /**
     * Constructs a new {@code PrintWriter} with {@code wr} as its target
     * writer and a custom buffer size. The parameter {@code autoFlush} determines
     * if the Print writer automatically flushes its contents to the target writer
     * when a newline is encountered.
     *
     * @param wr
     *            the target writer.
     * @param autoFlush
     *            indicates whether to flush contents upon encountering a
     *            newline sequence.
     * @param bufferLen
     *            specifies the size of the FastPrintWriter's internal buffer; the
     *            default is 8192.
     * @throws NullPointerException
     *             if {@code wr} is {@code NULL}.
     */
    CARAPI constructor(
        /* [in] */ IWriter* wr,
        /* [in] */ Boolean autoFlush,
        /* [in] */ Int32 bufferLen);

    /**
     * Constructs a new {@code PrintWriter} with {@code pr} as its target
     * printer and the default buffer size.  Because a {@link Printer} is line-base,
     * autoflush is always enabled.
     *
     * @param pr
     *            the target writer.
     * @throws NullPointerException
     *             if {@code pr} is {@code NULL}.
     */
    CARAPI constructor(
        /* [in] */ IPrinter* pr);

    /**
     * Constructs a new {@code PrintWriter} with {@code pr} as its target
     * printer and a custom buffer size.  Because a {@link Printer} is line-base,
     * autoflush is always enabled.
     *
     * @param pr
     *            the target writer.
     * @param bufferLen
     *            specifies the size of the FastPrintWriter's internal buffer; the
     *            default is 512.
     * @throws NullPointerException
     *             if {@code pr} is {@code NULL}.
     */
    CARAPI constructor(
        /* [in] */ IPrinter* pr,
        /* [in] */ Int32 bufferLen);

    /**
     * Flushes this writer and returns the value of the error flag.
     *
     * @return {@code TRUE} if either an {@code IOException} has been thrown
     *         previously or if {@code setError()} has been called;
     *         {@code FALSE} otherwise.
     * @see #setError()
     */
    CARAPI CheckError(
        /* [out] */ Boolean* hasError);

    /**
     * Ensures that all pending data is sent out to the target. It also
     * flushes the target. If an I/O error occurs, this writer's error
     * state is set to {@code TRUE}.
     */
    // @Override
    CARAPI Flush();

    // @Override
    CARAPI Close();

    /**
     * Prints the string representation of the specified character array
     * to the target.
     *
     * @param charArray
     *            the character array to Print to the target.
     * @see #Print(String)
     */
    CARAPI Print(
        /* [in] */ ArrayOf<Char32>* charArray);

    /**
     * Prints the string representation of the specified character to the
     * target.
     *
     * @param ch
     *            the character to Print to the target.
     * @see #Print(String)
     */
    CARAPI PrintChar(
        /* [in] */ Char32 ch);

    /**
     * Prints a string to the target. The string is converted to an array of
     * bytes using the encoding chosen during the construction of this writer.
     * The bytes are then written to the target with {@code write(Int32)}.
     * <p>
     * If an I/O error occurs, this writer's error flag is set to {@code TRUE}.
     *
     * @param str
     *            the string to Print to the target.
     * @see #write(Int32)
     */
    CARAPI Print(
        /* [in] */ const String& str);

    // @Override
    CARAPI Print(
        /* [in] */ Int32 inum);

    // @Override
    CARAPI Print(
        /* [in] */ Int64 lnum);

    /**
     * Prints a newline. Flushes this writer if the autoFlush flag is set to {@code TRUE}.
     */
    CARAPI Println();

    // @Override
    CARAPI Println(
        /* [in] */ Int32 inum);

    // @Override
    CARAPI Println(
        /* [in] */ Int64 lnum);

    /**
     * Prints the string representation of the character array {@code chars} followed by a newline.
     * Flushes this writer if the autoFlush flag is set to {@code TRUE}.
     */
    CARAPI Println(
        /* [in] */ ArrayOf<Char32>* chars);

    /**
     * Prints the string representation of the Char32 {@code c} followed by a newline.
     * Flushes this writer if the autoFlush flag is set to {@code TRUE}.
     */
    CARAPI PrintCharln(
        /* [in] */ Char32 c);

    /**
     * Writes {@code count} characters from {@code buffer} starting at {@code
     * offset} to the target.
     * <p>
     * This writer's error flag is set to {@code TRUE} if this writer is closed
     * or an I/O error occurs.
     *
     * @param buf
     *            the buffer to write to the target.
     * @param offset
     *            the index of the first character in {@code buffer} to write.
     * @param count
     *            the number of characters in {@code buffer} to write.
     * @throws IndexOutOfBoundsException
     *             if {@code offset < 0} or {@code count < 0}, or if {@code
     *             offset + count} is greater than the length of {@code buf}.
     */
    // @Override
    CARAPI Write(
        /* [in] */ ArrayOf<Char32>* buf,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count);

    /**
     * Writes one character to the target. Only the two least significant bytes
     * of the integer {@code oneChar} are written.
     * <p>
     * This writer's error flag is set to {@code TRUE} if this writer is closed
     * or an I/O error occurs.
     *
     * @param oneChar
     *            the character to write to the target.
     */
    // @Override
    CARAPI Write(
        /* [in] */ Int32 oneChar);

    /**
     * Writes the characters from the specified string to the target.
     *
     * @param str
     *            the non-NULL string containing the characters to write.
     */
    // @Override
    CARAPI Write(
        /* [in] */ const String& str);

    /**
     * Writes {@code count} characters from {@code str} starting at {@code
     * offset} to the target.
     *
     * @param str
     *            the non-NULL string containing the characters to write.
     * @param offset
     *            the index of the first character in {@code str} to write.
     * @param count
     *            the number of characters from {@code str} to write.
     * @throws IndexOutOfBoundsException
     *             if {@code offset < 0} or {@code count < 0}, or if {@code
     *             offset + count} is greater than the length of {@code str}.
     */
    // @Override
    CARAPI Write(
        /* [in] */ const String& str,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count);

    /**
     * Appends a subsequence of the character sequence {@code csq} to the
     * target. This method works the same way as {@code
     * PrintWriter.Print(csq.subsequence(start, end).toString())}. If {@code
     * csq} is {@code NULL}, then the specified subsequence of the string "NULL"
     * will be written to the target.
     *
     * @param csq
     *            the character sequence appended to the target.
     * @param start
     *            the index of the first Char32 in the character sequence appended
     *            to the target.
     * @param end
     *            the index of the character following the last character of the
     *            subsequence appended to the target.
     * @return this writer.
     * @throws StringIndexOutOfBoundsException
     *             if {@code start > end}, {@code start < 0}, {@code end < 0} or
     *             either {@code start} or {@code end} are greater or equal than
     *             the length of {@code csq}.
     */
    // @Override
    CARAPI Append(
        /* [in] */ ICharSequence* csq,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

protected:
    /**
     * Sets the error state of the stream to FALSE.
     * @since 1.6
     */
    CARAPI ClearError();

    /**
     * Sets the error flag of this writer to TRUE.
     */
    CARAPI_(void) SetError();

    CARAPI InitEncoder(
        /* [in] */ const String& csn);

private:
    CARAPI InitDefaultEncoder();

    CARAPI AppendLocked(
        /* [in] */ Char32 c);

    CARAPI AppendLocked(
        /* [in] */ const String& str,
        /* [in] */ Int32 i,
        /* [in] */ Int32 length);

    CARAPI AppendLocked(
        /* [in] */ ArrayOf<Char32>* buf,
        /* [in] */ Int32 i,
        /* [in] */ Int32 length);

    CARAPI FlushBytesLocked();

    CARAPI FlushLocked();

private:
    static AutoPtr<IWriter> sDummyWriter;
    Int32 mBufferLen;
    AutoPtr<ArrayOf<Char32> > mText;
    Int32 mPos;

    AutoPtr<IOutputStream> mOutputStream;
    Boolean mAutoFlush;
    String mSeparator;

    AutoPtr<IWriter> mWriter;
    AutoPtr<IPrinter> mPrinter;

    AutoPtr<ICharsetEncoder> mCharset;
    AutoPtr<IByteBuffer> mBytes;

    Boolean mIoError;
};

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_FASTPRINTWRITER_H__
