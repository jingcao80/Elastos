
#ifndef __ELASTOS_APPS_DIALER_CALLLOG_CALLTYPEHELPER_H__
#define __ELASTOS_APPS_DIALER_CALLLOG_CALLTYPEHELPER_H__

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace CallLog {

/**
 * Helper class to perform operations related to call types.
 */
class CallTypeHelper
    : public Object
    , public ICallTypeHelper
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IResources* resources);

    /** Returns the text used to represent the given call type. */
    CARAPI GetCallTypeText(
        /* [in] */ Int32 callType,
        /* [in] */ Boolean isVideoCall,
        /* [out] */ ICharSequence** text);

    /** Returns the color used to highlight the given call type, null if not highlight is needed. */
    CARAPI GetHighlightedColor(
        /* [in] */ Int32 callType,
        /* [out] */ IInteger32** result);

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
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_CALLLOG_CALLTYPEHELPER_H__
