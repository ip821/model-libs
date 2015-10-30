#pragma once

class CComVar :	public tagVARIANT
{
	// Constructors
public:
	CComVar() throw()
	{
		// Make sure that variant data are initialized to 0
		memset(this, 0, sizeof(tagVARIANT));
		::VariantInit(this);
	}
	~CComVar() throw()
	{
		HRESULT hr = Clear();
		ATLASSERT(SUCCEEDED(hr));
		(hr);
	}
	CComVar(_In_ const VARIANT& varSrc) ATLVARIANT_THROW()
	{
		vt = VT_EMPTY;
		InternalCopy(&varSrc);
	}
	CComVar(_In_ const CComVar& varSrc) ATLVARIANT_THROW()
	{
		vt = VT_EMPTY;
		InternalCopy(&varSrc);
	}
	CComVar(_In_z_ LPCOLESTR lpszSrc) ATLVARIANT_THROW()
	{
		vt = VT_EMPTY;
		*this = lpszSrc;
	}
	CComVar(_In_z_ LPCSTR lpszSrc) ATLVARIANT_THROW()
	{
		vt = VT_EMPTY;
		*this = lpszSrc;
	}
	CComVar(_In_ bool bSrc) throw()
	{
		vt = VT_BOOL;
		boolVal = bSrc ? TRUE : FALSE;
	}

	CComVar(_In_ int nSrc, _In_ VARTYPE vtSrc = VT_I4) ATLVARIANT_THROW()
	{
		ATLASSERT(vtSrc == VT_I4 || vtSrc == VT_INT);
		if (vtSrc == VT_I4 || vtSrc == VT_INT)
		{
			vt = vtSrc;
			intVal = nSrc;
		}
		else
		{
			vt = VT_ERROR;
			scode = E_INVALIDARG;
#ifndef _ATL_NO_VARIANT_THROW
			AtlThrow(E_INVALIDARG);
#endif
		}
	}

	CComVar(_In_ BYTE nSrc) throw()
	{
		vt = VT_UI1;
		bVal = nSrc;
	}
	CComVar(_In_ short nSrc) throw()
	{
		vt = VT_I2;
		iVal = nSrc;
	}
	CComVar(_In_ long nSrc, _In_ VARTYPE vtSrc = VT_I4) ATLVARIANT_THROW()
	{
		ATLASSERT(vtSrc == VT_I4 || vtSrc == VT_ERROR);
		if (vtSrc == VT_I4 || vtSrc == VT_ERROR)
		{
			vt = vtSrc;
			lVal = nSrc;
		}
		else
		{
			vt = VT_ERROR;
			scode = E_INVALIDARG;
#ifndef _ATL_NO_VARIANT_THROW
			AtlThrow(E_INVALIDARG);
#endif
		}
	}

	CComVar(_In_ float fltSrc) throw()
	{
		vt = VT_R4;
		fltVal = fltSrc;
	}
	CComVar(_In_ double dblSrc, _In_ VARTYPE vtSrc = VT_R8) ATLVARIANT_THROW()
	{
		ATLASSERT(vtSrc == VT_R8 || vtSrc == VT_DATE);
		if (vtSrc == VT_R8 || vtSrc == VT_DATE)
		{
			vt = vtSrc;
			dblVal = dblSrc;
		}
		else
		{
			vt = VT_ERROR;
			scode = E_INVALIDARG;
#ifndef _ATL_NO_VARIANT_THROW
			AtlThrow(E_INVALIDARG);
#endif
		}
	}

	CComVar(_In_ LONGLONG nSrc) throw()
	{
		vt = VT_I8;
		llVal = nSrc;
	}
	CComVar(_In_ ULONGLONG nSrc) throw()
	{
		vt = VT_UI8;
		ullVal = nSrc;
	}
	CComVar(_In_ CY cySrc) throw()
	{
		vt = VT_CY;
		cyVal.Hi = cySrc.Hi;
		cyVal.Lo = cySrc.Lo;
	}
	CComVar(_In_opt_ IDispatch* pSrc) throw()
	{
		vt = VT_DISPATCH;
		pdispVal = pSrc;
		// Need to AddRef as VariantClear will Release
		if (pdispVal != NULL)
			pdispVal->AddRef();
	}
	CComVar(_In_opt_ IUnknown* pSrc) throw()
	{
		vt = VT_UNKNOWN;
		punkVal = pSrc;
		// Need to AddRef as VariantClear will Release
		if (punkVal != NULL)
			punkVal->AddRef();
	}
	CComVar(_In_ char cSrc) throw()
	{
		vt = VT_I1;
		cVal = cSrc;
	}
	CComVar(_In_ unsigned short nSrc) throw()
	{
		vt = VT_UI2;
		uiVal = nSrc;
	}
	CComVar(_In_ unsigned long nSrc) throw()
	{
		vt = VT_UI4;
		ulVal = nSrc;
	}
	CComVar(_In_ unsigned int nSrc, _In_ VARTYPE vtSrc = VT_UI4) ATLVARIANT_THROW()
	{
		ATLASSERT(vtSrc == VT_UI4 || vtSrc == VT_UINT);
		if (vtSrc == VT_UI4 || vtSrc == VT_UINT)
		{
			vt = vtSrc;
			uintVal = nSrc;
		}
		else
		{
			vt = VT_ERROR;
			scode = E_INVALIDARG;
#ifndef _ATL_NO_VARIANT_THROW
			AtlThrow(E_INVALIDARG);
#endif
		}
	}
	CComVar(_In_ const CComBSTR& bstrSrc) ATLVARIANT_THROW()
	{
		vt = VT_EMPTY;
		*this = bstrSrc;
	}
	CComVar(_In_ const SAFEARRAY *pSrc) ATLVARIANT_THROW()
	{
		ATLASSERT(pSrc != NULL);
		if (pSrc == NULL)
		{
			vt = VT_ERROR;
			scode = E_INVALIDARG;
#ifndef _ATL_NO_VARIANT_THROW
			AtlThrow(E_INVALIDARG);
#endif
		}
		else
		{
			LPSAFEARRAY pCopy;
			HRESULT hRes = ::SafeArrayCopy((LPSAFEARRAY)pSrc, &pCopy);
			if (SUCCEEDED(hRes))
			{
				hRes = ::ATL::AtlSafeArrayGetActualVartype((LPSAFEARRAY)pSrc, &vt);
				if (SUCCEEDED(hRes))
				{
					vt |= VT_ARRAY;
					parray = pCopy;
				}
				else
				{
					vt = VT_ERROR;
					scode = hRes;
				}
			}
			else
			{
				vt = VT_ERROR;
				scode = hRes;
			}

#ifndef _ATL_NO_VARIANT_THROW
			if (FAILED(hRes))
			{
				if (hRes == E_OUTOFMEMORY)
				{
					AtlThrow(E_OUTOFMEMORY);
				}
				else
				{
					ATLENSURE_THROW(FALSE, hRes);
				}
			}
#endif
		}
	}
	// Assignment Operators
public:
	CComVar& operator=(_In_ const CComVar& varSrc) ATLVARIANT_THROW()
	{
		if (this != &varSrc)
		{
			InternalCopy(&varSrc);
		}
		return *this;
	}

	CComVar& operator=(_In_ const VARIANT& varSrc) ATLVARIANT_THROW()
	{
		if (static_cast<VARIANT *>(this) != &varSrc)
		{
			InternalCopy(&varSrc);
		}
		return *this;
	}

	CComVar& operator=(_In_ const CComBSTR& bstrSrc) ATLVARIANT_THROW()
	{
		ClearThrow();

		vt = VT_BSTR;
		bstrVal = bstrSrc.Copy();

		if (bstrVal == NULL && bstrSrc.m_str != NULL)
		{
			vt = VT_ERROR;
			scode = E_OUTOFMEMORY;
#ifndef _ATL_NO_VARIANT_THROW
			AtlThrow(E_OUTOFMEMORY);
#endif
		}

		return *this;
	}

	CComVar& operator=(_In_z_ LPCOLESTR lpszSrc) ATLVARIANT_THROW()
	{
		if (vt != VT_BSTR || bstrVal != lpszSrc)
		{
			ClearThrow();

			vt = VT_BSTR;
			bstrVal = ::SysAllocString(lpszSrc);

			if (bstrVal == NULL && lpszSrc != NULL)
			{
				vt = VT_ERROR;
				scode = E_OUTOFMEMORY;
#ifndef _ATL_NO_VARIANT_THROW
				AtlThrow(E_OUTOFMEMORY);
#endif
			}
		}
		return *this;
	}

