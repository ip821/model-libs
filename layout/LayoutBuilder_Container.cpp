#include "stdafx.h"
#include "LayoutBuilder.h"
#include "GdilPlusUtils.h"
#include "..\layout\Functions.h"

STDMETHODIMP CLayoutBuilder::BuildContainerInternal(HDC hdc, RECT* pSourceRect, RECT* pDestRect, IVariantObject* pLayoutObject, IVariantObject* pValueObject, IImageManagerService* pImageManagerService, IColumnsInfo* pColumnInfo, IColumnsInfoItem** ppColumnsInfoItem, ElementType containerElementType)
{
	CRect sourceRect = *pSourceRect;
	sourceRect.MoveToX(0);
	sourceRect.MoveToY(0);

	CRect containerRect;
	containerRect.left = sourceRect.left;
	containerRect.right = sourceRect.left;
	containerRect.top = sourceRect.top;
	containerRect.bottom = sourceRect.top;

#ifdef DEBUG
	{
		CComVar vName;
		pLayoutObject->GetVariantValue(Layout::Metadata::Element::Name, &vName);
		auto str = CString(vName.bstrVal);
		if (str == L"UserNameContainer")
		{
			str = vName.bstrVal;
		}
	}
#endif

	RETURN_IF_FAILED(FitToParentStart(pLayoutObject, sourceRect, containerRect));

	CComPtr<IColumnsInfoItem> pColumnsInfoItem;
	RETURN_IF_FAILED(pColumnInfo->AddItem(pLayoutObject, &pColumnsInfoItem));
	RETURN_IF_FAILED(pColumnsInfoItem->QueryInterface(ppColumnsInfoItem));

	CComPtr<IColumnsInfo> pChildItems;
	RETURN_IF_FAILED(pColumnsInfoItem->GetChildItems(&pChildItems));

	CComPtr<IObjArray> pElements;
	RETURN_IF_FAILED(GetElements(pLayoutObject, &pElements));
	UINT uiCount = 0;
	RETURN_IF_FAILED(pElements->GetCount(&uiCount));
	for (size_t i = 0; i < uiCount; i++)
	{
		CComPtr<IVariantObject> pElement;
		RETURN_IF_FAILED(pElements->GetAt(i, __uuidof(IVariantObject), (LPVOID*)&pElement));

#ifdef DEBUG
		{
			CComVar vName;
			pElement->GetVariantValue(Layout::Metadata::Element::Name, &vName);
			auto str = CString(vName.bstrVal);

			if (str == L"UserDisplayName")
			{
				str = vName.bstrVal;
			}
		}
#endif

		CRect elementRect;
		ElementType elementType = ElementType::UnknownValue;
		RETURN_IF_FAILED(GetElementType(pElement, &elementType));

		CRect localSourceRect = sourceRect;

		CComVar vVisible;
		RETURN_IF_FAILED(pElement->GetVariantValue(Layout::Metadata::Element::Visible, &vVisible));
		ATLASSERT(vVisible.vt == VT_BOOL);

		CComPtr<IColumnsInfoItem> pColumnsInfoItemElement;

		switch (elementType)
		{
			case ElementType::VerticalContainer:
			{
				CRect rectParent = *pSourceRect;
				RETURN_IF_FAILED(FitToParentStart(pElement, rectParent, localSourceRect));
				RETURN_IF_FAILED(CalculateRelativeWidth(pElement, localSourceRect, rectParent));
				RETURN_IF_FAILED(BuildVerticalContainer(hdc, &localSourceRect, &elementRect, pElement, pValueObject, pImageManagerService, pChildItems, &pColumnsInfoItemElement));
				RETURN_IF_FAILED(FitToParentEnd(pElement, rectParent, elementRect));
				break;
			}
			case ElementType::HorizontalContainer:
			{
				CRect rectParent = *pSourceRect;
				RETURN_IF_FAILED(FitToParentStart(pElement, rectParent, localSourceRect));
				RETURN_IF_FAILED(CalculateRelativeWidth(pElement, localSourceRect, rectParent));
				RETURN_IF_FAILED(BuildHorizontalContainer(hdc, &localSourceRect, &elementRect, pElement, pValueObject, pImageManagerService, pChildItems, &pColumnsInfoItemElement));
				RETURN_IF_FAILED(FitToParentEnd(pElement, rectParent, elementRect));
				break;
			}
			case ElementType::TextMultiColumn:
			{
				CRect rectParent = *pSourceRect;
				RETURN_IF_FAILED(FitToParentStart(pElement, rectParent, localSourceRect));
				RETURN_IF_FAILED(FitToParentEnd(pElement, rectParent, localSourceRect));
				RETURN_IF_FAILED(ApplyStartMargins(pElement, localSourceRect));
				RETURN_IF_FAILED(CalculateRelativeWidth(pElement, localSourceRect, rectParent));
				RETURN_IF_FAILED(BuildTextMultiColumn(hdc, &localSourceRect, &elementRect, pElement, pValueObject, pChildItems, &pColumnsInfoItemElement));
				RETURN_IF_FAILED(ApplyEndMargins(pElement, elementRect));
				CRect elementRectWithMargins = elementRect;
				if (elementRectWithMargins != elementRect)
				{
					RETURN_IF_FAILED(pColumnsInfoItemElement->SetRect(elementRect));
				}
				break;
			}
			case ElementType::TextColumn:
			{
				CRect rectParent = *pSourceRect;
				RETURN_IF_FAILED(FitToParentStart(pElement, rectParent, localSourceRect));
				RETURN_IF_FAILED(ApplyStartMargins(pElement, localSourceRect));
				RETURN_IF_FAILED(CalculateRelativeWidth(pElement, localSourceRect, rectParent));
				RETURN_IF_FAILED(BuildTextColumn(hdc, &localSourceRect, &elementRect, pElement, pValueObject, pChildItems, &pColumnsInfoItemElement));
				RETURN_IF_FAILED(ApplyEndMargins(pElement, elementRect));
				CRect elementRectWithMargins = elementRect;
				RETURN_IF_FAILED(FitToParentEnd(pElement, rectParent, elementRect));
				if (elementRectWithMargins != elementRect)
				{
					RETURN_IF_FAILED(pColumnsInfoItemElement->SetRect(elementRect));
				}
				break;
			}
			case ElementType::ImageColumn:
			{
				CRect rectParent = *pSourceRect;
				RETURN_IF_FAILED(FitToParentStart(pElement, rectParent, localSourceRect));
				RETURN_IF_FAILED(ApplyStartMargins(pElement, localSourceRect));
				RETURN_IF_FAILED(CalculateRelativeWidth(pElement, localSourceRect, rectParent));
				RETURN_IF_FAILED(BuildImageColumn(hdc, &localSourceRect, &elementRect, pElement, pValueObject, pImageManagerService, pChildItems, &pColumnsInfoItemElement));
				RETURN_IF_FAILED(ApplyEndMargins(pElement, elementRect));
				CRect elementRectWithMargins = elementRect;
				RETURN_IF_FAILED(FitToParentEnd(pElement, rectParent, elementRect));
				if (elementRectWithMargins != elementRect)
				{
					RETURN_IF_FAILED(pColumnsInfoItemElement->SetRect(elementRect));
				}
				break;
			}
			case ElementType::MarqueeProgressColumn:
			{
				CRect rectParent = *pSourceRect;
				RETURN_IF_FAILED(FitToParentStart(pElement, rectParent, localSourceRect));
				RETURN_IF_FAILED(ApplyStartMargins(pElement, localSourceRect));
				RETURN_IF_FAILED(CalculateRelativeWidth(pElement, localSourceRect, rectParent));
				RETURN_IF_FAILED(BuildMarqueeProgressColumn(hdc, &localSourceRect, &elementRect, pElement, pValueObject, pChildItems, &pColumnsInfoItemElement));
				RETURN_IF_FAILED(ApplyEndMargins(pElement, elementRect));
				CRect elementRectWithMargins = elementRect;
				RETURN_IF_FAILED(FitToParentEnd(pElement, rectParent, elementRect));
				if (elementRectWithMargins != elementRect)
				{
					RETURN_IF_FAILED(pColumnsInfoItemElement->SetRect(elementRect));
				}
				break;
			}
		}

#ifdef DEBUG
		if (pColumnsInfoItemElement)
		{
			pColumnsInfoItemElement->SetVariantValueRecursive(Layout::Metadata::Element::BorderTop, &CComVar(true));
			pColumnsInfoItemElement->SetVariantValueRecursive(Layout::Metadata::Element::BorderTop, &CComVar(true));
			pColumnsInfoItemElement->SetVariantValueRecursive(Layout::Metadata::Element::BorderTopWidth, &CComVar(1));
			pColumnsInfoItemElement->SetVariantValueRecursive(Layout::Metadata::Element::BorderTopColor, &CComVar(L"TwitterDelimiter"));
			pColumnsInfoItemElement->SetVariantValueRecursive(Layout::Metadata::Element::BorderBottom, &CComVar(true));
			pColumnsInfoItemElement->SetVariantValueRecursive(Layout::Metadata::Element::BorderBottomWidth, &CComVar(1));
			pColumnsInfoItemElement->SetVariantValueRecursive(Layout::Metadata::Element::BorderBottomColor, &CComVar(L"TwitterDelimiter"));
			pColumnsInfoItemElement->SetVariantValueRecursive(Layout::Metadata::Element::BorderLeft, &CComVar(true));
			pColumnsInfoItemElement->SetVariantValueRecursive(Layout::Metadata::Element::BorderLeftWidth, &CComVar(1));
			pColumnsInfoItemElement->SetVariantValueRecursive(Layout::Metadata::Element::BorderLeftColor, &CComVar(L"TwitterDelimiter"));
			pColumnsInfoItemElement->SetVariantValueRecursive(Layout::Metadata::Element::BorderRight, &CComVar(true));
			pColumnsInfoItemElement->SetVariantValueRecursive(Layout::Metadata::Element::BorderRightWidth, &CComVar(1));
			pColumnsInfoItemElement->SetVariantValueRecursive(Layout::Metadata::Element::BorderRightColor, &CComVar(L"TwitterDelimiter"));
		}
#endif

		if (containerElementType == ElementType::HorizontalContainer)
		{
			if (vVisible.boolVal)
			{
				sourceRect.left = elementRect.right;
				containerRect.right = elementRect.right;
				containerRect.bottom = max(elementRect.bottom, containerRect.bottom);
			}
			else
			{
				elementRect = localSourceRect;
				elementRect.right = elementRect.left;
				elementRect.bottom = elementRect.top;
			}
		}
		else if (containerElementType == ElementType::VerticalContainer)
		{
			if (vVisible.boolVal)
			{
				sourceRect.top = elementRect.bottom;
				containerRect.bottom = elementRect.bottom;
				containerRect.right = max(elementRect.right, containerRect.right);
			}
			else
			{
				elementRect = localSourceRect;
				elementRect.bottom = elementRect.top;
				elementRect.right = elementRect.left;
			}
		}
	}

	CRect parentRect = *pSourceRect;
	containerRect.OffsetRect(parentRect.left, parentRect.top);

	RETURN_IF_FAILED(CalculateRelativeWidth(pLayoutObject, containerRect, parentRect));
	RETURN_IF_FAILED(FitToParentEnd(pLayoutObject, parentRect, containerRect));
	RETURN_IF_FAILED(ApplyStartMargins(pLayoutObject, containerRect));
	RETURN_IF_FAILED(ApplyAlignHorizontal(pChildItems, containerRect, containerElementType));
	RETURN_IF_FAILED(ApplyAlignVertical(pChildItems, containerRect, containerElementType));

	RETURN_IF_FAILED(pColumnsInfoItem->SetRect(containerRect));
	RETURN_IF_FAILED(ApplyEndMargins(pLayoutObject, containerRect));
	*pDestRect = containerRect;
	return S_OK;
}

