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

#include "SelectorImpl.h"
#include "CLibcore.h"
#include "IoUtils.h"
#include "CStructPollfd.h"
#include "OsConstants.h"
#include "IoBridge.h"
#include "SelectionKeyImpl.h"
#include "AutoLock.h"

using Libcore::IO::IOs;
using Libcore::IO::ILibcore;
using Libcore::IO::CLibcore;
using Libcore::IO::IoUtils;
using Elastos::Droid::System::IStructPollfd;
using Elastos::Droid::System::CStructPollfd;
using Elastos::Droid::System::OsConstants;
using Libcore::IO::IoBridge;
using Elastos::IO::Channels::EIID_ISelector;
using Elastos::IO::Channels::Spi::IAbstractSelectionKey;
using Elastos::IO::Channels::Spi::EIID_IAbstractSelector;
using Elastos::Utility::ICollection;
using Elastos::Utility::IList;
using Elastos::Utility::EIID_ISet;
using Elastos::Utility::EIID_ICollection;
using Elastos::Utility::EIID_IIterable;

namespace Elastos {
namespace IO {

//==========================================================
//       SelectorImpl::UnaddableSet
//==========================================================
SelectorImpl::UnaddableSet::UnaddableSet(
    /* [in] */ ISet* set)
{
    mSet = set;
}

CAR_INTERFACE_IMPL_3(SelectorImpl::UnaddableSet, Object, ISet, ICollection, IIterable)

ECode SelectorImpl::UnaddableSet::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* value)
{
    return IObject::Probe(mSet)->Equals(object, value);
}

ECode SelectorImpl::UnaddableSet::GetHashCode(
    /* [out] */ Int32* value)
{
    return IObject::Probe(mSet)->GetHashCode(value);
}

ECode SelectorImpl::UnaddableSet::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    // throw new UnsupportedOperationException();
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode SelectorImpl::UnaddableSet::AddAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* value)
{
    // throw new UnsupportedOperationException();
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode SelectorImpl::UnaddableSet::Clear()
{
    return mSet->Clear();
}

ECode SelectorImpl::UnaddableSet::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return mSet->Contains(object, result);
}

ECode SelectorImpl::UnaddableSet::ContainsAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* value)
{
    return mSet->ContainsAll(c, value);
}

ECode SelectorImpl::UnaddableSet::IsEmpty(
    /* [out] */ Boolean* result)
{
    return mSet->IsEmpty(result);
}

ECode SelectorImpl::UnaddableSet::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return mSet->Remove(object, modified);
}

ECode SelectorImpl::UnaddableSet::RemoveAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* value)
{
    return mSet->RemoveAll(c, value);
}

ECode SelectorImpl::UnaddableSet::GetSize(
    /* [out] */ Int32* size)
{
    return mSet->GetSize(size);
}

ECode SelectorImpl::UnaddableSet::GetIterator(
    /* [out] */ IIterator** it)
{
    return mSet->GetIterator(it);
}

ECode SelectorImpl::UnaddableSet::RetainAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* value)
{
    return mSet->RetainAll(c,value);
}

ECode SelectorImpl::UnaddableSet::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** outarr)
{
    return mSet->ToArray(outarr);
}

ECode SelectorImpl::UnaddableSet::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inarr,
    /* [out, callee] */ ArrayOf<IInterface*>** outarr)
{
    return mSet->ToArray(inarr, outarr);
}

//==========================================================
//       SelectorImpl
//==========================================================
ECode SelectorImpl::constructor()
{
    return NOERROR;
}

