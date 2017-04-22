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

static FILE * log; // �α� ����

void _CreateDirectory(LPCTSTR dstPath) // ���丮 ��� ���� �Լ�
{
	char pathBuffer[MAX_PATH];

	int len = strlen(dstPath);

	for (int i = 0; i <= len; i++)
	{
		pathBuffer[i] = *(dstPath + i);
		if (pathBuffer[i] == '\\' || pathBuffer[i] == '/' || i == len) // ��ΰ� �̹� �����ϴ��� üũ
		{
			pathBuffer[i + 1] = NULL; // �ι��� ���
			if (_access(pathBuffer, 0) != 0) // �������� ������ ����
			{
				CreateDirectory(pathBuffer, NULL);
				printf("dst path�� �������� �ʾƼ� �����߽��ϴ�!: %s\n", dstPath);
			}
		}
	}
}

void myBackUp(char * srcPath, char * dstPath)
{
	// src ����� ������ �����ϴ��� Ȯ���ϰ�, ���� ������ ���� �޽��� ���.
	struct _stat stat_srcPath;
	if (_stat(srcPath, &stat_srcPath) != 0) {
		switch (errno) {
		case ENOENT:
			fprintf(stderr, "%s ������ ã�� �� ����.\n", srcPath);
			break;
		case EINVAL:
			fprintf(stderr, "_stat �Ķ���� ����.\n");
			break;
		default:
			fprintf(stderr, "_stat ����ġ ���� ����.\n");
			break;
		}
		return;
	}
	else
	{
		printf("src path�� �����մϴ�!: %s\n", srcPath);
	}


	// ����� ���丮�� �������� ������ �����Ѵ�.
	if (_access(dstPath, 0) != 0)
	{
		_CreateDirectory(dstPath); // CreateDirectory�� �����.
	}


	// src ��ΰ� ���丮�� �ƴ� ���� �����̸� �ٷ� dst���� ���翩�� �� �������� �ľ��ؼ� ����
	if ((_S_IFDIR & stat_srcPath.st_mode) != _S_IFDIR)
	{
		struct _stat stat_dstPath;
		_stat(dstPath, &stat_dstPath);
		if (stat_dstPath.st_nlink > 0) // dst������ �����ϸ�
		{
			if (difftime(stat_srcPath.st_mtime, stat_dstPath.st_mtime) > 0) // �������ڸ� ���ؼ� �����̸�
			{
				CopyFile(srcPath, dstPath, false); // �����Ѵ�. �� �Լ��� MFC �Լ���
				printf("������ ��¥�� �����̾ �ٲ���ϴ�!: %s\n", dstPath);
			}
		}
		else // �������� ������
		{
			if (CopyFile(srcPath, dstPath, false) != 0) // ���� �Լ��� �����ϰ�, nonzero�� ������ ����.
			{
				printf("������ �������� �ʾƼ� �����߽��ϴ�!: %s\n", dstPath);
				fprintf(log, "%s\n", dstPath);
			}
			else
			{
				fprintf(stderr, "���� ���翡 �����߽��ϴ�.��: %s\n", dstPath);
			}
		}
		return;
	}


	// src ���丮 ���� ���� ����� ����Ѵ�.
	_finddata_t foundFile;
	long findHandle;

	_chdir(srcPath);
	char curWorkDir[1024];
	_getcwd(curWorkDir, 1024); // �۾� ���丮�� ��ȯ�ϴ� �Լ�.
	printf("\n�۾� ���丮: %s\n", curWorkDir);
	findHandle = _findfirst(".\\*.*", &foundFile); // src���丮 �� ��� ������ ã�´�.

	struct _stat stat_srcFile;

	for (int findResult = 1; findResult != -1; findResult = _findnext(findHandle, &foundFile)) // ���丮 ���� ���ϵ��� �ϳ��� �о��
	{
		if (_stat(foundFile.name, &stat_srcFile) == 0) // ������ ��Ÿ�����͸� �������� (0�̸� ������ �����ϴ� ��)
		{
			if (strcmp(foundFile.name, ".") == 0 || strcmp(foundFile.name, "..") == 0) // .�̳� ..�� �����µ�, ��״� �ǳʶٵ��� ��
			{
				continue;
			}

			// ã�� ������ ���� ��θ� ����
			char srcAbsPath[1024] = "";
			strcpy(srcAbsPath, srcPath);
			strcat(srcAbsPath, "\\");
			strcat(srcAbsPath, foundFile.name);

			// ������ ����� ���� ��θ� ����
			char dstAbsPath[1024] = "";
			strcpy(dstAbsPath, dstPath);
			strcat(dstAbsPath, "\\");
			strcat(dstAbsPath, foundFile.name);

			if ((_S_IFDIR & stat_srcFile.st_mode) != _S_IFDIR) // ��Ͽ��� ���� ������ ���丮�� �ƴ� ���
			{
				struct _stat stat_dstFile;

				if (_stat(dstAbsPath, &stat_dstFile) == 0) // �����Ǵ� dst������ �̹� �����ϸ�
				{
					if (difftime(stat_srcFile.st_mtime, stat_dstFile.st_mtime) > 0) // �������ڸ� ���ؼ� �����̸�
					{
						if (CopyFile(srcAbsPath, dstAbsPath, false) != 0) // ���� �Լ��� �����ϰ�, nonzero�� ������ ����.
						{
							printf("������ ��¥�� �����̾ �ٲ���ϴ�!: %s\\%s\n", dstPath, foundFile.name);
							fprintf(log, "%s\n", dstAbsPath);
						}
						else
						{
							fprintf(stderr, "���� ���翡 �����߽��ϴ�.��: %s\\%s\n", dstPath, foundFile.name);
						}
					}
					else
					{
						printf("������ ��¥�� ���ų� �� �ֱ��̾ �ȹٲ���ϴ�.��: %s\\%s\n", dstPath, foundFile.name);
					}
				}
				else // ���� dst������ �������� ������ �׳� ����
				{
					CopyFile(srcAbsPath, dstAbsPath, false);
					printf("������ �������� �ʾƼ� �����߽��ϴ�!: %s\\%s\n", dstPath, foundFile.name);
					fprintf(log, "%s\n", dstAbsPath);
				}
			}
			else // ��Ͽ��� ���� ������ ���丮�� ���� �� ���丮�� ��θ� �Ѱ��ְ� ��ͷ� Ž����.
			{
				myBackUp(srcAbsPath, dstAbsPath); // ���
				_chdir(".."); // ��� ������ ��� ����
				_getcwd(curWorkDir, 1024); // ������ �۾� ���丮�� �������
				printf("\n�۾� ���丮: %s\n", curWorkDir);
			}
		}
		else // ������ ��Ÿ�����͸� �������� ���ϸ� �� ������ ���� �����޽��� ���.
		{
			switch (errno) {
			case ENOENT:
				fprintf(stderr, "%s ������ ã�� �� ����.\n", srcPath);
				break;
			case EINVAL:
				fprintf(stderr, "_stat �Ķ���� ����.\n");
				break;
			default:
				fprintf(stderr, "_stat ����ġ ���� ����.\n");
				break;
			}
		}
	}

	_findclose(findHandle);

}

int main(int argc, char * argv[]) {
	time_t curTime = time(NULL); // ���� �ð� �޾ƿ�
	struct tm * d = localtime(&curTime);

	// �����
	if (argc != 3)
	{
		printf("Usage : mybackup [srcpath] [dstpath]\n");
		return -1;
	}

	// ������� ����
	log = fopen("mybackup.log", "a");
	fprintf(log, "%d�� %d�� %d�� %d�� %d�� %d��\n", d->tm_year + 1900, d->tm_mon + 1, d->tm_mday, d->tm_hour, d->tm_min, d->tm_sec);

	// ����
	myBackUp(argv[1], argv[2]);

	return 0;
}
