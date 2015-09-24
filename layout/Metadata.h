#pragma once

namespace IP
{
	namespace Layout
	{
		namespace Metadata
		{
			namespace Column
			{
				const static CComBSTR Name(L"name");
			}

			namespace ImageColumn
			{
				const static CComBSTR ImageOverlay(L"image_overlay");
				const static CComBSTR ImageKey(L"image");
				const static CComBSTR ImageStyle(L"image_style");
				const static CComBSTR Height(L"height");
				const static CComBSTR Width(L"width");
				const static CComBSTR Alpha(L"alpha");
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
				const static CComBSTR Multiline(L"text_multiline");
				const static CComBSTR TextKey(L"text_key");
				const static CComBSTR TextFullKey(L"text_full_key");
				const static CComBSTR Font(L"font");
				const static CComBSTR FontSelected(L"font_selected");
				const static CComBSTR FontDisabled(L"font_disabled");
			}

			namespace TextMultiColumn
			{
				const static CComBSTR ColumnDefinition(L"column_definition_key");
				const static CComBSTR ColumnDefinitions(L"column_definitions");
				const static CComBSTR ColumnStartIndex(L"StartIndex");
				const static CComBSTR ColumnEndIndex(L"EndIndex");
			}

			namespace Element
			{
				const static CComBSTR Name(L"name");
				const static CComBSTR Alpha(L"alpha");
				const static CComBSTR Elements(L"elements");
				const static CComBSTR Type(L"type");
				const static CComBSTR Visible(L"visible");
				const static CComBSTR Selected(L"selected");
				const static CComBSTR Disabled(L"disabled");
				const static CComBSTR Color(L"color");
				const static CComBSTR ColorSelected(L"color_selected");
				const static CComBSTR ColorDisabled(L"color_disabled");
				const static CComBSTR ColorBackground(L"color_background");
				const static CComBSTR ColorBackgroundSelected(L"color_background_selected");
				const static CComBSTR ColorBackgroundDisabled(L"color_background_disabled");
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
				const static CComBSTR BorderLeft(L"border_left");
				const static CComBSTR BorderLeftColor(L"border_left_color");
				const static CComBSTR BorderLeftWidth(L"border_left_width");
				const static CComBSTR BorderRight(L"border_right");
				const static CComBSTR BorderRightColor(L"border_right_color");
				const static CComBSTR BorderRightWidth(L"border_right_width");
				const static CComBSTR AlignHorizontal(L"align_horizontal");
				const static CComBSTR AlignVertical(L"align_vertical");
				const static CComBSTR WidthPercent(L"width_percent");
			}

			namespace ImageStyles
			{
				const static CComBSTR Fill(L"fill");
			}

			namespace BackgroundStyles
			{
				const static CComBSTR Rounded(L"rounded");
				const static CComBSTR RoundedStrict(L"rounded_strict");
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
				const static CComBSTR Up(L"up");
			}

			namespace LayoutTypes
			{
				const static CComBSTR HorizontalContainer(L"horizontal_container");
				const static CComBSTR VerticalContainer(L"vertical_container");
				const static CComBSTR ImageColumn(L"image_column");
				const static CComBSTR TextColumn(L"text_column");
				const static CComBSTR TextMultiColumn(L"text_multi_column");
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