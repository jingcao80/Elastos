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

#include "elastos/droid/launcher2/ItemInfo.h"
#include "elastos/droid/launcher2/LauncherModel.h"
#include "elastos/droid/launcher2/LauncherSettings.h"
#include "elastos/droid/os/Process.h"
#include "Elastos.Droid.Service.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Graphics::BitmapCompressFormat_PNG;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Os::Process;
using Elastos::Core::StringBuilder;
using Elastos::IO::ICloseable;
using Elastos::IO::IFlushable;
using Elastos::IO::IOutputStream;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::CByteArrayOutputStream;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(ItemInfo, Object, IItemInfo);

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
}

ECode ItemInfo::constructor()
{
    return Process::MyUserHandle((IUserHandle**)&mUser);
}

ECode ItemInfo::constructor(
    /* [in] */ ItemInfo* info)
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
    return LauncherModel::CheckItemInfo(this);
}

String ItemInfo::GetPackageName(
    /* [in] */ IIntent* intent)
{
    if (intent != NULL) {
        String packageName;
        intent->GetPackage(&packageName);
        if (packageName.IsNull()) {
            AutoPtr<IComponentName> componentName;
            intent->GetComponent((IComponentName**)&componentName);
            if (componentName != NULL) {
                componentName->GetPackageName(&packageName);
            }
        }
        if (!packageName.IsNull()) {
            return packageName;
        }
    }
    return String("");
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

ECode ItemInfo::OnAddToDatabase(
    /* [in] */ IContext* context,
    /* [in] */ IContentValues* values)
{
    values->Put(ILauncherSettingsBaseLauncherColumns::ITEM_TYPE, mItemType);
    values->Put(LauncherSettings::Favorites::CONTAINER, mContainer);
    values->Put(LauncherSettings::Favorites::SCREEN, mScreen);
    values->Put(LauncherSettings::Favorites::CELLX, mCellX);
    values->Put(LauncherSettings::Favorites::CELLY, mCellY);
    values->Put(LauncherSettings::Favorites::SPANX, mSpanX);
    values->Put(LauncherSettings::Favorites::SPANY, mSpanY);

    AutoPtr<IInterface> object;
    context->GetSystemService(IContext::USER_SERVICE, (IInterface**)&object);
    AutoPtr<IUserManager> manager = IUserManager::Probe(object);
    Int64 serialNumber;
    manager->GetSerialNumberForUser(mUser, &serialNumber);
    values->Put(LauncherSettings::Favorites::PROFILE_ID, serialNumber);
    return NOERROR;
}

ECode ItemInfo::UpdateValuesWithCoordinates(
    /* [in] */ IContentValues* values,
    /* [in] */ Int32 cellX,
    /* [in] */ Int32 cellY)
{
    values->Put(LauncherSettings::Favorites::CELLX, cellX);
    values->Put(LauncherSettings::Favorites::CELLY, cellY);
    return NOERROR;
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
    Boolean isSucceeded;
    bitmap->Compress(BitmapCompressFormat_PNG, 100, IOutputStream::Probe(out), &isSucceeded);
    ECode ec;
    AutoPtr<ArrayOf<Byte> > array;
    FAIL_GOTO(ec = IFlushable::Probe(out)->Flush(), ERROR)
    FAIL_GOTO(ec = ICloseable::Probe(out)->Close(), ERROR)
    FAIL_GOTO(ec = out->ToByteArray((ArrayOf<Byte>**)&array), ERROR)
    return array;
    //} catch (IOException e) {
ERROR:
    if (ec == (ECode)E_IO_EXCEPTION) {
        Slogger::W("Favorite", "Could not write icon");
        return NULL;
    }
    //}
    return NULL;
}

void ItemInfo::WriteBitmap(
    /* [in] */ IContentValues* values,
    /* [in] */ IBitmap* bitmap)
{
    if (bitmap != NULL) {
        AutoPtr<ArrayOf<Byte> > data = FlattenBitmap(bitmap);
        values->Put(LauncherSettings::Favorites::ICON, data);
    }
}

ECode ItemInfo::Unbind()
{
    return NOERROR;
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
    sb += "(";
    if (mDropPos == NULL) {
        sb += "NULL";
    }
    else {
        for (Int32 i = 0; i < mDropPos->GetLength(); i++) {
            sb += (*mDropPos)[i];
        }
    }
    sb += ")";

    sb += " user=";
    sb += TO_STR(mUser);
    sb += ")";
    return sb.ToString(str);
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos