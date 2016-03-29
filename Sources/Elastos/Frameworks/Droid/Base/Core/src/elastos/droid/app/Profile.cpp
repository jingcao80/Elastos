
#include "elastos/droid/app/CAirplaneModeSettings.h"
#include "elastos/droid/app/CBrightnessSettings.h"
#include "elastos/droid/app/CConnectionSettings.h"
#include "elastos/droid/app/CRingModeSettings.h"
#include "elastos/droid/app/CStreamSettings.h"
#include "elastos/droid/app/CProfileGroup.h"
#include "elastos/droid/app/Profile.h"
#include "elastos/droid/app/CProfile.h"
#include "elastos/droid/app/CProfileTrigger.h"
#include "elastos/droid/os/CParcelUuid.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>
#include <Elastos.Droid.Content.h>

using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Core::EIID_IComparable;
using Elastos::Droid::App::CAirplaneModeSettings;
using Elastos::Droid::App::CBrightnessSettings;
using Elastos::Droid::App::CConnectionSettings;
using Elastos::Droid::App::CRingModeSettings;
using Elastos::Droid::App::CStreamSettings;
using Elastos::Droid::App::CProfileGroup;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::IParcelUuid;
using Elastos::Droid::Os::CParcelUuid;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Text::TextUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::ISet;
using Elastos::Utility::IIterator;
using Elastos::Utility::IUUIDHelper;
using Elastos::Utility::CUUIDHelper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace App {

//========================================================================
// Profile::ProfileTrigger
//========================================================================
CAR_INTERFACE_IMPL_2(Profile::ProfileTrigger, Object, IProfileTrigger, IParcelable)

Profile::ProfileTrigger::ProfileTrigger()
    : mType(0)
    , mState(0)
{}

Profile::ProfileTrigger::~ProfileTrigger()
{}

ECode Profile::ProfileTrigger::constructor(
    /* [in] */ Int32 type,
    /* [in] */ const String& id,
    /* [in] */ Int32 state,
    /* [in] */ const String& name)
{
    mType = type;
    mId = id;
    mState = state;
    mName = name;
    return NOERROR;
}

ECode Profile::ProfileTrigger::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mType);
    source->ReadString(&mId);
    source->ReadInt32(&mState);
    source->ReadString(&mName);
    return NOERROR;
}

ECode Profile::ProfileTrigger::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mType);
    dest->WriteString(mId);
    dest->WriteInt32(mState);
    dest->WriteString(mName);
    return NOERROR;
}

ECode Profile::ProfileTrigger::GetType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type)
    *type = mType;
    return NOERROR;
}

ECode Profile::ProfileTrigger::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = mName;
    return NOERROR;
}

ECode Profile::ProfileTrigger::GetId(
    /* [out] */ String* id)
{
    VALIDATE_NOT_NULL(id)
    *id = mId;
    return NOERROR;
}

ECode Profile::ProfileTrigger::GetState(
    /* [out] */ Int32* state)
{
    VALIDATE_NOT_NULL(state)
    *state = mState;
    return NOERROR;
}

ECode Profile::ProfileTrigger::GetXmlString(
    /* [in] */ IStringBuilder* builder,
    /* [in] */ IContext* context)
{
    String itemType = mType == IProfileTriggerType::WIFI
            ? String("wifiAP") : String("btDevice");

    builder->Append(String("<"));
    builder->Append(itemType);
    builder->Append(String(" "));
    builder->Append(GetIdType(mType));
    builder->Append(String("=\""));
    builder->Append(mId);
    builder->Append(String("\" state=\""));
    builder->Append(mState);
    builder->Append(String("\" name=\""));
    builder->Append(mName);
    builder->Append(String("\"></"));
    builder->Append(itemType);
    builder->Append(String(">\n"));
    return NOERROR;
}

ECode Profile::ProfileTrigger::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = "Profile::ProfileTrigger";
    return NOERROR;
}

AutoPtr<IProfileTrigger> Profile::ProfileTrigger::FromXml(
        /* [in] */ IXmlPullParser* xpp,
        /* [in] */ IContext* context)
{
    String name;
    xpp->GetName(&name);
    Int32 type;

    if (name.Equals("wifiAP")) {
        type = IProfileTriggerType::WIFI;
    }
    else if (name.Equals("btDevice")) {
        type = IProfileTriggerType::BLUETOOTH;
    }
    else {
        return NULL;
    }

    String id;
    xpp->GetAttributeValue(String(NULL), GetIdType(type), &id);
    String value;
    xpp->GetAttributeValue(String(NULL), String("state"), &value);
    Int32 state = StringUtils::ParseInt32(value);
    String triggerName;
    xpp->GetAttributeValue(String(NULL), String("name"), &triggerName);
    if (triggerName.IsNull()) {
        triggerName = id;
    }

    AutoPtr<IProfileTrigger> trigger;
    CProfileTrigger::New(type, id, state, triggerName, (IProfileTrigger**)&trigger);
    return trigger;
}

String Profile::ProfileTrigger::GetIdType(
    /* [in] */ Int32 type)
{
    return type == IProfileTriggerType::WIFI ? String("ssid") : String("address");
}

//========================================================================
// Profile
//========================================================================

const String Profile::TAG("Profile");

const Int32 Profile::CONDITIONAL_TYPE = 1;

