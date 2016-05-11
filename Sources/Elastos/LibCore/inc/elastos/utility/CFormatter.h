
#ifndef __ELASTOS_UTILITY_CFORMATTER_H__
#define __ELASTOS_UTILITY_CFORMATTER_H__

#include "Elastos.CoreLibrary.Libcore.h"
#include "_Elastos_Utility_CFormatter.h"
#include "StringBuilder.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Core::IAppendable;
using Elastos::Core::StringBuilder;
using Elastos::Core::ICharSequence;
using Libcore::ICU::ILocaleData;
using Libcore::ICU::INativeDecimalFormat;
using Elastos::IO::IFile;
using Elastos::IO::IOutputStream;
using Elastos::IO::IPrintStream;
using Elastos::IO::ICloseable;
using Elastos::IO::IFlushable;
using Elastos::Utility::ILocale;

namespace Elastos {
namespace Utility {

CarClass(CFormatter)
    , public Object
    , public IFormatter
    , public ICloseable
    , public IFlushable
{
public:
    /**
     * The enumeration giving the available styles for formatting very large
     * decimal numbers.
     */
    enum BigDecimalLayoutForm {
        /**
         * Use scientific style for BigDecimals.
         */
        SCIENTIFIC,
        /**
         * Use normal decimal/float style for BigDecimals.
         */
        DECIMAL_FLOAT
    };

    class CachedDecimalFormat : public Object
    {
    public:
        CachedDecimalFormat();

        AutoPtr<INativeDecimalFormat> Update(
            /* [in] */ ILocaleData* localeData,
            /* [in] */ const String& pattern);

    public:
        AutoPtr<INativeDecimalFormat> mDecimalFormat;
        AutoPtr<ILocaleData> mCurrentLocaleData;
        String mCurrentPattern;
    };

private:

    /*
     * Complete details of a single format specifier parsed from a format string.
     */
    class FormatToken : public Object
    {
    public:
        FormatToken();

        // Tests whether there were no flags, no width, and no precision specified.
        CARAPI_(Boolean) IsDefault();

        CARAPI_(Boolean) IsPrecisionSet();

        CARAPI_(Int32) GetArgIndex();

        CARAPI_(void) SetArgIndex(
            /* [in] */ Int32 index);

        CARAPI_(Int32) GetWidth();

        CARAPI_(void) SetWidth(
            /* [in] */ Int32 width);

        CARAPI_(Int32) GetPrecision();

        CARAPI_(void) SetPrecision(
            /* [in] */ Int32 precise);

        CARAPI_(String) GetStrFlags();

        /*
         * Sets qualified char as one of the flags. If the char is qualified,
         * sets it as a flag and returns true. Or else returns false.
         */
        CARAPI_(Boolean) SetFlag(
            /* [in] */ Int32 ch);

        CARAPI_(Char32) GetConversionType();

        CARAPI_(void) SetConversionType(
            /* [in] */ Char32 c);

        CARAPI_(Char32) GetDateSuffix();

        CARAPI_(void) SetDateSuffix(
            /* [in] */ Char32 c);

        CARAPI_(Boolean) RequireArgument();

        CARAPI CheckFlags(
            /* [in] */ IInterface* arg);

        CARAPI UnknownFormatConversionException();

    public:
        static const Int32 LAST_ARGUMENT_INDEX; // = -2;

        static const Int32 UNSET; // = -1;

        static const Int32 FLAGS_UNSET; // = 0;

        static const Int32 DEFAULT_PRECISION; // = 6;

        static const Int32 FLAG_ZERO; // = 1 << 4;

        // These have package access for performance. They used to be represented by an int bitmask
        // and accessed via methods, but Android's JIT doesn't yet do a good job of such code.
        // Direct field access, on the other hand, is fast.
        Boolean mFlagComma;
        Boolean mFlagMinus;
        Boolean mFlagParenthesis;
        Boolean mFlagPlus;
        Boolean mFlagSharp;
        Boolean mFlagSpace;
        Boolean mFlagZero;

    private:
        Int32 mArgIndex; // = UNSET;

        Char32 mConversionType; // = UNSET;
        Char32 mDateSuffix;

        Int32 mPrecision; // = UNSET;
        Int32 mWidth; // = UNSET;

        StringBuilder mStrFlags;
    };

    class FormatSpecifierParser : public Object
    {
    public:
        /**
         * Constructs a new parser for the given format string.
         */
        FormatSpecifierParser(
            /* [in] */ const String& format);

        /**
         * Returns a FormatToken representing the format specifier starting at 'offset'.
         * @param offset the first character after the '%'
         */
        CARAPI_(AutoPtr<FormatToken>) ParseFormatToken(
            /* [in] */ Int32 offset);

        /**
         * Returns a string corresponding to the last format specifier that was parsed.
         * Used to construct error messages.
         */
        CARAPI_(String) GetFormatSpecifierText();

    private:
        CARAPI_(Int32) Peek();

        CARAPI_(Char32) Advance();

        CARAPI UnknownFormatConversionException();

        CARAPI_(AutoPtr<FormatToken>) ParseArgumentIndexAndFlags(
            /* [in] */ FormatToken* token);

        // We pass the width in because in some cases we've already parsed it.
        // (Because of the ambiguity between argument indexes and widths.)
        CARAPI_(AutoPtr<FormatToken>) ParseWidth(
            /* [in] */ FormatToken* token,
            /* [in] */ Int32 width);

        CARAPI_(AutoPtr<FormatToken>) ParsePrecision(
            /* [in] */ FormatToken* token);

        CARAPI_(AutoPtr<FormatToken>) ParseConversionType(
            /* [in] */ FormatToken* token);

        // Parses an integer (of arbitrary length, but typically just one digit).
        CARAPI_(Int32) NextInt();

        // Swallow remaining digits to resync our attempted parse, but return failure.
        CARAPI_(Int32) FailNextInt();

    private:
        String mFormat;
        Int32 mLength;

        Int32 mStartIndex;
    public:
        Int32 mI;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CFormatter();

    /**
     * Constructs a {@code Formatter}.
     *
     * <p>The output is written to a {@code StringBuilder} which can be acquired by invoking
     * {@link #out()} and whose content can be obtained by calling {@code toString}.
     *
     * <p>The {@code Locale} used is the user's default locale.
     * See "<a href="../util/Locale.html#default_locale">Be wary of the default locale</a>".
     */
    CARAPI constructor();

    /**
     * Constructs a {@code Formatter} whose output will be written to the
     * specified {@code Appendable}.
     *
     * <p>The {@code Locale} used is the user's default locale.
     * See "<a href="../util/Locale.html#default_locale">Be wary of the default locale</a>".
     *
     * @param a
     *            the output destination of the {@code Formatter}. If {@code a} is {@code null},
     *            then a {@code StringBuilder} will be used.
     */
    CARAPI constructor(
        /* [in] */ IAppendable* a);

    /**
     * Constructs a {@code Formatter} with the specified {@code Locale}.
     *
     * <p>The output is written to a {@code StringBuilder} which can be acquired by invoking
     * {@link #out()} and whose content can be obtained by calling {@code toString}.
     *
     * @param l
     *            the {@code Locale} of the {@code Formatter}. If {@code l} is {@code null},
     *            then no localization will be used.
     */
    CARAPI constructor(
        /* [in] */ ILocale* l);

    /**
     * Constructs a {@code Formatter} with the specified {@code Locale}
     * and whose output will be written to the
     * specified {@code Appendable}.
     *
     * @param a
     *            the output destination of the {@code Formatter}. If {@code a} is {@code null},
     *            then a {@code StringBuilder} will be used.
     * @param l
     *            the {@code Locale} of the {@code Formatter}. If {@code l} is {@code null},
     *            then no localization will be used.
     */
    CARAPI constructor(
        /* [in] */ IAppendable* a,
        /* [in] */ ILocale* l);

    /**
     * Constructs a {@code Formatter} whose output is written to the specified file.
     *
     * <p>The charset of the {@code Formatter} is the default charset.
     *
     * <p>The {@code Locale} used is the user's default locale.
     * See "<a href="../util/Locale.html#default_locale">Be wary of the default locale</a>".
     *
     * @param fileName
     *            the filename of the file that is used as the output
     *            destination for the {@code Formatter}. The file will be truncated to
     *            zero size if the file exists, or else a new file will be
     *            created. The output of the {@code Formatter} is buffered.
     * @throws FileNotFoundException
     *             if the filename does not denote a normal and writable file,
     *             or if a new file cannot be created, or if any error arises when
     *             opening or creating the file.
     */
    CARAPI constructor(
        /* [in] */ const String& fileName);

    /**
     * Constructs a {@code Formatter} whose output is written to the specified file.
     *
     * <p>The {@code Locale} used is the user's default locale.
     * See "<a href="../util/Locale.html#default_locale">Be wary of the default locale</a>".
     *
     * @param fileName
     *            the filename of the file that is used as the output
     *            destination for the {@code Formatter}. The file will be truncated to
     *            zero size if the file exists, or else a new file will be
     *            created. The output of the {@code Formatter} is buffered.
     * @param csn
     *            the name of the charset for the {@code Formatter}.
     * @throws FileNotFoundException
     *             if the filename does not denote a normal and writable file,
     *             or if a new file cannot be created, or if any error arises when
     *             opening or creating the file.
     * @throws UnsupportedEncodingException
     *             if the charset with the specified name is not supported.
     */
    CARAPI constructor(
        /* [in] */ const String& fileName,
        /* [in] */ const String& csn);

    /**
     * Constructs a {@code Formatter} with the given {@code Locale} and charset,
     * and whose output is written to the specified file.
     *
     * @param fileName
     *            the filename of the file that is used as the output
     *            destination for the {@code Formatter}. The file will be truncated to
     *            zero size if the file exists, or else a new file will be
     *            created. The output of the {@code Formatter} is buffered.
     * @param csn
     *            the name of the charset for the {@code Formatter}.
     * @param l
     *            the {@code Locale} of the {@code Formatter}. If {@code l} is {@code null},
     *            then no localization will be used.
     * @throws FileNotFoundException
     *             if the filename does not denote a normal and writable file,
     *             or if a new file cannot be created, or if any error arises when
     *             opening or creating the file.
     * @throws UnsupportedEncodingException
     *             if the charset with the specified name is not supported.
     */
    CARAPI constructor(
        /* [in] */ const String& fileName,
        /* [in] */ const String& csn,
        /* [in] */ ILocale* l);

    /**
     * Constructs a {@code Formatter} whose output is written to the specified {@code File}.
     *
     * The charset of the {@code Formatter} is the default charset.
     *
     * <p>The {@code Locale} used is the user's default locale.
     * See "<a href="../util/Locale.html#default_locale">Be wary of the default locale</a>".
     *
     * @param file
     *            the {@code File} that is used as the output destination for the
     *            {@code Formatter}. The {@code File} will be truncated to zero size if the {@code File}
     *            exists, or else a new {@code File} will be created. The output of the
     *            {@code Formatter} is buffered.
     * @throws FileNotFoundException
     *             if the {@code File} is not a normal and writable {@code File}, or if a
     *             new {@code File} cannot be created, or if any error rises when opening or
     *             creating the {@code File}.
     */
    CARAPI constructor(
        /* [in] */ IFile* file);

    /**
     * Constructs a {@code Formatter} with the given charset,
     * and whose output is written to the specified {@code File}.
     *
     * <p>The {@code Locale} used is the user's default locale.
     * See "<a href="../util/Locale.html#default_locale">Be wary of the default locale</a>".
     *
     * @param file
     *            the {@code File} that is used as the output destination for the
     *            {@code Formatter}. The {@code File} will be truncated to zero size if the {@code File}
     *            exists, or else a new {@code File} will be created. The output of the
     *            {@code Formatter} is buffered.
     * @param csn
     *            the name of the charset for the {@code Formatter}.
     * @throws FileNotFoundException
     *             if the {@code File} is not a normal and writable {@code File}, or if a
     *             new {@code File} cannot be created, or if any error rises when opening or
     *             creating the {@code File}.
     * @throws UnsupportedEncodingException
     *             if the charset with the specified name is not supported.
     */
    CARAPI constructor(
        /* [in] */ IFile* file,
        /* [in] */ const String& csn);

    /**
     * Constructs a {@code Formatter} with the given {@code Locale} and charset,
     * and whose output is written to the specified {@code File}.
     *
     * @param file
     *            the {@code File} that is used as the output destination for the
     *            {@code Formatter}. The {@code File} will be truncated to zero size if the {@code File}
     *            exists, or else a new {@code File} will be created. The output of the
     *            {@code Formatter} is buffered.
     * @param csn
     *            the name of the charset for the {@code Formatter}.
     * @param l
     *            the {@code Locale} of the {@code Formatter}. If {@code l} is {@code null},
     *            then no localization will be used.
     * @throws FileNotFoundException
     *             if the {@code File} is not a normal and writable {@code File}, or if a
     *             new {@code File} cannot be created, or if any error rises when opening or
     *             creating the {@code File}.
     * @throws UnsupportedEncodingException
     *             if the charset with the specified name is not supported.
     */
    CARAPI constructor(
        /* [in] */ IFile* file,
        /* [in] */ const String& csn,
        /* [in] */ ILocale* l);

    /**
     * Constructs a {@code Formatter} whose output is written to the specified {@code OutputStream}.
     *
     * <p>The charset of the {@code Formatter} is the default charset.
     *
     * <p>The {@code Locale} used is the user's default locale.
     * See "<a href="../util/Locale.html#default_locale">Be wary of the default locale</a>".
     *
     * @param os
     *            the stream to be used as the destination of the {@code Formatter}.
     */
    CARAPI constructor(
        /* [in] */ IOutputStream* os);

    /**
     * Constructs a {@code Formatter} with the given charset,
     * and whose output is written to the specified {@code OutputStream}.
     *
     * <p>The {@code Locale} used is the user's default locale.
     * See "<a href="../util/Locale.html#default_locale">Be wary of the default locale</a>".
     *
     * @param os
     *            the stream to be used as the destination of the {@code Formatter}.
     * @param csn
     *            the name of the charset for the {@code Formatter}.
     * @throws UnsupportedEncodingException
     *             if the charset with the specified name is not supported.
     */
    CARAPI constructor(
        /* [in] */ IOutputStream* os,
        /* [in] */ const String& csn);

    /**
     * Constructs a {@code Formatter} with the given {@code Locale} and charset,
     * and whose output is written to the specified {@code OutputStream}.
     *
     * @param os
     *            the stream to be used as the destination of the {@code Formatter}.
     * @param csn
     *            the name of the charset for the {@code Formatter}.
     * @param l
     *            the {@code Locale} of the {@code Formatter}. If {@code l} is {@code null},
     *            then no localization will be used.
     * @throws UnsupportedEncodingException
     *             if the charset with the specified name is not supported.
     */
    CARAPI constructor(
        /* [in] */ IOutputStream* os,
        /* [in] */ const String& csn,
        /* [in] */ ILocale* l);

    /**
     * Constructs a {@code Formatter} whose output is written to the specified {@code PrintStream}.
     *
     * <p>The charset of the {@code Formatter} is the default charset.
     *
     * <p>The {@code Locale} used is the user's default locale.
     * See "<a href="../util/Locale.html#default_locale">Be wary of the default locale</a>".
     *
     * @param ps
     *            the {@code PrintStream} used as destination of the {@code Formatter}. If
     *            {@code ps} is {@code null}, then a {@code NullPointerException} will
     *            be raised.
     */
    CARAPI constructor(
        /* [in] */ IPrintStream* ps);

    /**
     * Writes a formatted string to the output destination of the {@code Formatter}.
     *
     * @param format
     *            a format string.
     * @param args
     *            the arguments list used in the {@code format()} method. If there are
     *            more arguments than those specified by the format string, then
     *            the additional arguments are ignored.
     * @return this {@code Formatter}.
     * @throws IllegalFormatException
     *             if the format string is illegal or incompatible with the
     *             arguments, or if fewer arguments are sent than those required by
     *             the format string, or any other illegal situation.
     * @throws FormatterClosedException
     *             if the {@code Formatter} has been closed.
     */
    CARAPI Format(
        /* [in] */ const String& format,
        /* [in] */ ArrayOf<IInterface*>* args);

    /**
     * Writes a formatted string to the output destination of the {@code Formatter}.
     *
     * @param l
     *            the {@code Locale} used in the method. If {@code locale} is
     *            {@code null}, then no localization will be applied. This
     *            parameter does not change this Formatter's default {@code Locale}
     *            as specified during construction, and only applies for the
     *            duration of this call.
     * @param format
     *            a format string.
     * @param args
     *            the arguments list used in the {@code format()} method. If there are
     *            more arguments than those specified by the format string, then
     *            the additional arguments are ignored.
     * @return this {@code Formatter}.
     * @throws IllegalFormatException
     *             if the format string is illegal or incompatible with the
     *             arguments, or if fewer arguments are sent than those required by
     *             the format string, or any other illegal situation.
     * @throws FormatterClosedException
     *             if the {@code Formatter} has been closed.
     */
    CARAPI Format(
        /* [in] */ ILocale* l,
        /* [in] */ const String& format,
        /* [in] */ ArrayOf<IInterface*>* args);

    /**
     * Returns the content by calling the {@code toString()} method of the output
     * destination.
     *
     * @return the content by calling the {@code toString()} method of the output
     *         destination.
     * @throws FormatterClosedException
     *             if the {@code Formatter} has been closed.
     */
    CARAPI ToString(
        /* [out] */ String* str);

    /**
     * Closes the {@code Formatter}. If the output destination is {@link Closeable},
     * then the method {@code close()} will be called on that destination.
     *
     * If the {@code Formatter} has been closed, then calling the this method will have no
     * effect.
     *
     * Any method but the {@link #ioException()} that is called after the
     * {@code Formatter} has been closed will raise a {@code FormatterClosedException}.
     */
    CARAPI Close();

    /**
     * Flushes the {@code Formatter}. If the output destination is {@link Flushable},
     * then the method {@code flush()} will be called on that destination.
     *
     * @throws FormatterClosedException
     *             if the {@code Formatter} has been closed.
     */
    CARAPI Flush();

    /**
     * Returns the {@code Locale} of the {@code Formatter}.
     *
     * @return the {@code Locale} for the {@code Formatter} or {@code null} for no {@code Locale}.
     * @throws FormatterClosedException
     *             if the {@code Formatter} has been closed.
     */
    CARAPI GetLocale(
        /* [out] */ ILocale** outlocale);

    /**
     * Returns the output destination of the {@code Formatter}.
     *
     * @return the output destination of the {@code Formatter}.
     * @throws FormatterClosedException
     *             if the {@code Formatter} has been closed.
     */
    CARAPI GetOut(
        /* [out] */ IAppendable** outapp);

    /**
     * Returns the last {@code IOException} thrown by the {@code Formatter}'s output
     * destination. If the {@code append()} method of the destination does not throw
     * {@code IOException}s, the {@code ioException()} method will always return {@code null}.
     *
     * @return the last {@code IOException} thrown by the {@code Formatter}'s output
     *         destination.
     */
    CARAPI GetIoException();

private:
    CARAPI_(AutoPtr<INativeDecimalFormat>) GetDecimalFormat(
        /* [in] */ const String& pattern);

    CARAPI CheckNotClosed();

    CARAPI DoFormat(
        /* [in] */ const String& format,
        /* [in] */ ArrayOf<IInterface*>* args);

    CARAPI_(void) OutputCharSequence(
        /* [in] */ ICharSequence* cs,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    CARAPI GetArgument(
        /* [in] */ ArrayOf<IInterface*>* args,
        /* [in] */ Int32 index,
        /* [in] */ FormatSpecifierParser* fsp,
        /* [in] */ IInterface* lastArgument,
        /* [in] */ Boolean hasLastArgumentSet,
        /* [out] */ IInterface** argument);

    CARAPI_(AutoPtr<ICharSequence>) Transform(
        /* [in] */ FormatToken* token,
        /* [in] */ IInterface* argument);

    CARAPI BadArgumentType();

    CARAPI_(AutoPtr<ICharSequence>) LocalizeDigits(
        /* [in] */ ICharSequence* s);

    CARAPI_(AutoPtr<ICharSequence>) InsertGrouping(
        /* [in] */ ICharSequence* s);

    CARAPI_(AutoPtr<ICharSequence>) TransformFromBoolean();

    CARAPI_(AutoPtr<ICharSequence>) TransformFromHashCode();

    CARAPI_(AutoPtr<ICharSequence>) TransformFromString();

    CARAPI_(AutoPtr<ICharSequence>) TransformFromCharacter();

    CARAPI_(AutoPtr<ICharSequence>) TransformFromPercent();

    CARAPI_(AutoPtr<ICharSequence>) Padding(
        /* [in] */ ICharSequence* source,
        /* [in] */ Int32 startIndex);

    CARAPI_(AutoPtr<StringBuilder>) ToStringBuilder(
        /* [in] */ ICharSequence* cs);

    CARAPI_(AutoPtr<StringBuilder>) WrapParentheses(
        /* [in] */ StringBuilder* result);

    CARAPI_(AutoPtr<ICharSequence>) TransformFromInteger();

    CARAPI_(AutoPtr<ICharSequence>) TransformFromNull();

    CARAPI_(AutoPtr<ICharSequence>) TransformFromBigInteger();

    CARAPI_(AutoPtr<ICharSequence>) TransformFromDateTime();

    CARAPI_(Boolean) AppendT(
        /* [in] */ StringBuilder* result,
        /* [in] */ Char32 conversion,
        /* [in] */ ICalendar* calendar);

    CARAPI_(Int32) To12Hour(
        /* [in] */ Int32 hour);

    CARAPI_(void) AppendLocalized(
        /* [in] */ StringBuilder* result,
        /* [in] */ Int64 value,
        /* [in] */ Int32 width);

    CARAPI_(AutoPtr<ICharSequence>) TransformFromSpecialNumber(
        /* [in] */ Double d);

    CARAPI_(AutoPtr<ICharSequence>) TransformFromFloat();

    CARAPI_(Boolean) StartsWithMinusSign(
        /* [in] */ const String& cs,
        /* [in] */ const String& minusSign);

    CARAPI_(void) TransformE(
        /* [in] */ StringBuilder* result);

    CARAPI_(void) TransformG(
        /* [in] */ StringBuilder* result);

    CARAPI_(void) TransformF(
        /* [in] */ StringBuilder* result);

    CARAPI_(void) TransformA(
        /* [in] */ StringBuilder* result);

public:
    static pthread_key_t sCachedDecimalFormat;
    static Boolean sHaveKey;

private:
    static const AutoPtr<ArrayOf<Char32> > ZEROS;

    // User-settable parameters.
    AutoPtr<IAppendable> mOut;
    AutoPtr<ILocale> mLocale;

    // Implementation details.
    AutoPtr<IInterface> mArg;
    Boolean mClosed; // = false;
    AutoPtr<FormatToken> mFormatToken;
    ECode mLastIOException;
    AutoPtr<ILocaleData> mLocaleData;
};

} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CFORMATTER_H__
