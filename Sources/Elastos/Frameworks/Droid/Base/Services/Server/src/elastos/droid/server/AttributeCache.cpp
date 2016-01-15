#include "elastos/droid/server/AttributeCache.h"
#include <Elastos.Droid.Os.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Content::Res::CConfiguration;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Core::CoreUtils;
using Elastos::Core::ICharSequence;
using Elastos::Utility::CWeakHashMap;

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
    CWeakHashMap::New((IWeakHashMap**)&mPackages);
}

AttributeCache::~AttributeCache()
{
}

void AttributeCache::UpdateConfiguration(
    /* [in] */ IConfiguration* config)
{
    synchronized(this) {
        Int32 changes;
        mConfiguration->UpdateFrom(config, &changes);
        if ((changes & ~(IActivityInfo::CONFIG_FONT_SCALE |
                IActivityInfo::CONFIG_KEYBOARD_HIDDEN |
                IActivityInfo::CONFIG_ORIENTATION)) != 0) {
            mPackages->Clear();
        }
    }
}

void AttributeCache::RemovePackage(
    /* [in] */ const String& packageName)
{
    synchronized(this) {
        AutoPtr<ICharSequence> csq = CoreUtils::Convert(packageName);
        mPackages->Remove(csq.Get());
    }
}

AutoPtr<Entry> AttributeCache::Get(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 resId,
    /* [in] */ ArrayOf<Int32>* styleable,
    /* [in] */ Int32 userId)
{
    AutoPtr<Entry> ent;
    synchronized(this) {
        AutoPtr<ICharSequence> csq = CoreUtils::Convert(packageName);
        AutoPtr<IInterface> obj;
        mPackages->Get(csq.Get(), (IInterface**)&obj);

        AutoPtr<Package> pkg;
        if (obj != NULL) {
            pkg = (Package*)IObject::Probe(obj);
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
            mPackages->Put(csq.Get(), TO_IINTERFACE(pkg));
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
        ent = new Entry(pkg->mContext, typeArray);
        (*entryMap)[styleable] = ent;
    }
    return ent;
}

}// Server
}// Droid
}// Elastos
