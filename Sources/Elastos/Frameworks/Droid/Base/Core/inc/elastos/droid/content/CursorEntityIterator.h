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

#ifndef __ELASTOS_DROID_CONTENT_CURSORENTITYITERATOR_H__
#define __ELASTOS_DROID_CONTENT_CURSORENTITYITERATOR_H__

#include "Elastos.Droid.Content.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Database::ICursor;
using Elastos::IO::ICloseable;

namespace Elastos {
namespace Droid {
namespace Content {

class CursorEntityIterator
    : public Object
    , public ICursorEntityIterator
{
public:
    CAR_INTERFACE_DECL()

    CursorEntityIterator();

    virtual ~CursorEntityIterator();

    /**
     * Constructor that makes initializes the cursor such that the iterator points to the
     * first Entity, if there are any.
     * @param cursor the cursor that contains the rows that make up the entities
     */
    CARAPI constructor(
        /* [in] */ ICursor* cursor);

    /**
     * Returns true if there is at least one more element, false otherwise.
     * @see #next
     */
    CARAPI HasNext(
        /* [out] */ Boolean* value);

    /**
     * Returns the next object and advances the iterator.
     *
     * @return the next object.
     * @throws NoSuchElementException
     *             if there are no more elements.
     * @see #hasNext
     */
    CARAPI GetNext(
        /* [out] */ IEntity** entity);

    /**
     * Removes the last object returned by {@code next} from the collection.
     * This method can only be called once between each call to {@code next}.
     *
     * @throws UnsupportedOperationException
     *             if removing is not supported by the collection being
     *             iterated.
     * @throws IllegalStateException
     *             if {@code next} has not been called, or {@code remove} has
     *             already been called after the last call to {@code next}.
     */
    CARAPI Remove();

    /**
     * Reset the iterator back to the beginning.
     */
    CARAPI Reset();

    /**
     * Indicates that this iterator is no longer needed and that any associated resources
     * may be released (such as a SQLite cursor).
     */
    CARAPI Close();

    /**
     * Returns the entity that the cursor is currently pointing to. This must take care to advance
     * the cursor past this entity. This will never be called if the cursor is at the end.
     * @param cursor the cursor that contains the entity rows
     * @return the entity that the cursor is currently pointing to
     * @throws RemoteException if a RemoteException is caught while attempting to build the Entity
     */
    virtual CARAPI GetEntityAndIncrementCursor(
        /* [in] */ ICursor* cursor,
        /* [out] */ IEntity** entity) = 0;

private:
    AutoPtr<ICursor> mCursor;
    Boolean mIsClosed;

};

} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_CONTENT_CURSORENTITYITERATOR_H__
