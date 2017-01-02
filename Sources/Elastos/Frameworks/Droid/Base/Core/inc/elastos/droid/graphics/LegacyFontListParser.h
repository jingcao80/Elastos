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

#ifndef __ELASTOS_DROID_GRAPHICS_LEGACYFONTLISTPARSER_H__
#define __ELASTOS_DROID_GRAPHICS_LEGACYFONTLISTPARSER_H__

#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.CoreLibrary.External.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::IO::IInputStream;
using Elastos::Utility::IList;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace Graphics {

/**
 * Parses an XML font config. Example:
 *
 *<familyset>
 *
 *  <family>
 *      <nameset>
 *          <name>sans-serif</name>
 *          <name>arial</name>
 *      </nameset>
 *      <fileset>
 *          <file>Roboto-Regular.ttf</file>
 *          <file>Roboto-Bold.ttf</file>
 *          <file>Roboto-Italic.ttf</file>
 *          <file>Roboto-BoldItalic.ttf</file>
 *      </fileset>
 *  </family>
 *  <family>
 *    ...
 *  </family>
 *</familyset>
 * @hide
 */
class LegacyFontListParser
{
public:
    class Family
        : public Object
    {
    public:
        Family();

        CARAPI_(String) GetName();

    public:
        AutoPtr<IList> mNameset;
        AutoPtr<IList> mFileset;
    };

public:
    static CARAPI Parse(
        /* [in] */ IInputStream* in,
        /* [out] */ IList** result);

private:
    static CARAPI ReadFamilySet(
        /* [in] */ IXmlPullParser* parser,
        /* [out] */ IList** result);

    static CARAPI ReadFamily(
        /* [in] */ IXmlPullParser* parser,
        /* [out] */ Family** result);

    static CARAPI ReadNameset(
        /* [in] */ IXmlPullParser* parser,
        /* [out] */ IList** result);

    static CARAPI ReadFileset(
        /* [in] */ IXmlPullParser* parser,
        /* [out] */ IList** result);

    // For the tags title and summary, extracts their text values.
    static CARAPI ReadText(
        /* [in] */ IXmlPullParser* parser,
        /* [out] */ String* result);

    static CARAPI Skip(
        /* [in] */ IXmlPullParser* parser);
};

} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_LEGACYFONTLISTPARSER_H__
