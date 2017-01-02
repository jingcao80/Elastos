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

#include "MonkeyFlipEvent.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::CFileOutputStream;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

const Byte MonkeyFlipEvent::mFlip0[16] = {
    0x7f, 0x06,
    0x00, 0x00,
    (byte) 0xe0, 0x39,
    0x01, 0x00,
    0x05, 0x00,
    0x00, 0x00,
    0x01, 0x00,
    0x00, 0x00
};

const Byte MonkeyFlipEvent::mFlip1[16] = {
    (byte) 0x85, 0x06,
    0x00, 0x00,
    (byte) 0x9f, (byte) 0xa5,
    0x0c, 0x00,
    0x05, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00
};

MonkeyFlipEvent::MonkeyFlipEvent()
    : MonkeyEvent(IMonkeyEvent::EVENT_TYPE_FLIP)
    , mIsKeyBoardOpen(FALSE)
{
}

MonkeyFlipEvent::MonkeyFlipEvent(
    /* [in] */ Boolean isKeyboardOpen)
    : MonkeyEvent(IMonkeyEvent::EVENT_TYPE_FLIP)
    , mIsKeyBoardOpen(isKeyboardOpen)
{
}

ECode MonkeyFlipEvent::Init(
    /* [in] */ Boolean isKeyboardOpen)
{
    MonkeyEvent::Init(IMonkeyEvent::EVENT_TYPE_FLIP);
    Boolean* pTmp = (Boolean*)&mIsKeyBoardOpen;
    *pTmp = isKeyboardOpen;
    return NOERROR;
}

Int32 MonkeyFlipEvent::InjectEvent(
    /* [in] */ IIWindowManager *iwm,
    /* [in] */ IIActivityManager *iam,
    /* [in] */ Int32 verbose)
{
    if (verbose > 0) {
        PFL_EX(String(":Sending Flip keyboardOpen = ") + StringUtils::BooleanToString(mIsKeyBoardOpen))
    }

    //inject flip event
    AutoPtr<IFileOutputStream> fos;
    ECode ec = CFileOutputStream::New(String("/dev/input/event0"), (IFileOutputStream**)&fos);
    if (FAILED(ec)) {
        if (fos) fos->Close();
        return IMonkeyEvent::INJECT_FAIL;
    }

    AutoPtr<ArrayOf<Byte> > tmp = mIsKeyBoardOpen
        ? (ArrayOf<Byte>::Alloc((Byte*)mFlip0, 16))
        : (ArrayOf<Byte>::Alloc((Byte*)mFlip1, 16));
    fos->WriteBytes(*tmp);
    fos->Close();
    return IMonkeyEvent::INJECT_SUCCESS;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos
