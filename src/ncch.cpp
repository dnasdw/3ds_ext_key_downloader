#include "ncch.h"

CNcch::CNcch()
	: m_bVerbose(false)
	, m_nDownloadBegin(-1)
	, m_nDownloadEnd(-1)
{
}

CNcch::~CNcch()
{
}

void CNcch::SetVerbose(bool a_bVerbose)
{
	m_bVerbose = a_bVerbose;
}

void CNcch::SetDownloadBegin(n32 a_nDownloadBegin)
{
	m_nDownloadBegin = a_nDownloadBegin;
}

void CNcch::SetDownloadEnd(n32 a_nDownloadEnd)
{
	m_nDownloadEnd = a_nDownloadEnd;
}

bool CNcch::Download(bool a_bReadExtKey /* = true */)
{
	if (a_bReadExtKey)
	{
		readExtKey();
	}
	return writeExtKey();
}

void CNcch::readExtKey()
{
	UString sExtKeyPath = UGetModuleDirName() + USTR("/ext_key.txt");
	UPrintf(USTR("%") PRIUS USTR("\n"), sExtKeyPath.c_str());
}

bool CNcch::writeExtKey()
{
	return false;
}