const Int32 Profile::TOGGLE_TYPE = 0;

CAR_INTERFACE_IMPL_3(Profile, Object, IProfile, IParcelable, IComparable)

Profile::Profile()
    : mNameResId(0)
    , mStatusBarIndicator(FALSE)
    , mDirty(FALSE)
    , mProfileType(0)
    , mScreenLockMode(IProfileLockMode::DEFAULT)
    , mExpandedDesktopMode(IProfileExpandedDesktopMode::DEFAULT)
{
    CArrayList::New((IArrayList**)&mSecondaryUuids);
    CHashMap::New((IHashMap**)&mProfileGroups);
    CHashMap::New((IHashMap**)&mStreams);
    CHashMap::New((IHashMap**)&mTriggers);
    CHashMap::New((IHashMap**)&mConnections);
    CRingModeSettings::New((IRingModeSettings**)&mRingMode);
    CAirplaneModeSettings::New((IAirplaneModeSettings**)&mAirplaneMode);
    CBrightnessSettings::New((IBrightnessSettings**)&mBrightness);
}

Profile::~Profile()
{}

ECode Profile::constructor(
    /* [in] */ const String& name)
{
    AutoPtr<IUUIDHelper> helper;
    CUUIDHelper::AcquireSingleton((IUUIDHelper**)&helper);
    AutoPtr<IUUID> uuid;
    helper->RandomUUID((IUUID**)&uuid);
    return constructor(name, -1, uuid);
}

ECode Profile::constructor(
    /* [in] */ const String& name,
    /* [in] */ Int32 nameResId,
    /* [in] */ IUUID* uuid)
{
    mName = name;
    mNameResId = nameResId;
    mUuid = uuid;
    mProfileType = TOGGLE_TYPE;  //Default to toggle type
    mDirty = FALSE;
    return NOERROR;
}

ECode Profile::GetTrigger(
    /* [in] */ Int32 type,
    /* [in] */ const String& id,
    /* [out] */ Int32* triggerState)
{
    VALIDATE_NOT_NULL(triggerState)

    AutoPtr<IProfileTrigger> trigger;
    if (!id.IsNull()) {
        AutoPtr<IInterface> value;
        mTriggers->Get(CoreUtils::Convert(id), (IInterface**)&value);
        trigger = IProfileTrigger::Probe(value);
    }
    if (trigger != NULL) {
        return trigger->GetState(triggerState);
    }

    *triggerState = IProfileTriggerState::DISABLED;
    return NOERROR;
}

ECode Profile::GetTriggersFromType(
    /* [in] */ Int32 type,
    /* [out] */ IArrayList** triggers)
{
    VALIDATE_NOT_NULL(triggers)
    *triggers = NULL;
    AutoPtr<IArrayList> result;
    CArrayList::New((IArrayList**)&result);
    AutoPtr<ISet> entries;
    mTriggers->GetEntrySet((ISet**)&entries);
    AutoPtr<IIterator> it;
    entries->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> item;
        it->GetNext((IInterface**)&item);
        IMapEntry* entry = IMapEntry::Probe(item);
        AutoPtr<IInterface> vo;
        entry->GetValue((IInterface**)&vo);
        IProfileTrigger* trigger = IProfileTrigger::Probe(vo);
        Int32 triggerType;
        trigger->GetType(&triggerType);
        if (triggerType == type) {
            result->Add(trigger);
        }
    }

    *triggers = result;
    REFCOUNT_ADD(*triggers);
    return NOERROR;
}

ECode Profile::SetTrigger(
    /* [in] */ Int32 type,
    /* [in] */ const String& id,
    /* [in] */ Int32 state,
    /* [in] */ const String& name)
{
    if (id.IsNull()
            || type < IProfileTriggerType::WIFI
            || type > IProfileTriggerType::BLUETOOTH
            || state < IProfileTriggerState::ON_CONNECT
            || state > IProfileTriggerState::ON_A2DP_DISCONNECT) {
        return NOERROR;
    }

    AutoPtr<ICharSequence> key = CoreUtils::Convert(id);
    AutoPtr<IInterface> value;
    mTriggers->Get(key.Get(), (IInterface**)&value);
    IProfileTrigger* trigger = IProfileTrigger::Probe(value);

    if (state == IProfileTriggerState::DISABLED) {
        if (trigger != NULL) {
            mTriggers->Remove(key);
        }
    }
    else if (trigger != NULL) {
        ((ProfileTrigger*)trigger)->mState = state;
    }
    else {
        AutoPtr<IProfileTrigger> pt;
        CProfileTrigger::New(type, id, state, name, (IProfileTrigger**)&pt);
        mTriggers->Put(key, pt);
    }

    mDirty = TRUE;
    return NOERROR;
}

ECode Profile::CompareTo(
    /* [in] */ IInterface* obj,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IProfile> tmp = IProfile::Probe(obj);
    VALIDATE_NOT_NULL(tmp)

    if (mName.Compare(((Profile*)tmp.Get())->mName) < 0) {
        *result = -1;
    }
    else if (mName.Compare(((Profile*)tmp.Get())->mName) > 0) {
        *result = 1;
    }
    else {
        *result = 0;
    }
    return NOERROR;
}

