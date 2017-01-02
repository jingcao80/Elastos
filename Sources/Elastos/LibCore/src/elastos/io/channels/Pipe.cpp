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

#include "Pipe.h"
#include "SelectorProvider.h"

using Elastos::IO::Channels::Spi::ISelectorProvider;
using Elastos::IO::Channels::Spi::SelectorProvider;

namespace Elastos{
namespace IO{
namespace Channels{

Pipe::Pipe()
{
}

CAR_INTERFACE_IMPL(Pipe, Object, IPipe)

ECode Pipe::Open(
    /* [out] */ IPipe** pipe)
{
    AutoPtr<ISelectorProvider> sp;
    FAIL_RETURN(SelectorProvider::GetProvider((ISelectorProvider**)&sp));
    return sp->OpenPipe(pipe);
}

} // namespace Channels
} // namespace IO
} // namespace Elastoss
