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

#include "elastos/droid/content/pm/CPackageInstallerSession.h"
#include "elastos/droid/os/FileBridge.h"
#include "elastos/droid/os/CParcelFileDescriptorAutoCloseInputStream.h"

using Elastos::Droid::Os::CParcelFileDescriptorAutoCloseInputStream;
using Elastos::Droid::Os::FileBridge;
using Elastos::Droid::Os::IFileBridgeOutputStream;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::IO::EIID_ICloseable;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CAR_INTERFACE_IMPL_2(CPackageInstallerSession, Object, IPackageInstallerSession, ICloseable)

CAR_OBJECT_IMPL(CPackageInstallerSession)

CPackageInstallerSession::CPackageInstallerSession()
{}

CPackageInstallerSession::~CPackageInstallerSession()
{}

ECode CPackageInstallerSession::constructor()
{
    return NOERROR;
}

/** {@hide} */
ECode CPackageInstallerSession::constructor(
    /* [in] */ IIPackageInstallerSession* session)
{
    mSession = session;
    return NOERROR;
}

/** {@hide} */
ECode CPackageInstallerSession::SetProgress(
    /* [in] */ Float progress)
{
    return SetStagingProgress(progress);
}

ECode CPackageInstallerSession::SetStagingProgress(
    /* [in] */ Float progress)
{
    // try {
    return mSession->SetClientProgress(progress);
    // } catch (RemoteException e) {
    //     throw e.rethrowAsRuntimeException();
    // }
    return NOERROR;
}

/** {@hide} */
ECode CPackageInstallerSession::AddProgress(
    /* [in] */ Float progress)
{
    // try {
    mSession->AddClientProgress(progress);
    // } catch (RemoteException e) {
    //     throw e.rethrowAsRuntimeException();
    // }
    return NOERROR;
}

ECode CPackageInstallerSession::OpenWrite(
    /* [in] */ const String& name,
    /* [in] */ Int64 offsetBytes,
    /* [in] */ Int64 lengthBytes,
    /* [out] */ IOutputStream** stream)
{
    VALIDATE_NOT_NULL(stream)
    // try {
    AutoPtr<IParcelFileDescriptor> clientSocket;
    mSession->OpenWrite(name, offsetBytes, lengthBytes, (IParcelFileDescriptor**)&clientSocket);
    AutoPtr<IOutputStream> os = new FileBridge::FileBridgeOutputStream(clientSocket);
    *stream = os;
    REFCOUNT_ADD(*stream)
    return NOERROR;
    // } catch (RuntimeException e) {
    //     ExceptionUtils.maybeUnwrapIOException(e);
    //     throw e;
    // } catch (RemoteException e) {
    //     throw e.rethrowAsRuntimeException();
    // }
}

ECode CPackageInstallerSession::Fsync(
    /* [in] */ IOutputStream* stream)
{
    IFileBridgeOutputStream* fb = IFileBridgeOutputStream::Probe(stream);
    if (fb != NULL) {
        FileBridge::FileBridgeOutputStream* fbos = (FileBridge::FileBridgeOutputStream*)fb;
        return fbos->Fsync();
    }

    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode CPackageInstallerSession::GetNames(
    /* [out, callee] */ ArrayOf<String>** names)
{
    // try {
    return mSession->GetNames(names);
    // } catch (RuntimeException e) {
    //     ExceptionUtils.maybeUnwrapIOException(e);
    //     throw e;
    // } catch (RemoteException e) {
    //     throw e.rethrowAsRuntimeException();
    // }
}

ECode CPackageInstallerSession::OpenRead(
    /* [in] */ const String& name,
    /* [out] */ IInputStream** stream)
{
    VALIDATE_NOT_NULL(stream)
    // try {
    AutoPtr<IParcelFileDescriptor> pfd;
    mSession->OpenRead(name, (IParcelFileDescriptor**)&pfd);
    return CParcelFileDescriptorAutoCloseInputStream::New(pfd, stream);
    // } catch (RuntimeException e) {
    //     ExceptionUtils.maybeUnwrapIOException(e);
    //     throw e;
    // } catch (RemoteException e) {
    //     throw e.rethrowAsRuntimeException();
    // }
}

ECode CPackageInstallerSession::Commit(
    /* [in] */ IIntentSender* statusReceiver)
{
    // try {
    return mSession->Commit(statusReceiver);
    // } catch (RemoteException e) {
    //     throw e.rethrowAsRuntimeException();
    // }
    return NOERROR;
}

ECode CPackageInstallerSession::Close()
{
    // try {
    return mSession->Close();
    // } catch (RemoteException e) {
    //     throw e.rethrowAsRuntimeException();
    // }
}

ECode CPackageInstallerSession::Abandon()
{
    // try {
    return mSession->Abandon();
    // } catch (RemoteException e) {
    //     throw e.rethrowAsRuntimeException();
    // }
}


} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos