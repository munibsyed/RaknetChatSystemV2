#pragma once
#include <iostream>
#include <FileListTransfer.h>
#include <FileListTransferCBInterface.h>
#include <string>
#include <fstream>

class ChatServer; //circular headers

class CallbackInterface : public RakNet::FileListTransferCBInterface
{
public:
	CallbackInterface();

	bool OnFile(OnFileStruct *onFileStruct);

	void OnFileProgress(FileProgressStruct *fps);

private:
};

