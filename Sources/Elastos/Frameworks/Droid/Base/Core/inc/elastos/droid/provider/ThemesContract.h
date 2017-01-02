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

#ifndef __ELASTOS_DROID_PROVIDER_THEMESCONTRACT_H__
#define __ELASTOS_DROID_PROVIDER_THEMESCONTRACT_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Droid.Provider.h"
#include "_Elastos.Droid.Net.h"

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

class ECO_PUBLIC ThemesContract
{
public:
    class ThemesColumns
    {
    public:
        static const AutoPtr<IUri> CONTENT_URI;
    };

    /**
     * Key-value table which assigns a component (ex wallpaper) to a theme's package
     */
    class MixnMatchColumns
    {
    public:
        /**
         * For a given key value in the MixNMatch table, return the column
         * associated with it in the Themes Table. This is useful for URI based
         * elements like wallpaper where the caller wishes to determine the
         * wallpaper URI.
         */
        static CARAPI ComponentToImageColName(
            /* [in] */ const String& component,
            /* [out] */ String* str);

        /**
         * A component in the themes table (IE "mods_wallpaper") has an
         * equivalent key in mixnmatch table
         */
        static CARAPI ComponentToMixNMatchKey(
            /* [in] */ const String& component,
            /* [out] */ String* str);

        /**
         * A mixnmatch key in has an
         * equivalent value in the themes table
         */
        static CARAPI MixNMatchKeyToComponent(
            /* [in] */ const String& mixnmatchKey,
            /* [out] */ String* str);

    public:
        static const AutoPtr<IUri> CONTENT_URI;

        static const AutoPtr<ArrayOf<String> > ROWS;
    };

    /**
     * Table containing cached preview blobs for a given theme
     */
    class PreviewColumns
    {
    public:
        static const AutoPtr<IUri> CONTENT_URI;

        /**
         * Uri for retrieving the previews for the currently applied components.
         * Querying the themes provider using this URI will return a cursor with a single row
         * containing all the previews for the components that are currently applied.
         */
        static const AutoPtr<IUri> APPLIED_URI;
    };

public:
    static const AutoPtr<IUri> AUTHORITY_URI;
};

} // namespace Provider
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PROVIDER_THEMESCONTRACT_H__
