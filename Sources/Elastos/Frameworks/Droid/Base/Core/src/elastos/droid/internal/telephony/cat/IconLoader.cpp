//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/internal/telephony/cat/IconLoader.h"
#include "elastos/droid/internal/telephony/cat/CatLog.h"
#include "elastos/droid/internal/telephony/cat/ImageDescriptor.h"
#include "elastos/droid/graphics/CBitmapHelper.h"
#include "elastos/droid/graphics/CColor.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/os/CHandlerThread.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::Graphics::IBitmapHelper;
using Elastos::Droid::Graphics::CBitmapHelper;
using Elastos::Droid::Graphics::CColor;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Core::CoreUtils;
using Elastos::Core::IThread;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IArrayList;

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

IconLoader::IconLoader(
    /* [in] */ ILooper* looper,
    /* [in] */ IIccFileHandler* fh)
{
    mSimFH = fh;

    CHashMap::New(50, (IHashMap**)&mIconsCache);
}

AutoPtr<IconLoader> IconLoader::GetInstance(
    /* [in] */ IHandler* caller,
    /* [in] */ IIccFileHandler* fh)
{
    if (sLoader != NULL) {
        return sLoader;
    }
    if (fh != NULL) {
        CHandlerThread::New(String("Cat Icon Loader"), (IHandlerThread**)&sThread);
        IThread::Probe(sThread)->Start();
        AutoPtr<ILooper> lp;
        sThread->GetLooper((ILooper**)&lp);
        AutoPtr<IconLoader> p = new IconLoader(lp, fh);
        return p;
    }
    return NULL;
}

ECode IconLoader::LoadIcons(
    /* [in] */ ArrayOf<Int32>* recordNumbers,
    /* [in] */ IMessage* msg)
{
    if (recordNumbers == NULL || recordNumbers->GetLength() == 0 || msg == NULL) {
        return NOERROR;
    }
    mEndMsg = msg;
    // initialize multi icons load variables.
    mIcons = ArrayOf<IBitmap*>::Alloc(recordNumbers->GetLength());
    mRecordNumbers = recordNumbers;
    mCurrentRecordIndex = 0;
    mState = STATE_MULTI_ICONS;
    StartLoadingIcon((*recordNumbers)[0]);
    return NOERROR;
}

ECode IconLoader::LoadIcon(
    /* [in] */ Int32 recordNumber,
    /* [in] */ IMessage* msg)
{
    if (msg == NULL) {
        return NOERROR;
    }
    mEndMsg = msg;
    mState = STATE_SINGLE_ICON;
    StartLoadingIcon(recordNumber);
    return NOERROR;
}

ECode IconLoader::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<AsyncResult> ar;

    // try {
    Int32 what = 0;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    switch (what) {
        case EVENT_READ_EF_IMG_RECOED_DONE: {
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            AutoPtr<IArrayList> pArr = IArrayList::Probe(ar->mResult);
            Int32 size = 0;
            pArr->GetSize(&size);
            AutoPtr<ArrayOf<Byte> > byteArr = ArrayOf<Byte>::Alloc(size);
            for (Int32 i = 0; i < size; ++i) {
                AutoPtr<IInterface> p;
                pArr->Get(i, (IInterface**)&p);
                IByte::Probe(p)->GetValue(&((*byteArr)[i]));
            }
            if (HandleImageDescriptor(byteArr)) {
                ReadIconData();
            }
            else {
                // throw new Exception("Unable to parse image descriptor");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            break;
        }
        case EVENT_READ_ICON_DONE: {
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            AutoPtr<IArrayList> pArr = IArrayList::Probe(ar->mResult);
            Int32 size = 0;
            pArr->GetSize(&size);
            AutoPtr<ArrayOf<Byte> > rawData = ArrayOf<Byte>::Alloc(size);
            for (Int32 i = 0; i < size; ++i) {
                AutoPtr<IInterface> p;
                pArr->Get(i, (IInterface**)&p);
                IByte::Probe(p)->GetValue(&((*rawData)[i]));
            }
            AutoPtr<ImageDescriptor> _mId = (ImageDescriptor*)mId.Get();
            if (_mId->mCodingScheme == ImageDescriptor::CODING_SCHEME_BASIC) {
                mCurrentIcon = ParseToBnW(rawData, rawData->GetLength());
                mIconsCache->Put(CoreUtils::Convert(mRecordNumber), mCurrentIcon);
                PostIcon();
            }
            else if (_mId->mCodingScheme == ImageDescriptor::CODING_SCHEME_COLOUR) {
                mIconData = rawData;
                ReadClut();
            }
            else {
                // post NULL icon back to the caller.
                PostIcon();
            }
            break;
        }
        case EVENT_READ_CLUT_DONE: {
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            AutoPtr<IArrayList> pArr = IArrayList::Probe(ar->mResult);
            Int32 size = 0;
            pArr->GetSize(&size);
            AutoPtr<ArrayOf<Byte> > clut = ArrayOf<Byte>::Alloc(size);
            for (Int32 i = 0; i < size; ++i) {
                AutoPtr<IInterface> p;
                pArr->Get(i, (IInterface**)&p);
                IByte::Probe(p)->GetValue(&((*clut)[i]));
            }
            mCurrentIcon = ParseToRGB(mIconData, mIconData->GetLength(),
                    FALSE, clut);
            mIconsCache->Put(CoreUtils::Convert(mRecordNumber), mCurrentIcon);
            PostIcon();
            break;
        }
    }
    // } catch (Exception e) {
    //     CatLog.d(this, "Icon load failed");
    //     // post NULL icon back to the caller.
    //     postIcon();
    // }
    return NOERROR;
}

