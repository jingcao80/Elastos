
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_DRIVERCALL_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_DRIVERCALL_H__

#include "_Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>

using Elastos::Core::IComparable;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

/**
 * {@hide}
 */
class DriverCall
    : public Object
    , public IComparable
{
public:
    enum State {
        ACTIVE,
        HOLDING,
        DIALING,    // MO call only
        ALERTING,   // MO call only
        INCOMING,   // MT call only
        WAITING    // MT call only
        // If you add a state, make sure to look for the switch()
        // statements that use this enum
    };

public:
    CAR_INTERFACE_DECL()

    /** returns null on error */
    static CARAPI_(AutoPtr<DriverCall>) FromCLCCLine(
        /* [in] */ const String& line);

    DriverCall();

    // @Override
    CARAPI ToString(
        /* [out] */ String* str);

    static CARAPI StateFromCLCC(
        /* [in] */ Int32 state,
        /* [out] */ State* result) /*throws ATParseEx*/;

    static CARAPI PresentationFromCLIP(
        /* [in] */ Int32 cli,
        /* [out] */ Int32* result) /*throws ATParseEx*/;

    //***** Comparable Implementation

    /** For sorting by index */
    // @Override
    CARAPI CompareTo(
        /* [in] */ IInterface* _dc,
        /* [out] */ Int32* result);

public:
    static const String TAG;
    Int32 mIndex;
    Boolean mIsMT;
    State mState;     // May be null if unavail
    Boolean mIsMpty;
    String mNumber;
    Int32 mTOA;
    Boolean mIsVoice;
    Boolean mIsVoicePrivacy;
    Int32 mAls;
    Int32 mNumberPresentation;
    String mName;
    Int32 mNamePresentation;
    AutoPtr<IUUSInfo> mUusInfo;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_DRIVERCALL_H__
