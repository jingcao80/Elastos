
#ifndef __ORG_APACHE_HTTP_UTILITY_CCHARARRAYBUFFER_H_
#define __ORG_APACHE_HTTP_UTILITY_CCHARARRAYBUFFER_H_

#include "_Org_Apache_Http_Utility_CCharArrayBuffer.h"
#include "elastos/core/Object.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Utility {

/**
 * A request interceptor that adds a User-Agent header.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 496070 $
 *
 * @since 4.0
 */
CarClass(CCharArrayBuffer)
    , public Object
    , public ICharArrayBuffer
{
public:
    CCharArrayBuffer();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI Append(
        /* [in] */ ArrayOf<Char32>* b,
        /* [in] */ Int32 off,
        /* [in] */ Int32 len);

    CARAPI Append(
        /* [in] */ const String& str);

    CARAPI Append(
        /* [in] */ ICharArrayBuffer* b,
        /* [in] */ Int32 off,
        /* [in] */ Int32 len);

    CARAPI Append(
        /* [in] */ ICharArrayBuffer* b);

    CARAPI Append(
        /* [in] */ Char32 ch);

    CARAPI Append(
        /* [in] */ ArrayOf<Byte>* b,
        /* [in] */ Int32 off,
        /* [in] */ Int32 len);

    CARAPI Append(
        /* [in] */ IByteArrayBuffer* b,
        /* [in] */ Int32 off,
        /* [in] */ Int32 len);

    CARAPI Append(
        /* [in] */ IInterface* obj);

    CARAPI Clear();

    CARAPI ToCharArray(
        /* [out, callee] */ ArrayOf<Char32>** charArray);

    CARAPI CharAt(
        /* [in] */ Int32 i,
        /* [out] */ Char32* charAt);

    CARAPI GetBuffer(
        /* [out, callee] */ ArrayOf<Char32>** buffer);

    CARAPI GetCapacity(
        /* [out] */ Int32* capacity);

    CARAPI GetLength(
        /* [out] */ Int32* length);

    CARAPI EnsureCapacity(
        /* [in] */ Int32 required);

    CARAPI SetLength(
        /* [in] */ Int32 len);

    CARAPI IsEmpty(
        /* [out] */ Boolean* isEmpty);

    CARAPI IsFull(
        /* [out] */ Boolean* isFull);

    CARAPI IndexOf(
        /* [in] */ Int32 ch,
        /* [in] */ Int32 beginIndex,
        /* [in] */ Int32 endIndex,
        /* [out] */ Int32* index);

    CARAPI IndexOf(
        /* [in] */ Int32 ch,
        /* [out] */ Int32* index);

    CARAPI Substring(
        /* [in] */ Int32 beginIndex,
        /* [in] */ Int32 endIndex,
        /* [out] */ String* string);

    CARAPI SubstringTrimmed(
        /* [in] */ Int32 beginIndex,
        /* [in] */ Int32 endIndex,
        /* [out] */ String* string);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI constructor(
        /* [in] */ Int32 capacity);

private:
    CARAPI_(void) Expand(
        /* [in] */ Int32 newlen);

private:
    AutoPtr< ArrayOf<Char32> > mBuffer;
    Int32 mLen;

};

} // namespace Utility
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_UTILITY_CCHARARRAYBUFFER_H_
