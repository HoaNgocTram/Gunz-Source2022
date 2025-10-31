#ifndef XORSTR_H
#define XORSTR_H

#pragma once

// Raizel: (30/12/2020) - Xor Update.

template <unsigned int XORSTART, unsigned int BUFLEN, unsigned int XREFKILLER>
class XorStr
{
public:
	char s[BUFLEN];

	XorStr(const char* xs);
	~XorStr()
	{
		for (auto i = 0; i < BUFLEN; i++)
			s[i] = 0;
	}
private:
	XorStr();
};

template <unsigned int XORSTART, unsigned int BUFLEN, unsigned int XREFKILLER>
XorStr<XORSTART, BUFLEN, XREFKILLER>::XorStr(const char* xs)
{
	auto xvalue = XORSTART;

	for (auto i = 0; i < (BUFLEN - 1); i++)
	{
		s[i] = xs[i - XREFKILLER] ^ xvalue;
		xvalue += 1;
		xvalue %= 256;
	}
	s[BUFLEN - 1] = 0;
}
#endif // !XORSTR_H