
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/internal/telephony/cat/IconLoader.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

//=====================================================================
//                              IconLoader
//=====================================================================
AutoPtr<IconLoader> IconLoader::sLoader = NULL;
AutoPtr<IHandlerThread> IconLoader::sThread = NULL;
const Int32 IconLoader::STATE_SINGLE_ICON;
const Int32 IconLoader::STATE_MULTI_ICONS;
const Int32 IconLoader::EVENT_READ_EF_IMG_RECOED_DONE;
const Int32 IconLoader::EVENT_READ_ICON_DONE;
const Int32 IconLoader::EVENT_READ_CLUT_DONE;
const Int32 IconLoader::CLUT_LOCATION_OFFSET;
const Int32 IconLoader::CLUT_ENTRY_SIZE;

AutoPtr<IconLoader> IconLoader::GetInstance(
    /* [in] */ IHandler* caller,
    /* [in] */ IIccFileHandler* fh)
{
    // ==================before translated======================
    // if (sLoader != null) {
    //     return sLoader;
    // }
    // if (fh != null) {
    //     sThread = new HandlerThread("Cat Icon Loader");
    //     sThread.start();
    //     return new IconLoader(sThread.getLooper(), fh);
    // }
    // return null;
    assert(0);
    AutoPtr<IconLoader> empty;
    return empty;
}

ECode IconLoader::LoadIcons(
    /* [in] */ ArrayOf<Int32>* recordNumbers,
    /* [in] */ IMessage* msg)
{
    // ==================before translated======================
    // if (recordNumbers == null || recordNumbers.length == 0 || msg == null) {
    //     return;
    // }
    // mEndMsg = msg;
    // // initialize multi icons load variables.
    // mIcons = new Bitmap[recordNumbers.length];
    // mRecordNumbers = recordNumbers;
    // mCurrentRecordIndex = 0;
    // mState = STATE_MULTI_ICONS;
    // startLoadingIcon(recordNumbers[0]);
    assert(0);
    return NOERROR;
}

ECode IconLoader::LoadIcon(
    /* [in] */ Int32 recordNumber,
    /* [in] */ IMessage* msg)
{
    // ==================before translated======================
    // if (msg == null) {
    //     return;
    // }
    // mEndMsg = msg;
    // mState = STATE_SINGLE_ICON;
    // startLoadingIcon(recordNumber);
    assert(0);
    return NOERROR;
}

ECode IconLoader::HandleMessage(
    /* [in] */ IMessage* msg)
{
    // ==================before translated======================
    // AsyncResult ar;
    //
    // try {
    //     switch (msg.what) {
    //     case EVENT_READ_EF_IMG_RECOED_DONE:
    //         ar = (AsyncResult) msg.obj;
    //         if (handleImageDescriptor((byte[]) ar.result)) {
    //             readIconData();
    //         } else {
    //             throw new Exception("Unable to parse image descriptor");
    //         }
    //         break;
    //     case EVENT_READ_ICON_DONE:
    //         ar = (AsyncResult) msg.obj;
    //         byte[] rawData = ((byte[]) ar.result);
    //         if (mId.mCodingScheme == ImageDescriptor.CODING_SCHEME_BASIC) {
    //             mCurrentIcon = parseToBnW(rawData, rawData.length);
    //             mIconsCache.put(mRecordNumber, mCurrentIcon);
    //             postIcon();
    //         } else if (mId.mCodingScheme == ImageDescriptor.CODING_SCHEME_COLOUR) {
    //             mIconData = rawData;
    //             readClut();
    //         } else {
    //             // post null icon back to the caller.
    //             postIcon();
    //         }
    //         break;
    //     case EVENT_READ_CLUT_DONE:
    //         ar = (AsyncResult) msg.obj;
    //         byte [] clut = ((byte[]) ar.result);
    //         mCurrentIcon = parseToRGB(mIconData, mIconData.length,
    //                 false, clut);
    //         mIconsCache.put(mRecordNumber, mCurrentIcon);
    //         postIcon();
    //         break;
    //     }
    // } catch (Exception e) {
    //     CatLog.d(this, "Icon load failed");
    //     // post null icon back to the caller.
    //     postIcon();
    // }
    assert(0);
    return NOERROR;
}

