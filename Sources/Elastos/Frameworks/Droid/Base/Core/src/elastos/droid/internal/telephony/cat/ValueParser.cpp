
#include "Elastos.Droid.Internal.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/internal/telephony/cat/ValueParser.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

//=====================================================================
//                             ValueParser
//=====================================================================
AutoPtr<CommandDetails> ValueParser::RetrieveCommandDetails(
    /* [in] */ ComprehensionTlv* ctlv)
{
    // ==================before translated======================
    //
    // CommandDetails cmdDet = new CommandDetails();
    // byte[] rawValue = ctlv.getRawValue();
    // int valueIndex = ctlv.getValueIndex();
    // try {
    //     cmdDet.compRequired = ctlv.isComprehensionRequired();
    //     cmdDet.commandNumber = rawValue[valueIndex] & 0xff;
    //     cmdDet.typeOfCommand = rawValue[valueIndex + 1] & 0xff;
    //     cmdDet.commandQualifier = rawValue[valueIndex + 2] & 0xff;
    //     return cmdDet;
    // } catch (IndexOutOfBoundsException e) {
    //     throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
    // }
    assert(0);
    AutoPtr<CommandDetails> empty;
    return empty;
}

AutoPtr<DeviceIdentities> ValueParser::RetrieveDeviceIdentities(
    /* [in] */ ComprehensionTlv* ctlv)
{
    // ==================before translated======================
    //
    // DeviceIdentities devIds = new DeviceIdentities();
    // byte[] rawValue = ctlv.getRawValue();
    // int valueIndex = ctlv.getValueIndex();
    // try {
    //     devIds.sourceId = rawValue[valueIndex] & 0xff;
    //     devIds.destinationId = rawValue[valueIndex + 1] & 0xff;
    //     return devIds;
    // } catch (IndexOutOfBoundsException e) {
    //     throw new ResultException(ResultCode.REQUIRED_VALUES_MISSING);
    // }
    assert(0);
    AutoPtr<DeviceIdentities> empty;
    return empty;
}

AutoPtr<IDuration> ValueParser::RetrieveDuration(
    /* [in] */ ComprehensionTlv* ctlv)
{
    // ==================before translated======================
    // int timeInterval = 0;
    // TimeUnit timeUnit = TimeUnit.SECOND;
    //
    // byte[] rawValue = ctlv.getRawValue();
    // int valueIndex = ctlv.getValueIndex();
    //
    // try {
    //     timeUnit = TimeUnit.values()[(rawValue[valueIndex] & 0xff)];
    //     timeInterval = rawValue[valueIndex + 1] & 0xff;
    // } catch (IndexOutOfBoundsException e) {
    //     throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
    // }
    // return new Duration(timeInterval, timeUnit);
    assert(0);
    AutoPtr<IDuration> empty;
    return empty;
}

AutoPtr<IItem> ValueParser::RetrieveItem(
    /* [in] */ ComprehensionTlv* ctlv)
{
    // ==================before translated======================
    // Item item = null;
    //
    // byte[] rawValue = ctlv.getRawValue();
    // int valueIndex = ctlv.getValueIndex();
    // int length = ctlv.getLength();
    //
    // if (length != 0) {
    //     int textLen = length - 1;
    //
    //     try {
    //         int id = rawValue[valueIndex] & 0xff;
    //         String text = IccUtils.adnStringFieldToString(rawValue,
    //                 valueIndex + 1, textLen);
    //         item = new Item(id, text);
    //     } catch (IndexOutOfBoundsException e) {
    //         throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
    //     }
    // }
    //
    // return item;
    assert(0);
    AutoPtr<IItem> empty;
    return empty;
}

Int32 ValueParser::RetrieveItemId(
    /* [in] */ ComprehensionTlv* ctlv)
{
    // ==================before translated======================
    // int id = 0;
    //
    // byte[] rawValue = ctlv.getRawValue();
    // int valueIndex = ctlv.getValueIndex();
    //
    // try {
    //     id = rawValue[valueIndex] & 0xff;
    // } catch (IndexOutOfBoundsException e) {
    //     throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
    // }
    //
    // return id;
    assert(0);
    return 0;
}

AutoPtr<IconId> ValueParser::RetrieveIconId(
    /* [in] */ ComprehensionTlv* ctlv)
{
    // ==================before translated======================
    // IconId id = new IconId();
    //
    // byte[] rawValue = ctlv.getRawValue();
    // int valueIndex = ctlv.getValueIndex();
    // try {
    //     id.selfExplanatory = (rawValue[valueIndex++] & 0xff) == 0x00;
    //     id.recordNumber = rawValue[valueIndex] & 0xff;
    // } catch (IndexOutOfBoundsException e) {
    //     throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
    // }
    //
    // return id;
    assert(0);
    AutoPtr<IconId> empty;
    return empty;
}