	CComVar& operator=(_In_z_ LPCSTR lpszSrc) ATLVARIANT_THROW()
	{
		USES_CONVERSION_EX;
		ClearThrow();

		vt = VT_BSTR;
		bstrVal = ::SysAllocString(A2COLE_EX(lpszSrc, _ATL_SAFE_ALLOCA_DEF_THRESHOLD));

		if (bstrVal == NULL && lpszSrc != NULL)
		{
			vt = VT_ERROR;
			scode = E_OUTOFMEMORY;
#ifndef _ATL_NO_VARIANT_THROW
			AtlThrow(E_OUTOFMEMORY);
#endif
		}
		return *this;
	}

	CComVar& operator=(_In_ bool bSrc) ATLVARIANT_THROW()
	{
		if (vt != VT_BOOL)
		{
			ClearThrow();
			vt = VT_BOOL;
		}
		boolVal = bSrc ? TRUE : FALSE;
		return *this;
	}

	CComVar& operator=(_In_ int nSrc) ATLVARIANT_THROW()
	{
		if (vt != VT_I4)
		{
			ClearThrow();
			vt = VT_I4;
		}
		intVal = nSrc;

		return *this;
	}

	CComVar& operator=(_In_ BYTE nSrc) ATLVARIANT_THROW()
	{
		if (vt != VT_UI1)
		{
			ClearThrow();
			vt = VT_UI1;
		}
		bVal = nSrc;
		return *this;
	}

	CComVar& operator=(_In_ short nSrc) ATLVARIANT_THROW()
	{
		if (vt != VT_I2)
		{
			ClearThrow();
			vt = VT_I2;
		}
		iVal = nSrc;
		return *this;
	}

	CComVar& operator=(_In_ long nSrc) ATLVARIANT_THROW()
	{
		if (vt != VT_I4)
		{
			ClearThrow();
			vt = VT_I4;
		}
		lVal = nSrc;
		return *this;
	}

	CComVar& operator=(_In_ float fltSrc) ATLVARIANT_THROW()
	{
		if (vt != VT_R4)
		{
			ClearThrow();
			vt = VT_R4;
		}
		fltVal = fltSrc;
		return *this;
	}

	CComVar& operator=(_In_ double dblSrc) ATLVARIANT_THROW()
	{
		if (vt != VT_R8)
		{
			ClearThrow();
			vt = VT_R8;
		}
		dblVal = dblSrc;
		return *this;
	}

	CComVar& operator=(_In_ CY cySrc) ATLVARIANT_THROW()
	{
		if (vt != VT_CY)
		{
			ClearThrow();
			vt = VT_CY;
		}
		cyVal.Hi = cySrc.Hi;
		cyVal.Lo = cySrc.Lo;
		return *this;
	}

	CComVar& operator=(_Inout_opt_ IDispatch* pSrc) ATLVARIANT_THROW()
	{
		if (vt != VT_DISPATCH || pSrc != pdispVal)
		{
			ClearThrow();

			vt = VT_DISPATCH;
			pdispVal = pSrc;
			// Need to AddRef as VariantClear will Release
			if (pdispVal != NULL)
				pdispVal->AddRef();
		}
		return *this;
	}

	CComVar& operator=(_Inout_opt_ IUnknown* pSrc) ATLVARIANT_THROW()
	{
		if (vt != VT_UNKNOWN || pSrc != punkVal)
		{
			ClearThrow();

			vt = VT_UNKNOWN;
			punkVal = pSrc;

			// Need to AddRef as VariantClear will Release
			if (punkVal != NULL)
				punkVal->AddRef();
		}
		return *this;
	}

	CComVar& operator=(_In_ char cSrc) ATLVARIANT_THROW()
	{
		if (vt != VT_I1)
		{
			ClearThrow();
			vt = VT_I1;
		}
		cVal = cSrc;
		return *this;
	}

	CComVar& operator=(_In_ unsigned short nSrc) ATLVARIANT_THROW()
	{
		if (vt != VT_UI2)
		{
			ClearThrow();
			vt = VT_UI2;
		}
		uiVal = nSrc;
		return *this;
	}

	CComVar& operator=(_In_ unsigned long nSrc) ATLVARIANT_THROW()
	{
		if (vt != VT_UI4)
		{
			ClearThrow();
			vt = VT_UI4;
		}
		ulVal = nSrc;
		return *this;
	}

	CComVar& operator=(_In_ unsigned int nSrc) ATLVARIANT_THROW()
	{
		if (vt != VT_UI4)
		{
			ClearThrow();
			vt = VT_UI4;
		}
		uintVal = nSrc;
		return *this;
	}

