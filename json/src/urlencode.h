#ifndef __URLENCODE_H__
#define __URLENCODE_H__

#include <iostream>
#include <string>

std::string char2hex( char dec );
std::wstring char2hex(wchar_t dec);
std::string urlencode( const std::string &c );
std::wstring urlencodew(const std::wstring &c);

#endif // __URLENCODE_H__