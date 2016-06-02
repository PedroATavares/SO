#pragma once

#ifdef UNICODE
#define WIDE_CHAR
#else 
#ifdef _UNICODE
#define WIDE_CHAR
#else
#undef WIDE_CHAR
#endif
#endif

// check if string "s" matches pattern "rs"
bool strmatch(char *rs, char *s);

// check if string "s" matches pattern "rs"
bool w_strmatch(WCHAR *rs, WCHAR *s);

// convert wide char  string to char  string
void w2c_strcpy(char *dest, WCHAR *src);

#ifdef WIDE_CHAR
#define _tstrmatch w_strmatch
#else
#define _tstrmatch strmatch
#endif