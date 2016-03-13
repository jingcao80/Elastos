
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
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
       mArray = array;

#endif
}

ECode UnmodifiableSparseArray::GetSize(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mArray->GetSize(result);
#endif
}

ECode UnmodifiableSparseArray::Get(
    /* [in] */ Int32 key,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)

    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        AutoPtr<IInterface> obj;
        mArray->Get(key, (IInterface**)&obj);
        return I::Probe(obj);
#endif
}

ECode UnmodifiableSparseArray::Get(
    /* [in] */ Int32 key,
    /* [in] */ IInterface* valueIfKeyNotFound,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)

    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mArray->Get(key, valueIfKeyNotFound);
#endif
}

ECode UnmodifiableSparseArray::KeyAt(
    /* [in] */ Int32 index,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mArray->KeyAt(index);
#endif
}

ECode UnmodifiableSparseArray::ValueAt(
    /* [in] */ Int32 index,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)

    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mArray->ValueAt(index);
#endif
}

ECode UnmodifiableSparseArray::IndexOfValue(
    /* [in] */ IInterface* value,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mArray->IndexOfValue(value);
#endif
}

ECode UnmodifiableSparseArray::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mArray->ToString();
#endif
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
