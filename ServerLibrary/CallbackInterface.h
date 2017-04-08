#pragma once
#include <iostream>
#include <FileListTransferCBInterface.h>
#include <string>
#include <fstream>

class CallbackInterface : public RakNet::FileListTransferCBInterface
{
public:
	bool OnFile(OnFileStruct *onFileStruct);

	void OnFileProgress(FileProgressStruct *fps);

};

