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

#include "MonkeySourceNetwork.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringBuffer.h>
#include "elastos/droid/os/SystemClock.h"
#include "CMonkeySourceNetworkVars.h"
#include "CMonkeyCommandReturn.h"
#include "CMonkeySourceNetworkViews.h"
#include "CMonkeySourceRandomHelper.h"
#include "CMonkeySourceNetworkViewsHelper.h"

using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::IKeyCharacterMapHelper;
using Elastos::Droid::View::IKeyCharacterMap;
using Elastos::Droid::View::CKeyCharacterMapHelper;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Core::StringBuffer;
using Elastos::Core::StringUtils;
using Elastos::IO::ICloseable;
using Elastos::IO::IInputStreamReader;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::IO::CPrintWriter;
using Elastos::IO::CInputStreamReader;
using Elastos::IO::CBufferedReader;
using Elastos::Net::IInetAddressHelper;
using Elastos::Net::CInetAddressHelper;
using Elastos::Net::IInetAddress;
using Elastos::Net::CServerSocket;
using Elastos::Net::IServerSocket;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

static const String TAG = String("MonkeyStub");
AutoPtr<IMonkeyCommandReturn> MonkeySourceNetwork::OK = MonkeySourceNetwork::InitStatic();
AutoPtr<IMonkeyCommandReturn> MonkeySourceNetwork::ERROR = NULL;
AutoPtr<IMonkeyCommandReturn> MonkeySourceNetwork::EARG = NULL;
static AutoPtr<HashMap<String, AutoPtr<IMonkeyCommand> > > COMMAND_MAP;
Mutex MonkeySourceNetwork::DeferredReturn::mLockLock;

AutoPtr<MonkeySourceNetwork::DeferredReturn> MonkeySourceNetwork::sDeferredReturn;

//QUIT command
static const String QUIT = String("quit");
//Done command
static const String DONE = String("done");

// command response strings
static const String OK_STR = String("OK");
static const String ERROR_STR = String("ERROR");
/**
 * Force the device to wake up.
 *
 * @return true if woken up OK.
 */
 static Boolean Wake()
 {
    AutoPtr<IServiceManager> sm;
    ASSERT_SUCCEEDED(CServiceManager::AcquireSingleton((IServiceManager**)&sm));
    AutoPtr<IInterface> obj;
    sm->GetService(IContext::POWER_SERVICE, (IInterface**)&obj);
    AutoPtr<IPowerManager> pm = IPowerManager::Probe(obj);
    ECode e = pm->WakeUp(SystemClock::GetUptimeMillis());
    if (FAILED(e)) {
        Logger::E(TAG, "Got remote exception!");
        return FALSE;
    }
    return TRUE;
 }

AutoPtr<IMonkeyCommandReturn> MonkeySourceNetwork::InitStatic()
{
    AutoPtr<IMonkeyCommandReturn> mcr;
    ASSERT_SUCCEEDED(CMonkeyCommandReturn::New(TRUE, (IMonkeyCommandReturn**)&mcr));
    ASSERT_SUCCEEDED(CMonkeyCommandReturn::New(FALSE, (IMonkeyCommandReturn**)&ERROR));
    ASSERT_SUCCEEDED(CMonkeyCommandReturn::New(FALSE, String("Invalid Argment"),
        (IMonkeyCommandReturn**)&EARG));
    COMMAND_MAP = new HashMap<String, AutoPtr<IMonkeyCommand> >();
    (*COMMAND_MAP)[String("flip")] = new FlipCommand();
    (*COMMAND_MAP)[String("touch")] = new TouchCommand();
    (*COMMAND_MAP)[String("trackball")] = new TrackballCommand();
    (*COMMAND_MAP)[String("key")] = new KeyCommand();
    (*COMMAND_MAP)[String("sleep")] = new SleepCommand();
    (*COMMAND_MAP)[String("wake")] = new WakeCommand();
    (*COMMAND_MAP)[String("tap")] = new TapCommand();
    (*COMMAND_MAP)[String("press")] = new PressCommand();
    (*COMMAND_MAP)[String("type")] = new TypeCommand();
    (*COMMAND_MAP)[String("listvar")] = new CMonkeySourceNetworkVars::ListVarCommand();
    (*COMMAND_MAP)[String("getvar")] = new CMonkeySourceNetworkVars::GetVarCommand();
    (*COMMAND_MAP)[String("listviews")] = new CMonkeySourceNetworkViews::ListViewsCommand();
    (*COMMAND_MAP)[String("queryview")] = new CMonkeySourceNetworkViews::QueryViewCommand();
    (*COMMAND_MAP)[String("getrootview")] = new CMonkeySourceNetworkViews::GetRootViewCommand();
    (*COMMAND_MAP)[String("getviewswithtext")] = new CMonkeySourceNetworkViews::GetViewsWithTextCommand();
    (*COMMAND_MAP)[String("deferreturn")] = new DeferReturnCommand();
    return mcr;
}

