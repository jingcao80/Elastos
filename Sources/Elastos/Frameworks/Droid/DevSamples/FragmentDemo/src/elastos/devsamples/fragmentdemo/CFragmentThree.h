
#ifndef __Elastos_DevSamples_FragmentDemo_CFragmentThree_H__
#define __Elastos_DevSamples_FragmentDemo_CFragmentThree_H__

#include <elastos/droid/app/Fragment.h>
#include "_Elastos_DevSamples_FragmentDemo_CFragmentThree.h"

using Elastos::Droid::App::Fragment;
using Elastos::Droid::View::IViewOnClickListener;

namespace Elastos {
namespace DevSamples {
namespace FragmentDemo {

CarClass(CFragmentThree)
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
            /* [in] */ CFragmentThree* host);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CFragmentThree* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CFragmentThree();

    ~CFragmentThree();

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

#endif // __Elastos_DevSamples_FragmentDemo_CFragmentThree_H__
