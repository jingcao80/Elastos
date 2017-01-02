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

#ifndef __ELASTOS_DROID_WIDGET_CURSORFILTER_H__
#define __ELASTOS_DROID_WIDGET_CURSORFILTER_H__

#include "elastos/droid/widget/Filter.h"

namespace Elastos {
namespace Droid {
namespace Widget {

/**
 * <p>The CursorFilter delegates most of the work to the CursorAdapter.
 * Subclasses should override these delegate methods to run the queries
 * and convert the results into String that can be used by auto-completion
 * widgets.</p>
 */
class CursorFilter
    : public Filter
    , public ICursorFilter
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ ICursorFilterClient* client);

    virtual CARAPI ConvertResultToString(
        /* [in] */ IInterface* resultValue,
        /* [out] */ ICharSequence** cs);

    virtual CARAPI PerformFiltering(
        /* [in] */ ICharSequence* constraint,
        /* [out] */ IFilterResults** filterResults);

    virtual CARAPI PublishResults(
        /* [in] */ ICharSequence* constraint,
        /* [in] */ IFilterResults* results);

    AutoPtr<ICursorFilterClient> mClient;
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos
#endif
