#ifndef __ELASTOS_DROID_DATABASE_BULKCURSORTOCURSORADAPTOR_H__
#define __ELASTOS_DROID_DATABASE_BULKCURSORTOCURSORADAPTOR_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/database/AbstractWindowedCursor.h"

using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Database {

/**
 * Adapts an {@link IBulkCursor} to a {@link Cursor} for use in the local process.
 *
 * {@hide}
 */
class BulkCursorToCursorAdaptor
    : public AbstractWindowedCursor
{
public:
    BulkCursorToCursorAdaptor();

    CARAPI constructor();

    /**
     * Initializes the adaptor.
     * Must be called before first use.
     */
    CARAPI Initialize(
        /* [in] */ IBulkCursorDescriptor* d);

    /**
     * Gets a SelfDataChangeOberserver that can be sent to a remote
     * process to receive change notifications over IPC.
     *
     * @return A SelfContentObserver hooked up to this Cursor
     */
    CARAPI GetObserver(
        /* [out] */ IIContentObserver** observer);

    //@Override
    CARAPI GetCount(
        /* [out] */ Int32* count);

    //@Override
    CARAPI OnMove(
        /* [in] */ Int32 oldPosition,
        /* [in] */ Int32 newPosition,
        /* [out] */ Boolean* succeeded);

    //@Override
    CARAPI Deactivate();

    //@Override
    CARAPI Close();

    //@Override
    CARAPI Requery(
        /* [out] */ Boolean* succeeded);

    //@Override
    CARAPI GetColumnNames(
        /* [out] */ ArrayOf<String>** names);

    //@Override
    CARAPI GetExtras(
        /* [out] */ IBundle** extras);

    //@Override
    CARAPI Respond(
        /* [in] */ IBundle* extras,
        /* [out] */ IBundle** v);

private:
    CARAPI ThrowIfCursorIsClosed();

private:
    static const String TAG;

    AutoPtr<SelfContentObserver> mObserverBridge;
    AutoPtr<IBulkCursor> mBulkCursor;
    AutoPtr<ArrayOf<String> > mColumns;
    Boolean mWantsAllOnMoveCalls;
    Int32 mCount;
};

} //Database
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_DATABASE_BULKCURSORTOCURSORADAPTOR_H__
