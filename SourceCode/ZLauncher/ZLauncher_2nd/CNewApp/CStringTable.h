#pragma once

#ifndef _CSTRINGTABLE_H_
#define _CSTRINGTABLE_H_

extern void STDestroy();

extern bool STLoadFromFile(const char* szFileName);

extern bool STLoadFromMemory(const char* szMemString);

extern const char* STGetString(const char* szName);

extern void STGetString(const char* szName, char* szString);

#endif
