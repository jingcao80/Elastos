#ifndef __CAboutACTIVITY_H__
#define __CAboutACTIVITY_H__

#include "_CAboutActivity.h"
#include "elastos/droid/app/Activity.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::ITextView;

namespace MusicPlayer {

class CAboutActivity: public Activity
{
public:
    class MyListener
        : public ElRefBase
        , public IViewOnClickListener
    {
    public:
        MyListener(
          /* [in] */ CAboutActivity* host);

        CAR_INTERFACE_DECL()

        CARAPI OnClick(
          /* [in] */ IView* v);

    private:
        CAboutActivity* mActivity;
    };

protected:
    CARAPI OnCreate(
        /* [in] */IBundle* savedInstanceState);

private:
    AutoPtr<IView> mImgViewBack;
    AutoPtr<MyListener> mActionListener;
    AutoPtr<ITextView> mTextViewVersion;
    AutoPtr<ITextView> mTextViewDeveloper;
};

} // namespace MusicPlayer

#endif // __CAboutACTIVITY_H__
