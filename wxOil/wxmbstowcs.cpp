

#include "camtypes.h"
#include "wxmbstowcs.h"


size_t wxMbstowcs(wchar_t* out, const char* in, size_t outlen) {
  if (!out) {
    size_t outsize = 0;
    while(*in++)
      outsize++;
    return outsize;
  }
  const char* origin = in;
  while (outlen-- && *in) {
    *out++ = (wchar_t) *in++;
  }
  *out = '\0';
  return in - origin;
}



size_t wxWcstombs (char * out, const wchar_t * in, size_t outlen) {
  if (!out)
    {
      size_t outsize = 0;
      while(*in++)
	outsize++;
      return outsize;
    }

  const wchar_t* origin = in;

  while (outlen-- && *in)
    {
      *out++ = (char) *in++;
    }

  *out = '\0';

  return in - origin;
}
