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

#include "CClient.h"
#include "R.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IProgressDialogHelper;
using Elastos::Droid::App::CProgressDialogHelper;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::InputMethod::IEditorInfo;
using Elastos::Droid::Widget::CArrayAdapter;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Droid::Widget::EIID_IOnEditorActionListener;
using Elastos::Core::CoreUtils;
using Elastos::Core::EIID_IInteger32;
using Elastos::Core::EIID_ICharSequence;
using Elastos::Utility::IIterator;
using Elastos::Utility::ISet;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Logging::Logger;

using Org::Alljoyn::Bus::IVariant;
using Org::Alljoyn::Bus::CVariant;
using Org::Alljoyn::Bus::CBusAttachment;
using Org::Alljoyn::Bus::CMutableInteger32Value;
using Org::Alljoyn::Bus::CSessionOpts;
using Org::Alljoyn::Bus::IBusListener;
using Org::Alljoyn::Bus::IMutableInteger32Value;
using Org::Alljoyn::Bus::ISessionListener;
using Org::Alljoyn::Bus::ISessionOpts;
using Org::Alljoyn::Bus::RemoteMessage_Receive;
using Org::Alljoyn::Bus::Alljoyn::IDaemonInit;
using Org::Alljoyn::Bus::Alljoyn::CDaemonInit;

namespace Elastos {
namespace DevSamples {
namespace DFBasicDemo {
namespace BasicClient {

//======================================================================
//  CClient::MyHander
//======================================================================
CClient::MyHandler::MyHandler(
    /* [in] */ CClient* host)
    : Handler(FALSE)
    , mHost(host)
{}

ECode CClient::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);