ECode Profile::AddProfileGroup(
    /* [in] */ IProfileGroup* value)
{
    Boolean isDefault;
    if (value->IsDefaultGroup(&isDefault), isDefault) {
        /* we must not have more than one default group */
        if (mDefaultGroup != NULL) {
            return NOERROR;
        }
        mDefaultGroup = value;
    }
    AutoPtr<IUUID> uuid;
    value->GetUuid((IUUID**)&uuid);
    mProfileGroups->Put(uuid, value);
    mDirty = TRUE;
    return NOERROR;
}

ECode Profile::RemoveProfileGroup(
    /* [in] */ IUUID* uuid)
{
    AutoPtr<IInterface> value;
    mProfileGroups->Get(uuid, (IInterface**)&value);
    IProfileGroup* group = IProfileGroup::Probe(value);
    Boolean isDefault;
    if (group->IsDefaultGroup(&isDefault), !isDefault) {
        mProfileGroups->Remove(uuid);
    }
    else {
        String str;
        uuid->ToString(&str);
        Logger::E(TAG, "Cannot remove default group: %s\n", str.string());
    }

    return NOERROR;
}

ECode Profile::GetProfileGroups(
    /* [out, callee] */ ArrayOf<IProfileGroup*>** groups)
{
    VALIDATE_NOT_NULL(groups)
    Int32 size;
    mProfileGroups->GetSize(&size);
    AutoPtr<ICollection> collection;
    mProfileGroups->GetValues((ICollection**)&collection);
    AutoPtr<ArrayOf<IInterface*> > array;
    collection->ToArray((ArrayOf<IInterface*>**)&array);
    AutoPtr<ArrayOf<IProfileGroup*> > result = ArrayOf<IProfileGroup*>::Alloc(size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IProfileGroup> item = IProfileGroup::Probe((*array)[i]);
        result->Set(i, item);
    }
    *groups = result;
    REFCOUNT_ADD(*groups)

    return NOERROR;
}

ECode Profile::GetProfileGroup(
    /* [in] */ IUUID* uuid,
    /* [out] */ IProfileGroup** group)
{
    VALIDATE_NOT_NULL(group)
    AutoPtr<IInterface> result;
    mProfileGroups->Get(uuid, (IInterface**)&result);
    *group = IProfileGroup::Probe(result);
    REFCOUNT_ADD(*group)
    return NOERROR;
}

ECode Profile::GetDefaultGroup(
    /* [out] */ IProfileGroup** group)
{
    VALIDATE_NOT_NULL(group)
    *group = mDefaultGroup;
    REFCOUNT_ADD(*group);
    return NOERROR;
}

ECode Profile::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteString(mName);
    dest->WriteInt32(mNameResId);
    AutoPtr<IParcelUuid> uuid;
    CParcelUuid::New(mUuid, (IParcelUuid**)&uuid);
    dest->WriteInterfacePtr(uuid);

    Int32 size;
    mSecondaryUuids->GetSize(&size);
    AutoPtr<ArrayOf<IParcelUuid*> > uuids = ArrayOf<IParcelUuid*>::Alloc(size);
    for (Int32 i = 0; i < size; i++) {
        uuid = NULL;
        AutoPtr<IInterface> item;
        mSecondaryUuids->Get(i, (IInterface**)&item);
        CParcelUuid::New(IUUID::Probe(item), (IParcelUuid**)&uuid);
        uuids->Set(i, uuid);
    }
    dest->WriteArrayOf((Handle32)uuids.Get());
    dest->WriteInt32(mStatusBarIndicator ? 1 : 0);
    dest->WriteInt32(mProfileType);
    dest->WriteInt32(mDirty ? 1 : 0);

    AutoPtr<ICollection> collection;
    AutoPtr<ArrayOf<IInterface*> > groups;
    mProfileGroups->GetValues((ICollection**)&collection);
    collection->ToArray((ArrayOf<IInterface*>**)&groups);
    dest->WriteArrayOf((Handle32)groups.Get());

    collection = NULL;
    AutoPtr<ArrayOf<IInterface*> > streams;
    mStreams->GetValues((ICollection**)&collection);
    collection->ToArray((ArrayOf<IInterface*>**)&streams);
    dest->WriteArrayOf((Handle32)streams.Get());

    collection = NULL;
    AutoPtr<ArrayOf<IInterface*> > connections;
    mStreams->GetValues((ICollection**)&collection);
    collection->ToArray((ArrayOf<IInterface*>**)&connections);
    dest->WriteArrayOf((Handle32)connections.Get());

    dest->WriteInterfacePtr(mRingMode);
    dest->WriteInterfacePtr(mAirplaneMode);
    dest->WriteInterfacePtr(mBrightness);
    dest->WriteInt32(mScreenLockMode);

    mTriggers->GetSize(&size);
    dest->WriteInt32(size);
    AutoPtr<ISet> entrySet;
    mTriggers->GetEntrySet((ISet**)&entrySet);
    AutoPtr<IIterator> it;
    entrySet->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> item;
        it->GetNext((IInterface**)&item);
        IMapEntry* entry = IMapEntry::Probe(item);
        AutoPtr<IInterface> ko, vo;
        entry->GetKey((IInterface**)&ko);
        entry->GetValue((IInterface**)&vo);
        dest->WriteString(Object::ToString(ko));
        dest->WriteInterfacePtr(vo);
    }

    dest->WriteInt32(mExpandedDesktopMode);
    return NOERROR;
}

