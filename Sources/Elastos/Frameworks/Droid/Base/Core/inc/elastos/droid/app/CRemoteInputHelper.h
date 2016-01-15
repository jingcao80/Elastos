
#ifndef __ELASTOS_DROID_APP_CREMOTE_INPUT_HELPER_H__
#define __ELASTOS_DROID_APP_CREMOTE_INPUT_HELPER_H__

#include "_Elastos_Droid_App_CRemoteInputHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CRemoteInputHelper)
    , public Singleton
    , public IRemoteInputHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Get the remote input results bundle from an intent. The returned Bundle will
     * contain a key/value for every result key populated by remote input collector.
     * Use the {@link Bundle#getCharSequence(String)} method to retrieve a value.
     * @param intent The intent object that fired in response to an action or content intent
     *               which also had one or more remote input requested.
     */
    CARAPI GetResultsFromIntent(
        /* [in] */ IIntent* intent,
        /* [out] */ IBundle** bundle);

    /**
     * Populate an intent object with the results gathered from remote input. This method
     * should only be called by remote input collection services when sending results to a
     * pending intent.
     * @param remoteInputs The remote inputs for which results are being provided
     * @param intent The intent to add remote inputs to. The {@link ClipData}
     *               field of the intent will be modified to contain the results.
     * @param results A bundle holding the remote input results. This bundle should
     *                be populated with keys matching the result keys specified in
     *                {@code remoteInputs} with values being the result per key.
     */
    CARAPI AddResultsToIntent(
        /* [in] */ ArrayOf<IRemoteInput*>* remoteInputs,
        /* [in] */ IIntent* intent,
        /* [in] */ IBundle* results);
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CREMOTE_INPUT_HELPER_H__
