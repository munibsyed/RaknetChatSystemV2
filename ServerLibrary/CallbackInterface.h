#pragma once
#include <iostream>
#include <FileListTransfer.h>
#include <FileListTransferCBInterface.h>
#include <string>
#include <fstream>


class CallbackInterface : public RakNet::FileListTransferCBInterface
{
public:
	CallbackInterface();

	bool OnFile(OnFileStruct *onFileStruct);

	void OnFileProgress(FileProgressStruct *fps);

	RakNet::FileList* GetFileList();

private:
	std::string m_receivedFileData;
	std::string m_receivedFilePath;
};

