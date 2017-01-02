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

#include "Elastos.CoreLibrary.External.h"
#include "_Elastos.Droid.Content.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/app/CNotificationGroup.h"
#include "elastos/droid/os/CParcelUuid.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::IParcelUuid;
using Elastos::Droid::Os::CParcelUuid;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::CoreUtils;
using Elastos::Utility::CHashSet;
using Elastos::Utility::IUUIDHelper;
using Elastos::Utility::CUUIDHelper;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace App {

const String CNotificationGroup::TAG("CNotificationGroup");

CAR_INTERFACE_IMPL_2(CNotificationGroup, Object, INotificationGroup, IParcelable)

CAR_OBJECT_IMPL(CNotificationGroup)

CNotificationGroup::CNotificationGroup()
    : mNameResId(0)
    , mDirty(FALSE)
{
    CHashSet::New((ISet**)&mPackages);
}

ECode CNotificationGroup::constructor()
{
    return NOERROR;
}

ECode CNotificationGroup::constructor(
    /* [in] */ const String& name)
{
    return constructor(name, -1, NULL);
}

ECode CNotificationGroup::constructor(
    /* [in] */ const String& name,
    /* [in] */ Int32 nameResId,
    /* [in] */ IUUID* uuid)
{
    mName = name;
    mNameResId = nameResId;
    mUuid = uuid;
    if (mUuid == NULL) {
        AutoPtr<IUUIDHelper> helper;
        CUUIDHelper::AcquireSingleton((IUUIDHelper**)&helper);
        helper->RandomUUID((IUUID**)&mUuid);
    }
    mDirty = uuid == NULL;
    return NOERROR;
}

ECode CNotificationGroup::ToString(
    /* [out] */ String* str)
{
    return GetName(str);
}

ECode CNotificationGroup::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mName;
    return NOERROR;
}

ECode CNotificationGroup::SetName(
    /* [in] */ const String& name)
{
    mName = name;
    mNameResId = -1;
    mDirty = TRUE;
    return NOERROR;
}

ECode CNotificationGroup::GetUuid(
    /* [out] */ IUUID** uuid)
{
    VALIDATE_NOT_NULL(uuid);
    *uuid = mUuid;
    REFCOUNT_ADD(*uuid);
    return NOERROR;
}

ECode CNotificationGroup::AddPackage(
    /* [in] */ const String& pkg)
{
    mPackages->Add(CoreUtils::Convert(pkg));
    mDirty = TRUE;
    return NOERROR;
}

ECode CNotificationGroup::GetPackages(
    /* [out, callee] */ ArrayOf<String>** packages)
{
    VALIDATE_NOT_NULL(packages);
    AutoPtr< ArrayOf<IInterface*> > pkgArray;
    mPackages->ToArray((ArrayOf<IInterface*>**)&pkgArray);
    if (pkgArray == NULL) {
        *packages = NULL;
        return NOERROR;
    }
    *packages = ArrayOf<String>::Alloc(pkgArray->GetLength());
    for (Int32 i = 0; i < pkgArray->GetLength(); i++) {
        (**packages)[i] = CoreUtils::Unbox(ICharSequence::Probe((*pkgArray)[i]));
    }
    REFCOUNT_ADD(*packages);
    return NOERROR;
}

ECode CNotificationGroup::RemovePackage(
    /* [in] */ const String& pkg)
{
    mPackages->Remove(CoreUtils::Convert(pkg));
    mDirty = TRUE;
    return NOERROR;
}

ECode CNotificationGroup::HasPackage(
    /* [in] */ const String& pkg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mPackages->Contains(CoreUtils::Convert(pkg), result);
}

ECode CNotificationGroup::IsDirty(
    /* [out] */ Boolean* isDirty)
{
    VALIDATE_NOT_NULL(isDirty);
    *isDirty = mDirty;
    return NOERROR;
}

ECode CNotificationGroup::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteString(mName);
    dest->WriteInt32(mNameResId);
    dest->WriteBoolean(mDirty);
    AutoPtr<IParcelUuid> parcelUuid;
    CParcelUuid::New(mUuid, (IParcelUuid**)&parcelUuid);
    dest->WriteInterfacePtr(parcelUuid);
    AutoPtr< ArrayOf<String> > packages;
    GetPackages((ArrayOf<String>**)&packages);
    dest->WriteArrayOfString(packages);
    return NOERROR;
}

