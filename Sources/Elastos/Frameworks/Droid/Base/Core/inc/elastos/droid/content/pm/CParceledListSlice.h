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

#ifndef __ELASTOS_DROID_CONTENT_PM_CPARCELEDLISTSLICE_H__
#define __ELASTOS_DROID_CONTENT_PM_CPARCELEDLISTSLICE_H__

#include "_Elastos_Droid_Content_Pm_CParceledListSlice.h"
#include <elastos/core/Object.h>

using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

/**
 * Transfer a large list of Parcelable objects across an IPC.  Splits into
 * multiple transactions if needed.
 *
 * Caveat: for efficiency and security, all elements must be the same concrete type.
 * In order to avoid writing the class name of each object, we must ensure that
 * each object is the same type, or else unparceling then reparceling the data may yield
 * a different result if the class name encoded in the Parcelable is a Base type.
 * See b/17671747.
 *
 * @hide
 */
CarClass(CParceledListSlice)
    , public Object
    , public IParceledListSlice
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CParceledListSlice();

    virtual ~CParceledListSlice();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IList* list);

    CARAPI constructor(
        /* [in] */ IParcel* p,
        /* [in] */ Int32 numItems,
        /* [in] */ Boolean lastSlice);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    /**
     * Write this to another Parcel. Note that this discards the internal Parcel
     * and should not be used anymore. This is so we can pass this to a Binder
     * where we won't have a chance to call recycle on this.
     */
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetList(
        /* [out] */ IList** list);

private:
    static CARAPI VerifySameType(
        /* [in] */ IInterface* expected,
        /* [in] */ IInterface* actual);

private:
    static String TAG;// = "ParceledListSlice";
    static Boolean DEBUG;// = FALSE;

    /*
     * TODO get this number from somewhere else. For now set it to a quarter of
     * the 1MB limit.
     */
    static const Int32 MAX_IPC_SIZE;// = 256 * 1024;
    static const Int32 MAX_FIRST_IPC_SIZE;// = MAX_IPC_SIZE / 2;

    Int32 mNumItems;

    Boolean mIsLastSlice;

    AutoPtr<IList> mList;
    friend class CParceledListSliceBinder;
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_CPARCELEDLISTSLICE_H__
