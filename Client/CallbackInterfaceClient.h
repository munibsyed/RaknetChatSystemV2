#pragma once
#include <FileListTransferCBInterface.h>
#include <string>
#include <fstream>

class CallbackInterfaceClient : public RakNet::FileListTransferCBInterface
{
public:
	
	bool OnFile(OnFileStruct *onFileStruct);

	void OnFileProgress(FileProgressStruct *fps);
};

