//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

/*
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.settings.users;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::Content::IClipData;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IBitmapFactory;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::Bitmap::IConfig;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IAsyncTask;
using Elastos::Droid::Provider::IMediaStore;
using Elastos::Droid::Provider::ContactsContract::IDisplayPhoto;
using Elastos::Droid::Support::V4::Content::IFileProvider;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::View::IOnClickListener;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::IListAdapter;
using Elastos::Droid::Widget::IListPopupWindow;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::Drawable::ICircleFramedDrawable;

using Elastos::IO::IFile;
using Elastos::IO::IInputStream;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

public class EditUserPhotoController {
    private static const String TAG = "EditUserPhotoController";

    private static const Int32 POPUP_LIST_ITEM_ID_CHOOSE_PHOTO = 1;
    private static const Int32 POPUP_LIST_ITEM_ID_TAKE_PHOTO = 2;

    // It seems that this class generates custom request codes and they may
    // collide with ours, these values are very unlikely to have a conflict.
    private static const Int32 REQUEST_CODE_CHOOSE_PHOTO = 1001;
    private static const Int32 REQUEST_CODE_TAKE_PHOTO   = 1002;
    private static const Int32 REQUEST_CODE_CROP_PHOTO   = 1003;

    private static const String CROP_PICTURE_FILE_NAME = "CropEditUserPhoto.jpg";
    private static const String TAKE_PICTURE_FILE_NAME = "TakeEditUserPhoto2.jpg";

    private final Int32 mPhotoSize;

    private final Context mContext;
    private final Fragment mFragment;
    private final ImageView mImageView;

    private final Uri mCropPictureUri;
    private final Uri mTakePictureUri;

    private Bitmap mNewUserPhotoBitmap;
    private Drawable mNewUserPhotoDrawable;

    public EditUserPhotoController(Fragment fragment, ImageView view,
            Bitmap bitmap, Drawable drawable, Boolean waiting) {
        mContext = view->GetContext();
        mFragment = fragment;
        mImageView = view;
        mCropPictureUri = CreateTempImageUri(mContext, CROP_PICTURE_FILE_NAME, !waiting);
        mTakePictureUri = CreateTempImageUri(mContext, TAKE_PICTURE_FILE_NAME, !waiting);
        mPhotoSize = GetPhotoSize(mContext);
        mImageView->SetOnClickListener(new OnClickListener() {
            //@Override
            CARAPI OnClick(View v) {
                ShowUpdatePhotoPopup();
            }
        });
        mNewUserPhotoBitmap = bitmap;
        mNewUserPhotoDrawable = drawable;
    }

    public Boolean OnActivityResult(Int32 requestCode, Int32 resultCode, Intent data) {
        if (resultCode != Activity.RESULT_OK) {
            return FALSE;
        }
        final Uri pictureUri = data != NULL && data->GetData() != NULL
                ? data->GetData() : mTakePictureUri;
        switch (requestCode) {
            case REQUEST_CODE_CROP_PHOTO:
                OnPhotoCropped(pictureUri, TRUE);
                return TRUE;
            case REQUEST_CODE_TAKE_PHOTO:
            case REQUEST_CODE_CHOOSE_PHOTO:
                CropPhoto(pictureUri);
                return TRUE;
        }
        return FALSE;
    }

    public Bitmap GetNewUserPhotoBitmap() {
        return mNewUserPhotoBitmap;
    }

    public Drawable GetNewUserPhotoDrawable() {
        return mNewUserPhotoDrawable;
    }

    private void ShowUpdatePhotoPopup() {
        final Boolean canTakePhoto = CanTakePhoto();
        final Boolean canChoosePhoto = CanChoosePhoto();

        if (!canTakePhoto && !canChoosePhoto) {
            return;
        }

        Context context = mImageView->GetContext();
        final List<EditUserPhotoController.AdapterItem> items = new ArrayList<EditUserPhotoController.AdapterItem>();

        if (CanTakePhoto()) {
            String title = mImageView->GetContext()->GetString( R::string::user_image_take_photo);
            EditUserPhotoController.AdapterItem item = new AdapterItem(title, POPUP_LIST_ITEM_ID_TAKE_PHOTO);
            items->Add(item);
        }

        if (canChoosePhoto) {
            String title = context->GetString(R::string::user_image_choose_photo);
            EditUserPhotoController.AdapterItem item = new AdapterItem(title, POPUP_LIST_ITEM_ID_CHOOSE_PHOTO);
            items->Add(item);
        }

        final ListPopupWindow listPopupWindow = new ListPopupWindow(context);

        listPopupWindow->SetAnchorView(mImageView);
        listPopupWindow->SetModal(TRUE);
        listPopupWindow->SetInputMethodMode(ListPopupWindow.INPUT_METHOD_NOT_NEEDED);

        ListAdapter adapter = new ArrayAdapter<EditUserPhotoController.AdapterItem>(context,
                R.layout.edit_user_photo_popup_item, items);
        listPopupWindow->SetAdapter(adapter);

        final Int32 width = Math->Max(mImageView->GetWidth(), context->GetResources()
                .GetDimensionPixelSize(R.dimen.update_user_photo_popup_min_width));
        listPopupWindow->SetWidth(width);

        listPopupWindow->SetOnItemClickListener(new AdapterView->OnItemClickListener() {
            //@Override
            CARAPI OnItemClick(AdapterView<?> parent, View view, Int32 position, Int64 id) {
                EditUserPhotoController.AdapterItem item = items->Get(position);
                switch (item.id) {
                    case POPUP_LIST_ITEM_ID_CHOOSE_PHOTO: {
                        ChoosePhoto();
                        listPopupWindow->Dismiss();
                    } break;
                    case POPUP_LIST_ITEM_ID_TAKE_PHOTO: {
                        TakePhoto();
                        listPopupWindow->Dismiss();
                    } break;
                }
            }
        });

        listPopupWindow->Show();
    }

    private Boolean CanTakePhoto() {
        return mImageView->GetContext()->GetPackageManager().QueryIntentActivities(
                new Intent(MediaStore.ACTION_IMAGE_CAPTURE),
                PackageManager.MATCH_DEFAULT_ONLY).Size() > 0;
    }

    private Boolean CanChoosePhoto() {
        Intent intent = new Intent(IIntent::ACTION_GET_CONTENT);
        intent->SetType("image/*");
        return mImageView->GetContext()->GetPackageManager().QueryIntentActivities(
                intent, 0).Size() > 0;
    }

    private void TakePhoto() {
        Intent intent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);
        AppendOutputExtra(intent, mTakePictureUri);
        mFragment->StartActivityForResult(intent, REQUEST_CODE_TAKE_PHOTO);
    }

    private void ChoosePhoto() {
        Intent intent = new Intent(IIntent::ACTION_GET_CONTENT, NULL);
        intent->SetType("image/*");
        AppendOutputExtra(intent, mTakePictureUri);
        mFragment->StartActivityForResult(intent, REQUEST_CODE_CHOOSE_PHOTO);
    }

    private void CropPhoto(Uri pictureUri) {
        // TODO: Use a public intent, when there is one.
        Intent intent = new Intent("com.android.camera.action.CROP");
        intent->SetDataAndType(pictureUri, "image/*");
        AppendOutputExtra(intent, mCropPictureUri);
        AppendCropExtras(intent);
        if (intent->ResolveActivity(mContext->GetPackageManager()) != NULL) {
            mFragment->StartActivityForResult(intent, REQUEST_CODE_CROP_PHOTO);
        } else {
            OnPhotoCropped(pictureUri, FALSE);
        }
    }

    private void AppendOutputExtra(Intent intent, Uri pictureUri) {
        intent->PutExtra(MediaStore.EXTRA_OUTPUT, pictureUri);
        intent->AddFlags(IIntent::FLAG_GRANT_WRITE_URI_PERMISSION
                | IIntent::FLAG_GRANT_READ_URI_PERMISSION);
        intent->SetClipData(ClipData->NewRawUri(MediaStore.EXTRA_OUTPUT, pictureUri));
    }

    private void AppendCropExtras(Intent intent) {
        intent->PutExtra("crop", "TRUE");
        intent->PutBooleanExtra("scale", TRUE);
        intent->PutBooleanExtra("scaleUpIfNeeded", TRUE);
        intent->PutExtra("aspectX", 1);
        intent->PutExtra("aspectY", 1);
        intent->PutExtra("outputX", mPhotoSize);
        intent->PutExtra("outputY", mPhotoSize);
    }

    private void OnPhotoCropped(final Uri data, final Boolean cropped) {
        new AsyncTask<Void, Void, Bitmap>() {
            //@Override
            protected Bitmap DoInBackground(Void... params) {
                if (cropped) {
                    InputStream imageStream = NULL;
                    try {
                        imageStream = mContext->GetContentResolver()
                                .OpenInputStream(data);
                        return BitmapFactory->DecodeStream(imageStream);
                    } catch (FileNotFoundException fe) {
                        Logger::W(TAG, "Cannot find image file", fe);
                        return NULL;
                    } finally {
                        if (imageStream != NULL) {
                            try {
                                imageStream->Close();
                            } catch (IOException ioe) {
                                Logger::W(TAG, "Cannot close image stream", ioe);
                            }
                        }
                    }
                } else {
                    // Scale and crop to a square aspect ratio
                    Bitmap croppedImage = Bitmap->CreateBitmap(mPhotoSize, mPhotoSize,
                            Config.ARGB_8888);
                    Canvas canvas = new Canvas(croppedImage);
                    Bitmap fullImage = NULL;
                    try {
                        InputStream imageStream = mContext->GetContentResolver()
                                .OpenInputStream(data);
                        fullImage = BitmapFactory->DecodeStream(imageStream);
                    } catch (FileNotFoundException fe) {
                        return NULL;
                    }
                    if (fullImage != NULL) {
                        final Int32 squareSize = Math->Min(fullImage->GetWidth(),
                                fullImage->GetHeight());
                        final Int32 left = (fullImage->GetWidth() - squareSize) / 2;
                        final Int32 top = (fullImage->GetHeight() - squareSize) / 2;
                        Rect rectSource = new Rect(left, top,
                                left + squareSize, top + squareSize);
                        Rect rectDest = new Rect(0, 0, mPhotoSize, mPhotoSize);
                        Paint paint = new Paint();
                        canvas->DrawBitmap(fullImage, rectSource, rectDest, paint);
                        return croppedImage;
                    } else {
                        // Bah! Got nothin.
                        return NULL;
                    }
                }
            }

            //@Override
            protected void OnPostExecute(Bitmap bitmap) {
                if (bitmap != NULL) {
                    mNewUserPhotoBitmap = bitmap;
                    mNewUserPhotoDrawable = CircleFramedDrawable
                            .GetInstance(mImageView->GetContext(), mNewUserPhotoBitmap);
                    mImageView->SetImageDrawable(mNewUserPhotoDrawable);
                }
                new File(mContext->GetCacheDir(), TAKE_PICTURE_FILE_NAME).Delete();
                new File(mContext->GetCacheDir(), CROP_PICTURE_FILE_NAME).Delete();
            }
        }.ExecuteOnExecutor(AsyncTask.THREAD_POOL_EXECUTOR, (Void[]) NULL);
    }

    private static Int32 GetPhotoSize(Context context) {
        Cursor cursor = context->GetContentResolver()->Query(
                DisplayPhoto.CONTENT_MAX_DIMENSIONS_URI,
                new String[]{DisplayPhoto.DISPLAY_MAX_DIM}, NULL, NULL, NULL);
        try {
            cursor->MoveToFirst();
            return cursor->GetInt(0);
        } finally {
            cursor->Close();
        }
    }

    private Uri CreateTempImageUri(Context context, String fileName, Boolean purge) {
        final File folder = context->GetCacheDir();
        folder->Mkdirs();
        final File fullPath = new File(folder, fileName);
        if (purge) {
            fullPath->Delete();
        }
        final Uri fileUri =
                FileProvider->GetUriForFile(context, RestrictedProfileSettings.FILE_PROVIDER_AUTHORITY, fullPath);
        return fileUri;
    }

    private static const class AdapterItem {
        final String title;
        final Int32 id;

        public AdapterItem(String title, Int32 id) {
            this.title = title;
            this.id = id;
        }

        //@Override
        CARAPI ToString(
        /* [out] */ String* str)
    {
            return title;
        }
    }
}