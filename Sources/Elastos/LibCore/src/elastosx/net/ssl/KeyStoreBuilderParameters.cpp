
#include "Elastos.CoreLibrary.Security.h"
#include "KeyStoreBuilderParameters.h"
#include "Collections.h"

using Elastos::Utility::Collections;

namespace Elastosx {
namespace Net {
namespace Ssl {

CAR_INTERFACE_IMPL_2(KeyStoreBuilderParameters, Object, IKeyStoreBuilderParameters, IManagerFactoryParameters)

ECode KeyStoreBuilderParameters::constructor(
    /* [in] */ IKeyStoreBuilder* builder)
{
    if (builder == NULL) {
        //throw new NullPointerException("builder == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    return Collections::SingletonList(TO_IINTERFACE(builder), (IList**)&mKsbuilders);
}

ECode KeyStoreBuilderParameters::constructor(
    /* [in] */ IList* parameters)
{
    if (parameters == NULL) {
        //throw new NullPointerException("parameters == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    Boolean result;
    if (parameters->IsEmpty(&result), result) {
        //throw new IllegalArgumentException("parameters.isEmpty()");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return Collections::UnmodifiableList(parameters, (IList**)&mKsbuilders);
}

ECode KeyStoreBuilderParameters::GetParameters(
    /* [out] */ IList** param)
{
    VALIDATE_NOT_NULL(param)

    *param = mKsbuilders;
    REFCOUNT_ADD(*param);
    return NOERROR;
}

} // namespace Ssl
} // namespace Net
} // namespace Elastosx
