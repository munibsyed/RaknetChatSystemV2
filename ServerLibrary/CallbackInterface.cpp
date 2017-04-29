#include "CallbackInterface.h"

CallbackInterface::CallbackInterface()
{

}

bool CallbackInterface::OnFile(OnFileStruct * onFileStruct)
{
	//MAKE ALL THIS MULITHREADED

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
	outputFile.open(onFileStruct->fileName, std::ios::app | std::ios::binary);

	outputFile << fileData;
	outputFile.close();

	m_receivedFileData = fileData;
	m_receivedFilePath = onFileStruct->fileName;
	return true;
}

void CallbackInterface::OnFileProgress(FileProgressStruct * fps)
{
	std::cout << "Receiving file" << std::endl;
}


RakNet::FileList* CallbackInterface::GetFileList()
{
	RakNet::FileList* fList = new RakNet::FileList;
	FileListNodeContext context;
	int slashIndex = -1;
	for (int i = m_receivedFilePath.length() - 1; i >= 0; i--)
	{
		if (m_receivedFilePath[i] == '\\')
		{
			slashIndex = i;
			break;
		}
	}

	m_receivedFilePath = m_receivedFilePath.substr(slashIndex + 1, m_receivedFilePath.length());
	fList->AddFile(m_receivedFilePath.c_str(), m_receivedFilePath.c_str(), m_receivedFileData.c_str(), m_receivedFileData.length(), m_receivedFileData.length(), context);

	return fList;
}

