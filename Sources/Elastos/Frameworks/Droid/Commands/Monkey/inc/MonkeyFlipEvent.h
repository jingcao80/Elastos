#ifndef __MONKEYFLIPEVENT_H__
#define __MONKEYFLIPEVENT_H__

#include "MonkeyEvent.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

class MonkeyFlipEvent : public MonkeyEvent
{
public:

protected:
    MonkeyFlipEvent();

    MonkeyFlipEvent(
        /* [in] */ Boolean isKeyboardOpen);

    virtual CARAPI_(Int32) InjectEvent(
        /* [in] */ IIWindowManager *iwm,
        /* [in] */ IIActivityManager *iam,
        /* [in] */ Int32 verbose);

    CARAPI Init(
        /* [in] */ Boolean isKeyboardOpen);

private:
    const volatile Boolean mIsKeyBoardOpen;
    static const Byte mFlip0[16];
    static const Byte mFlip1[16];
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif //__MONKEYFLIPEVENT_H__
