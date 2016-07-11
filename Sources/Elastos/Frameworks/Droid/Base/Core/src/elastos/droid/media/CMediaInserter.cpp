
#include "elastos/droid/media/CMediaInserter.h"
#include "elastos/droid/content/CContentValues.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CContentValues;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Media {

static const String TAG("CMediaInserter");

CAR_INTERFACE_IMPL(CMediaInserter, Object, IMediaInserter)

CAR_OBJECT_IMPL(CMediaInserter)

CMediaInserter::CMediaInserter()
    : mBufferSizePerUri(0)
{
    CHashMap::New((IHashMap**)&mRowMap);
    CHashMap::New((IHashMap**)&mPriorityRowMap);
}

CMediaInserter::~CMediaInserter()
{
    mRowMap->Clear();
    mPriorityRowMap->Clear();
}

ECode CMediaInserter::constructor()
{
    return NOERROR;
}

ECode CMediaInserter::constructor(
    /* [in] */ IIContentProvider* provider,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 bufferSizePerUri)
{
    mProvider = provider;
    mPackageName = packageName;
    mBufferSizePerUri = bufferSizePerUri;
    return NOERROR;
}

ECode CMediaInserter::Insert(
    /* [in] */ IUri* tableUri,
    /* [in] */ IContentValues* values)
{
    return Insert(tableUri, values, FALSE);
}

ECode CMediaInserter::InsertwithPriority(
    /* [in] */ IUri* tableUri,
    /* [in] */ IContentValues* values)
{
    return Insert(tableUri, values, TRUE);
}

ECode CMediaInserter::Insert(
    /* [in] */ IUri* tableUri,
    /* [in] */ IContentValues* values,
    /* [in] */ Boolean priority)
{
    VALIDATE_NOT_NULL(tableUri);

    AutoPtr<IHashMap> rowmap = priority ? mPriorityRowMap : mRowMap;
    AutoPtr<IInterface> obj;
    rowmap->Get(tableUri, (IInterface**)&obj);
    AutoPtr<IArrayList> list = IArrayList::Probe(obj);
    if (list == NULL) {
        CArrayList::New((IArrayList**)&list);
        rowmap->Put(tableUri, list);
    }

    AutoPtr<IContentValues> contentValues;
    CContentValues::New(values, (IContentValues**)&contentValues);
    list->Add(contentValues);
    Int32 size;
    list->GetSize(&size);
    if (size >= mBufferSizePerUri) {
        FlushAllPriority();
        Flush(tableUri, IList::Probe(list));
    }
    return NOERROR;
}

ECode CMediaInserter::FlushAll()
{
    FlushAllPriority();

    AutoPtr<ISet> set;
    mRowMap->GetEntrySet((ISet**)&set);
    AutoPtr<IIterator> it;
    set->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj, key, value;
        it->GetNext((IInterface**)&obj);
        IMapEntry* me = IMapEntry::Probe(obj);
        me->GetKey((IInterface**)&key);
        me->GetValue((IInterface**)&value);
        Flush(IUri::Probe(key), IList::Probe(value));
    }
    mRowMap->Clear();
    return NOERROR;
}

ECode CMediaInserter::FlushAllPriority() // throws RemoteException
{
    AutoPtr<ISet> set;
    mPriorityRowMap->GetEntrySet((ISet**)&set);
    AutoPtr<IIterator> it;
    set->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj, key, value;
        it->GetNext((IInterface**)&obj);
        IMapEntry* me = IMapEntry::Probe(obj);
        me->GetKey((IInterface**)&key);
        me->GetValue((IInterface**)&value);
        Flush(IUri::Probe(key), IList::Probe(value));
    }
    mPriorityRowMap->Clear();
    return NOERROR;
}

ECode CMediaInserter::Flush(
    /* [in] */ IUri* tableUri,
    /* [in] */ IList* list)
{
    if (list != NULL) {
        Int32 size;
        list->GetSize(&size);
        AutoPtr<ArrayOf<IContentValues*> > values = ArrayOf<IContentValues*>::Alloc(size);

        AutoPtr<IIterator> it;
        list->GetIterator((IIterator**)&it);
        Boolean hasNext;
        Int32 i = 0;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            values->Set(i++, IContentValues::Probe(obj));
        }
        Int32 result;
        mProvider->BulkInsert(mPackageName, tableUri, values, &result);
        list->Clear();
    }
    return NOERROR;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
