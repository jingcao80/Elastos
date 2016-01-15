
#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/internal/os/CTransferPipe.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/CParcelFileDescriptorHelper.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IParcelFileDescriptorHelper;
using Elastos::Droid::Os::CParcelFileDescriptorHelper;
using Elastos::Core::AutoLock;
using Elastos::Core::StringBuilder;
using Elastos::Core::EIID_IRunnable;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

const String CTransferPipe::TAG("CTransferPipe");
const Boolean CTransferPipe::DEBUG;
const Int64 CTransferPipe::DEFAULT_TIMEOUT;

CAR_INTERFACE_IMPL_2(CTransferPipe, Object, ITransferPipe, IRunnable)

CAR_OBJECT_IMPL(CTransferPipe)

CTransferPipe::CTransferPipe()
{
}

ECode CTransferPipe::constructor()
{
    FAIL_RETURN(CThread::New(this, String("CTransferPipe"), (IThread**)&mThread))
    AutoPtr<IParcelFileDescriptorHelper> helper;
    CParcelFileDescriptorHelper::AcquireSingleton((IParcelFileDescriptorHelper**)&helper);
    return helper->CreatePipe((ArrayOf<IParcelFileDescriptor*>**)&mFds);
}

AutoPtr<IParcelFileDescriptor> CTransferPipe::GetReadFd()
{
    return (*mFds)[0];
}

ECode CTransferPipe::GetWriteFd(
    /* [out] */ IParcelFileDescriptor** writeFd)
{
    VALIDATE_NOT_NULL(writeFd)
    *writeFd = (*mFds)[1];
    REFCOUNT_ADD(*writeFd)
    return NOERROR;
}

ECode CTransferPipe::SetBufferPrefix(
    /* [in] */ const String& prefix)
{
    mBufferPrefix = prefix;
    return NOERROR;
}

ECode CTransferPipe::Go(
    /* [in] */ ICaller* caller,
    /* [in] */ IInterface* iface,
    /* [in] */ IFileDescriptor* out,
    /* [in] */ const String& prefix,
    /* [in] */ ArrayOf<String>* args)
{
    return Go(caller, iface, out, prefix, args, DEFAULT_TIMEOUT);
}

ECode CTransferPipe::Go(
    /* [in] */ ICaller* caller,
    /* [in] */ IInterface* iface,
    /* [in] */ IFileDescriptor* out,
    /* [in] */ const String& prefix,
    /* [in] */ ArrayOf<String>* args,
    /* [in] */ Int64 timeout)
{
    if (IBinder::Probe(iface) != NULL) {
        // This is a local object...  just call it directly.
        // try {
            caller->Go(iface, out, prefix, args);
        // } catch (RemoteException e) {
        // }
        return NOERROR;
    }

    AutoPtr<CTransferPipe> tp;
    CTransferPipe::NewByFriend((CTransferPipe**)&tp);
    // try {
    AutoPtr<IParcelFileDescriptor> writeFd;
    tp->GetWriteFd((IParcelFileDescriptor**)&writeFd);
    AutoPtr<IFileDescriptor> fDes;
    writeFd->GetFileDescriptor((IFileDescriptor**)&fDes);
    caller->Go(iface, fDes, prefix, args);
    tp->Go(out, timeout);
    // } finally {
    tp->Kill();
    // }
    return NOERROR;
}

ECode CTransferPipe::GoDump(
    /* [in] */ IBinder* binder,
    /* [in] */ IFileDescriptor* out,
    /* [in] */ ArrayOf<String>* args)
{
    return GoDump(binder, out, args, DEFAULT_TIMEOUT);
}

ECode CTransferPipe::GoDump(
    /* [in] */ IBinder* binder,
    /* [in] */ IFileDescriptor* out,
    /* [in] */ ArrayOf<String>* args,
    /* [in] */ Int64 timeout)
{
    if (binder != NULL) {
        // This is a local object...  just call it directly.
        // try {
    // TODO:
    // binder->Dump(out, args);
        // } catch (RemoteException e) {
        // }
        return NOERROR;
    }

    AutoPtr<CTransferPipe> tp;
    CTransferPipe::NewByFriend((CTransferPipe**)&tp);
    // try {
        // TODO:
        // binder->DumpAsync(tp.getWriteFd().getFileDescriptor(), args);
    tp->Go(out, timeout);
    // } finally {
    tp->Kill();
    // }
    return NOERROR;
}

ECode CTransferPipe::Go(
    /* [in] */ IFileDescriptor* out)
{
    return Go(out, DEFAULT_TIMEOUT);
}

