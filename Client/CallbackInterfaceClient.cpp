#include "CallbackInterfaceClient.h"

bool CallbackInterfaceClient::OnFile(OnFileStruct * onFileStruct)
{
	std::cout << "Received file on client end." << std::endl;
	std::string fileData = "";
	for (int i = 0; i < onFileStruct->byteLengthOfThisFile; i++)
	{
		fileData += onFileStruct->fileData[i];
	}

	std::string filename = onFileStruct->fileName;
	//filename = "C:\\Users\\munib\\Documents\\" + filename;
	std::cout << "Writing to: " << filename << std::endl;

	std::ofstream outputFile;
	outputFile.open(filename, std::ios::binary);
	outputFile << fileData;
	outputFile.close();
	return false;
}

void CallbackInterfaceClient::OnFileProgress(FileProgressStruct * fps)
{
}