
#include "CService.h"
#include "CRawService.h"
#include "R.h"
#include "Elastos.CoreLibrary.IO.h"
#include <elastos/droid/os/Looper.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/Thread.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Widget::CArrayAdapter;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Core::CoreUtils;
using Elastos::Core::CThread;
using Elastos::Core::StringBuilder;
using Elastos::Core::Thread;
using Elastos::IO::CBufferedReader;
using Elastos::IO::CFileDescriptor;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::CFileInputStream;
using Elastos::IO::CInputStreamReader;
using Elastos::IO::IInputStream;
using Elastos::Utility::Logging::Logger;

using Org::Alljoyn::Bus::CBusAttachment;
using Org::Alljoyn::Bus::CBusListener;
using Org::Alljoyn::Bus::CMutableInteger16Value;
using Org::Alljoyn::Bus::CMutableInteger32Value;
using Org::Alljoyn::Bus::CSessionOpts;
using Org::Alljoyn::Bus::EIID_IBusObject;
using Org::Alljoyn::Bus::IBusListener;
using Org::Alljoyn::Bus::IMutableInteger16Value;
using Org::Alljoyn::Bus::IMutableInteger32Value;
using Org::Alljoyn::Bus::ISessionOpts;
using Org::Alljoyn::Bus::ISessionPortListener;
using Org::Alljoyn::Bus::RemoteMessage_Receive;
using Org::Alljoyn::Bus::Alljoyn::IDaemonInit;
using Org::Alljoyn::Bus::Alljoyn::CDaemonInit;

namespace Elastos {
namespace DevSamples {
namespace DFRawDemo {
namespace RawService {

//======================================================================
//  CService::MyHander
//======================================================================
CService::MyHandler::MyHandler(
    /* [in] */ CService* host)
    : Handler(FALSE)
    , mHost(host)
{}

ECode CService::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);

