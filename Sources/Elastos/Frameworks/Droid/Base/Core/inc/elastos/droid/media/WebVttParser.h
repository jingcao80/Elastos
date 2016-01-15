#ifndef __ELASTOS_DROID_MEDIA_WEBVTTPARSER_H__
#define __ELASTOS_DROID_MEDIA_WEBVTTPARSER_H__

#include "Elastos.Droid.Media.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Utility::IVector;

namespace Elastos {
namespace Droid {
namespace Media {

class WebVttParser
    : public Object
    , public IWebVttParser
{
private:
    class SkipRestPhase
        : public Object
        , public IWebVttParserPhase
    {
    public:
        SkipRestPhase(
            /* [in] */ WebVttParser* host);

        ~SkipRestPhase();

        CAR_INTERFACE_DECL()

        CARAPI Parse(
            /* [in] */ const String& line);

    private:
        WebVttParser* mHost;
    };

    class ParseStartPhase
        : public Object
        , public IWebVttParserPhase
    {
    public:
        ParseStartPhase(
            /* [in] */ WebVttParser* host);

        ~ParseStartPhase();

        CAR_INTERFACE_DECL()

        CARAPI Parse(
            /* [in] */ const String& line);

    private:
        WebVttParser* mHost;
    };

    class ParseCueIdPhase
        : public Object
        , public IWebVttParserPhase
    {
    public:
        ParseCueIdPhase(
            /* [in] */ WebVttParser* host);

        ~ParseCueIdPhase();

        CAR_INTERFACE_DECL()

        CARAPI Parse(
            /* [in] */ const String& line);

    private:
        WebVttParser* mHost;
    };

    class ParseCueTimePhase
        : public Object
        , public IWebVttParserPhase
    {
    public:
        ParseCueTimePhase(
            /* [in] */ WebVttParser* host);

        ~ParseCueTimePhase();

        CAR_INTERFACE_DECL()

        CARAPI Parse(
            /* [in] */ const String& line);

    private:
        WebVttParser* mHost;
    };

    class ParseCueTextPhase
        : public Object
        , public IWebVttParserPhase
    {
    public:
        ParseCueTextPhase(
            /* [in] */ WebVttParser* host);

        ~ParseCueTextPhase();

        CAR_INTERFACE_DECL()

        CARAPI Parse(
            /* [in] */ const String& line);

    private:
        WebVttParser* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    WebVttParser();

    virtual ~WebVttParser();

    CARAPI constructor(
        /* [in] */ IWebVttCueListener* listener);

    CARAPI Parse(
        /* [in] */ const String& s);

    CARAPI Eos();

    CARAPI YieldCue();

private:
    CARAPI_(void) Log_warning(
        /* [in] */ const String& nameType,
        /* [in] */ const String& name,
        /* [in] */ const String& message,
        /* [in] */ const String& subMessage,
        /* [in] */ const String& value);

    CARAPI_(void) Log_warning(
        /* [in] */ const String& nameType,
        /* [in] */ const String& name,
        /* [in] */ const String& message,
        /* [in] */ const String& value);

    CARAPI_(void) Log_warning(
        /* [in] */ const String& message,
        /* [in] */ const String& value);

private:
    static const String TAG;
    AutoPtr<IWebVttParserPhase> mPhase;
    AutoPtr<ITextTrackCue> mCue;
    AutoPtr<IVector> mCueTexts;
    AutoPtr<IWebVttCueListener> mListener;
    String mBuffer;
    AutoPtr<IWebVttParserPhase> mSkipRest;
    AutoPtr<IWebVttParserPhase> mParseStart;
    AutoPtr<IWebVttParserPhase> mParseHeader;
    AutoPtr<IWebVttParserPhase> mParseCueId;
    AutoPtr<IWebVttParserPhase> mParseCueTime;
    AutoPtr<IWebVttParserPhase> mParseCueText;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_WEBVTTPARSER_H__