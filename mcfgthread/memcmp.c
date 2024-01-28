/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022 - 2024, LH_Mouse. All wrongs reserved.  */

#include "precompiled.h"

__MCF_DLLEXPORT
int
__cdecl
memcmp(const void* src, const void* cmp, size_t size);

__MCF_DLLEXPORT
int
__cdecl
memcmp(const void* src, const void* cmp, size_t size)
  {
    int diff = 0;
    const uint8_t* psrc = src;
    const uint8_t* pcmp = cmp;
    size_t dp = size;

#if defined __i386__ || defined __amd64__
    /* Perform string comparison with hardware.  */
    __asm__ volatile (
      "xor eax, eax; "  /* clear ZF and CF  */
      "repz cmpsb; "    /* perform comparison  */
      "setnz al; "      /* EAX = 0 if equal, 1 if less or greater  */
      "sbb ecx, ecx; "  /* ECX = 0 if equal or greater, -1 if less  */
      "or eax, ecx; "
      : "=a"(diff), "+S"(psrc), "+D"(pcmp), "+c"(dp) :
      : "memory");
#else
    /* Get the number of matching bytes. If there is a mismatch, get the
     * difference between the first pair of mismatching bytes.  */
    dp = RtlCompareMemory(psrc, pcmp, dp);
    if(dp != size)
      diff = psrc[dp] - pcmp[dp];
#endif
    return diff;
  }
