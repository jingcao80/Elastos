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

#ifndef __ELASTOSX_XML_PARSERS_FILEPATHTOURI_H__
#define __ELASTOSX_XML_PARSERS_FILEPATHTOURI_H__

#include "Object.h"

namespace Elastosx {
namespace Xml {
namespace Parsers {

#define HEXCHS_NUM 16

class FilePathToURI
{
public:
    static CARAPI_(String) Filepath2URI(
        /* [in] */ const String& path);

private:
    static CARAPI_(Boolean) Init();

private:
    // which ASCII characters need to be escaped
    static Boolean gNeedEscaping[128];
    // the first hex character if a character needs to be escaped
    static Char32 gAfterEscaping1[128];
    // the second hex character if a character needs to be escaped
    static Char32 gAfterEscaping2[128];
    static Char32 gHexChs[HEXCHS_NUM];

    static Boolean sIsInitStatic;
};

} // namespace Parsers
} // namespace Xml
} // namespace Elastosx

#endif
