
#include "org/javia/arity/FunctionStack.h"

namespace Org {
namespace Javia {
namespace Arity {

FunctionStack::FunctionStack()
    : mSize(0)
{
    mData = ArrayOf<IFunction*>::Alloc(8);
}

void FunctionStack::Clear()
{
    mSize = 0;
}

void FunctionStack::Push(
    /* [in] */ IFunction* b)
{
    if (mSize >= mData->GetLength()) {
        AutoPtr<ArrayOf<IFunction*> > newData = ArrayOf<IFunction*>::Alloc(mData->GetLength() << 1);
        newData->Copy(0, mData, 0, mData->GetLength());
        mData = newData;
    }
    mData->Set(mSize++, b);
}

AutoPtr<IFunction> FunctionStack::Pop()
{
    return (*mData)[--mSize];
}

AutoPtr<ArrayOf<IFunction*> > FunctionStack::ToArray()
{
    AutoPtr<ArrayOf<IFunction*> > trimmed = ArrayOf<IFunction*>::Alloc(mSize);
    trimmed->Copy(0, mData, 0, mSize);
    return trimmed;
}

} // namespace Arity
} // namespace Javia
} // namespace Org