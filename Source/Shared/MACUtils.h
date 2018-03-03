// @todo delete
#pragma once

#include "config.h"

#ifndef HAVE_WCSCASECMP
#include <wchar.h>

int mac_wcscasecmp(const wchar_t *s1, const wchar_t *s2);
int mac_wcsncasecmp(const wchar_t *s1, const wchar_t *s2, size_t n);

#endif
