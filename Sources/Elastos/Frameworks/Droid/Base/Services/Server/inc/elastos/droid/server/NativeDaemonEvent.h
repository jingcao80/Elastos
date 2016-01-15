#ifndef __ELASTOS_DROID_SERVER_NATIVEDAEMONEVENT_H__
#define __ELASTOS_DROID_SERVER_NATIVEDAEMONEVENT_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Server {

extern "C" const InterfaceID EIID_NativeDaemonEvent;

/**
 * Parsed event from native side of {@link NativeDaemonConnector}.
 */
class NativeDaemonEvent
    : public ElRefBase
    , public IInterface
{
public:
    CAR_INTERFACE_DECL()

    CARAPI_(Int32) GetCmdNumber();

    CARAPI_(Int32) GetCode();

    CARAPI_(String) GetMessage();

    CARAPI_(String) GetRawEvent();

    CARAPI_(String) ToString();

    CARAPI_(Boolean) IsClassContinue();

    CARAPI_(Boolean) IsClassOk();

    CARAPI_(Boolean) IsClassServerError();

    CARAPI_(Boolean) IsClassClientError();

    CARAPI_(Boolean) IsClassUnsolicited();

    CARAPI CheckCode(
        /* [in] */ Int32 code);

    static CARAPI ParseRawEvent(
        /* [in] */ const String& rawEvent,
        /* [out] */ NativeDaemonEvent** event);

    static CARAPI_(AutoPtr< ArrayOf<String> >) FilterMessageList(
        /* [in] */ const ArrayOf<NativeDaemonEvent*>& events,
        /* [in] */ Int32 matchCode);

    CARAPI_(String) GetField(
        /* [in] */ Int32 n);

    static CARAPI_(AutoPtr<ArrayOf<String> >) UnescapeArgs(
        /* [in] */ const String& rawEvent);

private:
    NativeDaemonEvent(
        /* [in] */ Int32 cmdNumber,
        /* [in] */ Int32 code,
        /* [in] */ const String& message,
        /* [in] */ const String& rawEvent);

    static CARAPI_(Boolean) IsClassUnsolicited(
        /* [in] */ Int32 code);

private:
    Int32 mCmdNumber;
    Int32 mCode;
    String mMessage;
    String mRawEvent;
    AutoPtr< ArrayOf<String> > mParsed;
};

} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_NATIVEDAEMONEVENT_H__