	CComVar& operator=(_In_ BYTE* pbSrc) ATLVARIANT_THROW()
	{
		if (vt != (VT_UI1 | VT_BYREF))
		{
			ClearThrow();
			vt = VT_UI1 | VT_BYREF;
		}
		pbVal = pbSrc;
		return *this;
	}

	CComVar& operator=(_In_ short* pnSrc) ATLVARIANT_THROW()
	{
		if (vt != (VT_I2 | VT_BYREF))
		{
			ClearThrow();
			vt = VT_I2 | VT_BYREF;
		}
		piVal = pnSrc;
		return *this;
	}

#ifdef _NATIVE_WCHAR_T_DEFINED
	CComVar& operator=(_In_ USHORT* pnSrc) ATLVARIANT_THROW()
	{
		if (vt != (VT_UI2 | VT_BYREF))
		{
			ClearThrow();
			vt = VT_UI2 | VT_BYREF;
		}
		puiVal = pnSrc;
		return *this;
	}
#endif

	CComVar& operator=(_In_ int* pnSrc) ATLVARIANT_THROW()
	{
		if (vt != (VT_I4 | VT_BYREF))
		{
			ClearThrow();
			vt = VT_I4 | VT_BYREF;
		}
		pintVal = pnSrc;
		return *this;
	}

	CComVar& operator=(_In_ UINT* pnSrc) ATLVARIANT_THROW()
	{
		if (vt != (VT_UI4 | VT_BYREF))
		{
			ClearThrow();
			vt = VT_UI4 | VT_BYREF;
		}
		puintVal = pnSrc;
		return *this;
	}

	CComVar& operator=(_In_ long* pnSrc) ATLVARIANT_THROW()
	{
		if (vt != (VT_I4 | VT_BYREF))
		{
			ClearThrow();
			vt = VT_I4 | VT_BYREF;
		}
		plVal = pnSrc;
		return *this;
	}

	CComVar& operator=(_In_ ULONG* pnSrc) ATLVARIANT_THROW()
	{
		if (vt != (VT_UI4 | VT_BYREF))
		{
			ClearThrow();
			vt = VT_UI4 | VT_BYREF;
		}
		pulVal = pnSrc;
		return *this;
	}

	CComVar& operator=(_In_ LONGLONG nSrc) ATLVARIANT_THROW()
	{
		if (vt != VT_I8)
		{
			ClearThrow();
			vt = VT_I8;
		}
		llVal = nSrc;

		return *this;
	}

	CComVar& operator=(_In_ LONGLONG* pnSrc) ATLVARIANT_THROW()
	{
		if (vt != (VT_I8 | VT_BYREF))
		{
			ClearThrow();
			vt = VT_I8 | VT_BYREF;
		}
		pllVal = pnSrc;
		return *this;
	}

	CComVar& operator=(_In_ ULONGLONG nSrc) ATLVARIANT_THROW()
	{
		if (vt != VT_UI8)
		{
			ClearThrow();
			vt = VT_UI8;
		}
		ullVal = nSrc;

		return *this;
	}

	CComVar& operator=(_In_ ULONGLONG* pnSrc) ATLVARIANT_THROW()
	{
		if (vt != (VT_UI8 | VT_BYREF))
		{
			ClearThrow();
			vt = VT_UI8 | VT_BYREF;
		}
		pullVal = pnSrc;
		return *this;
	}

	CComVar& operator=(_In_ float* pfSrc) ATLVARIANT_THROW()
	{
		if (vt != (VT_R4 | VT_BYREF))
		{
			ClearThrow();
			vt = VT_R4 | VT_BYREF;
		}
		pfltVal = pfSrc;
		return *this;
	}

	CComVar& operator=(_In_ double* pfSrc) ATLVARIANT_THROW()
	{
		if (vt != (VT_R8 | VT_BYREF))
		{
			ClearThrow();
			vt = VT_R8 | VT_BYREF;
		}
		pdblVal = pfSrc;
		return *this;
	}

