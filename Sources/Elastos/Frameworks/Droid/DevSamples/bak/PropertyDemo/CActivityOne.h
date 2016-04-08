
#ifndef __CACTIVITYONE_H__
#define __CACTIVITYONE_H__

#include "elastos/droid/app/Activity.h"
#include "_CActivityOne.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Utility::IProperty;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace PropertyDemo {


class CActivityOne : public Activity
{
private:
    class MyListener
        : public IViewOnClickListener
        , public ElRefBase
    {
    public:
        CAR_INTERFACE_DECL()

        MyListener(
            /* [in] */ CActivityOne* host);

        CARAPI OnClick(
            /* [in] */ IView* v);
    private:
        CARAPI_(AutoPtr<IProperty>) CreateProperty(
            /* [in] */ CarDataType carDateType,
            /* [in] */ const String& name);

        template<typename T>
            AutoPtr<ICharSequence> GetResultStr_Get(
                /* [in] */ T lValue,
                /* [in] */ T rValue);

        template<typename T>
            AutoPtr<ICharSequence> GetResultStr_Set(
                /* [in] */ T lValue,
                /* [in] */ T rValue);

        CARAPI GetByte();

        CARAPI SetByte();

        CARAPI GetChar8();

        CARAPI SetChar8();

        CARAPI GetChar16();

        CARAPI SetChar16();

        CARAPI GetBoolean();

        CARAPI SetBoolean();

        CARAPI GetInt16();

        CARAPI SetInt16();

        CARAPI GetInt32();

        CARAPI SetInt32();

        CARAPI GetInt64();

        CARAPI SetInt64();

        CARAPI GetString();

        CARAPI SetString();

        CARAPI GetFloat();

        CARAPI SetFloat();

        CARAPI GetDouble();

        CARAPI SetDouble();

        CARAPI GetInterface();

        CARAPI SetInterface();

    private:
        CActivityOne* mHost;
        AutoPtr<ITestClass> mClsTarget;
        AutoPtr<ICharSequence> mRstStr;
    };
protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnStart();

    CARAPI OnResume();

    CARAPI OnPause();

    CARAPI OnStop();

    CARAPI OnDestroy();
private:
    AutoPtr<IButton> mGetByteButton;
    AutoPtr<IButton> mSetByteButton;
    AutoPtr<IButton> mGetChar8Button;
    AutoPtr<IButton> mSetChar8Button;
    AutoPtr<IButton> mGetChar16Button;
    AutoPtr<IButton> mSetChar16Button;
    AutoPtr<IButton> mGetBooleanButton;
    AutoPtr<IButton> mSetBooleanButton;
    AutoPtr<IButton> mGetInt16Button;
    AutoPtr<IButton> mSetInt16Button;
    AutoPtr<IButton> mGetInt32Button;
    AutoPtr<IButton> mSetInt32Button;
    AutoPtr<IButton> mGetInt64Button;
    AutoPtr<IButton> mSetInt64Button;
    AutoPtr<IButton> mGetStringButton;
    AutoPtr<IButton> mSetStringButton;
    AutoPtr<IButton> mGetFloatButton;
    AutoPtr<IButton> mSetFloatButton;
    AutoPtr<IButton> mGetDoubleButton;
    AutoPtr<IButton> mSetDoubleButton;
    AutoPtr<IButton> mGetInterfaceButton;
    AutoPtr<IButton> mSetInterfaceButton;
    AutoPtr<ITextView> mTextView;
};

} // namespace PropertyDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CACTIVITYONE_H__
