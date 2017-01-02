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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_TEXTMESSAGE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_TEXTMESSAGE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

using Elastos::Droid::Graphics::IBitmap;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

class TextMessage
    : public Object
    , public ITextMessage
    , public IParcelable
{
//private:
//    class InnerParcelableCreator
//        : public Object
//        , public Parcelable::Creator<TextMessage>
//    {
//    public:
//        InnerParcelableCreator(
//            /* [in] */ TextMessage* owner);
//
//        // @Override
//        CARAPI CreateFromParcel(
//            /* [in] */ IParcel* in,
//            /* [out] */ TextMessage** result);
//
//        // @Override
//        CARAPI NewArray(
//            /* [in] */ Int32 size,
//            /* [out] */ TextMessage[]** result);
//
//    private:
//        TextMessage* mOwner;
//    };

public:
    CAR_INTERFACE_DECL();

    TextMessage();

    CARAPI constructor();

    // @Override
    //CARAPI DescribeContents(
    //    /* [out] */ Int32* result);

    // @Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);
        ///* [in] */ Int32 flags);

    CARAPI ReadFromParcel(
            /* [in] */ IParcel* in);

public:
    String mTitle;
    String mText;
    AutoPtr<IBitmap> mIcon;
    Boolean mIconSelfExplanatory;
    Boolean mIsHighPriority;
    Boolean mResponseNeeded;
    Boolean mUserClear;
    AutoPtr<IDuration> mDuration;
    //static const AutoPtr<IParcelable> AutoPtr< ::Creator<TextMessage> > CREATOR;
};

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_TEXTMESSAGE_H__

