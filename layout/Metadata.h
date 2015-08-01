#pragma once

namespace IP
{
	namespace Layout
	{
		namespace Metadata
		{
			namespace Column
			{
				const static CComBSTR Name(L"ColumnName");
			}

			namespace ImageColumn
			{
				const static CComBSTR ImageKey(L"image");
			}

			namespace MarqueeProgressColumn
			{
				const static CComBSTR ItemSize(L"item_size");
				const static CComBSTR ItemDistance(L"item_distance");
				const static CComBSTR ItemCount(L"item_count");
				const static CComBSTR ColorActive(L"color_active");
				const static CComBSTR ColorInactive(L"color_inactive");
				const static CComBSTR Value(L"value");
			}

			namespace TextColumn
			{
				const static CComBSTR Text(L"text");
				const static CComBSTR Font(L"font");
				const static CComBSTR Color(L"color");
				const static CComBSTR ColorSelected(L"color_selected");
			}

			namespace Element
			{
				const static CComBSTR Name(L"name");
				const static CComBSTR Elements(L"elements");
				const static CComBSTR Type(L"type");
				const static CComBSTR Visible(L"visible");
				const static CComBSTR Selected(L"selected");
				const static CComBSTR ColorBackground(L"color_background");
				const static CComBSTR BackgroundStyle(L"background_style");
				const static CComBSTR MarginLeft(L"margin_left");
				const static CComBSTR MarginRight(L"margin_right");
				const static CComBSTR MarginTop(L"margin_top");
				const static CComBSTR MarginBottom(L"margin_bottom");
				const static CComBSTR FitHorizontal(L"fit_horizontal");
				const static CComBSTR FitVertical(L"fit_vertical");
				const static CComBSTR BorderBottom(L"border_bottom");
				const static CComBSTR BorderBottomColor(L"border_bottom_color");
				const static CComBSTR BorderBottomWidth(L"border_bottom_width");
				const static CComBSTR BorderTop(L"border_top");
				const static CComBSTR BorderTopColor(L"border_top_color");
				const static CComBSTR BorderTopWidth(L"border_top_width");
				const static CComBSTR AlignHorizontal(L"align_horizontal");
				const static CComBSTR AlignVertical(L"align_vertical");
			}

			namespace BackgroundStyles
			{
				const static CComBSTR Rounded(L"rounded");
			}

			namespace AlignHorizontalTypes
			{
				const static CComBSTR Right(L"right");
				const static CComBSTR Center(L"center");
			}

			namespace AlignVerticalTypes
			{
				const static CComBSTR Down(L"down");
				const static CComBSTR Center(L"center");
			}

			namespace LayoutTypes
			{
				const static CComBSTR HorizontalContainer(L"horizontal_container");
				const static CComBSTR VerticalContainer(L"vertical_container");
				const static CComBSTR ImageColumn(L"image_column");
				const static CComBSTR TextColumn(L"text_column");
				const static CComBSTR MarqueeProgressColumn(L"marquee_progress");
			}

			namespace FitTypes
			{
				const static CComBSTR Content(L"content");
				const static CComBSTR Parent(L"parent");
			}
		}
	}
}