    switch (what) {
    case MESSAGE_PING: {
        AutoPtr<IInterface> obj;
        msg->GetObj((IInterface**)&obj);
        String cat = CoreUtils::Unbox(ICharSequence::Probe(obj));
        mHost->mListViewArrayAdapter->Add(CoreUtils::Convert(String("Cat args:  ") + cat));
        break;
    }
    case MESSAGE_PING_REPLY: {
        AutoPtr<IInterface> obj;
        msg->GetObj((IInterface**)&obj);
        String ret = CoreUtils::Unbox(ICharSequence::Probe(obj));
        mHost->mListViewArrayAdapter->Add(CoreUtils::Convert(String("Reply:  ") + ret));
        ITextView::Probe(mHost->mEditText)->SetText(CoreUtils::Convert(String("")));
        break;
    }
    case MESSAGE_POST_TOAST: {
        AutoPtr<IContext> context;
        mHost->GetApplicationContext((IContext**)&context);
        AutoPtr<IInterface> obj;
        msg->GetObj((IInterface**)&obj);
        AutoPtr<IToastHelper> helper;
        CToastHelper::AcquireSingleton((IToastHelper**)&helper);
        AutoPtr<IToast> toast;
        helper->MakeText(context, ICharSequence::Probe(obj), IToast::LENGTH_LONG, (IToast**)&toast);
        toast->Show();
        break;
    }
    case MESSAGE_START_PROGRESS_DIALOG: {
        AutoPtr<IProgressDialogHelper> helper;
        CProgressDialogHelper::AcquireSingleton((IProgressDialogHelper**)&helper);
        helper->Show(mHost, CoreUtils::Convert(String("")),
                CoreUtils::Convert(String("Finding Basic Service.\nPlease wait...")),
                TRUE, TRUE, (IProgressDialog**)&mHost->mDialog);
        break;
    }
    case MESSAGE_STOP_PROGRESS_DIALOG:
        IDialogInterface::Probe(mHost->mDialog)->Dismiss();
        break;
    default:
        break;
    }
    return NOERROR;
}

ECode CClient::MyHandler::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = "CClient::MyHandler";
    return NOERROR;
}


//======================================================================
//  CClient::EditorListener
//======================================================================
CAR_INTERFACE_IMPL(CClient::EditorListener, Object, IOnEditorActionListener);

CClient::EditorListener::EditorListener(
    /* [in] */ CClient* host)
    : mHost(host)
{}

ECode CClient::EditorListener::OnEditorAction(
    /* [in] */ ITextView* v,
    /* [in] */ Int32 actionId,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 action;
    if (actionId == IEditorInfo::IME_NULL
        && (event->GetAction(&action), action == IKeyEvent::ACTION_UP)) {
        Logger::I("CClient", "Call the remote object's cat method.");
        /* Call the remote object's cat method. */
        AutoPtr<ICharSequence> text;
        v->GetText((ICharSequence**)&text);
        AutoPtr<IMessage> msg;
        mHost->mBusHandler->ObtainMessage(BusHandler::CAT, text, (IMessage**)&msg);
        mHost->mBusHandler->SendMessage(msg, result);
    }
    *result = TRUE;
    return NOERROR;
}


//======================================================================
//  CClient::BusHandler::InnerBusListener
//======================================================================
ECode CClient::BusHandler::InnerBusListener::FoundAdvertisedName(
    /* [in] */ const String& name,
    /* [in] */ Int16 transport,
    /* [in] */ const String& namePrefix)
{
    Logger::I("CClient", "MyBusListener.foundAdvertisedName(%s, 0x%04x, %s)",
            name.string(), transport, namePrefix.string());
    /*
     * This client will only join the first service that it sees advertising
     * the indicated well-known name.  If the program is already a member of
     * a session (i.e. connected to a service) we will not attempt to join
     * another session.
     * It is possible to join multiple session however joining multiple
     * sessions is not shown in this sample.
     */
    if (!mHost->mIsConnected) {
        AutoPtr<IMessage> msg;
        mHost->ObtainMessage(JOIN_SESSION, (IMessage**)&msg);
        msg->SetArg1(transport);
        msg->SetObj(CoreUtils::Convert(name));
        Boolean result;
        mHost->SendMessage(msg, &result);
    }
    return NOERROR;
}


//======================================================================
//  CClient::BusHandler::InnerSessionListener
//======================================================================
ECode CClient::BusHandler::InnerSessionListener::SessionLost(
    /* [in] */ Int32 sessionId,
    /* [in] */ Int32 reason)
{
    mHost->mIsConnected = FALSE;
    Logger::I("CClient", "MyBusListener.sessionLost(sessionId = %d, reason = %d)", sessionId,reason);
    Boolean result;
    mHost->mHost->mHandler->SendEmptyMessage(MESSAGE_START_PROGRESS_DIALOG, &result);
    return NOERROR;
}


//======================================================================
//  CClient::BusHandler
//======================================================================
const Int32 CClient::BusHandler::CONNECT = 1;
const Int32 CClient::BusHandler::JOIN_SESSION = 2;
const Int32 CClient::BusHandler::DISCONNECT = 3;
const Int32 CClient::BusHandler::CAT = 4;
const String CClient::BusHandler::SERVICE_NAME("org.alljoyn.Bus.sample");
const Int16 CClient::BusHandler::CONTACT_PORT = 25;

static void LogMap(
    /* [in] */ IMap* map,
    /* [in] */ const char* info)
{
    Int32 size;
    map->GetSize(&size);

    Logger::I("CClient", " >> %s map with %d elements:", info, size);
    AutoPtr<ISet> set;
    map->GetEntrySet((ISet**)&set);
    AutoPtr<IIterator> it;
    set->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IInterface> key, value;
        IMapEntry::Probe(obj)->GetKey((IInterface**)&key);
        IMapEntry::Probe(obj)->GetValue((IInterface**)&value);
        Logger::I("CClient", "    > [%s] : [%s]", TO_CSTR(key), TO_CSTR(value));
    }
}

