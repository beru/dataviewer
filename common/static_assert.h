#pragma once

template <bool expr>
static inline void static_assert()
{
	char dummy[expr ? 1 : 0];
}

