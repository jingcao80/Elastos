
#include "elastos/droid/contacts/common/util/WeakAsyncTask.h"

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Util {

WeakAsyncTask::WeakAsyncTask(
    /* [in] */ IInterface* target)
{
    AutoPtr<IWeakReferenceSource> wrs = IWeakReferenceSource::Probe(target);
    wrs->GetWeakReference((IWeakReference**)&mTarget);
}

ECode WeakAsyncTask::OnPreExecute()
{
    AutoPtr<IInterface> target;
    mTarget->Resolve(EIID_IInterface, (IInterface**)&target);
    if (target != NULL) {
        return OnPreExecute(target);
    }
    return NOERROR;
}

ECode WeakAsyncTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    AutoPtr<IInterface> target;
    mTarget->Resolve(EIID_IInterface, (IInterface**)&target);
    if (target != NULL) {
        return DoInBackground(target, params, result);
    }
    return NOERROR;
}

ECode WeakAsyncTask::OnPostExecute(
    /* [in] */ IInterface* result)
{
    AutoPtr<IInterface> target;
    mTarget->Resolve(EIID_IInterface, (IInterface**)&target);
    if (target != NULL) {
        return OnPostExecute(target, result);
    }
    return NOERROR;
}

ECode WeakAsyncTask::OnPreExecute(
    /* [in] */ IInterface* target)
{
    // No default action
    return NOERROR;
}

ECode WeakAsyncTask::OnPostExecute(
    /* [in] */ IInterface* target,
    /* [in] */ IInterface* result)
{
    // No default action
    return NOERROR;
}

} // namespace Util
} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos
