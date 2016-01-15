
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
