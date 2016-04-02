
#include "elastos/droid/server/hdmi/UnmodifiableSparseArray.h"
#include <Elastos.Droid.Utility.h>

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

const String UnmodifiableSparseArray::TAG("ImmutableSparseArray");

ECode UnmodifiableSparseArray::constructor(
    /* [in] */ ISparseArray* array)
{
    mArray = array;
    return NOERROR;
}

ECode UnmodifiableSparseArray::GetSize(
    /* [out] */ Int32* result)
{
    return mArray->GetSize(result);
}

ECode UnmodifiableSparseArray::Get(
    /* [in] */ Int32 key,
    /* [out] */ IInterface** result)
{
    return mArray->Get(key, result);
}

ECode UnmodifiableSparseArray::Get(
    /* [in] */ Int32 key,
    /* [in] */ IInterface* valueIfKeyNotFound,
    /* [out] */ IInterface** result)
{
    return mArray->Get(key, valueIfKeyNotFound, result);
}

ECode UnmodifiableSparseArray::KeyAt(
    /* [in] */ Int32 index,
    /* [out] */ Int32* result)
{
    return mArray->KeyAt(index, result);
}

ECode UnmodifiableSparseArray::ValueAt(
    /* [in] */ Int32 index,
    /* [out] */ IInterface** result)
{
    return mArray->ValueAt(index, result);
}

ECode UnmodifiableSparseArray::IndexOfValue(
    /* [in] */ IInterface* value,
    /* [out] */ Int32* result)
{
    return mArray->IndexOfValue(value, result);
}

ECode UnmodifiableSparseArray::ToString(
    /* [out] */ String* result)
{
    return IObject::Probe(mArray)->ToString(result);
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