ECode Profile::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadString(&mName);
    source->ReadInt32(&mNameResId);

    AutoPtr<IInterface> obj;
    source->ReadInterfacePtr((Handle32*)&obj);
    IParcelUuid* uuid = IParcelUuid::Probe(obj);
    uuid->GetUuid((IUUID**)&mUuid);

    AutoPtr<ArrayOf<IInterface*> > array;
    source->ReadArrayOf((Handle32*)&array);
    for (Int32 i = 0; i < array->GetLength(); i++) {
        AutoPtr<IUUID> item;
        IParcelUuid::Probe((*array)[i])->GetUuid((IUUID**)&item);
        mSecondaryUuids->Add(item.Get());
    }

    Int32 value;
    source->ReadInt32(&value);
    mStatusBarIndicator = (value == 1);
    source->ReadInt32(&mProfileType);
    source->ReadInt32(&value);
    mDirty = (value == 1);

    array = NULL;
    source->ReadArrayOf((Handle32*)&array);
    Boolean isDefault;
    for (Int32 i = 0; i < array->GetLength(); i++) {
        IProfileGroup* group = IProfileGroup::Probe((*array)[i]);
        AutoPtr<IUUID> id;
        group->GetUuid((IUUID**)&id);
        mProfileGroups->Put(id, group);
        group->IsDefaultGroup(&isDefault);
        if (isDefault) {
            mDefaultGroup = group;
        }
    }

    array = NULL;
    source->ReadArrayOf((Handle32*)&array);
    Int32 id;
    for (Int32 i = 0; i < array->GetLength(); i++) {
        IStreamSettings* stream = IStreamSettings::Probe((*array)[i]);
        stream->GetStreamId(&id);
        mStreams->Put(CoreUtils::Convert(id).Get(), stream);
    }

    array = NULL;
    source->ReadArrayOf((Handle32*)&array);
    for (Int32 i = 0; i < array->GetLength(); i++) {
        IConnectionSettings* connection = IConnectionSettings::Probe((*array)[i]);
        connection->GetConnectionId(&id);
        mConnections->Put(CoreUtils::Convert(id), connection);
    }

    obj = NULL;
    source->ReadInterfacePtr((Handle32*)&obj);
    mRingMode = IRingModeSettings::Probe(obj);
    obj = NULL;
    source->ReadInterfacePtr((Handle32*)&obj);
    mAirplaneMode = IAirplaneModeSettings::Probe(obj);
    obj = NULL;
    source->ReadInterfacePtr((Handle32*)&obj);
    mBrightness = IBrightnessSettings::Probe(obj);
    source->ReadInt32(&mScreenLockMode);

    Int32 size;
    source->ReadInt32(&size);
    String key;
    for (Int32 i = 0; i < size; i++) {
        source->ReadString(&key);
        obj = NULL;
        source->ReadInterfacePtr((Handle32*)&obj);
        mTriggers->Put(CoreUtils::Convert(key), obj);
    }

    source->ReadInt32(&mExpandedDesktopMode);
    return NOERROR;
}

ECode Profile::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = mName;
    return NOERROR;
}

ECode Profile::SetName(
    /* [in] */ const String& name)
{
    mName = name;
    mNameResId = -1;
    mDirty = TRUE;
    return NOERROR;
}

ECode Profile::GetProfileType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type)
    *type = mProfileType;
    return NOERROR;
}

ECode Profile::SetProfileType(
    /* [in] */ Int32 type)
{
    mProfileType = type;
    mDirty = TRUE;
    return NOERROR;
}

ECode Profile::GetUuid(
    /* [out] */ IUUID** uuid)
{
    VALIDATE_NOT_NULL(uuid)
    if (mUuid == NULL) {
        AutoPtr<IUUIDHelper> helper;
        CUUIDHelper::AcquireSingleton((IUUIDHelper**)&helper);
        helper->RandomUUID((IUUID**)&mUuid);
    }

    *uuid = mUuid;
    REFCOUNT_ADD(*uuid);
    return NOERROR;
}

ECode Profile::GetSecondaryUuids(
    /* [out, callee] */ ArrayOf<IUUID*>** uuids)
{
    VALIDATE_NOT_NULL(uuids)
    AutoPtr<ArrayOf<IInterface*> > array;
    mSecondaryUuids->ToArray((ArrayOf<IInterface*>**)&array);
    Int32 length = array->GetLength();
    AutoPtr<ArrayOf<IUUID*> > result = ArrayOf<IUUID*>::Alloc(length);
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<IUUID> item = IUUID::Probe((*array)[i]);
        result->Set(i, item);
    }
    *uuids = result;
    REFCOUNT_ADD(*uuids)
    return NOERROR;
}

ECode Profile::SetSecondaryUuids(
    /* [in] */ IList* uuids)
{
     mSecondaryUuids->Clear();
    if (uuids != NULL) {
        mSecondaryUuids->AddAll(ICollection::Probe(uuids));
        mDirty = TRUE;
    }
    return NOERROR;
}

ECode Profile::AddSecondaryUuid(
    /* [in] */ IUUID* uuid)
{
    if (uuid != NULL) {
        mSecondaryUuids->Add(uuid);
        mDirty = TRUE;
    }
    return NOERROR;
}