AutoPtr<ItemsIconId> ValueParser::RetrieveItemsIconId(
    /* [in] */ ComprehensionTlv* ctlv)
{
    // ==================before translated======================
    // CatLog.d("ValueParser", "retrieveItemsIconId:");
    // ItemsIconId id = new ItemsIconId();
    //
    // byte[] rawValue = ctlv.getRawValue();
    // int valueIndex = ctlv.getValueIndex();
    // int numOfItems = ctlv.getLength() - 1;
    // id.recordNumbers = new int[numOfItems];
    //
    // try {
    //     // get icon self-explanatory
    //     id.selfExplanatory = (rawValue[valueIndex++] & 0xff) == 0x00;
    //
    //     for (int index = 0; index < numOfItems;) {
    //         id.recordNumbers[index++] = rawValue[valueIndex++];
    //     }
    // } catch (IndexOutOfBoundsException e) {
    //     throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
    // }
    // return id;
    assert(0);
    AutoPtr<ItemsIconId> empty;
    return empty;
}

AutoPtr<IList/*< AutoPtr<TextAttribute> >*/> ValueParser::RetrieveTextAttribute(
    /* [in] */ ComprehensionTlv* ctlv)
{
    // ==================before translated======================
    // ArrayList<TextAttribute> lst = new ArrayList<TextAttribute>();
    //
    // byte[] rawValue = ctlv.getRawValue();
    // int valueIndex = ctlv.getValueIndex();
    // int length = ctlv.getLength();
    //
    // if (length != 0) {
    //     // Each attribute is consisted of four bytes
    //     int itemCount = length / 4;
    //
    //     try {
    //         for (int i = 0; i < itemCount; i++, valueIndex += 4) {
    //             int start = rawValue[valueIndex] & 0xff;
    //             int textLength = rawValue[valueIndex + 1] & 0xff;
    //             int format = rawValue[valueIndex + 2] & 0xff;
    //             int colorValue = rawValue[valueIndex + 3] & 0xff;
    //
    //             int alignValue = format & 0x03;
    //             TextAlignment align = TextAlignment.fromInt(alignValue);
    //
    //             int sizeValue = (format >> 2) & 0x03;
    //             FontSize size = FontSize.fromInt(sizeValue);
    //             if (size == null) {
    //                 // Font size value is not defined. Use default.
    //                 size = FontSize.NORMAL;
    //             }
    //
    //             boolean bold = (format & 0x10) != 0;
    //             boolean italic = (format & 0x20) != 0;
    //             boolean underlined = (format & 0x40) != 0;
    //             boolean strikeThrough = (format & 0x80) != 0;
    //
    //             TextColor color = TextColor.fromInt(colorValue);
    //
    //             TextAttribute attr = new TextAttribute(start, textLength,
    //                     align, size, bold, italic, underlined,
    //                     strikeThrough, color);
    //             lst.add(attr);
    //         }
    //
    //         return lst;
    //
    //     } catch (IndexOutOfBoundsException e) {
    //         throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
    //     }
    // }
    // return null;
    assert(0);
    AutoPtr<IList/*< AutoPtr<TextAttribute>>*/> empty;
    return empty;
}

String ValueParser::RetrieveAlphaId(
    /* [in] */ ComprehensionTlv* ctlv)
{
    // ==================before translated======================
    //
    // if (ctlv != null) {
    //     byte[] rawValue = ctlv.getRawValue();
    //     int valueIndex = ctlv.getValueIndex();
    //     int length = ctlv.getLength();
    //     if (length != 0) {
    //         try {
    //             return IccUtils.adnStringFieldToString(rawValue, valueIndex,
    //                     length);
    //         } catch (IndexOutOfBoundsException e) {
    //             throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
    //         }
    //     } else {
    //         CatLog.d("ValueParser", "Alpha Id length=" + length);
    //         return null;
    //     }
    // } else {
    //     /* Per 3GPP specification 102.223,
    //      * if the alpha identifier is not provided by the UICC,
    //      * the terminal MAY give information to the user
    //      * noAlphaUsrCnf defines if you need to show user confirmation or not
    //      */
    //     boolean noAlphaUsrCnf = false;
    //     Resources resource = Resources.getSystem();
    //     try {
    //         noAlphaUsrCnf = resource.getBoolean(
    //                 com.android.internal.R.bool.config_stkNoAlphaUsrCnf);
    //     } catch (NotFoundException e) {
    //         noAlphaUsrCnf = false;
    //     }
    //     return (noAlphaUsrCnf ? null : CatService.STK_DEFAULT);
    // }
    assert(0);
    return String("");
}