ECode SelectorImpl::constructor(
    /* [in] */ ISelectorProvider* selectorProvider)
{
    AbstractSelector::constructor(selectorProvider);
    AutoPtr<ArrayOf<IFileDescriptor*> > fds;
    FAIL_RETURN(CLibcore::sOs->Pipe((ArrayOf<IFileDescriptor*>**)&fds));

    Int32 fdint = 0;
    (*fds)[0]->GetDescriptor(&fdint);
    mWakeupIn->SetDescriptor(fdint);
    (*fds)[1]->GetDescriptor(&fdint);
    mWakeupOut->SetDescriptor(fdint);
    FAIL_RETURN(IoUtils::SetBlocking(mWakeupIn, FALSE));

    AutoPtr<IStructPollfd> structfd;
    CStructPollfd::New((IStructPollfd**)&structfd);
    Boolean isflag = FALSE;
    mPollFds->Add(structfd, &isflag);
    SetPollFd(0, mWakeupIn, OsConstants::_POLLIN, NULL);
    return NOERROR;
}

ECode SelectorImpl::Cancel(
    /* [in] */ ISelectionKey* key)
{
    return E_NOT_IMPLEMENTED;
}

ECode SelectorImpl::ImplCloseSelector()
{
    Wakeup();

    AutoLock lock(mLock);
    FAIL_RETURN(IoUtils::Close(mWakeupIn));
    FAIL_RETURN(IoUtils::Close(mWakeupOut));
    DoCancel();

    AutoPtr< ArrayOf<IInterface*> > outarr;
    mMutableKeys->ToArray((ArrayOf<IInterface*>**)&outarr);
    for (Int32 i = 0; i < outarr->GetLength(); i++) {
        AutoPtr<IAbstractSelectionKey> ask = IAbstractSelectionKey::Probe((*outarr)[i]);
        Deregister(ISelectionKey::Probe(ask));
    }
    return NOERROR;
}

ECode SelectorImpl::Register(
    /* [in] */ IAbstractSelectableChannel* asc,
    /* [in] */ Int32 operations,
    /* [in] */ IObject* attachment,
    /* [out] */ ISelectionKey** key)
{
    VALIDATE_NOT_NULL(key)
    *key = NULL;
    VALIDATE_NOT_NULL(asc)

    AutoPtr<ISelectorProvider> sp1;
    GetProvider((ISelectorProvider**)&sp1);
    AutoPtr<ISelectorProvider> sp2;
    ISelectableChannel::Probe(asc)->GetProvider((ISelectorProvider**)&sp2);
    if (!Object::Equals(sp1,sp2)) {
        // throw new IllegalSelectorException();
        return E_ILLEGAL_SELECTOR_EXCEPTION;
    }
    AutoLock lock(mLock);
    AutoPtr<SelectionKeyImpl> selectionKey = new SelectionKeyImpl(asc, operations,
            attachment, this);
    mMutableKeys->Add(TO_IINTERFACE(selectionKey));
    EnsurePollFdsCapacity();
    *key = selectionKey;
    REFCOUNT_ADD(*key)
    return NOERROR;
}

ECode SelectorImpl::GetKeys(
    /* [out] */ ISet** keySet)
{
    VALIDATE_NOT_NULL(keySet)

    FAIL_RETURN(CheckClosed());
    *keySet = mUnmodifiableKeys;
    REFCOUNT_ADD(*keySet);
    return NOERROR;
}

ECode SelectorImpl::CheckClosed()
{
    Boolean ret;
    IsOpen(&ret);
    if(!ret) {
        return E_CLOSED_SELECTOR_EXCEPTION;
    }
    return NOERROR;
}

ECode SelectorImpl::Select(
    /* [out] */ Int32* ret)
{
    return SelectInternal(-1, ret);
}

ECode SelectorImpl::Select(
    /* [in] */ Int64 timeout,
    /* [out] */ Int32* ret)
{
    if(timeout < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return SelectInternal((timeout == 0) ? -1 : timeout, ret);
}

ECode SelectorImpl::SelectNow(
    /* [out] */ Int32* ret)
{
    return SelectInternal(0, ret);
}

ECode SelectorImpl::SelectInternal(
    /* [in] */ Int64 timeout,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret)

    FAIL_RETURN(CheckClosed());
    AutoLock lock(mLock);
    DoCancel();
    Boolean isBlock = (timeout != 0);
    PreparePollFds();
    Int32 rc = -1;
    if(isBlock) {
        Begin();
    }
    AutoPtr< ArrayOf<IInterface*> > outarr;
    mPollFds->ToArray((ArrayOf<IInterface*>**)&outarr);
    AutoPtr<ArrayOf<IStructPollfd*> > array = ArrayOf<IStructPollfd*>::Alloc(outarr->GetLength());
    for (Int32 i = 0; i < outarr->GetLength(); ++i) {
        AutoPtr<IStructPollfd> it = IStructPollfd::Probe((*outarr)[i]);
        array->Set(i, it);
    }
    FAIL_RETURN(CLibcore::sOs->Poll(array, (Int32)timeout, &rc));
    if (isBlock) {
        End();
    }
    Int32 readyCount = (rc > 0) ? ProcessPollFds() : 0;
    readyCount -= DoCancel();
    *ret = readyCount;
    return NOERROR;
}

