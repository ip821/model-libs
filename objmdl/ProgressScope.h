#pragma once

#include "Macros.h"
#include "objmdl_contract_i.h"
#include "servprov.h"

class CProgressScope
{
private:
	CComPtr<IServiceProvider> m_pServiceProvider;
	CComPtr<IProgressService> m_pProgressService;
public:
	CProgressScope(IProgressService* pProgressDialogService, BOOL bMarquee = 1)
	{
		m_pProgressService = pProgressDialogService;
		if (m_pProgressService)
			m_pProgressService->StartProgress(bMarquee);
	}

	CProgressScope(IServiceProvider* pServiceProvider)
	{
		m_pServiceProvider = pServiceProvider;
		m_pServiceProvider->QueryService(SERVICE_PROGRESS_DIALOG, &m_pProgressService);
		if (m_pProgressService)
			m_pProgressService->StartProgress();
	}

	HRESULT SetProgress(ULONG ulCompleted, ULONG ulTotal)
	{
		if (m_pProgressService)
			m_pProgressService->SetProgress(ulCompleted, ulTotal);
		return S_OK;
	}

	HRESULT SetCaption(BSTR bstrText)
	{
		if (m_pProgressService)
			m_pProgressService->SetProgressCaption(bstrText);
		return S_OK;
	}

	HRESULT SetText(BSTR bstrText)
	{
		if (m_pProgressService)
			m_pProgressService->SetProgressText(bstrText);
		return S_OK;
	}

	~CProgressScope()
	{
		if (m_pProgressService)
			m_pProgressService->EndProgress();
	}
};