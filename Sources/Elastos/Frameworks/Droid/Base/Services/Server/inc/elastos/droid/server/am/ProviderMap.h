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

#ifndef __ELASTOS_DROID_SERVER_AM_PROVIDERMAP_H__
#define __ELASTOS_DROID_SERVER_AM_PROVIDERMAP_H__

#define HASH_FOR_CONTENT
#include "elastos/droid/server/am/ContentProviderRecord.h"
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::IO::IFileDescriptor;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class CActivityManagerService;

class ProviderMap : public Object
{
public:

    typedef HashMap<String, AutoPtr<ContentProviderRecord> > NameRecordHashMap;
    typedef HashMap<AutoPtr<IComponentName>, AutoPtr<ContentProviderRecord> > ClassRecordHashMap;
    typedef NameRecordHashMap::ValueType NameRecordValueType;
    typedef NameRecordHashMap::Iterator NameRecordIterator;
    typedef ClassRecordHashMap::ValueType ClassRecordValueType;
    typedef ClassRecordHashMap::Iterator ClassRecordIterator;

    ProviderMap(
        /* [in] */ CActivityManagerService* am);

    ~ProviderMap();

    CARAPI_(AutoPtr<ContentProviderRecord>) GetProviderByName(
        /* [in] */ const String& name);

    CARAPI_(AutoPtr<ContentProviderRecord>) GetProviderByName(
        /* [in] */ const String& name,
        /* [in] */ Int32 userId);

    CARAPI_(AutoPtr<ContentProviderRecord>) GetProviderByClass(
        /* [in] */ IComponentName* name);

    CARAPI_(AutoPtr<ContentProviderRecord>) GetProviderByClass(
        /* [in] */ IComponentName* name,
        /* [in] */ Int32 userId);

    CARAPI PutProviderByName(
        /* [in] */ const String& name,
        /* [in] */ ContentProviderRecord* provider);

    CARAPI PutProviderByClass(
        /* [in] */ IComponentName* name,
        /* [in] */ ContentProviderRecord* provider);

    CARAPI RemoveProviderByName(
        /* [in] */ const String& name,
        /* [in] */ Int32 userId);

    CARAPI RemoveProviderByClass(
        /* [in] */ IComponentName* name,
        /* [in] */ Int32 userId);

    CARAPI_(AutoPtr<ProviderMap::NameRecordHashMap>) GetProvidersByName(
        /* [in] */ Int32 userId);

    CARAPI_(AutoPtr<ProviderMap::ClassRecordHashMap>) GetProvidersByClass(
        /* [in] */ Int32 userId);

    CARAPI_(Boolean) CollectForceStopProvidersLocked(
        /* [in] */ const String& name,
        /* [in] */ Int32 appId,
        /* [in] */ Boolean doit,
        /* [in] */ Boolean evenPersistent,
        /* [in] */ Int32 userId,
        /* [in] */ ProviderMap::ClassRecordHashMap* providers,
        /* [in] */ List<AutoPtr<ContentProviderRecord> >* result);

    CARAPI_(Boolean) CollectForceStopProviders(
        /* [in] */ const String& name,
        /* [in] */ Int32 appId,
        /* [in] */ Boolean doit,
        /* [in] */ Boolean evenPersistent,
        /* [in] */ Int32 userId,
        /* [in] */ List<AutoPtr<ContentProviderRecord> >* result);

    CARAPI_(Boolean) DumpProvidersLocked(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ Boolean dumpAll,
        /* [in] */ const String& dumpPackage);

    CARAPI_(Boolean) DumpProvider(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& name,
        /* [in] */ ArrayOf<String>* args,
        /* [in] */ Int32 opti,
        /* [in] */ Boolean dumpAll);

private:
    CARAPI_(Boolean) DumpProvidersByClassLocked(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ Boolean dumpAll,
        /* [in] */ const String& dumpPackage,
        /* [in] */ const String& header,
        /* [in] */ Boolean needSep,
        /* [in] */ ClassRecordHashMap* map);

    CARAPI_(Boolean) DumpProvidersByNameLocked(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& dumpPackage,
        /* [in] */ const String& header,
        /* [in] */ Boolean needSep,
        /* [in] */ NameRecordHashMap* map);

    /**
     * Invokes IApplicationThread.dumpProvider() on the thread of the specified provider if
     * there is a thread associated with the provider.
     */
    CARAPI_(void) DumpProvider(
        /* [in] */ const String& prefix,
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ContentProviderRecord* r,
        /* [in] */ ArrayOf<String>* args,
        /* [in] */ Boolean dumpAll);

private:
    static const String TAG;
    static const Boolean DBG;

    CActivityManagerService* mAm;

    NameRecordHashMap mSingletonByName;
    ClassRecordHashMap mSingletonByClass;

    HashMap<Int32, AutoPtr<NameRecordHashMap> > mProvidersByNamePerUser;
    HashMap<Int32, AutoPtr<ClassRecordHashMap> > mProvidersByClassPerUser;

    typedef HashMap<Int32, AutoPtr<NameRecordHashMap> >::ValueType ProviderByNameValueType;
    typedef HashMap<Int32, AutoPtr<ClassRecordHashMap> >::ValueType ProviderByClassValueType;

};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_AM_PROVIDERMAP_H__
