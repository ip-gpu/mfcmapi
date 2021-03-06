#include "stdafx.h"
#include "VerbStream.h"
#include <Interpret/SmartView/SmartView.h>
#include <Interpret/ExtraPropTags.h>

VerbStream::VerbStream()
{
	m_Version = 0;
	m_Count = 0;
	m_Version2 = 0;
}

void VerbStream::Parse()
{
	m_Version = m_Parser.Get<WORD>();
	m_Count = m_Parser.Get<DWORD>();

	if (m_Count && m_Count < _MaxEntriesSmall)
	{
		for (ULONG i = 0; i < m_Count; i++)
		{
			VerbData verbData;
			verbData.VerbType = m_Parser.Get<DWORD>();
			verbData.DisplayNameCount = m_Parser.Get<BYTE>();
			verbData.DisplayName = m_Parser.GetStringA(verbData.DisplayNameCount);
			verbData.MsgClsNameCount = m_Parser.Get<BYTE>();
			verbData.MsgClsName = m_Parser.GetStringA(verbData.MsgClsNameCount);
			verbData.Internal1StringCount = m_Parser.Get<BYTE>();
			verbData.Internal1String = m_Parser.GetStringA(verbData.Internal1StringCount);
			verbData.DisplayNameCountRepeat = m_Parser.Get<BYTE>();
			verbData.DisplayNameRepeat = m_Parser.GetStringA(verbData.DisplayNameCountRepeat);
			verbData.Internal2 = m_Parser.Get<DWORD>();
			verbData.Internal3 = m_Parser.Get<BYTE>();
			verbData.fUseUSHeaders = m_Parser.Get<DWORD>();
			verbData.Internal4 = m_Parser.Get<DWORD>();
			verbData.SendBehavior = m_Parser.Get<DWORD>();
			verbData.Internal5 = m_Parser.Get<DWORD>();
			verbData.ID = m_Parser.Get<DWORD>();
			verbData.Internal6 = m_Parser.Get<DWORD>();
			m_lpVerbData.push_back(verbData);
		}
	}

	m_Version2 = m_Parser.Get<WORD>();

	if (m_Count && m_Count < _MaxEntriesSmall)
	{
		for (ULONG i = 0; i < m_Count; i++)
		{
			VerbExtraData verbExtraData;
			verbExtraData.DisplayNameCount = m_Parser.Get<BYTE>();
			verbExtraData.DisplayName = m_Parser.GetStringW(verbExtraData.DisplayNameCount);
			verbExtraData.DisplayNameCountRepeat = m_Parser.Get<BYTE>();
			verbExtraData.DisplayNameRepeat = m_Parser.GetStringW(verbExtraData.DisplayNameCountRepeat);
			m_lpVerbExtraData.push_back(verbExtraData);
		}
	}
}

_Check_return_ wstring VerbStream::ToStringInternal()
{
	auto szVerbString = formatmessage(IDS_VERBHEADER, m_Version, m_Count);

	for (ULONG i = 0; i < m_lpVerbData.size(); i++)
	{
		auto szVerb = InterpretNumberAsStringProp(m_lpVerbData[i].ID, PR_LAST_VERB_EXECUTED);
		szVerbString += formatmessage(IDS_VERBDATA,
			i,
			m_lpVerbData[i].VerbType,
			m_lpVerbData[i].DisplayNameCount,
			m_lpVerbData[i].DisplayName.c_str(),
			m_lpVerbData[i].MsgClsNameCount,
			m_lpVerbData[i].MsgClsName.c_str(),
			m_lpVerbData[i].Internal1StringCount,
			m_lpVerbData[i].Internal1String.c_str(),
			m_lpVerbData[i].DisplayNameCountRepeat,
			m_lpVerbData[i].DisplayNameRepeat.c_str(),
			m_lpVerbData[i].Internal2,
			m_lpVerbData[i].Internal3,
			m_lpVerbData[i].fUseUSHeaders,
			m_lpVerbData[i].Internal4,
			m_lpVerbData[i].SendBehavior,
			m_lpVerbData[i].Internal5,
			m_lpVerbData[i].ID,
			szVerb.c_str(),
			m_lpVerbData[i].Internal6);
	}

	szVerbString += formatmessage(IDS_VERBVERSION2, m_Version2);

	for (ULONG i = 0; i < m_lpVerbExtraData.size(); i++)
	{
		szVerbString += formatmessage(IDS_VERBEXTRADATA,
			i,
			m_lpVerbExtraData[i].DisplayNameCount,
			m_lpVerbExtraData[i].DisplayName.c_str(),
			m_lpVerbExtraData[i].DisplayNameCountRepeat,
			m_lpVerbExtraData[i].DisplayNameRepeat.c_str());
	}

	return szVerbString;
}