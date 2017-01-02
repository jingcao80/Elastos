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

#ifndef __ELASTOS_IO_CHANNELS_SPI_ABSTRACTSELECTOR_H__
#define __ELASTOS_IO_CHANNELS_SPI_ABSTRACTSELECTOR_H__

#include "Selector.h"
#include <elastos/core/Runnable.h>

using Elastos::Core::Runnable;
using Elastos::IO::Channels::Selector;
using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;

namespace Elastos {
namespace IO {
namespace Channels {
namespace Spi {

/**
 * {@code AbstractSelector} is the base implementation class for selectors.
 * It realizes the interruption of selection by {@code begin} and
 * {@code end}. It also holds the cancellation and the deletion of the key
 * set.
 */
class AbstractSelector
    : public Selector
    , public IAbstractSelector
{
protected:
    class WakeupRunnable
        : public Runnable
    {
    public:
        WakeupRunnable(
            /* [in] */ AbstractSelector* selector);

        CARAPI Run();
    private:
        AbstractSelector* mSelector;
    };

public:
    AbstractSelector();

    AbstractSelector(
        /* [in] */ ISelectorProvider* provider);

    virtual ~AbstractSelector();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ ISelectorProvider* provider);

    CARAPI Close();

    CARAPI IsOpen(
        /* [out] */ Boolean* isOpen);

    CARAPI GetProvider(
        /* [out] */ ISelectorProvider** provider);

    virtual CARAPI Register(
        /* [in] */ IAbstractSelectableChannel* channel,
        /* [in] */ Int32 operations,
        /* [in] */ IObject* obj,
        /* [out] */ ISelectionKey** returnKey) = 0;

protected:
    virtual CARAPI ImplCloseSelector() = 0;

    CARAPI CancelledKeys(
        /* [out] */ ISet** keySet);

    CARAPI Deregister(
        /* [in] */ ISelectionKey* key);

    CARAPI Begin();

    CARAPI End();

    CARAPI Cancel(
        /* [in] */ ISelectionKey* key);

private:
    AutoPtr<IAtomicBoolean> mIsOpen;
    AutoPtr<ISelectorProvider> mSelectorProvider;
    AutoPtr<ISet> mCancelledKeySet;
    AutoPtr<IRunnable> mWakeupRunnable;
};

} // namespace Spi
} // namespace Channels
} // namespace IO
} // namespace Elastos

#endif
