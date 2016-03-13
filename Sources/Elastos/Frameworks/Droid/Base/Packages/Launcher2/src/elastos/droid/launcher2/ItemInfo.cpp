
#include "elastos/droid/launcher2/ItemInfo.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

const String ItemInfo:EXTRA_PROFILE("profile");

const Int32 ItemInfo:NO_ID = -1;

ItemInfo::ItemInfo()
   : mId(NO_ID)
   , mItemType(0)
   , mContainer(NO_ID)
   , mScreen(-1)
   , mCellX(-1)
   , mCellY(-1)
   , mSpanX(1)
   , mSpanY(1)
   , mMinSpanX(1)
   , mMinSpanY(1)
   , mRequiresDbUpdate(FALSE)
{
    Process::MyUserHandle(IUserHandle**)&mUser);
}

ItemInfo::ItemInfo(
    /* [in] */ ItemInfo* info)
    : mId(NO_ID)
    , mItemType(0)
    , mContainer(NO_ID)
    , mScreen(-1)
    , mCellX(-1)
    , mCellY(-1)
    , mSpanX(1)
    , mSpanY(1)
    , mMinSpanX(1)
    , mMinSpanY(1)
    , mRequiresDbUpdate(FALSE)
{
    mId = info->mId;
    mCellX = info->mCellX;
    mCellY = info->mCellY;
    mSpanX = info->mSpanX;
    mSpanY = info->mSpanY;
    mScreen = info->mScreen;
    mItemType = info->mItemType;
    mContainer = info->mContainer;
    mUser = info->mUser;
    mContentDescription = info->mContentDescription;
    // tempdebug:
    LauncherModel::CheckItemInfo(this);
}

String ItemInfo::GetPackageName(
    /* [in] */ IIntent* intent)
{
    if (intent != NULL) {
        String packageName;
        intent->GetPackage(&packageName);
        if (packageName.IsNull()) {
            AutoPtr<IComponentName> componentName;
            intent->GetComponent(IComponentName**)&componentName);
            if (componentName != NULL) {
                componentName->GetPackageName(&packageName);
            }
        }
        if (!packageName.IsNull()) {
            return packageName;
        }
    }
    return "";
}

void ItemInfo::UpdateUser(
    /* [in] */ IIntent* intent)
{
    if (intent != NULL) {
        Boolean res;
        intent->HasExtra(EXTRA_PROFILE, &res);
        if (res) {
            AutoPtr<IParcelable> p;
            intent->GetParcelableExtra(EXTRA_PROFILE, (IParcelable**)&p);
            mUser = IUserHandle::Probe(p);
        }
    }
    return;
}

void ItemInfo::OnAddToDatabase(
    /* [in] */ IContext* context,
    /* [in] */ IContentValues* values)
{
    values->Put(LauncherSettings::BaseLauncherColumns::ITEM_TYPE, mItemType);
    values->Put(LauncherSettings::Favorites::CONTAINER, mContainer);
    values->Put(LauncherSettings::Favorites::SCREEN, mScreen);
    values->Put(LauncherSettings::Favorites::CELLX, mCellX);
    values->Put(LauncherSettings::Favorites::CELLY, mCellY);
    values->Put(LauncherSettings::Favorites::SPANX, mSpanX);
    values->Put(LauncherSettings::Favorites::SPANY, mSpanY);

    AutoPtr<IInterface> object;
    GetSystemService(IContext::USER_SERVICE, (IInterface**)&object);
    AutoPtr<IUserManager> manager = IUserManager::Probe(object);
    Int64 serialNumber;
    manager->GetSerialNumberForUser(user, &serialNumber);
    values->Put(LauncherSettings::Favorites::PROFILE_ID, serialNumber);
    return;
}

void ItemInfo::UpdateValuesWithCoordinates(
    /* [in] */ IContentValues* values,
    /* [in] */ Int32 cellX,
    /* [in] */ Int32 cellY)
{
    values->Put(LauncherSettings::Favorites::CELLX, cellX);
    values->Put(LauncherSettings::Favorites::CELLY, cellY);
    return;
}

AutoPtr<ArrayOf<Byte> > ItemInfo::FlattenBitmap(
    /* [in] */ IBitmap* bitmap)
{
    // Try go guesstimate how much space the icon will take when serialized
    // to avoid unnecessary allocations/copies during the write.
    Int32 width;
    bitmap->GetWidth(&width);
    Int32 height;
    bitmap->GetHeight(&height);
    Int32 size = width * height * 4;
    AutoPtr<IByteArrayOutputStream> out;
    CByteArrayOutputStream::New(size, (IByteArrayOutputStream**)&out);
    //try {
    bitmap->Compress(Bitmap::CompressFormat::PNG, 100, out);
    ECode ec = out->Flush();
    if (ec == (ECode)E_IO_EXCEPTION;) {
        Slogger::W("Favorite", "Could not write icon");
        return NULL;
    }
    ec = out->Close();
    if (ec == (ECode)E_IO_EXCEPTION;) {
        Slogger::W("Favorite", "Could not write icon");
        return NULL;
    }
    AutoPtr<ArrayOf<Byte> > array;
    out->ToByteArray((ArrayOf<Byte>**)&array);
    return array;
    //} catch (IOException e) {
    // if (ec == (ECode)E_IO_EXCEPTION;) {
    //     Slogger::W("Favorite", "Could not write icon");
    //     return NULL;
    // }
    //}
}

void ItemInfo::WriteBitmap(
    /* [in] */ IContentValues* values,
    /* [in] */ IBitmap* bitmap)
{
    if (bitmap != NULL) {
        AutoPtr<ArrayOf<Byte> > data = FlattenBitmap(bitmap);
        values->Put(LauncherSettings::Favorites::ICON, data);
    }
    return;
}

void ItemInfo::Unbind()
{
    return;
}

ECode ItemInfo::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuilder sb;
    sb += "Item(id=";
    sb += mId;
    sb += " type=";
    sb += mItemType;
    sb += " container=";
    sb += mContainer;
    sb += " screen=";
    sb += mScreen;
    sb += " cellX=";
    sb += mCellX;
    sb += " cellY=";
    sb += mCellY;
    sb += " spanX=";
    sb += mSpanX;
    sb += " spanY=";
    sb += mSpanY;
    sb += " dropPos=";
    sb += mDropPos;
    sb += " user=";
    sb += mUser;
    sb += ")"
    return sb.ToString(str);
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos