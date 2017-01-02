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

#include "elastos/droid/os/UEventObserver.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/etl/Vector.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/AutoLock.h>
#include <hardware_legacy/uevent.h>
#include "Elastos.Droid.Content.h"


using Elastos::Core::StringBuilder;
using Elastos::Core::EIID_IThread;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Etl::Vector;

namespace Elastos {
namespace Droid {
namespace Os {

const String UEventObserver::TAG("UEventObserver");
const Boolean UEventObserver::DEBUG = FALSE;
AutoPtr<UEventObserver::UEventThread> UEventObserver::sThread;
Object UEventObserver::sClsLock;

//===============================================================
//UEventObserver::UEvent
//===============================================================.

CAR_INTERFACE_IMPL(UEventObserver::UEvent, Object, IUEvent)

UEventObserver::UEvent::UEvent(
    /* [in] */ const String& message)
{
    Int32 offset = 0;
    Int32 length = message.GetLength();
    Int32 equals, at;
    while (offset < length) {
        equals = message.IndexOf(String("="), offset);
        at = message.IndexOf('$', offset);
        if (at < 0) break;

        if (equals > offset && equals < at) {
            // key is before the equals sign, and value is after
            mMap[message.Substring(offset, equals)] = message.Substring(equals + 1, at);
        }

        offset = at + 1;
    }
}

String UEventObserver::UEvent::Get(
    /* [in] */ const String& key)
{
    HashMap<String, String>::Iterator it = mMap.Find(key);
    return it != mMap.End() ? it->mSecond : String(NULL);
}

String UEventObserver::UEvent::Get(
    /* [in] */ const String& key,
    /* [in] */ const String& defaultValue)
{
    HashMap<String, String>::Iterator it = mMap.Find(key);
    return it != mMap.End() ? it->mSecond : defaultValue;
}

ECode UEventObserver::UEvent::Get(
    /* [in] */ const String& key,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = Get(key);
    return NOERROR;
}

ECode UEventObserver::UEvent::Get(
    /* [in] */ const String& key,
    /* [in] */ const String& defaultValue,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = Get(key, defaultValue);
    return NOERROR;
}

ECode UEventObserver::UEvent::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    StringBuilder sb("UEvent:{");