ECode CNotificationGroup::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadString(&mName);
    source->ReadInt32(&mNameResId);
    source->ReadBoolean(&mDirty);
    AutoPtr<IInterface> obj;
    source->ReadInterfacePtr((Handle32*)&obj);
    IParcelUuid::Probe(obj)->GetUuid((IUUID**)&mUuid);
    AutoPtr< ArrayOf<String> > packages;
    source->ReadArrayOfString((ArrayOf<String>**)&packages);
    for (Int32 i = 0; i < packages->GetLength(); i++) {
        mPackages->Add(CoreUtils::Convert((*packages)[i]));
    }
    return NOERROR;
}

ECode CNotificationGroup::GetXmlString(
    /* [in] */ IStringBuilder* builder,
    /* [in] */ IContext* context)
{
    builder->Append(String("<notificationGroup "));
    if (mNameResId > 0) {
        builder->Append(String("nameres=\""));
        AutoPtr<IResources> res;
        context->GetResources((IResources**)&res);
        String name;
        res->GetResourceEntryName(mNameResId, &name);
        builder->Append(name);
    }
    else {
        builder->Append(String("name=\""));
        String name;
        GetName(&name);
        builder->Append(TextUtils::HtmlEncode(name));
    }
    builder->Append(String("\" uuid=\""));
    AutoPtr<IUUID> uuid;
    GetUuid((IUUID**)&uuid);
    String uuidStr;
    uuid->ToString(&uuidStr);
    builder->Append(TextUtils::HtmlEncode(uuidStr));
    builder->Append(String("\">\n"));
    AutoPtr<IIterator> it;
    mPackages->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        String pkg = CoreUtils::Unbox(ICharSequence::Probe(obj));
        builder->Append(String("<package>") + TextUtils::HtmlEncode(pkg) + String("</package>\n"));
    }
    builder->Append(String("</notificationGroup>\n"));
    mDirty = FALSE;
    return NOERROR;
}

ECode CNotificationGroup::FromXml(
    /* [in] */ IXmlPullParser* xpp,
    /* [in] */ IContext* context,
    /* [out] */ INotificationGroup** group)
{
    VALIDATE_NOT_NULL(group);

    String value;
    xpp->GetAttributeValue(String(NULL), String("nameres"), &value);

    Int32 nameResId = -1;
    String name;
    AutoPtr<IUUID> uuid;

    if (!value.IsNull()) {
        AutoPtr<IResources> res;
        context->GetResources((IResources**)&res);
        res->GetIdentifier(value, String("string"), String("android"), &nameResId);
        if (nameResId > 0) {
            res->GetString(nameResId, &name);
        }
    }

    if (name.IsNull()) {
        xpp->GetAttributeValue(String(NULL), String("name"), &name);
    }

    xpp->GetAttributeValue(String(NULL), String("uuid"), &value);
    if (!value.IsNull()) {
        // try {
        AutoPtr<IUUIDHelper> helper;
        CUUIDHelper::AcquireSingleton((IUUIDHelper**)&helper);
        ECode ec = helper->FromString(value, (IUUID**)&uuid);
        if (FAILED(ec)) {
            Logger::W(TAG, "UUID not recognized for %s, using new one.", name.string());
        }
        // } catch (IllegalArgumentException e) {
        //     Log.w(TAG, "UUID not recognized for " + name + ", using new one.");
        // }
    }

    AutoPtr<CNotificationGroup> notificationGroup;
    CNotificationGroup::NewByFriend(name, nameResId, uuid, (CNotificationGroup**)&notificationGroup);
    Int32 event;
    xpp->Next(&event);
    while (event != IXmlPullParser::END_TAG || (xpp->GetName(&name), !name.Equals("notificationGroup"))) {
        if (event == IXmlPullParser::START_TAG) {
            if (xpp->GetName(&name), name.Equals("package")) {
                String pkg;
                xpp->NextText(&pkg);
                notificationGroup->AddPackage(pkg);
            }
        }
        xpp->Next(&event);
    }

    /* we just loaded from XML, no need to save */
    notificationGroup->mDirty = FALSE;

    *group = (INotificationGroup*)notificationGroup.Get();
    REFCOUNT_ADD(*group);
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
