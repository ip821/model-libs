#include "stdafx.h"
#include "ThemeColorMap.h"

map<wstring, DWORD> CThemeColorMap::m_knownColors =
{
	{ L"AliceBlue", 0xFFF0F8FF },
	{ L"AntiqueWhite", 0xFFFAEBD7 },
	{ L"Aqua", 0xFF00FFFF },
	{ L"Aquamarine", 0xFF7FFFD4 },
	{ L"Azure", 0xFFF0FFFF },
	{ L"Beige", 0xFFF5F5DC },
	{ L"Bisque", 0xFFFFE4C4 },
	{ L"Black", 0xFF000000 },
	{ L"BlanchedAlmond", 0xFFFFEBCD },
	{ L"Blue", 0xFF0000FF },
	{ L"BlueViolet", 0xFF8A2BE2 },
	{ L"Brown", 0xFFA52A2A },
	{ L"BurlyWood", 0xFFDEB887 },
	{ L"CadetBlue", 0xFF5F9EA0 },
	{ L"Chartreuse", 0xFF7FFF00 },
	{ L"Chocolate", 0xFFD2691E },
	{ L"Coral", 0xFFFF7F50 },
	{ L"CornflowerBlue", 0xFF6495ED },
	{ L"Cornsilk", 0xFFFFF8DC },
	{ L"Crimson", 0xFFDC143C },
	{ L"Cyan", 0xFF00FFFF },
	{ L"DarkBlue", 0xFF00008B },
	{ L"DarkCyan", 0xFF008B8B },
	{ L"DarkGoldenrod", 0xFFB8860B },
	{ L"DarkGray", 0xFFA9A9A9 },
	{ L"DarkGreen", 0xFF006400 },
	{ L"DarkKhaki", 0xFFBDB76B },
	{ L"DarkMagenta", 0xFF8B008B },
	{ L"DarkOliveGreen", 0xFF556B2F },
	{ L"DarkOrange", 0xFFFF8C00 },
	{ L"DarkOrchid", 0xFF9932CC },
	{ L"DarkRed", 0xFF8B0000 },
	{ L"DarkSalmon", 0xFFE9967A },
	{ L"DarkSeaGreen", 0xFF8FBC8B },
	{ L"DarkSlateBlue", 0xFF483D8B },
	{ L"DarkSlateGray", 0xFF2F4F4F },
	{ L"DarkTurquoise", 0xFF00CED1 },
	{ L"DarkViolet", 0xFF9400D3 },
	{ L"DeepPink", 0xFFFF1493 },
	{ L"DeepSkyBlue", 0xFF00BFFF },
	{ L"DimGray", 0xFF696969 },
	{ L"DodgerBlue", 0xFF1E90FF },
	{ L"Firebrick", 0xFFB22222 },
	{ L"FloralWhite", 0xFFFFFAF0 },
	{ L"ForestGreen", 0xFF228B22 },
	{ L"Fuchsia", 0xFFFF00FF },
	{ L"Gainsboro", 0xFFDCDCDC },
	{ L"GhostWhite", 0xFFF8F8FF },
	{ L"Gold", 0xFFFFD700 },
	{ L"Goldenrod", 0xFFDAA520 },
	{ L"Gray", 0xFF808080 },
	{ L"Green", 0xFF008000 },
	{ L"GreenYellow", 0xFFADFF2F },
	{ L"Honeydew", 0xFFF0FFF0 },
	{ L"HotPink", 0xFFFF69B4 },
	{ L"IndianRed", 0xFFCD5C5C },
	{ L"Indigo", 0xFF4B0082 },
	{ L"Ivory", 0xFFFFFFF0 },
	{ L"Khaki", 0xFFF0E68C },
	{ L"Lavender", 0xFFE6E6FA },
	{ L"LavenderBlush", 0xFFFFF0F5 },
	{ L"LawnGreen", 0xFF7CFC00 },
	{ L"LemonChiffon", 0xFFFFFACD },
	{ L"LightBlue", 0xFFADD8E6 },
	{ L"LightCoral", 0xFFF08080 },
	{ L"LightCyan", 0xFFE0FFFF },
	{ L"LightGoldenrodYellow", 0xFFFAFAD2 },
	{ L"LightGray", 0xFFD3D3D3 },
	{ L"LightGreen", 0xFF90EE90 },
	{ L"LightPink", 0xFFFFB6C1 },
	{ L"LightSalmon", 0xFFFFA07A },
	{ L"LightSeaGreen", 0xFF20B2AA },
	{ L"LightSkyBlue", 0xFF87CEFA },
	{ L"LightSlateGray", 0xFF778899 },
	{ L"LightSteelBlue", 0xFFB0C4DE },
	{ L"LightYellow", 0xFFFFFFE0 },
	{ L"Lime", 0xFF00FF00 },
	{ L"LimeGreen", 0xFF32CD32 },
	{ L"Linen", 0xFFFAF0E6 },
	{ L"Magenta", 0xFFFF00FF },
	{ L"Maroon", 0xFF800000 },
	{ L"MediumAquamarine", 0xFF66CDAA },
	{ L"MediumBlue", 0xFF0000CD },
	{ L"MediumOrchid", 0xFFBA55D3 },
	{ L"MediumPurple", 0xFF9370DB },
	{ L"MediumSeaGreen", 0xFF3CB371 },
	{ L"MediumSlateBlue", 0xFF7B68EE },
	{ L"MediumSpringGreen", 0xFF00FA9A },
	{ L"MediumTurquoise", 0xFF48D1CC },
	{ L"MediumVioletRed", 0xFFC71585 },
	{ L"MidnightBlue", 0xFF191970 },
	{ L"MintCream", 0xFFF5FFFA },
	{ L"MistyRose", 0xFFFFE4E1 },
	{ L"Moccasin", 0xFFFFE4B5 },
	{ L"NavajoWhite", 0xFFFFDEAD },
	{ L"Navy", 0xFF000080 },
	{ L"OldLace", 0xFFFDF5E6 },
	{ L"Olive", 0xFF808000 },
	{ L"OliveDrab", 0xFF6B8E23 },
	{ L"Orange", 0xFFFFA500 },
	{ L"OrangeRed", 0xFFFF4500 },
	{ L"Orchid", 0xFFDA70D6 },
	{ L"PaleGoldenrod", 0xFFEEE8AA },
	{ L"PaleGreen", 0xFF98FB98 },
	{ L"PaleTurquoise", 0xFFAFEEEE },
	{ L"PaleVioletRed", 0xFFDB7093 },
	{ L"PapayaWhip", 0xFFFFEFD5 },
	{ L"PeachPuff", 0xFFFFDAB9 },
	{ L"Peru", 0xFFCD853F },
	{ L"Pink", 0xFFFFC0CB },
	{ L"Plum", 0xFFDDA0DD },
	{ L"PowderBlue", 0xFFB0E0E6 },
	{ L"Purple", 0xFF800080 },
	{ L"Red", 0xFFFF0000 },
	{ L"RosyBrown", 0xFFBC8F8F },
	{ L"RoyalBlue", 0xFF4169E1 },
	{ L"SaddleBrown", 0xFF8B4513 },
	{ L"Salmon", 0xFFFA8072 },
	{ L"SandyBrown", 0xFFF4A460 },
	{ L"SeaGreen", 0xFF2E8B57 },
	{ L"SeaShell", 0xFFFFF5EE },
	{ L"Sienna", 0xFFA0522D },
	{ L"Silver", 0xFFC0C0C0 },
	{ L"SkyBlue", 0xFF87CEEB },
	{ L"SlateBlue", 0xFF6A5ACD },
	{ L"SlateGray", 0xFF708090 },
	{ L"Snow", 0xFFFFFAFA },
	{ L"SpringGreen", 0xFF00FF7F },
	{ L"SteelBlue", 0xFF4682B4 },
	{ L"Tan", 0xFFD2B48C },
	{ L"Teal", 0xFF008080 },
	{ L"Thistle", 0xFFD8BFD8 },
	{ L"Tomato", 0xFFFF6347 },
	{ L"Transparent", 0x00FFFFFF },
	{ L"Turquoise", 0xFF40E0D0 },
	{ L"Violet", 0xFFEE82EE },
	{ L"Wheat", 0xFFF5DEB3 },
	{ L"White", 0xFFFFFFFF },
	{ L"WhiteSmoke", 0xFFF5F5F5 },
	{ L"Yellow", 0xFFFFFF00 },
	{ L"YellowGreen", 0xFF9ACD32 }
};