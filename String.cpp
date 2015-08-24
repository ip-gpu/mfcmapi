#include "stdafx.h"
#include "string.h"
#include <algorithm>
#include <locale>
#include <codecvt>

wstring format(const LPWSTR fmt, ...)
{
	LPWSTR buffer = NULL;
	va_list vl;
	va_start(vl, fmt);
	int len = _vscwprintf(fmt, vl);
	if (0 != len)
	{
		len++;
		buffer = new wchar_t[len];
		memset(buffer, 0, sizeof(wchar_t)* len);
		(void)_vsnwprintf_s(buffer, len, len, fmt, vl);
	}

	wstring ret(buffer);
	va_end(vl);
	delete[] buffer;
	return ret;
}

wstring loadstring(DWORD dwID)
{
	wstring fmtString;
	LPWSTR buffer = 0;
	size_t len = ::LoadStringW(NULL, dwID, (PWCHAR)&buffer, 0);

	if (len)
	{
		fmtString.assign(buffer, len);
	}

	return fmtString;
}

wstring formatmessage(DWORD dwID, ...)
{
	wstring format = loadstring(dwID);

	LPWSTR buffer = NULL;
	wstring ret;
	va_list vl;
	va_start(vl, dwID);
	DWORD dw = FormatMessageW(FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ALLOCATE_BUFFER, format.c_str(), 0, 0, (LPWSTR)&buffer, 0, &vl);
	if (dw)
	{
		ret = wstring(buffer);
		(void)LocalFree(buffer);
	}

	va_end(vl);
	return ret;
}

LPTSTR wstringToLPTSTR(wstring src)
{
	LPTSTR dst = NULL;
#ifdef UNICODE
	size_t cch = src.length();
	if (!cch) return NULL;

	cch++; // Null terminator
	dst = new WCHAR[cch];
	if (dst)
	{
		memcpy(dst, src.c_str(), cch * sizeof(WCHAR));
	}
#else
	HRESULT hRes = S_OK;
	EC_H(UnicodeToAnsi(src.c_str(), &dst));
#endif

	return dst;
}

LPWSTR wstringToLPWSTR(wstring src)
{
	size_t cch = src.length();
	if (!cch) return NULL;

	cch++; // Null terminator
	LPWSTR dst = new WCHAR[cch];
	if (dst)
	{
		memcpy(dst, src.c_str(), cch * sizeof(WCHAR));
	}

	return dst;
}

CString wstringToCString(wstring src)
{
	CString dst;
#ifdef UNICODE
	dst = src.c_str();
#else
	dst.Format("%ws", src.c_str());
#endif
	return dst;
}

wstring LPTSTRToWstring(LPTSTR src)
{
#ifdef UNICODE
	return src;
#else
	return LPSTRToWstring(src);
#endif
}

wstring LPSTRToWstring(LPSTR src)
{
	if (!src) return L"";
	string ansi = src;
	return wstring(ansi.begin(), ansi.end());
}

std::wstring_convert<std::codecvt<wchar_t, char, std::mbstate_t> > s_converter("");

wstring stringToWstring(string src)
{
	return s_converter.from_bytes(src);
}

void wstringToLower(wstring src)
{
	std::transform(src.begin(), src.end(), src.begin(), ::tolower);
}

// Converts a wstring to a number. Will return 0 if string is empty or contains non-numeric data.
ULONG wstringToUlong(wstring src, int radix)
{
	if (src.empty()) return 0;

	LPWSTR szEndPtr = NULL;
	ULONG ulArg = wcstoul(src.c_str(), &szEndPtr, radix);

	// if szEndPtr is pointing to something other than NULL, this must be a string
	if (!szEndPtr || *szEndPtr)
	{
		ulArg = NULL;
	}

	return ulArg;
}

// if cchszA == -1, MultiByteToWideChar will compute the length
// Delete with delete[]
_Check_return_ HRESULT AnsiToUnicode(_In_opt_z_ LPCSTR pszA, _Out_z_cap_(cchszA) LPWSTR* ppszW, size_t cchszA)
{
	HRESULT hRes = S_OK;
	if (!ppszW) return MAPI_E_INVALID_PARAMETER;
	*ppszW = NULL;
	if (NULL == pszA) return S_OK;
	if (!cchszA) return S_OK;

	// Get our buffer size
	int iRet = 0;
	EC_D(iRet, MultiByteToWideChar(
		CP_ACP,
		0,
		pszA,
		(int)cchszA,
		NULL,
		NULL));
	if (SUCCEEDED(hRes) && 0 != iRet)
	{
		// MultiByteToWideChar returns num of chars
		LPWSTR pszW = new WCHAR[iRet];

		EC_D(iRet, MultiByteToWideChar(
			CP_ACP,
			0,
			pszA,
			(int)cchszA,
			pszW,
			iRet));
		if (SUCCEEDED(hRes))
		{
			*ppszW = pszW;
		}
		else
		{
			delete[] pszW;
		}
	}

	return hRes;
}

// if cchszW == -1, WideCharToMultiByte will compute the length
// Delete with delete[]
_Check_return_ HRESULT UnicodeToAnsi(_In_z_ LPCWSTR pszW, _Out_z_cap_(cchszW) LPSTR* ppszA, size_t cchszW)
{
	HRESULT hRes = S_OK;
	if (!ppszA) return MAPI_E_INVALID_PARAMETER;
	*ppszA = NULL;
	if (NULL == pszW) return S_OK;

	// Get our buffer size
	int iRet = 0;
	EC_D(iRet, WideCharToMultiByte(
		CP_ACP,
		0,
		pszW,
		(int)cchszW,
		NULL,
		NULL,
		NULL,
		NULL));
	if (SUCCEEDED(hRes) && 0 != iRet)
	{
		// WideCharToMultiByte returns num of bytes
		LPSTR pszA = (LPSTR) new BYTE[iRet];

		EC_D(iRet, WideCharToMultiByte(
			CP_ACP,
			0,
			pszW,
			(int)cchszW,
			pszA,
			iRet,
			NULL,
			NULL));
		if (SUCCEEDED(hRes))
		{
			*ppszA = pszA;
		}
		else
		{
			delete[] pszA;
		}
	}

	return hRes;
}

bool IsNullOrEmptyW(LPWSTR szStr)
{
	return !szStr || !szStr[0];
}

bool IsNullOrEmptyA(LPCSTR szStr)
{
	return !szStr || !szStr[0];
}