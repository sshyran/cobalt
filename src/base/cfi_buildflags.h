// Generated by build/write_buildflag_header.py
// From "//base:cfi_buildflags"
//
// However for Cobalt we just hardcode the values of these #defines.

#ifndef BASE_CFI_BUILDFLAGS_H_
#define BASE_CFI_BUILDFLAGS_H_

#include "base/buildflag.h"

#define BUILDFLAG_INTERNAL_CFI_CAST_CHECK() (false && false)
#define BUILDFLAG_INTERNAL_CFI_ICALL_CHECK() (false && false)
#define BUILDFLAG_INTERNAL_CFI_ENFORCEMENT_TRAP() (false && !false)
#define BUILDFLAG_INTERNAL_CFI_ENFORCEMENT_DIAGNOSTIC() \
  (false && false && !false)

#endif  // BASE_CFI_BUILDFLAGS_H_