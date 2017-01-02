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

#ifndef __ELASTOS_UTILITY_JAR_ELASTOS_UTILITY_MANIFEST_READER_H__
#define __ELASTOS_UTILITY_JAR_ELASTOS_UTILITY_MANIFEST_READER_H__

#include "Object.h"
#include <elastos/utility/etl/HashMap.h>
#include "CManifest.h"

using Elastos::IO::IByteArrayOutputStream;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Utility {
namespace Jar {

/**
 * Reads a JAR file manifest. The specification is here:
 * http://java.sun.com/javase/6/docs/technotes/guides/jar/jar.html
 */
class ManifestReader
    : public Object
{
public:
    ManifestReader(
        /* [in] */ ArrayOf<Byte>* buf,
        /* [in] */ IAttributes* main);

    CARAPI ReadEntries(
        /* [in] */ HashMap<String, AutoPtr<IAttributes> > * entries,
        /* [in] */ HashMap<String, AutoPtr<CManifest::Chunk> > * chunks);

    CARAPI ReadEntries(
        /* [in] */ IMap * entries,
        /* [in] */ HashMap<String, AutoPtr<CManifest::Chunk> > * chunks);

    CARAPI_(Int32) GetEndOfMainSection();

    /**
     * Read a single line from the manifest buffer.
     */
    CARAPI ReadHeader(
        /* [out] */ Boolean * result);

    CARAPI ReadName();

    CARAPI ReadValue();

private:
    // There are relatively few unique attribute names,
    // but a manifest might have thousands of entries.
    HashMap<String, AutoPtr<IName> > mAttributeNameCache;

    AutoPtr<IByteArrayOutputStream> mValueBuffer;

    AutoPtr<ArrayOf<Byte> > mBuf;

    Int32 mEndOfMainSection;

    Int32 mPos;

    AutoPtr<IName> mName;

    String mValue;

    Int32 mConsecutiveLineBreaks;
};

} // namespace Jar
} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY_JAR_ELASTOS_UTILITY_MANIFEST_READER_H__
