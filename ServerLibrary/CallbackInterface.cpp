#include "CallbackInterface.h"

CallbackInterface::CallbackInterface()
{
}

bool CallbackInterface::OnFile(OnFileStruct * onFileStruct)
{
	
	//cause a crash when return value is true
	//onFileStruct->fileData[onFileStruct->byteLengthOfThisFile] = '\0';
	std::string fileData = "";
	for (int i = 0; i < onFileStruct->byteLengthOfThisFile; i++)
	{
		fileData += onFileStruct->fileData[i];
	}
	std::cout << onFileStruct->byteLengthOfThisFile << std::endl;
	//fileData[onFileStruct->byteLengthOfThisFile] = '\0';
	
	std::ofstream outputFile;
	outputFile.open(onFileStruct->fileName, std::ios::binary);

	outputFile << fileData;
	outputFile.close();

	RakNet::FileListTransfer *flTransfer = new RakNet::FileListTransfer;
	RakNet::FileList *fileList = new RakNet::FileList;
	FileListNodeContext context;
	fileList->AddFile(onFileStruct->fileName, onFileStruct->fileName, fileData.c_str(), fileData.length(), fileData.length(), context);

	//send file

	delete flTransfer;
	delete fileList;

	return false;
}

void CallbackInterface::OnFileProgress(FileProgressStruct * fps)
{
	std::cout << "Receiving file" << std::endl;
}
