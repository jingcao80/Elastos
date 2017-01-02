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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_DURATION_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_DURATION_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

/**
  * Class for representing "Duration" object for CAT.
  *
  * {@hide}
  */
class Duration
    : public Object
    , public IDuration
    , public IParcelable
{
//private:
//    class InnerParcelableCreator
//        : public Object
//        , public Parcelable::Creator<Duration>
//    {
//    public:
//        InnerParcelableCreator(
//            /* [in] */ Duration* owner);
//
//        // @Override
//        CARAPI CreateFromParcel(
//            /* [in] */ IParcel* in,
//            /* [out] */ Duration** result);
//
//        // @Override
//        CARAPI NewArray(
//            /* [in] */ Int32 size,
//            /* [out] */ Duration[]** result);
//
//    private:
//        Duration* mOwner;
//    };

public:
    CAR_INTERFACE_DECL();

    Duration();

    CARAPI constructor();
    /**
      * @param timeInterval Between 1 and 255 inclusive.
      */
    CARAPI constructor(
        /* [in] */ Int32 timeInterval,
        /* [in] */ IDurationTimeUnit timeUnit);

    // @Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);
        ///* [in] */ Int32 flags);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    // @Override
//    CARAPI DescribeContents(
//        /* [out] */ Int32* result);

public:
    Int32 mTimeInterval;
    IDurationTimeUnit mTimeUnit;
    //static const AutoPtr<IParcelable> AutoPtr< ::Creator<Duration> > CREATOR;
};

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_DURATION_H__

