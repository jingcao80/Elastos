
#ifndef __ELASTOS_DROID_SERVER_HDMI_REQUESTARCTERMINATIONACTION_H__
#define __ELASTOS_DROID_SERVER_HDMI_REQUESTARCTERMINATIONACTION_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>
#include "elastos/droid/server/hdmi/RequestArcAction.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * Feature action to handle <Request ARC Termination>.
 *
 * <p>It's initiated by user's manual termination or ARC channel close from TV.
 */
class RequestArcTerminationAction
    : public RequestArcAction
    , public IRequestArcTerminationAction
{
private:
    class InnerSub_SendMessageCallback
        : public Object
        , public IHdmiControlServiceSendMessageCallback
    {
    public:
        CAR_INTERFACE_DECL()

        InnerSub_SendMessageCallback(
            /* [in] */ RequestArcTerminationAction* host);

        //@Override
        CARAPI OnSendCompleted(
            /* [in] */ Int32 error);

    private:
        RequestArcTerminationAction* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    /**
     * @Constructor
     *
     * @see RequestArcAction#RequestArcAction
     */
    CARAPI constructor(
        /* [in] */ IHdmiCecLocalDevice* source,
        /* [in] */ Int32 avrAddress);

    // @Override
    CARAPI Start(
        /* [out] */ Boolean* result);

private:
    static const String TAG;
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_REQUESTARCTERMINATIONACTION_H__
