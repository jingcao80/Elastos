
#include "elastos/droid/view/inputmethod/CSparseRectFArray.h"
#include "elastos/droid/graphics/CRectF.h"

#include <elastos/utility/Arrays.h>

using Elastos::Droid::Graphics::CRectF;

using Elastos::Utility::Arrays;

namespace Elastos {
namespace Droid {
namespace View {
namespace InputMethod {

//========================================================================================
//              CSparseRectFArray::SparseRectFArrayBuilder::
//========================================================================================
const Int32 CSparseRectFArray::SparseRectFArrayBuilder::INITIAL_SIZE = 16;

CAR_INTERFACE_IMPL(CSparseRectFArray::SparseRectFArrayBuilder, Object, ISparseRectFArrayBuilder)

CSparseRectFArray::SparseRectFArrayBuilder::SparseRectFArrayBuilder()
    : mCount(0)
{
}

void CSparseRectFArray::SparseRectFArrayBuilder::CheckIndex(
    /* [in] */ Int32 key)
{
    if (mCount == 0) {
        return;
    }
    if ((*mKeys)[mCount - 1] >= key) {
        //throw new IllegalArgumentException("key must be greater than all existing keys.");
        return;
    }
}

void CSparseRectFArray::SparseRectFArrayBuilder::EnsureBufferSize()
{
    if (mKeys == NULL) {
        mKeys = ArrayOf<Int32>::Alloc(INITIAL_SIZE);
    }
    if (mCoordinates == NULL) {
        mCoordinates = ArrayOf<Float>::Alloc(INITIAL_SIZE * 4);
    }
    if (mFlagsArray == NULL) {
        mFlagsArray = ArrayOf<Int32>::Alloc(INITIAL_SIZE);
    }
    Int32 requiredIndexArraySize = mCount + 1;
    if (mKeys->GetLength() <= requiredIndexArraySize) {
        AutoPtr<ArrayOf<Int32> > newArray = ArrayOf<Int32>::Alloc(requiredIndexArraySize * 2);
        newArray->Copy(mKeys, 0, mCount);
        mKeys = newArray;
    }
    Int32 requiredCoordinatesArraySize = (mCount + 1) * 4;
    if (mCoordinates->GetLength() <= requiredCoordinatesArraySize) {
        AutoPtr<ArrayOf<Float> > newArray = ArrayOf<Float>::Alloc(requiredCoordinatesArraySize * 2);
        newArray->Copy(mCoordinates, 0, mCount * 4);
        mCoordinates = newArray;
    }
    Int32 requiredFlagsArraySize = requiredIndexArraySize;
    if (mFlagsArray->GetLength() <= requiredFlagsArraySize) {
        AutoPtr<ArrayOf<Int32> > newArray = ArrayOf<Int32>::Alloc(requiredFlagsArraySize * 2);
        newArray->Copy(mFlagsArray, 0, mCount);
        mFlagsArray = newArray;
    }
}

ECode CSparseRectFArray::SparseRectFArrayBuilder::Append(
    /* [in] */ Int32 key,
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ Int32 flags)
{
    CheckIndex(key);
    EnsureBufferSize();
    Int32 baseCoordinatesIndex = mCount * 4;
    (*mCoordinates)[baseCoordinatesIndex + 0] = left;
    (*mCoordinates)[baseCoordinatesIndex + 1] = top;
    (*mCoordinates)[baseCoordinatesIndex + 2] = right;
    (*mCoordinates)[baseCoordinatesIndex + 3] = bottom;
    Int32 flagsIndex = mCount;
    (*mFlagsArray)[flagsIndex] = flags;
    (*mKeys)[mCount] = key;
    ++mCount;
    return NOERROR;
}

ECode CSparseRectFArray::SparseRectFArrayBuilder::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mCount <= 0;
    return NOERROR;
}

ECode CSparseRectFArray::SparseRectFArrayBuilder::Build(
    /* [out] */ ISparseRectFArray** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ISparseRectFArray> obj;
    CSparseRectFArray::New((ISparseRectFArray**)&obj);
    ((CSparseRectFArray*)obj.Get())->constructor(this);
    *result = obj;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CSparseRectFArray::SparseRectFArrayBuilder::Reset()
{
    if (mCount == 0) {
        mKeys = NULL;
        mCoordinates = NULL;
        mFlagsArray = NULL;
    }
    mCount = 0;
    return NOERROR;
}

//========================================================================================
//              CSparseRectFArray::
//========================================================================================
CAR_INTERFACE_IMPL_2(CSparseRectFArray, Object, ISparseRectFArray, IParcelable)

CAR_OBJECT_IMPL(CSparseRectFArray)

ECode CSparseRectFArray::constructor()
{
    return NOERROR;
}

ECode CSparseRectFArray::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadArrayOf((Handle32*)&mKeys);
    source->ReadArrayOf((Handle32*)&mCoordinates);
    source->ReadArrayOf((Handle32*)&mFlagsArray);
    return NOERROR;
}

ECode CSparseRectFArray::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteArrayOf((Handle32)mKeys.Get());
    dest->WriteArrayOf((Handle32)mCoordinates.Get());
    dest->WriteArrayOf((Handle32)mFlagsArray.Get());
    return NOERROR;
}

ECode CSparseRectFArray::GetHashCode(
    /* [out] */ Int32* hc)
{
    VALIDATE_NOT_NULL(hc)

    // TODO: Improve the hash function.
    if (mKeys == NULL || mKeys->GetLength() == 0) {
        hc = 0;
        return NOERROR;
    }
    Int32 hash = mKeys->GetLength();
    // For performance reasons, only the first rectangle is used for the hash code now.
    for (Int32 i = 0; i < 4; i++) {
        hash *= 31;
        hash += (*mCoordinates)[i];
    }
    hash *= 31;
    hash += (*mFlagsArray)[0];
    *hc = hash;
    return NOERROR;
}

ECode CSparseRectFArray::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (obj == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    if (TO_IINTERFACE(this) == IInterface::Probe(obj)) {
        *result = TRUE;
        return NOERROR;
    }
    if (ISparseRectFArray::Probe(obj) == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<ISparseRectFArray> that = ISparseRectFArray::Probe(obj);
    AutoPtr<CSparseRectFArray> cthat = (CSparseRectFArray*)that.Get();

    *result = Arrays::Equals(mKeys, cthat->mKeys) && Arrays::Equals(mCoordinates, cthat->mCoordinates)
                && Arrays::Equals(mFlagsArray, cthat->mFlagsArray);
    return NOERROR;
}

ECode CSparseRectFArray::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    if (mKeys == NULL || mCoordinates == NULL || mFlagsArray == NULL) {
        *result = "SparseRectFArray{}";
        return NOERROR;
    }
    StringBuilder sb;
    sb.Append("SparseRectFArray{");
    for (Int32 i = 0; i < mKeys->GetLength(); i++) {
        if (i != 0) {
            sb.Append(", ");
        }
        Int32 baseIndex = i * 4;
        sb.Append((*mKeys)[i]);
        sb.Append(":[");
        sb.Append((*mCoordinates)[baseIndex + 0]);
        sb.Append(",");
        sb.Append((*mCoordinates)[baseIndex + 1]);
        sb.Append("],[");
        sb.Append((*mCoordinates)[baseIndex + 2]);
        sb.Append(",");
        sb.Append((*mCoordinates)[baseIndex + 3]);
        sb.Append("]:flagsArray=");
        sb.Append((*mFlagsArray)[i]);
    }
    sb.Append("}");
    return sb.ToString(result);
}

ECode CSparseRectFArray::constructor(
    /* [in] */ SparseRectFArrayBuilder* builder)
{
    if (builder->mCount == 0) {
        mKeys = NULL;
        mCoordinates = NULL;
        mFlagsArray = NULL;
    }
    else {
        mKeys = ArrayOf<Int32>::Alloc(builder->mCount);
        mCoordinates = ArrayOf<Float>::Alloc(builder->mCount * 4);
        mFlagsArray = ArrayOf<Int32>::Alloc(builder->mCount);
        mKeys->Copy(builder->mKeys, 0, builder->mCount);
        mCoordinates->Copy(builder->mCoordinates, 0, builder->mCount * 4);
        mFlagsArray->Copy(builder->mFlagsArray, 0, builder->mCount);
    }
    return NOERROR;
}

ECode CSparseRectFArray::Get(
    /* [in] */ Int32 index,
    /* [out] */ IRectF** result)
{
    VALIDATE_NOT_NULL(result)

    if (mKeys == NULL) {
        *result = NULL;
        return NOERROR;
    }
    if (index < 0) {
        *result = NULL;
        return NOERROR;
    }
    Int32 arrayIndex = 0;
    Arrays::BinarySearch(mKeys, index, &arrayIndex);
    if (arrayIndex < 0) {
        *result = NULL;
        return NOERROR;
    }
    Int32 baseCoordIndex = arrayIndex * 4;
    AutoPtr<IRectF> res;
    CRectF::New((*mCoordinates)[baseCoordIndex],
            (*mCoordinates)[baseCoordIndex + 1],
            (*mCoordinates)[baseCoordIndex + 2],
            (*mCoordinates)[baseCoordIndex + 3], (IRectF**)&res);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CSparseRectFArray::GetFlags(
    /* [in] */ Int32 index,
    /* [in] */ Int32 valueIfKeyNotFound,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    if (mKeys == NULL) {
        *result = valueIfKeyNotFound;
        return NOERROR;
    }
    if (index < 0) {
        *result = valueIfKeyNotFound;
        return NOERROR;
    }
    Int32 arrayIndex = 0;
    Arrays::BinarySearch(mKeys, index, &arrayIndex);
    if (arrayIndex < 0) {
        *result = valueIfKeyNotFound;
        return NOERROR;
    }
    *result = (*mFlagsArray)[arrayIndex];
    return NOERROR;
}

} // namespace InputMethod
} // namespace View
} // namespace Droid
} // namespace Elastos
