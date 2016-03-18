
#ifndef __ELASTOS_DROID_SERVER_HDMI_REQUESTARCINITIATIONACTION_H__
#define __ELASTOS_DROID_SERVER_HDMI_REQUESTARCINITIATIONACTION_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>
#include "elastos/droid/server/hdmi/RequestArcAction.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * Feature action that handles ARC action initiated by TV devices.
 *
 * <p>This action is created by TV's hot plug event of ARC port.
 */
class RequestArcInitiationAction
    : public RequestArcAction
    , public IRequestArcInitiationAction
{
private:
    class InnerSub_HdmiControlServiceSendMessageCallback
        : public Object
        , public IHdmiControlServiceSendMessageCallback
    {
    public:
        CAR_INTERFACE_DECL()

        InnerSub_HdmiControlServiceSendMessageCallback(
            /* [in] */ RequestArcInitiationAction* host);

        //@Override
        CARAPI OnSendCompleted(
            /* [in] */ Int32 error);

    private:
        RequestArcInitiationAction* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    /**
     * @Constructor
     *
     * For more details look at {@link RequestArcAction#RequestArcAction}.
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

#endif // __ELASTOS_DROID_SERVER_HDMI_REQUESTARCINITIATIONACTION_H__
