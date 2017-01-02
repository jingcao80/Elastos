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

#include "Elastos.CoreLibrary.Text.h"
#include "CollationElementIteratorICU.h"
#include "NativeCollation.h"

namespace Libcore {
namespace ICU {

extern "C" const _ELASTOS ClassID ECLSID_CollationElementIteratorICU = {
    { 0x015e3ac6, 0x25b8, 0x4982, { 0xa2, 0xb9, 0x24, 0x2f, 0x8f, 0xf1, 0x15, 0xde } },
    (char *)c_pElastos_CoreLibraryUunm,
    0xf6f1637e };

const Int32 CollationElementIteratorICU::PRIMARY_ORDER_MASK_ = 0xffff0000;
const Int32 CollationElementIteratorICU::SECONDARY_ORDER_MASK_ = 0x0000ff00;
const Int32 CollationElementIteratorICU::TERTIARY_ORDER_MASK_ = 0x000000ff;
const Int32 CollationElementIteratorICU::PRIMARY_ORDER_SHIFT_ = 16;
const Int32 CollationElementIteratorICU::SECONDARY_ORDER_SHIFT_ = 8;
const Int32 CollationElementIteratorICU::UNSIGNED_16_BIT_MASK_ = 0x0000FFFF;

CAR_INTERFACE_IMPL(CollationElementIteratorICU, Object, ICollationElementIteratorICU)

CollationElementIteratorICU::CollationElementIteratorICU()
    : mAddress(0)
{}

CollationElementIteratorICU::CollationElementIteratorICU(
    /* [in] */ Int64 address)
    : mAddress(address)
{}

CollationElementIteratorICU::~CollationElementIteratorICU()
{
    //try {
    NativeCollation::CloseElements(mAddress);
    //} finally {
    //    super.finalize();
    //}
}

ECode CollationElementIteratorICU::Reset()
{
    NativeCollation::Reset(mAddress);
    return NOERROR;
}

ECode CollationElementIteratorICU::GetNext(
    /* [out] */ Int32* nextValue)
{
    return NativeCollation::GetNext(mAddress, nextValue);
}

ECode CollationElementIteratorICU::GetPrevious(
    /* [out] */ Int32* previousValue)
{
    return NativeCollation::GetPrevious(mAddress, previousValue);
}

ECode CollationElementIteratorICU::GetMaxExpansion(
    /* [in] */ Int32 order,
    /* [out] */ Int32* maxExpansion)
{
    VALIDATE_NOT_NULL(maxExpansion);
    *maxExpansion = NativeCollation::GetMaxExpansion(mAddress, order);
    return NOERROR;
}

ECode CollationElementIteratorICU::SetText(
    /* [in] */ const String& source)
{
    return NativeCollation::SetText(mAddress, source);
}

ECode CollationElementIteratorICU::SetText(
    /* [in] */ ICharacterIterator* source)
{
    return NativeCollation::SetText(mAddress, Object::ToString(source));
}

ECode CollationElementIteratorICU::GetOffset(
    /* [out] */ Int32* offset)
{
    VALIDATE_NOT_NULL(offset);
    *offset = NativeCollation::GetOffset(mAddress);
    return NOERROR;
}

ECode CollationElementIteratorICU::SetOffset(
    /* [in] */ Int32 offset)
{
    return NativeCollation::SetOffset(mAddress, offset);
}

Int32 CollationElementIteratorICU::PrimaryOrder(
    /* [in] */ Int32 order)
{
    return ((order & PRIMARY_ORDER_MASK_) >> PRIMARY_ORDER_SHIFT_) &
            UNSIGNED_16_BIT_MASK_;
}

Int32 CollationElementIteratorICU::SecondaryOrder(
    /* [in] */ Int32 order)
{
    return (order & SECONDARY_ORDER_MASK_) >> SECONDARY_ORDER_SHIFT_;
}

Int32 CollationElementIteratorICU::TertiaryOrder(
    /* [in] */ Int32 order)
{
    return order & TERTIARY_ORDER_MASK_;
}

AutoPtr<ICollationElementIteratorICU> CollationElementIteratorICU::GetInstance(
    /* [in] */ Int64 collatorAddress,
    /* [in] */ const String& source)
{
    Int64 iteratorAddress = 0;
    NativeCollation::GetCollationElementIterator(collatorAddress, source , &iteratorAddress);
    return (ICollationElementIteratorICU*)new CollationElementIteratorICU(iteratorAddress);
}

ECode CollationElementIteratorICU::ToString(
        /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info)

    *info = String("CollationElementIteratorICU");
    return NOERROR;
}

ECode CollationElementIteratorICU::GetClassID(
    /* [out] */ ClassID *pCLSID)
{
    VALIDATE_NOT_NULL(pCLSID)

    *pCLSID = ECLSID_CollationElementIteratorICU;
    return NOERROR;
}

} // namespace ICU
} // namespace Libcore
