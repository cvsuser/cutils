#ifndef HASH_H_
#define HASH_H_

#include <stdint.h>
#include <stddef.h>

namespace hash
{
uint32_t Hash(const char *data, size_t n, uint32_t seed=0);
}

#endif
