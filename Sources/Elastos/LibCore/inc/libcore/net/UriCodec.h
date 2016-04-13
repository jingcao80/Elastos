#ifndef __LIBCORE_NET_URICODEC_H__
#define __LIBCORE_NET_URICODEC_H__

#include "elastos/core/Object.h"
#include "elastos/core/StringBuilder.h"

using Elastos::Core::StringBuilder;
using Elastos::Core::IStringBuilder;
using Elastos::IO::Charset::ICharset;

namespace Libcore {
namespace Net {

/**
 * Encodes and decodes {@code application/x-www-form-urlencoded} content.
 * Subclasses define exactly which characters are legal.
 *
 * <p>By default, UTF-8 is used to encode escaped characters. A single input
 * character like "\u0080" may be encoded to multiple octets like %C2%80.
 */
class UriCodec
    : public Object
{
public :
    /**
     * Throws if {@code s} is invalid according to this encoder.
     */
    CARAPI Validate(
        /* [in] */ const String& uri,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ const String& name,
        /* [out] */ String* result);

    /**
     * Throws if {@code s} contains characters that are not letters, digits or
     * in {@code legal}.
     */
    static CARAPI ValidateSimple(
        /* [in] */ const String& s,
        /* [in] */ const String& legal);

    CARAPI Encode(
        /* [in] */ const String& s,
        /* [in] */ ICharset* charset,
        /* [out] */ String* result);

    CARAPI AppendEncoded(
        /* [in] */ IStringBuilder* builder,
        /* [in] */ const String& s);

    CARAPI AppendPartiallyEncoded(
        /* [in] */ IStringBuilder* builder,
        /* [in] */ const String& s);

    CARAPI AppendEncoded(
        /* [in] */ StringBuilder& builder,
        /* [in] */ const String& s);

    CARAPI AppendPartiallyEncoded(
        /* [in] */ StringBuilder& builder,
        /* [in] */ const String& s);

    static CARAPI Decode(
        /* [in] */ const String& s,
        /* [out] */ String* result);

    /**
     * @param convertPlus true to convert '+' to ' '.
     * @param throwOnFailure true to throw an IllegalArgumentException on
     *     invalid escape sequences; false to replace them with the replacement
     *     character (U+fffd).
     */
    static CARAPI Decode(
        /* [in] */ const String& s,
        /* [in] */ Boolean convertPlus,
        /* [in] */ ICharset* charset,
        /* [in] */ Boolean throwOnFailure,
        /* [out] */ String* result);

private:
    /**
     * Encodes {@code s} and appends the result to {@code builder}.
     *
     * @param isPartiallyEncoded true to fix input that has already been
     *     partially or fully encoded. For example, input of "hello%20world" is
     *     unchanged with isPartiallyEncoded=true but would be double-escaped to
     *     "hello%2520world" otherwise.
     */
    CARAPI AppendEncoded(
        /* [in] */ StringBuilder& builder,
        /* [in] */ const String& s,
        /* [in] */ ICharset* charset,
        /* [in] */ Boolean isPartiallyEncoded);

    CARAPI AppendEncoded(
        /* [in] */ IStringBuilder * builder,
        /* [in] */ const String& s,
        /* [in] */ ICharset* charset,
        /* [in] */ Boolean isPartiallyEncoded);

    static CARAPI_(AutoPtr<ArrayOf<Byte> >) GetBytes(
        /* [in] */ const char* cPtr,
        /* [in] */ ICharset* charSet);

    /**
     * Like {@link Character#digit}, but without support for non-ASCII
     * characters.
     */
    static CARAPI_(Int32) HexToInt(
        /* [in] */ char c);

    static CARAPI AppendHex(
        /* [in] */ StringBuilder& builder,
        /* [in] */ Byte b);

    static CARAPI AppendHex(
        /* [in] */ StringBuilder& builder,
        /* [in] */ const String& s,
        /* [in] */ ICharset* charset);

    static CARAPI AppendHex(
        /* [in] */ IStringBuilder * builder,
        /* [in] */ Byte b);

    static CARAPI AppendHex(
        /* [in] */ IStringBuilder * builder,
        /* [in] */ const String& s,
        /* [in] */ ICharset* charset);
protected:
    /**
     * Returns true if {@code c} does not need to be escaped.
     */
    virtual Boolean IsRetained (
        /* [in] */ Char32 c) = 0;
};

} // namespace Net
} // namespace Libcore

#endif // __LIBCORE_NET_URICODEC_H__
