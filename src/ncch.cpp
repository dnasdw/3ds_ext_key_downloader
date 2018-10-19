#include "ncch.h"
#include "url_manager.h"
#include <curl/curl.h>

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
	CUrlManager urlManager;
	u32 uCount = m_nDownloadEnd - m_nDownloadBegin + 1;
	u32 uDownloadCount = 0;
	u32 uTotalLoadCount = 0;
	u32 uLoadCount = 0;
	while (uDownloadCount != uCount)
	{
		while (uTotalLoadCount != uCount && uLoadCount < 256)
		{
			size_t uUserData = m_nDownloadBegin + uTotalLoadCount;
			CUrl* pUrl = urlManager.HttpsGet(Format("https://kagiya-ctr.cdn.nintendo.net/title/0x000400000%05X00/ext_key?country=JP", m_nDownloadBegin + uTotalLoadCount), 30, *this, &CNcch::onHttpsGetExtKey, reinterpret_cast<void*>(uUserData));
			if (pUrl == nullptr)
			{
				urlManager.Cleanup();
				return false;
			}
			uTotalLoadCount++;
			uLoadCount++;
		}
		while (urlManager.GetCount() != 0)
		{
			u32 uCount0 = urlManager.GetCount();
			urlManager.Perform();
			u32 uCount1 = urlManager.GetCount();
			if (uCount1 != uCount0)
			{
				uDownloadCount += uCount0 - uCount1;
				uLoadCount -= uCount0 - uCount1;
				if (m_bVerbose)
				{
					UPrintf(USTR("download: %u/%u/%u\n"), uDownloadCount, uTotalLoadCount, uCount);
				}
				if (uTotalLoadCount != uCount)
				{
					break;
				}
			}
		}
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
	return true;
}

void CNcch::onHttpsGetExtKey(CUrl* a_pUrl, void* a_pUserData)
{
	size_t uUserData = reinterpret_cast<size_t>(a_pUserData);
	u32 uUniqueId = static_cast<u32>(uUserData);
	if (a_pUrl != nullptr)
	{
		const string& sData = a_pUrl->GetData();
		if (!sData.empty())
		{
			string sExtKey;
			for (int i = 0; i < static_cast<int>(sData.size()); i++)
			{
				sExtKey += Format("%02X", static_cast<u8>(sData[i]));
			}
			string sProgramId = Format("000400000%05X00", uUniqueId);
			m_mExtKey.insert(make_pair(sProgramId, sExtKey));
			if (m_bVerbose)
			{
				UPrintf(USTR("download: %") PRIUS USTR(" %") PRIUS USTR("\n"), AToU(sProgramId).c_str(), AToU(sExtKey).c_str());
			}
		}
	}
}