ECode Profile::GetStatusBarIndicator(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mStatusBarIndicator;
    return NOERROR;
}

ECode Profile::SetStatusBarIndicator(
    /* [in] */ Boolean newStatusBarIndicator)
{
    mStatusBarIndicator = newStatusBarIndicator;
    mDirty = TRUE;
    return NOERROR;
}

ECode Profile::IsConditionalType(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mProfileType == CONDITIONAL_TYPE ? TRUE : FALSE;
    return NOERROR;
}

ECode Profile::SetConditionalType()
{
    mProfileType = CONDITIONAL_TYPE;
    mDirty = TRUE;
    return NOERROR;
}

ECode Profile::GetRingMode(
    /* [out] */ IRingModeSettings** descriptor)
{
    VALIDATE_NOT_NULL(descriptor)
    *descriptor = mRingMode;
    REFCOUNT_ADD(*descriptor);
    return NOERROR;
}

ECode Profile::SetRingMode(
    /* [in] */ IRingModeSettings* descriptor)
{
    mRingMode = descriptor;
    mDirty = TRUE;
    return NOERROR;
}

ECode Profile::GetScreenLockMode(
    /* [out] */ Int32* screenLockMode)
{
    VALIDATE_NOT_NULL(screenLockMode)
    *screenLockMode = mScreenLockMode;
    return NOERROR;
}

ECode Profile::SetScreenLockMode(
    /* [in] */ Int32 screenLockMode)
{
    if (screenLockMode < IProfileLockMode::DEFAULT
            || screenLockMode > IProfileLockMode::DISABLE) {
        mScreenLockMode = IProfileLockMode::DEFAULT;
    }
    else {
        mScreenLockMode = screenLockMode;
    }
    mDirty = TRUE;
    return NOERROR;
}

ECode Profile::GetExpandedDesktopMode(
    /* [out] */ Int32* expandedDesktopMode)
{
    VALIDATE_NOT_NULL(expandedDesktopMode)
    *expandedDesktopMode = mExpandedDesktopMode;
    return NOERROR;
}

ECode Profile::SetExpandedDesktopMode(
    /* [in] */ Int32 expandedDesktopMode)
{
    if (expandedDesktopMode < IProfileExpandedDesktopMode::DEFAULT
            || expandedDesktopMode > IProfileExpandedDesktopMode::DISABLE) {
        mExpandedDesktopMode = IProfileExpandedDesktopMode::DEFAULT;
    }
    else {
        mExpandedDesktopMode = expandedDesktopMode;
    }
    mDirty = TRUE;
    return NOERROR;
}

ECode Profile::GetAirplaneMode(
    /* [out] */ IAirplaneModeSettings** descriptor)
{
    VALIDATE_NOT_NULL(descriptor)
    *descriptor = mAirplaneMode;
    REFCOUNT_ADD(*descriptor);
    return NOERROR;
}

ECode Profile::SetAirplaneMode(
    /* [in] */ IAirplaneModeSettings* descriptor)
{
    mAirplaneMode = descriptor;
    mDirty = TRUE;
    return NOERROR;
}

ECode Profile::GetBrightness(
    /* [out] */ IBrightnessSettings** descriptor)
{
    VALIDATE_NOT_NULL(descriptor)
    *descriptor = mBrightness;
    REFCOUNT_ADD(*descriptor);
    return NOERROR;
}

ECode Profile::SetBrightness(
    /* [in] */ IBrightnessSettings* descriptor)
{
    mBrightness = descriptor;
    mDirty = TRUE;
    return NOERROR;
}

