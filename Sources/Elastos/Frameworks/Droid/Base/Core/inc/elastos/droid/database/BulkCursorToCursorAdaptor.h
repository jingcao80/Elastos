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
