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

#ifndef __ELASTOS_DROID_DATABASE_OBSERVABLE_H__
#define __ELASTOS_DROID_DATABASE_OBSERVABLE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Database.h"
#include <elastos/utility/etl/List.h>
#include <elastos/core/Object.h>

using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace Database {
/**
 * Provides methods for (un)registering arbitrary observers in an ArrayList.
 */
class Observable
    : public Object
    , public IObservable
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Adds an observer to the list. The observer cannot be null and it must not already
     * be registered.
     * @param observer the observer to register
     * @throws IllegalArgumentException the observer is null
     * @throws IllegalStateException the observer is already registered
     */
    virtual CARAPI RegisterObserver(
        /* [in] */ IInterface* observer);

    /**
     * Removes a previously registered observer. The observer must not be null and it
     * must already have been registered.
     * @param observer the observer to unregister
     * @throws IllegalArgumentException the observer is null
     * @throws IllegalStateException the observer is not yet registered
     */
    virtual CARAPI UnregisterObserver(
        /* [in] */ IInterface* observer);

    /**
     * Remove all registered observers.
     */
    virtual CARAPI UnregisterAll();

protected:
    /**
     * The list of observers.  An observer can be in the list at most
     * once and will never be NULL.
     */
    List< AutoPtr<IInterface> > mObservers;
    Object mObserversLock;
};

} //Database
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_DATABASE_OBSERVABLE_H__
