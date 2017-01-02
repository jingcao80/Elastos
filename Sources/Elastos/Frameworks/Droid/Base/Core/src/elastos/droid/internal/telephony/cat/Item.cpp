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

#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/cat/Item.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

////=====================================================================
////                     Item::InnerParcelableCreator
////=====================================================================
//Item::InnerParcelableCreator::InnerParcelableCreator(
//    /* [in] */ Item* owner)
//    : mOwner(owner)
//{
//    // ==================before translated======================
//    // mOwner = owner;
//}
//
//ECode Item::InnerParcelableCreator::CreateFromParcel(
//    /* [in] */ IParcel* in,
//    /* [out] */ Item** result)
//{
//    VALIDATE_NOT_NULL(result);
//    // ==================before translated======================
//    // return new Item(in);
//    assert(0);
//    return NOERROR;
//}
//
//ECode Item::InnerParcelableCreator::NewArray(
//    /* [in] */ Int32 size,
//    /* [out] */ Item[]** result)
//{
//    VALIDATE_NOT_NULL(result);
//    // ==================before translated======================
//    // return new Item[size];
//    assert(0);
//    return NOERROR;
//}
//
////=====================================================================
////                                 Item
////=====================================================================
//const AutoPtr<IParcelable> AutoPtr< ::Creator<Item> > Item::CREATOR = new InnerParcelableCreator(this);
CAR_INTERFACE_IMPL_2(Item, Object, IItem, IParcelable);

Item::Item()
{
}

ECode Item::constructor()
{
    constructor(-1, String(NULL));
    return NOERROR;
}

ECode Item::constructor(
    /* [in] */ Int32 id,
    /* [in] */ const String& text)
{
    mId = id;
    mText = text;
    mIcon = NULL;
    return NOERROR;
}

//ECode Item::DescribeContents(
//    /* [out] */ Int32* result)
//{
//    VALIDATE_NOT_NULL(result);
//    // ==================before translated======================
//    // return 0;
//    assert(0);
//    return NOERROR;
//}

ECode Item::WriteToParcel(
    /* [in] */ IParcel* dest)
    ///* [in] */ Int32 flags)
{
    dest->WriteInt32(mId);
    dest->WriteString(mText);
    if (mIcon != NULL) {
        dest->WriteBoolean(TRUE);
        IParcelable::Probe(mIcon)->WriteToParcel(dest);
    }
    else {
        dest->WriteBoolean(FALSE);
    }
    return NOERROR;
}

ECode Item::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    in->ReadInt32(&mId);
    in->ReadString(&mText);
    Boolean flag = FALSE;
    in->ReadBoolean(&flag);
    if (flag) {
        IParcelable::Probe(mIcon)->ReadFromParcel(in);
    }
    return NOERROR;
}

ECode Item::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mText;
    return NOERROR;
}

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
