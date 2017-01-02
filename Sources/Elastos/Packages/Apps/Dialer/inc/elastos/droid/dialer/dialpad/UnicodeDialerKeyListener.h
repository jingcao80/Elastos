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

#ifndef __ELASTOS_DROID_DIALER_DIALPAD_UNICODEDIALERKEYLISTENER_H__
#define __ELASTOS_DROID_DIALER_DIALPAD_UNICODEDIALERKEYLISTENER_H__

#include "_Elastos.Droid.Dialer.h"
#include "Elastos.Droid.Text.h"
#include "Elastos.CoreLibrary.Core.h"
#include <elastos/droid/text/method/DialerKeyListener.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Text::ISpanned;
using Elastos::Droid::Text::Method::DialerKeyListener;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Dialpad {

/**
 * {@link DialerKeyListener} with Unicode support. Converts any Unicode(e.g. Arabic) characters
 * that represent digits into digits before filtering the results so that we can support
 * pasted digits from Unicode languages.
 */
class UnicodeDialerKeyListener
    : public DialerKeyListener
{
public:
    // @Override
    CARAPI Filter(
        /* [in] */ ICharSequence* source,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ ISpanned* dest,
        /* [in] */ Int32 dstart,
        /* [in] */ Int32 dend,
        /* [out] */ ICharSequence** ret);

public:
    static const AutoPtr<UnicodeDialerKeyListener> INSTANCE;
};

} // Dialpad
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_DIALPAD_UNICODEDIALERKEYLISTENER_H__
