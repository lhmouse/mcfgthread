/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "precompiled.i"

__MCF_DLLEXPORT
int
__cdecl
memcmp(const void* src, const void* cmp, size_t size);

__MCF_DLLEXPORT
int
__cdecl
memcmp(const void* src, const void* cmp, size_t size)
  {
    int diff;
    const uint8_t* psrc = src;
    const uint8_t* pcmp = cmp;
    size_t dp = size;

#if defined(__i386__) || defined(__amd64__)
    /* Perform string comparison, in a way that happens to look the same
     * in both AT&T and Intel syntaxes.  */
    __asm__ volatile (
      "xor %0, %0; "    /* clear ZF and CF  */
      "repz cmpsb; "    /* perform comparison  */
      "setnz %b0; "     /* EAX = 0 if equal, 1 if less or greater  */
      "sbb %k3, %k3; "  /* ECX = 0 if equal or greater, -1 if less  */
      : "=a"(diff), "+S"(psrc), "+D"(pcmp), "+c"(dp) :
      : "memory");

    /* Read comments above about this trick.  */
    diff |= (int) dp;
#else
    /* Get the number of matching bytes.  */
    dp = RtlCompareMemory(psrc, pcmp, dp);
    if(dp == size)
      return 0;

    /* Get the difference between the first pair of mismatching bytes.  */
    diff = psrc[dp] - pcmp[dp];
#endif
    return diff;
  }
