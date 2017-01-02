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

#include "elastos/droid/server/AttributeCache.h"
#include <Elastos.Droid.Os.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Content::Res::CConfiguration;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Core::AutoLock;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {

AutoPtr<AttributeCache> AttributeCache::sInstance;

//=======================================================================
// Entry
//=======================================================================
Entry::Entry(
    /* [in] */ IContext* c,
    /* [in] */ ITypedArray* ta)
    : mContext(c)
    , mArray(ta)
{
}

//=======================================================================
// AttributeCache::Package
//=======================================================================
AttributeCache::Package::Package(
    /* [in] */ IContext* c)
{
    mContext = c;
}

AttributeCache::Package::~Package()
{
}

//=======================================================================
// AttributeCache
//=======================================================================
void AttributeCache::Init(
        /* [in] */ IContext* context)
{
    if (sInstance == NULL) {
        sInstance = new AttributeCache(context);
    }
}

AutoPtr<AttributeCache> AttributeCache::GetInstance()
{
    assert(sInstance && "AttributeCache is not init");
    return sInstance;
}

AttributeCache::AttributeCache(
    /* [in] */ IContext* context)
    : mContext(context)
{
    CConfiguration::New((IConfiguration**)&mConfiguration);
}

AttributeCache::~AttributeCache()
{
}

void AttributeCache::UpdateConfiguration(
    /* [in] */ IConfiguration* config)
{
    AutoLock syncLock(this);
    Int32 changes;
    mConfiguration->UpdateFrom(config, &changes);
    if ((changes & ~(IActivityInfo::CONFIG_FONT_SCALE |
            IActivityInfo::CONFIG_KEYBOARD_HIDDEN |
            IActivityInfo::CONFIG_ORIENTATION)) != 0) {
        mPackages.Clear();
    }
}

void AttributeCache::RemovePackage(
    /* [in] */ const String& packageName)
{
    AutoLock syncLock(this);
    HashMap<String, AutoPtr<Package> >::Iterator it = mPackages.Find(packageName);
    if (it != mPackages.End()) {
        mPackages.Erase(it);
    }
}

AutoPtr<Entry> AttributeCache::Get(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 resId,
    /* [in] */ ArrayOf<Int32>* styleable,
    /* [in] */ Int32 userId)
{
    AutoLock syncLock(this);

    AutoPtr<Package> pkg;
    HashMap<String, AutoPtr<Package> >::Iterator it = mPackages.Find(packageName);
    if (it != mPackages.End()) {
        pkg = it->mSecond;
    }

    AutoPtr<EntryMap> entryMap;
    if (pkg != NULL) {
        PackageMapIterator pit = pkg->mMap.Find(resId);
        if (pit != pkg->mMap.End()) {
            entryMap = pit->mSecond;
            if (entryMap != NULL) {
                AutoPtr<ArrayOf<Int32> > key = styleable;
                EntryMapIterator eit = entryMap->Find(key);
                if (eit != entryMap->End()) {
                    return eit->mSecond;
                }
            }
        }
    }
    else {
        AutoPtr<IContext> context;
        AutoPtr<IUserHandle> userHandle;
        CUserHandle::New(userId, (IUserHandle**)&userHandle);
        ECode ec = mContext->CreatePackageContextAsUser(packageName, 0, userHandle, (IContext**)&context);
        if (FAILED(ec) || context == NULL) {
            return NULL;
        }

        pkg = new Package(context);
        mPackages[packageName] = pkg;
    }

    if (entryMap == NULL) {
        entryMap = new EntryMap();
        pkg->mMap[resId] = entryMap;
    }

    AutoPtr<ITypedArray> typeArray;
    ECode ec = pkg->mContext->ObtainStyledAttributes(resId, styleable, (ITypedArray**)&typeArray);
    if (FAILED(ec)) {
        return NULL;
    }

    AutoPtr<Entry> ent = new Entry(pkg->mContext, typeArray);
    (*entryMap)[styleable] = ent;
    return ent;
}

}// Server
}// Droid
}// Elastos
