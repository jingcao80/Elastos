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

#include "Elastos.Droid.Content.h"
#include "elastos/droid/settings/dashboard/CDashboardCategory.h"
#include "elastos/droid/text/TextUtils.h"

using Elastos::Droid::Text::TextUtils;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Dashboard {

const Int64 CDashboardCategory::CAT_ID_UNDEFINED = -1;

CAR_INTERFACE_IMPL(CDashboardCategory, Object, IParcelable);

CAR_OBJECT_IMPL(CDashboardCategory)

CDashboardCategory::CDashboardCategory()
    : mId(CAT_ID_UNDEFINED)
    , mTitleRes(0)
{
}

CDashboardCategory::~CDashboardCategory()
{}

ECode CDashboardCategory::constructor()
{
    CArrayList::New((IList**)&mTiles);
    return NOERROR;
}

ECode CDashboardCategory::AddTile(
    /* [in] */ CDashboardTile* tile)
{
    mTiles->Add((IObject*)tile);
    return NOERROR;
}

ECode CDashboardCategory::AddTile(
    /* [in] */ Int32 n,
    /* [in] */ CDashboardTile* tile)
{
    mTiles->Add(n, (IObject*)tile);
    return NOERROR;
}

ECode CDashboardCategory::RemoveTile(
    /* [in] */ CDashboardTile* tile)
{
    mTiles->Remove((IObject*)tile);
    return NOERROR;
}

ECode CDashboardCategory::RemoveTile(
    /* [in] */ Int32 n)
{
    mTiles->Remove(n);
    return NOERROR;
}

Int32 CDashboardCategory::GetTilesCount()
{
    Int32 size;
    mTiles->GetSize(&size);
    return size;
}

AutoPtr<CDashboardTile> CDashboardCategory::GetTile(
    /* [in] */ Int32 n)
{
    AutoPtr<IInterface> obj;
    mTiles->Get(n, (IInterface**)&obj);
    return (CDashboardTile*)IObject::Probe(obj);
}

AutoPtr<ICharSequence> CDashboardCategory::GetTitle(
    /* [in] */ IResources* res)
{
    if (mTitleRes != 0) {
        AutoPtr<ICharSequence> seq;
        res->GetText(mTitleRes, (ICharSequence**)&seq);
        return seq;
    }
    return mTitle;
}

ECode CDashboardCategory::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mTitleRes);
    TextUtils::WriteToParcel(mTitle, dest);

    Int32 count;
    mTiles->GetSize(&count);
    dest->WriteInt32(count);

    for (Int32 n = 0; n < count; n++) {
        AutoPtr<IInterface> obj;
        mTiles->Get(n, (IInterface**)&obj);
        IParcelable::Probe(obj)->WriteToParcel(dest);
    }
    return NOERROR;
}

ECode CDashboardCategory::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    in->ReadInt32(&mTitleRes);
    TextUtils::CHAR_SEQUENCE_CREATOR::CreateFromParcel(in, (ICharSequence**)&mTitle);

    Int32 count;
    in->ReadInt32(&count);

    for (Int32 n = 0; n < count; n++) {
        AutoPtr<CDashboardTile> tile;
        CDashboardTile::NewByFriend((CDashboardTile**)&tile);
        tile->ReadFromParcel(in);
        mTiles->Add((IObject*)tile);
    }
    return NOERROR;
}

} // namespace Dashboard
} // namespace Settings
} // namespace Droid
} // namespace Elastos