AutoPtr<IBitmap> IconLoader::ParseToBnW(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 length)
{
    Int32 valueIndex = 0;
    Int32 width = (*data)[valueIndex++] & 0xFF;
    Int32 height = (*data)[valueIndex++] & 0xFF;
    Int32 numOfPixels = width * height;

    AutoPtr<ArrayOf<Int32> > pixels = ArrayOf<Int32>::Alloc(numOfPixels);

    Int32 pixelIndex = 0;
    Int32 bitIndex = 7;
    Byte currentByte = 0x00;
    while (pixelIndex < numOfPixels) {
        // reassign data and index for every byte (8 bits).
        if (pixelIndex % 8 == 0) {
            currentByte = (*data)[valueIndex++];
            bitIndex = 7;
        }
        (*pixels)[pixelIndex++] = BitToBnW((currentByte >> bitIndex-- ) & 0x01);
    }

    if (pixelIndex != numOfPixels) {
        CatLog::D(String("IconLoader"), String("parseToBnW; size error"));
    }
    AutoPtr<IBitmapHelper> hlp;
    CBitmapHelper::AcquireSingleton((IBitmapHelper**)&hlp);
    AutoPtr<IBitmap> bm;
    hlp->CreateBitmap(pixels, width, height, BitmapConfig_ARGB_8888, (IBitmap**)&bm);
    return bm;
}

AutoPtr<IBitmap> IconLoader::ParseToRGB(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 length,
    /* [in] */ Boolean transparency,
    /* [in] */ ArrayOf<Byte>* clut)
{
    Int32 valueIndex = 0;
    Int32 width = (*data)[valueIndex++] & 0xFF;
    Int32 height = (*data)[valueIndex++] & 0xFF;
    Int32 bitsPerImg = (*data)[valueIndex++] & 0xFF;
    Int32 numOfClutEntries = (*data)[valueIndex++] & 0xFF;

    if (TRUE == transparency) {
        (*clut)[numOfClutEntries - 1] = IColor::TRANSPARENT;
    }

    Int32 numOfPixels = width * height;
    AutoPtr<ArrayOf<Int32> > pixels = ArrayOf<Int32>::Alloc(numOfPixels);

    valueIndex = 6;
    Int32 pixelIndex = 0;
    Int32 bitsStartOffset = 8 - bitsPerImg;
    Int32 bitIndex = bitsStartOffset;
    Byte currentByte = (*data)[valueIndex++];
    Int32 mask = GetMask(bitsPerImg);
    Boolean bitsOverlaps = (8 % bitsPerImg == 0);
    while (pixelIndex < numOfPixels) {
        // reassign data and index for every byte (8 bits).
        if (bitIndex < 0) {
            currentByte = (*data)[valueIndex++];
            bitIndex = bitsOverlaps ? (bitsStartOffset) : (bitIndex * -1);
        }
        Int32 clutEntry = ((currentByte >> bitIndex) & mask);
        Int32 clutIndex = clutEntry * CLUT_ENTRY_SIZE;
        AutoPtr<IColor> cl;
        CColor::AcquireSingleton((IColor**)&cl);
        cl->Rgb((*clut)[clutIndex],
                (*clut)[clutIndex + 1], (*clut)[clutIndex + 2],
                &((*pixels)[pixelIndex++]));
        bitIndex -= bitsPerImg;
    }

    AutoPtr<IBitmapHelper> hlp;
    CBitmapHelper::AcquireSingleton((IBitmapHelper**)&hlp);
    AutoPtr<IBitmap> bm;
    hlp->CreateBitmap(pixels, width, height, BitmapConfig_ARGB_8888, (IBitmap**)&bm);
    return bm;
}

