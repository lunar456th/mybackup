/**
 * Copyright ⓒ 2017 lunar456th@naver.com All rights reserved.
 */

#include <stdio.h>
#include <string.h>
#include <io.h>
#include <time.h>
#include <conio.h>
#include <errno.h>
#include <direct.h>
#include <windows.h>
#include <sys/stat.h>
#include <sys/types.h>

#pragma warning(disable:4996) // _CRT_SECURE_NO_WARNINGS

static FILE * log; // 로그 파일

void _CreateDirectory(LPCTSTR dstPath) // 디렉토리 재귀 생성 함수
{
	char pathBuffer[MAX_PATH];

	int len = strlen(dstPath);

	for (int i = 0; i <= len; i++)
	{
		pathBuffer[i] = *(dstPath + i);
		if (pathBuffer[i] == '\\' || pathBuffer[i] == '/' || i == len) // 경로가 이미 존재하는지 체크
		{
			pathBuffer[i + 1] = NULL; // 널문자 찍기
			if (_access(pathBuffer, 0) != 0) // 존재하지 않으면 생성
			{
				CreateDirectory(pathBuffer, NULL);
				printf("dst path가 존재하지 않아서 생성했습니다!: %s\n", dstPath);
			}
		}
	}
}

void myBackUp(char * srcPath, char * dstPath)
{
	// src 경로의 파일이 존재하는지 확인하고, 만약 없으면 에러 메시지 출력.
	struct _stat stat_srcPath;
	if (_stat(srcPath, &stat_srcPath) != 0) {
		switch (errno) {
		case ENOENT:
			fprintf(stderr, "%s 파일을 찾을 수 없음.\n", srcPath);
			break;
		case EINVAL:
			fprintf(stderr, "_stat 파라미터 오류.\n");
			break;
		default:
			fprintf(stderr, "_stat 예상치 못한 오류.\n");
			break;
		}
		return;
	}
	else
	{
		printf("src path가 존재합니다!: %s\n", srcPath);
	}


	// 백업할 디렉토리가 존재하지 않으면 생성한다.
	if (_access(dstPath, 0) != 0)
	{
		_CreateDirectory(dstPath); // CreateDirectory의 재귀판.
	}


	// src 경로가 디렉토리가 아닌 단일 파일이면 바로 dst파일 존재여부 및 수정일자 파악해서 복사
	if ((_S_IFDIR & stat_srcPath.st_mode) != _S_IFDIR)
	{
		struct _stat stat_dstPath;
		_stat(dstPath, &stat_dstPath);
		if (stat_dstPath.st_nlink > 0) // dst파일이 존재하면
		{
			if (difftime(stat_srcPath.st_mtime, stat_dstPath.st_mtime) > 0) // 수정일자를 비교해서 옛날이면
			{
				CopyFile(srcPath, dstPath, false); // 복사한다. 이 함수는 MFC 함수임
				printf("수정한 날짜가 옛날이어서 바꿨습니다!: %s\n", dstPath);
			}
		}
		else // 존재하지 않으면
		{
			if (CopyFile(srcPath, dstPath, false) != 0) // 복사 함수로 복사하고, nonzero면 성공한 것임.
			{
				printf("파일이 존재하지 않아서 생성했습니다!: %s\n", dstPath);
				fprintf(log, "%s\n", dstPath);
			}
			else
			{
				fprintf(stderr, "파일 복사에 실패했습니다.ㅠ: %s\n", dstPath);
			}
		}
		return;
	}


	// src 디렉토리 내의 파일 목록을 출력한다.
	_finddata_t foundFile;
	long findHandle;

	_chdir(srcPath);
	char curWorkDir[1024];
	_getcwd(curWorkDir, 1024); // 작업 디렉토리를 반환하는 함수.
	printf("\n작업 디렉토리: %s\n", curWorkDir);
	findHandle = _findfirst(".\\*.*", &foundFile); // src디렉토리 내 모든 파일을 찾는다.

	struct _stat stat_srcFile;

	for (int findResult = 1; findResult != -1; findResult = _findnext(findHandle, &foundFile)) // 디렉토리 내의 파일들을 하나씩 읽어옴
	{
		if (_stat(foundFile.name, &stat_srcFile) == 0) // 파일의 메타데이터를 가져오고 (0이면 파일이 존재하는 것)
		{
			if (strcmp(foundFile.name, ".") == 0 || strcmp(foundFile.name, "..") == 0) // .이나 ..도 읽히는데, 얘네는 건너뛰도록 함
			{
				continue;
			}

			// 찾은 파일의 절대 경로를 저장
			char srcAbsPath[1024] = "";
			strcpy(srcAbsPath, srcPath);
			strcat(srcAbsPath, "\\");
			strcat(srcAbsPath, foundFile.name);

			// 복사할 경로의 절대 경로를 저장
			char dstAbsPath[1024] = "";
			strcpy(dstAbsPath, dstPath);
			strcat(dstAbsPath, "\\");
			strcat(dstAbsPath, foundFile.name);

			if ((_S_IFDIR & stat_srcFile.st_mode) != _S_IFDIR) // 목록에서 읽은 파일이 디렉토리가 아닌 경우
			{
				struct _stat stat_dstFile;

				if (_stat(dstAbsPath, &stat_dstFile) == 0) // 대응되는 dst파일이 이미 존재하면
				{
					if (difftime(stat_srcFile.st_mtime, stat_dstFile.st_mtime) > 0) // 수정일자를 비교해서 옛날이면
					{
						if (CopyFile(srcAbsPath, dstAbsPath, false) != 0) // 복사 함수로 복사하고, nonzero면 성공한 것임.
						{
							printf("수정한 날짜가 옛날이어서 바꿨습니다!: %s\\%s\n", dstPath, foundFile.name);
							fprintf(log, "%s\n", dstAbsPath);
						}
						else
						{
							fprintf(stderr, "파일 복사에 실패했습니다.ㅠ: %s\\%s\n", dstPath, foundFile.name);
						}
					}
					else
					{
						printf("수정한 날짜가 같거나 더 최근이어서 안바꿨습니다.ㅠ: %s\\%s\n", dstPath, foundFile.name);
					}
				}
				else // 대응 dst파일이 존재하지 않으면 그냥 복사
				{
					CopyFile(srcAbsPath, dstAbsPath, false);
					printf("파일이 존재하지 않아서 생성했습니다!: %s\\%s\n", dstPath, foundFile.name);
					fprintf(log, "%s\n", dstAbsPath);
				}
			}
			else // 목록에서 읽은 파일이 디렉토리인 경우는 그 디렉토리의 경로를 넘겨주고 재귀로 탐색함.
			{
				myBackUp(srcAbsPath, dstAbsPath); // 재귀
				_chdir(".."); // 재귀 끝나고 경로 복귀
				_getcwd(curWorkDir, 1024); // 복귀한 작업 디렉토리를 출력해줌
				printf("\n작업 디렉토리: %s\n", curWorkDir);
			}
		}
		else // 파일의 메타데이터를 가져오지 못하면 그 이유에 따라 에러메시지 출력.
		{
			switch (errno) {
			case ENOENT:
				fprintf(stderr, "%s 파일을 찾을 수 없음.\n", srcPath);
				break;
			case EINVAL:
				fprintf(stderr, "_stat 파라미터 오류.\n");
				break;
			default:
				fprintf(stderr, "_stat 예상치 못한 오류.\n");
				break;
			}
		}
	}

	_findclose(findHandle);

}

int main(int argc, char * argv[]) {
	time_t curTime = time(NULL); // 현재 시간 받아옴
	struct tm * d = localtime(&curTime);

	// 사용방법
	if (argc != 3)
	{
		printf("Usage : mybackup [srcpath] [dstpath]\n");
		return -1;
	}

	// 백업파일 생성
	log = fopen("mybackup.log", "a");
	fprintf(log, "%d년 %d월 %d일 %d시 %d분 %d초\n", d->tm_year + 1900, d->tm_mon + 1, d->tm_mday, d->tm_hour, d->tm_min, d->tm_sec);

	// 시작
	myBackUp(argv[1], argv[2]);

	return 0;
}
