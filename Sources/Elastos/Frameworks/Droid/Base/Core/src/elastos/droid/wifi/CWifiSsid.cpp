
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/wifi/CWifiSsid.h"
#include <elastos/core/Character.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::Character;
using Elastos::Core::StringUtils;
using Elastos::IO::IOutputStream;
using Elastos::IO::IBuffer;
using Elastos::IO::IByteBuffer;
using Elastos::IO::ICharBuffer;
using Elastos::IO::ICharBufferHelper;
using Elastos::IO::CCharBufferHelper;
using Elastos::IO::IByteBufferHelper;
using Elastos::IO::CByteBufferHelper;
using Elastos::IO::CByteArrayOutputStream;
using Elastos::IO::Charset::ICharset;
using Elastos::IO::Charset::ICoderResult;
using Elastos::IO::Charset::ICharsetHelper;
using Elastos::IO::Charset::CCharsetHelper;
using Elastos::IO::Charset::ICharsetDecoder;
using Elastos::IO::Charset::ICodingErrorAction;
using Elastos::IO::Charset::ICodingErrorActionHelper;
using Elastos::IO::Charset::CCodingErrorActionHelper;

namespace Elastos {
namespace Droid {
namespace Wifi {

static String TAG("WifiSsid");
static Int32 HEX_RADIX = 16;

CAR_INTERFACE_IMPL_2(CWifiSsid, Object, IWifiSsid, IParcelable)

CAR_OBJECT_IMPL(CWifiSsid)

ECode CWifiSsid::constructor()
{
    return CByteArrayOutputStream::New((IByteArrayOutputStream**)&mOctets);
}

ECode CWifiSsid::CreateFromAsciiEncoded(
    /* [in] */ const String& asciiEncoded,
    /* [out] */ IWifiSsid** wifiSsid)
{
    VALIDATE_NOT_NULL(wifiSsid);

    AutoPtr<CWifiSsid> a;
    FAIL_RETURN(CWifiSsid::NewByFriend((CWifiSsid**)&a));
    a->ConvertToBytes(asciiEncoded);
    *wifiSsid = (IWifiSsid*)a.Get();
    REFCOUNT_ADD(*wifiSsid);
    return NOERROR;
}

ECode CWifiSsid::CreateFromHex(
    /* [in] */ const String& _hexStr,
    /* [out] */ IWifiSsid** wifiSsid)
{
    VALIDATE_NOT_NULL(wifiSsid);

    String hexStr = _hexStr;
    AutoPtr<CWifiSsid> a;
    FAIL_RETURN(CWifiSsid::NewByFriend((CWifiSsid**)&a));
    if (hexStr.IsNull()) {
        *wifiSsid = (IWifiSsid*)a.Get();
        REFCOUNT_ADD(*wifiSsid)
        return NOERROR;
    }

    if (hexStr.StartWith("0x") || hexStr.StartWith("0X")) {
        hexStr = hexStr.Substring(2);
    }

    for (Int32 i = 0; i < hexStr.GetLength() - 1; i += 2) {
        Int32 val;
        // try {
        val = StringUtils::ParseInt32(hexStr.Substring(i, i + 2), HEX_RADIX);
        // } catch(NumberFormatException e) {
        //     val = 0;
        // }
        IOutputStream::Probe(a->mOctets)->Write(val);
    }
    *wifiSsid = (IWifiSsid*)a.Get();
    REFCOUNT_ADD(*wifiSsid)
    return NOERROR;
}

void CWifiSsid::ConvertToBytes(
    /* [in] */ const String& asciiEncoded)
{
    if (asciiEncoded.IsNullOrEmpty()) return;

    Int32 i = 0, val = 0;
    Char32 c;
    AutoPtr<ArrayOf<Char32> > charArray = asciiEncoded.GetChars();
    while (i < charArray->GetLength()) {
        c = (*charArray)[i];
        switch (c) {
            case '\\':
                i++;
                switch ((*charArray)[i]) {
                    case '\\':
                        IOutputStream::Probe(mOctets)->Write('\\');
                        i++;
                        break;
                    case '"':
                        IOutputStream::Probe(mOctets)->Write('"');
                        i++;
                        break;
                    case 'n':
                        IOutputStream::Probe(mOctets)->Write('\n');
                        i++;
                        break;
                    case 'r':
                        IOutputStream::Probe(mOctets)->Write('\r');
                        i++;
                        break;
                    case 't':
                        IOutputStream::Probe(mOctets)->Write('\t');
                        i++;
                        break;
                    case 'e':
                        IOutputStream::Probe(mOctets)->Write(27); //escape char
                        i++;
                        break;
                    case 'x':
                        i++;
                        // try {
                        val = StringUtils::ParseInt32(asciiEncoded.Substring(i, i + 2), HEX_RADIX);
                        // } catch (NumberFormatException e) {
                        //     val = -1;
                        // }
                        if (val < 0) {
                            val = Character::ToDigit((*charArray)[i], HEX_RADIX);
                            if (val < 0) break;
                            IOutputStream::Probe(mOctets)->Write(val);
                            i++;
                        }
                        else {
                            IOutputStream::Probe(mOctets)->Write(val);
                            i += 2;
                        }
                        break;
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                        val = (*charArray)[i] - '0';
                        i++;
                        if ((*charArray)[i] >= '0' && (*charArray)[i] <= '7') {
                            val = val * 8 + (*charArray)[i] - '0';
                            i++;
                        }
                        if ((*charArray)[i] >= '0' && (*charArray)[i] <= '7') {
                            val = val * 8 + (*charArray)[i] - '0';
                            i++;
                        }
                        IOutputStream::Probe(mOctets)->Write(val);
                        break;
                    default:
                        break;
                }
                break;
            default:
                IOutputStream::Probe(mOctets)->Write(c);
                i++;
                break;
        }
    }
}

ECode CWifiSsid::ToString(
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);

