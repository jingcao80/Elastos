/************************************************************************************
 * filename:        CPlayActivity.h
 * description:     Declare functions which will be implemented in CPlayActivity.cpp
 * author:          kari.zhang
 *
 * modifications:
 *  1. Remove class named OnPlayCompletedListener
 *     kari.zhang @ 2014-07-11
 *
 ***********************************************************************************/

#ifndef __CPlayACTIVITY_H__
#define __CPlayACTIVITY_H__

#include "_CPlayActivity.h"
#include "elastos/droid/app/Activity.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::Media::IMediaPlayer;
using Elastos::Droid::Media::IMediaPlayerOnCompletionListener;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::IRadioButton;
using Elastos::Droid::Widget::ICompoundButtonOnCheckedChangeListener;
using Elastos::Droid::Widget::ISeekBar;
using Elastos::Droid::Widget::ISeekBarOnSeekBarChangeListener;

namespace MusicPlayer {

class CPlayActivity
    : public Activity
    , public IOnPlayCompletionListener
    , public IOnTickListener
    , public IServiceConnection
{
public:
    // IViewOnClickListener for Buttons or ImageViews
    class MyViewOnClickListener
        : public ElRefBase
        , public IViewOnClickListener
    {
    public:
        MyViewOnClickListener(
            /* [in] */ CPlayActivity* host);

        CAR_INTERFACE_DECL()

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CPlayActivity* mActivity;
    };

    // seekBar listener for music and volume
    class SeekBarChangedListener
        : public ElRefBase
        , public ISeekBarOnSeekBarChangeListener
    {
    public:
        SeekBarChangedListener(
            /* [in] */ CPlayActivity* activity);

        CAR_INTERFACE_DECL()

        ECode OnProgressChanged(
            /* [in] */ ISeekBar* seekBar,
            /* [in] */ Int32 progress,
            /* [in] */ Boolean fromUser);

        ECode OnStartTrackingTouch(
            /* [in] */ ISeekBar* seekBar);

        ECode OnStopTrackingTouch(
            /* [in] */ ISeekBar* seekBar);

    private:
        CPlayActivity* mActivity;
    };

    // ICompoundButtonOnCheckedChangeListener for 2 RadioButtons
    class RadioButtonCheckedChangeListener
        : public ElRefBase
        , public ICompoundButtonOnCheckedChangeListener
    {
    public:
        RadioButtonCheckedChangeListener(
            /* [in] */ CPlayActivity* host);

        CAR_INTERFACE_DECL()

        CARAPI OnCheckedChanged(
            /* [in] */ ICompoundButton* btn,
            /* [in] */ Boolean isChecked);

    private:
        CPlayActivity* mActivity;
    };

public:
    CPlayActivity();
    ~CPlayActivity();

    CAR_INTERFACE_DECL()

    CARAPI GetClassID(
        /* [out] */ ClassID* clsid);

    CARAPI OnServiceConnected(
        /* [in] */IComponentName* name,
        /* [in] */IBinder* service);

    CARAPI OnServiceDisconnected(
        /* [in] */ IComponentName* name);

    ECode OnTick(
        /* [in] */ Int32 duration);

    ECode OnCompletion();

protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnStart();

    CARAPI OnResume();

    CARAPI OnStop();

    CARAPI OnDestroy();

    ECode InitViews();

    ECode SetupListeners();

    ECode SetPlayButtonStatus(
        /* [in] */ Boolean isPlaying);

    ECode SetPlayModeStatus(
            /* [in] */ Int32 mode);

    ECode SetDuration(
        /* [in] */ Int32 duration);

    ECode SetProgress(
        /* [in] */ Int32 currentDuration);

    ECode InitStatus();

private:
    // file name
    String mFilePath;

    // listener
    AutoPtr<MyViewOnClickListener> mActionListener;
    AutoPtr<RadioButtonCheckedChangeListener> mSwitchListener;

    // title back
    AutoPtr<IView> mViewBack;

    // play & pause button
    AutoPtr<IView> mBtnPlay;
    AutoPtr<IView> mBtnPause;

    // front & next button
    AutoPtr<IView> mBtnPrevious;
    AutoPtr<IView> mBtnNext;

    // more button
    AutoPtr<IView> mBtnMore;

    // play mode button
    AutoPtr<IView> mBtnModeAllLoop;
    AutoPtr<IView> mBtnModeSingleLoop;
    AutoPtr<IView> mBtnModeRandom;
    AutoPtr<IView> mBtnModeOrder;

    AutoPtr<IView> mLayoutVolume;
    AutoPtr<IRadioButton> mBtnSwitchLeft;
    AutoPtr<IRadioButton> mBtnSwitchRight;

    AutoPtr<ITextView> mTextViewTitle;
    AutoPtr<ITextView> mTextViewAlbumAndName;

    // view layout for play list
    AutoPtr<IView> mLayoutPlayList;

    // audio duration & current duration
    AutoPtr<ITextView> mTextViewDuration;
    AutoPtr<ITextView> mTextViewCurrentDuration;

    // music seekBar
    AutoPtr<ISeekBar> mSeekBarMusic;
    AutoPtr<SeekBarChangedListener> mSeekBarListener;

    AutoPtr<IPlayController> mPlayController;
    AutoPtr<IOnTickListener> mOnTickListner;
    AutoPtr<IOnPlayCompletionListener> mOnPlayCompletionListener;

    Boolean mIsSeekBarMoving;
};

} // namespace MusicPlayer

#endif // __CPlayACTIVITY_H__
