
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Text.h>
#include "elastos/droid/webkit/native/net/NetStringUtil.h"

using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::IO::Charset::CCharsetHelper;
using Elastos::IO::Charset::CCodingErrorActionHelper;
using Elastos::IO::Charset::ICharset;
using Elastos::IO::Charset::ICharsetDecoder;
using Elastos::IO::Charset::ICharsetHelper;
using Elastos::IO::Charset::ICodingErrorAction;
using Elastos::IO::Charset::ICodingErrorActionHelper;
using Elastos::IO::ICharBuffer;
using Elastos::Text::CNormalizerHelper;
using Elastos::Text::INormalizer;
using Elastos::Text::NormalizerForm_NFC;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Net {

//=====================================================================
//                            NetStringUtil
//=====================================================================
String NetStringUtil::ConvertToUnicode(
    /* [in] */ IByteBuffer* text,
    /* [in] */ const String& charset_name)
{
    // ==================before translated======================
    // try {
    //     Charset charset = Charset.forName(charset_name);
    //     CharsetDecoder decoder = charset.newDecoder();
    //     // On invalid characters, this will throw an exception.
    //     return decoder.decode(text).toString();
    // } catch (Exception e) {
    //     return null;
    // }

    //try {
        AutoPtr<ICharset> charset;
        AutoPtr<ICharsetHelper> helper;
        CCharsetHelper::AcquireSingleton((ICharsetHelper**)&helper);
        helper->ForName(charset_name, (ICharset**)&charset);

        AutoPtr<ICharsetDecoder> decoder;
        charset->NewDecoder((ICharsetDecoder**)&decoder);
        AutoPtr<ICharBuffer> charBuffer;
        decoder->Decode(text, (ICharBuffer**)&charBuffer);
        // On invalid characters, this will throw an exception.

        AutoPtr< ArrayOf<Char32> > tmp;
        charBuffer->GetArray((ArrayOf<Char32>**)&tmp);
        String result(*tmp);
        return result;
    //} catch (Exception e) {
    //    return null;
    //}
}

String NetStringUtil::ConvertToUnicodeAndNormalize(
    /* [in] */ IByteBuffer* text,
    /* [in] */ const String& charset_name)
{
    // ==================before translated======================
    // String unicodeString = convertToUnicode(text, charset_name);
    // if (unicodeString == null)
    //     return unicodeString;
    // return Normalizer.normalize(unicodeString, Normalizer.Form.NFC);

    String unicodeString = ConvertToUnicode(text, charset_name);
    if (unicodeString == String(""))
        return unicodeString;

    AutoPtr<INormalizer> normalizer;
    CNormalizerHelper::AcquireSingleton((INormalizer**)&normalizer);

    String result;
    AutoPtr<ICharSequence> charSquenceStr;
    CString::New(unicodeString, (ICharSequence**)&charSquenceStr);
    normalizer->Normalize(charSquenceStr, NormalizerForm_NFC, &result);
    return result;
}

String NetStringUtil::ConvertToUnicodeWithSubstitutions(
    /* [in] */ IByteBuffer* text,
    /* [in] */ const String& charset_name)
{
    // ==================before translated======================
    // try {
    //     Charset charset = Charset.forName(charset_name);
    //
    //     // TODO(mmenke):  Investigate if Charset.decode() can be used
    //     // instead.  The question is whether it uses the proper replace
    //     // character.  JDK CharsetDecoder docs say U+FFFD is the default,
    //     // but Charset.decode() docs say it uses the "charset's default
    //     // replacement byte array".
    //     CharsetDecoder decoder = charset.newDecoder();
    //     decoder.onMalformedInput(CodingErrorAction.REPLACE);
    //     decoder.onUnmappableCharacter(CodingErrorAction.REPLACE);
    //     decoder.replaceWith("\uFFFD");
    //     return decoder.decode(text).toString();
    // } catch (Exception e) {
    //     return null;
    // }

    assert(0);
    //try {
        AutoPtr<ICharset> charset;
        AutoPtr<ICharsetHelper> helper;
        CCharsetHelper::AcquireSingleton((ICharsetHelper**)&helper);
        helper->ForName(charset_name, (ICharset**)&charset);

        // TODO(mmenke):  Investigate if Charset.decode() can be used
        // instead.  The question is whether it uses the proper replace
        // character.  JDK CharsetDecoder docs say U+FFFD is the default,
        // but Charset.decode() docs say it uses the "charset's default
        // replacement byte array".
        AutoPtr<ICharsetDecoder> decoder;
        charset->NewDecoder((ICharsetDecoder**)&decoder);

        AutoPtr<ICodingErrorActionHelper> errorActionhelper;
        CCodingErrorActionHelper::AcquireSingleton((ICodingErrorActionHelper**)&errorActionhelper);
        AutoPtr<ICodingErrorAction> errorActionReplace;
        errorActionhelper->GetREPLACE((ICodingErrorAction**)&errorActionReplace);

        decoder->OnMalformedInput((ICodingErrorAction*)&errorActionReplace);
        decoder->OnUnmappableCharacter((ICodingErrorAction*)&errorActionReplace);
        decoder->ReplaceWith(String("\uFFFD"));

        AutoPtr<ICharBuffer> charBuffer;
        decoder->Decode(text, (ICharBuffer**)&charBuffer);
        // On invalid characters, this will throw an exception.

        AutoPtr< ArrayOf<Char32> > tmp;
        charBuffer->GetArray((ArrayOf<Char32>**)&tmp);
        String result(*tmp);
        return result;
    //} catch (Exception e) {
    //    return null;
    //}
}

} // namespace Net
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


