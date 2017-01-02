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

#include "elastos/droid/media/CMediaCodecInfo.h"
#include "elastos/droid/media/CMediaCodecInfoCodecCapabilities.h"
#include "elastos/droid/media/CMediaCodecInfoCodecProfileLevel.h"
#include "elastos/droid/media/CMediaCodecInfoVideoCapabilities.h"
#include "elastos/droid/media/Utils.h"
#include "elastos/droid/utility/CSize.h"
#include "elastos/droid/utility/CRange.h"
#include "elastos/droid/utility/CRational.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Utility::CRange;
using Elastos::Droid::Utility::CRational;
using Elastos::Droid::Utility::EIID_IRational;
using Elastos::Droid::Utility::CSize;
using Elastos::Droid::Utility::IPair;
using Elastos::Core::CDouble;
using Elastos::Core::CInteger32;
using Elastos::Core::CInteger64;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::IDouble;
using Elastos::Core::IInteger64;
using Elastos::Core::EIID_IDouble;
using Elastos::Core::EIID_IInteger32;
using Elastos::Core::EIID_IInteger64;
using Elastos::Core::Math;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Media {

const String CMediaCodecInfoVideoCapabilities::TAG("VideoCapabilities");

CAR_INTERFACE_IMPL(CMediaCodecInfoVideoCapabilities, Object, IMediaCodecInfoVideoCapabilities)

CAR_OBJECT_IMPL(CMediaCodecInfoVideoCapabilities)

CMediaCodecInfoVideoCapabilities::CMediaCodecInfoVideoCapabilities()
    : mBlockWidth(0)
    , mBlockHeight(0)
    , mWidthAlignment(0)
    , mHeightAlignment(0)
    , mSmallerDimensionUpperLimit(0)
{
}

CMediaCodecInfoVideoCapabilities::~CMediaCodecInfoVideoCapabilities()
{
}

ECode CMediaCodecInfoVideoCapabilities::constructor()
{
    return NOERROR;
}

ECode CMediaCodecInfoVideoCapabilities::GetBitrateRange(
    /* [out] */ IRange** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mBitrateRange;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaCodecInfoVideoCapabilities::GetSupportedWidths(
    /* [out] */ IRange** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mWidthRange;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaCodecInfoVideoCapabilities::GetSupportedHeights(
    /* [out] */ IRange** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHeightRange;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaCodecInfoVideoCapabilities::GetWidthAlignment(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mWidthAlignment;
    return NOERROR;
}

ECode CMediaCodecInfoVideoCapabilities::GetHeightAlignment(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHeightAlignment;
    return NOERROR;
}

ECode CMediaCodecInfoVideoCapabilities::GetSmallerDimensionUpperLimit(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSmallerDimensionUpperLimit;
    return NOERROR;
}

ECode CMediaCodecInfoVideoCapabilities::GetSupportedFrameRates(
    /* [out] */ IRange** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mFrameRateRange;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaCodecInfoVideoCapabilities::GetSupportedWidthsFor(
    /* [in] */ Int32 height,
    /* [out] */ IRange** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    // try {
    AutoPtr<IRange> range = mWidthRange;
    AutoPtr<IInteger32> iVal;
    CInteger32::New(height, (IInteger32**)&iVal);
    Boolean b;
    mHeightRange->Contains(IInterface::Probe(iVal), &b);
    if (!b || (height % mHeightAlignment) != 0) {
        // throw new IllegalArgumentException("unsupported height");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 heightInBlocks;
    heightInBlocks = Utils::DivUp(height, mBlockHeight);

    // constrain by block count and by block aspect ratio
    AutoPtr<IInterface> bcUpper;
    mBlockCountRange->GetUpper((IInterface**)&bcUpper);
    AutoPtr<IInterface> bcLower;
    mBlockCountRange->GetLower((IInterface**)&bcLower);
    AutoPtr<IInterface> barUpper;
    mBlockAspectRatioRange->GetUpper((IInterface**)&barUpper);
    AutoPtr<IInterface> barLower;
    mBlockAspectRatioRange->GetLower((IInterface**)&barLower);
    Double ud, ld;
    INumber::Probe(barLower)->DoubleValue(&ld);
    INumber::Probe(barUpper)->DoubleValue(&ud);

    Int32 bcLowerVal;
    IInteger32::Probe(bcLower)->GetValue(&bcLowerVal);
    Int32 val = Utils::DivUp(bcLowerVal, heightInBlocks);
    Int32 minWidthInBlocks = Elastos::Core::Math::Max(
            val, (Int32)Elastos::Core::Math::Ceil(ld * heightInBlocks));
    IInteger32::Probe(bcUpper)->GetValue(&val);
    Int32 maxWidthInBlocks = Elastos::Core::Math::Min(
            val / heightInBlocks,
            (Int32)(ud * heightInBlocks));

    AutoPtr<IInteger32> i1;
    CInteger32::New((minWidthInBlocks - 1) * mBlockWidth + mWidthAlignment,
            (IInteger32**)&i1);
    AutoPtr<IInteger32> i2;
    CInteger32::New(maxWidthInBlocks * mBlockWidth, (IInteger32**)&i2);
    range->Intersect(i1, i2, (IRange**)&range);

    // constrain by smaller dimension limit
    if (height > mSmallerDimensionUpperLimit) {
        i1 = NULL;
        CInteger32::New(1, (IInteger32**)&i1);
        i2 = NULL;
        CInteger32::New(mSmallerDimensionUpperLimit, (IInteger32**)&i2);
        range->Intersect(i1,i2, (IRange**)&range);
    }

    // constrain by aspect ratio
    AutoPtr<IInterface> arUpper;
    mAspectRatioRange->GetUpper((IInterface**)&arUpper);
    AutoPtr<IInterface> arLower;
    mAspectRatioRange->GetLower((IInterface**)&arLower);
    INumber::Probe(arLower)->DoubleValue(&ld);
    INumber::Probe(arUpper)->DoubleValue(&ud);
    i1 = NULL;
    CInteger32::New((Int32)Elastos::Core::Math::Ceil(ld * height), (IInteger32**)&i1);
    i2 = NULL;
    CInteger32::New((Int32)(ud * height), (IInteger32**)&i2);
    range->Intersect(i1, i2, (IRange**)&range);
    *result = range;
    REFCOUNT_ADD(*result)
    return NOERROR;
    // } catch (IllegalArgumentException e) {
    //     // should not be here
    //     Log.w(TAG, "could not get supported widths for " + height , e);
    //     throw new IllegalArgumentException("unsupported height");
    // }
}

ECode CMediaCodecInfoVideoCapabilities::GetSupportedHeightsFor(
    /* [in] */ Int32 width,
    /* [out] */ IRange** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    // try {
    AutoPtr<IRange> range = mHeightRange;
    AutoPtr<IInteger32> iVal;
    CInteger32::New(width, (IInteger32**)&iVal);
    Boolean b;
    mWidthRange->Contains(iVal, &b);
    if (!b || (width % mWidthAlignment) != 0) {
        // throw new IllegalArgumentException("unsupported width");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 widthInBlocks;
    widthInBlocks = Utils::DivUp(width, mBlockWidth);

    // constrain by block count and by block aspect ratio
    AutoPtr<IInterface> bcUpper;
    mBlockCountRange->GetUpper((IInterface**)&bcUpper);
    AutoPtr<IInterface> bcLower;
    mBlockCountRange->GetLower((IInterface**)&bcLower);
    AutoPtr<IInterface> barUpper;
    mBlockAspectRatioRange->GetUpper((IInterface**)&barUpper);
    AutoPtr<IInterface> barLower;
    mBlockAspectRatioRange->GetLower((IInterface**)&barLower);
    Double ud, ld;
    INumber::Probe(barLower)->DoubleValue(&ld);
    INumber::Probe(barUpper)->DoubleValue(&ud);

    Int32 bcLowerVal;
    IInteger32::Probe(bcLower)->GetValue(&bcLowerVal);
    Int32 val = Utils::DivUp(bcLowerVal, widthInBlocks);
    Int32 minHeightInBlocks = Elastos::Core::Math::Max(
            val, (Int32)Elastos::Core::Math::Ceil(widthInBlocks / ud));
    IInteger32::Probe(bcUpper)->GetValue(&val);
    Int32 maxHeightInBlocks = Elastos::Core::Math::Min(
            val / widthInBlocks,
            (Int32)(widthInBlocks / ld));

    AutoPtr<IInteger32> i1;
    CInteger32::New((minHeightInBlocks - 1) * mBlockHeight + mHeightAlignment,
            (IInteger32**)&i1);
    AutoPtr<IInteger32> i2;
    CInteger32::New(maxHeightInBlocks * mBlockHeight, (IInteger32**)&i2);
    range->Intersect(i1, i2, (IRange**)&range);

    // constrain by smaller dimension limit
    if (width > mSmallerDimensionUpperLimit) {
        i1 = NULL;
        CInteger32::New(1, (IInteger32**)&i1);
        i2 = NULL;
        CInteger32::New(mSmallerDimensionUpperLimit, (IInteger32**)&i2);
        range->Intersect(i1, i2, (IRange**)&range);
    }

    // constrain by aspect ratio
    AutoPtr<IInterface> arUpper;
    mAspectRatioRange->GetUpper((IInterface**)&arUpper);
    AutoPtr<IInterface> arLower;
    mAspectRatioRange->GetLower((IInterface**)&arLower);
    INumber::Probe(arLower)->DoubleValue(&ld);
    INumber::Probe(arUpper)->DoubleValue(&ud);

    i1 = NULL;
    CInteger32::New((Int32)Elastos::Core::Math::Ceil(width / ud), (IInteger32**)&i1);
    i2 = NULL;
    CInteger32::New((Int32)(width / ld), (IInteger32**)&i2);
    range->Intersect(i1, i2, (IRange**)&range);
    *result = range;
    REFCOUNT_ADD(*result)
    return NOERROR;
    // } catch (IllegalArgumentException e) {
    //     // should not be here
    //     Log.w(TAG, "could not get supported heights for " + width , e);
    //     throw new IllegalArgumentException("unsupported width");
    // }
}

ECode CMediaCodecInfoVideoCapabilities::GetSupportedFrameRatesFor(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [out] */ IRange** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    AutoPtr<IRange> range = mHeightRange;
    AutoPtr<IInteger32> i1;
    CInteger32::New(width, (IInteger32**)&i1);
    AutoPtr<IInteger32> i2;
    CInteger32::New(height, (IInteger32**)&i2);
    if (!Supports(i1, i2, NULL)) {
        // throw new IllegalArgumentException("unsupported size");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 blockCount;
    blockCount = Utils::DivUp(width, mBlockWidth) * Utils::DivUp(height, mBlockHeight);

    AutoPtr<IInterface> bpsUpper;
    mBlocksPerSecondRange->GetUpper((IInterface**)&bpsUpper);
    Int64 bpsUpperVal;
    IInteger64::Probe(bpsUpper)->GetValue(&bpsUpperVal);
    AutoPtr<IInterface> bpsLower;
    mBlocksPerSecondRange->GetLower((IInterface**)&bpsLower);
    Int64 bpsLowerVal;
    IInteger64::Probe(bpsLower)->GetValue(&bpsLowerVal);
    AutoPtr<IInterface> frUpper;
    mFrameRateRange->GetUpper((IInterface**)&frUpper);
    Int64 frUpperVal;
    IInteger64::Probe(frUpper)->GetValue(&frUpperVal);
    AutoPtr<IInterface> frLower;
    mFrameRateRange->GetLower((IInterface**)&frLower);
    Int64 frLowerVal;
    IInteger64::Probe(frLower)->GetValue(&frLowerVal);

    AutoPtr<IDouble> begin;
    CDouble::New(Elastos::Core::Math::Max(bpsLowerVal / (Double)blockCount,
            (Double)frLowerVal), (IDouble**)&begin);
    AutoPtr<IDouble> end;
    CDouble::New(Elastos::Core::Math::Min(bpsUpperVal / (Double)blockCount,
            (Double)frUpperVal), (IDouble**)&end);

    return CRange::Create(EIID_IDouble, begin, end, result);
}

ECode CMediaCodecInfoVideoCapabilities::AreSizeAndRateSupported(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Double frameRate,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IInteger32> i1;
    CInteger32::New(width, (IInteger32**)&i1);
    AutoPtr<IInteger32> i2;
    CInteger32::New(height, (IInteger32**)&i2);
    AutoPtr<IDouble> d;
    CDouble::New(frameRate, (IDouble**)&d);
    *result = Supports(i1, i2, d);
    return NOERROR;
}

ECode CMediaCodecInfoVideoCapabilities::IsSizeSupported(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IInteger32> i1;
    CInteger32::New(width, (IInteger32**)&i1);
    AutoPtr<IInteger32> i2;
    CInteger32::New(height, (IInteger32**)&i2);
    *result = Supports(i1, i2, NULL);
    return NOERROR;
}

ECode CMediaCodecInfoVideoCapabilities::SupportsFormat(
    /* [in] */ IMediaFormat* format,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IMap> map;
    format->GetMap((IMap**)&map);
    AutoPtr<ICharSequence> cs;
    CString::New(IMediaFormat::KEY_WIDTH, (ICharSequence**)&cs);
    AutoPtr<IInterface> obj;
    map->Get(cs, (IInterface**)&obj);
    AutoPtr<IInteger32> width = IInteger32::Probe(obj);

    cs = NULL;
    CString::New(IMediaFormat::KEY_HEIGHT, (ICharSequence**)&cs);
    obj = NULL;
    map->Get(cs, (IInterface**)&obj);
    AutoPtr<IInteger32> height = IInteger32::Probe(obj);

    cs = NULL;
    CString::New(IMediaFormat::KEY_FRAME_RATE, (ICharSequence**)&cs);
    obj = NULL;
    map->Get(cs, (IInterface**)&obj);
    AutoPtr<IDouble> rate = IDouble::Probe(obj);

    // we ignore color-format for now as it is not reliably reported by codec

    *result = Supports(width, height, rate);
    return NOERROR;
}

ECode CMediaCodecInfoVideoCapabilities::Init(
    /* [in] */ IMediaFormat* info,
    /* [in] */ IMediaCodecInfoCodecCapabilities* parent)
{
    mParent = parent;
    InitWithPlatformLimits();
    ApplyLevelLimits();
    ParseFromInfo(info);
    UpdateLimits();
    return NOERROR;
}

ECode CMediaCodecInfoVideoCapabilities::GetBlockSize(
    /* [out] */ ISize** result)
{
    VALIDATE_NOT_NULL(result)
    return CSize::New(mBlockWidth, mBlockHeight, result);
}

ECode CMediaCodecInfoVideoCapabilities::GetBlockCountRange(
    /* [out] */ IRange** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mBlockCountRange;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaCodecInfoVideoCapabilities::GetBlocksPerSecondRange(
    /* [out] */ IRange** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mBlocksPerSecondRange;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaCodecInfoVideoCapabilities::GetAspectRatioRange(
    /* [in] */ Boolean blocks,
    /* [out] */ IRange** result)
{
    VALIDATE_NOT_NULL(result)
    *result = blocks ? mBlockAspectRatioRange : mAspectRatioRange;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaCodecInfoVideoCapabilities::Create(
    /* [in] */ IMediaFormat* info,
    /* [in] */ IMediaCodecInfoCodecCapabilities* parent,
    /* [out] */ IMediaCodecInfoVideoCapabilities** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IMediaCodecInfoVideoCapabilities> caps;
    CMediaCodecInfoVideoCapabilities::New((IMediaCodecInfoVideoCapabilities**)&caps);
    caps->Init(info, parent);
    *result = caps;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

Boolean CMediaCodecInfoVideoCapabilities::Supports(
    /* [in] */ IInteger32* width,
    /* [in] */ IInteger32* height,
    /* [in] */ IDouble* rate)
{
    Boolean ok = TRUE;

    Int32 widthVal;
    width->GetValue(&widthVal);
    Int32 heightVal;
    width->GetValue(&heightVal);
    Boolean b;
    if (ok && width != NULL) {
        ok = (mWidthRange->Contains(width, &b), b)
                && (widthVal % mWidthAlignment == 0);
    }
    if (ok && height != NULL) {
        ok = (mHeightRange->Contains(height, &b), b)
                && (heightVal % mHeightAlignment == 0);
    }
    if (ok && rate != NULL) {
        AutoPtr<IRange> r;
        Double val;
        rate->GetValue(&val);
        Utils::IntRangeFor(val, (IRange**)&r);
        mFrameRateRange->Contains(r, &ok);
    }
    if (ok && height != NULL && width != NULL) {
        ok = Elastos::Core::Math::Min(heightVal, widthVal) <= mSmallerDimensionUpperLimit;

        Int32 widthInBlocks;
        widthInBlocks = Utils::DivUp(widthVal, mBlockWidth);
        Int32 heightInBlocks;
        heightInBlocks = Utils::DivUp(heightVal, mBlockHeight);
        Int32 blockCount = widthInBlocks * heightInBlocks;
        AutoPtr<IInteger32> iBlockCount;
        CInteger32::New(blockCount, (IInteger32**)&iBlockCount);
        Boolean b1, b2, b3;
        mBlockCountRange->Contains(iBlockCount, &b1);
        AutoPtr<IRational> rational;
        CRational::New(widthInBlocks, heightInBlocks, (IRational**)&rational);
        mBlockAspectRatioRange->Contains(rational, &b2);
        rational = NULL;
        CRational::New(widthVal, heightVal, (IRational**)&rational);
        mAspectRatioRange->Contains(rational, &b3);
        ok = ok && b1 && b2 && b3;

        if (ok && rate != NULL) {
            Double blocksPerSec;
            rate->GetValue(&blocksPerSec);
            blocksPerSec = blockCount * blocksPerSec;
            AutoPtr<IRange> r;
            Utils::Int64RangeFor(blocksPerSec, (IRange**)&r);
            mBlocksPerSecondRange->Contains(r, &ok);
        }
    }
    return ok;
}

void CMediaCodecInfoVideoCapabilities::InitWithPlatformLimits()
{
    AutoPtr<IInteger32> begin;
    CInteger32::New(0, (IInteger32**)&begin);
    AutoPtr<IInteger32> end;
    CInteger32::New(Elastos::Core::Math::INT32_MAX_VALUE, (IInteger32**)&end);
    CRange::Create(EIID_IInteger32, begin, end, (IRange**)&mBitrateRange);

    mWidthRange  = CMediaCodecInfo::SIZE_RANGE;
    mHeightRange = CMediaCodecInfo::SIZE_RANGE;
    mFrameRateRange = CMediaCodecInfo::FRAME_RATE_RANGE;

    mHorizontalBlockRange = CMediaCodecInfo::SIZE_RANGE;
    mVerticalBlockRange   = CMediaCodecInfo::SIZE_RANGE;

    // full positive ranges are supported as these get calculated
    mBlockCountRange      = CMediaCodecInfo::POSITIVE_INTEGERS;
    mBlocksPerSecondRange = CMediaCodecInfo::POSITIVE_LONGS;

    mBlockAspectRatioRange = CMediaCodecInfo::POSITIVE_RATIONALS;
    mAspectRatioRange      = CMediaCodecInfo::POSITIVE_RATIONALS;

    // YUV 4:2:0 requires 2:2 alignment
    mWidthAlignment = 2;
    mHeightAlignment = 2;
    mBlockWidth = 2;
    mBlockHeight = 2;
    AutoPtr<IInterface> upper;
    CMediaCodecInfo::SIZE_RANGE->GetUpper((IInterface**)&upper);
    IInteger32::Probe(upper)->GetValue(&mSmallerDimensionUpperLimit);
}

void CMediaCodecInfoVideoCapabilities::ParseFromInfo(
    /* [in] */ IMediaFormat* info)
{
    AutoPtr<IMap> map;
    info->GetMap((IMap**)&map);
    AutoPtr<ISize> blockSize;
    CSize::New(mBlockWidth, mBlockHeight, (ISize**)&blockSize);
    AutoPtr<ISize> alignment;
    CSize::New(mWidthAlignment, mHeightAlignment, (ISize**)&alignment);

    AutoPtr<ICharSequence> cs;
    CString::New(String("block-size"), (ICharSequence**)&cs);
    AutoPtr<IInterface> obj;
    map->Get(cs, (IInterface**)&obj);
    blockSize = Utils::ParseSize(obj, blockSize);

    cs = NULL;
    CString::New(String("alignment"), (ICharSequence**)&cs);
    obj = NULL;
    map->Get(cs, (IInterface**)&obj);
    alignment = Utils::ParseSize(obj, alignment);

    cs = NULL;
    CString::New(String("block-count-range"), (ICharSequence**)&cs);
    obj = NULL;
    map->Get(cs, (IInterface**)&obj);
    AutoPtr<IRange> counts;
    counts = Utils::ParseIntRange(obj, (IRange*)NULL);

    cs = NULL;
    CString::New(String("blocks-per-second-range"), (ICharSequence**)&cs);
    obj = NULL;
    map->Get(cs, (IInterface**)&obj);
    AutoPtr<IRange> blockRates;
    blockRates = Utils::ParseInt64Range(obj, (IRange*)NULL);

    AutoPtr<IRange> widths;
    AutoPtr<IRange> heights;
    {
        cs = NULL;
        CString::New(String("size-range"), (ICharSequence**)&cs);
        obj = NULL;
        map->Get(cs, (IInterface**)&obj);
        AutoPtr<IPair> sizeRange;
        sizeRange = Utils::ParseSizeRange(obj);
        if (sizeRange != NULL) {
            // try {
            AutoPtr<IInterface> first;
            sizeRange->GetFirst((IInterface**)&first);
            Int32 firstWidth, firstHeight;
            ISize::Probe(first)->GetWidth(&firstWidth);
            ISize::Probe(first)->GetHeight(&firstHeight);
            AutoPtr<IInterface> second;
            sizeRange->GetSecond((IInterface**)&second);
            Int32 secondWidth, secondHeight;
            ISize::Probe(second)->GetWidth(&secondWidth);
            ISize::Probe(second)->GetHeight(&secondHeight);

            AutoPtr<IInteger32> begin;
            CInteger32::New(firstWidth, (IInteger32**)&begin);
            AutoPtr<IInteger32> end;
            CInteger32::New(secondWidth, (IInteger32**)&end);
            CRange::Create(EIID_IInteger32, begin, end, (IRange**)&widths);

            begin = NULL;
            CInteger32::New(firstHeight, (IInteger32**)&begin);
            end = NULL;
            CInteger32::New(secondHeight, (IInteger32**)&end);
            CRange::Create(EIID_IInteger32, begin, end, (IRange**)&heights);
            // } catch (IllegalArgumentException e) {
            //     Log.w(TAG, "could not parse size range '" + o + "'");
            //     widths = NULL;
            //     heights = NULL;
            // }
        }
    }
    // for now this just means using the smaller max size as 2nd
    // upper limit.
    // for now we are keeping the profile specific "width/height
    // in macroblocks" limits.
    cs = NULL;
    CString::New(String("feature-can-swap-width-height"), (ICharSequence**)&cs);
    obj = NULL;
    map->Get(cs, (IInterface**)&obj);
    AutoPtr<IInteger32> val;
    CInteger32::New(1, (IInteger32**)&val);

    Boolean b;
    IObject::Probe(val)->Equals(obj, &b);
    if (b) {
        if (widths != NULL) {
            AutoPtr<IInterface> upper;
            widths->GetUpper((IInterface**)&upper);
            AutoPtr<IInterface> lower;
            heights->GetLower((IInterface**)&lower);
            Int32 upperVal, lowerVal;
            IInteger32::Probe(upper)->GetValue(&upperVal);
            IInteger32::Probe(lower)->GetValue(&lowerVal);
            mSmallerDimensionUpperLimit =
                Elastos::Core::Math::Min(upperVal, lowerVal);
            widths->Extend(heights, (IRange**)&heights);
            widths = heights;
        }
        else {
            Logger::W(TAG, "feature can-swap-width-height is best used with size-range");
            AutoPtr<IInterface> upper1;
            mWidthRange->GetUpper((IInterface**)&upper1);
            AutoPtr<IInterface> upper2;
            mHeightRange->GetLower((IInterface**)&upper2);
            Int32 upper1Val, upper2Val;
            IInteger32::Probe(upper1)->GetValue(&upper1Val);
            IInteger32::Probe(upper2)->GetValue(&upper2Val);
            mSmallerDimensionUpperLimit =
                Elastos::Core::Math::Min(upper1Val, upper2Val);
            mWidthRange->Extend(mHeightRange, (IRange**)&mHeightRange);
            mWidthRange = mHeightRange;
        }
    }

    cs = NULL;
    CString::New(String("block-aspect-ratio-range"), (ICharSequence**)&cs);
    obj = NULL;
    map->Get(cs, (IInterface**)&obj);
    AutoPtr<IRange> ratios;
    ratios = Utils::ParseRationalRange(obj, (IRange*)NULL);

    cs = NULL;
    CString::New(String("pixel-aspect-ratio-range"), (ICharSequence**)&cs);
    obj = NULL;
    map->Get(cs, (IInterface**)&obj);
    AutoPtr<IRange> blockRatios;
    blockRatios = Utils::ParseRationalRange(obj, (IRange*)NULL);

    cs = NULL;
    CString::New(String("frame-rate-range"), (ICharSequence**)&cs);
    obj = NULL;
    map->Get(cs, (IInterface**)&obj);
    AutoPtr<IRange> frameRates;
    frameRates = Utils::ParseIntRange(obj, (IRange*)NULL);

    if (frameRates != NULL) {
        // try {
        frameRates->Intersect(CMediaCodecInfo::FRAME_RATE_RANGE, (IRange**)&frameRates);
        // } catch (IllegalArgumentException e) {
        //     Log.w(TAG, "frame rate range (" + frameRates
        //             + ") is out of limits: " + CMediaCodecInfo::FRAME_RATE_RANGE);
        //     frameRates = NULL;
        // }
    }

    Int32 bsWidth, bsHeight;
    blockSize->GetWidth(&bsWidth);
    blockSize->GetHeight(&bsHeight);
    Int32 aWidth, aHeight;
    alignment->GetWidth(&aWidth);
    alignment->GetHeight(&aHeight);
    Int32 result;
    CMediaCodecInfo::CheckPowerOfTwo(
            bsWidth, String("block-size width must be power of two"), &result);
    CMediaCodecInfo::CheckPowerOfTwo(
            bsHeight, String("block-size height must be power of two"), &result);

    CMediaCodecInfo::CheckPowerOfTwo(
            aWidth, String("alignment width must be power of two"), &result);
    CMediaCodecInfo::CheckPowerOfTwo(
            aHeight, String("alignment height must be power of two"), &result);

    // update block-size and alignment
    ApplyMacroBlockLimits(
            Elastos::Core::Math::INT32_MAX_VALUE,
            Elastos::Core::Math::INT32_MAX_VALUE,
            Elastos::Core::Math::INT32_MAX_VALUE,
            Elastos::Core::Math::INT64_MAX_VALUE,
            bsWidth, bsHeight, aWidth, aHeight);

    if ((((CMediaCodecInfoCodecCapabilities*)mParent.Get())->mError &
            CMediaCodecInfo::ERROR_UNSUPPORTED) != 0) {
        // codec supports profiles that we don't know.
        // Use supplied values clipped to platform limits
        if (widths != NULL) {
            CMediaCodecInfo::SIZE_RANGE->Intersect(widths,
                    (IRange**)&mWidthRange);
        }
        if (heights != NULL) {
            CMediaCodecInfo::SIZE_RANGE->Intersect(heights,
                    (IRange**)&mHeightRange);
        }
        if (counts != NULL) {
            AutoPtr<IRange> range;
            range = Utils::FactorRange(counts,
                    mBlockWidth * mBlockHeight / bsWidth / bsHeight);
            CMediaCodecInfo::POSITIVE_INTEGERS->Intersect(
                    range, (IRange**)&mBlockCountRange);
        }
        if (blockRates != NULL) {
            AutoPtr<IRange> range;
            range = Utils::FactorRange(blockRates,
                    mBlockWidth * mBlockHeight / bsWidth / bsHeight);
            CMediaCodecInfo::POSITIVE_LONGS->Intersect(
                    range, (IRange**)&mBlocksPerSecondRange);
        }
        if (blockRatios != NULL) {
            AutoPtr<IRange> range;
            range = Utils::ScaleRange(blockRatios,
                    mBlockHeight / bsHeight, mBlockWidth / bsWidth);
            CMediaCodecInfo::POSITIVE_RATIONALS->Intersect(
                    range,(IRange**)&mBlockAspectRatioRange);
        }
        if (ratios != NULL) {
            CMediaCodecInfo::POSITIVE_RATIONALS->Intersect(
                    ratios, (IRange**)&mAspectRatioRange);
        }
        if (frameRates != NULL) {
            CMediaCodecInfo::FRAME_RATE_RANGE->Intersect(
                    frameRates, (IRange**)&mFrameRateRange);
        }
    }
    else {
        // no unsupported profile/levels, so restrict values to known limits
        if (widths != NULL) {
            mWidthRange->Intersect(widths, (IRange**)&mWidthRange);
        }
        if (heights != NULL) {
            mHeightRange->Intersect(heights, (IRange**)&mHeightRange);
        }
        if (counts != NULL) {
            AutoPtr<IRange> range;
            range = Utils::FactorRange(counts,
                    mBlockWidth * mBlockHeight / bsWidth / bsHeight);
            mBlockCountRange->Intersect(range, (IRange**)&mBlockCountRange);
        }
        if (blockRates != NULL) {
            AutoPtr<IRange> range;
            range = Utils::FactorRange(blockRates,
                    mBlockWidth * mBlockHeight / bsWidth / bsHeight);
            mBlocksPerSecondRange->Intersect(range , (IRange**)&mBlocksPerSecondRange);
        }
        if (blockRatios != NULL) {
            AutoPtr<IRange> range;
            range = Utils::ScaleRange(blockRatios,
                    mBlockHeight / bsHeight, mBlockWidth / bsWidth);
            mBlockAspectRatioRange->Intersect(range, (IRange**)&mBlockAspectRatioRange);
        }
        if (ratios != NULL) {
            mAspectRatioRange->Intersect(ratios, (IRange**)&mAspectRatioRange);
        }
        if (frameRates != NULL) {
            mFrameRateRange->Intersect(frameRates, (IRange**)&mFrameRateRange);
        }
    }
    UpdateLimits();
}

void CMediaCodecInfoVideoCapabilities::ApplyBlockLimits(
    /* [in] */ Int32 blockWidth,
    /* [in] */ Int32 blockHeight,
    /* [in] */ IRange* counts,
    /* [in] */ IRange* rates,
    /* [in] */ IRange* ratios)
{
    Int32 result;
    CMediaCodecInfo::CheckPowerOfTwo(blockWidth,
            String("blockWidth must be a power of two"), &result);
    CMediaCodecInfo::CheckPowerOfTwo(blockHeight,
            String("blockHeight must be a power of two"), &result);

    Int32 newBlockWidth = Elastos::Core::Math::Max(blockWidth, mBlockWidth);
    Int32 newBlockHeight = Elastos::Core::Math::Max(blockHeight, mBlockHeight);

    // factor will always be a power-of-2
    Int32 factor =
        newBlockWidth * newBlockHeight / mBlockWidth / mBlockHeight;
    if (factor != 1) {
        mBlockCountRange = Utils::FactorRange(mBlockCountRange, factor);
        mBlocksPerSecondRange = Utils::FactorRange(
                mBlocksPerSecondRange, factor);
        mBlockAspectRatioRange = Utils::ScaleRange(
                mBlockAspectRatioRange,
                newBlockHeight / mBlockHeight,
                newBlockWidth / mBlockWidth);
        mHorizontalBlockRange = Utils::FactorRange(
                mHorizontalBlockRange, newBlockWidth / mBlockWidth);
        mVerticalBlockRange = Utils::FactorRange(
                mVerticalBlockRange, newBlockHeight / mBlockHeight);
    }
    factor = newBlockWidth * newBlockHeight / blockWidth / blockHeight;
    if (factor != 1) {
        counts = Utils::FactorRange(counts, factor);
        rates = Utils::FactorRange(rates, factor);
        ratios = Utils::ScaleRange(
                ratios, newBlockHeight / blockHeight,
                newBlockWidth / blockWidth);
    }
    mBlockCountRange->Intersect(counts, (IRange**)&mBlockCountRange);
    mBlocksPerSecondRange->Intersect(rates, (IRange**)&mBlocksPerSecondRange);
    mBlockAspectRatioRange->Intersect(ratios, (IRange**)&mBlockAspectRatioRange);
    mBlockWidth = newBlockWidth;
    mBlockHeight = newBlockHeight;
}

void CMediaCodecInfoVideoCapabilities::ApplyAlignment(
    /* [in] */ Int32 widthAlignment,
    /* [in] */ Int32 heightAlignment)
{
    Int32 result;
    CMediaCodecInfo::CheckPowerOfTwo(widthAlignment,
        String("widthAlignment must be a power of two"), &result);
    CMediaCodecInfo::CheckPowerOfTwo(heightAlignment,
        String("heightAlignment must be a power of two"), &result);

    if (widthAlignment > mBlockWidth || heightAlignment > mBlockHeight) {
        // maintain assumption that 0 < alignment <= block-size
        ApplyBlockLimits(
                Elastos::Core::Math::Max(widthAlignment, mBlockWidth),
                Elastos::Core::Math::Max(heightAlignment, mBlockHeight),
                CMediaCodecInfo::POSITIVE_INTEGERS,
                CMediaCodecInfo::POSITIVE_LONGS,
                CMediaCodecInfo::POSITIVE_RATIONALS);
    }

    mWidthAlignment = Elastos::Core::Math::Max(widthAlignment, mWidthAlignment);
    mHeightAlignment = Elastos::Core::Math::Max(heightAlignment, mHeightAlignment);

    mWidthRange = Utils::AlignRange(mWidthRange, mWidthAlignment);
    mHeightRange = Utils::AlignRange(mHeightRange, mHeightAlignment);
}

void CMediaCodecInfoVideoCapabilities::UpdateLimits()
{
    // pixels -> blocks <- counts
    AutoPtr<IRange> range;
    range = Utils::FactorRange(mWidthRange, mBlockWidth);
    mHorizontalBlockRange->Intersect(range, (IRange**)&mHorizontalBlockRange);

    AutoPtr<IInterface> bcUpper;
    mBlockCountRange->GetUpper((IInterface**)&bcUpper);
    Int32 bcUpperVal;
    IInteger32::Probe(bcUpper)->GetValue(&bcUpperVal);

    AutoPtr<IInterface> bcLower;
    mBlockCountRange->GetLower((IInterface**)&bcLower);
    Int32 bcLowerVal;
    IInteger32::Probe(bcLower)->GetValue(&bcLowerVal);

    AutoPtr<IInterface> vbUpper;
    mVerticalBlockRange->GetUpper((IInterface**)&vbUpper);
    Int32 vbUpperVal;
    IInteger32::Probe(vbUpper)->GetValue(&vbUpperVal);

    AutoPtr<IInterface> vbLower;
    mVerticalBlockRange->GetLower((IInterface**)&vbLower);
    Int32 vbLowerVal;
    IInteger32::Probe(vbLower)->GetValue(&vbLowerVal);

    AutoPtr<IInteger32> begin;
    CInteger32::New(bcLowerVal / vbUpperVal, (IInteger32**)&begin);
    AutoPtr<IInteger32> end;
    CInteger32::New(bcUpperVal / vbLowerVal, (IInteger32**)&end);
    range = NULL;
    CRange::Create(EIID_IInteger32, begin, end, (IRange**)&range);
    mHorizontalBlockRange->Intersect(range, (IRange**)&mHorizontalBlockRange);

    range = NULL;
    range = Utils::FactorRange(mHeightRange, mBlockHeight);
    mVerticalBlockRange->Intersect(range, (IRange**)&mVerticalBlockRange);

    AutoPtr<IInterface> hbUpper;
    mHorizontalBlockRange->GetUpper((IInterface**)&hbUpper);
    Int32 hbUpperVal;
    IInteger32::Probe(hbUpper)->GetValue(&hbUpperVal);
    AutoPtr<IInterface> hbLower;
    mHorizontalBlockRange->GetLower((IInterface**)&hbLower);
    Int32 hbLowerVal;
    IInteger32::Probe(hbLower)->GetValue(&hbLowerVal);

    begin = NULL;
    CInteger32::New(bcLowerVal / hbUpperVal, (IInteger32**)&begin);
    end = NULL;
    CInteger32::New(bcUpperVal / hbLowerVal, (IInteger32**)&end);
    range = NULL;
    CRange::Create(EIID_IInteger32, begin, end, (IRange**)&range);
    mVerticalBlockRange->Intersect(range, (IRange**)&mVerticalBlockRange);

    begin = NULL;
    CInteger32::New(hbLowerVal * vbLowerVal, (IInteger32**)&begin);
    end = NULL;
    CInteger32::New(hbUpperVal * vbUpperVal, (IInteger32**)&end);
    range = NULL;
    CRange::Create(EIID_IInteger32, begin, end, (IRange**)&range);
    mBlockCountRange->Intersect(range, (IRange**)&mBlockCountRange);

    AutoPtr<IRational> lower;
    CRational::New(hbLowerVal, vbUpperVal, (IRational**)&lower);
    AutoPtr<IRational> upper;
    CRational::New(hbUpperVal, vbLowerVal, (IRational**)&upper);
    mBlockAspectRatioRange->Intersect(lower, upper, (IRange**)&mBlockAspectRatioRange);

    // blocks -> pixels
    AutoPtr<IInteger32> i1;
    CInteger32::New((hbLowerVal - 1) * mBlockWidth + mWidthAlignment,
            (IInteger32**)&i1);
    AutoPtr<IInteger32> i2;
    CInteger32::New(hbUpperVal * mBlockWidth, (IInteger32**)&i2);
    mWidthRange->Intersect(i1, i2, (IRange**)&mWidthRange);

    i1 = NULL;
    CInteger32::New((vbLowerVal - 1) * mBlockHeight + mHeightAlignment,
            (IInteger32**)&i1);
    i2 = NULL;
    CInteger32::New(vbUpperVal * mBlockHeight, (IInteger32**)&i2);
    mHeightRange->Intersect(i1, i2, (IRange**)&mHeightRange);

    AutoPtr<IInterface> wUpper;
    mWidthRange->GetUpper((IInterface**)&wUpper);
    AutoPtr<IInterface> wLower;
    mWidthRange->GetLower((IInterface**)&wLower);
    AutoPtr<IInterface> hUpper;
    mHeightRange->GetUpper((IInterface**)&hUpper);
    AutoPtr<IInterface> hLower;
    mHeightRange->GetLower((IInterface**)&hLower);

    Int32 val1, val2;
    IInteger32::Probe(wLower)->GetValue(&val1);
    IInteger32::Probe(hUpper)->GetValue(&val2);
    lower = NULL;
    CRational::New(val1, val2, (IRational**)&lower);
    IInteger32::Probe(wUpper)->GetValue(&val1);
    IInteger32::Probe(hLower)->GetValue(&val2);
    upper = NULL;
    CRational::New(val1, val2, (IRational**)&upper);
    mAspectRatioRange->Intersect(lower, upper, (IRange**)&mAspectRatioRange);

    IInteger32::Probe(wUpper)->GetValue(&val1);
    IInteger32::Probe(hUpper)->GetValue(&val2);
    mSmallerDimensionUpperLimit = Elastos::Core::Math::Min(
            mSmallerDimensionUpperLimit,
            Elastos::Core::Math::Min(val1, val2));

    // blocks -> rate
    AutoPtr<IInterface> frUpper;
    mFrameRateRange->GetUpper((IInterface**)&frUpper);
    AutoPtr<IInterface> frLower;
    mFrameRateRange->GetLower((IInterface**)&frLower);
    AutoPtr<IInterface> bpsUpper;
    mBlocksPerSecondRange->GetUpper((IInterface**)&bpsUpper);
    AutoPtr<IInterface> bpsLower;
    mBlocksPerSecondRange->GetLower((IInterface**)&bpsLower);

    Int64 frLowerVal, frUpperVal;
    IInteger64::Probe(frLower)->GetValue(&frLowerVal);
    IInteger64::Probe(frUpper)->GetValue(&frUpperVal);
    AutoPtr<IInteger64> i3;
    CInteger64::New(bcLowerVal * frLowerVal, (IInteger64**)&i3);
    AutoPtr<IInteger64> i4;
    CInteger64::New(bcUpperVal * frUpperVal, (IInteger64**)&i4);
    mBlocksPerSecondRange->Intersect(
            i3, i4, (IRange**)&mBlocksPerSecondRange);

    Int32 bpsLowerVal, bpsUpperVal;
    IInteger32::Probe(bpsLower)->GetValue(&bpsLowerVal);
    IInteger32::Probe(bpsUpper)->GetValue(&bpsUpperVal);
    i1 = NULL;
    CInteger32::New((Int32)(bpsLowerVal / bcUpperVal), (IInteger32**)&i1);
    i2 = NULL;
    CInteger32::New((Int32)(bpsUpperVal / bcLowerVal), (IInteger32**)&i2);
    mFrameRateRange->Intersect(
            i1, i2, (IRange**)&mFrameRateRange);
}

void CMediaCodecInfoVideoCapabilities::ApplyMacroBlockLimits(
    /* [in] */ Int32 maxHorizontalBlocks,
    /* [in] */ Int32 maxVerticalBlocks,
    /* [in] */ Int32 maxBlocks,
    /* [in] */ Int64 maxBlocksPerSecond,
    /* [in] */ Int32 blockWidth,
    /* [in] */ Int32 blockHeight,
    /* [in] */ Int32 widthAlignment,
    /* [in] */ Int32 heightAlignment)
{
    ApplyAlignment(widthAlignment, heightAlignment);
    AutoPtr<IInteger32> begin;
    CInteger32::New(1, (IInteger32**)&begin);
    AutoPtr<IInteger32> end;
    CInteger32::New(maxBlocks, (IInteger32**)&end);
    AutoPtr<IRange> counts;
    CRange::Create(EIID_IInteger32, begin, end, (IRange**)&counts);

    AutoPtr<IInteger64> begin64;
    CInteger64::New(1L, (IInteger64**)&begin64);
    AutoPtr<IInteger64> end64;
    CInteger64::New(maxBlocksPerSecond, (IInteger64**)&end64);
    AutoPtr<IRange> rates;
    CRange::Create(EIID_IInteger64, begin64, end64, (IRange**)&rates);

    AutoPtr<IRational> lower;
    CRational::New(1, maxVerticalBlocks, (IRational**)&lower);
    AutoPtr<IRational> upper;
    CRational::New(maxHorizontalBlocks, 1, (IRational**)&upper);
    AutoPtr<IRange> ratios;
    CRange::Create(EIID_IRational, lower, upper, (IRange**)&ratios);
    ApplyBlockLimits(
            blockWidth, blockHeight, counts, rates, ratios);

    AutoPtr<IInteger32> i1;
    CInteger32::New(1, (IInteger32**)&i1);
    AutoPtr<IInteger32> i2;
    CInteger32::New(maxHorizontalBlocks / (mBlockWidth / blockWidth),
            (IInteger32**)&i2);
    mHorizontalBlockRange->Intersect(
            i1, i2, (IRange**)&mHorizontalBlockRange);

    i2 = NULL;
    CInteger32::New(maxVerticalBlocks / (mBlockHeight / blockHeight),
            (IInteger32**)&i2);
    mVerticalBlockRange->Intersect(
            i1, i2, (IRange**)&mVerticalBlockRange);
}

void CMediaCodecInfoVideoCapabilities::ApplyLevelLimits()
{
    Int32 maxBlocksPerSecond = 0;
    Int32 maxBlocks = 0;
    Int32 maxBps = 0;
    Int32 maxDPBBlocks = 0;

    Int32 errors = CMediaCodecInfo::ERROR_NONE_SUPPORTED;
    AutoPtr<ArrayOf<IMediaCodecInfoCodecProfileLevel*> > profileLevels =
            ((CMediaCodecInfoCodecCapabilities*)mParent.Get())->mProfileLevels;
    String mime;
    mParent->GetMimeType(&mime);

    if (mime.EqualsIgnoreCase(IMediaFormat::MIMETYPE_VIDEO_AVC)) {
        maxBlocks = 99;
        maxBlocksPerSecond = 1485;
        maxBps = 64000;
        maxDPBBlocks = 396;
        for (Int32 i = 0; i < profileLevels->GetLength(); i++) {
            AutoPtr<CMediaCodecInfoCodecProfileLevel> profileLevel =
                    (CMediaCodecInfoCodecProfileLevel*)(*profileLevels)[i];
            Int32 MBPS = 0, FS = 0, BR = 0, DPB = 0;
            Boolean supported = TRUE;

            switch (profileLevel->mLevel) {
                case IMediaCodecInfoCodecProfileLevel::AVCLevel1:
                    MBPS =    1485; FS =    99; BR =     64; DPB =    396; break;
                case IMediaCodecInfoCodecProfileLevel::AVCLevel1b:
                    MBPS =    1485; FS =    99; BR =    128; DPB =    396; break;
                case IMediaCodecInfoCodecProfileLevel::AVCLevel11:
                    MBPS =    3000; FS =   396; BR =    192; DPB =    900; break;
                case IMediaCodecInfoCodecProfileLevel::AVCLevel12:
                    MBPS =    6000; FS =   396; BR =    384; DPB =   2376; break;
                case IMediaCodecInfoCodecProfileLevel::AVCLevel13:
                    MBPS =   11880; FS =   396; BR =    768; DPB =   2376; break;
                case IMediaCodecInfoCodecProfileLevel::AVCLevel2:
                    MBPS =   11880; FS =   396; BR =   2000; DPB =   2376; break;
                case IMediaCodecInfoCodecProfileLevel::AVCLevel21:
                    MBPS =   19800; FS =   792; BR =   4000; DPB =   4752; break;
                case IMediaCodecInfoCodecProfileLevel::AVCLevel22:
                    MBPS =   20250; FS =  1620; BR =   4000; DPB =   8100; break;
                case IMediaCodecInfoCodecProfileLevel::AVCLevel3:
                    MBPS =   40500; FS =  1620; BR =  10000; DPB =   8100; break;
                case IMediaCodecInfoCodecProfileLevel::AVCLevel31:
                    MBPS =  108000; FS =  3600; BR =  14000; DPB =  18000; break;
                case IMediaCodecInfoCodecProfileLevel::AVCLevel32:
                    MBPS =  216000; FS =  5120; BR =  20000; DPB =  20480; break;
                case IMediaCodecInfoCodecProfileLevel::AVCLevel4:
                    MBPS =  245760; FS =  8192; BR =  20000; DPB =  32768; break;
                case IMediaCodecInfoCodecProfileLevel::AVCLevel41:
                    MBPS =  245760; FS =  8192; BR =  50000; DPB =  32768; break;
                case IMediaCodecInfoCodecProfileLevel::AVCLevel42:
                    MBPS =  522240; FS =  8704; BR =  50000; DPB =  34816; break;
                case IMediaCodecInfoCodecProfileLevel::AVCLevel5:
                    MBPS =  589824; FS = 22080; BR = 135000; DPB = 110400; break;
                case IMediaCodecInfoCodecProfileLevel::AVCLevel51:
                    MBPS =  983040; FS = 36864; BR = 240000; DPB = 184320; break;
                case IMediaCodecInfoCodecProfileLevel::AVCLevel52:
                    MBPS = 2073600; FS = 36864; BR = 240000; DPB = 184320; break;
                default:
                    Logger::W(TAG, "Unrecognized level %d for %s",
                            profileLevel->mLevel, mime.string());
                    errors |= CMediaCodecInfo::ERROR_UNRECOGNIZED;
            }
            switch (profileLevel->mProfile) {
                case IMediaCodecInfoCodecProfileLevel::AVCProfileHigh:
                    BR *= 1250; break;
                case IMediaCodecInfoCodecProfileLevel::AVCProfileHigh10:
                    BR *= 3000; break;
                case IMediaCodecInfoCodecProfileLevel::AVCProfileExtended:
                case IMediaCodecInfoCodecProfileLevel::AVCProfileHigh422:
                case IMediaCodecInfoCodecProfileLevel::AVCProfileHigh444:
                    Logger::W(TAG, "Unsupported profile %d for %s",
                            profileLevel->mProfile, mime.string());
                    errors |= CMediaCodecInfo::ERROR_UNSUPPORTED;
                    supported = FALSE;
                    // fall through - treat as base profile
                case IMediaCodecInfoCodecProfileLevel::AVCProfileBaseline:
                case IMediaCodecInfoCodecProfileLevel::AVCProfileMain:
                    BR *= 1000; break;
                default:
                    Logger::W(TAG, "Unrecognized profile %d for %s",
                            profileLevel->mProfile, mime.string());
                    errors |= CMediaCodecInfo::ERROR_UNRECOGNIZED;
                    BR *= 1000;
            }
            if (supported) {
                errors &= ~CMediaCodecInfo::ERROR_NONE_SUPPORTED;
            }
            maxBlocksPerSecond = Elastos::Core::Math::Max(MBPS, maxBlocksPerSecond);
            maxBlocks = Elastos::Core::Math::Max(FS, maxBlocks);
            maxBps = Elastos::Core::Math::Max(BR, maxBps);
            maxDPBBlocks = Elastos::Core::Math::Max(maxDPBBlocks, DPB);
        }

        Int32 maxLengthInBlocks = (Int32)(Elastos::Core::Math::Sqrt(maxBlocks * 8));
        ApplyMacroBlockLimits(
                maxLengthInBlocks, maxLengthInBlocks,
                maxBlocks, maxBlocksPerSecond,
                16 /* blockWidth */, 16 /* blockHeight */,
                1 /* widthAlignment */, 1 /* heightAlignment */);
    }
    else if (mime.EqualsIgnoreCase(IMediaFormat::MIMETYPE_VIDEO_MPEG4)) {
        Int32 maxWidth = 11, maxHeight = 9, maxRate = 15;
        maxBlocks = 99;
        maxBlocksPerSecond = 1485;
        maxBps = 64000;

        for (Int32 i = 0; i < profileLevels->GetLength(); i++) {
            AutoPtr<CMediaCodecInfoCodecProfileLevel> profileLevel =
                    (CMediaCodecInfoCodecProfileLevel*)(*profileLevels)[i];
            Int32 MBPS = 0, FS = 0, BR = 0, FR = 0, W = 0, H = 0;
            Boolean supported = TRUE;
            switch (profileLevel->mProfile) {
                case IMediaCodecInfoCodecProfileLevel::MPEG4ProfileSimple:
                    switch (profileLevel->mLevel) {
                        case IMediaCodecInfoCodecProfileLevel::MPEG4Level0:
                            FR = 15; W = 11; H =  9; MBPS =  1485; FS =  99; BR =  64; break;
                        case IMediaCodecInfoCodecProfileLevel::MPEG4Level1:
                            FR = 30; W = 11; H =  9; MBPS =  1485; FS =  99; BR =  64; break;
                        case IMediaCodecInfoCodecProfileLevel::MPEG4Level0b:
                            FR = 30; W = 11; H =  9; MBPS =  1485; FS =  99; BR = 128; break;
                        case IMediaCodecInfoCodecProfileLevel::MPEG4Level2:
                            FR = 30; W = 22; H = 18; MBPS =  5940; FS = 396; BR = 128; break;
                        case IMediaCodecInfoCodecProfileLevel::MPEG4Level3:
                            FR = 30; W = 22; H = 18; MBPS = 11880; FS = 396; BR = 384; break;
                        case IMediaCodecInfoCodecProfileLevel::MPEG4Level4:
                        case IMediaCodecInfoCodecProfileLevel::MPEG4Level4a:
                        case IMediaCodecInfoCodecProfileLevel::MPEG4Level5:
                            // While MPEG4 SP does not have level 4 or 5, some vendors
                            // report it. Use the same limits as level 3, but mark as
                            // unsupported.
                            FR = 30; W = 22; H = 18; MBPS = 11880; FS = 396; BR = 384;
                            supported = FALSE;
                            break;
                        default:
                            Logger::W(TAG, "Unrecognized profile/level %d/%d for %s",
                                    profileLevel->mProfile, profileLevel->mLevel, mime.string());
                            errors |= CMediaCodecInfo::ERROR_UNRECOGNIZED;
                    }
                    break;
                case IMediaCodecInfoCodecProfileLevel::MPEG4ProfileAdvancedSimple:
                    switch (profileLevel->mLevel) {
                        case IMediaCodecInfoCodecProfileLevel::MPEG4Level0:
                        case IMediaCodecInfoCodecProfileLevel::MPEG4Level1:
                            FR = 30; W = 11; H =  9; MBPS =  2970; FS =   99; BR =  128; break;
                        case IMediaCodecInfoCodecProfileLevel::MPEG4Level2:
                            FR = 30; W = 22; H = 18; MBPS =  5940; FS =  396; BR =  384; break;
                        case IMediaCodecInfoCodecProfileLevel::MPEG4Level3:
                            FR = 30; W = 22; H = 18; MBPS = 11880; FS =  396; BR =  768; break;
                        // case IMediaCodecInfoCodecProfileLevel::MPEG4Level3b:
                        // TODO: MPEG4 level 3b is not defined in OMX
                        //  MBPS = 11880; FS =  396; BR = 1500; break;
                        case IMediaCodecInfoCodecProfileLevel::MPEG4Level4:
                        case IMediaCodecInfoCodecProfileLevel::MPEG4Level4a:
                            // TODO: MPEG4 level 4a is not defined in spec
                            FR = 30; W = 44; H = 36; MBPS = 23760; FS =  792; BR = 3000; break;
                        case IMediaCodecInfoCodecProfileLevel::MPEG4Level5:
                            FR = 30; W = 45; H = 36; MBPS = 48600; FS = 1620; BR = 8000; break;
                        default:
                            Logger::W(TAG, "Unrecognized profile/level %d/%d for %s",
                                    profileLevel->mProfile, profileLevel->mLevel, mime.string());
                            errors |= CMediaCodecInfo::ERROR_UNRECOGNIZED;
                    }
                    break;
                case IMediaCodecInfoCodecProfileLevel::MPEG4ProfileMain:             // 2-4
                case IMediaCodecInfoCodecProfileLevel::MPEG4ProfileNbit:             // 2
                case IMediaCodecInfoCodecProfileLevel::MPEG4ProfileAdvancedRealTime: // 1-4
                case IMediaCodecInfoCodecProfileLevel::MPEG4ProfileCoreScalable:     // 1-3
                case IMediaCodecInfoCodecProfileLevel::MPEG4ProfileAdvancedCoding:   // 1-4
                case IMediaCodecInfoCodecProfileLevel::MPEG4ProfileCore:             // 1-2
                case IMediaCodecInfoCodecProfileLevel::MPEG4ProfileAdvancedCore:     // 1-4
                case IMediaCodecInfoCodecProfileLevel::MPEG4ProfileSimpleScalable:   // 0-2
                case IMediaCodecInfoCodecProfileLevel::MPEG4ProfileAdvancedScalable: // 1-3
                case IMediaCodecInfoCodecProfileLevel::MPEG4ProfileHybrid:           // 1-2
                case IMediaCodecInfoCodecProfileLevel::MPEG4ProfileBasicAnimated:    // 1-2
                case IMediaCodecInfoCodecProfileLevel::MPEG4ProfileScalableTexture:  // 1
                case IMediaCodecInfoCodecProfileLevel::MPEG4ProfileSimpleFace:       // 1-2
                case IMediaCodecInfoCodecProfileLevel::MPEG4ProfileSimpleFBA:        // 1-2
                    Logger::I(TAG, "Unsupported profile %d for %s",
                            profileLevel->mProfile, mime.string());
                    errors |= CMediaCodecInfo::ERROR_UNSUPPORTED;
                    supported = FALSE;
                    break;
                default:
                    Logger::W(TAG, "Unrecognized profile %d for %s",
                            profileLevel->mProfile, mime.string());
                    errors |= CMediaCodecInfo::ERROR_UNRECOGNIZED;
            }
            if (supported) {
                errors &= ~CMediaCodecInfo::ERROR_NONE_SUPPORTED;
            }
            maxBlocksPerSecond = Elastos::Core::Math::Max(MBPS, maxBlocksPerSecond);
            maxBlocks = Elastos::Core::Math::Max(FS, maxBlocks);
            maxBps = Elastos::Core::Math::Max(BR * 1000, maxBps);
            maxWidth = Elastos::Core::Math::Max(W, maxWidth);
            maxHeight = Elastos::Core::Math::Max(H, maxHeight);
            maxRate = Elastos::Core::Math::Max(FR, maxRate);
        }

        ApplyMacroBlockLimits(maxWidth, maxHeight,
                maxBlocks, maxBlocksPerSecond,
                16 /* blockWidth */, 16 /* blockHeight */,
                1 /* widthAlignment */, 1 /* heightAlignment */);
        AutoPtr<IInteger32> i1;
        CInteger32::New(12, (IInteger32**)&i1);
        AutoPtr<IInteger32> i2;
        CInteger32::New(maxRate, (IInteger32**)&i2);
        mFrameRateRange->Intersect(i1, i2, (IRange**)&mFrameRateRange);
    }
    else if (mime.EqualsIgnoreCase(IMediaFormat::MIMETYPE_VIDEO_H263)) {
        Int32 maxWidth = 11, maxHeight = 9, maxRate = 15;
        maxBlocks = 99;
        maxBlocksPerSecond = 1485;
        maxBps = 64000;

        for (Int32 i = 0; i < profileLevels->GetLength(); i++) {
            AutoPtr<CMediaCodecInfoCodecProfileLevel> profileLevel =
                    (CMediaCodecInfoCodecProfileLevel*)(*profileLevels)[i];
            Int32 MBPS = 0, BR = 0, FR = 0, W = 0, H = 0;
            switch (profileLevel->mLevel) {
                case IMediaCodecInfoCodecProfileLevel::H263Level10:
                    FR = 15; W = 11; H =  9; BR =   1; MBPS =  W * H * FR; break;
                case IMediaCodecInfoCodecProfileLevel::H263Level20:
                    // only supports CIF, 0..QCIF
                    FR = 30; W = 22; H = 18; BR =   2; MBPS =  W * H * FR; break;
                case IMediaCodecInfoCodecProfileLevel::H263Level30:
                    // only supports CIF, 0..QCIF
                    FR = 30; W = 22; H = 18; BR =   6; MBPS =  W * H * FR; break;
                case IMediaCodecInfoCodecProfileLevel::H263Level40:
                    // only supports CIF, 0..QCIF
                    FR = 30; W = 22; H = 18; BR =  32; MBPS =  W * H * FR; break;
                case IMediaCodecInfoCodecProfileLevel::H263Level45:
                    // only implies level 10 support
                    FR = 30; W = 11; H =  9; BR =   2; MBPS =  W * H * FR; break;
                case IMediaCodecInfoCodecProfileLevel::H263Level50:
                    // only supports 50fps for H > 15
                    FR = 60; W = 22; H = 18; BR =  64; MBPS =  W * H * 50; break;
                case IMediaCodecInfoCodecProfileLevel::H263Level60:
                    // only supports 50fps for H > 15
                    FR = 60; W = 45; H = 18; BR = 128; MBPS =  W * H * 50; break;
                case IMediaCodecInfoCodecProfileLevel::H263Level70:
                    // only supports 50fps for H > 30
                    FR = 60; W = 45; H = 36; BR = 256; MBPS =  W * H * 50; break;
                default:
                    Logger::W(TAG, "Unrecognized profile/level %d/%d for %s",
                            profileLevel->mProfile, profileLevel->mLevel, mime.string());
                    errors |= CMediaCodecInfo::ERROR_UNRECOGNIZED;
            }
            switch (profileLevel->mProfile) {
                case IMediaCodecInfoCodecProfileLevel::H263ProfileBackwardCompatible:
                case IMediaCodecInfoCodecProfileLevel::H263ProfileBaseline:
                case IMediaCodecInfoCodecProfileLevel::H263ProfileH320Coding:
                case IMediaCodecInfoCodecProfileLevel::H263ProfileHighCompression:
                case IMediaCodecInfoCodecProfileLevel::H263ProfileHighLatency:
                case IMediaCodecInfoCodecProfileLevel::H263ProfileInterlace:
                case IMediaCodecInfoCodecProfileLevel::H263ProfileInternet:
                case IMediaCodecInfoCodecProfileLevel::H263ProfileISWV2:
                case IMediaCodecInfoCodecProfileLevel::H263ProfileISWV3:
                    break;
                default:
                    Logger::W(TAG, "Unrecognized profile %d for %s",
                            profileLevel->mProfile, mime.string());
                    errors |= CMediaCodecInfo::ERROR_UNRECOGNIZED;
            }
            errors &= ~CMediaCodecInfo::ERROR_NONE_SUPPORTED;
            maxBlocksPerSecond = Elastos::Core::Math::Max(MBPS, maxBlocksPerSecond);
            maxBlocks = Elastos::Core::Math::Max(W * H, maxBlocks);
            maxBps = Elastos::Core::Math::Max(BR * 64000, maxBps);
            maxWidth = Elastos::Core::Math::Max(W, maxWidth);
            maxHeight = Elastos::Core::Math::Max(H, maxHeight);
            maxRate = Elastos::Core::Math::Max(FR, maxRate);
        }

        ApplyMacroBlockLimits(maxWidth, maxHeight,
                maxBlocks, maxBlocksPerSecond,
                16 /* blockWidth */, 16 /* blockHeight */,
                1 /* widthAlignment */, 1 /* heightAlignment */);

        AutoPtr<IInteger32> begin;
        CInteger32::New(1, (IInteger32**)&begin);
        AutoPtr<IInteger32> end;
        CInteger32::New(maxRate, (IInteger32**)&end);
        CRange::Create(EIID_IInteger32, begin, end, (IRange**)&mFrameRateRange);
    }
    else if (mime.EqualsIgnoreCase(IMediaFormat::MIMETYPE_VIDEO_VP8) ||
            mime.EqualsIgnoreCase(IMediaFormat::MIMETYPE_VIDEO_VP9)) {
        maxBlocks = maxBlocksPerSecond = Elastos::Core::Math::INT32_MAX_VALUE;

        // TODO: set to 100Mbps for now, need a number for VPX
        maxBps = 100000000;

        // profile levels are not indicative for VPx, but verify
        // them nonetheless
        for (Int32 i = 0; i < profileLevels->GetLength(); i++) {
            AutoPtr<CMediaCodecInfoCodecProfileLevel> profileLevel =
                    (CMediaCodecInfoCodecProfileLevel*)(*profileLevels)[i];
            switch (profileLevel->mLevel) {
                case IMediaCodecInfoCodecProfileLevel::VP8Level_Version0:
                case IMediaCodecInfoCodecProfileLevel::VP8Level_Version1:
                case IMediaCodecInfoCodecProfileLevel::VP8Level_Version2:
                case IMediaCodecInfoCodecProfileLevel::VP8Level_Version3:
                    break;
                default:
                    Logger::W(TAG, "Unrecognized level %d for %s",
                            profileLevel->mLevel, mime.string());
                    errors |= CMediaCodecInfo::ERROR_UNRECOGNIZED;
            }
            switch (profileLevel->mProfile) {
                case IMediaCodecInfoCodecProfileLevel::VP8ProfileMain:
                    break;
                default:
                    Logger::W(TAG, "Unrecognized profile %d for %s",
                            profileLevel->mProfile, mime.string());
                    errors |= CMediaCodecInfo::ERROR_UNRECOGNIZED;
            }
            errors &= ~CMediaCodecInfo::ERROR_NONE_SUPPORTED;
        }

        Int32 blockSize =
            mime.EqualsIgnoreCase(IMediaFormat::MIMETYPE_VIDEO_VP8) ? 16 : 8;
        ApplyMacroBlockLimits(Elastos::Core::Math::INT16_MAX_VALUE,
                Elastos::Core::Math::INT16_MAX_VALUE,
                maxBlocks, maxBlocksPerSecond, blockSize, blockSize,
                1 /* widthAlignment */, 1 /* heightAlignment */);
    }
    else if (mime.EqualsIgnoreCase(IMediaFormat::MIMETYPE_VIDEO_HEVC)) {
        maxBlocks = 36864;
        maxBlocksPerSecond = maxBlocks * 15;
        maxBps = 128000;
        for (Int32 i = 0; i < profileLevels->GetLength(); i++) {
            AutoPtr<CMediaCodecInfoCodecProfileLevel> profileLevel =
                    (CMediaCodecInfoCodecProfileLevel*)(*profileLevels)[i];
            Double FR = 0;
            Int32 FS = 0;
            Int32 BR = 0;
            switch (profileLevel->mLevel) {
                case IMediaCodecInfoCodecProfileLevel::HEVCMainTierLevel1:
                case IMediaCodecInfoCodecProfileLevel::HEVCHighTierLevel1:
                    FR =    15; FS =    36864; BR =    128; break;
                case IMediaCodecInfoCodecProfileLevel::HEVCMainTierLevel2:
                case IMediaCodecInfoCodecProfileLevel::HEVCHighTierLevel2:
                    FR =    30; FS =   122880; BR =   1500; break;
                case IMediaCodecInfoCodecProfileLevel::HEVCMainTierLevel21:
                case IMediaCodecInfoCodecProfileLevel::HEVCHighTierLevel21:
                    FR =    30; FS =   245760; BR =   3000; break;
                case IMediaCodecInfoCodecProfileLevel::HEVCMainTierLevel3:
                case IMediaCodecInfoCodecProfileLevel::HEVCHighTierLevel3:
                    FR =    30; FS =   552960; BR =   6000; break;
                case IMediaCodecInfoCodecProfileLevel::HEVCMainTierLevel31:
                case IMediaCodecInfoCodecProfileLevel::HEVCHighTierLevel31:
                    FR = 33.75; FS =   983040; BR =  10000; break;
                case IMediaCodecInfoCodecProfileLevel::HEVCMainTierLevel4:
                    FR =    30; FS =  2228224; BR =  12000; break;
                case IMediaCodecInfoCodecProfileLevel::HEVCHighTierLevel4:
                    FR =    30; FS =  2228224; BR =  30000; break;
                case IMediaCodecInfoCodecProfileLevel::HEVCMainTierLevel41:
                    FR =    60; FS =  2228224; BR =  20000; break;
                case IMediaCodecInfoCodecProfileLevel::HEVCHighTierLevel41:
                    FR =    60; FS =  2228224; BR =  50000; break;
                case IMediaCodecInfoCodecProfileLevel::HEVCMainTierLevel5:
                    FR =    30; FS =  8912896; BR =  25000; break;
                case IMediaCodecInfoCodecProfileLevel::HEVCHighTierLevel5:
                    FR =    30; FS =  8912896; BR = 100000; break;
                case IMediaCodecInfoCodecProfileLevel::HEVCMainTierLevel51:
                    FR =    60; FS =  8912896; BR =  40000; break;
                case IMediaCodecInfoCodecProfileLevel::HEVCHighTierLevel51:
                    FR =    60; FS =  8912896; BR = 160000; break;
                case IMediaCodecInfoCodecProfileLevel::HEVCMainTierLevel52:
                    FR =   120; FS =  8912896; BR =  60000; break;
                case IMediaCodecInfoCodecProfileLevel::HEVCHighTierLevel52:
                    FR =   120; FS =  8912896; BR = 240000; break;
                case IMediaCodecInfoCodecProfileLevel::HEVCMainTierLevel6:
                    FR =    30; FS = 35651584; BR =  60000; break;
                case IMediaCodecInfoCodecProfileLevel::HEVCHighTierLevel6:
                    FR =    30; FS = 35651584; BR = 240000; break;
                case IMediaCodecInfoCodecProfileLevel::HEVCMainTierLevel61:
                    FR =    60; FS = 35651584; BR = 120000; break;
                case IMediaCodecInfoCodecProfileLevel::HEVCHighTierLevel61:
                    FR =    60; FS = 35651584; BR = 480000; break;
                case IMediaCodecInfoCodecProfileLevel::HEVCMainTierLevel62:
                    FR =   120; FS = 35651584; BR = 240000; break;
                case IMediaCodecInfoCodecProfileLevel::HEVCHighTierLevel62:
                    FR =   120; FS = 35651584; BR = 800000; break;
                default:
                    Logger::W(TAG, "Unrecognized level %d for %s",
                            profileLevel->mLevel, mime.string());
                    errors |= CMediaCodecInfo::ERROR_UNRECOGNIZED;
            }
            switch (profileLevel->mProfile) {
                case IMediaCodecInfoCodecProfileLevel::HEVCProfileMain:
                case IMediaCodecInfoCodecProfileLevel::HEVCProfileMain10:
                    break;
                default:
                    Logger::W(TAG, "Unrecognized profile %d for %s",
                            profileLevel->mProfile, mime.string());
                    errors |= CMediaCodecInfo::ERROR_UNRECOGNIZED;
            }

            /* DPB logic:
            if      (width * height <= FS / 4)    DPB = 16;
            else if (width * height <= FS / 2)    DPB = 12;
            else if (width * height <= FS * 0.75) DPB = 8;
            else                                  DPB = 6;
            */

            errors &= ~CMediaCodecInfo::ERROR_NONE_SUPPORTED;
            maxBlocksPerSecond = Elastos::Core::Math::Max((Int32)(FR * FS), maxBlocksPerSecond);
            maxBlocks = Elastos::Core::Math::Max(FS, maxBlocks);
            maxBps = Elastos::Core::Math::Max(BR * 1000, maxBps);
        }

        Int32 maxLengthInBlocks = (Int32)(Elastos::Core::Math::Sqrt(maxBlocks * 8));
        // CTBs are at least 8x8
        maxBlocks = Utils::DivUp(maxBlocks, 8 * 8);
        maxBlocksPerSecond = Utils::DivUp(maxBlocksPerSecond, 8 * 8);
        maxLengthInBlocks = Utils::DivUp(maxLengthInBlocks, 8);

        ApplyMacroBlockLimits(
                maxLengthInBlocks, maxLengthInBlocks,
                maxBlocks, maxBlocksPerSecond,
                8 /* blockWidth */, 8 /* blockHeight */,
                1 /* widthAlignment */, 1 /* heightAlignment */);
    }
    else {
        Logger::W(TAG, "Unsupported mime %s", mime.string());
        // using minimal bitrate here.  should be overriden by
        // info from media_codecs.xml
        maxBps = 64000;
        errors |= CMediaCodecInfo::ERROR_UNSUPPORTED;
    }
    AutoPtr<IInteger32> begin;
    CInteger32::New(1, (IInteger32**)&begin);
    AutoPtr<IInteger32> end;
    CInteger32::New(maxBps, (IInteger32**)&end);
    CRange::Create(EIID_IInteger32, begin, end, (IRange**)&mBitrateRange);
    ((CMediaCodecInfoCodecCapabilities*)mParent.Get())->mError |= errors;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