ECode CTransferPipe::Go(
    /* [in] */ IFileDescriptor* out,
    /* [in] */ Int64 timeout)
{
    // try {
    {
        AutoLock lock(this);
        mOutFd = out;
        mEndTime = SystemClock::GetUptimeMillis() + timeout;

        if (DEBUG) {
            AutoPtr<IParcelFileDescriptor> writeFd;
            GetWriteFd((IParcelFileDescriptor**)&writeFd);
            String strReadFd, strWriteFd, strOut;
            IObject::Probe(GetReadFd())->ToString(&strReadFd);
            IObject::Probe(writeFd)->ToString(&strWriteFd);
            IObject::Probe(out)->ToString(&strOut);
            Slogger::I(TAG, "read=%s write=%s out=%s", strReadFd.string(), strWriteFd.string(), strOut.string());
        }

        // Close the write fd, so we know when the other side is done.
        CloseFd(1);

        mThread->Start();

        while (mFailure.IsNull() && !mComplete) {
            Int64 waitTime = mEndTime - SystemClock::GetUptimeMillis();
            if (waitTime <= 0) {
                if (DEBUG) Slogger::I(TAG, "TIMEOUT!");
                mThread->Interrupt();
                // throw new IOException("Timeout");
                Kill();
                return E_IO_EXCEPTION;
            }

            // try {
            Wait(waitTime);
            // } catch (InterruptedException e) {
            // }
        }

        if (DEBUG) Slogger::I(TAG, "Finished: %s", mFailure.string());
        if (!mFailure.IsNull()) {
            // throw new IOException(mFailure);
            Kill();
            return E_IO_EXCEPTION;
        }
    }
    // } finally {
    Kill();
    // }
    return NOERROR;
}

ECode CTransferPipe::CloseFd(
    /* [in] */ Int32 num)
{
    if ((*mFds)[num] != NULL) {
        if (DEBUG) Slogger::I(TAG,  "Closing: %p", (*mFds)[num]);
        // try {
        (*mFds)[num]->Close();
        // } catch (IOException e) {
        // }
        mFds->Set(num, NULL);
    }
    return NOERROR;
}

ECode CTransferPipe::Kill()
{
    AutoLock lock(this);
    CloseFd(0);
    CloseFd(1);
    return NOERROR;
}

ECode CTransferPipe::Run()
{
    AutoPtr<ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(1024);
    AutoPtr<IFileInputStream> fis;
    AutoPtr<IFileOutputStream> fos;
    {
        AutoLock lock(this);
        AutoPtr<IParcelFileDescriptor> readFd = GetReadFd();
        if (readFd == NULL) {
            Slogger::W(TAG, "Pipe has been closed...");
            return NOERROR;
        }
        AutoPtr<IFileDescriptor> fDes;
        readFd->GetFileDescriptor((IFileDescriptor**)&fDes);
        CFileInputStream::New(fDes, (IFileInputStream**)&fis);
        CFileOutputStream::New(mOutFd, (IFileOutputStream**)&fos);
    }

    if (DEBUG) Slogger::I(TAG, "Ready to read pipe...");
    AutoPtr<ArrayOf<Byte> > bufferPrefix;
    Boolean needPrefix = TRUE;
    if (!mBufferPrefix.IsNull()) {
        bufferPrefix = mBufferPrefix.GetBytes();
    }

    Int32 size;
    // try {
        while (IInputStream::Probe(fis)->Read(buffer, &size), size > 0) {
            if (DEBUG) Slogger::I(TAG, "Got %d bytes", size);
            if (bufferPrefix == NULL) {
                IOutputStream::Probe(fos)->Write(buffer, 0, size);
            }
            else {
                Int32 start = 0;
                for (Int32 i = 0; i < size; i++) {
                    if ((*buffer)[i] != '\n') {
                        if (i > start) {
                            IOutputStream::Probe(fos)->Write(buffer, start, i-start);
                        }
                        start = i;
                        if (needPrefix) {
                            IOutputStream::Probe(fos)->Write(bufferPrefix);
                            needPrefix = FALSE;
                        }
                        do {
                            i++;
                        } while (i < size && (*buffer)[i] != '\n');
                        if (i < size) {
                            needPrefix = TRUE;
                        }
                    }
                }
                if (size > start) {
                    IOutputStream::Probe(fos)->Write(buffer, start, size-start);
                }
            }
        }
        if (DEBUG) Slogger::I(TAG, "End of pipe: size=%d", size);
        Boolean interrupted;
        mThread->IsInterrupted(&interrupted);
        if (interrupted) {
            if (DEBUG) Slogger::I(TAG, "Interrupted!");
        }
    // } catch (IOException e) {
    //     synchronized(this) {
    //         mFailure = e.toString();
    //         notifyAll();
    //         return;
    //     }
    // }

    AutoLock lock(this);
    mComplete = TRUE;
    NotifyAll();

    return NOERROR;
}

} // namespace Os
} // namespace Internal
} // namespace Droid
} // namespace Elastos
