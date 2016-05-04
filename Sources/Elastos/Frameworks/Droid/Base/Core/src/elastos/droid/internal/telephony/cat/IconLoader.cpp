/*
 * Copyright (C) 2006 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.internal.telephony.cat;

using Elastos::Droid::Internal::Telephony::Uicc::IIccFileHandler;

using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Utility::IHashMap;

/**
 * Class for loading icons from the SIM card. Has two states: single, for loading
 * one icon. Multi, for loading icons list.
 *
 */
class IconLoader extends Handler {
    // members
    private Int32 mState = STATE_SINGLE_ICON;
    private ImageDescriptor mId = NULL;
    private Bitmap mCurrentIcon = NULL;
    private Int32 mRecordNumber;
    private IccFileHandler mSimFH = NULL;
    private Message mEndMsg = NULL;
    private Byte[] mIconData = NULL;
    // multi icons state members
    private Int32[] mRecordNumbers = NULL;
    private Int32 mCurrentRecordIndex = 0;
    private Bitmap[] mIcons = NULL;
    private HashMap<Integer, Bitmap> mIconsCache = NULL;

    private static IconLoader sLoader = NULL;
    private static HandlerThread sThread = NULL;

    // Loader state values.
    private static const Int32 STATE_SINGLE_ICON = 1;
    private static const Int32 STATE_MULTI_ICONS = 2;

    // Finished loading single record from a linear-fixed EF-IMG.
    private static const Int32 EVENT_READ_EF_IMG_RECOED_DONE  = 1;
    // Finished loading single icon from a Transparent DF-Graphics.
    private static const Int32 EVENT_READ_ICON_DONE           = 2;
    // Finished loading single colour icon lookup table.
    private static const Int32 EVENT_READ_CLUT_DONE           = 3;

    // Color lookup table offset inside the EF.
    private static const Int32 CLUT_LOCATION_OFFSET = 4;
    // CLUT entry size, {Red, Green, Black}
    private static const Int32 CLUT_ENTRY_SIZE = 3;


    private IconLoader(Looper looper , IccFileHandler fh) {
        Super(looper);
        mSimFH = fh;

        mIconsCache = new HashMap<Integer, Bitmap>(50);
    }

    static IconLoader GetInstance(Handler caller, IccFileHandler fh) {
        If (sLoader != NULL) {
            return sLoader;
        }
        If (fh != NULL) {
            sThread = new HandlerThread("Cat Icon Loader");
            sThread->Start();
            return new IconLoader(sThread->GetLooper(), fh);
        }
        return NULL;
    }

    void LoadIcons(Int32[] recordNumbers, Message msg) {
        If (recordNumbers == NULL || recordNumbers.length == 0 || msg == NULL) {
            return;
        }
        mEndMsg = msg;
        // initialize multi icons load variables.
        mIcons = new Bitmap[recordNumbers.length];
        mRecordNumbers = recordNumbers;
        mCurrentRecordIndex = 0;
        mState = STATE_MULTI_ICONS;
        StartLoadingIcon(recordNumbers[0]);
    }

    void LoadIcon(Int32 recordNumber, Message msg) {
        If (msg == NULL) {
            return;
        }
        mEndMsg = msg;
        mState = STATE_SINGLE_ICON;
        StartLoadingIcon(recordNumber);
    }

    private void StartLoadingIcon(Int32 recordNumber) {
        // Reset the load variables.
        mId = NULL;
        mIconData = NULL;
        mCurrentIcon = NULL;
        mRecordNumber = recordNumber;

        // make sure the icon was not already loaded and saved in the local cache.
        If (mIconsCache->ContainsKey(recordNumber)) {
            mCurrentIcon = mIconsCache->Get(recordNumber);
            PostIcon();
            return;
        }

        // start the first phase ==> loading Image Descriptor.
        ReadId();
    }