ECode Profile::IsDirty(
    /* [out] */ Boolean* result)
{
    if (mDirty) {
        *result = TRUE;
        return NOERROR;
    }
    AutoPtr<ICollection> values;
    AutoPtr<IIterator> it;
    mProfileGroups->GetValues((ICollection**)&values);
    values->GetIterator((IIterator**)&it);
    Boolean hasNext, isDirty;
    AutoPtr<IInterface> value;
    while(it->HasNext(&hasNext), hasNext) {
        it->GetNext((IInterface**)&value);
        IProfileGroup::Probe(value)->IsDirty(&isDirty);
        if (isDirty) {
           *result = TRUE;
            return NOERROR;
        }
        value = NULL;
    }

    values = NULL;
    it = NULL;
    mStreams->GetValues((ICollection**)&values);
    values->GetIterator((IIterator**)&it);
    while(it->HasNext(&hasNext), hasNext) {
        it->GetNext((IInterface**)&value);
        IStreamSettings::Probe(value)->IsDirty(&isDirty);
        if (isDirty) {
           *result = TRUE;
            return NOERROR;
        }
        value = NULL;
    }

    values = NULL;
    it = NULL;
    mConnections->GetValues((ICollection**)&values);
    values->GetIterator((IIterator**)&it);
    while(it->HasNext(&hasNext), hasNext) {
        it->GetNext((IInterface**)&value);
        IConnectionSettings::Probe(value)->IsDirty(&isDirty);
        if (isDirty) {
           *result = TRUE;
            return NOERROR;
        }
        value = NULL;
    }

    if (mRingMode->IsDirty(&isDirty), isDirty) {
        *result = TRUE;
        return NOERROR;
    }
    if (mAirplaneMode->IsDirty(&isDirty), isDirty) {
        *result = TRUE;
        return NOERROR;
    }
    if (mBrightness->IsDirty(&isDirty), isDirty) {
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode Profile::GetXmlString(
    /* [in] */ IStringBuilder* builder,
    /* [in] */ IContext* context)
{
    builder->Append(String("<profile "));
    if (mNameResId > 0) {
        builder->Append(String("nameres=\""));
        AutoPtr<IResources> res;
        String entryName;
        context->GetResources((IResources**)&res);
        res->GetResourceEntryName(mNameResId, &entryName);
        builder->Append(entryName);
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
    String str;
    uuid->ToString(&str);
    builder->Append(TextUtils::HtmlEncode(str));
    builder->Append(String("\">\n"));

    builder->Append(String("<uuids>"));
    Int32 size;
    mSecondaryUuids->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        builder->Append(String("<uuid>"));
        AutoPtr<IInterface> item;
        mSecondaryUuids->Get(i, (IInterface**)&item);
        IUUID::Probe(item)->ToString(&str);
        builder->Append(TextUtils::HtmlEncode(str));
        builder->Append(String("</uuid>"));
    }
    builder->Append(String("</uuids>\n"));

    builder->Append(String("<profiletype>"));
    Int32 type;
    GetProfileType(&type);
    builder->Append(type == TOGGLE_TYPE ? String("toggle") : String("conditional"));
    builder->Append(String("</profiletype>\n"));

    builder->Append(String("<statusbar>"));
    Boolean indicator;
    GetStatusBarIndicator(&indicator);
    builder->Append(indicator ? String("yes") : String("no"));
    builder->Append(String("</statusbar>\n"));

    builder->Append(String("<screen-lock-mode>"));
    builder->Append(mScreenLockMode);
    builder->Append(String("</screen-lock-mode>\n"));

    builder->Append(String("<expanded-desktop-mode>"));
    builder->Append(mExpandedDesktopMode);
    builder->Append(String("</expanded-desktop-mode>\n"));

    mAirplaneMode->GetXmlString(builder, context);

    mBrightness->GetXmlString(builder, context);

    mRingMode->GetXmlString(builder, context);

    AutoPtr<ICollection> values;
    AutoPtr<IIterator> it;
    AutoPtr<IInterface> value;
    Boolean hasNext;
    mProfileGroups->GetValues((ICollection**)&values);
    values->GetIterator((IIterator**)&it);
    while(it->HasNext(&hasNext), hasNext) {
        it->GetNext((IInterface**)&value);
        IProfileGroup::Probe(value)->GetXmlString(builder, context);
        value = NULL;
    }

    values = NULL;
    it = NULL;
    mStreams->GetValues((ICollection**)&values);
    values->GetIterator((IIterator**)&it);
    while(it->HasNext(&hasNext), hasNext) {
        it->GetNext((IInterface**)&value);
        IStreamSettings::Probe(value)->GetXmlString(builder, context);
        value = NULL;
    }

    values = NULL;
    it = NULL;
    mConnections->GetValues((ICollection**)&values);
    values->GetIterator((IIterator**)&it);
    while(it->HasNext(&hasNext), hasNext) {
        it->GetNext((IInterface**)&value);
        IConnectionSettings::Probe(value)->GetXmlString(builder, context);
        value = NULL;
    }

    Boolean empty;
    if (mTriggers->IsEmpty(&empty), !empty) {
        builder->Append(String("<triggers>\n"));
        values = NULL;
        it = NULL;
        mTriggers->GetValues((ICollection**)&values);
        values->GetIterator((IIterator**)&it);
        while(it->HasNext(&hasNext), hasNext) {
            it->GetNext((IInterface**)&value);
            IProfileTrigger::Probe(value)->GetXmlString(builder, context);
            value = NULL;
        }
        builder->Append(String("</triggers>\n"));
    }

    builder->Append(String("</profile>\n"));
    mDirty = FALSE;
    return NOERROR;
}

AutoPtr<IList> Profile::ReadSecondaryUuidsFromXml(
    /* [in] */ IXmlPullParser* xpp,
    /* [in] */ IContext* context)
{
    AutoPtr<IArrayList> uuids;
    CArrayList::New((IArrayList**)&uuids);
    Int32 event;
    String name;
    xpp->Next(&event);
    xpp->GetName(&name);

    AutoPtr<IUUIDHelper> helper;
    CUUIDHelper::AcquireSingleton((IUUIDHelper**)&helper);
    while (event != IXmlPullParser::END_TAG || !name.Equals("uuids")) {
        if (event == IXmlPullParser::START_TAG) {
            if (name.Equals("uuid")) {
                // try {
                AutoPtr<IUUID> uuid;
                String text;
                xpp->NextText(&text);
                ECode ec = helper->FromString(text, (IUUID**)&uuid);
                if (ec == (ECode) E_NULL_POINTER_EXCEPTION) {
                    Logger::W(TAG, "Null Pointer - invalid UUID");
                    continue;
                }
                else if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
                    Logger::W(TAG, "UUID not recognized");
                    continue;
                }
                uuids->Add(uuid);
                // } catch (NullPointerException e) {
                //     Log.w(TAG, "Null Pointer - invalid UUID");
                // } catch (IllegalArgumentException e) {
                //     Log.w(TAG, "UUID not recognized");
                // }
            }
        }
        xpp->Next(&event);
    }

    return IList::Probe(uuids);
}

ECode Profile::ReadTriggersFromXml(
    /* [in] */ IXmlPullParser* xpp,
    /* [in] */ IContext* context,
    /* [in] */ IProfile* profile)
{
    Int32 event;
    String name;
    FAIL_RETURN(xpp->Next(&event));
    xpp->GetName(&name);
    while (event != IXmlPullParser::END_TAG || !name.Equals("triggers")) {
        if (event == IXmlPullParser::START_TAG) {
            AutoPtr<IProfileTrigger> trigger = ProfileTrigger::FromXml(xpp, context);
            if (trigger != NULL) {
                ((Profile*)profile)->mTriggers->Put(
                        CoreUtils::Convert(((ProfileTrigger*)trigger.Get())->mId).Get(), trigger);
            }
        }
        FAIL_RETURN(xpp->Next(&event));
    }
    return NOERROR;
}

ECode Profile::ValidateRingtones(
    /* [in] */ IContext* context)
{
    AutoPtr<ICollection> values;
    AutoPtr<IIterator> it;
    mProfileGroups->GetValues((ICollection**)&values);
    values->GetIterator((IIterator**)&it);
    Boolean hasNext;
    AutoPtr<IInterface> value;
    while(it->HasNext(&hasNext), hasNext) {
        it->GetNext((IInterface**)&value);
        ((CProfileGroup*)IProfileGroup::Probe(value))->ValidateOverrideUris(context);
        value = NULL;
    }
    return NOERROR;
}

AutoPtr<IProfile> Profile::FromXml(
    /* [in] */ IXmlPullParser* xpp,
    /* [in] */ IContext* context)
{
    String value;
    xpp->GetAttributeValue(String(NULL), String("nameres"), &value);
    Int32 profileNameResId = -1;
    String profileName;

    if (!value.IsNull()) {
        AutoPtr<IResources> res;
        context->GetResources((IResources**)&res);
        res->GetIdentifier(value, String("string"), String("android"), &profileNameResId);
        if (profileNameResId > 0) {
            res->GetString(profileNameResId, &profileName);
        }
    }

    if (profileName.IsNull()) {
        xpp->GetAttributeValue(String(NULL), String("name"), &profileName);
    }

    AutoPtr<IUUIDHelper> helper;
    CUUIDHelper::AcquireSingleton((IUUIDHelper**)&helper);
    AutoPtr<IUUID> profileUuid;
    helper->RandomUUID((IUUID**)&profileUuid);
    // try {
    xpp->GetAttributeValue(String(NULL), String("uuid"), &value);
    ECode ec = helper->FromString(value, (IUUID**)&profileUuid);
    if (ec == (ECode)E_NULL_POINTER_EXCEPTION) {
        String uuidStr;
        profileUuid->ToString(&uuidStr);
        Logger::W(TAG, "Null Pointer - UUID not found for %s.  New UUID generated: %s",
                profileName.string(), uuidStr.string());
    }
    else if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
        String uuidStr;
        profileUuid->ToString(&uuidStr);
        Logger::W(TAG, "UUID not recognized for %s.  New UUID generated: %s",
                profileName.string(), uuidStr.string());
    }
    // } catch (NullPointerException e) {
    //     Log.w(TAG,
    //             "Null Pointer - UUID not found for "
    //             + profileName
    //             + ".  New UUID generated: "
    //             + profileUuid.toString()
    //             );
    // } catch (IllegalArgumentException e) {
    //     Log.w(TAG,
    //             "UUID not recognized for "
    //             + profileName
    //             + ".  New UUID generated: "
    //             + profileUuid.toString()
    //             );
    // }

    AutoPtr<IProfile> profile;
    CProfile::New(profileName, profileNameResId, profileUuid, (IProfile**)&profile);
    Int32 event;
    xpp->Next(&event);
    while (event != IXmlPullParser::END_TAG) {
        if (event == IXmlPullParser::START_TAG) {
            String name;
            xpp->GetName(&name);
            String text;
            xpp->NextText(&text);
            if (name.Equals("uuids")) {
                profile->SetSecondaryUuids(ReadSecondaryUuidsFromXml(xpp, context));
            }
            if (name.Equals("statusbar")) {
                profile->SetStatusBarIndicator(text.Equals("yes"));
            }
            if (name.Equals("profiletype")) {
                profile->SetProfileType(text.Equals("toggle") ? TOGGLE_TYPE : CONDITIONAL_TYPE);
            }
            if (name.Equals("ringModeDescriptor")) {
                AutoPtr<IRingModeSettings> smd;
                CRingModeSettings::FromXml(xpp, context, (IRingModeSettings**)&smd);
                profile->SetRingMode(smd);
            }
            if (name.Equals("airplaneModeDescriptor")) {
                AutoPtr<IAirplaneModeSettings> amd;
                CAirplaneModeSettings::FromXml(xpp, context, (IAirplaneModeSettings**)&amd);
                profile->SetAirplaneMode(amd);
            }
            if (name.Equals("brightnessDescriptor")) {
                AutoPtr<IBrightnessSettings> bd;
                CBrightnessSettings::FromXml(xpp, context, (IBrightnessSettings**)&bd);
                profile->SetBrightness(bd);
            }
            if (name.Equals("screen-lock-mode")) {
                profile->SetScreenLockMode(StringUtils::ParseInt32(text));
            }
            if (name.Equals("expanded-desktop-mode")) {
                profile->SetExpandedDesktopMode(StringUtils::ParseInt32(text));
            }
            if (name.Equals("profileGroup")) {
                AutoPtr<IProfileGroup> pg;
                CProfileGroup::FromXml(xpp, context, (IProfileGroup**)&pg);
                profile->AddProfileGroup(pg);
            }
            if (name.Equals("streamDescriptor")) {
                AutoPtr<IStreamSettings> sd;
                CStreamSettings::FromXml(xpp, context, (IStreamSettings**)&sd);
                profile->SetStreamSettings(sd);
            }
            if (name.Equals("connectionDescriptor")) {
                AutoPtr<IConnectionSettings> cs;
                CConnectionSettings::FromXml(xpp, context, (IConnectionSettings**)&cs);
                Int32 id;
                cs->GetConnectionId(&id);
                ((Profile*)profile.Get())->mConnections->Put(CoreUtils::Convert(id), cs);
            }
            if (name.Equals("triggers")) {
                ReadTriggersFromXml(xpp, context, profile);
            }
        }
        xpp->Next(&event);
    }

    /* we just loaded from XML, so nothing needs saving */
    ((Profile*)profile.Get())->mDirty = FALSE;

    return profile;
}

