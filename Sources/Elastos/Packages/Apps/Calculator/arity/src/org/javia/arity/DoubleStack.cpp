
#include "org/javia/arity/DoubleStack.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;

namespace Org {
namespace Javia {
namespace Arity {

DoubleStack::DoubleStack()
    : mSize(0)
{
    mRe = ArrayOf<Double>::Alloc(8);
    mIm = ArrayOf<Double>::Alloc(8);
}

void DoubleStack::Clear()
{
    mSize = 0;
}

void DoubleStack::Push(
    /* [in] */ Double a,
    /* [in] */ Double b)
{
    if (mSize >= mRe->GetLength()) {
        Int32 newSize = mRe->GetLength() << 1;
        AutoPtr<ArrayOf<Double> > newRe = ArrayOf<Double>::Alloc(newSize);
        AutoPtr<ArrayOf<Double> > newIm = ArrayOf<Double>::Alloc(newSize);
        newRe->Copy(0, mRe, 0, mRe->GetLength());
        newIm->Copy(0, mIm, 0, mRe->GetLength());
        mRe = newRe;
        mIm = newIm;
    }
    (*mRe)[mSize] = a;
    (*mIm)[mSize] = b;
    ++mSize;
}

ECode DoubleStack::Pop(
    /* [in] */ Int32 cnt)
{
    if (cnt > mSize) {
        Slogger::E("DoubleStack", "pop %d from %d", cnt, mSize);
        return E_INVALID_ARGUMENT;
    }
    mSize -= cnt;
    return NOERROR;
}

void DoubleStack::Pop()
{
    --mSize;
}

AutoPtr<ArrayOf<Double> > DoubleStack::GetRe()
{
    AutoPtr<ArrayOf<Double> > trimmed = ArrayOf<Double>::Alloc(mSize);
    trimmed->Copy(0, mRe, 0, mSize);
    return trimmed;
}

AutoPtr<ArrayOf<Double> > DoubleStack::GetIm()
{
    Boolean allZero = TRUE;
    for (Int32 i = 0; i < mSize; ++i) {
        if ((*mIm)[i] != 0) {
            allZero = FALSE;
            break;
        }
    }
    if (allZero) {
        return NULL;
    }

    AutoPtr<ArrayOf<Double> > trimmed = ArrayOf<Double>::Alloc(mSize);
    trimmed->Copy(0, mIm, 0, mSize);
    return trimmed;
}

} // namespace Arity
} // namespace Javia
} // namespace Org