	CComVar& operator=(_In_ const SAFEARRAY *pSrc) ATLVARIANT_THROW()
	{
		ATLASSERT(pSrc != NULL);

		if (pSrc == NULL)
		{
			ClearThrow();
			vt = VT_ERROR;
			scode = E_INVALIDARG;
#ifndef _ATL_NO_VARIANT_THROW
			AtlThrow(E_INVALIDARG);
#endif
		}
		else if ((vt & VT_ARRAY) == 0 || pSrc != parray)
		{
			ClearThrow();
			LPSAFEARRAY pCopy;
			HRESULT hr = ::SafeArrayCopy((LPSAFEARRAY)pSrc, &pCopy);
			if (SUCCEEDED(hr))
			{
				::ATL::AtlSafeArrayGetActualVartype((LPSAFEARRAY)pSrc, &vt);
				vt |= VT_ARRAY;
				parray = pCopy;
			}
			else
			{
				vt = VT_ERROR;
				scode = hr;
#ifndef _ATL_NO_VARIANT_THROW
				if (hr == E_OUTOFMEMORY)
				{
					AtlThrow(E_OUTOFMEMORY);
				}
				else
				{
					ATLENSURE_THROW(FALSE, hr);
				}
#endif
			}
		}

		return *this;
	}

	// Comparison Operators
public:
	bool operator==(_In_ const VARIANT& varSrc) const throw()
	{
		// For backwards compatibility
		if (vt == VT_NULL && varSrc.vt == VT_NULL)
		{
			return true;
		}
		// Variants not equal if types don't match
		if (vt != varSrc.vt)
		{
			return false;
		}
		return VarCmp((VARIANT*)this, (VARIANT*)&varSrc, LOCALE_USER_DEFAULT, 0) == static_cast<HRESULT>(VARCMP_EQ);
	}

	bool operator!=(_In_ const VARIANT& varSrc) const throw()
	{
		return !operator==(varSrc);
	}

	bool operator<(_In_ const VARIANT& varSrc) const throw()
	{
		if (vt == VT_NULL && varSrc.vt == VT_NULL)
			return false;
		return VarCmp((VARIANT*)this, (VARIANT*)&varSrc, LOCALE_USER_DEFAULT, 0) == static_cast<HRESULT>(VARCMP_LT);
	}

	bool operator>(_In_ const VARIANT& varSrc) const throw()
	{
		if (vt == VT_NULL && varSrc.vt == VT_NULL)
			return false;
		return VarCmp((VARIANT*)this, (VARIANT*)&varSrc, LOCALE_USER_DEFAULT, 0) == static_cast<HRESULT>(VARCMP_GT);
	}

private:
    inline HRESULT VarCmp(
        _In_ LPVARIANT pvarLeft,
        _In_ LPVARIANT pvarRight,
        _In_ LCID lcid,
        _In_ ULONG dwFlags) const throw()
    {
        switch (vt)
        {
            case VT_I1:
                if (pvarLeft->cVal == pvarRight->cVal)
                {
                    return VARCMP_EQ;
                }
                return pvarLeft->cVal > pvarRight->cVal ? VARCMP_GT : VARCMP_LT;
            case VT_UI2:
                if (pvarLeft->uiVal == pvarRight->uiVal)
                {
                    return VARCMP_EQ;
                }
                return pvarLeft->uiVal > pvarRight->uiVal ? VARCMP_GT : VARCMP_LT;

            case VT_UI4:
                if (pvarLeft->uintVal == pvarRight->uintVal)
                {
                    return VARCMP_EQ;
                }
                return pvarLeft->uintVal > pvarRight->uintVal ? VARCMP_GT : VARCMP_LT;

            case VT_UI8:
                if (pvarLeft->ullVal == pvarRight->ullVal)
                {
                    return VARCMP_EQ;
                }
                return pvarLeft->ullVal > pvarRight->ullVal ? VARCMP_GT : VARCMP_LT;

            default:
                return ::VarCmp(pvarLeft, pvarRight, lcid, dwFlags);
        }
    }

	// Operations
public:
	HRESULT Clear()
	{
		return ::VariantClear(this);
	}
	HRESULT Copy(_In_ const VARIANT* pSrc)
	{
		return ::VariantCopy(this, const_cast<VARIANT*>(pSrc));
	}

	// copy VARIANT to BSTR
	HRESULT CopyTo(_Outptr_result_z_ BSTR *pstrDest) const
	{
		ATLASSERT(pstrDest != NULL && vt == VT_BSTR);
		HRESULT hRes = E_POINTER;
		if (pstrDest != NULL && vt == VT_BSTR)
		{
			*pstrDest = ::SysAllocStringByteLen((char*)bstrVal, ::SysStringByteLen(bstrVal));
			if (*pstrDest == NULL)
				hRes = E_OUTOFMEMORY;
			else
				hRes = S_OK;
		}
		else if (vt != VT_BSTR)
			hRes = DISP_E_TYPEMISMATCH;

		return hRes;
	}

