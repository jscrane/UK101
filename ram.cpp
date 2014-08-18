#include <Stream.h>
#include "Memory.h"
#include "ram.h"

void ram::checkpoint(Stream &s)
{
  s.write(_mem, sizeof(_mem));
}

void ram::restore(Stream &s)
{
  s.readBytes((char *)_mem, sizeof(_mem));
}
