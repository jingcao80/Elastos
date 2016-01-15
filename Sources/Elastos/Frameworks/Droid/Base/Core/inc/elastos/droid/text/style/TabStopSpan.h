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
