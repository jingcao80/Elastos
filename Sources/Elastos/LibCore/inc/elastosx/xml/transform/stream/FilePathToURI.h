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

#ifndef  __ELASTOSX_XML_TRANSFORM_STREAM_FILEPATHTOURI_H__
#define  __ELASTOSX_XML_TRANSFORM_STREAM_FILEPATHTOURI_H__

#include <elastos.h>

using namespace Elastos;

namespace Elastosx {
namespace Xml {
namespace Transform {
namespace Stream {

class FilePathToURI
{
public:
    // To escape a file path to a URI, by using %HH to represent
    // special ASCII characters: 0x00~0x1F, 0x7F, ' ', '<', '>', '#', '%'
    // and '"' and non-ASCII characters (whose value >= 128).
    static String Filepath2URI(
        /* [in] */ const String& path);

private:
    // initialize the above 3 arrays
    static CARAPI_(Boolean) InitArrays();

private:
    // which ASCII characters need to be escaped
    static AutoPtr<ArrayOf<Boolean> > gNeedEscaping/*[] = new Boolean[128]*/;
    // the first hex character if a character needs to be escaped
    static AutoPtr<ArrayOf<Char32> > gAfterEscaping1/* = new char[128]*/;
    // the second hex character if a character needs to be escaped
    static AutoPtr<ArrayOf<Char32> > gAfterEscaping2/* = new char[128]*/;
    static Char32 gHexChs[]/* = {'0', '1', '2', '3', '4', '5', '6', '7',
                                     '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'}*/;

    static Boolean sInit;
};

} // namespace Stream
} // namespace Transform
} // namespace Xml
} // namespace Elastosx

#endif // __ELASTOSX_XML_TRANSFORM_STREAM_FILEPATHTOURI_H__
