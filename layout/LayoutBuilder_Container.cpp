#include "stdafx.h"
#include "LayoutBuilder.h"
#include "GdilPlusUtils.h"

STDMETHODIMP CLayoutBuilder::BuildContainerInternal(HDC hdc, RECT* pSourceRect, RECT* pDestRect, IVariantObject* pLayoutObject, IVariantObject* pValueObject, IImageManagerService* pImageManagerService, IColumnsInfo* pColumnInfo, IColumnsInfoItem** ppColumnsInfoItem, function<void(CComVar&, CRect&, CRect&, CRect&, CRect&)> itemAction)
{
	CRect sourceRect = *pSourceRect;
	sourceRect.MoveToX(0);
	sourceRect.MoveToY(0);

	CRect containerRect;
	containerRect.left = sourceRect.left;
	containerRect.right = sourceRect.left;
	containerRect.top = sourceRect.top;
	containerRect.bottom = sourceRect.top;

	RETURN_IF_FAILED(FitToParentStart(pLayoutObject, sourceRect, containerRect));

	CComPtr<IColumnsInfoItem> pColumnsInfoItem;
	RETURN_IF_FAILED(pColumnInfo->AddItem(&pColumnsInfoItem));
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
		CComVar vName;
		pElement->GetVariantValue(Layout::Metadata::Element::Name, &vName);
		auto str = vName.bstrVal;
#endif

		CRect elementRect;
		ElementType elementType = ElementType::UnknownValue;
		RETURN_IF_FAILED(GetElementType(pElement, &elementType));

		CRect localSourceRect = sourceRect;

		CComVar vVisible;
		RETURN_IF_FAILED(pElement->GetVariantValue(Layout::Metadata::Element::Visible, &vVisible));
		ATLASSERT(vVisible.vt == VT_BOOL);
		switch (elementType)
		{
			case ElementType::VerticalContainer:
			{
				CComPtr<IColumnsInfoItem> pColumnsInfoItemElement;
				CRect rectParent = *pSourceRect;
				RETURN_IF_FAILED(FitToParentStart(pElement, rectParent, localSourceRect));
				RETURN_IF_FAILED(CalculateRelativeWidth(pElement, localSourceRect));
				RETURN_IF_FAILED(BuildVerticalContainer(hdc, &localSourceRect, &elementRect, pElement, pValueObject, pImageManagerService, pChildItems, &pColumnsInfoItemElement));
				RETURN_IF_FAILED(FitToParentEnd(pElement, rectParent, elementRect));
				break;
			}
			case ElementType::HorizontalContainer:
			{
				CComPtr<IColumnsInfoItem> pColumnsInfoItemElement;
				CRect rectParent = *pSourceRect;
				RETURN_IF_FAILED(FitToParentStart(pElement, rectParent, localSourceRect));
				RETURN_IF_FAILED(CalculateRelativeWidth(pElement, localSourceRect));
				RETURN_IF_FAILED(BuildHorizontalContainer(hdc, &localSourceRect, &elementRect, pElement, pValueObject, pImageManagerService, pChildItems, &pColumnsInfoItemElement));
				RETURN_IF_FAILED(FitToParentEnd(pElement, rectParent, elementRect));
				break;
			}
			case ElementType::TextColumn:
			{
				CComPtr<IColumnsInfoItem> pColumnsInfoItemElement;
				RETURN_IF_FAILED(ApplyStartMargins(pElement, localSourceRect));
				CRect rectParent = *pSourceRect;
				RETURN_IF_FAILED(FitToParentStart(pElement, rectParent, localSourceRect));
				RETURN_IF_FAILED(CalculateRelativeWidth(pElement, localSourceRect));
				RETURN_IF_FAILED(BuildTextColumn(hdc, &localSourceRect, &elementRect, pElement, pValueObject, pChildItems, &pColumnsInfoItemElement));
				RETURN_IF_FAILED(ApplyEndMargins(pElement, elementRect));
				CRect elementRectWithMargins = elementRect;
				RETURN_IF_FAILED(FitToParentStart(pElement, rectParent, elementRect));
				if (elementRectWithMargins != elementRect)
				{
					RETURN_IF_FAILED(pColumnsInfoItemElement->SetRect(elementRect));
				}
				break;
			}
			case ElementType::ImageColumn:
			{
				CComPtr<IColumnsInfoItem> pColumnsInfoItemElement;
				RETURN_IF_FAILED(ApplyStartMargins(pElement, localSourceRect));
				CRect rectParent = *pSourceRect;
				RETURN_IF_FAILED(FitToParentStart(pElement, rectParent, localSourceRect));
				RETURN_IF_FAILED(CalculateRelativeWidth(pElement, localSourceRect));
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
				CComPtr<IColumnsInfoItem> pColumnsInfoItemElement;
				RETURN_IF_FAILED(ApplyStartMargins(pElement, localSourceRect));
				CRect rectParent = *pSourceRect;
				RETURN_IF_FAILED(FitToParentStart(pElement, rectParent, localSourceRect));
				RETURN_IF_FAILED(CalculateRelativeWidth(pElement, localSourceRect));
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

		itemAction(vVisible, sourceRect, localSourceRect, elementRect, containerRect);
	}

	CRect parentRect = *pSourceRect;
	containerRect.OffsetRect(parentRect.left, parentRect.top);

	RETURN_IF_FAILED(CalculateRelativeWidth(pLayoutObject, containerRect));
	RETURN_IF_FAILED(ApplyStartMargins(pLayoutObject, containerRect));
	RETURN_IF_FAILED(ApplyEndMargins(pLayoutObject, containerRect));
	RETURN_IF_FAILED(FitToParentEnd(pLayoutObject, parentRect, containerRect));
	RETURN_IF_FAILED(ApplyAlignHorizontal(pChildItems, parentRect, containerRect));
	RETURN_IF_FAILED(ApplyAlignVertical(pChildItems, parentRect, containerRect));

	RETURN_IF_FAILED(SetColumnProps(pLayoutObject, pColumnsInfoItem));
	RETURN_IF_FAILED(pColumnsInfoItem->SetRect(containerRect));
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
			[&](CComVar& vVisible, CRect& sourceRect, CRect& localSourceRect, CRect& elementRect, CRect& containerRect)
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
			[&](CComVar& vVisible, CRect& sourceRect, CRect& localSourceRect, CRect& elementRect, CRect& containerRect)
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
	));
	return S_OK;
}