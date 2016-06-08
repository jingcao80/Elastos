
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/cat/ComprehensionTlv.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

//=====================================================================
//                           ComprehensionTlv
//=====================================================================
const String ComprehensionTlv::LOGTAG("ComprehensionTlv");

ECode ComprehensionTlv::GetTag(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mTag;
    assert(0);
    return NOERROR;
}

ECode ComprehensionTlv::IsComprehensionRequired(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mCr;
    assert(0);
    return NOERROR;
}

ECode ComprehensionTlv::GetLength(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mLength;
    assert(0);
    return NOERROR;
}

ECode ComprehensionTlv::GetValueIndex(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mValueIndex;
    assert(0);
    return NOERROR;
}

ECode ComprehensionTlv::GetRawValue(
    /* [out] */ ArrayOf<Byte>* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mRawValue;
    assert(0);
    return NOERROR;
}

AutoPtr<IList/*< AutoPtr<ComprehensionTlv> >*/> ComprehensionTlv::DecodeMany(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 startIndex)
{
    // ==================before translated======================
    // ArrayList<ComprehensionTlv> items = new ArrayList<ComprehensionTlv>();
    // int endIndex = data.length;
    // while (startIndex < endIndex) {
    //     ComprehensionTlv ctlv = ComprehensionTlv.decode(data, startIndex);
    //     if (ctlv != null) {
    //         items.add(ctlv);
    //         startIndex = ctlv.mValueIndex + ctlv.mLength;
    //     } else {
    //         CatLog.d(LOGTAG, "decodeMany: ctlv is null, stop decoding");
    //         break;
    //     }
    // }
    //
    // return items;
    assert(0);
    AutoPtr<IList/*< AutoPtr<ComprehensionTlv> >*/> empty;
    return empty;
}

AutoPtr<ComprehensionTlv> ComprehensionTlv::Decode(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 startIndex)
{
    // ==================before translated======================
    // int curIndex = startIndex;
    // int endIndex = data.length;
    //
    // try {
    //     /* tag */
    //     int tag;
    //     boolean cr; // Comprehension required flag
    //     int temp = data[curIndex++] & 0xff;
    //     switch (temp) {
    //     case 0:
    //     case 0xff:
    //     case 0x80:
    //         Rlog.d("CAT     ", "decode: unexpected first tag byte=" + Integer.toHexString(temp) +
    //                 ", startIndex=" + startIndex + " curIndex=" + curIndex +
    //                 " endIndex=" + endIndex);
    //         // Return null which will stop decoding, this has occurred
    //         // with Ghana MTN simcard and JDI simcard.
    //         return null;
    //
    //     case 0x7f: // tag is in three-byte format
    //         tag = ((data[curIndex] & 0xff) << 8)
    //                 | (data[curIndex + 1] & 0xff);
    //         cr = (tag & 0x8000) != 0;
    //         tag &= ~0x8000;
    //         curIndex += 2;
    //         break;
    //
    //     default: // tag is in single-byte format
    //         tag = temp;
    //         cr = (tag & 0x80) != 0;
    //         tag &= ~0x80;
    //         break;
    //     }
    //
    //     /* length */
    //     int length;
    //     temp = data[curIndex++] & 0xff;
    //     if (temp < 0x80) {
    //         length = temp;
    //     } else if (temp == 0x81) {
    //         length = data[curIndex++] & 0xff;
    //         if (length < 0x80) {
    //             throw new ResultException(
    //                     ResultCode.CMD_DATA_NOT_UNDERSTOOD,
    //                     "length < 0x80 length=" + Integer.toHexString(length) +
    //                     " startIndex=" + startIndex + " curIndex=" + curIndex +
    //                     " endIndex=" + endIndex);
    //         }
    //     } else if (temp == 0x82) {
    //         length = ((data[curIndex] & 0xff) << 8)
    //                 | (data[curIndex + 1] & 0xff);
    //         curIndex += 2;
    //         if (length < 0x100) {
    //             throw new ResultException(
    //                     ResultCode.CMD_DATA_NOT_UNDERSTOOD,
    //                     "two byte length < 0x100 length=" + Integer.toHexString(length) +
    //                     " startIndex=" + startIndex + " curIndex=" + curIndex +
    //                     " endIndex=" + endIndex);
    //         }
    //     } else if (temp == 0x83) {
    //         length = ((data[curIndex] & 0xff) << 16)
    //                 | ((data[curIndex + 1] & 0xff) << 8)
    //                 | (data[curIndex + 2] & 0xff);
    //         curIndex += 3;
    //         if (length < 0x10000) {
    //             throw new ResultException(
    //                     ResultCode.CMD_DATA_NOT_UNDERSTOOD,
    //                     "three byte length < 0x10000 length=0x" + Integer.toHexString(length) +
    //                     " startIndex=" + startIndex + " curIndex=" + curIndex +
    //                     " endIndex=" + endIndex);
    //         }
    //     } else {
    //         throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD,
    //                 "Bad length modifer=" + temp +
    //                 " startIndex=" + startIndex + " curIndex=" + curIndex +
    //                 " endIndex=" + endIndex);
    //
    //     }
    //
    //     return new ComprehensionTlv(tag, cr, length, data, curIndex);
    //
    // } catch (IndexOutOfBoundsException e) {
    //     throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD,
    //             "IndexOutOfBoundsException" + " startIndex=" + startIndex +
    //             " curIndex=" + curIndex + " endIndex=" + endIndex);
    // }
    assert(0);
    AutoPtr<ComprehensionTlv> empty;
    return empty;
}

ComprehensionTlv::ComprehensionTlv(
    /* [in] */ Int32 tag,
    /* [in] */ Boolean cr,
    /* [in] */ Int32 length,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 valueIndex)
{
    // ==================before translated======================
    // mTag = tag;
    // mCr = cr;
    // mLength = length;
    // mValueIndex = valueIndex;
    // mRawValue = data;
}

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
