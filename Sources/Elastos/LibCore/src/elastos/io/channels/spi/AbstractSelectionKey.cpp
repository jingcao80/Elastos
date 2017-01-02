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

#include "AbstractSelectionKey.h"

namespace Elastos {
namespace IO {
namespace Channels {
namespace Spi {

CAR_INTERFACE_IMPL(AbstractSelectionKey, SelectionKey, IAbstractSelectionKey)

AbstractSelectionKey::AbstractSelectionKey()
    : mIsValid(FALSE)
{}

AbstractSelectionKey::~AbstractSelectionKey()
{}

ECode AbstractSelectionKey::IsValid(
    /* out*/ Boolean* isValid)
{
    *isValid = mIsValid;
    return NOERROR;
}

ECode AbstractSelectionKey::Cancel()
{
    if (mIsValid) {
        mIsValid = FALSE;
        AutoPtr<ISelector> selector;
        ECode ecRet = GetSelector((ISelector**)&selector);
        if (NOERROR == ecRet) {
            IAbstractSelector* absSel = IAbstractSelector::Probe(selector);
            absSel->Cancel(this);
        }
    }
    return NOERROR;
}

} // namespace Spi
} // namespace Channels
} // namespace IO
} // namespace Elastos