ECode CClient::BusHandler::constructor(
    /* [in] */ ILooper* looper,
    /* [in] */ CClient* host)
{
    Handler::constructor(looper);

    mSessionId = 0;
    mIsInASession = FALSE;
    mIsConnected = FALSE;
    mIsStoppingDiscovery = FALSE;
    mHost = host;
    return NOERROR;
}

ECode CClient::BusHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch(what) {
    /* Connect to a remote instance of an object implementing the BasicInterface. */
    case CONNECT: {
        AutoPtr<IContext> context;
        mHost->GetApplicationContext((IContext**)&context);
        AutoPtr<IDaemonInit> di;
        CDaemonInit::AcquireSingleton((IDaemonInit**)&di);
        Boolean result;
        di->PrepareDaemon(context, &result);
        /*
         * All communication through AllJoyn begins with a BusAttachment.
         *
         * A BusAttachment needs a name. The actual name is unimportant except for internal
         * security. As a default we use the class name as the name.
         *
         * By default AllJoyn does not allow communication between devices (i.e. bus to bus
         * communication). The second argument must be set to Receive to allow communication
         * between devices.
         */
        String pName;
        mHost->GetPackageName(&pName);
        mBus = NULL;
        CBusAttachment::New(pName, RemoteMessage_Receive, (IBusAttachment**)&mBus);

        /*
         * Create a bus listener class
         */
        AutoPtr<IBusListener> bl = new InnerBusListener(this);
        mBus->RegisterBusListener(bl);

        /* To communicate with AllJoyn objects, we must connect the BusAttachment to the bus. */
        ECode ec = mBus->Connect();
        mHost->LogStatus(String("BusAttachment.connect()"), ec);
        if ((ECode)E_STATUS_OK != ec) {
            mHost->Finish();
            return NOERROR;
        }

        /*
         * Now find an instance of the AllJoyn object we want to call.  We start by looking for
         * a name, then connecting to the device that is advertising that name.
         *
         * In this case, we are looking for the well-known SERVICE_NAME.
         */
        ec = mBus->FindAdvertisedName(SERVICE_NAME);
        String strMsg("");
        strMsg.AppendFormat("BusAttachement.findAdvertisedName(%s)", SERVICE_NAME.string());
        mHost->LogStatus(strMsg, ec);
        if ((ECode)E_STATUS_OK != ec) {
            mHost->Finish();
            return NOERROR;
        }

        break;
    }
    case JOIN_SESSION: {
        /*
         * If discovery is currently being stopped don't join to any other sessions.
         */
        if (mIsStoppingDiscovery) {
            break;
        }

        /*
         * In order to join the session, we need to provide the well-known
         * contact port.  This is pre-arranged between both sides as part
         * of the definition of the chat service.  As a result of joining
         * the session, we get a session identifier which we must use to
         * identify the created session communication channel whenever we
         * talk to the remote side.
         */
        Int16 contactPort = CONTACT_PORT;
        AutoPtr<ISessionOpts> sessionOpts;
        CSessionOpts::New((ISessionOpts**)&sessionOpts);
        Int32 arg1;
        msg->GetArg1(&arg1);
        sessionOpts->SetTransports(arg1);
        AutoPtr<IMutableInteger32Value> sessionId;
        CMutableInteger32Value::New((IMutableInteger32Value**)&sessionId);

        AutoPtr<IInterface> obj;
        msg->GetObj((IInterface**)&obj);
        AutoPtr<ISessionListener> sl = new InnerSessionListener(this);
        ECode ec = mBus->JoinSession(CoreUtils::Unbox(ICharSequence::Probe(obj)),
                contactPort, sessionId, sessionOpts, sl);
        Int32 value;
        sessionId->GetValue(&value);
        String strMsg("");
        strMsg.AppendFormat("BusAttachment.joinSession() - sessionId: %d", value);
        mHost->LogStatus(strMsg, ec);

        if (ec == (ECode)E_STATUS_OK) {
            /*
             * To communicate with an AllJoyn object, we create a ProxyBusObject.
             * A ProxyBusObject is composed of a name, path, sessionID and interfaces.
             *
             * This ProxyBusObject is located at the well-known SERVICE_NAME, under path
             * "/sample", uses sessionID of CONTACT_PORT, and implements the BasicInterface.
             */
            AutoPtr<IClassLoader> loader;
            mHost->GetClassLoader((IClassLoader**)&loader);
            AutoPtr<IInterfaceInfo> itfcInfo;
            loader->LoadInterface(String("Elastos.DevSamples.DFBasicDemo.BasicClient.IBasicInterface"),
                    (IInterfaceInfo**)&itfcInfo);
            AutoPtr< ArrayOf<IInterfaceInfo*> > busInterfaces = ArrayOf<IInterfaceInfo*>::Alloc(1);
            busInterfaces->Set(0, itfcInfo);
            mProxyObj = NULL;
            mBus->GetProxyBusObject(SERVICE_NAME, String("/sample"),
                    value, busInterfaces, (IProxyBusObject**)&mProxyObj);

            /* We make calls to the methods of the AllJoyn object through one of its interfaces. */
            mBasicInterface = NULL;
            mProxyObj->GetInterface(EIID_IBasicInterface, (IInterface**)&mBasicInterface);

            mSessionId = value;
            mIsConnected = TRUE;
            Boolean result;
            mHost->mHandler->SendEmptyMessage(MESSAGE_STOP_PROGRESS_DIALOG, &result);
        }
        break;
    }

    /* Release all resources acquired in the connect. */
    case DISCONNECT: {
        mIsStoppingDiscovery = TRUE;
        if (mIsConnected) {
            ECode ec = mBus->LeaveSession(mSessionId);
            mHost->LogStatus(String("BusAttachment.leaveSession()"), ec);
        }
        mBus->Disconnect();
        AutoPtr<ILooper> looper;
        GetLooper((ILooper**)&looper);
        looper->Quit();
        break;
    }

    /*
     * Call the service's Cat method through the ProxyBusObject.
     *
     * This will also print the String that was sent to the service and the String that was
     * received from the service to the user interface.
     */
    case CAT: {
//        try {
        if (mBasicInterface != NULL) {
            ECode ec = NOERROR;

            // if (0) {
                AutoPtr<IInterface> obj;
                msg->GetObj((IInterface**)&obj);
                String str = CoreUtils::Unbox(ICharSequence::Probe(obj));
                SendUiMessage(MESSAGE_PING, str + String(" and ") + str);
                String reply;
                mBasicInterface->Cat(str, str, &reply);
                SendUiMessage(MESSAGE_PING_REPLY, reply);
            // }

            // if (0) {
            //     Byte ip1 = 1, op1;
            //     Boolean ip2 = TRUE, op2;
            //     Int16 ip3 = 2, op3;
            //     Int32 ip4 = 3, op4;
            //     Int64 ip5 = 4, op5;
            //     Double ip6 = 5.5, op6;
            //     String ip7("Elastos"), op7;
            //     AutoPtr<ArrayOf<String> > ip8 = ArrayOf<String>::Alloc(2);
            //     (*ip8)[0] = "test0";
            //     (*ip8)[1] = "test1";
            //     AutoPtr<ArrayOf<String> > op8;
            //     mBasicInterface->Test(ip1, ip2, ip3, ip4, ip5, ip6, ip7, ip8, &op1, &op2,
            //         &op3, &op4, &op5, &op6, &op7, (ArrayOf<String>**)&op8);
            //     Boolean equals = TRUE;
            //     if (ip1 != op1 || ip2 != op2 || ip3 != op3 ||
            //         ip4 != op4 || ip5 != op5 || ip6 != op6 ||
            //         ip7 != op7 || op8 == NULL ||
            //         ip8->GetLength() != op8->GetLength()) {
            //         equals = FALSE;
            //     }
            //     else {
            //         for (Int32 i = 0; i < ip8->GetLength(); i++) {
            //             if ((*ip8)[i] != (*op8)[i]) {
            //                 equals = FALSE;
            //                 break;
            //             }
            //         }
            //     }
            //     if (equals) {
            //         Logger::D(TAG, "Test succeeded!");
            //     }
            //     else {
            //         Logger::E(TAG, "Test failed!");
            //         Logger::D(TAG, "Test reply Byte: %d, Boolean: %s Int16: %d Int32: %d, Int64: %lld Double: %f, String: %s",
            //             op1, (op2 ? "TURE" : "FALSE"), op3, op4, op5, op6, op7.string());
            //         Int32 len = ip8 ? ip8->GetLength() : 0;
            //         Logger::D(TAG, "ArrayOf len = %d", len);
            //         for (Int32 i = 0; i < len; i++) {
            //             Logger::D(TAG, "ArrayOf[%d] = %s", i, (*ip8)[i].string());
            //         }
            //         assert(0);
            //     }
            // }

            // if (0) {
            //     Logger::I(TAG, " ========= Test ArrayOf =============");
            //     AutoPtr<ArrayOf<Int32> > inarray = ArrayOf<Int32>::Alloc(4);
            //     for (Int32 i = 0; i < inarray->GetLength(); i++) {
            //         inarray->Set(i, i * 2);
            //     }

            //     AutoPtr<ArrayOf<Int32> > outarray;
            //     ec = mBasicInterface->TestArrayOf(inarray, (ArrayOf<Int32>**)&outarray);
            //     if (FAILED(ec)) {
            //         Logger::I(TAG, " >> TestArrayOf failed, ec=%08x", ec);
            //     }
            //     else {
            //         Logger::I(TAG, " >> TestArrayOf results:");
            //         for (Int32 i = 0; i < outarray->GetLength(); i++) {
            //             Logger::I(TAG, "    > item %d : %d", i, (*outarray)[i]);
            //         }
            //     }

            //     Logger::I(TAG, " ========= Test ArrayOf Done =============");
            // }

            // if (0) {
            //     Logger::I(TAG, " ========= Test In Map =============");
            //     AutoPtr<IMap> bytebooleanMap, int16int32Map, int64doubleMap, strstrMap;
            //     CHashMap::New((IMap**)&bytebooleanMap);
            //     CHashMap::New((IMap**)&int16int32Map);
            //     CHashMap::New((IMap**)&int64doubleMap);
            //     CHashMap::New((IMap**)&strstrMap);
            //     bytebooleanMap->Put(CoreUtils::ConvertByte('a'), CoreUtils::Convert(TRUE));
            //     bytebooleanMap->Put(CoreUtils::ConvertByte('b'), CoreUtils::Convert(FALSE));

            //     int16int32Map->Put(CoreUtils::Convert((Int16)15), CoreUtils::Convert((Int32)31));
            //     int16int32Map->Put(CoreUtils::Convert((Int16)16), CoreUtils::Convert((Int32)32));

            //     int64doubleMap->Put(CoreUtils::Convert((Int64)63), CoreUtils::Convert((Double)100.1));
            //     int64doubleMap->Put(CoreUtils::Convert((Int64)64), CoreUtils::Convert((Double)100.2));

            //     strstrMap->Put(CoreUtils::Convert("key a"), CoreUtils::Convert("value a"));
            //     strstrMap->Put(CoreUtils::Convert("key b"), CoreUtils::Convert("value b"));

            //     AutoPtr<IMap> strint32arrayMap, strstrarrayMap, strstrint32mapMap, strint32Map;
            //     CHashMap::New((IMap**)&strint32arrayMap);
            //     CHashMap::New((IMap**)&strstrarrayMap);
            //     CHashMap::New((IMap**)&strstrint32mapMap);
            //     CHashMap::New((IMap**)&strint32Map);

            //     AutoPtr<IArrayOf> int32Array;
            //     CArrayOf::New(EIID_IInteger32, 2, (IArrayOf**)&int32Array);
            //     int32Array->Set(0, CoreUtils::Convert(1));
            //     int32Array->Set(1, CoreUtils::Convert(2));
            //     strint32arrayMap->Put(CoreUtils::Convert("String-ArrayOf<Int32>"), int32Array);

            //     AutoPtr<IArrayOf> strArray;
            //     CArrayOf::New(EIID_ICharSequence, 2, (IArrayOf**)&strArray);
            //     strArray->Set(0, CoreUtils::Convert("string array item 0"));
            //     strArray->Set(1, CoreUtils::Convert("string array item 1"));
            //     strstrarrayMap->Put(CoreUtils::Convert("String-ArrayOf<String>"), strArray);

            //     strint32Map->Put(CoreUtils::Convert("key-11"), CoreUtils::Convert(11));
            //     strint32Map->Put(CoreUtils::Convert("key-12"), CoreUtils::Convert(12));
            //     strstrint32mapMap->Put(CoreUtils::Convert("String-Map<String, Int32>"), strint32Map);

            //     Logger::I(TAG, " ========= Test TestMapOne =============");
            //     Logger::I(TAG, " >> TestMapOne:"
            //         "\n      Map<Byte, Boolean>: %p, %s"
            //         "\n      Map<Int16, Int32>: %p, %s"
            //         "\n      Map<Int64, Double>: %p, %s"
            //         "\n      Map<String, String>: %p : %s",
            //         bytebooleanMap.Get(), TO_CSTR(bytebooleanMap),
            //         int16int32Map.Get(), TO_CSTR(int16int32Map),
            //         int64doubleMap.Get(), TO_CSTR(int64doubleMap),
            //         strstrMap.Get(), TO_CSTR(strstrMap));

            //     ec = mBasicInterface->TestMapOne(bytebooleanMap, int16int32Map, int64doubleMap, strstrMap);
            //     if (FAILED(ec)) {
            //         Logger::I(TAG, " >> TestMapOne failed. ec=%08x", ec);
            //     }

            //     Logger::I(TAG, " ========= Test TestMapTwo =============");
            //     Logger::I(TAG, " >> TestMapTwo:"
            //         "\n      Map<String, ArrayOf<Int32>>: %p, %s"
            //         "\n      Map<String, ArrayOf<String>>: %p, %s"
            //         "\n      Map<String, Map<String, Int32>>: %p : %s",
            //         strint32arrayMap.Get(), TO_CSTR(strint32arrayMap),
            //         strstrarrayMap.Get(), TO_CSTR(strstrarrayMap),
            //         strstrint32mapMap.Get(), TO_CSTR(strstrint32mapMap)
            //         );

            //     ec = mBasicInterface->TestMapTwo(strint32arrayMap, strstrarrayMap, strstrint32mapMap);
            //     if (FAILED(ec)) {
            //         Logger::I(TAG, " >> TestMapTwo failed. ec=%08x", ec);
            //     }
            //     Logger::I(TAG, " ========= Test In Map succeeded! =============");
            // }

            // if (0) {
            //     Logger::I(TAG, " ========= Test Out Map =============");
            //     AutoPtr<IMap> strint32arrayMap, strstrarrayMap, strstrint32mapMap;
            //     ec = mBasicInterface->TestOutMap(
            //         (IMap**)&strint32arrayMap,
            //         (IMap**)&strstrarrayMap,
            //         (IMap**)&strstrint32mapMap);
            //     if (FAILED(ec)) {
            //         Logger::I(TAG, " >> TestOutMap failed. ec=%08x", ec);
            //     }
            //     else {
            //         LogMap(strint32arrayMap, "Map<String, ArrayOf<Int32>>");
            //         LogMap(strstrarrayMap, "Map<String, ArrayOf<String>>");
            //         LogMap(strstrint32mapMap, "Map<String, Map<String, Int32>>");
            //     }

            //     Logger::I(TAG, " ========= Test Out Map succeeded! =============");
            // }

            // if (0) {
            //     Logger::I(TAG, " ========= Test Variant =============");
            //     AutoPtr<IVariant> v;
            //     // CVariant::New(CoreUtils::Convert(111), String("i"), (IVariant**)&v);
            //     CVariant::New(CoreUtils::Convert("a String"), String("s"), (IVariant**)&v);

            //     if (0) {
            //         AutoPtr<IArrayOf> int32Array;
            //         CArrayOf::New(EIID_IInteger32, 2, (IArrayOf**)&int32Array);
            //         int32Array->Set(0, CoreUtils::Convert(1));
            //         int32Array->Set(1, CoreUtils::Convert(2));
            //         CVariant::New(int32Array, String("ai"), (IVariant**)&v);
            //     }

            //     if (0) {
            //         AutoPtr<IArrayOf> strArray;
            //         CArrayOf::New(EIID_ICharSequence, 2, (IArrayOf**)&strArray);
            //         strArray->Set(0, CoreUtils::Convert("string array item 0"));
            //         strArray->Set(1, CoreUtils::Convert("string array item 1"));
            //         CVariant::New(strArray, String("as"), (IVariant**)&v);
            //     }

            //     if (1) {
            //         AutoPtr<IMap> strint32Map, strstrint32mapMap;
            //         CHashMap::New((IMap**)&strint32Map);
            //         CHashMap::New((IMap**)&strstrint32mapMap);

            //         strint32Map->Put(CoreUtils::Convert("key-11"), CoreUtils::Convert(11));
            //         strint32Map->Put(CoreUtils::Convert("key-12"), CoreUtils::Convert(12));
            //         strstrint32mapMap->Put(CoreUtils::Convert("String-Map<String, Int32>"), strint32Map);
            //         CVariant::New(strstrint32mapMap, String("a{sa{si}}"), (IVariant**)&v);
            //     }

            //     Logger::I(TAG, " >> TestVariant: %s", TO_CSTR(v));
            //     ec = mBasicInterface->TestVariant(v);
            //     if (FAILED(ec)) {
            //         Logger::I(TAG, " >> TestVariant failed. ec=%08x", ec);
            //     }

            //     Logger::I(TAG, " ========= Test Variant succeeded! =============");
            // }

            // if (1) {
            //     Logger::I(TAG, " ========= Test Out Variant =============");
            //     AutoPtr<IVariant> v;
            //     ec = mBasicInterface->TestOutVariant((IVariant**)&v);
            //     if (FAILED(ec)) {
            //         Logger::I(TAG, " >> TestOutVariant failed. ec=%08x", ec);
            //     }
            //     else {
            //         Logger::I(TAG, " >> TestOutVariant: result: %s", TO_CSTR(v));
            //     }
            //     Logger::I(TAG, " ========= Test Out Variant succeeded! =============");
            // }
        }


//        } catch (BusException ex) {
//            logException("BasicInterface.cat()", ex);
//        }
        break;
    }
    default:
        break;
    }
    return NOERROR;
}

