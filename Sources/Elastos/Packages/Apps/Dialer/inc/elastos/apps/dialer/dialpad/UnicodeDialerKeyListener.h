#ifndef __ELASTOS_APPS_DIALER_DIALPAD_UNICODEDIALERKEYLISTENER_H__
#define __ELASTOS_APPS_DIALER_DIALPAD_UNICODEDIALERKEYLISTENER_H__

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace Dialpad {

/**
 * {@link DialerKeyListener} with Unicode support. Converts any Unicode(e.g. Arabic) characters
 * that represent digits into digits before filtering the results so that we can support
 * pasted digits from Unicode languages.
 */
class UnicodeDialerKeyListener
    : public DialerKeyListener
    , public IUnicodeDialerKeyListener
{
public:
    CAR_INTERFACE_DECL()

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
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_DIALPAD_UNICODEDIALERKEYLISTENER_H__
