
#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/content/pm/CParceledListSlice.h"
#include "elastos/droid/os/CParcel.h"
#include <binder/Parcel.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Os::CParcel;

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
    // final int N = p.readInt();
    // mList = new ArrayList<T>(N);
    // if (DEBUG) Log.d(TAG, "Retrieving " + N + " items");
    // if (N <= 0) {
    //     return;
    // }
    // Parcelable.Creator<T> creator = p.readParcelableCreator(loader);
    // int i = 0;
    // while (i < N) {
    //     if (p.readInt() == 0) {
    //         break;
    //     }
    //     mList.add(p.readCreator(creator, loader));
    //     if (DEBUG) Log.d(TAG, "Read inline #" + i + ": " + mList.get(mList.size()-1));
    //     i++;
    // }
    // if (i >= N) {
    //     return;
    // }
    // final IBinder retriever = p.readStrongBinder();
    // while (i < N) {
    //     if (DEBUG) Log.d(TAG, "Reading more @" + i + " of " + N + ": retriever=" + retriever);
    //     Parcel data = Parcel.obtain();
    //     Parcel reply = Parcel.obtain();
    //     data.writeInt(i);
    //     try {
    //         retriever.transact(IBinder.FIRST_CALL_TRANSACTION, data, reply, 0);
    //     } catch (RemoteException e) {
    //         Log.w(TAG, "Failure retrieving array; only received " + i + " of " + N, e);
    //         return;
    //     }
    //     while (i < N && reply.readInt() != 0) {
    //         mList.add(reply.readCreator(creator, loader));
    //         if (DEBUG) Log.d(TAG, "Read extra #" + i + ": " + mList.get(mList.size()-1));
    //         i++;
    //     }
    //     reply.recycle();
    //     data.recycle();
    // }
    return NOERROR;
}

ECode CParceledListSlice::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    // final int N = mList.size();
    // final int callFlags = flags;
    // dest.writeInt(N);
    // if (DEBUG) Log.d(TAG, "Writing " + N + " items");
    // if (N > 0) {
    //     final Class<?> listElementClass = mList.get(0).getClass();
    //     dest.writeParcelableCreator(mList.get(0));
    //     int i = 0;
    //     while (i < N && dest.dataSize() < MAX_FIRST_IPC_SIZE) {
    //         dest.writeInt(1);
    //
    //         final T parcelable = mList.get(i);
    //         verifySameType(listElementClass, parcelable.getClass());
    //         parcelable.writeToParcel(dest, callFlags);
    //
    //         mList.get(i).writeToParcel(dest, callFlags);
    //         if (DEBUG) Log.d(TAG, "Wrote inline #" + i + ": " + mList.get(i));
    //         i++;
    //     }
    //     if (i < N) {
    //         dest.writeInt(0);
    //         Binder retriever = new Binder() {
    //             @Override
    //             protected boolean onTransact(int code, Parcel data, Parcel reply, int flags)
    //                     throws RemoteException {
    //                 if (code != FIRST_CALL_TRANSACTION) {
    //                     return super.onTransact(code, data, reply, flags);
    //                 }
    //                 int i = data.readInt();
    //                 if (DEBUG) Log.d(TAG, "Writing more @" + i + " of " + N);
    //                 while (i < N && reply.dataSize() < MAX_IPC_SIZE) {
    //                     reply.writeInt(1);
    //                     final T parcelable = mList.get(i);
    //                     verifySameType(listElementClass, parcelable.getClass());
    //                     parcelable.writeToParcel(reply, callFlags);
    //                     mList.get(i).writeToParcel(reply, callFlags);
    //                     if (DEBUG) Log.d(TAG, "Wrote extra #" + i + ": " + mList.get(i));
    //                     i++;
    //                 }
    //                 if (i < N) {
    //                     if (DEBUG) Log.d(TAG, "Breaking @" + i + " of " + N);
    //                     reply.writeInt(0);
    //                 }
    //                 return true;
    //             }
    //         };
    //         if (DEBUG) Log.d(TAG, "Breaking @" + i + " of " + N + ": retriever=" + retriever);
    //         dest.writeStrongBinder(retriever);
    //     }
    // }

    return NOERROR;
}

ECode CParceledListSlice::VerifySameType(
    /* [in] */ IInterface* expected,
    /* [in] */ IInterface* actual)
{
    Boolean equals;
    if (IObject::Probe(actual)->Equals(expected, &equals), !equals) {
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
