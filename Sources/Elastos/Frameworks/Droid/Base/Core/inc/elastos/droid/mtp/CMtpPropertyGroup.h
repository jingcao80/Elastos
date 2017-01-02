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

#ifndef __ELASTOS_DROID_MTP_CMTPPROPERTYGROUP_H__
#define __ELASTOS_DROID_MTP_CMTPPROPERTYGROUP_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include "_Elastos_Droid_Mtp_CMtpPropertyGroup.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Elastos::Droid::Content::IIContentProvider;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Mtp::IMtpDatabase;
using Elastos::Droid::Mtp::IMtpPropertyList;

namespace Elastos {
namespace Droid {
namespace Mtp {

CarClass(CMtpPropertyGroup)
    , public Object
    , public IMtpPropertyGroup
{
public:
    CMtpPropertyGroup();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IMtpDatabase* database,
        /* [in] */ IIContentProvider* provider,
        /* [in] */ const String& packageName,
        /* [in] */ const String& volume,
        /* [in] */ ArrayOf<Int32>* properties);

    CARAPI GetPropertyList(
        /* [in] */ Int32 handle,
        /* [in] */ Int32 format,
        /* [in] */ Int32 depth,
        /* [out] */ IMtpPropertyList** list);

private:
    class Property
        : public ElRefBase
    {
    public:
        // MTP property code
        Int32 mCode;
        // MTP data type
        Int32 mType;
        // column index for our query
        Int32 mColumn;

        Property(
            /* [in] */ Int32 code,
            /* [in] */ Int32 type,
            /* [in] */ Int32 column);
    };

private:
    CARAPI_(AutoPtr<Property>) CreateProperty(
        /* [in] */ Int32 code,
        /* [in] */ List<String>* columns);

    CARAPI_(String) QueryString(
        /* [in] */ Int32 id,
        /* [in] */ const String& column);

    CARAPI_(String) QueryAudio(
        /* [in] */ Int32 id,
        /* [in] */ const String& column);

    CARAPI_(String) QueryGenre(
        /* [in] */ Int32 id);

    CARAPI_(Int64) QueryInt64(
        /* [in] */ Int32 id,
        /* [in] */ const String& column);

    static CARAPI_(String) NameFromPath(
        /* [in] */ const String& path);

    CARAPI_(String) NativeFormatDateTime(
        /* [in] */ Int64 seconds);

private:
    AutoPtr<IMtpDatabase> mDatabase;
    AutoPtr<IIContentProvider> mProvider;
    String mPackageName;
    String mVolumeName;
    AutoPtr<IUri> mUri;

    // list of all properties in this group
    AutoPtr<ArrayOf<Property*> > mProperties;

    // list of columns for database query
    AutoPtr<ArrayOf<String> > mColumns;

    static const String TAG;
    static const String ID_WHERE;
    static const String FORMAT_WHERE;
    static const String ID_FORMAT_WHERE;
    static const String PARENT_WHERE;
    static const String PARENT_FORMAT_WHERE;
};

} // namespace Mtp
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MTP_CMTPPROPERTYGROUP_H__