void CClient::BusHandler::SendUiMessage(
    /* [in] */ Int32 what,
    /* [in] */ const String& str)
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(what, CoreUtils::Convert(str), (IMessage**)&msg);
    Boolean result;
    mHost->mHandler->SendMessage(msg, &result);
}

ECode CClient::BusHandler::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = "CClient::BusHandler";
    return NOERROR;
}


//======================================================================
//  CClient
//======================================================================
const Int32 CClient::MESSAGE_PING;
const Int32 CClient::MESSAGE_PING_REPLY;
const Int32 CClient::MESSAGE_POST_TOAST;
const Int32 CClient::MESSAGE_START_PROGRESS_DIALOG;
const Int32 CClient::MESSAGE_STOP_PROGRESS_DIALOG;
const String CClient::TAG("BasicClient");

ECode CClient::constructor()
{
    mHandler = new MyHandler(this);
    return Activity::constructor();
}

ECode CClient::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    FAIL_RETURN(Activity::OnCreate(savedInstanceState));
    SetContentView(R::layout::main);

    CArrayAdapter::New((IContext*)this, R::layout::message, (IArrayAdapter**)&mListViewArrayAdapter);
    AutoPtr<IView> v;
    FindViewById(R::id::ListView, (IView**)&v);
    mListView = IListView::Probe(v);
    IAdapterView::Probe(mListView)->SetAdapter(IAdapter::Probe(mListViewArrayAdapter));

    v = NULL;
    FindViewById(R::id::EditText, (IView**)&v);
    mEditText = IEditText::Probe(v);
    AutoPtr<IOnEditorActionListener> editorListener = new EditorListener(this);
    ITextView::Probe(mEditText)->SetOnEditorActionListener(editorListener);

    /* Make all AllJoyn calls through a separate handler thread to prevent blocking the UI. */
    AutoPtr<IHandlerThread> busThread;
    CHandlerThread::New(String("BusHandler"), (IHandlerThread**)&busThread);
    IThread::Probe(busThread)->Start();
    AutoPtr<ILooper> looper;
    busThread->GetLooper((ILooper**)&looper);
    mBusHandler = new BusHandler();
    mBusHandler->constructor(looper, this);

    /* Connect to an AllJoyn object. */
    Boolean result;
    mBusHandler->SendEmptyMessage(BusHandler::CONNECT, &result);
    mHandler->SendEmptyMessage(MESSAGE_START_PROGRESS_DIALOG, &result);
    return NOERROR;
}

