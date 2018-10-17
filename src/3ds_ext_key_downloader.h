#ifndef _3DS_EXT_KEY_DOWNLOADER_H_
#define _3DS_EXT_KEY_DOWNLOADER_H_

#include <sdw.h>

class C3dsExtKeyDownloader
{
public:
	enum EParseOptionReturn
	{
		kParseOptionReturnSuccess,
		kParseOptionReturnIllegalOption,
		kParseOptionReturnNoArgument,
		kParseOptionReturnUnknownArgument,
		kParseOptionReturnOptionConflict
	};
	enum EAction
	{
		kActionNone,
		kActionDownload,
		kActionSample,
		kActionHelp
	};
	struct SOption
	{
		const UChar* Name;
		int Key;
		const UChar* Doc;
	};
	C3dsExtKeyDownloader();
	~C3dsExtKeyDownloader();
	int ParseOptions(int a_nArgc, UChar* a_pArgv[]);
	int CheckOptions();
	int Help();
	int Action();
	static SOption s_Option[];
private:
	EParseOptionReturn parseOptions(const UChar* a_pName, int& a_nIndex, int a_nArgc, UChar* a_pArgv[]);
	EParseOptionReturn parseOptions(int a_nKey, int& a_nIndex, int a_nArgc, UChar* a_pArgv[]);
	bool download();
	int sample();
	EAction m_eAction;
	bool m_bVerbose;
	n32 m_nDownloadBegin;
	n32 m_nDownloadEnd;
	UString m_sMessage;
};

#endif	// _3DS_EXT_KEY_DOWNLOADER_H_