    AutoPtr< ArrayOf<Byte> > ssidBytes;
    FAIL_RETURN(mOctets->ToByteArray((ArrayOf<Byte>**)&ssidBytes));

    // Supplicant returns \x00\x00\x00\x00\x00\x00\x00\x00 hex string
    // for a hidden access point. Make sure we maintain the previous
    // behavior of returning empty string for this case.
    Int32 size;
    if ((mOctets->GetSize(&size), size <= 0) || IsArrayAllZeroes(ssidBytes)) {
        *value = "";
        return NOERROR;
    }
    // TODO: Handle conversion to other charsets upon failure
    AutoPtr<ICharsetHelper> helper;
    CCharsetHelper::AcquireSingleton((ICharsetHelper**)&helper);
    AutoPtr<ICharset> charset;
    helper->ForName(String("UTF-8"), (ICharset**)&charset);
    AutoPtr<ICharsetDecoder> decoder;
    charset->NewDecoder((ICharsetDecoder**)&decoder);

    AutoPtr<ICodingErrorActionHelper> ceActionHelper;
    CCodingErrorActionHelper::AcquireSingleton((ICodingErrorActionHelper**)&ceActionHelper);
    AutoPtr<ICodingErrorAction> replace;
    ceActionHelper->GetREPLACE((ICodingErrorAction**)&replace);

    decoder->OnMalformedInput(replace);
    decoder->OnUnmappableCharacter(replace);
    AutoPtr<ICharBufferHelper> bufferHelper;
    CCharBufferHelper::AcquireSingleton((ICharBufferHelper**)&bufferHelper);
    AutoPtr<ICharBuffer> out;
    bufferHelper->Allocate(32, (ICharBuffer**)&out);

    AutoPtr<IByteBufferHelper> byteHelper;
    CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&byteHelper);
    AutoPtr<IByteBuffer> buffer;
    byteHelper->Wrap(ssidBytes, (IByteBuffer**)&buffer);
    AutoPtr<ICoderResult> result;
    decoder->Decode(buffer, out, TRUE, (ICoderResult**)&result);
    IBuffer::Probe(out)->Flip();
    Boolean error;
    if (result->IsError(&error), error) {
        *value = NONE;
        return NOERROR;
    }
    IObject::Probe(out)->ToString(value);
    return NOERROR;
}

Boolean CWifiSsid::IsArrayAllZeroes(
    /* [in] */ const ArrayOf<Byte>* ssidBytes)
{
    for (Int32 i = 0; i < (Int32)ssidBytes->GetLength(); i++) {
        if ((*ssidBytes)[i] != 0) return FALSE;
    }
    return TRUE;
}

ECode CWifiSsid::GetOctets(
    /* [out] */ IByteArrayOutputStream** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOctets;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CWifiSsid::IsHidden(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ArrayOf<Byte> > array;
    mOctets->ToByteArray((ArrayOf<Byte>**)&array);
    *result = IsArrayAllZeroes(array);
    return NOERROR;
}

ECode CWifiSsid::GetOctets(
    /* [out, callee] */ ArrayOf<Byte>** octets)
{
    VALIDATE_NOT_NULL(octets);
    return mOctets->ToByteArray(octets);
}

ECode CWifiSsid::GetHexString(
    /* [out] */ String* hexString)
{
    VALIDATE_NOT_NULL(hexString);

    String out("0x");
    AutoPtr<ArrayOf<Byte> > ssidbytes;
    FAIL_RETURN(GetOctets((ArrayOf<Byte>**)&ssidbytes));
    for (Int32 i = 0; i < ssidbytes->GetLength(); i++) {
        out.AppendFormat(/*Locale.US, */"%02x", (*ssidbytes)[i]);
    }
    *hexString = out;
    return NOERROR;
}

ECode CWifiSsid::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    Int32 length;
    source->ReadInt32(&length);
    AutoPtr< ArrayOf<Byte> > b;
    source->ReadArrayOf((Handle32*)&b);
    IOutputStream::Probe(mOctets)->Write(b, 0, length);
    return NOERROR;
}

ECode CWifiSsid::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    Int32 size;
    mOctets->GetSize(&size);
    dest->WriteInt32(size);
    AutoPtr<ArrayOf<Byte> > array;
    mOctets->ToByteArray((ArrayOf<Byte>**)&array);
    dest->WriteArrayOf((Handle32)array.Get());
    return NOERROR;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