void SelectorImpl::SetPollFd(
    /* [in] */ Int32 index,
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 events,
    /* [in] */ IInterface* object)
{
    AutoPtr<IInterface> outface;
    IList::Probe(mPollFds)->Get(index, (IInterface**)&outface);
    AutoPtr<IStructPollfd> pollFd = IStructPollfd::Probe(outface);
    if (pollFd != NULL) {
        pollFd->SetFd(fd);
        pollFd->SetEvents(events);
        pollFd->SetUserData(object);
    }
}

void SelectorImpl::PreparePollFds()
{
    AutoPtr< ArrayOf<IInterface*> > outarr;
    mMutableKeys->ToArray((ArrayOf<IInterface*>**)&outarr);

    for (Int32 i = 1; i < outarr->GetLength(); i++) {
        AutoPtr<ISelectionKey> key = ISelectionKey::Probe((*outarr)[i]);
        if (key != NULL) {
            Int32 interestOps = ((SelectionKeyImpl*)key.Get())->InterestOpsNoCheck();
            Int16 eventMask = 0;
            if (((ISelectionKey::OP_ACCEPT | ISelectionKey::OP_READ) & interestOps) != 0) {
                eventMask |= OsConstants::_POLLIN;
            }
            if (((ISelectionKey::OP_CONNECT | ISelectionKey::OP_WRITE) & interestOps) != 0) {
                eventMask |= OsConstants::_POLLOUT;
            }
            if (eventMask != 0) {
                AutoPtr<ISelectableChannel> sc;
                ISelectionKey::Probe(key)->GetChannel((ISelectableChannel**)&sc);
                AutoPtr<IFileDescriptor> outfd;
                if (IFileDescriptorChannel::Probe(sc) != NULL) {
                    IFileDescriptorChannel::Probe(sc)->GetFD((IFileDescriptor**)&outfd);
                }
                SetPollFd(i, outfd, eventMask, key);
            }
        }
    }
}

void SelectorImpl::EnsurePollFdsCapacity()
{
    // We need one slot for each element of mutableKeys, plus one for the wakeup pipe.
    Int32 fdsvalue = 0;
    Int32 keysvalue = 0;
    mPollFds->GetSize(&fdsvalue);
    mMutableKeys->GetSize(&keysvalue);
    while (fdsvalue < keysvalue + 1) {
        AutoPtr<IStructPollfd> res;
        CStructPollfd::New((IStructPollfd**)&res);
        Boolean isflag = FALSE;
        mPollFds->Add(res, &isflag);
        mPollFds->GetSize(&fdsvalue);
        mMutableKeys->GetSize(&keysvalue);
    }
}

