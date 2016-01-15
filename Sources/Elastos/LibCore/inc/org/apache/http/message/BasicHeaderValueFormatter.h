
#ifndef __ORG_APACHE_HTTP_MESSAGE_BASICHEADERVALUEFORMATTER_H_
#define __ORG_APACHE_HTTP_MESSAGE_BASICHEADERVALUEFORMATTER_H_

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Elastos::Utility::IIterator;
using Org::Apache::Http::IHeaderElement;
using Org::Apache::Http::INameValuePair;
using Org::Apache::Http::Utility::ICharArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

/**
 * Basic implementation of a {@link TokenIterator}.
 * This implementation parses <tt>#token<tt> sequences as
 * defined by RFC 2616, section 2.
 * It extends that definition somewhat beyond US-ASCII.
 *
 * @version $Revision: 602520 $
 */
class BasicHeaderValueFormatter
    : public Object
    , public IBasicHeaderValueFormatter
    , public IHeaderValueFormatter
{
public:
    CAR_INTERFACE_DECL()

    virtual ~BasicHeaderValueFormatter() {}

    /**
     * Formats an array of header elements.
     *
     * @param elems     the header elements to format
     * @param quote     <code>true</code> to always format with quoted values,
     *                  <code>false</code> to use quotes only when necessary
     * @param formatter         the formatter to use, or <code>null</code>
     *                          for the {@link #DEFAULT default}
     *
     * @return  the formatted header elements
     */
    static CARAPI FormatElements(
        /* [in] */ ArrayOf<IHeaderElement*>* elems,
        /* [in] */ Boolean quote,
        /* [in] */ IHeaderValueFormatter* formatter,
        /* [out] */ String* elements);

    /**
     * Formats an array of header elements.
     *
     * @param buffer    the buffer to append to, or
     *                  <code>null</code> to create a new buffer
     * @param elems     the header elements to format
     * @param quote     <code>true</code> to always format with quoted values,
     *                  <code>false</code> to use quotes only when necessary
     *
     * @return  a buffer with the formatted header elements.
     *          If the <code>buffer</code> argument was not <code>null</code>,
     *          that buffer will be used and returned.
     */
    CARAPI FormatElements(
        /* [in] */ ICharArrayBuffer* buffer,
        /* [in] */ ArrayOf<IHeaderElement*>* elems,
        /* [in] */ Boolean quote,
        /* [out] */ ICharArrayBuffer** buf);

    /**
     * Formats a header element.
     *
     * @param elem      the header element to format
     * @param quote     <code>true</code> to always format with quoted values,
     *                  <code>false</code> to use quotes only when necessary
     * @param formatter         the formatter to use, or <code>null</code>
     *                          for the {@link #DEFAULT default}
     *
     * @return  the formatted header element
     */
    static CARAPI FormatHeaderElement(
        /* [in] */ IHeaderElement* elem,
        /* [in] */ Boolean quote,
        /* [in] */ IHeaderValueFormatter* formatter,
        /* [out] */ String* element);

    /**
     * Formats one header element.
     *
     * @param buffer    the buffer to append to, or
     *                  <code>null</code> to create a new buffer
     * @param elem      the header element to format
     * @param quote     <code>true</code> to always format with quoted values,
     *                  <code>false</code> to use quotes only when necessary
     *
     * @return  a buffer with the formatted header element.
     *          If the <code>buffer</code> argument was not <code>null</code>,
     *          that buffer will be used and returned.
     */
    CARAPI FormatHeaderElement(
        /* [in] */ ICharArrayBuffer* buffer,
        /* [in] */ IHeaderElement* elem,
        /* [in] */ Boolean quote,
        /* [out] */ ICharArrayBuffer** buf);

    /**
     * Formats a set of parameters.
     *
     * @param nvps      the parameters to format
     * @param quote     <code>true</code> to always format with quoted values,
     *                  <code>false</code> to use quotes only when necessary
     * @param formatter         the formatter to use, or <code>null</code>
     *                          for the {@link #DEFAULT default}
     *
     * @return  the formatted parameters
     */
    static CARAPI FormatParameters(
        /* [in] */ ArrayOf<INameValuePair*>* nvps,
        /* [in] */ Boolean quote,
        /* [in] */ IHeaderValueFormatter* formatter,
        /* [out] */ String* parameters);

    /**
     * Formats the parameters of a header element.
     * That's a list of name-value pairs, to be separated by semicolons.
     * This method will <i>not</i> generate a leading semicolon.
     *
     * @param buffer    the buffer to append to, or
     *                  <code>null</code> to create a new buffer
     * @param nvps      the parameters (name-value pairs) to format
     * @param quote     <code>true</code> to always format with quoted values,
     *                  <code>false</code> to use quotes only when necessary
     *
     * @return  a buffer with the formatted parameters.
     *          If the <code>buffer</code> argument was not <code>null</code>,
     *          that buffer will be used and returned.
     */
    CARAPI FormatParameters(
        /* [in] */ ICharArrayBuffer* buffer,
        /* [in] */ ArrayOf<INameValuePair*>* nvps,
        /* [in] */ Boolean quote,
        /* [out] */ ICharArrayBuffer** buf);

    /**
     * Formats a name-value pair.
     *
     * @param nvp       the name-value pair to format
     * @param quote     <code>true</code> to always format with a quoted value,
     *                  <code>false</code> to use quotes only when necessary
     * @param formatter         the formatter to use, or <code>null</code>
     *                          for the {@link #DEFAULT default}
     *
     * @return  the formatted name-value pair
     */
    static CARAPI FormatNameValuePair(
        /* [in] */ INameValuePair* nvp,
        /* [in] */ Boolean quote,
        /* [in] */ IHeaderValueFormatter* formatter,
        /* [out] */ String* pair);

    /**
     * Formats one name-value pair, where the value is optional.
     *
     * @param buffer    the buffer to append to, or
     *                  <code>null</code> to create a new buffer
     * @param nvp       the name-value pair to format
     * @param quote     <code>true</code> to always format with a quoted value,
     *                  <code>false</code> to use quotes only when necessary
     *
     * @return  a buffer with the formatted name-value pair.
     *          If the <code>buffer</code> argument was not <code>null</code>,
     *          that buffer will be used and returned.
     */
    CARAPI FormatNameValuePair(
        /* [in] */ ICharArrayBuffer* buffer,
        /* [in] */ INameValuePair* nvp,
        /* [in] */ Boolean quote,
        /* [out] */ ICharArrayBuffer** buf);

protected:
    /**
     * Estimates the length of formatted header elements.
     *
     * @param elems     the header elements to format, or <code>null</code>
     *
     * @return  a length estimate, in number of characters
     */
    CARAPI EstimateElementsLen(
        /* [in] */ ArrayOf<IHeaderElement*>* elems,
        /* [out] */ Int32* length);

    /**
     * Estimates the length of a formatted header element.
     *
     * @param elem      the header element to format, or <code>null</code>
     *
     * @return  a length estimate, in number of characters
     */
    CARAPI EstimateHeaderElementLen(
        /* [in] */ IHeaderElement* elem,
        /* [out] */ Int32* length);

    /**
     * Estimates the length of formatted parameters.
     *
     * @param nvps      the parameters to format, or <code>null</code>
     *
     * @return  a length estimate, in number of characters
     */
    CARAPI EstimateParametersLen(
        /* [in] */ ArrayOf<INameValuePair*>* nvps,
        /* [out] */ Int32* length);

    /**
     * Estimates the length of a formatted name-value pair.
     *
     * @param nvp       the name-value pair to format, or <code>null</code>
     *
     * @return  a length estimate, in number of characters
     */
    CARAPI EstimateNameValuePairLen(
        /* [in] */ INameValuePair* nvp,
        /* [out] */ Int32* length);

    /**
     * Actually formats the value of a name-value pair.
     * This does not include a leading = character.
     * Called from {@link #formatNameValuePair formatNameValuePair}.
     *
     * @param buffer    the buffer to append to, never <code>null</code>
     * @param value     the value to append, never <code>null</code>
     * @param quote     <code>true</code> to always format with quotes,
     *                  <code>false</code> to use quotes only when necessary
     */
    CARAPI DoFormatValue(
        /* [in] */ ICharArrayBuffer* buffer,
        /* [in] */ const String& value,
        /* [in] */ Boolean quote);

    /**
     * Checks whether a character is a {@link #SEPARATORS separator}.
     *
     * @param ch        the character to check
     *
     * @return  <code>true</code> if the character is a separator,
     *          <code>false</code> otherwise
     */
    CARAPI IsSeparator(
        /* [in] */ Char32 ch,
        /* [out] */ Boolean* isSeparator);

    /**
     * Checks whether a character is {@link #UNSAFE_CHARS unsafe}.
     *
     * @param ch        the character to check
     *
     * @return  <code>true</code> if the character is unsafe,
     *          <code>false</code> otherwise
     */
    CARAPI IsUnsafe(
        /* [in] */ Char32 ch,
        /* [out] */ Boolean* isUnsafe);

public:
    /**
     * A default instance of this class, for use as default or fallback.
     * Note that {@link BasicHeaderValueFormatter} is not a singleton, there
     * can be many instances of the class itself and of derived classes.
     * The instance here provides non-customized, default behavior.
     */
    static const AutoPtr<IBasicHeaderValueFormatter> DEFAULT;
};

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_MESSAGE_BASICHEADERVALUEFORMATTER_H_
