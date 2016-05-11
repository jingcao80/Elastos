
#ifndef __ELASTOS_IO_STREAMTOKENIZER_H__
#define __ELASTOS_IO_STREAMTOKENIZER_H__

#include "Elastos.CoreLibrary.IO.h"
#include "Object.h"

using Elastos::Core::Object;

namespace Elastos {
namespace IO {

class StreamTokenizer
    : public Object
    , public IStreamTokenizer
{
public:
    CAR_INTERFACE_DECL()

    StreamTokenizer();

    /**
     * Constructs a new {@code StreamTokenizer} with {@code is} as source input
     * stream. This constructor is deprecated; instead, the constructor that
     * takes a {@code Reader} as an argument should be used.
     *
     * @param is
     *            the source stream from which to parse tokens.
     * @throws NullPointerException
     *             if {@code is} is {@code null}.
     * @deprecated Use {@link #StreamTokenizer(Reader)} instead.
     */
    // @Deprecated
    CARAPI constructor(
        /* [in] */ IInputStream* is);

    /**
     * Constructs a new {@code StreamTokenizer} with {@code r} as source reader.
     * The tokenizer's initial state is as follows:
     * <ul>
     * <li>All byte values 'A' through 'Z', 'a' through 'z', and '&#92;u00A0'
     * through '&#92;u00FF' are considered to be alphabetic.</li>
     * <li>All byte values '&#92;u0000' through '&#92;u0020' are considered to
     * be white space. '/' is a comment character.</li>
     * <li>Single quote '\'' and double quote '"' are string quote characters.
     * </li>
     * <li>Numbers are parsed.</li>
     * <li>End of lines are considered to be white space rather than separate
     * tokens.</li>
     * <li>C-style and C++-style comments are not recognized.</LI>
     * </ul>
     *
     * @param r
     *            the source reader from which to parse tokens.
     */
    CARAPI constructor(
        /* [in] */ IReader* r);

    /**
     * Specifies that the character {@code ch} shall be treated as a comment
     * character.
     *
     * @param ch
     *            the character to be considered a comment character.
     */
    CARAPI SetCommentChar(
        /* [in] */ Int32 ch);

    /**
     * Specifies whether the end of a line is significant and should be returned
     * as {@code TT_EOF} in {@code ttype} by this tokenizer.
     *
     * @param flag
     *            {@code true} if EOL is significant, {@code false} otherwise.
     */
    CARAPI SetEolIsSignificant(
        /* [in] */ Boolean flag);

    /**
     * Returns the current line number.
     *
     * @return this tokenizer's current line number.
     */
    CARAPI GetLineno(
        /* [out] */ Int32* rev);

    /**
     * Specifies whether word tokens should be converted to lower case when they
     * are stored in {@code sval}.
     *
     * @param flag
     *            {@code true} if {@code sval} should be converted to lower
     *            case, {@code false} otherwise.
     */
    CARAPI SetLowerCaseMode(
        /* [in] */ Boolean flag);

    /**
     * Parses the next token from this tokenizer's source stream or reader. The
     * type of the token is stored in the {@code ttype} field, additional
     * information may be stored in the {@code nval} or {@code sval} fields.
     *
     * @return the value of {@code ttype}.
     * @throws IOException
     *             if an I/O error occurs while parsing the next token.
     */
    CARAPI GetNextToken(
        /* [out] */ Int32* rev);

    /**
     * Specifies that the character {@code ch} shall be treated as an ordinary
     * character by this tokenizer. That is, it has no special meaning as a
     * comment character, word component, white space, string delimiter or
     * number.
     *
     * @param ch
     *            the character to be considered an ordinary character.
     */
    CARAPI SetOrdinaryChar(
        /* [in] */ Int32 ch);

    /**
     * Specifies that the characters in the range from {@code low} to {@code hi}
     * shall be treated as an ordinary character by this tokenizer. That is,
     * they have no special meaning as a comment character, word component,
     * white space, string delimiter or number.
     *
     * @param low
     *            the first character in the range of ordinary characters.
     * @param hi
     *            the last character in the range of ordinary characters.
     */
    CARAPI SetOrdinaryChars(
        /* [in] */ Int32 low,
        /* [in] */ Int32 hi);

    /**
     * Specifies that this tokenizer shall parse numbers.
     */
    CARAPI ParseNumbers();

    /**
     * Indicates that the current token should be pushed back and returned again
     * the next time {@code nextToken()} is called.
     */
    CARAPI PushBack();

    /**
     * Specifies that the character {@code ch} shall be treated as a quote
     * character.
     *
     * @param ch
     *            the character to be considered a quote character.
     */
    CARAPI SetQuoteChar(
        /* [in] */ Int32 ch);

    /**
     * Specifies that all characters shall be treated as ordinary characters.
     */
    CARAPI ResetSyntax();

    /**
     * Specifies whether "slash-slash" (C++-style) comments shall be recognized.
     * This kind of comment ends at the end of the line.
     *
     * @param flag
     *            {@code true} if {@code //} should be recognized as the start
     *            of a comment, {@code false} otherwise.
     */
    CARAPI SetSlashSlashComments(
        /* [in] */ Boolean flag);

    /**
     * Specifies whether "slash-star" (C-style) comments shall be recognized.
     * Slash-star comments cannot be nested and end when a star-slash
     * combination is found.
     *
     * @param flag
     *            {@code true} if {@code } should be recognized as the start
     *            of a comment, {@code false} otherwise.
     */
    CARAPI SetSlashStarComments(
        /* [in] */ Boolean flag);

    /**
     * Returns the state of this tokenizer in a readable format.
     *
     * @return the current state of this tokenizer.
     */
    // @Override
    CARAPI ToString(
        /* [in] */ String* rev);

    /**
     * Specifies that the characters in the range from {@code low} to {@code hi}
     * shall be treated as whitespace characters by this tokenizer.
     *
     * @param low
     *            the first character in the range of whitespace characters.
     * @param hi
     *            the last character in the range of whitespace characters.
     */
    CARAPI SetWhitespaceChars(
        /* [in] */ Int32 low,
        /* [in] */ Int32 hi);

    /**
     * Specifies that the characters in the range from {@code low} to {@code hi}
     * shall be treated as word characters by this tokenizer. A word consists of
     * a word character followed by zero or more word or number characters.
     *
     * @param low
     *            the first character in the range of word characters.
     * @param hi
     *            the last character in the range of word characters.
     */
    CARAPI SetWordChars(
        /* [in] */ Int32 low,
        /* [in] */ Int32 hi);

    CARAPI GetNval(
        /* [out] */ Double* nval);

    CARAPI SetNval(
        /* [in] */ Double nval);

    CARAPI GetSval(
        /* [out] */ String* sval);

    CARAPI SetSval(
        /* [in] */ const String& sval);

    CARAPI GetTtype(
        /* [out] */ Int32* ttype);

    CARAPI SetTtype(
        /* [in] */ Int32 ttype);

private:
    /**
     * Private constructor to initialize the default values according to the
     * specification.
     */
    CARAPI constructor();

    CARAPI Read(
        /* [out] */ Int32* rev);

public:
    /**
     * Contains a number if the current token is a number ({@code ttype} ==
     * {@code TT_NUMBER}).
     */
    Double mNval;

    /**
     * Contains a string if the current token is a word ({@code ttype} ==
     * {@code TT_WORD}).
     */
    String mSval;

    /**
     * After calling {@code nextToken()}, {@code ttype} contains the type of
     * token that has been read. When a single character is read, its value
     * converted to an integer is stored in {@code ttype}. For a quoted string,
     * the value is the quoted character. Otherwise, its value is one of the
     * following:
     * <ul>
     * <li> {@code TT_WORD} - the token is a word.</li>
     * <li> {@code TT_NUMBER} - the token is a number.</li>
     * <li> {@code TT_EOL} - the end of line has been reached. Depends on
     * whether {@code eolIsSignificant} is {@code true}.</li>
     * <li> {@code TT_EOF} - the end of the stream has been reached.</li>
     * </ul>
     */
    Int32 mTtype;

private:
    /**
     * Internal character meanings, 0 implies TOKEN_ORDINARY
     */
    AutoPtr<ArrayOf<Byte> > mTokenTypes;

    static const Byte TOKEN_COMMENT;

    static const Byte TOKEN_QUOTE;

    static const Byte TOKEN_WHITE;

    static const Byte TOKEN_WORD;

    /**
     * Internal representation of unknown state.
     */
    static const Int32 TT_UNKNOWN;

    static const Byte TOKEN_DIGIT;

    Int32 mLineNumber;

    Boolean mForceLowercase;

    Boolean mIsEOLSignificant;

    Boolean mSlashStarComments;

    Boolean mSlashSlashComments;

    Boolean mPushBackToken;

    Boolean mLastCr;

    /* One of these will have the stream */
    AutoPtr<IInputStream> mInStream;

    AutoPtr<IReader> mInReader;

    Int32 mPeekChar;
};



} // namespace IO
} // namespace Elastos
#endif // __ELASTOS_IO_STREAMTOKENIZER_H__
