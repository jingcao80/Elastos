#include "ChatManager.h"
#include "CActivityOne.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::ICharSequence;
using Elastos::Core::CStringWrapper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Os::CMessageHelper;


namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace WifiP2pServiceDemo {

// 319214e9-58c2-4719-9cba-84eac3134acc
extern "C" const InterfaceID EIID_ChatManager =
    { 0x319214e9, 0x58c2, 0x4719, { 0x9c, 0xba, 0x84, 0xea, 0xc3, 0x13, 0x4a, 0xcc } };

const String ChatManager::TAG("ChatHandler");

PInterface ChatManager::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    else if (riid == EIID_IRunnable) {
        return this;
    }
    else if (riid == EIID_ChatManager) {
        return reinterpret_cast<PInterface>(this);
    }

    return NULL;
}

UInt32 ChatManager::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 ChatManager::Release()
{
    return ElRefBase::Release();
}

ECode ChatManager::GetInterfaceID(
    /* [in] */ IInterface* pObject,
    /* [out] */ InterfaceID* pIID)
{
    if (pIID == NULL) {
        return E_INVALID_ARGUMENT;
    }

    if (pObject == (IInterface*)this) {
        *pIID = EIID_IRunnable;
    }
    else {
        return E_INVALID_ARGUMENT;
    }
    return NOERROR;
}

ChatManager::ChatManager(
    /* [in] */ ISocket* socket,
    /* [in] */ IHandler* handler)
    : mSocket(socket)
    , mHandler(handler)
{}

ECode ChatManager::Run()
{
    // try {
    mInputStream = NULL;
    mOutputStream = NULL;
    AutoPtr<ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(1024);
    Int32 bytes;

    mSocket->GetInputStream((IInputStream**)&mInputStream);
    mSocket->GetOutputStream((IOutputStream**)&mOutputStream);

    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    AutoPtr<IMessage> msg;
    helper->Obtain(mHandler, CActivityOne::MY_HANDLE, TO_IINTERFACE(this),
        (IMessage**)&msg);
    msg->SendToTarget();

    ECode ec = NOERROR;
    while (TRUE) {
        // try {
            // Read from the InputStream
            ec = mInputStream->ReadBytes(buffer, &bytes);
            if (FAILED(ec)) break;

            if (bytes == -1) {
                break;
            }

            String content((const char*)buffer->GetPayload(), bytes);
            // Send the obtained bytes to the UI Activity
            Slogger::D(TAG, "Rec: %d", content.string());

            AutoPtr<ICharSequence> seq;
            CStringWrapper::New(content, (ICharSequence**)&seq);

            AutoPtr<IMessage> readMsg;
            helper->Obtain(mHandler, CActivityOne::MESSAGE_READ, seq, (IMessage**)&readMsg);
            readMsg->SendToTarget();
        // } catch (IOException e) {
        //     Log.e(TAG, "disconnected", e);
        // }
    }

    if (mSocket != NULL) {
        mSocket->Close();
    }
    // } catch (IOException e) {
    //     e.printStackTrace();
    // } finally {
    //     try {
    //         socket.close();
    //     } catch (IOException e) {
    //         e.printStackTrace();
    //     }
    // }
    return ec;
}

void ChatManager::Write(
    /* [in] */ ArrayOf<Byte>* buffer)
{
    assert(buffer != NULL);
    if (mOutputStream != NULL) {
        ECode ec = mOutputStream->WriteBytes(*buffer);
        if (FAILED(ec)) {
            Slogger::E(TAG, "Exception during write");
        }
    }
}

} // namespace WifiP2pServiceDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
