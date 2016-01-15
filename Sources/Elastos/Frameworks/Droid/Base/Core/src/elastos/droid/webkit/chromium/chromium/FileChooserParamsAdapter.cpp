
namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Chromium {
namespace Chromium {

FileChooserParamsAdapter::FileChooserParamsAdapter(
    /* [in] */ AwContentsClient::FileChooserParams* params,
    /* [in] */ IContext* context)
    : mParams(params)
{
}

AutoPtr< ArrayOf<IUri> > FileChooserParamsAdapter::ParseFileChooserResult(
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* intent)
{
    if (resultCode == IActivity::RESULT_CANCELED) {
        return NULL;
    }

    AutoPtr<IUri> result;
    if (intent == NULL || resultCode != IActivity::RESULT_OK) {
        result = NULL;
    }
    else {
        intent->GetData((IUri**)&result);
    }

    AutoPtr< ArrayOf<IUri> > uris;
    if (result != NULL) {
        uris = ArrayOf<IUri>::Alloc(1);
        (*uris)[0] = result;
    }

    return uris;
}

//@Override
ECode FileChooserParamsAdapter::GetMode(
    /* [out] */ Int32* mode)
{
    VALIDATE_NOT_NULL(mode);
    *mode = mParams->mode;
    return NOERROR;
}

//@Override
ECode FileChooserParamsAdapter::GetAcceptTypes(
    /* [out] */ ArrayOf<String>** acceptTypes)
{
    VALIDATE_NOT_NULL(acceptTypes);
    if (mParams->AcceptTypes == NULL) {
        return new String[0];
        *acceptTypes = ArrayOf<String>::Alloc(0);
        REFCOUNT_ADD(*acceptTypes);
        return NOERROR;
    }

    return mParams.acceptTypes.split(";");
}

//@Override
ECode FileChooserParamsAdapter::IsCaptureEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mParams->capture;
    result NOERROR;
}

//@Override
ECode FileChooserParamsAdapter::GetTitle(
    /* [out] */ ICharSequence** title)
{
    VALIDATE_NOT_NULL(title);
    *result = mParams->title;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//@Override
ECode FileChooserParamsAdapter::GetFilenameHint(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mParams->defaultFilename;
    return NOERROR;
}

//@Override
ECode FileChooserParamsAdapter::CreateIntent(
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent);

    // TODO: Move this code to Aw. Once code is moved
    // and merged to M37 get rid of this.
    String mimeType("*/*");
    if (mParams->acceptTypes != NULL && !mParams->acceptTypes.Trim().IsEmpty()) {
        mimeType = mParams->acceptTypes.Split(";")[0];
    }

    AutoPtr<IIntent> i;
    CIntent::New(IIntent::ACTION_GET_CONTENT, (IIntent**)&i);
    i->AddCategory(IIntent::CATEGORY_OPENABLE);
    i->SetType(mimeType);

    *intent = i;
    REFCOUNT_ADD(*intent);

    return NOERROR;
}

} // namespace Chromium
} // namespace Chromium
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
