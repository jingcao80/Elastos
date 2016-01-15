
#ifndef __ELASTOS_DROID_MEDIA_CMEDIAINSERTER_H__
#define __ELASTOS_DROID_MEDIA_CMEDIAINSERTER_H__

#include "Elastos.Droid.Net.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "_Elastos_Droid_Media_CMediaInserter.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IIContentProvider;
using Elastos::Droid::Net::IUri;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CMediaInserter)
    , public Object
    , public IMediaInserter
{
public:
    CMediaInserter();

    virtual ~CMediaInserter();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IIContentProvider* provider,
        /* [in] */ const String& packageName,
        /* [in] */ Int32 bufferSizePerUri);

    CARAPI Insert(
        /* [in] */ IUri* tableUri,
        /* [in] */ IContentValues* values);

    CARAPI InsertwithPriority(
        /* [in] */ IUri* tableUri,
        /* [in] */ IContentValues* values);

    CARAPI FlushAll();

private:
    CARAPI Insert(
        /* [in] */ IUri* tableUri,
        /* [in] */ IContentValues* values,
        /* [in] */ Boolean priority);

    CARAPI FlushAllPriority();

    CARAPI Flush(
        /* [in] */ IUri* tableUri,
        /* [in] */ IList* list);

private:
    // typedef List< AutoPtr<IContentValues> > ContentValuesList;
    // typedef typename ContentValuesList::Iterator ContentValuesListIterator;
    // typedef HashMap<AutoPtr<IUri>, AutoPtr<ContentValuesList> > UriContentValuesMap;
    // typedef typename UriContentValuesMap::Iterator UriContentValuesMapIterator;
    // UriContentValuesMap mRowMap;
    // UriContentValuesMap mPriorityRowMap;
    AutoPtr<IHashMap> mRowMap;
    AutoPtr<IHashMap> mPriorityRowMap;

    AutoPtr<IIContentProvider> mProvider;
    String mPackageName;
    Int32 mBufferSizePerUri;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CMEDIAINSERTER_H__
