#include "3ds_ext_key_downloader.h"
#include "ncch.h"

C3dsExtKeyDownloader::SOption C3dsExtKeyDownloader::s_Option[] =
{
	{ nullptr, 0, USTR("action:") },
	{ USTR("download"), USTR('d'), USTR("download ext key") },
	{ USTR("sample"), 0, USTR("show the samples") },
	{ USTR("help"), USTR('h'), USTR("show this help") },
	{ nullptr, 0, USTR("\ncommon:") },
	{ USTR("verbose"), USTR('v'), USTR("show the info") },
	{ nullptr, 0, USTR(" download:") },
	{ USTR("download-begin"), 0, USTR("[0-9A-Fa-f]{1,5}\n\t\tthe begin unique id of download range") },
	{ USTR("download-end"), 0, USTR("[0-9A-Fa-f]{1,5}\n\t\tthe end unique id of download range") },
	{ nullptr, 0, nullptr }
};

C3dsExtKeyDownloader::C3dsExtKeyDownloader()
	: m_eAction(kActionNone)
	, m_bVerbose(false)
	, m_nDownloadBegin(-1)
	, m_nDownloadEnd(-1)
{
}

C3dsExtKeyDownloader::~C3dsExtKeyDownloader()
{
}

int C3dsExtKeyDownloader::ParseOptions(int a_nArgc, UChar* a_pArgv[])
{
	if (a_nArgc <= 1)
	{
		return 1;
	}
	for (int i = 1; i < a_nArgc; i++)
	{
		int nArgpc = static_cast<int>(UCslen(a_pArgv[i]));
		if (nArgpc == 0)
		{
			continue;
		}
		int nIndex = i;
		if (a_pArgv[i][0] != USTR('-'))
		{
			UPrintf(USTR("ERROR: illegal option\n\n"));
			return 1;
		}
		else if (nArgpc > 1 && a_pArgv[i][1] != USTR('-'))
		{
			for (int j = 1; j < nArgpc; j++)
			{
				switch (parseOptions(a_pArgv[i][j], nIndex, a_nArgc, a_pArgv))
				{
				case kParseOptionReturnSuccess:
					break;
				case kParseOptionReturnIllegalOption:
					UPrintf(USTR("ERROR: illegal option\n\n"));
					return 1;
				case kParseOptionReturnNoArgument:
					UPrintf(USTR("ERROR: no argument\n\n"));
					return 1;
				case kParseOptionReturnUnknownArgument:
					UPrintf(USTR("ERROR: unknown argument \"%") PRIUS USTR("\"\n\n"), m_sMessage.c_str());
					return 1;
				case kParseOptionReturnOptionConflict:
					UPrintf(USTR("ERROR: option conflict\n\n"));
					return 1;
				}
			}
		}
		else if (nArgpc > 2 && a_pArgv[i][1] == USTR('-'))
		{
			switch (parseOptions(a_pArgv[i] + 2, nIndex, a_nArgc, a_pArgv))
			{
			case kParseOptionReturnSuccess:
				break;
			case kParseOptionReturnIllegalOption:
				UPrintf(USTR("ERROR: illegal option\n\n"));
				return 1;
			case kParseOptionReturnNoArgument:
				UPrintf(USTR("ERROR: no argument\n\n"));
				return 1;
			case kParseOptionReturnUnknownArgument:
				UPrintf(USTR("ERROR: unknown argument \"%") PRIUS USTR("\"\n\n"), m_sMessage.c_str());
				return 1;
			case kParseOptionReturnOptionConflict:
				UPrintf(USTR("ERROR: option conflict\n\n"));
				return 1;
			}
		}
		i = nIndex;
	}
	return 0;
}

int C3dsExtKeyDownloader::CheckOptions()
{
	if (m_eAction == kActionNone)
	{
		UPrintf(USTR("ERROR: nothing to do\n\n"));
		return 1;
	}
	if (m_eAction == kActionDownload)
	{
		if (m_nDownloadBegin < 0 && m_nDownloadEnd < 0)
		{
			UPrintf(USTR("ERROR: no --download-begin or --download-end option\n\n"));
			return 1;
		}
	}
	return 0;
}

int C3dsExtKeyDownloader::Help()
{
	UPrintf(USTR("3ds_ext_key_downloader %") PRIUS USTR(" by dnasdw\n\n"), AToU(_3DS_EXT_KEY_DOWNLOADER_VERSION).c_str());
	UPrintf(USTR("usage: 3ds_ext_key_downloader [option...] [option]...\n\n"));
	UPrintf(USTR("option:\n"));
	SOption* pOption = s_Option;
	while (pOption->Name != nullptr || pOption->Doc != nullptr)
	{
		if (pOption->Name != nullptr)
		{
			UPrintf(USTR("  "));
			if (pOption->Key != 0)
			{
				UPrintf(USTR("-%c,"), pOption->Key);
			}
			else
			{
				UPrintf(USTR("   "));
			}
			UPrintf(USTR(" --%-8") PRIUS, pOption->Name);
			if (UCslen(pOption->Name) >= 8 && pOption->Doc != nullptr)
			{
				UPrintf(USTR("\n%16") PRIUS, USTR(""));
			}
		}
		if (pOption->Doc != nullptr)
		{
			UPrintf(USTR("%") PRIUS, pOption->Doc);
		}
		UPrintf(USTR("\n"));
		pOption++;
	}
	return 0;
}

