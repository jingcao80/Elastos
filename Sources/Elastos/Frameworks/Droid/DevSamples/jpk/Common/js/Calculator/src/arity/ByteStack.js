
#include "org/javia/arity/ByteStack.h"

namespace Org {
namespace Javia {
namespace Arity {

ByteStack::ByteStack()
    : mSize(0)
{
    mData = ArrayOf<Byte>::Alloc(8);
}

void ByteStack::Clear()
{
    mSize = 0;
}

void ByteStack::Push(
    /* [in] */ Byte b)
{
    if (mSize >= mData->GetLength()) {
        AutoPtr<ArrayOf<Byte> > newData = ArrayOf<Byte>::Alloc(mData->GetLength() << 1);
        newData->Copy(0, mData, 0, mData->GetLength());
        mData = newData;
    }
    (*mData)[mSize++] = b;
}

Byte ByteStack::Pop()
{
    return (*mData)[--mSize];
}

AutoPtr<ArrayOf<Byte> > ByteStack::ToArray()
{
    AutoPtr<ArrayOf<Byte> > trimmed = ArrayOf<Byte>::Alloc(mSize);
    trimmed->Copy(0, mData, 0, mSize);
    return trimmed;
}

} // namespace Arity
} // namespace Javia
} // namespace Org