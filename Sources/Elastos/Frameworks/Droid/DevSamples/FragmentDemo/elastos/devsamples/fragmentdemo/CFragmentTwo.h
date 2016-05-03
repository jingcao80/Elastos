
#ifndef __Elastos_DevSamples_FragmentDemo_CFragmentTwo_H__
#define __Elastos_DevSamples_FragmentDemo_CFragmentTwo_H__

#include <elastos/droid/app/Fragment.h>
#include "_Elastos_DevSamples_FragmentDemo_CFragmentTwo.h"

using Elastos::Droid::App::Fragment;
using Elastos::Droid::View::IViewOnClickListener;

namespace Elastos {
namespace DevSamples {
namespace FragmentDemo {

CarClass(CFragmentTwo)
    , public Fragment
{
public:
    class MyListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyListener(
            /* [in] */ CFragmentTwo* host);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CFragmentTwo* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI OnCreateView(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IViewGroup* container,
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IView** result);

};

} // namespace FragmentDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_FragmentDemo_CFragmentTwo_H__
