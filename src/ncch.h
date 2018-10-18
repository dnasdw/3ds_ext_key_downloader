#ifndef NCCH_H_
#define NCCH_H_

#include <sdw.h>

class CNcch
{
public:
	CNcch();
	~CNcch();
	void SetVerbose(bool a_bVerbose);
	void SetDownloadBegin(n32 a_nDownloadBegin);
	void SetDownloadEnd(n32 a_nDownloadEnd);
	bool Download(bool a_bReadExtKey = true);
private:
	void readExtKey();
	bool writeExtKey();
	bool m_bVerbose;
	n32 m_nDownloadBegin;
	n32 m_nDownloadEnd;
	map<string, string> m_mExtKey;
};

#endif	// NCCH_H_
