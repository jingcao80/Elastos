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
