/******************************************************************************
 Header: SecureEngineCustomVMsMacros.h
 Description: Definition of CustomVM macros

 Author/s: Oreans Technologies 
 (c) 2013 Oreans Technologies

 --- File generated automatically from Oreans VM Generator (14/9/2013) ---
******************************************************************************/


// ****************************************************************************
// Declaration of Custom VM macros
// ****************************************************************************

#ifdef __cplusplus
 extern "C" {
#endif

#if defined(PLATFORM_X32)

void __stdcall VM_TIGER_WHITE_START_ASM32();
void __stdcall VM_TIGER_WHITE_END_ASM32();
void __stdcall VM_TIGER_RED_START_ASM32();
void __stdcall VM_TIGER_RED_END_ASM32();
void __stdcall VM_TIGER_BLACK_START_ASM32();
void __stdcall VM_TIGER_BLACK_END_ASM32();
void __stdcall VM_FISH_WHITE_START_ASM32();
void __stdcall VM_FISH_WHITE_END_ASM32();
void __stdcall VM_FISH_RED_START_ASM32();
void __stdcall VM_FISH_RED_END_ASM32();
void __stdcall VM_FISH_BLACK_START_ASM32();
void __stdcall VM_FISH_BLACK_END_ASM32();

#define VM_TIGER_WHITE_START VM_TIGER_WHITE_START_ASM32();
#define VM_TIGER_WHITE_END VM_TIGER_WHITE_END_ASM32();
#define VM_TIGER_RED_START VM_TIGER_RED_START_ASM32();
#define VM_TIGER_RED_END VM_TIGER_RED_END_ASM32();
#define VM_TIGER_BLACK_START VM_TIGER_BLACK_START_ASM32();
#define VM_TIGER_BLACK_END VM_TIGER_BLACK_END_ASM32();
#define VM_FISH_WHITE_START VM_FISH_WHITE_START_ASM32();
#define VM_FISH_WHITE_END VM_FISH_WHITE_END_ASM32();
#define VM_FISH_RED_START VM_FISH_RED_START_ASM32();
#define VM_FISH_RED_END VM_FISH_RED_END_ASM32();
#define VM_FISH_BLACK_START VM_FISH_BLACK_START_ASM32();
#define VM_FISH_BLACK_END VM_FISH_BLACK_END_ASM32();

#endif

#if defined(PLATFORM_X64)

void __stdcall VM_TIGER_WHITE_START_ASM64();
void __stdcall VM_TIGER_WHITE_END_ASM64();
void __stdcall VM_TIGER_RED_START_ASM64();
void __stdcall VM_TIGER_RED_END_ASM64();
void __stdcall VM_TIGER_BLACK_START_ASM64();
void __stdcall VM_TIGER_BLACK_END_ASM64();
void __stdcall VM_FISH_WHITE_START_ASM64();
void __stdcall VM_FISH_WHITE_END_ASM64();
void __stdcall VM_FISH_RED_START_ASM64();
void __stdcall VM_FISH_RED_END_ASM64();
void __stdcall VM_FISH_BLACK_START_ASM64();
void __stdcall VM_FISH_BLACK_END_ASM64();

#define VM_TIGER_WHITE_START VM_TIGER_WHITE_START_ASM64();
#define VM_TIGER_WHITE_END VM_TIGER_WHITE_END_ASM64();
#define VM_TIGER_RED_START VM_TIGER_RED_START_ASM64();
#define VM_TIGER_RED_END VM_TIGER_RED_END_ASM64();
#define VM_TIGER_BLACK_START VM_TIGER_BLACK_START_ASM64();
#define VM_TIGER_BLACK_END VM_TIGER_BLACK_END_ASM64();
#define VM_FISH_WHITE_START VM_FISH_WHITE_START_ASM64();
#define VM_FISH_WHITE_END VM_FISH_WHITE_END_ASM64();
#define VM_FISH_RED_START VM_FISH_RED_START_ASM64();
#define VM_FISH_RED_END VM_FISH_RED_END_ASM64();
#define VM_FISH_BLACK_START VM_FISH_BLACK_START_ASM64();
#define VM_FISH_BLACK_END VM_FISH_BLACK_END_ASM64();

#endif

#ifdef __cplusplus
}
#endif

