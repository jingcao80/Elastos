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

#ifndef __MONKEYSOURCENETWORK_H__
#define __MONKEYSOURCENETWORK_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/List.h>
#include <elastos/core/Object.h>
#include <elautoptr.h>
#include "CMonkeyFlipEvent.h"
#include "CMonkeyTouchEvent.h"
#include "CMonkeyTrackballEvent.h"
#include "CMonkeyKeyEvent.h"
#include "CMonkeyThrottleEvent.h"
#include "CMonkeyNoopEvent.h"
#include <Elastos.CoreLibrary.h>

using Elastos::Utility::Etl::List;
using Elastos::IO::IBufferedReader;
using Elastos::IO::IPrintWriter;
using Elastos::Net::IServerSocket;
using Elastos::Net::ISocket;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

class MonkeySourceNetwork {
public:
    MonkeySourceNetwork();
    static AutoPtr<IMonkeyCommandReturn> InitStatic();

//Todo
    class DeferredReturn : public ElRefBase {
    public:
        DeferredReturn(
            /* [in] */ Int32 event,
            /* [in] */ IMonkeyCommandReturn *deferredReturn,
            /* [in] */ Int64 timeout);
        /**
         * Wait until the given event has occurred before returning the value.
         * @return The MonkeyCommandReturn from the command that was deferred.
         */
         WaitForEvent(IMonkeyCommandReturn** mcr);
    private:
    public:
        static const Int32 ON_WINDOW_STATE_CHANGE = 1;
    private:
        Int32 mEvent;
        AutoPtr<IMonkeyCommandReturn> mDeferredReturn;
        Int64 mTimeout;
        Object mLock;
        static Object mLockLock;
    };
protected:
    CARAPI Init(
        /* [in] */ Int32 port);

    CARAPI GetNextEvent(
    /* [out] */ IMonkeyEvent **event);

    CARAPI SetVerbose(
        /* [in] */ Int32 verbose);

    CARAPI Validate(
        /* [out] */ Boolean *result);
private:
    class FlipCommand : public ElRefBase, public IMonkeyCommand{
    public:
        CAR_INTERFACE_DECL()

        CARAPI TranslateCommand(
            /* [in] */ ArrayOf<String> *command,
            /* [in] */ ICommandQueue *queue,
            /* [out] */ IMonkeyCommandReturn **ret);
    protected:
    private:
    };

    class TouchCommand : public ElRefBase, public IMonkeyCommand {
    public:
        CAR_INTERFACE_DECL()

        CARAPI TranslateCommand(
            /* [in] */ ArrayOf<String> *command,
            /* [in] */ ICommandQueue *queue,
            /* [out] */ IMonkeyCommandReturn **ret);
    };

    class TrackballCommand : public ElRefBase, public IMonkeyCommand {
    public:
        CAR_INTERFACE_DECL()

        CARAPI TranslateCommand(
            /* [in] */ ArrayOf<String> *command,
            /* [in] */ ICommandQueue *queue,
            /* [out] */ IMonkeyCommandReturn **ret);
    };

    class KeyCommand : public ElRefBase, public IMonkeyCommand {
    public:
        CAR_INTERFACE_DECL()

        CARAPI TranslateCommand(
            /* [in] */ ArrayOf<String> *command,
            /* [in] */ ICommandQueue *queue,
            /* [out] */ IMonkeyCommandReturn **ret);
    };

    class SleepCommand : public ElRefBase, public IMonkeyCommand {
    public:
        CAR_INTERFACE_DECL()

        CARAPI TranslateCommand(
            /* [in] */ ArrayOf<String> *command,
            /* [in] */ ICommandQueue *queue,
            /* [out] */ IMonkeyCommandReturn **ret);
    };

    class TypeCommand : public ElRefBase, public IMonkeyCommand {
    public:
        CAR_INTERFACE_DECL()

        CARAPI TranslateCommand(
            /* [in] */ ArrayOf<String> *command,
            /* [in] */ ICommandQueue *queue,
            /* [out] */ IMonkeyCommandReturn **ret);
    };

    class WakeCommand : public ElRefBase, public IMonkeyCommand {
    public:
        CAR_INTERFACE_DECL()

        CARAPI TranslateCommand(
            /* [in] */ ArrayOf<String> *command,
            /* [in] */ ICommandQueue *queue,
            /* [out] */ IMonkeyCommandReturn **ret);
    };

    class TapCommand : public ElRefBase, public IMonkeyCommand {
    public:
        CAR_INTERFACE_DECL()

        CARAPI TranslateCommand(
            /* [in] */ ArrayOf<String> *command,
            /* [in] */ ICommandQueue *queue,
            /* [out] */ IMonkeyCommandReturn **ret);
    };

