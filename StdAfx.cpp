// StdAfx.cpp : source file that includes just the standard includes
//	PlayDemo.pch will be the pre-compiled header
//	StdAfx.obj will contain the pre-compiled type information

#include "StdAfx.h"
#include "CharactorTansfer.h"

CString GetMoudlePath()
{
	TCHAR szAppName[MAX_PATH];
	// stdafx.cpp : 只包括标准包含文件的源文件
	// PlayDemo.pch 将作为预编译头
	// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"



	TCHAR szDir[MAX_PATH];
	TCHAR szDrive[MAX_PATH];
	GetModuleFileName(GetModuleHandle(NULL), szAppName, MAX_PATH);
	_tsplitpath(szAppName, szDrive, szDir, NULL, NULL);

	TCHAR szPath[MAX_PATH];
	_tmakepath(szPath, szDrive, szDir, NULL, NULL);

	return szPath;
}

