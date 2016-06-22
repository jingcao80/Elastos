#include "elastos/droid/google/mms/pdu/CPduPersisterHelper.h"
#include "elastos/droid/google/mms/pdu/PduPersister.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

CAR_INTERFACE_IMPL(CPduPersisterHelper, Singleton, IPduPersisterHelper)
CAR_SINGLETON_IMPL(CPduPersisterHelper)

ECode CPduPersisterHelper::GetPduPersister(
    /* [in] */ IContext* context,
    /* [out] */ IPduPersister** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IPduPersister> pp = PduPersister::GetPduPersister(context);
    *result = pp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CPduPersisterHelper::ConvertUriToPath(
    /* [in] */ IContext* context,
    /* [in] */ IUri* uri,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = PduPersister::ConvertUriToPath(context, uri);
    return NOERROR;
}

ECode CPduPersisterHelper::ToIsoString(
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = PduPersister::ToIsoString(bytes);
    return NOERROR;
}

ECode CPduPersisterHelper::GetBytes(
    /* [in] */ const String& data,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ArrayOf<Byte> > array = PduPersister::GetBytes(data);
    *result = array;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
