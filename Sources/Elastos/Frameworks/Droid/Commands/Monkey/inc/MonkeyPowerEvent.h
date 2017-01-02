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

#ifndef __MONKEYPOWEREVENT_H__
#define __MONKEYPOWEREVENT_H__

#include "MonkeyEvent.h"
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::CContentValues;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

class MonkeyPowerEvent : public MonkeyEvent
{
public:
    typedef List<AutoPtr<IContentValues> >::Iterator ContentValuesIterator;
protected:
    MonkeyPowerEvent();

    MonkeyPowerEvent(
        /* [in] */ const String& powerLogTag,
        /* [in] */ const String& powerTestResult);

    MonkeyPowerEvent(
        /* [in] */ const String& powerLogTag);

    ~MonkeyPowerEvent();

    CARAPI Init(
        /* [in] */ const String& powerLogTag,
        /* [in] */ const String& powerTestResult);

    CARAPI Init(
        /* [in] */ const String& powerLogTag);

    CARAPI Init();

    CARAPI_(Int32) InjectEvent(
        /* [in] */ IIWindowManager *iwm,
        /* [in] */ IIActivityManager *iam,
        /* [in] */ Int32 verbose);

    //Todo
private:
    CARAPI BufferLogEvent(
        /* [in] */ const String& tag,
        /* [in] */ const String& value);

    CARAPI WriteLogEvents();

private:
    static const String TAG;
    static const String LOG_FILE;
    static AutoPtr<List<AutoPtr<IContentValues> > > mLogEvents;
    static const String TEST_SEQ_BEGIN;
    static const String TEST_STARTED;
    static const String TEST_DELAY_STARTED;
    static const String TEST_ENDED;
    static const String TEST_IDLE_ENDED;
    static Int64 mTestStartTime;
    String mPowerLogTag;
    String mTestResult;
    static const Int64 USB_DELAY_TIME;
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __MONKEYPOWEREVENT_H__
