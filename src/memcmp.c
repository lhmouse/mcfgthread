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
    const uint8_t* psrc = src;
    const uint8_t* pcmp = cmp;

    /* Get the number of matching bytes.  */
    SIZE_T mlen = RtlCompareMemory(psrc, pcmp, size);
    if(mlen == size)
      return 0;

    /* Return the difference between mismatching bytes.  */
    return psrc[mlen] - pcmp[mlen];
  }
