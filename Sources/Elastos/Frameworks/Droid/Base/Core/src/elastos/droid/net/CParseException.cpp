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


#include "elastos/droid/net/CParseException.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Thrown when parsing a URL fails.
*/
// See non-public class {@link WebAddress}.
CParseException::ParseException(
    /* [in] */ const String& in_response)
{
    this->response = in_response;
}


} //namespace Net
} //namespace Droid
} //namespace Elastos