ECode Profile::DoSelect(
    /* [in] */ IContext* context)
{
    // Set stream volumes
    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&service);
    IAudioManager* am = IAudioManager::Probe(service) ;

    AutoPtr<ICollection> values;
    AutoPtr<IIterator> it;
    AutoPtr<IInterface> value;
    mStreams->GetValues((ICollection**)&values);
    values->GetIterator((IIterator**)&it);
    Boolean hasNext;
    Boolean isOverride;
    while(it->HasNext(&hasNext), hasNext) {
        it->GetNext((IInterface**)&value);
        IStreamSettings* sd = IStreamSettings::Probe(value);
        sd->IsOverride(&isOverride);
        if (isOverride) {
            Int32 id, value;
            sd->GetStreamId(&id);
            sd->GetValue(&value);
            am->SetStreamVolume(id, value, 0);
        }
        value = NULL;
    }

    // Set connections
    values = NULL;
    it = NULL;
    mConnections->GetValues((ICollection**)&values);
    values->GetIterator((IIterator**)&it);
    IConnectionSettings* cs;
    while(it->HasNext(&hasNext), hasNext) {
        it->GetNext((IInterface**)&value);
        cs = IConnectionSettings::Probe(value);
        cs->IsOverride(&isOverride);
        if (isOverride) {
            cs->ProcessOverride(context);
        }
        value = NULL;
    }

    // Set ring mode
    mRingMode->ProcessOverride(context);
    // Set airplane mode
    mAirplaneMode->ProcessOverride(context);

    // Set brightness
    mBrightness->ProcessOverride(context);

    // Set expanded desktop
    // if (mExpandedDesktopMode != ExpandedDesktopMode.DEFAULT) {
    //     Settings.System.putIntForUser(context.getContentResolver(),
    //             Settings.System.EXPANDED_DESKTOP_STATE,
    //             mExpandedDesktopMode == ExpandedDesktopMode.ENABLE ? 1 : 0,
    //             UserHandle.USER_CURRENT);
    // }
    return NOERROR;
}

