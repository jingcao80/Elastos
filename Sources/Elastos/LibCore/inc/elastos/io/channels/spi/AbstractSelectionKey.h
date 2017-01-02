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

#ifndef __ELASTOS_IO_CHANNELS_SPI_ABSTRACTSELECTIONKEY_H__
#define __ELASTOS_IO_CHANNELS_SPI_ABSTRACTSELECTIONKEY_H__

#include "SelectionKey.h"

using Elastos::IO::Channels::SelectionKey;

namespace Elastos {
namespace IO {
namespace Channels {
namespace Spi {

/**
 * {@code AbstractSelectionKey} is the base implementation class for selection keys.
 * It implements validation and cancellation methods.
 */
class AbstractSelectionKey
    : public SelectionKey
    , public IAbstractSelectionKey
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Indicates whether this key is valid. A key is valid as long as it has not
     * been canceled.
     *
     * @return {@code true} if this key has not been canceled, {@code false}
     *         otherwise.
     */
    CARAPI IsValid(
        /* [out] */ Boolean* isValid);

    /**
     * Cancels this key.
     * <p>
     * A key that has been canceled is no longer valid. Calling this method on
     * an already canceled key does nothing.
     */
    CARAPI Cancel();

protected:

    AbstractSelectionKey();

    virtual ~AbstractSelectionKey();

private:
    Boolean mIsValid;
};

} // namespace Spi
} // namespace Channels
} // namespace IO
} // namespace Elastos

#endif
