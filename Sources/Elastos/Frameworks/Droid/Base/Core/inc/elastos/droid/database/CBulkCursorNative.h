#ifndef __ELASTOS_DROID_DATABASE_CBULKCURSORNATIVE_H__
#define __ELASTOS_DROID_DATABASE_CBULKCURSORNATIVE_H__

#include "_Elastos_Droid_Database_CBulkCursorNative.h"
#include "elastos/droid/database/BulkCursorNative.h"

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Database::IIContentObserver;

namespace Elastos {
namespace Droid {
namespace Database {

CarClass(CBulkCursorNative), public BulkCursorNative
{
public:
    CARAPI_(PInterface) Probe(
        /* [in]  */ REIID riid);

    CARAPI OnTransact(
        /* [in] */ Int32 code,
        /* [in] */ IParcel* data,
        /* [in] */ IParcel* reply,
        /* [in] */ Int32 flags,
        /* [out] */ Boolean * result);

    CARAPI AsBinder(
        /* [out] */ IBinder** result);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI GetWindow(
        /* [in] */ Int32 position,
        /* [out] */ ICursorWindow** result);

    CARAPI OnMove(
        /* [in] */ Int32 position);

    CARAPI Deactivate();

    CARAPI Close();

    CARAPI Requery(
        /* [in] */ IIContentObserver* observer,
        /* [out] */ Int32* result);

    CARAPI GetExtras(
        /* [out] */ IBundle** extras);

    CARAPI Respond(
        /* [in] */ IBundle* extras,
        /* [out] */ IBundle** result);

    CARAPI constructor();

private:
};

} //Database
} //Droid
} //Elastos

#endif // __ELASTOS_DROID_DATABASE_CBULKCURSORNATIVE_H__
