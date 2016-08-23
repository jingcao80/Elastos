
#ifndef __ELASTOS_DROID_CONTACTS_COMMON_UTIL_WEAKASYNCTASK_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_UTIL_WEAKASYNCTASK_H__

#include "elastos/droid/os/AsyncTask.h"

using Elastos::Droid::Os::AsyncTask;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Util {

/** Static methods that are useful for scheduling actions to occur at a later time. */
class WeakAsyncTask
    : public AsyncTask
{
public:
    WeakAsyncTask(
        /* [in] */ IInterface* target);

    virtual ~WeakAsyncTask() {}

protected:
    /** {@inheritDoc} */
    // @Override
    CARAPI OnPreExecute();

    /** {@inheritDoc} */
    // @Override
    CARAPI DoInBackground(
        /* [in] */ ArrayOf<IInterface*>* params,
        /* [out] */ IInterface** result);

    /** {@inheritDoc} */
    // @Override
    CARAPI OnPostExecute(
        /* [in] */ IInterface* result);

    CARAPI OnPreExecute(
        /* [in] */ IInterface* target);

    virtual CARAPI DoInBackground(
        /* [in] */ IInterface* target,
        /* [in] */ ArrayOf<IInterface*>* params,
        /* [out] */ IInterface** result) = 0;

    CARAPI OnPostExecute(
        /* [in] */ IInterface* target,
        /* [in] */ IInterface* result);

protected:
    AutoPtr<IWeakReference> mTarget;
};

} // namespace Util
} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_UTIL_WEAKASYNCTASK_H__
