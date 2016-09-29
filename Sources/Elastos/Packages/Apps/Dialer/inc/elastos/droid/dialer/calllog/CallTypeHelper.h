
#ifndef __ELASTOS_DROID_DIALER_CALLLOG_CALLTYPEHELPER_H__
#define __ELASTOS_DROID_DIALER_CALLLOG_CALLTYPEHELPER_H__

#include "_Elastos.Droid.Dialer.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Core::IInteger32;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

/**
 * Helper class to perform operations related to call types.
 */
class CallTypeHelper : public Object
{
public:
    CallTypeHelper(
        /* [in] */ IResources* resources);

    /** Returns the text used to represent the given call type. */
    CARAPI_(AutoPtr<ICharSequence>) GetCallTypeText(
        /* [in] */ Int32 callType,
        /* [in] */ Boolean isVideoCall,
        /* [out] */ ICharSequence** text);

    /** Returns the color used to highlight the given call type, null if not highlight is needed. */
    CARAPI_(AutoPtr<IInteger32>) GetHighlightedColor(
        /* [in] */ Int32 callType);

    static CARAPI_(Boolean) IsMissedCallType(
        /* [in] */ Int32 callType);

private:
    /** Name used to identify incoming calls. */
    String mIncomingName;
    /** Name used to identify outgoing calls. */
    String mOutgoingName;
    /** Name used to identify missed calls. */
    String mMissedName;
    /** Name used to identify incoming video calls. */
    String mIncomingVideoName;
    /** Name used to identify outgoing video calls. */
    String mOutgoingVideoName;
    /** Name used to identify missed video calls. */
    String mMissedVideoName;
    /** Name used to identify voicemail calls. */
    String mVoicemailName;
    /** Color used to identify new missed calls. */
    Int32 mNewMissedColor;
    /** Color used to identify new voicemail calls. */
    Int32 mNewVoicemailColor;
};

} // CallLog
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_CALLLOG_CALLTYPEHELPER_H__
