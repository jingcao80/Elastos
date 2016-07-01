#ifndef __ELASTOS_DROID_DIALER_DIALPAD_UNICODEDIALERKEYLISTENER_H__
#define __ELASTOS_DROID_DIALER_DIALPAD_UNICODEDIALERKEYLISTENER_H__

#include "_Elastos.Droid.Dialer.h"
#include <elastos/core/Object.h>
#include "Elastos.Droid.Text.h"
#include "Elastos.CoreLibrary.Core.h"

using Elastos::Droid::Text::ISpanned;
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
    // TODO:
    /*: public DialerKeyListener*/
    : public Object
    , public IUnicodeDialerKeyListener
{
public:
    CAR_INTERFACE_DECL();

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
    static const AutoPtr<IUnicodeDialerKeyListener> INSTANCE;
};

} // Dialpad
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_DIALPAD_UNICODEDIALERKEYLISTENER_H__
