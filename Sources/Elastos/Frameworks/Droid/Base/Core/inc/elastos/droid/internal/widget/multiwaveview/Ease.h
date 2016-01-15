
#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_MULTIWAVEVIEW_EASE_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_MULTIWAVEVIEW_EASE_H__

#include "Elastos.Droid.Animation.h"
#include "elastos/droid/ext/frameworkext.h"

#include <elastos/core/Object.h>

using Elastos::Droid::Animation::ITimeInterpolator;

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {
namespace Multiwaveview {

class Ease
    : public Object
{
public:
    class Linear
        : public Object
    {
    public:
        class TimeInterpolator
            : public Object
            , public ITimeInterpolator
        {
        public:
            CAR_INTERFACE_DECL()

            CARAPI GetInterpolation(
                /* [in] */ Float input,
                /* [out] */ Float* res);

            CARAPI HasNativeInterpolator(
                /* [out] */ Boolean* res);
        };

    public:
        static AutoPtr<TimeInterpolator> mEaseNone;
    };

    class Cubic
        : public Object
    {
    public:
        class TimeInterpolatorIn
            : public Object
            , public ITimeInterpolator
        {
        public:
            CAR_INTERFACE_DECL()

            CARAPI GetInterpolation(
                /* [in] */ Float input,
                /* [out] */ Float* res);

            CARAPI HasNativeInterpolator(
                /* [out] */ Boolean* res);
        };

        class TimeInterpolatorOut
            : public Object
            , public ITimeInterpolator
        {
        public:
            CAR_INTERFACE_DECL()

            CARAPI GetInterpolation(
                /* [in] */ Float input,
                /* [out] */ Float* res);

            CARAPI HasNativeInterpolator(
                /* [out] */ Boolean* res);
        };

        class TimeInterpolatorInOut
            : public Object
            , public ITimeInterpolator
        {
        public:
            CAR_INTERFACE_DECL()

            CARAPI GetInterpolation(
                /* [in] */ Float input,
                /* [out] */ Float* res);

            CARAPI HasNativeInterpolator(
                /* [out] */ Boolean* res);
        };

    public:
        static AutoPtr<TimeInterpolatorIn> mEaseIn;

        static AutoPtr<TimeInterpolatorOut> mEaseOut;

        static AutoPtr<TimeInterpolatorInOut> mEaseInOut;
    };

    class Quad
        : public Object
    {
    public:
        class TimeInterpolatorIn
            : public Object
            , public ITimeInterpolator
        {
        public:
            CAR_INTERFACE_DECL()

            CARAPI GetInterpolation(
                /* [in] */ Float input,
                /* [out] */ Float* res);

            CARAPI HasNativeInterpolator(
                /* [out] */ Boolean* res);
        };

        class TimeInterpolatorOut
            : public Object
            , public ITimeInterpolator
        {
        public:
            CAR_INTERFACE_DECL()

            CARAPI GetInterpolation(
                /* [in] */ Float input,
                /* [out] */ Float* res);

            CARAPI HasNativeInterpolator(
                /* [out] */ Boolean* res);
        };

        class TimeInterpolatorInOut
            : public Object
            , public ITimeInterpolator
        {
        public:
            CAR_INTERFACE_DECL()

            CARAPI GetInterpolation(
                /* [in] */ Float input,
                /* [out] */ Float* res);

            CARAPI HasNativeInterpolator(
                /* [out] */ Boolean* res);
        };

    public:
        static AutoPtr<TimeInterpolatorIn> mEaseIn;

        static AutoPtr<TimeInterpolatorOut> mEaseOut;

        static AutoPtr<TimeInterpolatorInOut> mEaseInOut;
    };

    class Quart
        : public Object
    {
    public:
        class TimeInterpolatorIn
            : public Object
            , public ITimeInterpolator
        {
        public:
            CAR_INTERFACE_DECL()

            CARAPI GetInterpolation(
                /* [in] */ Float input,
                /* [out] */ Float* res);

            CARAPI HasNativeInterpolator(
                /* [out] */ Boolean* res);
        };

        class TimeInterpolatorOut
            : public Object
            , public ITimeInterpolator
        {
        public:
            CAR_INTERFACE_DECL()

            CARAPI GetInterpolation(
                /* [in] */ Float input,
                /* [out] */ Float* res);

            CARAPI HasNativeInterpolator(
                /* [out] */ Boolean* res);
        };

        class TimeInterpolatorInOut
            : public Object
            , public ITimeInterpolator
        {
        public:
            CAR_INTERFACE_DECL()

            CARAPI GetInterpolation(
                /* [in] */ Float input,
                /* [out] */ Float* res);

            CARAPI HasNativeInterpolator(
                /* [out] */ Boolean* res);
        };

    public:
        static AutoPtr<TimeInterpolatorIn> mEaseIn;

        static AutoPtr<TimeInterpolatorOut> mEaseOut;

        static AutoPtr<TimeInterpolatorInOut> mEaseInOut;
    };

    class Quint
        : public Object
    {
    public:
        class TimeInterpolatorIn
            : public Object
            , public ITimeInterpolator
        {
        public:
            CAR_INTERFACE_DECL()

            CARAPI GetInterpolation(
                /* [in] */ Float input,
                /* [out] */ Float* res);

            CARAPI HasNativeInterpolator(
                /* [out] */ Boolean* res);
        };

        class TimeInterpolatorOut
            : public Object
            , public ITimeInterpolator
        {
        public:
            CAR_INTERFACE_DECL()

            CARAPI GetInterpolation(
                /* [in] */ Float input,
                /* [out] */ Float* res);

            CARAPI HasNativeInterpolator(
                /* [out] */ Boolean* res);
        };

        class TimeInterpolatorInOut
            : public Object
            , public ITimeInterpolator
        {
        public:
            CAR_INTERFACE_DECL()

            CARAPI GetInterpolation(
                /* [in] */ Float input,
                /* [out] */ Float* res);

            CARAPI HasNativeInterpolator(
                /* [out] */ Boolean* res);
        };

    public:
        static AutoPtr<TimeInterpolatorIn> mEaseIn;

        static AutoPtr<TimeInterpolatorOut> mEaseOut;

        static AutoPtr<TimeInterpolatorInOut> mEaseInOut;
    };

    class Sine
        : public Object
    {
    public:
        class TimeInterpolatorIn
            : public Object
            , public ITimeInterpolator
        {
        public:
            CAR_INTERFACE_DECL()

            CARAPI GetInterpolation(
                /* [in] */ Float input,
                /* [out] */ Float* res);

            CARAPI HasNativeInterpolator(
                /* [out] */ Boolean* res);
        };

        class TimeInterpolatorOut
            : public Object
            , public ITimeInterpolator
        {
        public:
            CAR_INTERFACE_DECL()

            CARAPI GetInterpolation(
                /* [in] */ Float input,
                /* [out] */ Float* res);

            CARAPI HasNativeInterpolator(
                /* [out] */ Boolean* res);
        };

        class TimeInterpolatorInOut
            : public Object
            , public ITimeInterpolator
        {
        public:
            CAR_INTERFACE_DECL()

            CARAPI GetInterpolation(
                /* [in] */ Float input,
                /* [out] */ Float* res);

            CARAPI HasNativeInterpolator(
                /* [out] */ Boolean* res);
        };

    public:
        static AutoPtr<TimeInterpolatorIn> mEaseIn;

        static AutoPtr<TimeInterpolatorOut> mEaseOut;

        static AutoPtr<TimeInterpolatorInOut> mEaseInOut;
    };

public:
    static const Float DOMAIN;
    static const Float DURATION;
    static const Float START;
};

} // namespace Multiwaveview
} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_MULTIWAVEVIEW_EASE_H__
