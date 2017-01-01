//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ORG_ALLJOYN_BUS_SIGNALEMITTER_H__
#define __ORG_ALLJOYN_BUS_SIGNALEMITTER_H__

#include "_Org.Alljoyn.Bus.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Core::Reflect::IInvocationHandler;

namespace Org {
namespace Alljoyn {
namespace Bus {

/**
 * A helper proxy used by BusObjects to send signals.  A SignalEmitter
 * instance can be used to send any signal from a given AllJoyn interface.
 */
class SignalEmitter
    : public Object
    , public ISignalEmitter
{
private:
    class Emitter
        : public Object
        , public IInvocationHandler
    {
    public:
        CAR_INTERFACE_DECL()

        Emitter(
            /* [in] */ SignalEmitter* emitter);

        CARAPI Invoke(
            /* [in] */ IInterface* proxy,
            /* [in] */ IMethodInfo* method,
            /* [in] */ IArgumentList* args);

    private:
        AutoPtr<SignalEmitter> mSignalEmitter;   // hold host
    };

public:
    CAR_INTERFACE_DECL()

    SignalEmitter();

    virtual ~SignalEmitter();

    /**
     * Constructs a SignalEmitter.
     *
     * @param source the source object of any signals sent from this emitter
     * @param destination well-known or unique name of destination for signal
     * @param sessionId A unique SessionId for this AllJoyn session instance,
     *                  or BusAttachment.SESSION_ID_ALL_HOSTED to emit on all
     *                  sessions hosted by the BusAttachment.
     * @param globalBroadcast whether to forward broadcast signals
     *                        across bus-to-bus connections
     */
    CARAPI constructor(
        /* [in] */ IBusObject* source,
        /* [in] */ const String& destination,
        /* [in] */ Int32 sessionId,
        /* [in] */ GlobalBroadcast globalBroadcast);

    CARAPI constructor(
        /* [in] */ IBusObject* source,
        /* [in] */ Int32 sessionId,
        /* [in] */ GlobalBroadcast globalBroadcast);

    CARAPI constructor(
        /* [in] */ IBusObject* source,
        /* [in] */ GlobalBroadcast globalBroadcast);

    CARAPI constructor(
        /* [in] */ IBusObject* source);

    /**
     * Sets the time-to-live of future signals sent from this emitter.
     *
     * @param timeToLive if non-zero this specifies the useful lifetime for signals sent
     *                   by this emitter. The units are in milliseconds for
     *                   non-sessionless signals and seconds for sessionless signals. If
     *                   delivery of the signal is delayed beyond the timeToLive due to
     *                   network congestion or other factors the signal may be
     *                   discarded. There is no guarantee that expired signals will not
     *                   still be delivered.
     */
    CARAPI SetTimeToLive(
        /* [in] */ Int32 timeToLive);

    /**
     * Enables header compression of future signals sent from this emitter.
     *
     * @param compress if {@code true} compress header for destinations that can handle
     *                 header compression
     *
     * @deprecated March 2015 for 15.04 release
     */
    //@Deprecated
    CARAPI SetCompressHeader(
        /* [in] */ Boolean compress);

    /**
     * Sets the signal to be sent out as a sessionless signal
     *
     * @param sessionless if {@code true} the signal is set to be sent out as a sessionless
     *                       signal
     *
     */
    CARAPI SetSessionlessFlag(
        /* [in] */ Boolean sessionless);

    /**
     * Get the MessageContext of the last signal sent from this emitter.
     *
     * @return  MessageContext of the last signal sent from this emitter.
     */
    CARAPI GetMessageContext(
        /* [out] */ IMessageContext** ctx);

    /**
     * Cancel a sessionless signal sent from this SignalEmitter
     *
     * @param serialNum   Serial number of message to cancel
     * @return
     * <ul>
     * <li>OK if request completed successfully.</li>
     * <li>BUS_NO_SUCH_MESSAGE if message with given serial number could not be located.</li>
     * <li>BUS_NOT_ALLOWED if message with serial number was sent by a different sender.</li>
     * </ul>
     */
    CARAPI CancelSessionlessSignal(
        /* [in] */ Int32 serialNum);

    /**
     * Gets a proxy to the interface that emits signals.
     *
     * @param <T> any class implementation of a interface annotated with AllJoyn interface annotations
     * @param intf the interface of the bus object that emits the signals
     *
     * @return the proxy implementing the signal emitter
     */
    CARAPI GetInterface(
        /* [in] */ const InterfaceID& intf,
        /* [out] */ IInterface** obj);

private:
    /** Sends the signal. */
    CARAPI Signal(
        /* [in] */ IBusObject* busObj,
        /* [in] */ const String& destination,
        /* [in] */ Int32 sessionId,
        /* [in] */ const String& ifaceName,
        /* [in] */ IMethodInfo* methodInfo,
        /* [in] */ IArgumentList* args,
        /* [in] */ Int32 timeToLive,
        /* [in] */ Int32 flags,
        /* [in] */ IMessageContext* ctx);

    ECode CancelSessionlessSignal(
        /* [in] */ IBusObject* busObject,
        /* [in] */ Int32 serialNum);

protected:
    AutoPtr<IBusObject> mSource;

private:
    friend class Emitter;

    static Int32 GLOBAL_BROADCAST;
    static Int32 SESSIONLESS;

    String mDestination;
    Int32 mSessionId;
    Int32 mTimeToLive;
    Int32 mFlags;
    AutoPtr<IProxy> mProxy;
    AutoPtr<IMessageContext> mMsgContext;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_SIGNALEMITTER_H__
