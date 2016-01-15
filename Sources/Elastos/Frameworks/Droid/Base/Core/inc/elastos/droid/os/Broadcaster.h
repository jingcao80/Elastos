#ifndef __ELASTOS_DROID_OS_BROADCASTER_H__
#define __ELASTOS_DROID_OS_BROADCASTER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Os {

/** @hide */
class Broadcaster
    : public Object
{
public:
    Broadcaster();

    /**
     *  Sign up for notifications about something.
     *
     *  When this broadcaster pushes a message with senderWhat in the what field,
     *  target will be sent a copy of that message with targetWhat in the what field.
     */
    CARAPI_(Void) Request(
        /* [in] */ Int32 senderWhat,
        /* [in] */ IHandler* target,
        /* [in] */ Int32 targetWhat);

    /**
     * Unregister for notifications for this senderWhat/target/targetWhat tuple.
     */
    CARAPI_(Void) CancelRequest(
        /* [in] */ Int32 senderWhat,
        /* [in] */ IHandler* target,
        /* [in] */ Int32 targetWhat);

    /**
     * For debugging purposes, print the registrations to System.out
     */
    CARAPI_(Void) DumpRegistrations();

    /**
     * Send out msg.  Anyone who has registered via the request() method will be
     * sent the message.
     */
    CARAPI_(Void) Broadcast(
        /* [in] */ IMessage* msg);

private:
    class Registration : public Object
    {
    public:
        Registration();

        AutoPtr<Registration> mNext;
        AutoPtr<Registration> mPrev;

        Int32 mSenderWhat;
        AutoPtr<ArrayOf<IHandler*> > mTargets;
        AutoPtr<ArrayOf<Int32> > mTargetWhats;
    };

    AutoPtr<Registration> mReg;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_BROADCASTER_H__
