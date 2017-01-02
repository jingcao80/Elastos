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

#ifndef __ELASTOS_DEVSAMPLES_DFSESSIONLESSDEMO_SLSERVICE_CSERVICE_H__
#define __ELASTOS_DEVSAMPLES_DFSESSIONLESSDEMO_SLSERVICE_CSERVICE_H__

#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "_Elastos_DevSamples_DFSessionLessDemo_SLService_CService.h"
#include <elastos/droid/app/Activity.h>
#include <elastos/droid/os/Handler.h>
#include <elastos/core/Object.h>
#include <_Org.Alljoyn.Bus.h>

using Elastos::Droid::App::Activity;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::IListView;
using Elastos::Core::Object;

using Org::Alljoyn::Bus::IBusAttachment;
using Org::Alljoyn::Bus::IBusObject;

namespace Elastos {
namespace DevSamples {
namespace DFSessionLessDemo {
namespace SLService {

CarClass(CService)
    , public Activity
{
public:
    class SimpleService
        : public Object
        , public ISimpleInterface
        , public IBusObject
    {
    public:
        CAR_INTERFACE_DECL();

        CARAPI constructor(
            /* [in] */ IObject* host);

        CARAPI Chat(
            /* [in] */ const String& inStr);

    private:
        /* Helper function to send a message to the UI thread. */
        CARAPI_(void) SendUiMessage(
            /* [in] */ Int32 what,
            /* [in] */ const String& str);

    private:
        CService* mHost;
    };

private:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CService::MyHandler")

        MyHandler(
            /* [in] */ CService* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CService* mHost;
    };

    /* This class will handle all AllJoyn calls. See onCreate(). */
    class BusHandler
        : public Handler
    {
        friend class SLService;
    public:
        TO_STRING_IMPL("CService::BusHandler")

        CARAPI constructor(
            /* [in] */ ILooper* looper,
            /* [in] */ CService* host);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    public:
        /* These are the messages sent to the BusHandler from the UI. */
        static const Int32 ADDMATCH;
        static const Int32 DISCONNECT;

        Boolean mStreamUp;

    private:
        /*
         * Name used as the well-known name and the advertised name.  This name must be a unique name
         * both to the bus and to the network as a whole.  The name uses reverse URL style of naming.
         */
        static const String SERVICE_NAME;
        static const Int16 CONTACT_PORT;

        AutoPtr<IBusAttachment> mBus;
        CService* mHost;
    };

public:
    CARAPI constructor();

    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    // @Override
    CARAPI OnCreateOptionsMenu(
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnOptionsItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* result);

protected:
    // @Override
    CARAPI OnDestroy();

private:
    CARAPI_(void) LogStatus(
        /* [in] */ const String& msg,
        /* [in] */ ECode status);

private:
    static const String TAG;

    static const Int32 MESSAGE_PING = 1;
    static const Int32 MESSAGE_PING_REPLY = 2;
    static const Int32 MESSAGE_POST_TOAST = 3;

    AutoPtr<IArrayAdapter> mListViewArrayAdapter;
    AutoPtr<IListView> mListView;
    AutoPtr<IMenu> mMenu;

    AutoPtr<IHandler> mHandler;

    /* The AllJoyn object that is our service. */
    AutoPtr<SimpleService> mSimpleService;

    /* Handler used to make calls to AllJoyn methods. See onCreate(). */
    AutoPtr<BusHandler> mBusHandler;
};

} // namespace SLService
} // namespace DFSessionLessDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __ELASTOS_DEVSAMPLES_DFSESSIONLESSDEMO_SLSERVICE_CSERVICE_H__
