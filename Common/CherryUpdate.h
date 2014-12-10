#pragma once

//#define USE_MFC_CLASS
//#define INCLUDE_FILE_SIZE_INFO

#ifdef USE_MFC_CLASS
#include "afxinet.h"
#else
#include "WinInet.h"
#pragma comment(lib, "WinInet.lib")
#endif

#include "Sensapi.h"
#pragma comment(lib, "Sensapi.lib")

#include <vector>
using namespace std;

#include "tinyxml2.h"
#pragma comment(lib, "TinyXML2.lib")
using namespace tinyxml2;

///////////////////////////////////////////////////////////////////////////
///
/// \class		CCherryUpdate
/// \brief		���̺� ������Ʈ �Ŵ��� Ŭ����
/// \author		ogoons
/// \date		2014-05-19
///
/// \remark		Win32 / MFC ���� �� ������ ���еǸ� 
///             ��� USE_MFC_CLASS define�� ����ϸ� MFC Ŭ������ �����
///             ���̺� ������Ʈ dll ���尡 �����ϴ�.
///             �ӵ��� ������ ũ�Ƿ� Win32 ���� ���带 ��õ�Ѵ�.
///
/// \warning	- �ѱ� �̸����� �� ������ �ٿ�ε� �� �� ����.
///				- ������ ���� �ʼ�	
///				- TinyXML2 ���̺귯�� �ʿ�
/// \section	
///
///////////////////////////////////////////////////////////////////////////