AutoPtr<IBitmap> IconLoader::ParseToBnW(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 length)
{
    // ==================before translated======================
    // int valueIndex = 0;
    // int width = data[valueIndex++] & 0xFF;
    // int height = data[valueIndex++] & 0xFF;
    // int numOfPixels = width*height;
    //
    // int[] pixels = new int[numOfPixels];
    //
    // int pixelIndex = 0;
    // int bitIndex = 7;
    // byte currentByte = 0x00;
    // while (pixelIndex < numOfPixels) {
    //     // reassign data and index for every byte (8 bits).
    //     if (pixelIndex % 8 == 0) {
    //         currentByte = data[valueIndex++];
    //         bitIndex = 7;
    //     }
    //     pixels[pixelIndex++] = bitToBnW((currentByte >> bitIndex-- ) & 0x01);
    // }
    //
    // if (pixelIndex != numOfPixels) {
    //     CatLog.d("IconLoader", "parseToBnW; size error");
    // }
    // return Bitmap.createBitmap(pixels, width, height, Bitmap.Config.ARGB_8888);
    assert(0);
    AutoPtr<IBitmap> empty;
    return empty;
}

AutoPtr<IBitmap> IconLoader::ParseToRGB(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 length,
    /* [in] */ Boolean transparency,
    /* [in] */ ArrayOf<Byte>* clut)
{
    // ==================before translated======================
    // int valueIndex = 0;
    // int width = data[valueIndex++] & 0xFF;
    // int height = data[valueIndex++] & 0xFF;
    // int bitsPerImg = data[valueIndex++] & 0xFF;
    // int numOfClutEntries = data[valueIndex++] & 0xFF;
    //
    // if (true == transparency) {
    //     clut[numOfClutEntries - 1] = Color.TRANSPARENT;
    // }
    //
    // int numOfPixels = width * height;
    // int[] pixels = new int[numOfPixels];
    //
    // valueIndex = 6;
    // int pixelIndex = 0;
    // int bitsStartOffset = 8 - bitsPerImg;
    // int bitIndex = bitsStartOffset;
    // byte currentByte = data[valueIndex++];
    // int mask = getMask(bitsPerImg);
    // boolean bitsOverlaps = (8 % bitsPerImg == 0);
    // while (pixelIndex < numOfPixels) {
    //     // reassign data and index for every byte (8 bits).
    //     if (bitIndex < 0) {
    //         currentByte = data[valueIndex++];
    //         bitIndex = bitsOverlaps ? (bitsStartOffset) : (bitIndex * -1);
    //     }
    //     int clutEntry = ((currentByte >> bitIndex) & mask);
    //     int clutIndex = clutEntry * CLUT_ENTRY_SIZE;
    //     pixels[pixelIndex++] = Color.rgb(clut[clutIndex],
    //             clut[clutIndex + 1], clut[clutIndex + 2]);
    //     bitIndex -= bitsPerImg;
    // }
    //
    // return Bitmap.createBitmap(pixels, width, height,
    //         Bitmap.Config.ARGB_8888);
    assert(0);
    AutoPtr<IBitmap> empty;
    return empty;
}

ECode IconLoader::Dispose()
{
    // ==================before translated======================
    // mSimFH = null;
    // if (sThread != null) {
    //     sThread.quit();
    //     sThread = null;
    // }
    // mIconsCache = null;
    // sLoader = null;
    assert(0);
    return NOERROR;
}

IconLoader::IconLoader(
    /* [in] */ ILooper* looper,
    /* [in] */ IIccFileHandler* fh)
{
    // ==================before translated======================
    // super(looper);
    // mSimFH = fh;
    //
    // mIconsCache = new HashMap<Integer, Bitmap>(50);
}