CAR_INTERFACE_IMPL(MonkeySourceNetwork::FlipCommand, IMonkeyCommand);

ECode MonkeySourceNetwork::FlipCommand::TranslateCommand(
    /* [in] */ ArrayOf<String>* command,
    /* [in] */ ICommandQueue* queue,
    /* [out] */ IMonkeyCommandReturn** ret)
{
    VALIDATE_NOT_NULL(ret);

    if (command && command->GetLength() > 1) {
        String direction = (*command)[1];
        AutoPtr<IMonkeyFlipEvent> e;
        if (direction.Equals("open")) {
            CMonkeyFlipEvent::New(TRUE, (IMonkeyFlipEvent**)&e);
            queue->EnqueueEvent(IMonkeyEvent::Probe(e));
            *ret = OK;
            REFCOUNT_ADD(*ret)
            return NOERROR;
        }
        else if(direction.Equals("close")) {
            CMonkeyFlipEvent::New(FALSE, (IMonkeyFlipEvent**)&e);
            queue->EnqueueEvent((IMonkeyEvent*)e.Get());
            *ret = OK;
            REFCOUNT_ADD(*ret)
            return NOERROR;
        }
    }

    *ret = EARG;
    REFCOUNT_ADD(*ret)
    return NOERROR;
}

CAR_INTERFACE_IMPL(MonkeySourceNetwork::TouchCommand, IMonkeyCommand);

ECode MonkeySourceNetwork::TouchCommand::TranslateCommand(
    /* [in] */ ArrayOf<String>* command,
    /* [in] */ ICommandQueue* queue,
    /* [out] */ IMonkeyCommandReturn** ret)
{
    VALIDATE_NOT_NULL(ret);

    if (command && command->GetLength() == 4) {
        String actionName = (*command)[1];
        Int32 x = 0;
        Int32 y = 0;
        x = StringUtils::ParseInt32((*command)[2], 10);
        y = StringUtils::ParseInt32((*command)[3], 10);

        Int32 action = -1;
        if(actionName.Equals("down")) {
            action = IMotionEvent::ACTION_DOWN;
        }
        else if(actionName.Equals("up")) {
            action = IMotionEvent::ACTION_UP;
        }
        else if(actionName.Equals("move")) {
            action = IMotionEvent::ACTION_MOVE;
        }

        if (-1 == action) {
            *ret = EARG;
            REFCOUNT_ADD(*ret)
            return NOERROR;
        }

        AutoPtr<IMonkeyTouchEvent> e;
        CMonkeyTouchEvent::New(action, (IMonkeyTouchEvent**)&e);
        e->AddPointer(0, x, y);
        queue->EnqueueEvent((IMonkeyEvent*)e.Get());
        *ret = OK;
        REFCOUNT_ADD(*ret)
        return NOERROR;
    }

    *ret = EARG;
    REFCOUNT_ADD(*ret)
    return NOERROR;
}

