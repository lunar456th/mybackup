# mybackup
File Structure Homework #1

Copyright(c)2017 lunar456th@naver.com All rights reserved. 

개발환경: Windows 10, Visual Studio 2015
사용법: > mybackup [srcpath] [dstpath]

※꼭 '절대 경로'를 넣어주세요.

경로에 file path를 넣을 경우에는 해당 파일이 복사되며,

복사할 파일의 이름이나 경로명이 달라도 원활히 복사가 됩니다.

예) mybackup C:\Users\sj\Desktop\a.txt C:\Users\sj\Desktop\temp\b.txt

경로에 directory path를 넣을 경우에는 하위 디렉토리를 재귀 탐색하여

디렉토리 내 모든 파일이 원래의 디렉토리 구조를 유지하며 복사가 됩니다.

로그 파일은 복사 시작 시에 생성되며, 덮어 쓰기가 아닌 이어 쓰기 방식입니다.

로그 파일 생성 시 현재 시간이 기록되고, 복사가 일어날 때마다 해당 파일의 경로를 로그 파일에 기록합니다.
