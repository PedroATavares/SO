#include "stdafx.h"

bool strmatch(char *rs, char *s) {
	if (*s == 0) {
		return *rs == 0 || *rs == '*' && *(rs + 1) == 0;
	}
	else if (*rs == '?')  {
		if (strmatch(rs + 1, s)) return true;
		return strmatch(rs + 1, s + 1);
	}
	else if (*rs == '*') {
		int i = 0;
		if (*(rs + 1) == 0) return true;
		while (*(s + i) != 0) {
			if (strmatch(rs + 1, s + i)) return true;
			i++;
		}
		return false;
	}
	else if (*rs != *s) return false;
	else return strmatch(rs + 1, s + 1);
}

bool w_strmatch(WCHAR *rs, WCHAR *s) {
	if (*s == 0) {
		return *rs == 0 || *rs == '*'  && *(rs + 1) == 0;
	}
	else if (*rs == '?')  {
		if (w_strmatch(rs + 1, s)) return true;
		return w_strmatch(rs + 1, s + 1);
	}
	else if (*rs == '*') {
		int i = 0;
		if (*(rs + 1) == 0)
			return true;
		while (*(s + i) != 0) {
			if (w_strmatch(rs + 1, s + i)) return true;
			i++;
		}
		return false;
	}
	else if (*rs != *s) return false;
	else return w_strmatch(rs + 1, s + 1);
}

void w2c_strcpy(char *dest, WCHAR *src)
{
	while (*dest++ = (char) *src++);
}

#ifdef TEST_CODE

void test(char *rs, char *s) {
	char c = strmatch(rs, s) ? 'Y' : 'N';
	printf("'%s' and '%s' match? %c\n", rs, s, c);
}

void test(TCHAR *rs, TCHAR *s) {
	TCHAR c = _tstrmatch(rs, s) ? 'Y' : 'N';
	_tprintf(_T("'%s' and '%s' match? %c\n"), rs, s, c);
}

void StrMatchTests() {
	test("a", "a");
	test("", "");
	test("a", "b");
	test("a*", "a");
	test("a*", "abc");
	test("a****", "abc");
	test("a*cd", "abcd");
	test("a*cd**df", "abcdssssdf");
	test("*a*cd**df*", "xptoabcdssssdfxpto");
	test("a*cd**d?a", "abcdssssda");
	test("a*cd**d?a", "abcdssssdaaa");

	test(_T("a"), _T("a"));
	test(_T(""), _T(""));
	test(_T("a"), _T("b"));
	test(_T("a*"), _T("a"));
	test(_T("a*"), _T("abc"));
	test(_T("a****"), _T("abc"));
	test(_T("a*cd"), _T("abcd"));
	test(_T("a*cd**df"), _T("abcdssssdf"));
	test(_T("*a*cd**df*"), _T("xptoabcdssssdfxpto"));
	test(_T("a*cd**d?a"), _T("abcdssssda"));
	test(_T("a*cd**d?a"), _T("abcdssssdaaa"));

}

#endif