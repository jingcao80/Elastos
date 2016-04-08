
#include "CMyBackupAgent.h"
#include <elastos/utility/logging/Slogger.h>
#include "CActivityOne.h"

using Elastos::Utility::Logging::Slogger;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::CByteArrayOutputStream;
using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::CByteArrayInputStream;
using Elastos::IO::IDataInput;
using Elastos::IO::IDataOutputStream;
using Elastos::IO::CDataOutputStream;
using Elastos::IO::IDataInputStream;
using Elastos::IO::CDataInputStream;
using Elastos::IO::IDataOutput;
using Elastos::IO::CFile;
using Elastos::IO::CFileInputStream;
using Elastos::IO::CFileOutputStream;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace BackupManagerDemo {

const String CMyBackupAgent::TAG("CMyBackupAgent");
const Int32 CMyBackupAgent::AGENT_VERSION = 1;
const String CMyBackupAgent::APP_DATA_KEY("alldata");

ECode CMyBackupAgent::OnCreate()
{
    AutoPtr<IFile> fileDir;
    GetFilesDir((IFile**)&fileDir);
    CFile::New(fileDir, CActivityOne::DATA_FILE_NAME, (IFile**)&mDataFile);
}

ECode CMyBackupAgent::OnBackup(
    /* [in] */ IParcelFileDescriptor* oldState,
    /* [in] */ IBackupDataOutput* data,
    /* [in] */ IParcelFileDescriptor* newState)
{
    Slogger::D(TAG, "OnBackup");
    {
        AutoLock lock(CActivityOne::sDatalock);
        AutoPtr<IRandomAccessFile> file;
        CRandomAccessFile::New(mDataFile, String("r"), (IRandomAccessFile**)&file);
        AutoPtr<IDataInput> input = IDataInput::Probe(file);
        assert(input != NULL);
        input->ReadInt32(&mFilling);
        input->ReadBoolean(&mAddMayo);
        input->ReadBoolean(&mAddTomato);
    }


    Boolean doBackup = (oldState == NULL);
    if (!doBackup) {
        doBackup = CompareStateFile(oldState);
    }

    Slogger::D(TAG, "OnBackup::doBackup[%d] filling[%d] mAddMayo[%d], mAddTomato[%d]", doBackup, mFilling, mAddMayo, mAddTomato);
    //need backup
    if (doBackup) {
        AutoPtr<IByteArrayOutputStream> bufStream;
        CByteArrayOutputStream::New((IByteArrayOutputStream**)&bufStream);
        AutoPtr<IDataOutputStream> outWriter;
        CDataOutputStream::New(bufStream, (IDataOutputStream**)&outWriter);

        AutoPtr<IDataOutput> output = IDataOutput::Probe(outWriter);
        assert(output != NULL);
        output->WriteInt32(mFilling);
        output->WriteBoolean(mAddMayo);
        output->WriteBoolean(mAddTomato);

        AutoPtr<ArrayOf<Byte> > bytes;
        bufStream->ToByteArray((ArrayOf<Byte>**)&bytes);
        Int32 len = bytes->GetLength();
        Int32 writtenSize = 0;
        data->WriteEntityHeader(APP_DATA_KEY, len, &writtenSize);
        data->WriteEntityData(bytes, len, &writtenSize);
    }

    // Finally, in all cases, we need to write the new state blob
    WriteStateFile(newState);
    return NOERROR;
}

ECode CMyBackupAgent::OnRestore(
    /* [in] */ IBackupDataInput* data,
    /* [in] */ Int32 appVersionCode,
    /* [in] */ IParcelFileDescriptor* newState)
{
    Boolean bNext = FALSE;
    while(data->ReadNextHeader(&bNext), bNext) {
        String key;
        data->GetKey(&key);
        Int32 dataSize;
        data->GetDataSize(&dataSize);

        Slogger::V(TAG, "got key=%s, datasize=%d", key.string(), dataSize);

        if (APP_DATA_KEY.Equals(key)) {
            AutoPtr<ArrayOf<Byte> > dataBuf = ArrayOf<Byte>::Alloc(dataSize);
            Int32 readSize;
            data->ReadEntityData((ArrayOf<Byte>*)dataBuf, 0, dataSize, &readSize);
            AutoPtr<IByteArrayInputStream> inputBuffer;
            CByteArrayInputStream::New(dataBuf, (IByteArrayInputStream**)&inputBuffer);
            AutoPtr<IDataInputStream> intputBufferStream;
            CDataInputStream::New(inputBuffer, (IDataInputStream**)&intputBufferStream);

            AutoPtr<IDataInput> in = IDataInput::Probe(intputBufferStream);
            in->ReadInt32(&mFilling);
            in->ReadBoolean(&mAddMayo);
            in->ReadBoolean(&mAddTomato);

            {
                AutoLock lock(CActivityOne::sDatalock);
                AutoPtr<IRandomAccessFile> file;
                CRandomAccessFile::New(mDataFile, String("rw"), (IRandomAccessFile**)&file);
                file->SetLength(0);
                AutoPtr<IDataOutput> dataOutput = IDataOutput::Probe(file);
                assert(dataOutput != NULL);
                dataOutput->WriteInt32(mFilling);
                dataOutput->WriteBoolean(mAddMayo);
                dataOutput->WriteBoolean(mAddTomato);
                Slogger::V(TAG, "NEW State: mayo=%d, tomato = %d, filling=%d", mAddMayo, mAddTomato, mFilling);
            }

        }
        else {
            Slogger::E(TAG, "Hominlinx-->key not Equals APP_DATA_KEY");
            data->SkipEntityData();
        }
    }
    WriteStateFile(newState);
    return NOERROR;
}


Boolean CMyBackupAgent::CompareStateFile(
 /* [in] */ IParcelFileDescriptor* oldState)
{
    AutoPtr<IFileDescriptor> fd;
    oldState->GetFileDescriptor((IFileDescriptor**)&fd);
    AutoPtr<IFileInputStream> instream;
    CFileInputStream::New(fd, (IFileInputStream**)&instream);

    AutoPtr<IDataInputStream> indata;
    CDataInputStream::New(instream, (IDataInputStream**)&indata);
    AutoPtr<IDataInput> in = IDataInput::Probe(indata);
    assert(in != NULL);

    Int32 stateVersion;
    in->ReadInt32(&stateVersion);
    if (stateVersion > AGENT_VERSION) {
        //whoops
        return TRUE;
    }

    Int32 lastFilling;
    Boolean lastMayo;
    Boolean lastTomato;

    in->ReadInt32(&lastFilling);
    in->ReadBoolean(&lastMayo);
    in->ReadBoolean(&lastTomato);
    return (lastFilling != mFilling)
        || (lastTomato != mAddTomato)
        || (lastMayo != mAddMayo);

}

void CMyBackupAgent::WriteStateFile(
 /* [in] */ IParcelFileDescriptor* stateFile)
{
    AutoPtr<IFileDescriptor> fd;
    stateFile->GetFileDescriptor((IFileDescriptor**)&fd);

    AutoPtr<IFileOutputStream> outstream;
    CFileOutputStream::New(fd, (IFileOutputStream**)&outstream);
    AutoPtr<IDataOutputStream> outdata;
    CDataOutputStream::New(outstream, (IDataOutputStream**)&outdata);

    AutoPtr<IDataOutput> out = IDataOutput::Probe(outdata);
    assert(out != NULL);

    out->WriteInt32(AGENT_VERSION);
    out->WriteInt32(mFilling);
    out->WriteBoolean(mAddMayo);
    out->WriteBoolean(mAddTomato);
}

}
}
}
}
