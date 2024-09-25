/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "xprecompiled.h"
#define __MCF_FWD_IMPORT  __MCF_DLLEXPORT
#define __MCF_FWD_INLINE  __MCF_DLLEXPORT
#include "fwd.h"
#include "xglobals.h"

static inline
void
do_append_string(WCHAR** sp, PCWSTR end_of_buffer, WCHAR c)
  {
    if(*sp != end_of_buffer)
      *((*sp) ++) = c;
  }

static
DWORD
do_format_message(DWORD code, WCHAR* outptr, PCWSTR end_of_buffer)
  {
    return FormatMessageW(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | 0xFF,
        __MCF_nullptr, code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        outptr, (DWORD) (end_of_buffer - outptr), __MCF_nullptr);
  }

__MCF_DLLEXPORT
void
__MCF_runtime_failure(const char* where)
  {
    DWORD last_error = GetLastError();
    WCHAR buffer[1536];
    WCHAR* sptr = buffer;
    WCHAR* end_of_buffer = buffer + RTL_NUMBER_OF(buffer);

    /* Get a piece of localized text for the caption of the message box.  */
    UNICODE_STRING caption;
    caption.Buffer = sptr;
    caption.Length = 0;
    caption.MaximumLength = (USHORT) ((UINT_PTR) end_of_buffer - (UINT_PTR) sptr);

    /* #define ERROR_UNHANDLED_EXCEPTION   574L
     * {Application Error}
     * The exception %s (0x%08lx) occurred in the application at location 0x%08lx.  */
    WCHAR* lptr = end_of_buffer - 127;
    DWORD outlen = do_format_message(574, lptr, end_of_buffer);
    if((outlen != 0) && (*lptr == L'{')) {
      lptr ++;

      while((*lptr != 0) && (*lptr != L'}'))
        do_append_string(&sptr, end_of_buffer, *(lptr ++));

      caption.Length = (USHORT) ((UINT_PTR) sptr - (UINT_PTR) caption.Buffer);
    }

    /* Get a piece of localized text for the text of the message box.  */
    UNICODE_STRING text;
    text.Buffer = sptr;
    text.Length = 0;
    text.MaximumLength = (USHORT) ((UINT_PTR) end_of_buffer - (UINT_PTR) sptr);

    /* Get the file name of the executable.  */
    outlen = GetModuleFileNameW(NULL, sptr, (DWORD) (end_of_buffer - sptr));
    if(outlen != 0) {
      sptr += outlen;

      do_append_string(&sptr, end_of_buffer, L'\r');
      do_append_string(&sptr, end_of_buffer, L'\n');

      text.Length = (USHORT) ((UINT_PTR) sptr - (UINT_PTR) text.Buffer);
    }

    /* #define ERROR_DEBUG_ATTACH_FAILED   590L
     * {Unexpected Failure in DebugActiveProcess}
     * An unexpected failure occurred while processing a DebugActiveProcess API
     * request. You may choose OK to terminate the process, or Cancel to ignore
     * the error.  */
    lptr = end_of_buffer - 127;
    outlen = do_format_message(590, lptr, end_of_buffer);
    if((outlen != 0) && (*lptr == L'{')) {
      lptr ++;

      while((*lptr != 0) && (*lptr != L'}'))
        if((*lptr == L'D') && __MCF_mequal(lptr, L"DebugActiveProcess", 18 * sizeof(WCHAR))) {
          lptr += 18;
          do_append_string(&sptr, end_of_buffer, L'`');
          const char* lw = where;
          while(*lw != 0) do_append_string(&sptr, end_of_buffer, (unsigned char) *(lw ++));
          do_append_string(&sptr, end_of_buffer, L'`');
        }
        else
          do_append_string(&sptr, end_of_buffer, *(lptr ++));

      do_append_string(&sptr, end_of_buffer, L':');
      do_append_string(&sptr, end_of_buffer, L' ');

      outlen = do_format_message(last_error, sptr, end_of_buffer);
      sptr += outlen;

      text.Length = (USHORT) ((UINT_PTR) sptr - (UINT_PTR) text.Buffer);
    }

    /* If we are in a DLL entry-point function or a TLS callback, it is not safe
     * to call `MessageBoxW()` from USER32.DLL, so request CSRSS.EXE to display
     * the message box for us.  */
    __MCF_show_hard_error_message_box(&caption, &text);
    TerminateProcess(GetCurrentProcess(), STATUS_FATAL_APP_EXIT);
    __builtin_unreachable();
  }

__MCF_DLLEXPORT
uint32_t
_MCF_get_win32_error(void)
  {
    return GetLastError();
  }

__MCF_DLLEXPORT
size_t
_MCF_get_page_size(void)
  {
    return __MCF_crt_sysinfo.dwPageSize;
  }

__MCF_DLLEXPORT
size_t
_MCF_get_processor_count(void)
  {
    return __MCF_crt_sysinfo.dwNumberOfProcessors;
  }

__MCF_DLLEXPORT
uintptr_t
_MCF_get_active_processor_mask(void)
  {
    return __MCF_crt_sysinfo.dwActiveProcessorMask;
  }