CAR_INTERFACE_IMPL(MonkeySourceNetwork::TrackballCommand, IMonkeyCommand)

ECode MonkeySourceNetwork::TrackballCommand::TranslateCommand(
    /* [in] */ ArrayOf<String>* command,
    /* [in] */ ICommandQueue* queue,
    /* [out] */ IMonkeyCommandReturn** ret)
{
    VALIDATE_NOT_NULL(ret);

    if (command && command->GetLength() == 3) {
        Int32 dx = 0;
        Int32 dy = 0;
        dx = StringUtils::ParseInt32((*command)[1], 10);
        dy = StringUtils::ParseInt32((*command)[2], 10);

        AutoPtr<IMonkeyTrackballEvent> e;
        CMonkeyTrackballEvent::New(IMotionEvent::ACTION_MOVE, (IMonkeyTrackballEvent**)&e);
        e->AddPointer(0, dx, dy);
        queue->EnqueueEvent(IMonkeyEvent::Probe(e));
        *ret = OK;
        REFCOUNT_ADD(*ret)
        return NOERROR;
    }

    *ret = EARG;
    REFCOUNT_ADD(*ret)
    return NOERROR;
}

CAR_INTERFACE_IMPL(MonkeySourceNetwork::KeyCommand, IMonkeyCommand)

ECode MonkeySourceNetwork::KeyCommand::TranslateCommand(
    /* [in] */ ArrayOf<String>* command,
    /* [in] */ ICommandQueue* queue,
    /* [out] */ IMonkeyCommandReturn** ret)
{
    VALIDATE_NOT_NULL(ret);

    if (command && command->GetLength() == 3) {
        Int32 keyCode = GetKeyCode((*command)[2]);
        if (keyCode < 0) {
            //Ok, you gave us something bad.
            *ret = EARG;
            REFCOUNT_ADD(*ret)
            return NOERROR;
        }

        Int32 action = -1;
        if ((*command)[1].Equals("down")) {
            action = IKeyEvent::ACTION_DOWN;
        }
        else if ((*command)[1].Equals("up")) {
            action = IKeyEvent::ACTION_UP;
        }

        if (-1 == action) {
            *ret = EARG;
            REFCOUNT_ADD(*ret)
            return NOERROR;
        }

        AutoPtr<IMonkeyKeyEvent> e;
        CMonkeyKeyEvent::New(action, keyCode, (IMonkeyKeyEvent**)&e);
        queue->EnqueueEvent((IMonkeyEvent*)e.Get());
        *ret = OK;
        REFCOUNT_ADD(*ret)
        return NOERROR;
    }

    *ret = EARG;
    REFCOUNT_ADD(*ret)
    return NOERROR;
}

/*
* Get an integer keycode value from a given keyname
* @param keyName the key name to get the code fo
* @return the ingeger keycode value ,or -1 on error
*/
Int32 MonkeySourceNetwork::GetKeyCode(
    /* [in] */ const String& keyName)
{
    Int32 keyCode = -1;
    ECode e = StringUtils::ParseInt32(keyName, &keyCode);
    if (FAILED(e)) {
        //Ok, It wasn't a number, see if we have a keycode name for it
        AutoPtr<IMonkeySourceRandomHelper> helper;
        CMonkeySourceRandomHelper::AcquireSingleton((IMonkeySourceRandomHelper**)&helper);
        ASSERT_SUCCEEDED(helper->GetKeyCode(keyName, &keyCode))
        if (IKeyEvent::KEYCODE_UNKNOWN == keyCode) {
            //OK, one last ditch effort to find a match
            String tmpKeyName = keyName.ToUpperCase();
            ASSERT_SUCCEEDED(helper->GetKeyCode(String("KEYCODE_")
                + tmpKeyName, &keyCode))
            if (IKeyEvent::KEYCODE_UNKNOWN == keyCode) {
                //Still unknown
                return -1;
            }
        }
    }
    return keyCode;
}

