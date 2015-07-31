#include "stdafx.h"
#include "LayoutBuilder.h"
#include "GdilPlusUtils.h"

STDMETHODIMP CLayoutBuilder::BuildHorizontalContainer(HDC hdc, RECT* pSourceRect, RECT* pDestRect, IVariantObject* pLayoutObject, IVariantObject* pValueObject, IImageManagerService* pImageManagerService, IColumnsInfo* pColumnInfo, IColumnsInfoItem** ppColumnsInfoItem)
{
	CRect sourceRect = *pSourceRect;
	sourceRect.MoveToX(0);
	sourceRect.MoveToY(0);

	CRect containerRect;
	containerRect.left = sourceRect.left;
	containerRect.right = sourceRect.left;
	containerRect.top = sourceRect.top;

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

		CRect elementRect;
		ElementType elementType = ElementType::UnknownValue;
		RETURN_IF_FAILED(GetElementType(pElement, &elementType));

		CRect localSourceRect = sourceRect;

		CComVar vVisible;
		RETURN_IF_FAILED(pElement->GetVariantValue(Layout::Metadata::Element::Visible, &vVisible));
		ATLASSERT(vVisible.vt == VT_BOOL);
		switch (elementType)
		{
			case ElementType::HorizontalContainer:
			{
				CComPtr<IColumnsInfoItem> pColumnsInfoItemElement;
				RETURN_IF_FAILED(BuildHorizontalContainer(hdc, &localSourceRect, &elementRect, pElement, pValueObject, pImageManagerService, pChildItems, &pColumnsInfoItemElement));
				break;
			}
			case ElementType::TextColumn:
			{
				CComPtr<IColumnsInfoItem> pColumnsInfoItemElement;
				RETURN_IF_FAILED(ApplyStartMargins(pElement, localSourceRect));
				RETURN_IF_FAILED(BuildTextColumn(hdc, &localSourceRect, &elementRect, pElement, pValueObject, pChildItems, &pColumnsInfoItemElement));
				RETURN_IF_FAILED(ApplyEndMargins(pElement, elementRect));
				RETURN_IF_FAILED(FitToParent(pElement, localSourceRect, elementRect));
				RETURN_IF_FAILED(CenterToParent(pElement, localSourceRect, elementRect));
				RETURN_IF_FAILED(pColumnsInfoItemElement->SetRect(elementRect));
				break;
			}
			case ElementType::ImageColumn:
			{
				CComPtr<IColumnsInfoItem> pColumnsInfoItemElement;
				RETURN_IF_FAILED(ApplyStartMargins(pElement, localSourceRect));
				RETURN_IF_FAILED(BuildImageColumn(hdc, &localSourceRect, &elementRect, pElement, pValueObject, pImageManagerService, pChildItems, &pColumnsInfoItemElement));
				RETURN_IF_FAILED(ApplyEndMargins(pElement, elementRect));
				RETURN_IF_FAILED(FitToParent(pElement, localSourceRect, elementRect));
				RETURN_IF_FAILED(CenterToParent(pElement, localSourceRect, elementRect));
				RETURN_IF_FAILED(pColumnsInfoItemElement->SetRect(elementRect));
				break;
			}
			case ElementType::MarqueeProgressColumn:
			{
				CComPtr<IColumnsInfoItem> pColumnsInfoItemElement;
				RETURN_IF_FAILED(ApplyStartMargins(pElement, localSourceRect));
				RETURN_IF_FAILED(BuildMarqueeProgressColumn(hdc, &localSourceRect, &elementRect, pElement, pValueObject, pChildItems, &pColumnsInfoItemElement));
				RETURN_IF_FAILED(ApplyEndMargins(pElement, elementRect));
				RETURN_IF_FAILED(FitToParent(pElement, localSourceRect, elementRect));
				RETURN_IF_FAILED(CenterToParent(pElement, localSourceRect, elementRect));
				RETURN_IF_FAILED(pColumnsInfoItemElement->SetRect(elementRect));
				break;
			}
		}

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

	CRect parentRect = *pSourceRect;
	containerRect.OffsetRect(parentRect.left, parentRect.top);

	RETURN_IF_FAILED(ApplyStartMargins(pLayoutObject, containerRect));
	RETURN_IF_FAILED(ApplyEndMargins(pLayoutObject, containerRect));
	RETURN_IF_FAILED(FitToParent(pLayoutObject, parentRect, containerRect));
	RETURN_IF_FAILED(CenterToParent(pLayoutObject, parentRect, containerRect));
	RETURN_IF_FAILED(ApplyRightAlign(pChildItems, parentRect, containerRect));

	RETURN_IF_FAILED(SetColumnProps(pLayoutObject, pColumnsInfoItem));
	RETURN_IF_FAILED(pColumnsInfoItem->SetRect(containerRect));
	*pDestRect = containerRect;
	return S_OK;
}
