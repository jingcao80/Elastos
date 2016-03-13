
#include "elastos/droid/settings/dashboard/DashboardCategory.h"
#include "elastos/droid/text/TextUtils.h"

using Elastos::Droid::Text::TextUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Dashboard {

const Int64 DashboardCategory::CAT_ID_UNDEFINED = -1;

CAR_INTERFACE_IMPL(DashboardCategory, Object, IParcelable);

DashboardCategory::DashboardCategory()
    : mId(CAT_ID_UNDEFINED)
    , mTitleRes(0)
{
    CArryList::New((IList**)&mTiles);
}

DashboardCategory::~DashboardCategory()
{}

ECode DashboardCategory::AddTile(
    /* [in] */ DashboardTile* tile)
{
    mTiles->Add((IObject*)tile);
    return NOERROR;
}

ECode DashboardCategory::AddTile(
    /* [in] */ Int32 n,
    /* [in] */ DashboardTile* tile)
{
    mTiles->Add(n, (IObject*)tile);
    return NOERROR;
}

ECode DashboardCategory::RemoveTile(
    /* [in] */ DashboardTile* tile)
{
    mTiles->Remove((IObject*)tile);
    return NOERROR;
}

ECode DashboardCategory::RemoveTile(
    /* [in] */ Int32 n)
{
    mTiles->Remove(n);
    return NOERROR;
}

Int32 DashboardCategory::GetTilesCount()
{
    Int32 size;
    mTiles->GetSize(&size);
    return size;
}

AutoPtr<DashboardTile> DashboardCategory::GetTile(
    /* [in] */ Int32 n)
{
    AutoPtr<IInterface> obj;
    mTiles->Get(n, (IInterface**)&obj);
    return (DashboardTile*)IObject::Probe(obj);
}

AutoPtr<ICharSequence> DashboardCategory::GetTitle(
    /* [in] */ IResources* res)
{
    if (mTitleRes != 0) {
        AutoPtr<ICharSequence> seq;
        res->GetText(mTitleRes, (ICharSequence**)&seq);
        return req;
    }
    return mTitle;
}

ECode DashboardCategory::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mTitleRes);
    TextUtils::WriteToParcel(title, dest);

    Int32 count;
    mTiles->GetSize(&count);
    dest->WriteInt32(count);

    for (Int32 n = 0; n < count; n++) {
        AutoPtr<IInterface> obj;
        mTiles->Get(n, (IInterface**)&obj);
        AutoPtr<DashboardTile> tile = (DashboardTile*)IObject::Probe(obj);
        tile->WriteToParcel(dest);
    }
    return NOERROR;
}

ECode DashboardCategory::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    in->ReadInt32(&mTitleRes);
    TextUtils::CHAR_SEQUENCE_CREATOR::CreateFromParcel(in, (ICharSequence**)&mTitle);

    Int32 count;
    in->ReadInt32(&count);

    for (Int32 n = 0; n < count; n++) {
        AutoPtr<DashboardTile> tile = new DashboardTile();
        IParcelable::Probe((IObject*)tile))->ReadFromParcel(in);
        mTiles->Add((IObject*)tile);
    }
    return NOERROR;
}

} // namespace Dashboard
} // namespace Settings
} // namespace Droid
} // namespace Elastos
