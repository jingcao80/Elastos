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

#ifndef __ELASTOS_DROID_SERVER_PROCESSMAP_H__
#define __ELASTOS_DROID_SERVER_PROCESSMAP_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Server {

template <typename E>
class ProcessMap : public ElRefBase
{
    typedef HashMap<Int32, E> Int32ObjectMap;
    typedef typename Int32ObjectMap::Iterator Int32ObjectMapIterator;
    typedef HashMap<String, AutoPtr<Int32ObjectMap> > StringInt32ObjectMapMap;
    typedef typename StringInt32ObjectMapMap::Iterator StringInt32ObjectMapMapIterator;

public:
    ProcessMap();

    ~ProcessMap();

    CARAPI_(E) Get(
        /* [in] */ const String& name,
        /* [in] */ Int32 uid);

    CARAPI_(E) Put(
        /* [in] */ const String& name,
        /* [in] */ Int32 uid,
        /* [in] */ E value);

    CARAPI_(void) Remove(
        /* [in] */ const String& name,
        /* [in] */ Int32 uid);

    AutoPtr<ProcessMap::StringInt32ObjectMapMap> GetMap();

public:
    AutoPtr<StringInt32ObjectMapMap > mMap;
};

template <typename E>
ProcessMap<E>::ProcessMap()
{
    mMap = new StringInt32ObjectMapMap(11);
}

template <typename E>
ProcessMap<E>::~ProcessMap()
{
    mMap = NULL;
}

template <typename E>
E ProcessMap<E>::Get(
    /* [in] */ const String& name,
    /* [in] */ Int32 uid)
{
    AutoPtr<Int32ObjectMap> uids;
    StringInt32ObjectMapMapIterator it = mMap->Find(name);
    if (it != mMap->End()) {
        uids = it->mSecond;
    }
    if (uids == NULL) return E(NULL);
    Int32ObjectMapIterator it2 = uids->Find(uid);
    if (it2 != uids->End()) return it2->mSecond;
    else return E(NULL);
}

template <typename E>
E ProcessMap<E>::Put(
    /* [in] */ const String& name,
    /* [in] */ Int32 uid,
    /* [in] */ E value)
{
    AutoPtr<Int32ObjectMap> uids;
    StringInt32ObjectMapMapIterator it = mMap->Find(name);
    if (it != mMap->End()) {
        uids = it->mSecond;
    }
    if (uids == NULL) {
        uids = new HashMap<Int32, E>(11);
        (*mMap)[name] = uids;
    }
    (*uids)[uid] = value;
    return value;
}

template <typename E>
void ProcessMap<E>::Remove(
    /* [in] */ const String& name,
    /* [in] */ Int32 uid)
{
    AutoPtr<Int32ObjectMap> uids;
    StringInt32ObjectMapMapIterator it = mMap->Find(name);
    if (it != mMap->End()) {
        uids = it->mSecond;
    }
    if (uids != NULL) {
        uids->Erase(uid);
        if (uids->IsEmpty()) {
            mMap->Erase(it);
        }
    }
}

template <typename E>
AutoPtr<HashMap<String, AutoPtr<HashMap<Int32, E> > > > ProcessMap<E>::GetMap()
{
    return mMap;
}

} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_PROCESSMAP_H__