    //@Override
    CARAPI HandleMessage(Message msg) {
        AsyncResult ar;

        try {
            Switch (msg.what) {
            case EVENT_READ_EF_IMG_RECOED_DONE:
                ar = (AsyncResult) msg.obj;
                If (HandleImageDescriptor((Byte[]) ar.result)) {
                    ReadIconData();
                } else {
                    throw new Exception("Unable to parse image descriptor");
                }
                break;
            case EVENT_READ_ICON_DONE:
                ar = (AsyncResult) msg.obj;
                Byte[] rawData = ((Byte[]) ar.result);
                If (mId.mCodingScheme == ImageDescriptor.CODING_SCHEME_BASIC) {
                    mCurrentIcon = ParseToBnW(rawData, rawData.length);
                    mIconsCache->Put(mRecordNumber, mCurrentIcon);
                    PostIcon();
                } else If (mId.mCodingScheme == ImageDescriptor.CODING_SCHEME_COLOUR) {
                    mIconData = rawData;
                    ReadClut();
                } else {
                    // post NULL icon back to the caller.
                    PostIcon();
                }
                break;
            case EVENT_READ_CLUT_DONE:
                ar = (AsyncResult) msg.obj;
                Byte [] clut = ((Byte[]) ar.result);
                mCurrentIcon = ParseToRGB(mIconData, mIconData.length,
                        FALSE, clut);
                mIconsCache->Put(mRecordNumber, mCurrentIcon);
                PostIcon();
                break;
            }
        } Catch (Exception e) {
            CatLogger::D(this, "Icon load failed");
            // post NULL icon back to the caller.
            PostIcon();
        }
    }

    /**
     * Handles Image descriptor parsing and required processing. This is the
     * first step required to handle retrieving icons from the SIM.
     *
     * @param rawData Byte [] containing Image Instance descriptor as defined in
     * TS 51.011.
     */
    private Boolean HandleImageDescriptor(Byte[] rawData) {
        mId = ImageDescriptor->Parse(rawData, 1);
        If (mId == NULL) {
            return FALSE;
        }
        return TRUE;
    }

    // Start reading color lookup table from SIM card.
    private void ReadClut() {
        Int32 length = mIconData[3] * CLUT_ENTRY_SIZE;
        Message msg = ObtainMessage(EVENT_READ_CLUT_DONE);
        mSimFH->LoadEFImgTransparent(mId.mImageId,
                mIconData[CLUT_LOCATION_OFFSET],
                mIconData[CLUT_LOCATION_OFFSET + 1], length, msg);
    }

    // Start reading Image Descriptor from SIM card.
    private void ReadId() {
        If (mRecordNumber < 0) {
            mCurrentIcon = NULL;
            PostIcon();
            return;
        }
        Message msg = ObtainMessage(EVENT_READ_EF_IMG_RECOED_DONE);
        mSimFH->LoadEFImgLinearFixed(mRecordNumber, msg);
    }

    // Start reading icon bytes array from SIM card.
    private void ReadIconData() {
        Message msg = ObtainMessage(EVENT_READ_ICON_DONE);
        mSimFH->LoadEFImgTransparent(mId.mImageId, 0, 0, mId.mLength ,msg);
    }

    // When all is done pass icon back to caller.
    private void PostIcon() {
        If (mState == STATE_SINGLE_ICON) {
            mEndMsg.obj = mCurrentIcon;
            mEndMsg->SendToTarget();
        } else If (mState == STATE_MULTI_ICONS) {
            mIcons[mCurrentRecordIndex++] = mCurrentIcon;
            // If not all icons were loaded, start loading the next one.
            If (mCurrentRecordIndex < mRecordNumbers.length) {
                StartLoadingIcon(mRecordNumbers[mCurrentRecordIndex]);
            } else {
                mEndMsg.obj = mIcons;
                mEndMsg->SendToTarget();
            }
        }
    }

    /**
     * Convert a TS 131.102 image instance of code scheme '11' into Bitmap
     * @param data The raw data
     * @param length The length of image body
     * @return The bitmap
     */
    public static Bitmap ParseToBnW(Byte[] data, Int32 length){
        Int32 valueIndex = 0;
        Int32 width = data[valueIndex++] & 0xFF;
        Int32 height = data[valueIndex++] & 0xFF;
        Int32 numOfPixels = width*height;

        Int32[] pixels = new Int32[numOfPixels];

        Int32 pixelIndex = 0;
        Int32 bitIndex = 7;
        Byte currentByte = 0x00;
        While (pixelIndex < numOfPixels) {
            // reassign data and index for every Byte (8 bits).
            If (pixelIndex % 8 == 0) {
                currentByte = data[valueIndex++];
                bitIndex = 7;
            }
            pixels[pixelIndex++] = BitToBnW((currentByte >> bitIndex-- ) & 0x01);
        }

        If (pixelIndex != numOfPixels) {
            CatLogger::D("IconLoader", "parseToBnW; size error");
        }
        return Bitmap->CreateBitmap(pixels, width, height, Bitmap.Config.ARGB_8888);
    }

    /**
     * Decode one bit to a black and white color:
     * 0 is black
     * 1 is white
     * @param bit to decode
     * @return RGB color
     */
    private static Int32 BitToBnW(Int32 bit){
        If(bit == 1){
            return Color.WHITE;
        } else {
            return Color.BLACK;
        }
    }

    /**
     * a TS 131.102 image instance of code scheme '11' into color Bitmap
     *
     * @param data The raw data
     * @param length the length of image body
     * @param transparency with or without transparency
     * @param clut coulor lookup table
     * @return The color bitmap
     */
    public static Bitmap ParseToRGB(Byte[] data, Int32 length,
            Boolean transparency, Byte[] clut) {
        Int32 valueIndex = 0;
        Int32 width = data[valueIndex++] & 0xFF;
        Int32 height = data[valueIndex++] & 0xFF;
        Int32 bitsPerImg = data[valueIndex++] & 0xFF;
        Int32 numOfClutEntries = data[valueIndex++] & 0xFF;

        If (TRUE == transparency) {
            clut[numOfClutEntries - 1] = Color.TRANSPARENT;
        }

        Int32 numOfPixels = width * height;
        Int32[] pixels = new Int32[numOfPixels];

        valueIndex = 6;
        Int32 pixelIndex = 0;
        Int32 bitsStartOffset = 8 - bitsPerImg;
        Int32 bitIndex = bitsStartOffset;
        Byte currentByte = data[valueIndex++];
        Int32 mask = GetMask(bitsPerImg);
        Boolean bitsOverlaps = (8 % bitsPerImg == 0);
        While (pixelIndex < numOfPixels) {
            // reassign data and index for every Byte (8 bits).
            If (bitIndex < 0) {
                currentByte = data[valueIndex++];
                bitIndex = bitsOverlaps ? (bitsStartOffset) : (bitIndex * -1);
            }
            Int32 clutEntry = ((currentByte >> bitIndex) & mask);
            Int32 clutIndex = clutEntry * CLUT_ENTRY_SIZE;
            pixels[pixelIndex++] = Color->Rgb(clut[clutIndex],
                    clut[clutIndex + 1], clut[clutIndex + 2]);
            bitIndex -= bitsPerImg;
        }

        return Bitmap->CreateBitmap(pixels, width, height,
                Bitmap.Config.ARGB_8888);
    }

    /**
     * Calculate bit mask for a given number of bits. The mask should enable to
     * make a bitwise and to the given number of bits.
     * @param numOfBits number of bits to calculate mask for.
     * @return bit mask
     */
    private static Int32 GetMask(Int32 numOfBits) {
        Int32 mask = 0x00;

        Switch (numOfBits) {
        case 1:
            mask = 0x01;
            break;
        case 2:
            mask = 0x03;
            break;
        case 3:
            mask = 0x07;
            break;
        case 4:
            mask = 0x0F;
            break;
        case 5:
            mask = 0x1F;
            break;
        case 6:
            mask = 0x3F;
            break;
        case 7:
            mask = 0x7F;
            break;
        case 8:
            mask = 0xFF;
            break;
        }
        return mask;
    }
    CARAPI Dispose() {
        mSimFH = NULL;
        If (sThread != NULL) {
            sThread->Quit();
            sThread = NULL;
        }
        mIconsCache = NULL;
        sLoader = NULL;
    }
}