Int32 SelectorImpl::ProcessPollFds()
{
    AutoPtr< ArrayOf<IInterface*> > outarr;
    mPollFds->ToArray((ArrayOf<IInterface*>**)&outarr);
    Int16 revents = 0;
    if (IStructPollfd::Probe((*outarr)[0]) != NULL) {
        IStructPollfd::Probe((*outarr)[0])->GetRevents(&revents);
    }
    if (revents == OsConstants::_POLLIN) {
        // Read bytes from the wakeup pipe until the pipe is empty.
        AutoPtr< ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(8);
        Int32 readnum = 0;
        while (IoBridge::Read(mWakeupIn, buffer, 0, 1, &readnum), readnum > 0) {
        }
    }

    Int32 readyKeyCount = 0;
    AutoPtr<IFileDescriptor> fdvalue = 0;
    for (Int32 i = 1; i < outarr->GetLength(); ++i) {
        AutoPtr<IStructPollfd> pollFd = IStructPollfd::Probe((*outarr)[i]);
        if (pollFd != NULL) {
            pollFd->GetRevents(&revents);
            pollFd->GetFd((IFileDescriptor**)&fdvalue);
            if (revents == 0) {
                continue;
            }
            if (fdvalue == 0) {
                break;
            }

            AutoPtr<IInterface> outface;
            pollFd->GetUserData((IInterface**)&outface);
            AutoPtr<ISelectionKey> key = ISelectionKey::Probe(outface);

            pollFd->SetFd(0);
            pollFd->SetUserData(NULL);

            Int32 ops = ((SelectionKeyImpl*)key.Get())->InterestOpsNoCheck();
            Int32 selectedOp = 0;
            Boolean isflag = FALSE;
            if ((revents & OsConstants::_POLLIN) != 0) {
                selectedOp = ops & (ISelectionKey::OP_ACCEPT | ISelectionKey::OP_READ);
            }
            else if ((revents & OsConstants::_POLLOUT) != 0) {

                if (key->IsConnectable(&isflag), isflag) {
                    selectedOp = ops & ISelectionKey::OP_WRITE;
                }
                else {
                    selectedOp = ops & ISelectionKey::OP_CONNECT;
                }
            }

            if (selectedOp != 0) {
                Boolean wasSelected = 0;
                mMutableSelectedKeys->Contains(key, &wasSelected);
                Int32 opsvalue = 0;
                ISelectionKey::Probe(key)->GetReadyOps(&opsvalue);
                if (wasSelected && opsvalue != selectedOp) {
                    ((SelectionKeyImpl*)key.Get())->SetReadyOps(opsvalue | selectedOp);
                    ++readyKeyCount;
                }
                else if (!wasSelected) {
                    ((SelectionKeyImpl*)key.Get())->SetReadyOps(selectedOp);
                    mMutableSelectedKeys->Add(key, &isflag);
                    ++readyKeyCount;
                }
            }
        }
    }

    return readyKeyCount;
}

ECode SelectorImpl::SelectedKeys(
    /* [out] */ ISet** keySet)
{
    VALIDATE_NOT_NULL(keySet)
    *keySet = NULL;

    FAIL_RETURN(CheckClosed());
    *keySet = mSelectedKeys;
    REFCOUNT_ADD(*keySet);
    return NOERROR;
}

Int32 SelectorImpl::DoCancel()
{
    Int32 deselected = 0;
    AutoPtr<ISet> cancelledKeys;
    CancelledKeys((ISet**)&cancelledKeys);
    Int32 sizelen = 0;
    cancelledKeys->GetSize(&sizelen);
    if (sizelen > 0) {
        AutoPtr< ArrayOf<IInterface*> > outarr;
        cancelledKeys->ToArray((ArrayOf<IInterface*>**)&outarr);
        for (Int32 i = 0; i < outarr->GetLength(); i++) {
            AutoPtr<IInterface> currentKey = (*outarr)[i];
            Boolean isflag = FALSE;
            mMutableKeys->Remove(currentKey, &isflag);
            Deregister(ISelectionKey::Probe(currentKey));
            if (mMutableSelectedKeys->Remove(currentKey, &isflag), isflag) {
                deselected++;
            }
        }
        cancelledKeys->Clear();
    }

    return deselected;
}

ECode SelectorImpl::Wakeup()
{
    Int32 nWrite;
    AutoPtr< ArrayOf<Byte> > inbyte = ArrayOf<Byte>::Alloc(1);
    (*inbyte)[0] = 1;
    FAIL_RETURN(CLibcore::sOs->Write(mWakeupOut, inbyte, 0, 1, &nWrite));

    return NOERROR;
}

} // namespace IO
} // namespace Elastos