    switch (what) {
    case MESSAGE_PING: {
        AutoPtr<IInterface> obj;
        msg->GetObj((IInterface**)&obj);
        String cat = CoreUtils::Unbox(ICharSequence::Probe(obj));
        mHost->mListViewArrayAdapter->Add(CoreUtils::Convert(String("Ping:  ") + cat));
        break;
    }
    case MESSAGE_PING_REPLY: {
        AutoPtr<IInterface> obj;
        msg->GetObj((IInterface**)&obj);
        String reply = CoreUtils::Unbox(ICharSequence::Probe(obj));
        mHost->mListViewArrayAdapter->Add(CoreUtils::Convert(String("Reply:  ") + reply));
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
    default:
        break;
    }
    return NOERROR;
}

//======================================================================
//  CService::RawService
//======================================================================
CAR_INTERFACE_IMPL_2(CService::RawService, Object, IRawInterface, IBusObject)
CAR_OBJECT_IMPL(CRawService)

ECode CService::RawService::constructor(
    /* [in] */ IObject* host)
{
    mHost = (CService*)host;
    return NOERROR;
}

ECode CService::RawService::RequestRawSession(
    /* [out] */ Int16* port)
{
    VALIDATE_NOT_NULL(port);
    *port = BusHandler::RAW_PORT;
    return NOERROR;
}

//======================================================================
//  CService::BusHandler::InnerSessionPortListener
//======================================================================
ECode CService::BusHandler::InnerSessionPortListener::AcceptSessionJoiner(
    /* [in] */ Int16 sessionPort,
    /* [in] */ const String& joiner,
    /* [in] */ ISessionOpts* opts,
    /* [out] */ Boolean* accepted)
{
    VALIDATE_NOT_NULL(accepted);
    Logger::I(TAG, "BusListener.AcceptSessionJoiner(%d, %s, %s): accepted on CONTACT_PORT",
        sessionPort, joiner.string(), TO_CSTR(opts));
    *accepted = TRUE;
    return NOERROR;
}

ECode CService::BusHandler::InnerSessionPortListener::SessionJoined(
    /* [in] */ Int16 sessionPort,
    /* [in] */ Int32 sessionId,
    /* [in] */ const String& joiner)
{
    if (mAnother) {
        Logger::D(TAG, "BusListener.sessionJoined(%d, %d, %s): on RAW_PORT",
                sessionPort, sessionId, joiner.string());
        mHost->mRawSessionId = sessionId;
        Boolean res;
        mHost->SendEmptyMessage(BusHandler::JOINED, &res);
        return NOERROR;
    }

    return SessionPortListener::SessionJoined(sessionPort, sessionId, joiner);
}

CService::BusHandler::ReaderRunnable::ReaderRunnable(
    /* [in] */ IReader* reader,
    /* [in] */ IHandler* handler)
    : mReader(reader)
    , mHandler(handler)
{}

ECode CService::BusHandler::ReaderRunnable::Run()
{
    Logger::I(TAG, "Thread running");
    Looper::GetMyLooper();
    Looper::Prepare();
    StringBuilder stringBuilder;
    // try {
        while (TRUE) {
            Int32 c;
            /*
             * Wait until a character is available.
             */
            Boolean res;
            mReader->IsReady(&res);
            if (!res) {
                Thread::Sleep(100);
                continue;
            }

            /*
             * Build a string out of the characters and
             * when we see a newline, cook up a toast
             * do display them.
             */
            if (FAILED(mReader->Read(&c)))
                continue;
            Logger::I(TAG, "Read %d from stream", c);
            stringBuilder.AppendChar((Char32)c);
            if (c == 10) {
                String s = stringBuilder.ToString();
                Logger::I(TAG, "Read %s from stream", s.string());
                AutoPtr<IMessage> toastMsg;
                mHandler->ObtainMessage(MESSAGE_POST_TOAST, CoreUtils::Convert(s), (IMessage**)&toastMsg);
                Boolean result;
                mHandler->SendMessage(toastMsg, &result);
                stringBuilder.Delete(0, stringBuilder.GetLength() - 1);
            }
        }
    // } catch (Throwable ex) {
    //     Logger::I(TAG, String.format("Exception reading raw Java stream: %s", ex.toString()));
    // }
    Logger::I(TAG, "Thread exiting");

    return NOERROR;
}

//======================================================================
//  CService::BusHandler
//======================================================================
const Int32 CService::BusHandler::CONNECT = 1;
const Int32 CService::BusHandler::DISCONNECT = 2;
const Int32 CService::BusHandler::JOINED = 3;
const String CService::BusHandler::SERVICE_NAME("org.alljoyn.bus.samples.raw");
const Int16 CService::BusHandler::CONTACT_PORT = 88;
const String CService::BusHandler::RAW_SERVICE_NAME("org.alljoyn.bus.samples.yadda888");
const Int16 CService::BusHandler::RAW_PORT = 888;

ECode CService::BusHandler::constructor(
    /* [in] */ ILooper* looper,
    /* [in] */ CService* host)
{
    Handler::constructor(looper);

    mStreamUp = FALSE;
    mRawSessionId = -1;
    mHost = host;
    return NOERROR;
}

ECode CService::BusHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch(what) {
    /* Connect to the bus and start our service. */
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
         * If using the debug version of the AllJoyn libraries, tell
         * them to write debug output to the OS log so we can see it
         * using adb logcat.  Turn on all of the debugging output from
         * the Java language bindings (module ALLJOYN_JAVA).  When one
         * builds the samples in alljoyn_java, the library appropriate
         * to the build variant is copied in; so if the variant is
         * debug, the following will work.
         */
        mBus->UseOSLogging(TRUE);
        mBus->SetDebugLevel(String("ALLJOYN_JAVA"), 7);

        /*
         * To make a service available to other AllJoyn peers, first
         * register a BusObject with the BusAttachment at a specific
         * object path.  Our service is implemented by the RawService
         * BusObject found at the "/RawService" object path.
         */
        ECode ec = mBus->RegisterBusObject(mHost->mRawService, String("/RawService"));
        mHost->LogStatus(String("BusAttachment.registerBusObject()"), ec);
        if (ec != (ECode)E_STATUS_OK) {
            mHost->Finish();
            return NOERROR;
        }

        /*
         * The next step in making a service available to other AllJoyn peers is to connect the
         * BusAttachment to the bus with a well-known name.
         */
        /*
         * connect the BusAttachement to the bus
         */
        ec = mBus->Connect();
        mHost->LogStatus(String("BusAttachment.connect()"), ec);
        if (ec != (ECode)E_STATUS_OK) {
            mHost->Finish();
            return NOERROR;
        }

        /*
         * We now request a well-known name from the bus.  This is an
         * alias for the unique name which we are automatically given
         * when we hook up to the bus.  This name must be unique across
         * the distributed bus and acts as the human-readable name of
         * our service.
         *
         * We have the oppportunity to ask the underlying system to
         * queue our request to be granted the well-known name, but we
         * decline this opportunity so we will fail if another instance
         * of this service is already running.
         */
        Int32 flag = IBusAttachment::ALLJOYN_REQUESTNAME_FLAG_DO_NOT_QUEUE;

        ec = mBus->RequestName(SERVICE_NAME, flag);
        mHost->LogStatus(String::Format("BusAttachment.requestName(%s, 0x%08x)", SERVICE_NAME.string(), flag), ec);
        if (ec != (ECode)E_STATUS_OK) {
            mHost->Finish();
            return NOERROR;
        }

        /*
         * TODO: Remove this second advertisement which is used until
         * we can fix the ephemeral session port problem.
         */
        ec = mBus->RequestName(RAW_SERVICE_NAME, flag);
        mHost->LogStatus(String::Format("BusAttachment.requestName(%s, 0x%08x)", RAW_SERVICE_NAME.string(), flag), ec);
        if (ec != (ECode)E_STATUS_OK) {
            mHost->Finish();
            return NOERROR;
        }

        /*
         * If we sucessfully receive permission to use the requested
         * service name, we need to Create a new session listening on
         * the contact port of the raw service.  The default session
         * options are sufficient for our purposes
         */
        AutoPtr<IMutableInteger16Value> contactPort;
        CMutableInteger16Value::New(CONTACT_PORT, (IMutableInteger16Value**)&contactPort);

        AutoPtr<ISessionOpts> sessionOpts;
        CSessionOpts::New((ISessionOpts**)&sessionOpts);
        AutoPtr<ISessionPortListener> spl = new InnerSessionPortListener(this);
        ec = mBus->BindSessionPort(contactPort, sessionOpts, spl);
        Int16 value;
        contactPort->GetValue(&value);
        mHost->LogStatus(String::Format("BusAttachment.bindSessionPort(%d, %s)", value, TO_CSTR(sessionOpts)), ec);
        if (ec != (ECode)E_STATUS_OK) {
            mHost->Finish();
            return NOERROR;
        }

        /*
         * Create a new raw session listening on the another port.
         * This can be done on-demand in a "real" application, but it
         * is convenient to do the job as a well-known contact port
         * here.  We will just return this well-known port as if it
         * were an ephemeral session port when our (single) client asks
         * for a new raw session.
         *
         * We have to change the session options object to reflect that
         * it is a raw session we want to bind.  The difference is in
         * the traffic flowing across the session, so we need to change
         * the traffic type to RAW_RELIABLE, which will imply TCP, for
         * example, if we are using an IP based transport mechanism.
         */
        contactPort->SetValue(RAW_PORT);
        sessionOpts->SetTraffic(ISessionOpts::TRAFFIC_RAW_RELIABLE);
        spl = new InnerSessionPortListener(this, TRUE);
        ec = mBus->BindSessionPort(contactPort, sessionOpts, spl);
        contactPort->GetValue(&value);
        mHost->LogStatus(String::Format("BusAttachment.bindSessionPort(%d, %s)", value, TO_CSTR(sessionOpts)), ec);
        if (ec != (ECode)E_STATUS_OK) {
            mHost->Finish();
            return NOERROR;
        }

        /*
         * Advertise the same well-known name so future clients can
         * discover our existence.
         */
        ec = mBus->AdvertiseName(SERVICE_NAME, ISessionOpts::TRANSPORT_ANY);
        mHost->LogStatus(String::Format("BusAttachement.advertiseName(%s)", SERVICE_NAME.string()), ec);
        if (ec != (ECode)E_STATUS_OK) {
            mHost->Finish();
            return NOERROR;
        }

        ec = mBus->AdvertiseName(RAW_SERVICE_NAME, ISessionOpts::TRANSPORT_ANY);
        mHost->LogStatus(String::Format("BusAttachement.advertiseName(%s)", SERVICE_NAME.string()), ec);
        if (ec != (ECode)E_STATUS_OK) {
            mHost->Finish();
            return NOERROR;
        }
        break;
    }
    /*
     * We have a new joiner with a given mRawSessionId that has joined
     * our raw session.  This is provided to us in the sessionJoined
     * callback we provide during binding.
     *
     * The session we got is in raw mode, but we need to get a socket
     * file descriptor back from AllJoyn that represents the TCP
     * established connection.  Once we have the sock FD we are then
     * free to do whatever we want with it.
     *
     * What we are going to do with the sock FD is to create a Java
     * file descriptor out of it, and then use that Java FD to create
     * an input stream.
     */
    case JOINED: {
        /*
         * Get the socket FD from AllJoyn.  It is a socket FD just
         * any other file descriptor.
         */
        AutoPtr<IMutableInteger32Value> sockFd;
        CMutableInteger32Value::New((IMutableInteger32Value**)&sockFd);
        ECode ec = mBus->GetSessionFd(mRawSessionId, sockFd);
        mHost->LogStatus(String("BusAttachment.getSession()"), ec);
        // try {
            AutoPtr<IFileDescriptor> fd;
            CFileDescriptor::New((IFileDescriptor**)&fd);
            Int32 value;
            sockFd->GetValue(&value);
            fd->SetDescriptor(value);

            /*
             * Now that we have a FileDescriptor with an AllJoyn
             * raw session socket FD in it, we can use it like
             * any other "normal" FileDescriptor.
             */
            AutoPtr<IInputStream> is;
            CFileInputStream::New(fd, (IInputStream**)&is);
            AutoPtr<IReader> isReader;
            CInputStreamReader::New(is, (IReader**)&isReader);
            AutoPtr<IReader> reader;
            CBufferedReader::New(isReader, 80, (IReader**)&reader);

            /*
             * We don't want to block the AllJoyn message handler so we
             * spin up a thread to read characters from the raw reliable
             * stream.  This is far from a real example of something
             * useful to do with the stream, but we do show bytes moving
             * across the underlying TCP connection.
             */
            Logger::I(TAG, "Spinning up read thread");


            AutoPtr<ReaderRunnable> runnable = new ReaderRunnable(reader, mHost->mHandler);
            CThread::New(runnable, String("reader"), (IThread**)&mReader);
            mReader->Start();

        // } catch (Throwable ex) {
        //     Logger::I(TAG, "Exception Bringing up raw Java stream");
        // }
        break;
    }
    /* Release all resources acquired in connect. */
    case DISCONNECT: {
        /*
         * It is important to unregister the BusObject before disconnecting from the bus.
         * Failing to do so could result in a resource leak.
         */
        mBus->UnregisterBusObject(mHost->mRawService);
        mBus->Disconnect();
        AutoPtr<ILooper> looper;
        mHost->mBusHandler->GetLooper((ILooper**)&looper);
        looper->Quit();
        break;
    }

    default:
        break;
    }
    return NOERROR;
}

//======================================================================
//  CService
//======================================================================
const String CService::TAG("RawService");
const Int32 CService::MESSAGE_PING;
const Int32 CService::MESSAGE_PING_REPLY;
const Int32 CService::MESSAGE_POST_TOAST;

ECode CService::constructor()
{
    mHandler = new MyHandler(this);
    return Activity::constructor();
}

ECode CService::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    FAIL_RETURN(Activity::OnCreate(savedInstanceState));
    SetContentView(R::layout::main);

    CArrayAdapter::New((IContext*)this, R::layout::message, (IArrayAdapter**)&mListViewArrayAdapter);
    AutoPtr<IView> v;
    FindViewById(R::id::ListView, (IView**)&v);
    mListView = IListView::Probe(v);
    IAdapterView::Probe(mListView)->SetAdapter(IAdapter::Probe(mListViewArrayAdapter));

    /*
     * You always need to make all AllJoyn calls through a separate handler
     * thread to prevent blocking the Activity.
     */
    AutoPtr<IHandlerThread> busThread;
    CHandlerThread::New(String("BusHandler"), (IHandlerThread**)&busThread);
    IThread::Probe(busThread)->Start();
    AutoPtr<ILooper> looper;
    busThread->GetLooper((ILooper**)&looper);
    mBusHandler = new BusHandler();
    mBusHandler->constructor(looper, this);

    /*
     * Create a ervice bus object and send a message to the AllJoyn bus
     * handler asking it to do whatever it takes to start the service
     * and make it available out to the rest of the world.
     */
    AutoPtr<IRawInterface> bi;
    CRawService::New(this, (IRawInterface**)&bi);
    mRawService = (RawService*)bi.Get();
    Boolean result;
    mBusHandler->SendEmptyMessage(BusHandler::CONNECT, &result);
    return NOERROR;
}

ECode CService::OnCreateOptionsMenu(
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

ECode CService::OnOptionsItemSelected(
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

ECode CService::OnDestroy()
{
    Activity::OnDestroy();

    /* Disconnect to prevent resource leaks. */
    Boolean result;
    mBusHandler->SendEmptyMessage(BusHandler::DISCONNECT, &result);
    return NOERROR;
}

void CService::LogStatus(
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

} // namespace RawService
} // namespace DFRawDemo
} // namespace DevSamples
} // namespace Elastos