int C3dsExtKeyDownloader::Action()
{
	if (m_eAction == kActionDownload)
	{
		if (!download())
		{
			UPrintf(USTR("ERROR: download failed\n\n"));
			return 1;
		}
	}
	if (m_eAction == kActionSample)
	{
		return sample();
	}
	if (m_eAction == kActionHelp)
	{
		return Help();
	}
	return 0;
}

C3dsExtKeyDownloader::EParseOptionReturn C3dsExtKeyDownloader::parseOptions(const UChar* a_pName, int& a_nIndex, int a_nArgc, UChar* a_pArgv[])
{
	if (UCscmp(a_pName, USTR("download")) == 0)
	{
		if (m_eAction == kActionNone)
		{
			m_eAction = kActionDownload;
		}
		else if (m_eAction != kActionDownload && m_eAction != kActionHelp)
		{
			return kParseOptionReturnOptionConflict;
		}
	}
	else if (UCscmp(a_pName, USTR("sample")) == 0)
	{
		if (m_eAction == kActionNone)
		{
			m_eAction = kActionSample;
		}
		else if (m_eAction != kActionSample && m_eAction != kActionHelp)
		{
			return kParseOptionReturnOptionConflict;
		}
	}
	else if (UCscmp(a_pName, USTR("help")) == 0)
	{
		m_eAction = kActionHelp;
	}
	else if (UCscmp(a_pName, USTR("verbose")) == 0)
	{
		m_bVerbose = true;
	}
	else if (UCscmp(a_pName, USTR("download-begin")) == 0)
	{
		if (a_nIndex + 1 >= a_nArgc)
		{
			return kParseOptionReturnNoArgument;
		}
		UString sDownloadBegin = a_pArgv[++a_nIndex];
		static URegex uniqueId(USTR("[0-9A-Fa-f]{1,5}"), regex_constants::ECMAScript | regex_constants::icase);
		if (!regex_match(sDownloadBegin, uniqueId))
		{
			m_sMessage = sDownloadBegin;
			return kParseOptionReturnUnknownArgument;
		}
		m_nDownloadBegin = SToN32(sDownloadBegin, 16);
	}
	else if (UCscmp(a_pName, USTR("download-end")) == 0)
	{
		if (a_nIndex + 1 >= a_nArgc)
		{
			return kParseOptionReturnNoArgument;
		}
		UString sDownloadEnd = a_pArgv[++a_nIndex];
		static URegex uniqueId(USTR("[0-9A-Fa-f]{1,5}"), regex_constants::ECMAScript | regex_constants::icase);
		if (!regex_match(sDownloadEnd, uniqueId))
		{
			m_sMessage = sDownloadEnd;
			return kParseOptionReturnUnknownArgument;
		}
		m_nDownloadEnd = SToN32(sDownloadEnd, 16);
	}
	return kParseOptionReturnSuccess;
}

C3dsExtKeyDownloader::EParseOptionReturn C3dsExtKeyDownloader::parseOptions(int a_nKey, int& a_nIndex, int m_nArgc, UChar* a_pArgv[])
{
	for (SOption* pOption = s_Option; pOption->Name != nullptr || pOption->Key != 0 || pOption->Doc != nullptr; pOption++)
	{
		if (pOption->Key == a_nKey)
		{
			return parseOptions(pOption->Name, a_nIndex, m_nArgc, a_pArgv);
		}
	}
	return kParseOptionReturnIllegalOption;
}


bool C3dsExtKeyDownloader::download()
{
	if (m_nDownloadBegin > m_nDownloadEnd)
	{
		n32 nTemp = m_nDownloadBegin;
		m_nDownloadBegin = m_nDownloadEnd;
		m_nDownloadEnd = nTemp;
	}
	if (m_nDownloadBegin < 0)
	{
		m_nDownloadBegin = m_nDownloadEnd;
	}
	CNcch ncch;
	ncch.SetVerbose(m_bVerbose);
	ncch.SetDownloadBegin(m_nDownloadBegin);
	ncch.SetDownloadEnd(m_nDownloadEnd);
	return ncch.Download();
}

int C3dsExtKeyDownloader::sample()
{
	UPrintf(USTR("sample:\n"));
	UPrintf(USTR("# download ext key\n"));
	UPrintf(USTR("3ds_ext_key_downloader -dv --download-begin 00000 --download-end 02FFF\n\n"));
	return 0;
}

int UMain(int argc, UChar* argv[])
{
	C3dsExtKeyDownloader downloader;
	if (downloader.ParseOptions(argc, argv) != 0)
	{
		return downloader.Help();
	}
	if (downloader.CheckOptions() != 0)
	{
		return 1;
	}
	return downloader.Action();
}
