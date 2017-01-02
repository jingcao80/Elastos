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

#ifndef __ELASTOS_DROID_PROVIDER_SEARCHINDEXABLERESOURCE_H__
#define __ELASTOS_DROID_PROVIDER_SEARCHINDEXABLERESOURCE_H__

#include "elastos/droid/provider/SearchIndexableData.h"

namespace Elastos {
namespace Droid {
namespace Provider {

class SearchIndexableResource
    : public SearchIndexableData
    , public ISearchIndexableResource
{
public:
    SearchIndexableResource();

    virtual ~SearchIndexableResource();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ Int32 rank,
        /* [in] */ Int32 xmlResId,
        /* [in] */ const String& className,
        /* [in] */ Int32 iconResId);

    //@Override
    CARAPI ToString(
        /* [out] */ String* info);

    CARAPI SetXmlResId(
        /* [in] */ Int32 xmlResId);

    CARAPI GetXmlResId(
        /* [out] */ Int32* xmlResId);

    //override
    CARAPI SetKey(
        /* [in] */ const String& key);

    //override
    CARAPI SetClassName(
        /* [in] */ const String& className);

    //override
    CARAPI SetIntentAction(
        /* [in] */ const String& intentAction);

    //override
    CARAPI SetIntentTargetPackage(
        /* [in] */ const String& intentTargetPackage);

    CARAPI GetIntentTargetClass(
        /* [out] */ String* intentTargetClass);

public:
    Int32 mXmlResId;
};

} // namespace Provider
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PROVIDER_SEARCHINDEXABLERESOURCE_H__