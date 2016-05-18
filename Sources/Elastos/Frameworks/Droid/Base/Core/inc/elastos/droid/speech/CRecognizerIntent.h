
#ifndef __ELASTOS_DROID_SPEECH_CRecognizerIntent_H__
#define __ELASTOS_DROID_SPEECH_CRecognizerIntent_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/core/Singleton.h"
#include "_Elastos_Droid_Speech_CRecognizerIntent.h"

using Elastos::Droid::Speech::IRecognizerIntent;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::App::IActivity;

namespace Elastos {
namespace Droid {
namespace Speech {

CarClass(CRecognizerIntent)
    , public Singleton
    , public IRecognizerIntent
{
public:
    CAR_SINGLETON_DECL();

    CAR_INTERFACE_DECL()

    /**
     * Returns the broadcast intent to fire with
     * {@link Context#sendOrderedBroadcast(Intent, String, BroadcastReceiver, android.os.Handler, int, String, Bundle)}
     * to receive details from the package that implements voice search.
     * <p>
     * This is based on the value specified by the voice search {@link Activity} in
     * {@link #DETAILS_META_DATA}, and if this is not specified, will return null. Also if there
     * is no chosen default to resolve for {@link #ACTION_WEB_SEARCH}, this will return null.
     * <p>
     * If an intent is returned and is fired, a {@link Bundle} of extras will be returned to the
     * provided result receiver, and should ideally contain values for
     * {@link #EXTRA_LANGUAGE_PREFERENCE} and {@link #EXTRA_SUPPORTED_LANGUAGES}.
     * <p>
     * (Whether these are actually provided is up to the particular implementation. It is
     * recommended that {@link Activity}s implementing {@link #ACTION_WEB_SEARCH} provide this
     * information, but it is not required.)
     *
     * @param context a context object
     * @return the broadcast intent to fire or null if not available
     */
    //public static final
    CARAPI GetVoiceDetailsIntent(
        /* [in] */ IContext* pContext,
        /* [out] */ IIntent** ppRet);
};

} // namespace Speech
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SPEECH_CRecognizerIntent_H__
