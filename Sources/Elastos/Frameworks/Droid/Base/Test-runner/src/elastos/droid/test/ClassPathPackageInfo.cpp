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

#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/test/ClassPathPackageInfo.h"

using Elastos::Core::ICharSequence;
using Elastos::Utility::ICollection;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::CHashSet;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace Droid {
namespace Test {

ClassPathPackageInfo::ClassPathPackageInfo(
    /* [in] */ ClassPathPackageInfoSource* source,
    /* [in] */ const String& packageName,
    /* [in] */ ISet* subpackageNames,
    /* [in] */ ISet* topLevelClasses)
    : mSource(source)
    , mPackageName(packageName)
{
    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);
    collections->UnmodifiableSet(subpackageNames, (ISet**)&mSubpackageNames);
    collections->UnmodifiableSet(topLevelClasses, (ISet**)&mTopLevelClasses);
}

AutoPtr<ISet> ClassPathPackageInfo::GetSubpackages()
{
    AutoPtr<ISet> info;
    CHashSet::New((ISet**)&info);
    AutoPtr<IIterator> it;
    mSubpackageNames->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        String name;
        ICharSequence::Probe(obj)->ToString(&name);
        AutoPtr<ClassPathPackageInfo> cppi = mSource->GetPackageInfo(name);
        Boolean result;
        info->Add((IObject*)cppi.Get(), &result);
    }
    return info;
}

AutoPtr<ISet> ClassPathPackageInfo::GetTopLevelClassesRecursive()
{
    AutoPtr<ISet> set;
    CHashSet::New((ISet**)&set);
    AddTopLevelClassesTo(set);
    return set;
}

void ClassPathPackageInfo::AddTopLevelClassesTo(
    /* [in] */ ISet* set)
{
    Boolean result;
    set->AddAll(ICollection::Probe(mTopLevelClasses), &result);
    AutoPtr<ISet> infos =  GetSubpackages();
    AutoPtr<IIterator> it;
    infos->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        ClassPathPackageInfo* info = (ClassPathPackageInfo*)IObject::Probe(obj);
        info->AddTopLevelClassesTo(set);
    }
}

} // namespace Test
} // namespace Droid
} // namespace Elastos
