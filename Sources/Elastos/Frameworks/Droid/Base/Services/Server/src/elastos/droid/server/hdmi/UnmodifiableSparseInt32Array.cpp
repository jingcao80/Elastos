
#include "elastos/droid/server/hdmi/UnmodifiableSparseInt32Array.h"
#include <Elastos.Droid.Utility.h>

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

const String UnmodifiableSparseInt32Array::TAG("ImmutableSparseIntArray");

ECode UnmodifiableSparseInt32Array::constructor(
    /* [in] */ ISparseInt32Array* array)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mArray = array;

#endif
}

ECode UnmodifiableSparseInt32Array::GetSize(
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mArray->Size();
#endif
}

ECode UnmodifiableSparseInt32Array::Get(
    /* [in] */ Int32 key,
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mArray->Get(key);
#endif
}

ECode UnmodifiableSparseInt32Array::Get(
    /* [in] */ Int32 key,
    /* [in] */ Int32 valueIfKeyNotFound,
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mArray->Get(key, valueIfKeyNotFound);
#endif
}

ECode UnmodifiableSparseInt32Array::KeyAt(
    /* [in] */ Int32 index,
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mArray->KeyAt(index);
#endif
}

ECode UnmodifiableSparseInt32Array::ValueAt(
    /* [in] */ Int32 index,
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mArray->ValueAt(index);
#endif
}

ECode UnmodifiableSparseInt32Array::IndexOfValue(
    /* [in] */ Int32 value,
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mArray->IndexOfValue(value);
#endif
}

ECode UnmodifiableSparseInt32Array::ToString(
    /* [out] */ String* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mArray->ToString();
#endif
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
