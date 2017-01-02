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

#ifndef __ELASTOS_DROID_TEXT_STYLE_TabStopSpan_H__
#define __ELASTOS_DROID_TEXT_STYLE_TabStopSpan_H__

#include "Elastos.Droid.Text.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

/**
 * Represents a single tab stop on a line.
 */
//public interface
class TabStopSpan
    : public Object
    , public ITabStopSpan
    , public IParagraphStyle
{
public:
    CAR_INTERFACE_DECL()

    TabStopSpan();

    virtual ~TabStopSpan();
};

/**
 * The default implementation of TabStopSpan.
 */
//static
class TabStopSpanStandard
    : public TabStopSpan
    , public ITabStopSpanStandard
{
public:
    CAR_INTERFACE_DECL()

    TabStopSpanStandard();

    virtual ~TabStopSpanStandard();

    /**
     * Constructor.
     *
     * @param where the offset of the tab stop from the leading margin of
     *        the line
     */
    CARAPI constructor(
        /* [in] */ Int32 where);

    CARAPI GetTabStop(
        /* [out] */ Int32* ret);

private:
    Int32 mTab;
};

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_STYLE_TabStopSpan_H__