    HashMap<String, String>::Iterator it;
    for (it = mMap.Begin(); it != mMap.End(); it++) {
        sb += "[";
        sb += it->mFirst;
        sb += ",";
        sb += it->mSecond;
        sb += "];";
    }
    sb += "}";
    *str = sb.ToString();
    return NOERROR;
}

//===============================================================
//UEventObserver::UEventThread
//===============================================================
UEventObserver::UEventThread::UEventThread()
{
    Thread::constructor(String("UEventObserver"));
}

ECode UEventObserver::UEventThread::Run()
{
    FAIL_RETURN(NativeSetup());

    while (TRUE) {
        String message = NativeWaitForNextEvent();
        if (!message.IsNull()) {
            if (DEBUG) {
                Logger::D(TAG, message);
            }
            SendEvent(message);
        }
    }
    return NOERROR;
}

void UEventObserver::UEventThread::SendEvent(
    /* [in] */ const String& message)
{
    {
        AutoLock lock(mKeysAndObserversLock);

        List< AutoPtr<IInterface> >::Iterator it = mKeysAndObservers.Begin();
        while (it != mKeysAndObservers.End()) {
            AutoPtr<ICharSequence> csStr = ICharSequence::Probe(*it);
            assert(csStr != NULL);
            String key;
            csStr->ToString(&key);
            if (message.Contains(key)) {
                List< AutoPtr<IInterface> >::Iterator tmpIt = it;
                AutoPtr<IInterface> obj = *(++tmpIt);
                AutoPtr<UEventObserver> observer = (UEventObserver*)IObject::Probe(obj);
                mTempObserversToSignal.PushBack(observer);
            }
            ++it;
            ++it;
        }
    }

    if (!mTempObserversToSignal.IsEmpty()) {
        AutoPtr<IUEvent> event = new UEvent(message);
        List< AutoPtr<UEventObserver> >::Iterator it;
        for (it = mTempObserversToSignal.Begin(); it != mTempObserversToSignal.End(); ++it) {
            AutoPtr<UEventObserver> observer = *it;
            observer->OnUEvent(event);
        }
        mTempObserversToSignal.Clear();
    }
}

void UEventObserver::UEventThread::AddObserver(
    /* [in] */ const String& match,
    /* [in] */ UEventObserver* observer)
{
    AutoLock lock(mKeysAndObserversLock);

    AutoPtr<ICharSequence> charSeq;
    CString::New(match, (ICharSequence**)&charSeq);
    mKeysAndObservers.PushBack(charSeq);
    mKeysAndObservers.PushBack(observer->Probe(EIID_IInterface));
    NativeAddMatch(match);
}

/** Removes every key/value pair where value=observer from mObservers */
void UEventObserver::UEventThread::RemoveObserver(
    /* [in] */ UEventObserver* observer)
{
    AutoLock lock(mKeysAndObserversLock);

    List< AutoPtr<IInterface> >::Iterator it = mKeysAndObservers.Begin();
    while (it != mKeysAndObservers.End()) {
        List< AutoPtr<IInterface> >::Iterator oit = it;
        AutoPtr<IInterface> obj = *(++oit);
        if(obj.Get() == observer->Probe(EIID_IInterface)) {
            AutoPtr<ICharSequence> csStr = ICharSequence::Probe(*it);
            assert(csStr != NULL);
            String match;
            csStr->ToString(&match);
            NativeRemoveMatch(match);
            mKeysAndObservers.Erase(it);
            mKeysAndObservers.Erase(oit);
        }
        else {
            ++it;
            ++it;
        }
    }
}


//===============================================================
//UEventObserver
//===============================================================
CAR_INTERFACE_IMPL(UEventObserver, Object, IUEventObserver)

UEventObserver::~UEventObserver()
{
    StopObserving();
}

ECode UEventObserver::NativeSetup()
{
    if (!uevent_init()) {
        //jniThrowException(env, "java/lang/RuntimeException", "Unable to open socket for UEventObserver");
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

static Object sMatchesMutex;
static Vector<String> sMatches;

static Boolean IsMatch(
    /* [in] */ const char* buffer,
    /* [in] */ Int32 length)
{
    AutoLock lock(sMatchesMutex);

    Vector<String>::Iterator it;
    for (it = sMatches.Begin(); it != sMatches.End(); ++it) {
        String match = *it;

        // Consider all zero-delimited fields of the buffer.
        const char* field = buffer;
        const char* end = buffer + length + 1;
        do {
            if (strstr(field, match.string())) {
                Logger::V("UEventObserver", "Matched uevent message with pattern: %s", match.string());
                return TRUE;
            }
            field += strlen(field) + 1;
        } while (field != end);
    }
    return FALSE;
}

String UEventObserver::NativeWaitForNextEvent()
{
    char buffer[1024];

    for (;;) {
        int length = uevent_next_event(buffer, sizeof(buffer) - 1);
        if (length <= 0) {
            return String(NULL);
        }
        buffer[length] = '\0';

        // Logger::V(TAG, "Received uevent message: %s", buffer);

        if (IsMatch(buffer, length)) {
            // Assume the message is ASCII.
            // String can't contain '\0' in middle, replace '\0' by '$' except end char
            for (Int32 i = 0; i < length; i++) {
                if (buffer[i] == '\0')
                    buffer[i] = '$';
            }
            return String(buffer, length);
        }
    }
}

void UEventObserver::NativeAddMatch(
    /* [in] */ const String& matchStr)
{
    AutoLock lock(sMatchesMutex);
    sMatches.PushBack(matchStr);
}

void UEventObserver::NativeRemoveMatch(
    /* [in] */ const String& matchStr)
{
    AutoLock lock(sMatchesMutex);

    Vector<String>::Iterator it;
    for (it = sMatches.Begin(); it != sMatches.End(); ++it) {
        if ((*it).Equals(matchStr)) {
            sMatches.Erase(it);
            break; // only remove first occurrence
        }
    }
}

AutoPtr<UEventObserver::UEventThread> UEventObserver::GetThread()
{
    AutoLock lock(sClsLock);

    if (sThread == NULL) {
        sThread = new UEventThread();
        sThread->Start();
    }
    return sThread;
}

AutoPtr<UEventObserver::UEventThread> UEventObserver::PeekThread()
{
    AutoLock lock(sClsLock);
    return sThread;
}

ECode UEventObserver::StartObserving(
    /* [in] */ const String& match)
{
    if (match.IsNullOrEmpty()) {
        //throw new IllegalArgumentException("match substring must be non-empty");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<UEventThread> t = GetThread();
    t->AddObserver(match, this);
    return NOERROR;
}

ECode UEventObserver::StopObserving()
{
    AutoPtr<UEventThread> t = GetThread();
    if (t != NULL) {
        t->RemoveObserver(this);
    }
    return NOERROR;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
