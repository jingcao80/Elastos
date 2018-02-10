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

#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/content/pm/CParceledListSlice.h"
#include "elastos/droid/content/pm/CParceledListSliceBinder.h"
#include "elastos/droid/os/CParcel.h"
#include <binder/Parcel.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::CParcel;
using Elastos::Droid::Os::IBinder;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

String CParceledListSlice::TAG("ParceledListSlice");
Boolean CParceledListSlice::DEBUG = FALSE;
const Int32 CParceledListSlice::MAX_IPC_SIZE = 256 * 1024;
const Int32 CParceledListSlice::MAX_FIRST_IPC_SIZE = 256 * 1024 * 2;

CAR_INTERFACE_IMPL_2(CParceledListSlice, Object, IParceledListSlice, IParcelable)

CAR_OBJECT_IMPL(CParceledListSlice)

CParceledListSlice::CParceledListSlice()
    : mNumItems(0)
    , mIsLastSlice(FALSE)
{}

CParceledListSlice::~CParceledListSlice()
{}

ECode CParceledListSlice::constructor()
{
    return NOERROR;
}

ECode CParceledListSlice::constructor(
    /* [in] */ IList* list)
{
    mList = list;
    return NOERROR;
}

ECode CParceledListSlice::constructor(
    /* [in] */ IParcel* p,
    /* [in] */ Int32 numItems,
    /* [in] */ Boolean lastSlice)
{
    mNumItems = numItems;
    mIsLastSlice = lastSlice;
    return NOERROR;
}

// private ParceledListSlice(Parcel p, ClassLoader loader) {
//     final int N = p.readInt();
//     mList = new ArrayList<T>(N);
//     if (DEBUG) Log.d(TAG, "Retrieving " + N + " items");
//     if (N <= 0) {
//         return;
//     }
//     Parcelable.Creator<T> creator = p.readParcelableCreator(loader);
//     int i = 0;
//     while (i < N) {
//         if (p.readInt() == 0) {
//             break;
//         }
//         mList.add(p.readCreator(creator, loader));
//         if (DEBUG) Log.d(TAG, "Read inline #" + i + ": " + mList.get(mList.size()-1));
//         i++;
//     }
//     if (i >= N) {
//         return;
//     }
//     final IBinder retriever = p.readStrongBinder();
//     while (i < N) {
//         if (DEBUG) Log.d(TAG, "Reading more @" + i + " of " + N + ": retriever=" + retriever);
//         Parcel data = Parcel.obtain();
//         Parcel reply = Parcel.obtain();
//         data.writeInt(i);
//         try {
//             retriever.transact(IBinder.FIRST_CALL_TRANSACTION, data, reply, 0);
//         } catch (RemoteException e) {
//             Log.w(TAG, "Failure retrieving array; only received " + i + " of " + N, e);
//             return;
//         }
//         while (i < N && reply.readInt() != 0) {
//            final T parcelable = reply.readCreator(creator, loader);
//            verifySameType(listElementClass, parcelable.getClass());
//
//            mList.add(parcelable);
//
//            if (DEBUG) Log.d(TAG, "Read extra #" + i + ": " + mList.get(mList.size()-1));
//            i++;
//        }
//         reply.recycle();
//         data.recycle();
//     }
// }

ECode CParceledListSlice::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    Int32 N = 0;
    source->ReadInt32(&N);
    CArrayList::New(N, (IList**)&mList);
    if (DEBUG) Logger::D(TAG, "Retrieving %d items", N);
    if (N <= 0) {
        return NOERROR;
    }
    AutoPtr<IInterface> creator/* = source.readParcelableCreator(loader)*/;
    source->ReadInterfacePtr((HANDLE*)&creator);
    Int32 i = 0;
    AutoPtr<IClassInfo> cl = Object::GetClassInfo(creator);
    while (i < N) {
        Int32 n = 0;
        if ((source->ReadInt32(&n), n) == 0) {
            break;
        }

        AutoPtr<IInterface> newObject;
        if (FAILED(cl->CreateObject((IInterface**)&newObject))) {
            assert(0 && "TODO: has an error.");
        }

        IParcelable::Probe(newObject)->ReadFromParcel(source);
        mList->Add(newObject);
        if (DEBUG) {
            Int32 size = 0;
            mList->GetSize(&size);
            AutoPtr<IInterface> obj;
            mList->Get(size - 1, (IInterface**)&obj);
            Logger::D(TAG, "Read inline #%d: %s", i, TO_CSTR(obj));
        }
        i++;
    }
    if (i >= N) {
        return NOERROR;
    }
    AutoPtr<IInterface> retriever;
    source->ReadInterfacePtr((HANDLE*)&retriever);
    CParceledListSliceBinder* _retriever = (CParceledListSliceBinder*)IBinder::Probe(retriever);
    while (i < N) {
        if (DEBUG) Logger::D(TAG, "Reading more @%d of %d: retriever=%s", i, N, TO_CSTR(retriever));
        AutoPtr<IParcel> data;
        CParcel::New((IParcel**)&data);
        AutoPtr<IParcel> reply;
        CParcel::New((IParcel**)&reply);
        data->WriteInt32(i);
        // try {
        if (FAILED(_retriever->Transact(IBinder::FIRST_CALL_TRANSACTION, data, reply))) {
            Logger::W(TAG, "Failure retrieving array; only received %d of %d", i, N);
            return NOERROR;
        }
        // } catch (RemoteException e) {
        //     Log.w(TAG, "Failure retrieving array; only received " + i + " of " + N, e);
        //     return;
        // }
        Int32 value = 0;
        while (i < N && ((reply->ReadInt32(&value), value) != 0)) {
            AutoPtr<IInterface> newObject;
            if (FAILED(cl->CreateObject((IInterface**)&newObject))) {
                assert(0 && "TODO: has an error.");
            }

            IParcelable::Probe(newObject)->ReadFromParcel(source);
            mList->Add(newObject);
            if (DEBUG) {
                Int32 size = 0;
                mList->GetSize(&size);
                AutoPtr<IInterface> o;
                mList->Get(size - 1, (IInterface**)&o);
                Logger::D(TAG, "Read extra #%d: %s", i, TO_CSTR(o));
            }
            i++;
        }
        // reply.recycle();
        // data.recycle();
    }
    return NOERROR;
}

ECode CParceledListSlice::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    Int32 N = 0;
    mList->GetSize(&N);
    dest->WriteInt32(N);
    if (DEBUG) Logger::D(TAG, "Writing %d items", N);
    if (N > 0) {
        AutoPtr<IInterface> obj;
        mList->Get(0, (IInterface**)&obj);
        assert(IParcelable::Probe(obj) != NULL);
        dest->WriteInterfacePtr(IParcelable::Probe(obj));

        Int32 i = 0, dataSize = 0;
        dest->GetDataSize(&dataSize);
        while (i < N && dataSize < MAX_FIRST_IPC_SIZE) {
            dest->WriteInt32(1);

            AutoPtr<IInterface> parcelable;
            mList->Get(i, (IInterface**)&parcelable);
            if (FAILED(VerifySameType(obj, parcelable))) {
                assert(0 && "Can't unparcel type.");
            }
            IParcelable::Probe(parcelable)->WriteToParcel(dest);

            if (DEBUG) Logger::D(TAG, "Wrote inline #%d: %s", i, TO_CSTR(parcelable));
            i++;
        }
        if (i < N) {
            dest->WriteInt32(0);
            AutoPtr<IBinder> retriever;
            CParceledListSliceBinder::New(obj, mList, N, (IBinder**)&retriever);
            if (DEBUG) Logger::D(TAG, "Breaking @%d of %d: retriever=%s", i, N , TO_CSTR(retriever));
            dest->WriteInterfacePtr(retriever);
        }
    }

    return NOERROR;
}

ECode CParceledListSlice::VerifySameType(
    /* [in] */ IInterface* expected,
    /* [in] */ IInterface* actual)
{
    ClassID id1, id2;
    IObject::Probe(expected)->GetClassID(&id1);
    IObject::Probe(actual)->GetClassID(&id2);
    if (id1 != id2) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("Can't unparcel type "
        //         + actual.getName() + " in list of type "
        //         + expected.getName());
    }
    return NOERROR;
}

ECode CParceledListSlice::GetList(
    /* [out] */ IList** list)
{
    VALIDATE_NOT_NULL(list)
    *list = mList;
    REFCOUNT_ADD(*list)
    return NOERROR;
}

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos
