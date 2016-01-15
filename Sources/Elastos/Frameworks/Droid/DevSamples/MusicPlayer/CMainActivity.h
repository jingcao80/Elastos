/**************************************************************************************
 * filename:        CMainActivity.h
 * description:     Declare functions which will be implemented in MainActivity.cpp
 * author:          kari.zhang
 *
 * modifications:
 *  1. Implement IOnPlayCompletionListener
 *     kari.zhang @ 2014-07-11
 *
 *  2. Use IView instead of IButton & Use Layout instead of play mode buttons.
 *     kari.zhang @ 2014-07-17
 *
 **************************************************************************************/

#ifndef __CMAINACTIVITY_H__
#define __CMAINACTIVITY_H__

#include "_CMainActivity.h"
#include "elastos/droid/app/Activity.h"

using Elastos::Core::IRunnable;
using Elastos::Droid::App::Activity;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::ICompoundButtonOnCheckedChangeListener;
using Elastos::Droid::Widget::IImageButton;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::IRadioButton;
using Elastos::Droid::Widget::ISeekBar;
using Elastos::Droid::Widget::ISeekBarOnSeekBarChangeListener;
using Elastos::Droid::Widget::ITextView;

namespace MusicPlayer {

class CMainActivity
    : public Activity
    , public IServiceConnection
{
public:
    class MyListener
        : public ElRefBase
        , public IViewOnClickListener
    {
    public:
        MyListener(
            /* [in] */ CMainActivity* host);

        CAR_INTERFACE_DECL()

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CMainActivity* mActivity;
    };

    // ICompoundButtonOnCheckedChangeListener for 4 tab RadioButtons
    class TabChangedListener
        : public ElRefBase
        , public ICompoundButtonOnCheckedChangeListener
    {
    public:
        TabChangedListener(
            /* [in] */ CMainActivity* host);

        CAR_INTERFACE_DECL()

        CARAPI OnCheckedChanged(
            /* [in] */ ICompoundButton* btn,
            /* [in] */ Boolean isChecked);

    private:
        CMainActivity* mActivity;
    };

    // seekBar listener for music and volume
    class SeekBarChangedListener
        : public ElRefBase
        , public ISeekBarOnSeekBarChangeListener
    {
    public:
        SeekBarChangedListener(
            /* [in] */ CMainActivity* activity);

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
        CMainActivity* mActivity;
    };

    class ItemOnClickListener
        : public ElRefBase
        , public IAdapterViewOnItemClickListener
    {
    public:
        ItemOnClickListener(
            /* [in] */ CMainActivity* host);

        CAR_INTERFACE_DECL()

        CARAPI OnItemClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);

    private:
        CMainActivity* mActivity;
    };

public:
    CMainActivity();

    ~CMainActivity();

    CAR_INTERFACE_DECL()

    CARAPI GetClassID(
        /* [out] */ ClassID* clsid);

    CARAPI OnServiceConnected(
        /* [in] */ IComponentName* name,
        /* [in] */ IBinder* service);

    CARAPI OnServiceDisconnected(
        /* [in] */ IComponentName* name);

    ECode OnTick(
        /* [in] */ Int32 duration);

    ECode OnCompletion();

    ECode Run();

protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnResume();

    CARAPI OnStop();

    CARAPI OnDestroy();

private:
    ECode InitView();

    ECode InitStatus();

    ECode SetCurrentAudioInfo();

    ECode SetupListener();

    ECode SetPlayBtnStatus(
        /* [in] */ Boolean isPlaying);

    ECode SetPlayModeStatus(
        /* [in] */ Int32 mode);

private:
    Boolean mIsSeekBarMoving;

    AutoPtr<IPlayController> mPlayController;
    AutoPtr<IOnTickListener> mOnTickListner;
    AutoPtr<IOnPlayCompletionListener> mOnPlayCompletionListener;

    AutoPtr<IView> mBtnAbout;
    AutoPtr<IView> mBtnSearch;

    AutoPtr<IRadioButton> mBtnTabSong;
    AutoPtr<IRadioButton> mBtnTabSinger;
    AutoPtr<IRadioButton> mBtnTabAlbum;
    AutoPtr<IRadioButton> mBtnTabPlayList;

    AutoPtr<IView> mLayoutTabRight;
    AutoPtr<IView> mBtnEdit;
    AutoPtr<IView> mBtnAddPlaylist;

    AutoPtr<IView> mLayoutPlayMode;
    AutoPtr<IView> mLayoutAllLoop;
   // AutoPtr<IView> mBtnAllLoop;
    AutoPtr<IView> mLayoutSingleLoop;
   // AutoPtr<IView> mBtnSingleLoop;
    AutoPtr<IView> mLayoutRandomPlay;
  //  AutoPtr<IView> mBtnRandomPlay;
    AutoPtr<IView> mLayoutOrderPlay;
  //  AutoPtr<IView> mBtnOrderPlay;

    AutoPtr<IButton> mBtnPlay;
    AutoPtr<IButton> mBtnPause;

    AutoPtr<ArrayOf<IInterface*> > mDataList;
    AutoPtr<IArrayAdapter> mAdapter;
    AutoPtr<IListView> mContentListView;

    AutoPtr<ISeekBar> mSeekBarMusic;
    AutoPtr<SeekBarChangedListener> mSeekBarListener;
    AutoPtr<IView> mLayoutFoot;
    AutoPtr<IImageView> mImgViewAlbumArt;
    AutoPtr<ITextView> mTextViewSong;
    AutoPtr<ITextView> mTextViewArtist;

    AutoPtr<MyListener> mActionListener;
    AutoPtr<TabChangedListener> mTabListener;
    AutoPtr<ItemOnClickListener> mItemListener;
};

} // namespace MusicPlayer

#endif // __CMAINACTIVITY_H__