#ifdef USE_MFC_CLASS
class AFX_EXT_CLASS CCherryUpdate : public CInternetSession
#else
class AFX_EXT_CLASS CCherryUpdate
#endif
{
// Constructors
public:
	CCherryUpdate();
	BOOL Open(
		LPCTSTR lpszAgent,
		LPCTSTR lpszClientVersion,
		LPCTSTR lpszSourceRootUrl,
		LPCTSTR lpszUpdateProfileXml,
		LPCTSTR lpszPatchTargetPath,
		BOOL bUseUTF8 = TRUE);							// ������Ʈ�� �ʿ��� ������ ��� �޸𸮿� �ε��ϰ� ������Ʈ �غ� ����

// Attributes
protected:
#ifndef USE_MFC_CLASS
	HINTERNET m_hSession;
#endif

#ifdef INCLUDE_FILE_SIZE_INFO
	// ���� ����Ʈ�� ���� ����ü
	typedef struct tagFILE_ATTR
	{
		CString		strFile;
		LONGLONG	llSize;
	} FILE_ATTR, *PFILE_ATTR;
#endif

	// ������Ʈ �������� ���� xml���� ������ �����͸� ���� ����ü
	typedef struct _UPDATE_PROFILE
	{
		vector<CString> packageFileVector;

#ifdef INCLUDE_FILE_SIZE_INFO
		vector<FILE_ATTR> partialFileVector;
#else
		vector<CString> partialFileVector;
#endif
		CString			strNewVerion;
		CString			strRootPath;
		BOOL			bPackageUpdate;
		CString			strRunPackageFilePath;
		ULONGLONG		llFileTotalSize;

		void Initialize()
		{
			packageFileVector.clear();
			partialFileVector.clear();
			strNewVerion.Empty();
			strRootPath.Empty();
			bPackageUpdate = TRUE;
			strRunPackageFilePath.Empty();
			llFileTotalSize = 0;
		}
	} UPDATE_PROFILE, *PUPDATE_PROFILE;

	UPDATE_PROFILE m_updateProfile;

	CString		m_strAgent;
	CString		m_strClientVersion;
	CString		m_strSourceRootURL;
	CString		m_strUpdateProfileXML;
	CString		m_strPatchTargetPath;

	CWinThread	*m_pUpdateProcessThread;
	BOOL		m_bStarted;
	BOOL		m_bRunPackage;
	ULONGLONG	m_llReceivedSize; // ������� �ٿ�ε� ���� ������ ���

	// Callback
	void (*m_pReceivingStatusCallback)(ULONGLONG, ULONGLONG, void *);
	void (*m_pReceiveCompleteCallback)(BOOL, BOOL, void *);
	void (*m_pApplyCompleteCallback)(BOOL, BOOL, void *);

	void *m_pReceivingStatusUserData;
	void *m_pReceiveCompleteUserData;
	void *m_pApplyCompleteUserData;

// Operations
public:
	BOOL		IsOpen() const;																									// Internet session�� ���� open�Ǿ ������Ʈ�� ������ ��������
	BOOL		IsStarted() const;																								// ������Ʈ ���� ����
	BOOL		CheckNewVersion(LPCTSTR lpszCurrentVersion);																	// �� ���� ���� ����
	BOOL		OpenUpdateProfile(LPCTSTR lpszSourceRootUrl, LPCTSTR lpszUpdateProfileXml, BOOL bUseUTF8 = TRUE);				// ������Ʈ ���� ���� ����(�ٿ�ε�, �Ľ�, �޸� �ε�)

	BOOL		StartUpdate();																									// ������Ʈ ����
	BOOL		StopUpdate();																									// ������Ʈ �ߴ�

	BOOL		ReceiveInternetFile(_In_ LPCTSTR lpszUrl, _In_ LPCTSTR lpszReceivePath, _Out_ CString &strReceivedFullPath);	// ���� �ٿ�ε�
	BOOL		ReceiveHttpResponse(_In_ LPCTSTR lpszUrl, _Out_ CString &strResponse, _In_ BOOL bUseUTF8 = TRUE);				// �ؽ�Ʈ ������ HTTP ���� ��ȯ
	
	ULONGLONG	GetTotalReceiveFileSize();																						// ���� ������ �� ������ �������� (Open�� �������� �� ����)
	void		EnableRunPackage(BOOL bRun = TRUE);																				// ��Ű�� ������Ʈ �� ��Ű�� ������ ����/����� ����

	// Set callback
	void		SetReceivingStatusCallback(LPVOID lpCallbackFunc, void *pUserData);												// �ٿ�ε� ���� ���� ��ȭ �� ȣ��
	void		SetReceiveCompleteCallback(LPVOID lpCallbackFunc, void *pUserData);												// �ٿ�ε� �Ϸ� �� ȣ��
	void		SetApplyCompleteCallback(LPVOID lpCallbackFunc, void *pUserData);												// �ٿ�ε� �� ������ �ش� ���丮�� ���� �Ϸ� �� ȣ��

	// Utilities
	static BOOL		IsInternetConnected();																						// ���ͳ� ���� ����
	static BOOL		GetFileVersion(_In_ LPCTSTR lpszFilePath, _Out_ CString &strVersion);										// ������ ���� ��������		
	static BOOL		GetFileSize(_In_ LPCTSTR lpszFilePath, _Out_ LONGLONG &llFileSize);											// ������ ������ ��������
	static BOOL		ExecuteFile(_In_ LPCTSTR lpszFilePath);																		// ���� ����
	static int		CompareVersion(LPCTSTR lpszCurrentVersion, LPCTSTR lpszNewVersion);											// ���� ��
	static void		CreateDirectoryAndParent(LPTSTR lpszPath);																	// ���丮 ����(path �� ���� ���丮�� ���ٸ� ���� ����)
	static BOOL		DeleteDirectoryAndChild(LPCTSTR lpszPath);																	// ���丮 ����(���� ���丮, ���� ��� ����)
	void			URLEncode(char *output, char *input);																		// ���� ��� �� ��

#ifdef USE_MFC_CLASS
	virtual void	Close();
#else
	BOOL			Close();																									// ���� �ݱ�
#endif

protected:
	void			Initialize();																								// �ʱ�ȭ
	CString			GetIncomingPath();																							// �ٿ�ε�� ������ �ӽ� ����� ���丮 ��� ��������

	void			ParseFileListChildNode(XMLNode *pFileListNode, LPCTSTR lpszSourceRootUrl, BOOL bUseUTF8);

	// Thread
	static UINT		UpdateProcessThread(LPVOID lpParam);																		// ���� �ٿ�ε忡�� ��������� ��� ������Ʈ ���� ������ (Worker thread)

// Implementation
public:
	virtual ~CCherryUpdate();
};