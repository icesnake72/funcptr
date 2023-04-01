// funcptr.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE		4096
#define NO_ERROR		0
#define READ_DATA		1
#define WRITE_DATA		2

typedef unsigned int			uint;
typedef unsigned long long		ulonglong;

int ioMessageProc(ulonglong llData, uint nCode);
void copyFile(const char* srcFileName, const char* destFileName, int (*pfn)(ulonglong llData, uint nCode));

int main()
{
	int (*pfn)(ulonglong llData, uint nCode);
	pfn = ioMessageProc;
	
	copyFile("Game.zip", "Game2.zip", pfn);
	
	return 0;
}

void copyFile(const char* srcFileName, const char* destFileName, int (*pfn)(ulonglong llData, uint nCode))
{
	char* buf = NULL;
	size_t readBytes = 0;
	size_t writtenBytes = 0;

	FILE* srcFile = NULL;
	FILE* destFile = NULL;
		
	errno_t error = NO_ERROR;

	buf = (char*)malloc(BUFFER_SIZE);
	if (buf == NULL)
	{
		printf("메모리 할당 실패!!!\n");
		return;
	}
	
	error = fopen_s(&srcFile, srcFileName, "r+b");
	if (error)
	{		
		printf("원본파일을 찾을수 없습니다.\n");
		goto Release_Area;
	}

	error = fopen_s(&destFile, destFileName, "w+b");	// destination file을 append모드로 열었을때는 파일이 있는지 체크를 해야함
	if (error)
	{
		printf("대상 파일을 열 수 없습니다.\n");
		goto Release_Area;
	}

	fseek(srcFile, 0, SEEK_SET);	// 파일을 처음부터 순차적으로 읽어야 할때는 필수!!!
	while (!feof(srcFile))
	{
		readBytes = fread(buf, sizeof(char), BUFFER_SIZE, srcFile);
		if (readBytes)
		{			
			pfn((ulonglong)readBytes, READ_DATA);
			writtenBytes = fwrite(buf, sizeof(char), readBytes, destFile);
			pfn((ulonglong)readBytes, WRITE_DATA);			
		}
	}

	printf("파일 복사가 완료되었습니다.\n");

Release_Area:

	if (buf)
	{
		free(buf);
		buf = NULL;
	}

	if (srcFile)
	{
		fclose(srcFile);
		srcFile = NULL;
	}

	if (destFile)
	{
		fclose(destFile);
		destFile = NULL;
	}

	return;
}



int ioMessageProc(ulonglong llData, uint nCode)
{
	if (nCode==READ_DATA)
	{
		printf("%lld Bytes 읽음\n", llData);
	}
	else if (nCode == WRITE_DATA)
	{
		printf("%lld Bytes 씀\n", llData);
	}

	return 0;
}