void IconLoader::StartLoadingIcon(
    /* [in] */ Int32 recordNumber)
{
    // ==================before translated======================
    // // Reset the load variables.
    // mId = null;
    // mIconData = null;
    // mCurrentIcon = null;
    // mRecordNumber = recordNumber;
    //
    // // make sure the icon was not already loaded and saved in the local cache.
    // if (mIconsCache.containsKey(recordNumber)) {
    //     mCurrentIcon = mIconsCache.get(recordNumber);
    //     postIcon();
    //     return;
    // }
    //
    // // start the first phase ==> loading Image Descriptor.
    // readId();
    assert(0);
}

Boolean IconLoader::HandleImageDescriptor(
    /* [in] */ ArrayOf<Byte>* rawData)
{
    // ==================before translated======================
    // mId = ImageDescriptor.parse(rawData, 1);
    // if (mId == null) {
    //     return false;
    // }
    // return true;
    assert(0);
    return FALSE;
}

void IconLoader::ReadClut()
{
    // ==================before translated======================
    // int length = mIconData[3] * CLUT_ENTRY_SIZE;
    // Message msg = obtainMessage(EVENT_READ_CLUT_DONE);
    // mSimFH.loadEFImgTransparent(mId.mImageId,
    //         mIconData[CLUT_LOCATION_OFFSET],
    //         mIconData[CLUT_LOCATION_OFFSET + 1], length, msg);
    assert(0);
}

void IconLoader::ReadId()
{
    // ==================before translated======================
    // if (mRecordNumber < 0) {
    //     mCurrentIcon = null;
    //     postIcon();
    //     return;
    // }
    // Message msg = obtainMessage(EVENT_READ_EF_IMG_RECOED_DONE);
    // mSimFH.loadEFImgLinearFixed(mRecordNumber, msg);
    assert(0);
}

void IconLoader::ReadIconData()
{
    // ==================before translated======================
    // Message msg = obtainMessage(EVENT_READ_ICON_DONE);
    // mSimFH.loadEFImgTransparent(mId.mImageId, 0, 0, mId.mLength ,msg);
    assert(0);
}

void IconLoader::PostIcon()
{
    // ==================before translated======================
    // if (mState == STATE_SINGLE_ICON) {
    //     mEndMsg.obj = mCurrentIcon;
    //     mEndMsg.sendToTarget();
    // } else if (mState == STATE_MULTI_ICONS) {
    //     mIcons[mCurrentRecordIndex++] = mCurrentIcon;
    //     // If not all icons were loaded, start loading the next one.
    //     if (mCurrentRecordIndex < mRecordNumbers.length) {
    //         startLoadingIcon(mRecordNumbers[mCurrentRecordIndex]);
    //     } else {
    //         mEndMsg.obj = mIcons;
    //         mEndMsg.sendToTarget();
    //     }
    // }
    assert(0);
}

Int32 IconLoader::BitToBnW(
    /* [in] */ Int32 bit)
{
    // ==================before translated======================
    // if(bit == 1){
    //     return Color.WHITE;
    // } else {
    //     return Color.BLACK;
    // }
    assert(0);
    return 0;
}

Int32 IconLoader::GetMask(
    /* [in] */ Int32 numOfBits)
{
    // ==================before translated======================
    // int mask = 0x00;
    //
    // switch (numOfBits) {
    // case 1:
    //     mask = 0x01;
    //     break;
    // case 2:
    //     mask = 0x03;
    //     break;
    // case 3:
    //     mask = 0x07;
    //     break;
    // case 4:
    //     mask = 0x0F;
    //     break;
    // case 5:
    //     mask = 0x1F;
    //     break;
    // case 6:
    //     mask = 0x3F;
    //     break;
    // case 7:
    //     mask = 0x7F;
    //     break;
    // case 8:
    //     mask = 0xFF;
    //     break;
    // }
    // return mask;
    assert(0);
    return 0;
}

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