CAR_INTERFACE_IMPL(MonkeySourceNetwork::SleepCommand, IMonkeyCommand)

ECode MonkeySourceNetwork::SleepCommand::TranslateCommand(
    /* [in] */ ArrayOf<String> *command,
    /* [in] */ ICommandQueue *queue,
    /* [out] */ IMonkeyCommandReturn **ret)
{
    VALIDATE_NOT_NULL(ret);

    if (command  && command->GetLength() == 2) {
        Int32 sleep = -1;
        String sleepStr = (*command)[1];
        ECode err = StringUtils::ParseInt32(sleepStr, &sleep);
        if (FAILED(err)) {
            *ret = EARG;
            REFCOUNT_ADD(*ret)
            return NOERROR;
        }

        AutoPtr<IMonkeyThrottleEvent> e;
        CMonkeyThrottleEvent::New(sleep, (IMonkeyThrottleEvent**)&e);
        queue->EnqueueEvent(IMonkeyEvent::Probe(e));
        *ret = OK;
        REFCOUNT_ADD(*ret)
        return NOERROR;
    }
    *ret = EARG;
    REFCOUNT_ADD(*ret)
    return NOERROR;
}

CAR_INTERFACE_IMPL(MonkeySourceNetwork::TypeCommand, IMonkeyCommand)

ECode MonkeySourceNetwork::TypeCommand::TranslateCommand(
    /* [in] */ ArrayOf<String> *command,
    /* [in] */ ICommandQueue *queue,
    /* [out] */ IMonkeyCommandReturn **ret)
{
    VALIDATE_NOT_NULL(ret);

    if (command && 2 == command->GetLength()) {
        String str = (*command)[1];
        AutoPtr<ArrayOf<Char32> > chars = str.GetChars();
        // Convert the string to an array of KeyEvent's for  the built in keymap
        AutoPtr<IKeyCharacterMap> keyCharacterMap;
        AutoPtr<IKeyCharacterMapHelper> helper;
        CKeyCharacterMapHelper::AcquireSingleton((IKeyCharacterMapHelper**)&helper);
        helper->Load(IKeyCharacterMap::VIRTUAL_KEYBOARD, (IKeyCharacterMap**)&keyCharacterMap);
        AutoPtr<ArrayOf<IKeyEvent*> > events;
        keyCharacterMap->GetEvents(chars, (ArrayOf<IKeyEvent*>**)&events);

        //enqueue all the events we just got
        Int32 length = events->GetLength();
        for (Int32 i = 0; i < length; ++i) {
            AutoPtr<IMonkeyKeyEvent> e;
            CMonkeyKeyEvent::New((*events)[i], (IMonkeyKeyEvent**)&e);
            queue->EnqueueEvent(IMonkeyEvent::Probe(e));
        }

        *ret = OK;
        REFCOUNT_ADD(*ret)
        return NOERROR;
    }
    *ret = EARG;
    REFCOUNT_ADD(*ret)
    return NOERROR;
}

CAR_INTERFACE_IMPL(MonkeySourceNetwork::WakeCommand, IMonkeyCommand)

ECode MonkeySourceNetwork::WakeCommand::TranslateCommand(
    /* [in] */ ArrayOf<String> *command,
    /* [in] */ ICommandQueue *queue,
    /* [out] */ IMonkeyCommandReturn **ret)
{
    VALIDATE_NOT_NULL(ret);

    if (!Wake()) {
        *ret = EARG;
        REFCOUNT_ADD(*ret)
        return NOERROR;
    }

    *ret = OK;
    REFCOUNT_ADD(*ret)
    return NOERROR;
}

CAR_INTERFACE_IMPL(MonkeySourceNetwork::TapCommand, IMonkeyCommand)