STDMETHODIMP CLayoutBuilder::BuildHorizontalContainer(HDC hdc, RECT* pSourceRect, RECT* pDestRect, IVariantObject* pLayoutObject, IVariantObject* pValueObject, IImageManagerService* pImageManagerService, IColumnsInfo* pColumnInfo, IColumnsInfoItem** ppColumnsInfoItem)
{
	RETURN_IF_FAILED(
		BuildContainerInternal(
			hdc,
			pSourceRect,
			pDestRect,
			pLayoutObject,
			pValueObject,
			pImageManagerService,
			pColumnInfo,
			ppColumnsInfoItem,
			ElementType::HorizontalContainer
			));
	return S_OK;
}

STDMETHODIMP CLayoutBuilder::BuildVerticalContainer(HDC hdc, RECT* pSourceRect, RECT* pDestRect, IVariantObject* pLayoutObject, IVariantObject* pValueObject, IImageManagerService* pImageManagerService, IColumnsInfo* pColumnInfo, IColumnsInfoItem** ppColumnsInfoItem)
{
	RETURN_IF_FAILED(
		BuildContainerInternal(
			hdc,
			pSourceRect,
			pDestRect,
			pLayoutObject,
			pValueObject,
			pImageManagerService,
			pColumnInfo,
			ppColumnsInfoItem,
			ElementType::VerticalContainer
			));
	return S_OK;
}