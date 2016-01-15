
#include "Elastos.Droid.Content.h"
#include "elastos/droid/os/Broadcaster.h"
#include "elastos/droid/os/CMessage.h"
#include <elastos/core/AutoLock.h>

namespace Elastos {
namespace Droid {
namespace Os {


Broadcaster::Registration::Registration()
    : mSenderWhat(0)
{
}

Broadcaster::Broadcaster()
{
}

Void Broadcaster::Request(
    /* [in] */ Int32 senderWhat,
    /* [in] */ IHandler* target,
    /* [in] */ Int32 targetWhat)
{
    AutoLock lock(this);

    AutoPtr<Registration> r;
    if (mReg == NULL) {
        r = new Registration();
        r->mSenderWhat = senderWhat;
        r->mTargets = ArrayOf<IHandler*>::Alloc(1);
        r->mTargetWhats = ArrayOf<Int32>::Alloc(1);
        r->mTargets->Set(0, target);
        r->mTargetWhats->Set(0, targetWhat);
        mReg = r;
        r->mNext = r;
        r->mPrev = r;
    }
    else {
        // find its place in the map
        AutoPtr<Registration> start = mReg;
        r = start;
        do {
            if (r->mSenderWhat >= senderWhat) {
                break;
            }
            r = r->mNext;
        }
        while (r != start);

        Int32 n;
        if (r->mSenderWhat != senderWhat) {
            // we didn't find a senderWhat match, but r is right
            // after where it goes
            AutoPtr<Registration> reg = new Registration();
            reg->mSenderWhat = senderWhat;
            reg->mTargets = ArrayOf<IHandler*>::Alloc(1);
            reg->mTargetWhats = ArrayOf<Int32>::Alloc(1);
            reg->mNext = r;
            reg->mPrev = r->mPrev;
            r->mPrev->mNext = reg;
            r->mPrev = reg;

            if (r == mReg && r->mSenderWhat > reg->mSenderWhat) {
                mReg = reg;
            }

            r = reg;
            n = 0;
        }
        else {
            n = r->mTargets->GetLength();
            AutoPtr<ArrayOf<IHandler*> > oldTargets = r->mTargets;
            AutoPtr<ArrayOf<Int32> > oldWhats = r->mTargetWhats;
            // check for duplicates, and don't do it if we are dup.
            for (Int32 i=0; i<n; i++) {
                if ((*oldTargets)[i] == target && (*oldWhats)[i] == targetWhat) {
                    return;
                }
            }
            r->mTargets = ArrayOf<IHandler*>::Alloc(n + 1);
            r->mTargets->Copy(oldTargets, 0, n);
            r->mTargetWhats = ArrayOf<Int32>::Alloc(n + 1);
            r->mTargetWhats->Copy(oldWhats, 0, n);
        }
        r->mTargets->Set(n, target);
        r->mTargetWhats->Set(n, targetWhat);
    }
}

Void Broadcaster::CancelRequest(
    /* [in] */ Int32 senderWhat,
    /* [in] */ IHandler* target,
    /* [in] */ Int32 targetWhat)
{
    AutoLock lock(this);

    AutoPtr<Registration> start = mReg;
    AutoPtr<Registration> r = start;

    if (r == NULL) {
        return;
    }

    do {
        if (r->mSenderWhat >= senderWhat) {
            break;
        }
        r = r->mNext;
    }
    while (r != start);

    if (r->mSenderWhat == senderWhat) {
        AutoPtr<ArrayOf<IHandler*> > targets = r->mTargets;
        AutoPtr<ArrayOf<Int32> > whats = r->mTargetWhats;
        Int32 oldLen = targets->GetLength();
        for (Int32 i=0; i<oldLen; i++) {
            if ((*targets)[i] == target && (*whats)[i] == targetWhat) {
                r->mTargets = ArrayOf<IHandler*>::Alloc(oldLen-1);
                r->mTargetWhats = ArrayOf<Int32>::Alloc(oldLen-1);
                if (i > 0) {
                    r->mTargets->Copy(targets, 0, i);
                    r->mTargetWhats->Copy(whats, 0, i);
                }

                Int32 remainingLen = oldLen-i-1;
                if (remainingLen != 0) {
                    r->mTargets->Copy(i, targets, 0 + 1, remainingLen);
                    r->mTargetWhats->Copy(i, whats, 0 + 1, remainingLen);
                }
                break;
            }
        }
    }
}

Void Broadcaster::DumpRegistrations()
{
    // synchronized(this) {
    //     Registration start = mReg;
    //     System.out.println("Broadcaster " + this + " {");
    //     if (start != NULL) {
    //         Registration r = start;
    //         do {
    //             System.out.println("    senderWhat=" + r->mSenderWhat);
    //             Int32 n = r->mTargets->GetLength();
    //             for (Int32 i=0; i<n; i++) {
    //                 System.out.println("        [" + r->mTargetWhats[i]
    //                                 + "] " + r->mTargets[i]);
    //             }
    //             r = r->mNext;
    //         } while (r != start);
    //     }
    //     System.out.println("}");
    // }
}

Void Broadcaster::Broadcast(
    /* [in] */ IMessage* msg)
{
    AutoLock lock(this);

    if (mReg == NULL) {
        return;
    }

    Int32 senderWhat;
    msg->GetWhat(&senderWhat);
    AutoPtr<Registration> start = mReg;
    AutoPtr<Registration> r = start;
    do {
        if (r->mSenderWhat >= senderWhat) {
            break;
        }
        r = r->mNext;
    } while (r != start);

    if (r->mSenderWhat == senderWhat) {
        AutoPtr<ArrayOf<IHandler*> > targets = r->mTargets;
        AutoPtr<ArrayOf<Int32> > whats = r->mTargetWhats;
        Int32 n = targets->GetLength();
        Boolean result;
        for (Int32 i=0; i<n; i++) {
            AutoPtr<IHandler> target = (*targets)[i];
            AutoPtr<IMessage> m = CMessage::Obtain();
            m->CopyFrom(msg);
            m->SetWhat((*whats)[i]);
            target->SendMessage(m, &result);
        }
    }
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