ECode MonkeySourceNetwork::TapCommand::TranslateCommand(
    /* [in] */ ArrayOf<String> *command,
    /* [in] */ ICommandQueue *queue,
    /* [out] */ IMonkeyCommandReturn **ret)
{
    VALIDATE_NOT_NULL(ret);

    if (command && 3 == command->GetLength()) {
        Int32 x = 0;
        Int32 y = 0;
        ECode err = StringUtils::ParseInt32((*command)[1], &x);
        if (FAILED(err)) {
            *ret = EARG;
            REFCOUNT_ADD(*ret)
            return NOERROR;
        }

        err = StringUtils::ParseInt32((*command)[2], &y);
        if(FAILED(err)) {
            *ret = EARG;
            REFCOUNT_ADD(*ret)
            return NOERROR;
        }

        AutoPtr<IMonkeyTouchEvent> e;
        CMonkeyTouchEvent::New(IMotionEvent::ACTION_DOWN, (IMonkeyTouchEvent**)&e);
        e->AddPointer(0, x, y);
        queue->EnqueueEvent(IMonkeyEvent::Probe(e));
        e = NULL;
        CMonkeyTouchEvent::New(IMotionEvent::ACTION_UP, (IMonkeyTouchEvent**)&e);
        e->AddPointer(0, x, y);
        queue->EnqueueEvent(IMonkeyEvent::Probe(e));
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(MonkeySourceNetwork::PressCommand, IMonkeyCommand)
//press keycode
ECode MonkeySourceNetwork::PressCommand::TranslateCommand(
    /* [in] */ ArrayOf<String> *command,
    /* [in] */ ICommandQueue *queue,
    /* [out] */ IMonkeyCommandReturn **ret)
{
    VALIDATE_NOT_NULL(ret);

    if (command && 2 == command->GetLength()) {
        Int32 keyCode = GetKeyCode((*command)[1]);
        if (keyCode < 0){
            //Ok, you gave us something bad.
            Logger::E(TAG, "Can't find keyname: %s", (*command)[1].string());
            *ret = EARG;
            REFCOUNT_ADD(*ret)
            return NOERROR;
        }
        AutoPtr<IMonkeyKeyEvent> e;
        CMonkeyKeyEvent::New(IKeyEvent::ACTION_DOWN, keyCode, (IMonkeyKeyEvent**)&e);
        queue->EnqueueEvent(IMonkeyEvent::Probe(e));
        e = NULL;
        CMonkeyKeyEvent::New(IKeyEvent::ACTION_UP, keyCode, (IMonkeyKeyEvent**)&e);
        queue->EnqueueEvent(IMonkeyEvent::Probe(e));
        *ret = OK;
        REFCOUNT_ADD(*ret)
        return NOERROR;
    }
    *ret = EARG;
    REFCOUNT_ADD(*ret)
    return NOERROR;
}

CAR_INTERFACE_IMPL(MonkeySourceNetwork::DeferReturnCommand, IMonkeyCommand)
// deferreturn [event] [timeout (ms)] [command]
// deferreturn screenchange 100 tap 10 10
ECode MonkeySourceNetwork::DeferReturnCommand::TranslateCommand(
    /* [in] */ ArrayOf<String> *command,
    /* [in] */ ICommandQueue *queue,
    /* [out] */ IMonkeyCommandReturn **ret)
{
    VALIDATE_NOT_NULL(ret);

    if (command && command->GetLength() > 3) {
        String event = (*command)[1];
        Int32 eventId = 0;
        if (event.Equals("screenchange")) {
            eventId = DeferredReturn::ON_WINDOW_STATE_CHANGE;
        }
        else {
            *ret = EARG;
            REFCOUNT_ADD(*ret)
            return NOERROR;
        }

        Int64 timeout = StringUtils::ParseInt64((*command)[2]);
        AutoPtr<IMonkeyCommand> deferredCommand = (*COMMAND_MAP)[(*command)[3]];
        if(deferredCommand.Get() != NULL)
        {
            AutoPtr<ArrayOf<String> > parts = ArrayOf<String>::Alloc(command->GetLength() - 3);
            parts->Copy(command, 3, command->GetLength() - 3);
            AutoPtr<IMonkeyCommandReturn> cmdRet;
            deferredCommand->TranslateCommand(parts, queue, (IMonkeyCommandReturn**)&cmdRet);
            sDeferredReturn = new DeferredReturn(eventId, cmdRet, timeout);
            *ret = OK;
            REFCOUNT_ADD(*ret)
            return NOERROR;
        }
    }
    *ret = EARG;
    REFCOUNT_ADD(*ret)
    return NOERROR;
}

CAR_INTERFACE_IMPL(MonkeySourceNetwork::CommandQueueImpl, ICommandQueue)

MonkeySourceNetwork::CommandQueueImpl::CommandQueueImpl()
    : queueEvents(new List<AutoPtr<IMonkeyEvent> >())
{
}

// Queue of Events to be processed.  This allows commands to push
// multiple events into the queue to be processed.
ECode MonkeySourceNetwork::CommandQueueImpl::EnqueueEvent(
    /* [in] */ IMonkeyEvent* e)
{
    queueEvents->PushBack(e);
    return NOERROR;
}

/**
 * Get the next queued event to excecute.
 *
 * @return the next event, or null if there aren't any more.
 */

ECode MonkeySourceNetwork::CommandQueueImpl::GetNextQueuedEvent(
    /* [out] */ IMonkeyEvent **event)
{
    VALIDATE_NOT_NULL(event)
    AutoPtr<IMonkeyEvent> temp = queueEvents->GetFront();
    queueEvents->PopFront();
    *event = temp;
    REFCOUNT_ADD(*event);
    return NOERROR;
}

MonkeySourceNetwork::DeferredReturn::DeferredReturn(
    /* [in] */ Int32 event,
    /* [in] */ IMonkeyCommandReturn *deferredReturn,
    /* [in] */ Int64 timeout)
    : mEvent(event)
    , mDeferredReturn(deferredReturn)
    , mTimeout(timeout)
{
}

MonkeySourceNetwork::DeferredReturn::WaitForEvent(
    /* [out] */ IMonkeyCommandReturn **mcr)
{
    VALIDATE_NOT_NULL(mcr);

    switch (mEvent) {
        case ON_WINDOW_STATE_CHANGE: {
            AutoLock lock(mLockLock);
            mLock.Wait(mTimeout);
        }
    }

    *mcr = mDeferredReturn;
    REFCOUNT_ADD(*mcr)
    return NOERROR;
}

MonkeySourceNetwork::MonkeySourceNetwork()
    : mCommandQueue(new CommandQueueImpl())
{
}

ECode MonkeySourceNetwork::Init(
        /* [in] */ Int32 port)
{
    AutoPtr<IInetAddressHelper> helper;
    ASSERT_SUCCEEDED(CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&helper));
    AutoPtr<IInetAddress> ia;
    helper->GetLocalHost((IInetAddress**)&ia);

    CServerSocket::New(port, 0, ia, (IServerSocket**)&mServerSocket);
    return NOERROR;
}

ECode MonkeySourceNetwork::StartServer()
{
    mClientSocket = NULL;
    ASSERT_SUCCEEDED(mServerSocket->Accept((ISocket**)&mClientSocket));
    // At this point, we have a client connected.
        // Attach the accessibility listeners so that we can start receiving
        // view events. Do this before wake so we can catch the wake event
        // if possible.
    AutoPtr<IMonkeySourceNetworkViewsHelper> helper;
    CMonkeySourceNetworkViewsHelper::AcquireSingleton((IMonkeySourceNetworkViewsHelper**)&helper);
    helper->Setup();
    // Wake the device up in preparation for doing some commands.
    Wake();

    AutoPtr<IInputStreamReader> reader;
    AutoPtr<IInputStream> stream;
    mClientSocket->GetInputStream((IInputStream**)&stream);
    CInputStreamReader::New(stream, (IInputStreamReader**)&reader);
    mInput = NULL;
    CBufferedReader::New(reader, (IBufferedReader**)&mInput);

    // auto-flush
    AutoPtr<IOutputStream> ostream;
    mClientSocket->GetOutputStream((IOutputStream**)&ostream);
    mOutput = NULL;
    CPrintWriter::New(ostream, TRUE, (IPrintWriter**)&mOutput);
    return NOERROR;
}

ECode MonkeySourceNetwork::StopServer()
{
    mClientSocket->Close();

    if (ICloseable::Probe(mInput)) ICloseable::Probe(mInput)->Close();
    if (ICloseable::Probe(mOutput)) ICloseable::Probe(mOutput)->Close();
    mStarted = FALSE;
    return NOERROR;
}

String MonkeySourceNetwork::ReplaceQuotedChars(
    /* [in] */ const String& input)
{
    String ret;
    StringUtils::ReplaceAll(input, String("\\\""), String("\""), &ret);
    return ret;
}

AutoPtr<List<String> > MonkeySourceNetwork::CommandLineSplit(
    /* [in] */ const String& line)
{
    AutoPtr<List<String> > result = new List<String>();
    AutoPtr<StringTokenizer> tok = new StringTokenizer(line.string());
    Boolean insideQuote = FALSE;
    StringBuffer quotedWord;
    while (tok->HasMoreTokens()) {
        String cur((const char*)(tok->NextToken()));
        if (!insideQuote && cur.StartWith("\"")) {
            // begin quote
            quotedWord.AppendString(ReplaceQuotedChars(cur));
            insideQuote = TRUE;
        }
        else if (insideQuote) {
            // end quote
            if (cur.EndWith("\"")) {
                insideQuote = FALSE;
                quotedWord.AppendString(String(" "));
                quotedWord.AppendString(ReplaceQuotedChars(cur));
                String word;
                quotedWord.Substring(0, &word);

                // trim off the quotes
                result->PushBack(word.Substring(1));
            }
            else {
                quotedWord.AppendString(String(" "));
                quotedWord.AppendString(ReplaceQuotedChars(cur));
            }
        }
        else {
            result->PushBack(ReplaceQuotedChars(cur));
        }
    }

    return result;
}

ECode MonkeySourceNetwork:: TranslateCommand(
    /* [in] */ const String& commandLine)
{
    Logger::D(TAG, "translateCommand: %s", commandLine.string());
    AutoPtr<List<String> > parts = CommandLineSplit(commandLine);
    if(parts && parts->IsEmpty() == FALSE) {
        AutoPtr<IMonkeyCommand> command = (*COMMAND_MAP)[(*parts)[0]];
        if (NULL != command.Get()) {
            Int32 length = parts->GetSize();
            AutoPtr<ArrayOf<String> > commands = ArrayOf<String>::Alloc(length);
            for (Int32 i = 0; i < length; ++i) {
                commands->Set(i, (*parts)[i]);
            }

            AutoPtr<IMonkeyCommandReturn> ret;
            command->TranslateCommand(commands,
                mCommandQueue, (IMonkeyCommandReturn**)&ret);
            HandleReturn(ret);
        }
    }
    return NOERROR;
}

ECode MonkeySourceNetwork::HandleReturn(
    /* [in] */ IMonkeyCommandReturn *ret)
{
    Boolean result = FALSE;
    ret->WasSuccessful(&result);
    if( result) {
        ret->HasMessage(&result);
        if (result){
            String str;
            ret->GetMessage(&str);
            ReturnOk(str);
        }
        else {
            ReturnOk();
        }
    }
    else {
        ret->HasMessage(&result);
        if (result) {
            String str;
            ret->GetMessage(&str);
            ReturnError(str);
        }
        else {
            ReturnError();
        }
    }
    return NOERROR;
}

ECode MonkeySourceNetwork::GetNextEvent(
    /* [out] */ IMonkeyEvent **monkeyEvent)
{
    VALIDATE_NOT_NULL(monkeyEvent)
    *monkeyEvent = NULL;

    if (!mStarted) {
        ECode e = StartServer();
        if (FAILED(e)) {
            Logger::E(TAG, "Got IOException from server");
            return NOERROR;
        }
        mStarted = TRUE;
    }

    // Now, get the next command.  This call may block, but that's OK
    while(TRUE) {
        // Check to see if we have any events queued up.  If
        // we do, use those until we have no more.  Then get
        // more input from the user.
        AutoPtr<IMonkeyEvent> queuedEvent;
        mCommandQueue->GetNextQueuedEvent((IMonkeyEvent**)&queuedEvent);
        if (queuedEvent != NULL) {
            // dispatch the event
            *monkeyEvent = queuedEvent;
            REFCOUNT_ADD(*monkeyEvent);
            return NOERROR;
        }

        // Check to see if we have any returns that have been deferred. If so, now that
        // we've run the queued commands, wait for the given event to happen (or the timeout
        // to be reached), and handle the deferred MonkeyCommandReturn.
        if (sDeferredReturn != NULL) {
            Logger::D(TAG, "Waiting for event");
            AutoPtr<IMonkeyCommandReturn> ret;
            sDeferredReturn->WaitForEvent((IMonkeyCommandReturn**)&ret);
            sDeferredReturn = NULL;
            HandleReturn(ret);
        }

        String command;
        mInput->ReadLine(&command);
        if (command.IsNull()) {
            Logger::D(TAG, "Connection dropped.");
            // Treat this exactly the same as if the user had
            // ended the session cleanly with a done commant.
            command = DONE;
        }

        if (DONE.Equals(command)) {
            // stop the server so it can accept new connections
            ECode e = StopServer();
            if (FAILED(e)) {
                Logger::E(TAG, "Got IOException shutting down!");
                return NOERROR;
            }

            // return a noop event so we keep executing the main
            // loop
            AutoPtr<IMonkeyNoopEvent> event;
            CMonkeyNoopEvent::New((IMonkeyNoopEvent**)&event);
            *monkeyEvent = event;
            REFCOUNT_ADD(*monkeyEvent)
            return NOERROR;
        }

        // Do quit checking here
        if (QUIT.Equals(command)) {
            // then we're done
            Logger::D(TAG, "Quit requested");
            // let the host know the command ran OK
            ReturnOk();
            return NOERROR;
        }

        // Do comment checking here.  Comments aren't a
        // command, so we don't echo anything back to the
        // user.
        if (command.StartWith("#")) {
            // keep going
            continue;
        }

        // Translate the command line.  This will handle returning error/ok to the user
        TranslateCommand(command);
    }
    return NOERROR;
}

ECode MonkeySourceNetwork::SetVerbose(
        /* [in] */ Int32 verbose)
{
    // We're not particualy verbose
    return NOERROR;
}

ECode MonkeySourceNetwork::Validate(
        /* [out] */ Boolean *result)
{
    // we have no pre-conditions to validate
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

ECode MonkeySourceNetwork::ReturnError()
{
    mOutput->PrintStringln(ERROR_STR);
    return NOERROR;
}

ECode MonkeySourceNetwork::ReturnError(
    /* [in] */ const String& msg)
{
    mOutput->PrintString(ERROR_STR);
    mOutput->PrintString(String(":"));
    mOutput->PrintStringln(msg);
    return NOERROR;
}

ECode MonkeySourceNetwork::ReturnOk()
{
    mOutput->PrintStringln(OK_STR);
    return NOERROR;
}

ECode MonkeySourceNetwork::ReturnOk(
        /* [in] */ const String& returnValue)
{
    mOutput->PrintString(OK_STR);
    mOutput->PrintString(String(":"));
    mOutput->PrintStringln(returnValue);
    return NOERROR;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos
