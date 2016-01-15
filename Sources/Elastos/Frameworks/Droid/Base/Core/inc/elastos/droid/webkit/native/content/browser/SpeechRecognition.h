// wuweizuo automatic build .h file from .java file.
// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_SPEECHRECOGNITION_H_
#define _ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_SPEECHRECOGNITION_H_

#include "Elastos.Droid.Speech.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

// package org.chromium.content.browser;
// import android.content.ComponentName;
// import android.content.Context;
// import android.content.Intent;
// import android.content.pm.PackageManager;
// import android.content.pm.PackageManager.NameNotFoundException;
// import android.content.pm.ResolveInfo;
// import android.content.pm.ServiceInfo;
// import android.os.Bundle;
// import android.speech.RecognitionListener;
// import android.speech.RecognitionService;
// import android.speech.RecognizerIntent;
// import android.speech.SpeechRecognizer;
// import org.chromium.base.CalledByNative;
// import org.chromium.base.JNINamespace;
// import java.util.ArrayList;
// import java.util.List;

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Speech::IRecognitionListener;
using Elastos::Droid::Speech::IRecognitionService;
using Elastos::Droid::Speech::IRecognizerIntent;
using Elastos::Droid::Speech::ISpeechRecognizer;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {

/**
  * This class uses Android's SpeechRecognizer to perform speech recognition for the Web Speech API
  * on Android. Using Android's platform recognizer offers several benefits, like good quality and
  * good local fallback when no data connection is available.
  */
// @JNINamespace("content")
class SpeechRecognition
    : public Object
{
public:
    // Internal class to handle events from Android's SpeechRecognizer and route them to native.
    class Listener
        : public Object
        , public IRecognitionListener
    {
    public:
        Listener(
            /* [in] */ SpeechRecognition* owner);

        CAR_INTERFACE_DECL();

        // @Override
        CARAPI OnBeginningOfSpeech();

        // @Override
        CARAPI OnBufferReceived(
            /* [in] */ ArrayOf<Byte>* buffer);

        // @Override
        CARAPI OnEndOfSpeech();

        // @Override
        CARAPI OnError(
            /* [in] */ Int32 error);

        // @Override
        CARAPI OnEvent(
            /* [in] */ Int32 event,
            /* [in] */ IBundle* bundle);

        // @Override
        CARAPI OnPartialResults(
            /* [in] */ IBundle* bundle);

        // @Override
        CARAPI OnReadyForSpeech(
            /* [in] */ IBundle* bundle);

        // @Override
        CARAPI OnResults(
            /* [in] */ IBundle* bundle);

        // @Override
        CARAPI OnRmsChanged(
            /* [in] */ Float rms);

    private:
        CARAPI HandleResults(
            /* [in] */ IBundle* bundle,
            /* [in] */ Boolean provisional);

    private:
        SpeechRecognition* mOwner;
    };

public:
    /**
      * This method must be called before any instance of SpeechRecognition can be created. It will
      * query Android's package manager to find a suitable speech recognition provider that supports
      * continuous recognition.
      */
    static CARAPI_(Boolean) Initialize(
        /* [in] */ IContext* context);

    static CARAPI_(void*) ElaSpeechRecognitionCallback_Init();

private:
    static CARAPI_(AutoPtr<IInterface>) CreateSpeechRecognition(
        /* [in] */ IInterface* context,
        /* [in] */ Int64 nativeSpeechRecognizerImplAndroid);

    static CARAPI_(void) StartRecognition(
        /* [in] */ IInterface* obj,
        /* [in] */ const String& language,
        /* [in] */ Boolean continuous,
        /* [in] */ Boolean interimResults);

    static CARAPI_(void) AbortRecognition(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) StopRecognition(
        /* [in] */ IInterface* obj);

    SpeechRecognition(
        /* [in] */ IContext* context,
        /* [in] */ Int64 nativeSpeechRecognizerImplAndroid);

    // This function destroys everything when recognition is done, taking care to properly tear
    // down by calling On{Sound,Audio}End if corresponding On{Audio,Sound}Start were called.
    CARAPI Terminate(
        /* [in] */ Int32 error);

    // @CalledByNative
    static CARAPI_(AutoPtr<SpeechRecognition>) CreateSpeechRecognition(
        /* [in] */ IContext* context,
        /* [in] */ Int64 nativeSpeechRecognizerImplAndroid);

    // @CalledByNative
    CARAPI StartRecognition(
        /* [in] */ const String& language,
        /* [in] */ Boolean continuous,
        /* [in] */ Boolean interimResults);

    // @CalledByNative
    CARAPI AbortRecognition();

    // @CalledByNative
    CARAPI StopRecognition();

    // Native JNI calls to content/browser/speech/speech_recognizer_impl_android.cc
    CARAPI NativeOnAudioStart(
        /* [in] */ Int64 nativeSpeechRecognizerImplAndroid);

    CARAPI NativeOnSoundStart(
        /* [in] */ Int64 nativeSpeechRecognizerImplAndroid);

    CARAPI NativeOnSoundEnd(
        /* [in] */ Int64 nativeSpeechRecognizerImplAndroid);

    CARAPI NativeOnAudioEnd(
        /* [in] */ Int64 nativeSpeechRecognizerImplAndroid);

    CARAPI NativeOnRecognitionResults(
        /* [in] */ Int64 nativeSpeechRecognizerImplAndroid,
        /* [in] */ ArrayOf<String>* results,
        /* [in] */ ArrayOf<Float>* scores,
        /* [in] */ Boolean provisional);

    CARAPI NativeOnRecognitionError(
        /* [in] */ Int64 nativeSpeechRecognizerImplAndroid,
        /* [in] */ Int32 error);

    CARAPI NativeOnRecognitionEnd(
        /* [in] */ Int64 nativeSpeechRecognizerImplAndroid);

private:
    // Constants describing the speech recognition provider we depend on.
    static const String PROVIDER_PACKAGE_NAME;
    static const Int32 PROVIDER_MIN_VERSION = 300207030;
    // We track the recognition state to remember what events we need to send when recognition is
    // being aborted. Once Android's recognizer is cancelled, its listener won't yield any more
    // events, but we still need to call OnSoundEnd and OnAudioEnd if corresponding On*Start were
    // called before.
    static const Int32 STATE_IDLE = 0;
    static const Int32 STATE_AWAITING_SPEECH = 1;
    static const Int32 STATE_CAPTURING_SPEECH = 2;
    Int32 mState;
    // The speech recognition provider (if any) matching PROVIDER_PACKAGE_NAME and
    // PROVIDER_MIN_VERSION as selected by initialize().
    static AutoPtr<IComponentName> sRecognitionProvider;
    /*const*/ AutoPtr<IContext> mContext;
    /*const*/ AutoPtr<IIntent> mIntent;
    /*const*/ AutoPtr<IRecognitionListener> mListener;
    AutoPtr<ISpeechRecognizer> mRecognizer;
    // Native pointer to C++ SpeechRecognizerImplAndroid.
    Int64 mNativeSpeechRecognizerImplAndroid;
    // Remember if we are using continuous recognition.
    Boolean mContinuous;
};

} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_SPEECHRECOGNITION_H_
