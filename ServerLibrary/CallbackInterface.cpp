#include "CallbackInterface.h"

bool CallbackInterface::OnFile(OnFileStruct * onFileStruct)
{
	std::cout << onFileStruct->fileData << std::endl;
	return true;
}

void CallbackInterface::OnFileProgress(FileProgressStruct * fps)
{
	
}