String ValueParser::RetrieveTextString(
    /* [in] */ ComprehensionTlv* ctlv)
{
    // ==================before translated======================
    // byte[] rawValue = ctlv.getRawValue();
    // int valueIndex = ctlv.getValueIndex();
    // byte codingScheme = 0x00;
    // String text = null;
    // int textLen = ctlv.getLength();
    //
    // // In case the text length is 0, return a null string.
    // if (textLen == 0) {
    //     return text;
    // } else {
    //     // one byte is coding scheme
    //     textLen -= 1;
    // }
    //
    // try {
    //     codingScheme = (byte) (rawValue[valueIndex] & 0x0c);
    //
    //     if (codingScheme == 0x00) { // GSM 7-bit packed
    //         text = GsmAlphabet.gsm7BitPackedToString(rawValue,
    //                 valueIndex + 1, (textLen * 8) / 7);
    //     } else if (codingScheme == 0x04) { // GSM 8-bit unpacked
    //         text = GsmAlphabet.gsm8BitUnpackedToString(rawValue,
    //                 valueIndex + 1, textLen);
    //     } else if (codingScheme == 0x08) { // UCS2
    //         text = new String(rawValue, valueIndex + 1, textLen, "UTF-16");
    //     } else {
    //         throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
    //     }
    //
    //     return text;
    // } catch (IndexOutOfBoundsException e) {
    //     throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
    // } catch (UnsupportedEncodingException e) {
    //     // This should never happen.
    //     throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
    // }
    assert(0);
    return String("");
}

Int32 ValueParser::RetrieveTarget(
    /* [in] */ ComprehensionTlv* ctlv)
{
    // ==================before translated======================
    // ActivateDescriptor activateDesc = new ActivateDescriptor();
    // byte[] rawValue = ctlv.getRawValue();
    // int valueIndex = ctlv.getValueIndex();
    // try {
    //     activateDesc.target = rawValue[valueIndex] & 0xff;
    //     return activateDesc.target;
    // } catch (IndexOutOfBoundsException e) {
    //     throw new ResultException(ResultCode.REQUIRED_VALUES_MISSING);
    // }
    assert(0);
    return 0;
}

String ValueParser::RetrieveSMSCaddress(
    /* [in] */ ComprehensionTlv* ctlv)
{
    // ==================before translated======================
    // byte[] rawValue = ctlv.getRawValue();
    // int valueIndex = ctlv.getValueIndex();
    // int length = ctlv.getLength();
    // byte[] outputValue = new byte[length + 1];
    //
    // for (int k = 0; k <= length; k++) {
    //     try {
    //         outputValue[k] = rawValue[k + (valueIndex - 1)];
    //     }
    //     catch (IndexOutOfBoundsException indexoutofboundsexception) {
    //         throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
    //     }
    // }
    // if (length != 0)
    //     return IccUtils.bytesToHexString(outputValue);
    // else
    //     throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
    assert(0);
    return String("");
}

String ValueParser::RetrieveSMSTPDU(
    /* [in] */ ComprehensionTlv* ctlv)
{
    // ==================before translated======================
    // byte[] rawValue = ctlv.getRawValue();
    // int valueIndex = ctlv.getValueIndex();
    // int pduLength = ctlv.getLength();
    // byte[] outputValue;
    // int k;
    // String result;
    // if (rawValue[valueIndex + 2] % 2 == 0)
    //     k = rawValue[valueIndex + 2] / 2;
    // else
    //     k = (1 + rawValue[valueIndex + 2]) / 2;
    //
    // if (pduLength == k + 6)
    //     outputValue = new byte[pduLength + 1];
    // else
    //     outputValue = new byte[pduLength];
    //
    // for (int l = 0; l < pduLength; l++) {
    //     try {
    //         outputValue[l] = rawValue[valueIndex + l];
    //     }
    //     catch (IndexOutOfBoundsException ex) {
    //         throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
    //     }
    // }
    // if (pduLength != 0)
    //     result = IccUtils.bytesToHexString(outputValue);
    // else
    //     throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
    //
    // return result;
    assert(0);
    return String("");
}

String ValueParser::RetrieveUSSDString(
    /* [in] */ ComprehensionTlv* ctlv)
{
    // ==================before translated======================
    // byte[] rawValue = ctlv.getRawValue();
    // int valueIndex = ctlv.getValueIndex();
    // int length = ctlv.getLength();
    //
    // // If length is 0 (shouldn't be), return null
    // if (length == 0) {
    //     return null;
    // }
    //
    // // Should be 0x0f
    // if (rawValue[valueIndex] != 0x0f) {
    //     throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
    // }
    //
    // try {
    //     return GsmAlphabet.gsm7BitPackedToString(rawValue,
    //             valueIndex + 1, ((length - 1) * 8) / 7);
    // } catch (IndexOutOfBoundsException e) {
    //     throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
    // }
    assert(0);
    return String("");
}

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
