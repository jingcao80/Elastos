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

#ifndef  __ELASTOS_DROID_PHONE_CCLIRLISTPREFERENCE_H__
#define  __ELASTOS_DROID_PHONE_CCLIRLISTPREFERENCE_H__

#include "_Elastos_Droid_TeleService_Phone_CCLIRListPreference.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

/**
 * {@link ListPreference} for CLIR (Calling Line Identification Restriction).
 * Right now this is used for "Caller ID" setting.
 */
CarClass(CCLIRListPreference)
    , public ListPreference
    , public ICLIRListPreference
{
private:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("AccelerometerListener::MySensorEventListener")

        MyHandler(
            /* [in] */ CCLIRListPreference* host);

        //@Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CARAPI_(void) HandleGetCLIRResponse(
            /* [in] */ IMessage* msg);

        CARAPI_(void) HandleSetCLIRResponse(
            /* [in] */ IMessage* msg);

    private:
        CCLIRListPreference* mHost;

        static const Int32 MESSAGE_GET_CLIR;
        static const Int32 MESSAGE_SET_CLIR;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL();

    CCLIRListPreference();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI Init(
        /* [in] */ ITimeConsumingPreferenceListener* listener,
        /* [in] */ Boolean skipReading);

    CARAPI HandleGetCLIRResult(
        /* [in] */ ArrayOf<Int32>* tmpClirArray);

protected:
    //@Override
    CARAPI OnDialogClosed(
        /* [in] */ Boolean positiveResult);

public:
    static const String TAG;
    Boolean DBG;

    AutoPtr<IHandler> mHandler;
    AutoPtr<IPhone> mPhone;
    AutoPtr<ITimeConsumingPreferenceListener> mTcpListener;

    AutoPtr<ArrayOf<Int32> > mClirArray;
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CCLIRLISTPREFERENCE_H__