ECode Profile::GetSettingsForStream(
    /* [in] */ Int32 streamId,
    /* [out] */ IStreamSettings** descriptor)
{
    VALIDATE_NOT_NULL(descriptor)
    AutoPtr<IInterface> sd;
    mStreams->Get(CoreUtils::Convert(streamId), (IInterface**)&sd);
    *descriptor = IStreamSettings::Probe(sd);
    REFCOUNT_ADD(*descriptor)
    return NOERROR;
}

ECode Profile::SetStreamSettings(
    /* [in] */ IStreamSettings* descriptor)
{
    Int32 id;
    descriptor->GetStreamId(&id);
    mStreams->Put(CoreUtils::Convert(id), descriptor);
    mDirty = TRUE;
    return NOERROR;
}

ECode Profile::GetStreamSettings(
    /* [out] */ ICollection** descriptors)
{
    VALIDATE_NOT_NULL(descriptors)
    return mStreams->GetValues(descriptors);
}

ECode Profile::GetSettingsForConnection(
    /* [in] */ Int32 connectionId,
    /* [out] */ IConnectionSettings** descriptor)
{
    VALIDATE_NOT_NULL(descriptor)
    AutoPtr<IInterface> cs;
    mConnections->Get(CoreUtils::Convert(connectionId), (IInterface**)&cs);
    *descriptor = IConnectionSettings::Probe(cs);
    REFCOUNT_ADD(*descriptor)
    return NOERROR;
}

ECode Profile::SetConnectionSettings(
    /* [in] */ IConnectionSettings* descriptor)
{
    Int32 id;
    descriptor->GetConnectionId(&id);
    mConnections->Put(CoreUtils::Convert(id), descriptor);
    return NOERROR;
}

ECode Profile::GetConnectionSettings(
    /* [out] */ ICollection** descriptors)
{
    VALIDATE_NOT_NULL(descriptors)
    return mConnections->GetValues(descriptors);
}

ECode Profile::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = "Profile";
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos

