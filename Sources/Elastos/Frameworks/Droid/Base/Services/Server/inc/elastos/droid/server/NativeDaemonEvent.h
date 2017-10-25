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

#ifndef __ELASTOS_DROID_SERVER_NATIVEDAEMONEVENT_H__
#define __ELASTOS_DROID_SERVER_NATIVEDAEMONEVENT_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Server {

/**
 * Parsed event from native side of {@link NativeDaemonConnector}.
 */
class NativeDaemonEvent
    : public Object
{
public:
    CARAPI_(Int32) GetCmdNumber();

    CARAPI_(Int32) GetCode();

    CARAPI_(String) GetMessage();

    CARAPI_(String) GetRawEvent();

    using Object::ToString;

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

DEFINE_CONVERSION_FOR(Elastos::Droid::Server::NativeDaemonEvent, IInterface)

#endif //__ELASTOS_DROID_SERVER_NATIVEDAEMONEVENT_H__