	HRESULT Attach(_In_ VARIANT* pSrc)
	{
		if (pSrc == NULL)
			return E_INVALIDARG;

		HRESULT hr = S_OK;
		if (this != pSrc)
		{
			// Clear out the variant
			hr = Clear();
			if (SUCCEEDED(hr))
			{
				// Copy the contents and give control to CComVar
				Checked::memcpy_s(this, sizeof(CComVar), pSrc, sizeof(VARIANT));
				pSrc->vt = VT_EMPTY;
				hr = S_OK;
			}
		}
		return hr;
	}

	HRESULT Detach(_Inout_ VARIANT* pDest)
	{
		ATLASSERT(pDest != NULL);
		if (pDest == NULL)
			return E_POINTER;

		// Clear out the variant
		HRESULT hr = ::VariantClear(pDest);
		if (SUCCEEDED(hr))
		{
			// Copy the contents and remove control from CComVar
			Checked::memcpy_s(pDest, sizeof(VARIANT), this, sizeof(VARIANT));
			vt = VT_EMPTY;
			hr = S_OK;
		}
		return hr;
	}

	HRESULT ChangeType(_In_ VARTYPE vtNew, _In_opt_ const VARIANT* pSrc = NULL)
	{
		VARIANT* pVar = const_cast<VARIANT*>(pSrc);
		// Convert in place if pSrc is NULL
		if (pVar == NULL)
			pVar = this;
		// Do nothing if doing in place convert and vts not different
		return ::VariantChangeType(this, pVar, 0, vtNew);
	}

	template< typename T >
	void SetByRef(_In_ T* pT) ATLVARIANT_THROW()
	{
		ClearThrow();
		vt = CVarTypeInfo< T* >::VT;
		byref = pT;
	}

#ifdef _ATL_USE_WINAPI_FAMILY_DESKTOP_APP
	_Check_return_ HRESULT WriteToStream(_Inout_ IStream* pStream);
	_Check_return_ HRESULT WriteToStream(
		_Inout_ IStream* pStream,
		_In_ VARTYPE vtWrite)
	{
		if (vtWrite != VT_EMPTY && vtWrite != vt)
		{
			CComVar varConv;
			HRESULT hr = varConv.ChangeType(vtWrite, this);
			if (FAILED(hr))
			{
				return hr;
			}
			return varConv.WriteToStream(pStream);
		}
		return WriteToStream(pStream);
	}

	_Check_return_ HRESULT ReadFromStream(
		_Inout_ IStream* pStream,
		_In_ VARTYPE vtExpected = VT_EMPTY);

	_Check_return_ HRESULT ReadFromStream(
		_Inout_ IStream* pStream,
		_In_ VARTYPE vtExpected,
		_In_ ClassesAllowedInStream rgclsidAllowed,
		_In_ DWORD cclsidAllowed);

	// Return the size in bytes of the current contents
	ULONG GetSize() const;
	HRESULT GetSizeMax(_Out_ ULARGE_INTEGER* pcbSize) const;
#endif // _ATL_USE_WINAPI_FAMILY_DESKTOP_APP

	// Implementation
private:
	void ClearThrow() ATLVARIANT_THROW()
	{
		HRESULT hr = Clear();
		ATLASSERT(SUCCEEDED(hr));
		(hr);
#ifndef _ATL_NO_VARIANT_THROW
		if (FAILED(hr))
		{
			AtlThrow(hr);
		}
#endif
	}

public:
	_Check_return_ HRESULT InternalClear() ATLVARIANT_THROW()
	{
		HRESULT hr = Clear();
		ATLASSERT(SUCCEEDED(hr));
		if (FAILED(hr))
		{
			vt = VT_ERROR;
			scode = hr;
#ifndef _ATL_NO_VARIANT_THROW
			AtlThrow(hr);
#endif
		}
		return hr;
	}

	void InternalCopy(_In_ const VARIANT* pSrc) ATLVARIANT_THROW()
	{
		HRESULT hr = Copy(pSrc);
		if (FAILED(hr))
		{
			vt = VT_ERROR;
			scode = hr;
#ifndef _ATL_NO_VARIANT_THROW
			AtlThrow(hr);
#endif
		}
	}
};
