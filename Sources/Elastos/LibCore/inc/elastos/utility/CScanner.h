
#ifndef __ELASTOS_UTILITY_CSCANNER_H__
#define __ELASTOS_UTILITY_CSCANNER_H__

#include "Elastos.CoreLibrary.Text.h"
#include "_Elastos_Utility_CScanner.h"
#include "elastos/core/StringBuilder.h"
#include "elastos/core/Object.h"

using Elastos::Core::StringBuilder;
using Elastos::IO::ICloseable;
using Elastos::IO::IFile;
using Elastos::IO::IInputStream;
using Elastos::IO::IReadable;
using Elastos::IO::ICharBuffer;
using Elastos::IO::Channels::IReadableByteChannel;
using Elastos::Utility::ILocale;
using Elastos::Utility::Regex::IPattern;
using Elastos::Utility::Regex::IMatcher;
using Elastos::Utility::Regex::IMatchResult;
using Elastos::Math::IBigDecimal;
using Elastos::Math::IBigInteger;
using Elastos::Text::IDecimalFormat;

namespace Elastos {
namespace Utility {

CarClass(CScanner)
    , public Object
    , public IScanner
    , public IIterator
    , public ICloseable
{
private:
    enum DataType {
        /*
         * Stands for Integer
         */
        INT,
        /*
         * Stands for Float
         */
        FLOAT
    };
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CScanner();

    /**
     * Creates a {@code Scanner} with the specified {@code File} as input. The default charset
     * is applied when reading the file.
     *
     * @param src
     *            the file to be scanned.
     * @throws FileNotFoundException
     *             if the specified file does not exist.
     */
    CARAPI constructor(
        /* [in] */ IFile* filesrc);

    /**
     * Creates a {@code Scanner} with the specified {@code File} as input. The specified charset
     * is applied when reading the file.
     *
     * @param src
     *            the file to be scanned.
     * @param charsetName
     *            the name of the encoding type of the file.
     * @throws FileNotFoundException
     *             if the specified file does not exist.
     * @throws IllegalArgumentException
     *             if the specified coding does not exist.
     */
    CARAPI constructor(
        /* [in] */ IFile* filesrc,
        /* [in] */ const String& charsetName);

    /**
     * Creates a {@code Scanner} on the specified string.
     *
     * @param src
     *            the string to be scanned.
     */
    CARAPI constructor(
        /* [in] */ const String& strsrc);

    /**
     * Creates a {@code Scanner} on the specified {@code InputStream}. The default charset is
     * applied when decoding the input.
     *
     * @param src
     *            the {@code InputStream} to be scanned.
     */
    CARAPI constructor(
        /* [in] */ IInputStream* inputsrc);

    /**
     * Creates a {@code Scanner} on the specified {@code InputStream}. The specified charset is
     * applied when decoding the input.
     *
     * @param src
     *            the {@code InputStream} to be scanned.
     * @param charsetName
     *            the encoding type of the {@code InputStream}.
     * @throws IllegalArgumentException
     *             if the specified character set is not found.
     */
    CARAPI constructor(
        /* [in] */ IInputStream* inputsrc,
        /* [in] */ const String& charsetName);

    /**
     * Creates a {@code Scanner} with the specified {@code Readable} as input.
     *
     * @param src
     *            the {@code Readable} to be scanned.
     */
    CARAPI constructor(
        /* [in] */ IReadable* readsrc);

    /**
     * Creates a {@code Scanner} with the specified {@code ReadableByteChannel} as
     * input. The default charset is applied when decoding the input.
     *
     * @param src
     *            the {@code ReadableByteChannel} to be scanned.
     */
    CARAPI constructor(
        /* [in] */ IReadableByteChannel* rbcsrc);

    /**
     * Creates a {@code Scanner} with the specified {@code ReadableByteChannel} as
     * input. The specified charset is applied when decoding the input.
     *
     * @param src
     *            the {@code ReadableByteChannel} to be scanned.
     * @param charsetName
     *            the encoding type of the content.
     * @throws IllegalArgumentException
     *             if the specified character set is not found.
     */
    CARAPI constructor(
        /* [in] */ IReadableByteChannel* rbcsrc,
        /* [in] */ const String& charsetName);

    /**
     * Returns true if there is at least one more element, false otherwise.
     * @see #next
     */
    CARAPI HasNext(
        /* [out] */ Boolean* result);

    /**
     * Returns the next object and advances the iterator.
     *
     * @return the next object.
     * @throws NoSuchElementException
     *             if there are no more elements.
     * @see #hasNext
     */
    CARAPI GetNext(
        /* [out] */ IInterface** object);

    /**
     * Removes the last object returned by {@code next} from the collection.
     * This method can only be called once between each call to {@code next}.
     *
     * @throws UnsupportedOperationException
     *             if removing is not supported by the collection being
     *             iterated.
     * @throws IllegalStateException
     *             if {@code next} has not been called, or {@code remove} has
     *             already been called after the last call to {@code next}.
     */
    CARAPI Remove();

    /**
     * Closes this {@code Scanner} and the underlying input if the input implements
     * {@code Closeable}. If the {@code Scanner} has been closed, this method will have
     * no effect. Any scanning operation called after calling this method will throw
     * an {@code IllegalStateException}.
     *
     * @see Closeable
     */
    CARAPI Close();

    /**
     * Returns the delimiter {@code Pattern} in use by this {@code Scanner}.
     *
     * @return the delimiter {@code Pattern} in use by this {@code Scanner}.
     */
    CARAPI Delimiter(
        /* [out] */ IPattern** outpat);

    /**
     * Tries to find the pattern in the input. Delimiters are ignored. If the
     * pattern is found before line terminator, the matched string will be
     * returned, and the {@code Scanner} will advance to the end of the matched string.
     * Otherwise, {@code null} will be returned and the {@code Scanner} will not advance.
     * When waiting for input, the {@code Scanner} may be blocked. All the
     * input may be cached if no line terminator exists in the buffer.
     *
     * @param pattern
     *            the pattern to find in the input.
     * @return the matched string or {@code null} if the pattern is not found
     *         before the next line terminator.
     * @throws IllegalStateException
     *             if the {@code Scanner} is closed.
     */
    CARAPI FindInLine(
        /* [in] */ IPattern* pattern,
        /* [out] */ String* str);

    /**
     * Compiles the pattern string and tries to find a substing matching it in the input data. The
     * delimiter will be ignored. This is the same as invoking
     * {@code findInLine(Pattern.compile(pattern))}.
     *
     * @param pattern
     *            a string used to construct a pattern which is in turn used to
     *            match a substring of the input data.
     * @return the matched string or {@code null} if the pattern is not found
     *         before the next line terminator.
     * @throws IllegalStateException
     *             if the {@code Scanner} is closed.
     * @see #findInLine(Pattern)
     */
    CARAPI FindInLine(
        /* [in] */ const String& pattern,
        /* [out] */ String* str);

    /**
     * Tries to find the pattern in the input between the current position and the specified
     * horizon. Delimiters are ignored. If the pattern is found, the matched
     * string will be returned, and the {@code Scanner} will advance to the end of the
     * matched string. Otherwise, null will be returned and {@code Scanner} will not
     * advance. When waiting for input, the {@code Scanner} may be blocked.
     * <p>
     * The {@code Scanner}'s search will never go more than {@code horizon} code points from current
     * position. The position of {@code horizon} does have an effect on the result of the
     * match. For example, when the input is "123" and current position is at zero,
     * <code>findWithinHorizon(Pattern.compile("\\p{Digit}{3}"), 2)</code>
     * will return {@code null}, while
     * <code>findWithinHorizon(Pattern.compile("\\p{Digit}{3}"), 3)</code>
     * will return {@code "123"}. {@code horizon} is treated as a transparent,
     * non-anchoring bound. (refer to
     * {@link Matcher#useTransparentBounds(boolean)} and
     * {@link Matcher#useAnchoringBounds(boolean)})
     * <p>
     * A {@code horizon} whose value is zero will be ignored and the whole input will be
     * used for search. In this situation, all the input may be cached.
     *
     * @param pattern
     *            the pattern used to scan.
     * @param horizon
     *            the search limit.
     * @return the matched string or {@code null} if the pattern is not found
     *         within the specified {@code horizon}.
     * @throws IllegalStateException
     *             if the {@code Scanner} is closed.
     * @throws IllegalArgumentException
     *             if {@code horizon} is less than zero.
     */
    CARAPI FindWithinHorizon(
        /* [in] */ IPattern* pattern,
        /* [in] */ Int32 horizon,
        /* [out] */ String* str);

    /**
     * Tries to find the pattern in the input between the current position and the specified
     * {@code horizon}. Delimiters are ignored. This call is the same as invoking
     * {@code findWithinHorizon(Pattern.compile(pattern))}.
     *
     * @param pattern
     *            the pattern used to scan.
     * @param horizon
     *            the search limit.
     * @return the matched string, or {@code null} if the pattern is not found
     *         within the specified horizon.
     * @throws IllegalStateException
     *             if the {@code Scanner} is closed.
     * @throws IllegalArgumentException
     *             if {@code horizon} is less than zero.
     * @see #findWithinHorizon(Pattern, int)
     */
    CARAPI FindWithinHorizon(
        /* [in] */ const String& pattern,
        /* [in] */ Int32 horizon,
        /* [out] */ String* str);

    /**
     * Returns whether this {@code Scanner} has one or more tokens remaining to parse
     * and the next token matches the given pattern. This method will block if the data is
     * still being read.
     *
     * @param pattern
     *            the pattern to check for.
     * @return {@code true} if this {@code Scanner} has more tokens and the next token
     *         matches the pattern, {@code false} otherwise.
     * @throws IllegalStateException
     *             if the {@code Scanner} has been closed.
     */
    CARAPI HasNext(
        /* [in] */ IPattern* pattern,
        /* [out] */ Boolean* value);

    /**
     * Returns {@code true} if this {@code Scanner} has one or more tokens remaining to parse
     * and the next token matches a pattern compiled from the given string. This method will
     * block if the data is still being read. This call is equivalent to
     * {@code hasNext(Pattern.compile(pattern))}.
     *
     * @param pattern
     *            the string specifying the pattern to scan for
     * @return {@code true} if the specified pattern matches this {@code Scanner}'s
     *         next token, {@code false} otherwise.
     * @throws IllegalStateException
     *             if the {@code Scanner} has been closed.
     */
    CARAPI HasNext(
        /* [in] */ const String& pattern,
        /* [out] */ Boolean* value);

    /**
     * Returns whether the next token can be translated into a valid
     * {@code BigDecimal}.
     *
     * @return {@code true} if the next token can be translated into a valid
     *         {@code BigDecimal}, otherwise {@code false.}
     * @throws IllegalStateException
     *             if the {@code Scanner} has been closed.
     */
    CARAPI HasNextBigDecimal(
        /* [out] */ Boolean* value);

    /**
     * Returns whether the next token can be translated into a valid
     * {@code BigInteger} in the default radix.
     *
     * @return {@code true} if the next token can be translated into a valid
     *         {@code BigInteger}, otherwise {@code false}.
     * @throws IllegalStateException
     *             if the {@code Scanner} has been closed.
     */
    CARAPI HasNextBigInteger(
        /* [out] */ Boolean* value);

    /**
     * Returns whether the next token can be translated into a valid
     * {@code BigInteger} in the specified radix.
     *
     * @param radix
     *            the radix used to translate the token into a
     *            {@code BigInteger}.
     * @return {@code true} if the next token can be translated into a valid
     *         {@code BigInteger}, otherwise {@code false}.
     * @throws IllegalStateException
     *             if the {@code Scanner} has been closed.
     */
    CARAPI HasNextBigInteger(
        /* [in] */ Int32 radix,
        /* [out] */ Boolean* value);

    /**
     * Returns whether the next token can be translated into a valid
     * {@code boolean} value.
     *
     * @return {@code true} if the next token can be translated into a valid
     *         {@code boolean} value, otherwise {@code false}.
     * @throws IllegalStateException
     *             if the {@code Scanner} has been closed.
     */
    CARAPI HasNextBoolean(
        /* [out] */ Boolean* value);

    /**
     * Returns whether the next token can be translated into a valid
     * {@code byte} value in the default radix.
     *
     * @return {@code true} if the next token can be translated into a valid
     *         {@code byte} value, otherwise {@code false}.
     * @throws IllegalStateException
     *             if the {@code Scanner} has been closed.
     */
    CARAPI HasNextByte(
        /* [out] */ Boolean* value);

    /**
     * Returns whether the next token can be translated into a valid
     * {@code byte} value in the specified radix.
     *
     * @param radix
     *            the radix used to translate the token into a {@code byte}
     *            value
     * @return {@code true} if the next token can be translated into a valid
     *         {@code byte} value, otherwise {@code false}.
     * @throws IllegalStateException
     *             if the {@code Scanner} has been closed.
     */
    CARAPI HasNextByte(
        /* [in] */ Int32 radix,
        /* [out] */ Boolean* value);

    /**
     * Returns whether the next token translated into a valid {@code double}
     * value.
     *
     * @return {@code true} if the next token can be translated into a valid
     *         {@code double} value, otherwise {@code false}.
     * @throws IllegalStateException
     *             if the {@code Scanner} has been closed.
     */
    CARAPI HasNextDouble(
        /* [out] */ Boolean* value);

    /**
     * Returns whether the next token can be translated into a valid
     * {@code float} value.
     *
     * @return {@code true} if the next token can be translated into a valid
     *         {@code float} value, otherwise {@code false}.
     * @throws IllegalStateException
     *             if the {@code Scanner} has been closed.
     */
    CARAPI HasNextFloat(
        /* [out] */ Boolean* value);

    /**
     * Returns whether the next token can be translated into a valid {@code int}
     * value in the default radix.
     *
     * @return {@code true} if the next token can be translated into a valid
     *         {@code int} value, otherwise {@code false}.
     * @throws IllegalStateException
     *             if the {@code Scanner} has been closed,
     */
    CARAPI HasNextInt32(
        /* [out] */ Boolean* value);

    /**
     * Returns whether the next token can be translated into a valid {@code int}
     * value in the specified radix.
     *
     * @param radix
     *            the radix used to translate the token into an {@code int}
     *            value.
     * @return {@code true} if the next token in this {@code Scanner}'s input can be
     *         translated into a valid {@code int} value, otherwise
     *         {@code false}.
     * @throws IllegalStateException
     *             if the {@code Scanner} has been closed.
     */
    CARAPI HasNextInt32(
        /* [in] */ Int32 radix,
        /* [out] */ Boolean* value);

    /**
     * Returns whether there is a line terminator in the input.
     * This method may block.
     *
     * @return {@code true} if there is a line terminator in the input,
     *         otherwise, {@code false}.
     * @throws IllegalStateException
     *             if the {@code Scanner} is closed.
     */
    CARAPI HasNextLine(
        /* [out] */ Boolean* value);

    /**
     * Returns whether the next token can be translated into a valid
     * {@code long} value in the default radix.
     *
     * @return {@code true} if the next token can be translated into a valid
     *         {@code long} value, otherwise {@code false}.
     * @throws IllegalStateException
     *             if the {@code Scanner} has been closed.
     */
    CARAPI HasNextInt64(
        /* [out] */ Boolean* value);

    /**
     * Returns whether the next token can be translated into a valid
     * {@code long} value in the specified radix.
     *
     * @param radix
     *            the radix used to translate the token into a {@code long}
     *            value.
     * @return {@code true} if the next token can be translated into a valid
     *         {@code long} value, otherwise {@code false}.
     * @throws IllegalStateException
     *             if the {@code Scanner} has been closed.
     */
    CARAPI HasNextInt64(
        /* [in] */ Int32 radix,
        /* [out] */ Boolean* value);

    /**
     * Returns whether the next token can be translated into a valid
     * {@code short} value in the default radix.
     *
     * @return {@code true} if the next token can be translated into a valid
     *         {@code short} value, otherwise {@code false}.
     * @throws IllegalStateException
     *             if the {@code Scanner} has been closed.
     */
    CARAPI HasNextInt16(
        /* [out] */ Boolean* value);

    /**
     * Returns whether the next token can be translated into a valid
     * {@code short} value in the specified radix.
     *
     * @param radix
     *            the radix used to translate the token into a {@code short}
     *            value.
     * @return {@code true} if the next token can be translated into a valid
     *         {@code short} value, otherwise {@code false}.
     * @throws IllegalStateException
     *             if the {@code Scanner} has been closed.
     */
    CARAPI HasNextInt16(
        /* [in] */ Int32 radix,
        /* [out] */ Boolean* value);

    /**
     * Returns the last {@code IOException} that was raised while reading from the underlying
     * input.
     *
     * @return the last thrown {@code IOException}, or {@code null} if none was thrown.
     */
    CARAPI IoException();

    /**
     * Return the {@code Locale} of this {@code Scanner}.
     *
     * @return the {@code Locale} of this {@code Scanner}.
     */
    CARAPI GetLocale(
        /* [out] */ ILocale** locale);

    /**
     * Returns the result of the last matching operation.
     * <p>
     * The next* and find* methods return the match result in the case of a
     * successful match.
     *
     * @return the match result of the last successful match operation
     * @throws IllegalStateException
     *             if the match result is not available, of if the last match
     *             was not successful.
     */
    CARAPI GetMatch(
        /* [out] */ IMatchResult** outmatch);

    /**
     * Returns the next token. The token will be both prefixed and suffixed by
     * the delimiter that is currently being used (or a string that matches the
     * delimiter pattern). This method will block if input is being read.
     *
     * @return the next complete token.
     * @throws IllegalStateException
     *             if this {@code Scanner} has been closed.
     * @throws NoSuchElementException
     *             if input has been exhausted.
     */
    CARAPI Next(
        /* [out] */ String* str);

    /**
     * Returns the next token if it matches the specified pattern. The token
     * will be both prefixed and postfixed by the delimiter that is currently
     * being used (or a string that matches the delimiter pattern). This method will block
     * if input is being read.
     *
     * @param pattern
     *            the specified pattern to scan.
     * @return the next token.
     * @throws IllegalStateException
     *             if this {@code Scanner} has been closed.
     * @throws NoSuchElementException
     *             if input has been exhausted.
     * @throws InputMismatchException
     *             if the next token does not match the pattern given.
     */
    CARAPI Next(
        /* [in] */ IPattern* pattern,
        /* [out] */ String* str);

    /**
     * Returns the next token if it matches the specified pattern. The token
     * will be both prefixed and postfixed by the delimiter that is currently
     * being used (or a string that matches the delimiter pattern). This method will block
     * if input is being read. Calling this method is equivalent to
     * {@code next(Pattern.compile(pattern))}.
     *
     * @param pattern
     *            the string specifying the pattern to scan for.
     * @return the next token.
     * @throws IllegalStateException
     *             if this {@code Scanner} has been closed.
     * @throws NoSuchElementException
     *             if input has been exhausted.
     * @throws InputMismatchException
     *             if the next token does not match the pattern given.
     */
    CARAPI Next(
        /* [in] */ const String& pattern,
        /* [out] */ String* str);

    /**
     * Returns the next token as a {@code BigDecimal}. This method will block if input is
     * being read. If the next token can be translated into a {@code BigDecimal}
     * the following is done: All {@code Locale}-specific prefixes, group separators,
     * and {@code Locale}-specific suffixes are removed. Then non-ASCII digits are
     * mapped into ASCII digits via {@link Character#digit(char, int)}, and a
     * negative sign (-) is added if the {@code Locale}-specific negative prefix or
     * suffix was present. Finally the resulting string is passed to
     * {@code BigDecimal(String) }.
     *
     * @return the next token as a {@code BigDecimal}.
     * @throws IllegalStateException
     *             if this {@code Scanner} has been closed.
     * @throws NoSuchElementException
     *             if input has been exhausted.
     * @throws InputMismatchException
     *             if the next token can not be translated into a valid
     *             {@code BigDecimal}.
     */
    CARAPI NextBigDecimal(
        /* [out] */ IBigDecimal** outbig);

    /**
     * Returns the next token as a {@code BigInteger}. This method will block if input is
     * being read. Equivalent to {@code nextBigInteger(DEFAULT_RADIX)}.
     *
     * @return the next token as {@code BigInteger}.
     * @throws IllegalStateException
     *             if this {@code Scanner} has been closed.
     * @throws NoSuchElementException
     *             if input has been exhausted.
     * @throws InputMismatchException
     *             if the next token can not be translated into a valid
     *             {@code BigInteger}.
     */
    CARAPI NextBigInteger(
        /* [out] */ IBigInteger** outbig);

    /**
     * Returns the next token as a {@code BigInteger} with the specified radix.
     * This method will block if input is being read. If the next token can be translated
     * into a {@code BigInteger} the following is done: All {@code Locale}-specific
     * prefixes, group separators, and {@code Locale}-specific suffixes are removed.
     * Then non-ASCII digits are mapped into ASCII digits via
     * {@link Character#digit(char, int)}, and a negative sign (-) is added if the
     * {@code Locale}-specific negative prefix or suffix was present. Finally the
     * resulting String is passed to {@link BigInteger#BigInteger(String, int)}}
     * with the specified radix.
     *
     * @param radix
     *            the radix used to translate the token into a
     *            {@code BigInteger}.
     * @return the next token as a {@code BigInteger}
     * @throws IllegalStateException
     *             if this {@code Scanner} has been closed.
     * @throws NoSuchElementException
     *             if input has been exhausted.
     * @throws InputMismatchException
     *             if the next token can not be translated into a valid
     *             {@code BigInteger}.
     */
    CARAPI NextBigInteger(
        /* [in] */ Int32 radix,
        /* [out] */ IBigInteger** outbig);

    /**
     * Returns the next token as a {@code boolean}. This method will block if input is
     * being read.
     *
     * @return the next token as a {@code boolean}.
     * @throws IllegalStateException
     *             if this {@code Scanner} has been closed.
     * @throws NoSuchElementException
     *             if input has been exhausted.
     * @throws InputMismatchException
     *             if the next token can not be translated into a valid
     *             {@code boolean} value.
     */
    CARAPI NextBoolean(
        /* [out] */ Boolean* value);

    /**
     * Returns the next token as a {@code byte}. This method will block if input is being
     * read. Equivalent to {@code nextByte(DEFAULT_RADIX)}.
     *
     * @return the next token as a {@code byte}.
     * @throws IllegalStateException
     *             if this {@code Scanner} has been closed.
     * @throws NoSuchElementException
     *             if input has been exhausted.
     * @throws InputMismatchException
     *             if the next token can not be translated into a valid
     *             {@code byte} value.
     */
    CARAPI NextByte(
        /* [out] */ Byte* value);

    /**
     * Returns the next token as a {@code byte} with the specified radix. Will
     * block if input is being read. If the next token can be translated into a
     * {@code byte} the following is done: All {@code Locale}-specific prefixes, group
     * separators, and {@code Locale}-specific suffixes are removed. Then non-ASCII
     * digits are mapped into ASCII digits via
     * {@link Character#digit(char, int)}, and a negative sign (-) is added if the
     * {@code Locale}-specific negative prefix or suffix was present. Finally the
     * resulting String is passed to {@link Byte#parseByte(String, int)}} with
     * the specified radix.
     *
     * @param radix
     *            the radix used to translate the token into {@code byte} value.
     * @return the next token as a {@code byte}.
     * @throws IllegalStateException
     *             if this {@code Scanner} has been closed.
     * @throws NoSuchElementException
     *             if input has been exhausted.
     * @throws InputMismatchException
     *             if the next token can not be translated into a valid
     *             {@code byte} value.
     */
    // @SuppressWarnings("boxing")
    CARAPI NextByte(
        /* [in] */ Int32 radix,
        /* [out] */ Byte* value);

    /**
     * Returns the next token as a {@code double}. This method will block if input is being
     * read. If the next token can be translated into a {@code double} the
     * following is done: All {@code Locale}-specific prefixes, group separators, and
     * {@code Locale}-specific suffixes are removed. Then non-ASCII digits are mapped
     * into ASCII digits via {@link Character#digit(char, int)}, and a negative
     * sign (-) is added if the {@code Locale}-specific negative prefix or suffix was
     * present. Finally the resulting String is passed to
     * {@link Double#parseDouble(String)}}. If the token matches the localized
     * NaN or infinity strings, it is also passed to
     * {@link Double#parseDouble(String)}}.
     *
     * @return the next token as a {@code double}.
     * @throws IllegalStateException
     *             if this {@code Scanner} has been closed.
     * @throws NoSuchElementException
     *             if input has been exhausted.
     * @throws InputMismatchException
     *             if the next token can not be translated into a valid
     *             {@code double} value.
     */
    // @SuppressWarnings("boxing")
    CARAPI NextDouble(
        /* [out] */ Double* value);

    /**
     * Returns the next token as a {@code float}. This method will block if input is being
     * read. If the next token can be translated into a {@code float} the
     * following is done: All {@code Locale}-specific prefixes, group separators, and
     * {@code Locale}-specific suffixes are removed. Then non-ASCII digits are mapped
     * into ASCII digits via {@link Character#digit(char, int)}, and a negative
     * sign (-) is added if the {@code Locale}-specific negative prefix or suffix was
     * present. Finally the resulting String is passed to
     * {@link Float#parseFloat(String)}}.If the token matches the localized NaN
     * or infinity strings, it is also passed to
     * {@link Float#parseFloat(String)}}.
     *
     * @return the next token as a {@code float}.
     * @throws IllegalStateException
     *             if this {@code Scanner} has been closed.
     * @throws NoSuchElementException
     *             if input has been exhausted.
     * @throws InputMismatchException
     *             if the next token can not be translated into a valid
     *             {@code float} value.
     */
    // @SuppressWarnings("boxing")
    CARAPI NextFloat(
        /* [out] */ Float* value);

    /**
     * Returns the next token as an {@code int}. This method will block if input is being
     * read. Equivalent to {@code nextInt(DEFAULT_RADIX)}.
     *
     * @return the next token as an {@code int}
     * @throws IllegalStateException
     *             if this {@code Scanner} has been closed.
     * @throws NoSuchElementException
     *             if input has been exhausted.
     * @throws InputMismatchException
     *             if the next token can not be translated into a valid
     *             {@code int} value.
     */
    CARAPI NextInt32(
        /* [out] */ Int32* value);

    /**
     * Returns the next token as an {@code int} with the specified radix. This method will
     * block if input is being read. If the next token can be translated into an
     * {@code int} the following is done: All {@code Locale}-specific prefixes, group
     * separators, and {@code Locale}-specific suffixes are removed. Then non-ASCII
     * digits are mapped into ASCII digits via
     * {@link Character#digit(char, int)}, and a negative sign (-) is added if the
     * {@code Locale}-specific negative prefix or suffix was present. Finally the
     * resulting String is passed to {@link Integer#parseInt(String, int)} with
     * the specified radix.
     *
     * @param radix
     *            the radix used to translate the token into an {@code int}
     *            value.
     * @return the next token as an {@code int}.
     * @throws IllegalStateException
     *             if this {@code Scanner} has been closed.
     * @throws NoSuchElementException
     *             if input has been exhausted.
     * @throws InputMismatchException
     *             if the next token can not be translated into a valid
     *             {@code int} value.
     */
    // @SuppressWarnings("boxing")
    CARAPI NextInt32(
        /* [in] */ Int32 radix,
        /* [out] */ Int32* value);

    /**
     * Returns the skipped input and advances the {@code Scanner} to the beginning of
     * the next line. The returned result will exclude any line terminator. When
     * searching, if no line terminator is found, then a large amount of input
     * will be cached. If no line at all can be found, a {@code NoSuchElementException}
     * will be thrown.
     *
     * @return the skipped line.
     * @throws IllegalStateException
     *             if the {@code Scanner} is closed.
     * @throws NoSuchElementException
     *             if no line can be found, e.g. when input is an empty string.
     */
    CARAPI NextLine(
        /* [out] */ String* str);

    /**
     * Returns the next token as a {@code long}. This method will block if input is being
     * read. Equivalent to {@code nextLong(DEFAULT_RADIX)}.
     *
     * @return the next token as a {@code long}.
     * @throws IllegalStateException
     *             if this {@code Scanner} has been closed.
     * @throws NoSuchElementException
     *             if input has been exhausted.
     * @throws InputMismatchException
     *             if the next token can not be translated into a valid
     *             {@code long} value.
     */
    CARAPI NextInt64(
        /* [out] */ Int64* value);

    /**
     * Returns the next token as a {@code long} with the specified radix. This method will
     * block if input is being read. If the next token can be translated into a
     * {@code long} the following is done: All {@code Locale}-specific prefixes, group
     * separators, and {@code Locale}-specific suffixes are removed. Then non-ASCII
     * digits are mapped into ASCII digits via
     * {@link Character#digit(char, int)}, and a negative sign (-) is added if the
     * {@code Locale}-specific negative prefix or suffix was present. Finally the
     * resulting String is passed to {@link Long#parseLong(String, int)}} with
     * the specified radix.
     *
     * @param radix
     *            the radix used to translate the token into a {@code long}
     *            value.
     * @return the next token as a {@code long}.
     * @throws IllegalStateException
     *             if this {@code Scanner} has been closed.
     * @throws NoSuchElementException
     *             if input has been exhausted.
     * @throws InputMismatchException
     *             if the next token can not be translated into a valid
     *             {@code long} value.
     */
    // @SuppressWarnings("boxing")
    CARAPI NextInt64(
        /* [in] */ Int32 radix,
        /* [out] */ Int64* value);

    /**
     * Returns the next token as a {@code short}. This method will block if input is being
     * read. Equivalent to {@code nextShort(DEFAULT_RADIX)}.
     *
     * @return the next token as a {@code short}.
     * @throws IllegalStateException
     *             if this {@code Scanner} has been closed.
     * @throws NoSuchElementException
     *             if input has been exhausted.
     * @throws InputMismatchException
     *             if the next token can not be translated into a valid
     *             {@code short} value.
     */
    CARAPI NextInt16(
        /* [out] */ Int16* value);

    /**
     * Returns the next token as a {@code short} with the specified radix. This method will
     * block if input is being read. If the next token can be translated into a
     * {@code short} the following is done: All {@code Locale}-specific prefixes, group
     * separators, and {@code Locale}-specific suffixes are removed. Then non-ASCII
     * digits are mapped into ASCII digits via
     * {@link Character#digit(char, int)}, and a negative sign (-) is added if the
     * {@code Locale}-specific negative prefix or suffix was present. Finally the
     * resulting String is passed to {@link Short#parseShort(String, int)}}
     * with the specified radix.
     *
     * @param radix
     *            the radix used to translate the token into {@code short}
     *            value.
     * @return the next token as a {@code short}.
     * @throws IllegalStateException
     *             if this {@code Scanner} has been closed.
     * @throws NoSuchElementException
     *             if input has been exhausted.
     * @throws InputMismatchException
     *             if the next token can not be translated into a valid
     *             {@code short} value.
     */
    // @SuppressWarnings("boxing")
    CARAPI NextInt16(
        /* [in] */ Int32 radix,
        /* [out] */ Int16* value);

    /**
     * Return the radix of this {@code Scanner}.
     *
     * @return the radix of this {@code Scanner}
     */
    CARAPI Radix(
        /* [out] */ Int32* value);

    /**
     * Tries to use specified pattern to match input starting from the current position.
     * The delimiter will be ignored. If a match is found, the matched input will be
     * skipped. If an anchored match of the specified pattern succeeds, the corresponding input
     * will also be skipped. Otherwise, a {@code NoSuchElementException} will be thrown.
     * Patterns that can match a lot of input may cause the {@code Scanner} to read
     * in a large amount of input.
     *
     * @param pattern
     *            used to skip over input.
     * @return the {@code Scanner} itself.
     * @throws IllegalStateException
     *             if the {@code Scanner} is closed.
     * @throws NoSuchElementException
     *             if the specified pattern match fails.
     */
    CARAPI Skip(
        /* [in] */ IPattern* pattern,
        /* [out] */ IScanner** outscan);

    /**
     * Tries to use the specified string to construct a pattern and then uses
     * the constructed pattern to match input starting from the current position. The
     * delimiter will be ignored. This call is the same as invoke
     * {@code skip(Pattern.compile(pattern))}.
     *
     * @param pattern
     *            the string used to construct a pattern which in turn is used to
     *            match input.
     * @return the {@code Scanner} itself.
     * @throws IllegalStateException
     *             if the {@code Scanner} is closed.
     */
    CARAPI Skip(
        /* [in] */ const String& pattern,
        /* [out] */ IScanner** outscan);

    /**
     * Returns a string representation of this {@code Scanner}. The information
     * returned may be helpful for debugging. The format of the string is unspecified.
     *
     * @return a string representation of this {@code Scanner}.
     */
    CARAPI ToString(
        /* [out] */ String* str);

    /**
     * Sets the delimiting pattern of this {@code Scanner}.
     *
     * @param pattern
     *            the delimiting pattern to use.
     * @return this {@code Scanner}.
     */
    CARAPI UseDelimiter(
        /* [in] */ IPattern* pattern,
        /* [out] */ IScanner** outscan);

    /**
     * Sets the delimiting pattern of this {@code Scanner} with a pattern compiled from
     * the supplied string value.
     *
     * @param pattern
     *            a string from which a {@code Pattern} can be compiled.
     * @return this {@code Scanner}.
     */
    CARAPI UseDelimiter(
        /* [in] */ const String& pattern,
        /* [out] */ IScanner** outscan);

    /**
     * Sets the {@code Locale} of this {@code Scanner} to a specified {@code Locale}.
     *
     * @param l
     *            the specified {@code Locale} to use.
     * @return this {@code Scanner}.
     */
    CARAPI UseLocale(
        /* [in] */ ILocale* l,
        /* [out] */ IScanner** outscan);

    /**
     * Sets the radix of this {@code Scanner} to the specified radix.
     *
     * @param radix
     *            the specified radix to use.
     * @return this {@code Scanner}.
     */
    CARAPI UseRadix(
        /* [in] */ Int32 radix,
        /* [out] */ IScanner** outscan);

    /**
     * Resets this scanner's delimiter, locale, and radix.
     *
     * @return this scanner
     * @since 1.6
     */
    CARAPI Reset();

private:
    CARAPI CheckRadix(
        /* [in] */ Int32 radix);

    /*
     * Initialize some components.
     */
    CARAPI Initialization(
        /* [in] */ IReadable* reader);

    /*
     * Check the {@code Scanner}'s state, if it is closed, IllegalStateException will be
     * thrown.
     */
    CARAPI CheckOpen();

    /*
     * Check the inputed pattern. If it is null, then a NullPointerException
     * will be thrown out.
     */
    CARAPI CheckNotNull(
        /* [in] */ IPattern* pattern);

    /*
     * Change the matcher's string after reading input
     */
    CARAPI ResetMatcher();

    /*
     * Save the matcher's last find position
     */
    CARAPI PrepareForScan();

    /*
     * Change the matcher's status to last find position
     */
    CARAPI RecoverPreviousStatus();

    /*
     * Get integer's pattern
     */
    CARAPI GetIntegerPattern(
        /* [in] */ Int32 radix,
        /* [out] */ IPattern** outpat);

    /*
     * Get pattern of float
     */
    CARAPI_(AutoPtr<IPattern>) GetFloatPattern();

    CARAPI_(AutoPtr<StringBuilder>) GetNumeral(
        /* [in] */ StringBuilder* digit,
        /* [in] */ StringBuilder* nonZeroDigit);

    /*
     * Add the locale specific positive prefixes and suffixes to the pattern
     */
    CARAPI_(AutoPtr<StringBuilder>) AddPositiveSign(
        /* [in] */ StringBuilder* unSignNumeral);

    /*
     * Add the locale specific negative prefixes and suffixes to the pattern
     */
    CARAPI_(AutoPtr<StringBuilder>) AddNegativeSign(
        /* [in] */ StringBuilder* unSignNumeral);

    /*
     * Remove locale related information from float String
     */
    CARAPI_(String) RemoveLocaleInfoFromFloat(
        /* [in] */ const String& floatString);

    /*
     * Remove the locale specific prefixes, group separators, and locale
     * specific suffixes from input string
     */
    CARAPI_(String) RemoveLocaleInfo(
        /* [in] */ const String& token,
        /* [in] */ DataType type);

    /*
     * Remove positive and negative sign from the parameter stringBuilder, and
     * return whether the input string is negative
     */
    CARAPI_(Boolean) RemoveLocaleSign(
        /* [in] */ StringBuilder* tokenBuilder);

    /*
     * Find the prefixed delimiter and posefixed delimiter in the input resource
     * and set the start index and end index of Matcher region. If postfixed
     * delimiter does not exist, the end index is set to be end of input.
     */
    CARAPI_(Boolean) SetTokenRegion();

    /*
     * Find prefix delimiter
     */
    CARAPI_(Int32) FindPreDelimiter();

    /*
     * Handle some special cases
     */
    CARAPI_(Boolean) SetHeadTokenRegion(
        /* [in] */ Int32 findIndex);

    /*
     * Find postfix delimiter
     */
    CARAPI_(Int32) FindDelimiterAfter();

    /*
     * Read more data from underlying Readable. If nothing is available or I/O
     * operation fails, global boolean variable inputExhausted will be set to
     * true, otherwise set to false.
     */
    CARAPI ReadMore();

    // Expand the size of internal buffer.
    CARAPI ExpandBuffer();

    void SetLocale(
        /* [in] */ ILocale* locale);

private:
    static Boolean sStaticflag;

    // Default delimiting pattern.
    static const AutoPtr<IPattern> DEFAULT_DELIMITER;

    // The boolean's pattern.
    static const AutoPtr<IPattern> BOOLEAN_PATTERN;

    // Pattern used to recognize line terminator.
    static const AutoPtr<IPattern> LINE_TERMINATOR;

    // Pattern used to recognize multiple line terminators.
    static const AutoPtr<IPattern> MULTI_LINE_TERMINATOR;

    // Pattern used to recognize a line with a line terminator.
    static const AutoPtr<IPattern> LINE_PATTERN;

    // The pattern matches anything.
    static const AutoPtr<IPattern> ANY_PATTERN;

//    static const Int32 DIPLOID = 2;

    // Default radix.
    static const Int32 DEFAULT_RADIX = 10;

    static const Int32 DEFAULT_TRUNK_SIZE = 1024;

    // The input source of scanner.
    AutoPtr<IReadable> mInput;

    AutoPtr<ICharBuffer> mBuffer;

    AutoPtr<IPattern> mDelimiter;

    AutoPtr<IMatcher> mMatcher;

    Int32 mCurrentRadix;

    AutoPtr<ILocale> mLocale;

    // The position where find begins.
    Int32 mFindStartIndex;

    // The last find start position.
    Int32 mPreStartIndex;

    // The length of the buffer.
    Int32 mBufferLength;

    // Record the status of this scanner. True if the scanner
    // is closed.
    Boolean mClosed;

    ECode mLastIOException;

    Boolean mMatchSuccessful;

    AutoPtr<IDecimalFormat> mDecimalFormat;

    // Records whether the underlying readable has more input.
    Boolean mInputExhausted;

    AutoPtr<IInterface> mCachedNextValue;

    Int32 mCachedNextIndex;

    AutoPtr<IPattern> mCachedFloatPattern;

    Int32 mCachedIntegerPatternRadix;// = -1;
    AutoPtr<IPattern> mCachedIntegerPattern;
};

} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CSCANNER_H__
