
#include "elastos/droid/launcher2/PagedViewIconCache.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(PagedViewIconCache::Key, Object, IKey);

PagedViewIconCache::Key::Key(
    /* [in] */ IApplicationInfo* info)
{
    info->GetComponentName((IComponentName**)&mComponentName);
    mType = Type::ApplicationInfoKey;
}

PagedViewIconCache::Key::Key(
    /* [in] */ IResolveInfo* info)
{
    AutoPtr<IActivityInfo> activityInfo;
    info->GetActivityInfo((IActivityInfo**)&activityInfo);
    AutoPtr<IComponentInfo> ci;
    if (activityInfo != NULL) {
        ci = activityInfo;
    }
    else {
        AutoPtr<IServiceInfo> serviceInfo;
        GetServiceInfo((IServiceInfo**)&serviceInfo);
        ci = serviceInfo;
    }

    String pname;
    IPackageItemInfo::Probe(ci)->GetPackageName(&pname);
    String name;
    IPackageItemInfo::Probe(ci)->GetName(&name);
    CComponentName::New(pname, name, (IComponentName**)&mComponentName);
    mType = Type::ResolveInfoKey;
}

PagedViewIconCache::Key::Key(
    /* [in] */ IAppWidgetProviderInfo* info)
{
    info->GetProvider((IComponentName**)&mComponentName);
    mType = Type::AppWidgetProviderInfoKey;
}

AutoPtr<IComponentName> PagedViewIconCache::Key::GetComponentName()
{
    return mComponentName;
}

Boolean PagedViewIconCache::Key::IsKeyType(
    /* [in] */ Type t)
{
    return (mType == t);
}

ECode PagedViewIconCache::Key::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* isEqual)
{
    VALIDATE_NOT_NULL(isEqual);

    if (IKey::Probe(o) != NULL) {
        AutoPtr<IKey> k = IKey::Probe(o);
        PagedViewIconCache::Key* key = (PagedViewIconCache::Key*)k;
        return mComponentName->Equals(key->mComponentName, isEqual);
    }
    return Object::Equals(o);
}

ECode PagedViewIconCache::Key::GetHashCode(
    /* [out] */ Int32* code)
{
    VALIDATE_NOT_NULL(code);

    AutoPtr<IComponentName> name = GetComponentName();
    return name->GetHashCode(code);
}

PagedViewIconCache::PagedViewIconCache()
{
    CHashMap::New((IHashMap**)&mIconOutlineCache);
}

ECode PagedViewIconCache::Clear()
{
    AutoPtr<ISet> keySet;
    mIconOutlineCache->GetKeySet((ISet**)&keySet);
    AutoPtr<ArrayOf<IInterface*> > array;
    keySet->ToArray((ArrayOf<IInterface*>**)&array);

    for (Int32 i = 0; i < array->GetLength(); i++) {
        AutoPtr<IInterface> obj = (*array)[i];
        AutoPtr<IKey> key = IKey::Probe(obj);
        AutoPtr<IInterface> value;
        mIconOutlineCache->Get(TO_IINTERFACE(key), (IInterface**)&value);
        AutoPtr<IBitmap> map = IBitmap::Probe(value);
        map->Recycle();
    }
    return mIconOutlineCache->Clear();
}

ECode PagedViewIconCache::RetainAll(
    /* [in] */ IHashSet* keysToKeep,
    /* [in] */ Key::Type t)
{
    AutoPtr<ISet> keySet;
    mIconOutlineCache->GetKeySet((ISet**)&keySet);
    AutoPtr<IHashSet> keysToRemove;
    CHashSet::New(keySet, (IHashSet**)&keysToRemove);
    keysToRemove->RemoveAll(keysToKeep);

    AutoPtr<ArrayOf<IInterface*> > array;
    keysToRemove->ToArray((ArrayOf<IInterface*>**)&array);
    for (Int32 i = 0; i < array->GetLength(); i++) {
        AutoPtr<IInterface> obj = (*array)[i];
        AutoPtr<IKey> key = IKey::Probe(obj);

        Boolean res;
        key->IsKeyType(t, &res);
        if (res) {
            AutoPtr<IInterface> value;
            mIconOutlineCache->Get(TO_IINTERFACE(key), (IInterface**)&value);
            AutoPtr<IBitmap> map = IBitmap::Probe(value);
            map->Recycle();
            mIconOutlineCache->Remove(key);
        }
    }

    return NOERROR;
}

ECode PagedViewIconCache::RetainAllApps(
    /* [in] */ IArrayList* keys)
{
    AutoPtr<HashSet> keysSet;
    CHashSet::New((IHashSet**)&keysSet);

    Int32 size;
    keys->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        keys->Get(i, (IInterface**)&obj);
        AutoPtr<IApplicationInfo> info = IApplicationInfo::Probe(obj);
        AutoPtr<IKey> key = new PagedViewIconCache::Key(info);
        keysSet->Add(TO_IINTERFACE(key));

    }
    return RetainAll(keysSet, Key::Type::ApplicationInfoKey);
}

ECode PagedViewIconCache::RetainAllShortcuts(
    /* [in] */ IList* keys)
{
    AutoPtr<HashSet> keysSet;
    CHashSet::New((IHashSet**)&keysSet);

    Int32 size;
    keys->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        keys->Get(i, (IInterface**)&obj);
        AutoPtr<IResolveInfo> info = IResolveInfo::Probe(obj);
        AutoPtr<IKey> key = new PagedViewIconCache::Key(info);
        keysSet->Add(TO_IINTERFACE(key));

    }
    return RetainAll(keysSet, Key::Type::ResolveInfoKey);
}

ECode PagedViewIconCache::RetainAllAppWidgets(
    /* [in] */ IList* keys)
{
    AutoPtr<HashSet> keysSet;
    CHashSet::New((IHashSet**)&keysSet);

    Int32 size;
    keys->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        keys->Get(i, (IInterface**)&obj);
        AutoPtr<IAppWidgetProviderInfo> info = IAppWidgetProviderInfo::Probe(obj);
        AutoPtr<IKey> key = new PagedViewIconCache::Key(info);
        keysSet->Add(TO_IINTERFACE(key));

    }
    return RetainAll(keysSet, Key::Type::AppWidgetProviderInfoKey);
}

ECode PagedViewIconCache::AddOutline(
    /* [in] */ IKey* key,
    /* [in] */ IBitmap* b)
{
    mIconOutlineCache->Put(TO_IINTERFACE(key), TO_IINTERFACE(b));
}

ECode PagedViewIconCache::RemoveOutline(
    /* [in] */ IKey* key)
{
    Boolean res;
    mIconOutlineCache->ContainsKey(key, &res);
    if (res) {
        AutoPtr<IInterface> value;
        mIconOutlineCache->Get(TO_IINTERFACE(key), (IInterface**)&value);
        AutoPtr<IBitmap> map = IBitmap::Probe(value);
        map->Recycle();
        mIconOutlineCache->Remove(key);
    }
    return NOERROR;
}

ECode PagedViewIconCache::GetOutline(
    /* [in] */ IKey* key,
    /* [out] */ IBitmap** map)
{
    VALIDATE_NOT_NULL(map);

    AutoPtr<IInterface> obj;
    mIconOutlineCache->Get(TO_IINTERFACE(key), (IInterface**)&obj);
    AutoPtr<IBitmap> _map = IBitmap::Probe(obj);
    *map = _map;
    REFCOUNT_ADD(*map);
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos