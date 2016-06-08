
#include "Elastos.Droid.Internal.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/internal/telephony/cat/BerTlv.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

//=====================================================================
//                                BerTlv
//=====================================================================
const Int32 BerTlv::BER_UNKNOWN_TAG;
const Int32 BerTlv::BER_PROACTIVE_COMMAND_TAG;
const Int32 BerTlv::BER_MENU_SELECTION_TAG;
const Int32 BerTlv::BER_EVENT_DOWNLOAD_TAG;

ECode BerTlv::GetComprehensionTlvs(
    /* [out] */ IList/*<ComprehensionTlv>*/** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mCompTlvs;
    assert(0);
    return NOERROR;
}

ECode BerTlv::GetTag(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mTag;
    assert(0);
    return NOERROR;
}

ECode BerTlv::IsLengthValid(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mLengthValid;
    assert(0);
    return NOERROR;
}

AutoPtr<BerTlv> BerTlv::Decode(
    /* [in] */ ArrayOf<Byte>* data)
{
    // ==================before translated======================
    // int curIndex = 0;
    // int endIndex = data.length;
    // int tag, length = 0;
    // boolean isLengthValid = true;
    //
    // try {
    //     /* tag */
    //     tag = data[curIndex++] & 0xff;
    //     if (tag == BER_PROACTIVE_COMMAND_TAG) {
    //         /* length */
    //         int temp = data[curIndex++] & 0xff;
    //         if (temp < 0x80) {
    //             length = temp;
    //         } else if (temp == 0x81) {
    //             temp = data[curIndex++] & 0xff;
    //             if (temp < 0x80) {
    //                 throw new ResultException(
    //                         ResultCode.CMD_DATA_NOT_UNDERSTOOD,
    //                         "length < 0x80 length=" + Integer.toHexString(length) +
    //                         " curIndex=" + curIndex + " endIndex=" + endIndex);
    //
    //             }
    //             length = temp;
    //         } else {
    //             throw new ResultException(
    //                     ResultCode.CMD_DATA_NOT_UNDERSTOOD,
    //                     "Expected first byte to be length or a length tag and < 0x81" +
    //                     " byte= " + Integer.toHexString(temp) + " curIndex=" + curIndex +
    //                     " endIndex=" + endIndex);
    //         }
    //     } else {
    //         if (ComprehensionTlvTag.COMMAND_DETAILS.value() == (tag & ~0x80)) {
    //             tag = BER_UNKNOWN_TAG;
    //             curIndex = 0;
    //         }
    //     }
    // } catch (IndexOutOfBoundsException e) {
    //     throw new ResultException(ResultCode.REQUIRED_VALUES_MISSING,
    //             "IndexOutOfBoundsException " +
    //             " curIndex=" + curIndex + " endIndex=" + endIndex);
    // } catch (ResultException e) {
    //     throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD, e.explanation());
    // }
    //
    // /* COMPREHENSION-TLVs */
    // if (endIndex - curIndex < length) {
    //     throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD,
    //             "Command had extra data endIndex=" + endIndex + " curIndex=" + curIndex +
    //             " length=" + length);
    // }
    //
    // List<ComprehensionTlv> ctlvs = ComprehensionTlv.decodeMany(data,
    //         curIndex);
    //
    // if (tag == BER_PROACTIVE_COMMAND_TAG) {
    //     int totalLength = 0;
    //     for (ComprehensionTlv item : ctlvs) {
    //         int itemLength = item.getLength();
    //         if (itemLength >= 0x80 && itemLength <= 0xFF) {
    //             totalLength += itemLength + 3; //3: 'tag'(1 byte) and 'length'(2 bytes).
    //         } else if (itemLength >= 0 && itemLength < 0x80) {
    //             totalLength += itemLength + 2; //2: 'tag'(1 byte) and 'length'(1 byte).
    //         } else {
    //             isLengthValid = false;
    //             break;
    //         }
    //     }
    //
    //     // According to 3gpp11.14, chapter 6.10.6 "Length errors",
    //
    //     // If the total lengths of the SIMPLE-TLV data objects are not
    //     // consistent with the length given in the BER-TLV data object,
    //     // then the whole BER-TLV data object shall be rejected. The
    //     // result field in the TERMINAL RESPONSE shall have the error
    //     // condition "Command data not understood by ME".
    //     if (length != totalLength) {
    //         isLengthValid = false;
    //     }
    // }
    //
    // return new BerTlv(tag, ctlvs, isLengthValid);
    assert(0);
    AutoPtr<BerTlv> empty;
    return empty;
}

BerTlv::BerTlv(
    /* [in] */ Int32 tag,
    /* [in] */ IList/*<ComprehensionTlv*>*/* ctlvs,
    /* [in] */ Boolean lengthValid)
{
    // ==================before translated======================
    // mTag = tag;
    // mCompTlvs = ctlvs;
    // mLengthValid = lengthValid;
}

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
