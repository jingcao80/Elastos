#ifndef __ELASTOS_DROID_VIEW_INPUTEVENT_H__
#define __ELASTOS_DROID_VIEW_INPUTEVENT_H__

#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Handler.h"

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::View::IIInputFilterHost;

namespace Elastos {
namespace Droid {
namespace View {

class ECO_PUBLIC InputFilter
    : public Object
    , public IInputFilter
    , public IIInputFilter
    , public IBinder
{
private:
    class ECO_LOCAL H
        : public Handler
    {
    public:
        TO_STRING_IMPL("InputFilter::H")

        H(
            /* [in] */ ILooper* loop,
            /* [in] */ InputFilter* host);

        virtual CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        InputFilter* mHost;
    };

public:
    InputFilter();

    CAR_INTERFACE_DECL()

    /**
     * Creates the input filter.
     *
     * @param looper The looper to run callbacks on.
     */
    CARAPI constructor(
        /* [in] */ ILooper* loop);

    virtual ~InputFilter();

    /**
     * Called when the input filter is installed.
     * This method is guaranteed to be non-reentrant.
     *
     * @param host The input filter host environment.
     */
    CARAPI Install(
        /* [in] */ IIInputFilterHost* host);

    /**
     * Called when the input filter is uninstalled.
     * This method is guaranteed to be non-reentrant.
     */
    CARAPI Uninstall();

    /**
     * Called to enqueue the input event for filtering.
     * The event will be recycled after the input filter processes it.
     * This method is guaranteed to be non-reentrant.
     *
     * @param event The input event to enqueue.
     */
    CARAPI FilterInputEvent(
        /* [in] */ IInputEvent* event,
        /* [in] */ Int32 policyFlags);

    /**
     * Sends an input event to the dispatcher.
     *
     * @param event The input event to publish.
     * @param policyFlags The input event policy flags.
     */
    CARAPI SendInputEvent(
        /* [in] */ IInputEvent* event,
        /* [in] */ Int32 policyFlags);

    /**
     * Called when an input event has been received from the dispatcher.
     * <p>
     * The default implementation sends the input event back to the dispatcher, unchanged.
     * </p><p>
     * The event will be recycled when this method returns.  If you want to keep it around,
     * make a copy!
     * </p>
     *
     * @param event The input event that was received.
     * @param policyFlags The input event policy flags.
     */
    CARAPI OnInputEvent(
        /* [in] */ IInputEvent* event,
        /* [in] */ Int32 policyFlags);

    /**
     * Called when the filter is installed into the dispatch pipeline.
     * <p>
     * This method is called before the input filter receives any input events.
     * The input filter should take this opportunity to prepare itself.
     * </p>
     */
    CARAPI OnInstalled();

    /**
     * Called when the filter is uninstalled from the dispatch pipeline.
     * <p>
     * This method is called after the input filter receives its last input event.
     * The input filter should take this opportunity to clean up.
     * </p>
     */
    CARAPI OnUninstalled();

private:
    static const Int32 MSG_INSTALL;// = 1;
    static const Int32 MSG_UNINSTALL;// = 2;
    static const Int32 MSG_INPUT_EVENT;// = 3;

    // Consistency verifiers for debugging purposes.
    // private final InputEventConsistencyVerifier mInboundInputEventConsistencyVerifier =
    //         InputEventConsistencyVerifier.isInstrumentationEnabled() ?
    //                 new InputEventConsistencyVerifier(this,
    //                         InputEventConsistencyVerifier.FLAG_RAW_DEVICE_INPUT,
    //                         "InputFilter#InboundInputEventConsistencyVerifier") : null;
    // private final InputEventConsistencyVerifier mOutboundInputEventConsistencyVerifier =
    //         InputEventConsistencyVerifier.isInstrumentationEnabled() ?
    //                 new InputEventConsistencyVerifier(this,
    //                         InputEventConsistencyVerifier.FLAG_RAW_DEVICE_INPUT,
    //                         "InputFilter#OutboundInputEventConsistencyVerifier") : null;

    AutoPtr<H> mH;
    AutoPtr<IIInputFilterHost> mInputFilterHost;

};

}   //namespace View
}   //namespace Droid
}   //namespace Elastos

#endif //__ELASTOS_DROID_VIEW_INPUTEVENT_H__