ECode IconLoader::Dispose()
{
    mSimFH = NULL;
    if (sThread != NULL) {
        Boolean b = FALSE;
        sThread->Quit(&b);
        sThread = NULL;
    }
    mIconsCache = NULL;
    sLoader = NULL;
    return NOERROR;
}

void IconLoader::StartLoadingIcon(
    /* [in] */ Int32 recordNumber)
{
    // Reset the load variables.
    mId = NULL;
    mIconData = NULL;
    mCurrentIcon = NULL;
    mRecordNumber = recordNumber;

    // make sure the icon was not already loaded and saved in the local cache.
    Boolean bContain = FALSE;
    mIconsCache->ContainsKey(CoreUtils::Convert(recordNumber), &bContain);
    if (bContain) {
        AutoPtr<IInterface> p;
        mIconsCache->Get(CoreUtils::Convert(recordNumber), (IInterface**)&p);
        mCurrentIcon = IBitmap::Probe(p);
        PostIcon();
        return;
    }

    // start the first phase ==> loading Image Descriptor.
    ReadId();
}

Boolean IconLoader::HandleImageDescriptor(
    /* [in] */ ArrayOf<Byte>* rawData)
{
    mId = ImageDescriptor::Parse(rawData, 1);
    if (mId == NULL) {
        return FALSE;
    }
    return TRUE;
}

void IconLoader::ReadClut()
{
    Int32 length = (*mIconData)[3] * CLUT_ENTRY_SIZE;
    AutoPtr<IMessage> msg;
    ObtainMessage(EVENT_READ_CLUT_DONE, (IMessage**)&msg);
    AutoPtr<ImageDescriptor> _mId = (ImageDescriptor*)mId.Get();
    mSimFH->LoadEFImgTransparent(_mId->mImageId,
            (*mIconData)[CLUT_LOCATION_OFFSET],
            (*mIconData)[CLUT_LOCATION_OFFSET + 1], length, msg);
}

void IconLoader::ReadId()
{
    if (mRecordNumber < 0) {
        mCurrentIcon = NULL;
        PostIcon();
        return;
    }
    AutoPtr<IMessage> msg;
    ObtainMessage(EVENT_READ_EF_IMG_RECOED_DONE, (IMessage**)&msg);
    mSimFH->LoadEFImgLinearFixed(mRecordNumber, msg);
}

void IconLoader::ReadIconData()
{
    AutoPtr<IMessage> msg;
    ObtainMessage(EVENT_READ_ICON_DONE, (IMessage**)&msg);
    AutoPtr<ImageDescriptor> _mId = (ImageDescriptor*)mId.Get();
    mSimFH->LoadEFImgTransparent(_mId->mImageId, 0, 0, _mId->mLength ,msg);
}

void IconLoader::PostIcon()
{
    if (mState == STATE_SINGLE_ICON) {
        mEndMsg->SetObj(mCurrentIcon);
        mEndMsg->SendToTarget();
    }
    else if (mState == STATE_MULTI_ICONS) {
        (*mIcons)[mCurrentRecordIndex++] = mCurrentIcon;
        // If not all icons were loaded, start loading the next one.
        if (mCurrentRecordIndex < mRecordNumbers->GetLength()) {
            StartLoadingIcon((*mRecordNumbers)[mCurrentRecordIndex]);
        }
        else {
            assert(0 && "TODO");
            // mEndMsg->SetObj(mIcons);
            mEndMsg->SendToTarget();
        }
    }
}

Int32 IconLoader::BitToBnW(
    /* [in] */ Int32 bit)
{
    if(bit == 1){
        return IColor::WHITE;
    }
    else {
        return IColor::BLACK;
    }
}

Int32 IconLoader::GetMask(
    /* [in] */ Int32 numOfBits)
{
    Int32 mask = 0x00;

    switch (numOfBits) {
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

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
