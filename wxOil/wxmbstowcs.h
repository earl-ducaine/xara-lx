

#ifndef WXMBSTOWCS_H
#define WXMBSTOWCS_H

size_t wxMbstowcs(wchar_t* out, const char* in, size_t outlen);
size_t wxWcstombs (char * out, const wchar_t * in, size_t outlen);
  
#endif