ECode CClient::OnCreateOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IMenuInflater> inflater;
    GetMenuInflater((IMenuInflater**)&inflater);
    inflater->Inflate(R::menu::mainmenu, menu);
    mMenu = menu;
    *result = TRUE;
    return NOERROR;
}

ECode CClient::OnOptionsItemSelected(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // Handle item selection
    Int32 id;
    item->GetItemId(&id);
    switch (id) {
    case R::id::quit:
        Finish();
        *result = TRUE;
        return NOERROR;
    default:
        return Activity::OnOptionsItemSelected(item, result);
    }
}

ECode CClient::OnDestroy()
{
    Activity::OnDestroy();

    /* Disconnect to prevent resource leaks. */
    Boolean result;
    mBusHandler->SendEmptyMessage(BusHandler::DISCONNECT, &result);
    return NOERROR;
}

void CClient::LogStatus(
    /* [in] */ const String& msg,
    /* [in] */ ECode status)
{
    String log("");
    log.AppendFormat("%s: 0x%08x", msg.string(), status);
    if (status == (ECode)E_STATUS_OK) {
        Logger::I(TAG, log.string());
    }
    else {
        AutoPtr<IMessage> toastMsg;
        mHandler->ObtainMessage(MESSAGE_POST_TOAST, CoreUtils::Convert(log), (IMessage**)&toastMsg);
        Boolean result;
        mHandler->SendMessage(toastMsg, &result);
        Logger::E(TAG, log.string());
    }
}

void CClient::LogException(
    /* [in] */ const String& msg,
    /* [in] */ ECode ex)
{
    String log("");
    log.AppendFormat("%s: 0x%08x", msg.string(), ex);
    AutoPtr<IMessage> toastMsg;
    mHandler->ObtainMessage(MESSAGE_POST_TOAST, CoreUtils::Convert(log), (IMessage**)&toastMsg);
    Boolean result;
    mHandler->SendMessage(toastMsg, &result);
    Logger::E(TAG, log.string());
}

ECode CClient::GetClassID(
    /* [out] */ ClassID* clsid)
{
    VALIDATE_NOT_NULL(clsid);
    *clsid = ECLSID_CClient;
    return NOERROR;
}

} // namespace BasicClient
} // namespace DFBasicDemo
} // namespace DevSamples
} // namespace Elastos