    /**
     * Command to "press" a buttons (Sends an up and down key event.)
     */
    class PressCommand : public ElRefBase, public IMonkeyCommand {
    public:
        CAR_INTERFACE_DECL()

        CARAPI TranslateCommand(
            /* [in] */ ArrayOf<String> *command,
            /* [in] */ ICommandQueue *queue,
            /* [out] */ IMonkeyCommandReturn **ret);
    };

    /**
     * Command to defer the return of another command until the given event occurs.
     * deferreturn takes three arguments. It takes an event to wait for (e.g. waiting for the
     * device to display a different activity would the "screenchange" event), a
     * timeout, which is the number of microseconds to wait for the event to occur, and it takes
     * a command. The command can be any other Monkey command that can be issued over the network
     * (e.g. press KEYCODE_HOME). deferreturn will then run this command, return an OK, wait for
     * the event to occur and return the deferred return value when either the event occurs or
     * when the timeout is reached (whichever occurs first). Note that there is no difference
     * between an event occurring and the timeout being reached; the client will have to verify
     * that the change actually occured.
     *
     * Example:
     *     deferreturn screenchange 1000 press KEYCODE_HOME
     * This command will press the home key on the device and then wait for the screen to change
     * for up to one second. Either the screen will change, and the results fo the key press will
     * be returned to the client, or the timeout will be reached, and the results for the key
     * press will be returned to the client.
     */
    class DeferReturnCommand : public ElRefBase, public IMonkeyCommand {
    public:
        CAR_INTERFACE_DECL()

        CARAPI TranslateCommand(
            /* [in] */ ArrayOf<String> *command,
            /* [in] */ ICommandQueue *queue,
            /* [out] */ IMonkeyCommandReturn **ret);
    };

    class CommandQueueImpl : public ElRefBase, public ICommandQueue {
    public:
        CAR_INTERFACE_DECL()

        CommandQueueImpl();

        CARAPI EnqueueEvent(
            /* [in] */ IMonkeyEvent *e);
        CARAPI GetNextQueuedEvent(
            /* [out] */ IMonkeyEvent **event);

    private:
        AutoPtr<List<AutoPtr<IMonkeyEvent> > > queueEvents;
    };

    static CARAPI_(Int32) GetKeyCode(const String& keyName);

    /**
     * Start a network server listening on the specified port.  The
     * network protocol is a line oriented protocol, where each line
     * is a different command that can be run.
     *
     * @param port the port to listen on
     */
     CARAPI StartServer();

     /**
     * Stop the server from running so it can reconnect a new client.
     */
     CARAPI StopServer();

     /**
     * Helper function for commandLineSplit that replaces quoted
     * charaters with their real values.
     *
     * @param input the string to do replacement on.
     * @return the results with the characters replaced.
     */
     CARAPI_(String) ReplaceQuotedChars(
        /* [in] */const String& input);

    /**
     * This function splits the given line into String parts.  It obey's quoted
     * strings and returns them as a single part.
     *
     * "This is a test" -> returns only one element
     * This is a test -> returns four elements
     *
     * @param line the line to parse
     * @return the List of elements
     */
     AutoPtr<List<String> > CommandLineSplit(
        /* [in] */ const String& line);

    /**
     * Translate the given command line into a MonkeyEvent.
     *
     * @param commandLine the full command line given.
     */
    CARAPI TranslateCommand(
        /* [in] */ const String& commandLine);

    CARAPI HandleReturn(
        /* [in] */ IMonkeyCommandReturn *ret);

    /**
     * Returns ERROR to the user.
    */
    CARAPI ReturnError();

    /**
     * Returns ERROR to the user.
     *
     * @param msg the error message to include
     */
    CARAPI ReturnError(
        /* [in] */ const String& msg);

    /**
     * Returns OK to the user.
     */
    CARAPI ReturnOk();

    /**
     * Returns OK to the user.
     *
     * @param returnValue the value to return from this command.
     */
    CARAPI ReturnOk(
        /* [in] */ const String& ret);

public:
    static AutoPtr<IMonkeyCommandReturn> OK;
    static AutoPtr<IMonkeyCommandReturn> ERROR;
    static AutoPtr<IMonkeyCommandReturn> EARG;

private:
    const AutoPtr<CommandQueueImpl> mCommandQueue;
    AutoPtr<IBufferedReader> mInput;
    AutoPtr<IPrintWriter> mOutput;
    Boolean mStarted;
    AutoPtr<IServerSocket> mServerSocket;
    AutoPtr<ISocket> mClientSocket;

//Todo:
public: //here DeferredReturn is also private class ,so cannot be private here.
    static AutoPtr<DeferredReturn> sDeferredReturn;
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __MONKEYSOURCENETWORK_H__
