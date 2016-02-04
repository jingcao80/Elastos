
#ifndef __ELASTOS_DROID_MEDIA_FOCUSREQUESTER_H__
#define __ELASTOS_DROID_MEDIA_FOCUSREQUESTER_H__

#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBinder;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Media {

class FocusRequester
    : public Object
    , public IFocusRequester
{
public:
    FocusRequester();

    virtual ~FocusRequester();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ Int32 streamType,
        /* [in] */ Int32 focusRequest,
        /* [in] */ IIAudioFocusDispatcher* afl,
        /* [in] */ IBinder* source,
        /* [in] */ const String& id,
        /* [in] */ IProxyDeathRecipient* hdlr,
        /* [in] */ const String& pn,
        /* [in] */ Int32 uid);

    CARAPI HasSameClient(
        /* [in] */ const String& otherClient,
        /* [out] */ Boolean* result);

    CARAPI HasSameBinder(
        /* [in] */ IBinder* ib,
        /* [out] */ Boolean* result);

    CARAPI HasSamePackage(
        /* [in] */ const String& pack,
        /* [out] */ Boolean* result);

    CARAPI HasSameUid(
        /* [in] */ Int32 uid,
        /* [out] */ Boolean* result);

    CARAPI GetGainRequest(
        /* [out] */ Int32* result);

    CARAPI GetStreamType(
        /* [out] */ Int32* result);

    CARAPI Dump(
        /* [in] */ IPrintWriter* pw);

    CARAPI ReleaseResources();

    CARAPI HandleExternalFocusGain(
        /* [in] */ Int32 focusGain);

    CARAPI HandleFocusGain(
        /* [in] */ Int32 focusGain);

    CARAPI HandleFocusLoss(
        /* [in] */ Int32 focusLoss);

private:
    static CARAPI_(String) FocusChangeToString(
        /* [in] */ Int32 focus);

    CARAPI_(String) FocusGainToString();

    CARAPI_(String) FocusLossToString();

    /**
     * For a given audio focus gain request, return the audio focus loss type that will result
     * from it, taking into account any previous focus loss.
     * @param gainRequest
     * @return the audio focus loss type that matches the gain request
     */
    CARAPI_(Int32) FocusLossForGainRequest(
        /* [in] */ Int32 gainRequest);

private:
    // on purpose not using this classe's name, as it will only be used from MediaFocusControl
    static String TAG;
    static Boolean DEBUG;

    AutoPtr<IProxyDeathRecipient> mDeathHandler;
    AutoPtr<IIAudioFocusDispatcher> mFocusDispatcher; // may be null
    AutoPtr<IBinder> mSourceRef;
    String mClientId;
    String mPackageName;
    Int32 mCallingUid;
    /**
     * the audio focus gain request that caused the addition of this object in the focus stack.
     */
    Int32 mFocusGainRequest;
    /**
     * the audio focus loss received my mFocusDispatcher, is AudioManager.AUDIOFOCUS_NONE if
     *  it never lost focus.
     */
    Int32 mFocusLossReceived;
    /**
     * the stream type associated with the focus request
     */
    Int32 mStreamType;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_FOCUSREQUESTER_H__
