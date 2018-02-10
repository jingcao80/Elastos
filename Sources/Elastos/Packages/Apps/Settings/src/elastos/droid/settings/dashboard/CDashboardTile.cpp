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

#include "elastos/droid/settings/dashboard/CDashboardTile.h"
#include "elastos/droid/text/TextUtils.h"

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Text::TextUtils;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Dashboard {

const Int64 CDashboardTile::TILE_ID_UNDEFINED = -1;

CAR_INTERFACE_IMPL(CDashboardTile, Object, IParcelable);

CAR_OBJECT_IMPL(CDashboardTile)

CDashboardTile::CDashboardTile()
    : mId(TILE_ID_UNDEFINED)
    , mTitleRes(0)
    , mSummaryRes(0)
    , mIconRes(0)
{}

CDashboardTile::~CDashboardTile()
{}

ECode CDashboardTile::constructor()
{
    return NOERROR;
}

AutoPtr<ICharSequence> CDashboardTile::GetTitle(
    /* [in] */ IResources* res)
{
    if (mTitleRes != 0) {
        AutoPtr<ICharSequence> seq;
        res->GetText(mTitleRes, (ICharSequence**)&seq);
        return seq;
    }
    return mTitle;
}

AutoPtr<ICharSequence> CDashboardTile::GetSummary(
    /* [in] */ IResources* res)
{
    if (mSummaryRes != 0) {
        AutoPtr<ICharSequence> seq;
        res->GetText(mSummaryRes, (ICharSequence**)&seq);
        return seq;
    }
    return mSummary;
}

ECode CDashboardTile::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt64(mId);
    dest->WriteInt32(mTitleRes);
    TextUtils::WriteToParcel(mTitle, dest);
    dest->WriteInt32(mSummaryRes);
    TextUtils::WriteToParcel(mSummary, dest);
    dest->WriteInt32(mIconRes);
    dest->WriteString(mFragment);
    dest->WriteInterfacePtr(mFragmentArguments);
    if (mIntent != NULL) {
        dest->WriteInt32(1);
        IParcelable::Probe(mIntent)->WriteToParcel(dest);
    }
    else {
        dest->WriteInt32(0);
    }
    dest->WriteInterfacePtr(mExtras);
    return NOERROR;
}

ECode CDashboardTile::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    in->ReadInt64(&mId);
    in->ReadInt32(&mTitleRes);
    TextUtils::CHAR_SEQUENCE_CREATOR::CreateFromParcel(in, (ICharSequence**)&mTitle);
    in->ReadInt32(&mSummaryRes);
    TextUtils::CHAR_SEQUENCE_CREATOR::CreateFromParcel(in, (ICharSequence**)&mSummary);
    in->ReadInt32(&mIconRes);
    in->ReadString(&mFragment);
    in->ReadInterfacePtr((HANDLE*)(IBundle**)&mFragmentArguments);
    Int32 data;
    if ((in->ReadInt32(&data), data) != 0) {
        CIntent::New((IIntent**)&mIntent);
        IParcelable::Probe(mIntent)->ReadFromParcel(in);
    }
    in->ReadInterfacePtr((HANDLE*)(IBundle**)&mExtras);
    return NOERROR;
}

} // namespace Dashboard
} // namespace Settings
} // namespace Droid
} // namespace Elastos
