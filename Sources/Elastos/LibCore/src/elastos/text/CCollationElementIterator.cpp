
#include "Elastos.CoreLibrary.Libcore.h"
#include "CCollationElementIterator.h"

namespace Elastos {
namespace Text {

CAR_OBJECT_IMPL(CCollationElementIterator)

CAR_INTERFACE_IMPL(CCollationElementIterator, Object, ICollationElementIterator)

ECode CCollationElementIterator::constructor(
    /* [in] */ ICollationElementIteratorICU* iterator)
{
    VALIDATE_NOT_NULL(iterator)
    mIcuIterator = iterator;
    return NOERROR;
}

ECode CCollationElementIterator::GetMaxExpansion(
    /* [in] */ Int32 order,
    /* [out] */ Int32* maxExpansion)
{
    return mIcuIterator->GetMaxExpansion(order, maxExpansion);
}

ECode CCollationElementIterator::GetOffset(
    /* [out] */ Int32* offset)
{
    return mIcuIterator->GetOffset(offset);
}

ECode CCollationElementIterator::GetNext(
    /* [out] */ Int32* nextValue)
{
    return mIcuIterator->GetNext(nextValue);
}

ECode CCollationElementIterator::GetPrevious(
    /* [out] */ Int32* previousValue)
{
    return mIcuIterator->GetPrevious(previousValue);
}

ECode CCollationElementIterator::Reset()
{
    return mIcuIterator->Reset();
}

ECode CCollationElementIterator::SetOffset(
    /* [in] */ Int32 newOffset)
{
    return mIcuIterator->SetOffset(newOffset);
}

ECode CCollationElementIterator::SetText(
    /* [in] */ ICharacterIterator* source)
{
    return mIcuIterator->SetText(source);
}

ECode CCollationElementIterator::SetText(
    /* [in] */ const String& source)
{
    return mIcuIterator->SetText(source);
}

} // namespace Text
} // namespace Elastos