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

#include "elastos/droid/server/location/GeoFencerBase.h"
#include "Elastos.Droid.App.h"
#include "Elastos.CoreLibrary.IO.h"
#include <elastos/core/AutoLock.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Location::CGeoFenceParams;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

const String GeoFencerBase::TAG("GeoFencerBase");

GeoFencerBase::GeoFencerBase()
{
    CHashMap::New((IHashMap**)&mGeoFences);
}

void GeoFencerBase::Add(
    /* [in] */ Double latitude,
    /* [in] */ Double longitude,
    /* [in] */ Float radius,
    /* [in] */ Int64 expiration,
    /* [in] */ IPendingIntent* intent,
    /* [in] */ const String& packageName)
{
    AutoPtr<IGeoFenceParams> params;
    CGeoFenceParams::New(latitude, longitude, radius,
        expiration, intent, packageName,(IGeoFenceParams**)&params);
    Add(params);
}

void GeoFencerBase::Add(
    /* [in] */ IGeoFenceParams* geoFence)
{
    AutoPtr<IPendingIntent> intent;
    geoFence->GetIntent((IPendingIntent**)&intent);
    {    AutoLock syncLock(mGeoFences);
        mGeoFences->Put(intent, geoFence);
    }
    if (!Start(geoFence)) {
        {    AutoLock syncLock(mGeoFences);
            mGeoFences->Remove(intent);
        }
    }
}

void GeoFencerBase::Remove(
    /* [in] */ IPendingIntent* intent)
{
    Remove(intent, FALSE);
}

void GeoFencerBase::Remove(
    /* [in] */ IPendingIntent* intent,
    /* [in] */ Boolean localOnly)
{
    AutoPtr<IGeoFenceParams> geoFence;

    {    AutoLock syncLock(mGeoFences);
        AutoPtr<IInterface> value;
        mGeoFences->Remove(intent, (IInterface**)&value);
        geoFence = IGeoFenceParams::Probe(value);
    }

    if (geoFence != NULL) {
        if (!localOnly && !Stop(intent)) {
            AutoPtr<IPendingIntent> intent;
            geoFence->GetIntent((IPendingIntent**)&intent);
            {    AutoLock syncLock(mGeoFences);
                mGeoFences->Put(intent, geoFence);
            }
        }
    }
}

Int32 GeoFencerBase::GetNumbOfGeoFences()
{
    Int32 size;
    mGeoFences->GetSize(&size);
    return size;
}

AutoPtr<ICollection> GeoFencerBase::GetAllGeoFences()
{
    AutoPtr<ICollection> values;
    mGeoFences->GetValues((ICollection**)&values);
    return values;
}

AutoPtr<IGeoFenceParams> GeoFencerBase::GetGeoFence(
    /* [in] */ IPendingIntent* intent)
{
    AutoPtr<IInterface> value;
    mGeoFences->Get(intent, (IInterface**)&value);
    return IGeoFenceParams::Probe(value);
}

Boolean GeoFencerBase::HasCaller(
    /* [in] */ Int32 uid)
{
    AutoPtr<ICollection> values;
    mGeoFences->GetValues((ICollection**)&values);
    AutoPtr<ArrayOf<IInterface*> > array;
    values->ToArray((ArrayOf<IInterface*>**)&array);
    for (Int32 i = 0; i < array->GetLength(); i++) {
        Int32 itemUid;
        IGeoFenceParams::Probe((*array)[i])->GetCallerUid(&itemUid);
        if (itemUid == uid) {
            return TRUE;
        }
    }
    return FALSE;
}

void GeoFencerBase::RemoveCaller(
    /* [in] */ Int32 uid)
{
    AutoPtr<IArrayList> removedFences;
    AutoPtr<ICollection> values;
    mGeoFences->GetValues((ICollection**)&values);
    AutoPtr<ArrayOf<IInterface*> > array;
    values->ToArray((ArrayOf<IInterface*>**)&array);
    for (Int32 i = 0; i < array->GetLength(); i++) {
        AutoPtr<IGeoFenceParams> alert = IGeoFenceParams::Probe((*array)[i]);
        Int32 itemUid;
        alert->GetCallerUid(&itemUid);
        if (itemUid == uid) {
            if (removedFences == NULL) {
                CArrayList::New((IArrayList**)&removedFences);
            }
            AutoPtr<IPendingIntent> intent;
            alert->GetIntent((IPendingIntent**)&intent);
            removedFences->Add(intent);
        }
    }
    if (removedFences != NULL) {
        Int32 size;
        removedFences->GetSize(&size);
        for (Int32 i = size - 1; i >= 0; i--) {
            AutoPtr<IInterface> item;
            removedFences->Get(i, (IInterface**)&item);
            mGeoFences->Remove(item);
        }
    }
}

void GeoFencerBase::TransferService(
    /* [in] */ GeoFencerBase* geofencer)
{
    AutoPtr<IArrayList> removedFences;
    AutoPtr<ICollection> values;
    geofencer->mGeoFences->GetValues((ICollection**)&values);
    AutoPtr<ArrayOf<IInterface*> > array;
    values->ToArray((ArrayOf<IInterface*>**)&array);
    for (Int32 i = 0; i < array->GetLength(); i++) {
        AutoPtr<IGeoFenceParams> alert = IGeoFenceParams::Probe((*array)[i]);
        AutoPtr<IPendingIntent> intent;
        alert->GetIntent((IPendingIntent**)&intent);
        geofencer->Stop(intent);
        Add(alert);
    }
}

void GeoFencerBase::Dump(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& _prefix)
{
    String prefix = _prefix;
    Int32 size;
    mGeoFences->GetSize(&size);
    if (size > 0) {
        pw->Print(prefix);
        pw->Println(String("  GeoFences:"));
        prefix += "    ";
        AutoPtr<ISet> entrySet;
        mGeoFences->GetEntrySet((ISet**)&entrySet);
        AutoPtr<ArrayOf<IInterface*> > array;
        entrySet->ToArray((ArrayOf<IInterface*>**)&array);
        for (Int32 i = 0; i < array->GetLength(); i++) {
            AutoPtr<IMapEntry> entry = IMapEntry::Probe((*array)[i]);
            AutoPtr<IInterface> key, value;
            entry->GetKey((IInterface**)&key);
            entry->GetValue((IInterface**)&value);
            pw->Print(prefix);
            pw->Print(key);
            pw->Println(String(":"));
            IGeoFenceParams::Probe(value)->Dump(pw, prefix);
        }
    }
}

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos
