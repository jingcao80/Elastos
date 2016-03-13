
#include "elastos/droid/settings/dashboard/DashboardTile.h"
#include "elastos/droid/text/TextUtils.h"

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Text::TextUtils;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Dashboard {

const Int64 DashboardTile::TILE_ID_UNDEFINED = -1;

CAR_INTERFACE_IMPL(DashboardTile, Object, IParcelable);

DashboardTile::DashboardTile()
    : mId(TILE_ID_UNDEFINED)
    , mTitleRes(0)
    , mSummaryRes(0)
    , mIconRes(0)
{}

DashboardTile::~DashboardTile()
{}

AutoPtr<ICharSequence>) DashboardTile::GetTitle(
    /* [in] */ IResources* res)
{
    if (mTitleRes != 0) {
        AutoPtr<ICharSequence> seq;
        res->GetText(mTitleRes, (ICharSequence**)&seq);
        return seq;
    }
    return mTitle;
}

AutoPtr<ICharSequence> DashboardTile::GetSummary(
    /* [in] */ IResources* res)
{
    if (mSummaryRes != 0) {
        AutoPtr<ICharSequence> seq;
        res->GetText(mSummaryRes, (ICharSequence**)&seq);
        return seq;
    }
    return mSummary;
}

ECode DashboardTile::WriteToParcel(
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

ECode DashboardTile::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    in->ReadInt64(&mId);
    in->ReadInt32(&mTitleRes);
    TextUtils::CHAR_SEQUENCE_CREATOR::CreateFromParcel(in, (ICharSequence**)&mTitle);
    in->ReadInt32(&mSummaryRes);
    TextUtils::CHAR_SEQUENCE_CREATOR::CreateFromParcel(in, (ICharSequence**)&mSummary);
    in->ReadInt32(&mIconRes);
    in->ReadString(&mFragment);
    in->ReadInterfacePtr((Handle32*)(IBundle**)&mFragmentArguments);
    Int32 data;
    if ((in->ReadInt32(&data), data) != 0) {
        CIntent::New((IIntent**)&mIntent);
        IParcelable::Probe(mIntent)->ReadFromParcel(in);
    }
    in->ReadInterfacePtr((Handle32*)(IBundle**)&mExtras);
    return NOERROR;
}

} // namespace Dashboard
} // namespace Settings
} // namespace Droid
} // namespace Elastos
