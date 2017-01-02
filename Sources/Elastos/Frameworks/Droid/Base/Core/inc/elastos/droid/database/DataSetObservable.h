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

#ifndef __ELASTOS_DROID_DATABASE_DATASETOBSERVABLE_H__
#define __ELASTOS_DROID_DATABASE_DATASETOBSERVABLE_H__

#include "elastos/droid/database/Observable.h"

namespace Elastos {
namespace Droid {
namespace Database {
/**
 * A specialization of Observable for DataSetObserver that provides methods for
 * invoking the various callback methods of DataSetObserver.
 */
class DataSetObservable
    : public Observable
    , public IDataSetObservable
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Invokes onChanged on each observer. Called when the data set being observed has
     * changed, and which when read contains the new state of the data.
     */
    virtual CARAPI NotifyChanged();

    /**
     * Invokes onInvalidated on each observer. Called when the data set being monitored
     * has changed such that it is no longer valid.
     */
    virtual CARAPI NotifyInvalidated();
};

} //Database
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_DATABASE_DATASETOBSERVABLE_H__
