#pragma once
#include <FileListTransferCBInterface.h>
#include <string>
#include <fstream>
#include <iostream>

class CallbackInterfaceClient : public RakNet::FileListTransferCBInterface
{
public:
	
	bool OnFile(OnFileStruct *onFileStruct);

	void OnFileProgress(FileProgressStruct *fps);
};

