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
	FILE* fp = UFopen(sExtKeyPath.c_str(), USTR("rb"));
	if (fp != nullptr)
	{
		Fseek(fp, 0, SEEK_END);
		u32 uSize = static_cast<u32>(Ftell(fp));
		Fseek(fp, 0, SEEK_SET);
		char* pTxt = new char[uSize + 1];
		fread(pTxt, 1, uSize, fp);
		fclose(fp);
		pTxt[uSize] = '\0';
		string sTxt(pTxt);
		delete[] pTxt;
		vector<string> vTxt = SplitOf(sTxt, "\r\n");
		for (vector<string>::const_iterator it = vTxt.begin(); it != vTxt.end(); ++it)
		{
			sTxt = Trim(*it);
			if (!sTxt.empty() && !StartWith(sTxt, "//"))
			{
				vector<string> vExtKey = Split(sTxt, " ");
				if (vExtKey.size() == 2)
				{
					if (!m_mExtKey.insert(make_pair(vExtKey[0], vExtKey[1])).second)
					{
						UPrintf(USTR("INFO: multiple ext key for %") PRIUS USTR("\n"), AToU(vExtKey[0]).c_str());
					}
				}
				else
				{
					UPrintf(USTR("INFO: unknown ext key record %") PRIUS USTR("\n"), AToU(sTxt).c_str());
				}
			}
		}
	}
}

bool CNcch::writeExtKey()
{
	UString sExtKeyPath = UGetModuleDirName() + USTR("/ext_key.txt");
	FILE* fp = UFopen(sExtKeyPath.c_str(), USTR("wb"));
	if (fp == nullptr)
	{
		return false;
	}
	for (map<string, string>::const_iterator it = m_mExtKey.begin(); it != m_mExtKey.end(); ++it)
	{
		fprintf(fp, "%s %s\r\n", it->first.c_str(), it->second.c_str());
	}
	fclose(fp);
	return false;
}
