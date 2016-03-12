/** 
 * Copyright (c) 2016 SQLines
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// Token class

#ifndef sqlines_token_h
#define sqlines_token_h

#include <string>

// Token types
#define TOKEN_WORD		1		// Keyword or unquoted identifier (will be refined later)
#define TOKEN_KEYWORD	2		// Keyword (part of language, not only reserved words)
#define TOKEN_IDENT		3		// Identifier (quoted, unquoted, qualified multi-part)
#define TOKEN_SYMBOL	4		// Single char symbol
#define TOKEN_NUMBER	5		// Number
#define TOKEN_STRING	6		// Quoted string
#define TOKEN_COMMENT	7		// Comment
#define TOKEN_BOM		8		// UTF byte order mark
#define TOKEN_FUNCTION	9		// Function
#define TOKEN_STATEMENT	10		// Statement

// Token data type
#define TOKEN_DT_DATETIME		1		// Date and time
#define TOKEN_DT_STRING			2		// String
#define TOKEN_DT_NUMBER			3		// Number

// Token data subtype
#define TOKEN_DT2_DATETIME		1		// Date and time
#define TOKEN_DT2_DATE			2		// Date only
#define TOKEN_DT2_TIME			3		// Time only
#define TOKEN_DT2_INT8			4		// 8-bit integer

// Token flags
#define TOKEN_REMOVED   0x1
#define TOKEN_INSERTED  0x2

class TokenStr;

class Token
{
public:
	// Token type
	char type;
	// Token target type (was function, converted to statement i.e.)
	char t_type;

	// Token data type (Column type, function return value)
	char data_type;
	char data_subtype;
	// Can contain NULL (function can return null)
	bool nullable;

	// Single char token data
	char chr;
	wchar_t wchr; 

	// String data
	const char *str;
	const wchar_t *wstr;

	// Target values
	const char *t_str;
	const wchar_t *t_wstr;

	// Length of string data (0 for single char)
	int len;
	int t_len;

	// Token flags
	int flags;

	// Number of input bytes remain after token
	int remain_size;
	// Pointer to the buffer right after token
	const char *next_start;

	// Source value was allocated for the token, and needs to be deleted in destructor
	bool source_allocated;

	// Constructor/destructor
	Token();
	~Token();

	// Get a copy of token
	static Token* GetCopy(Token *source);

	// Compare token value with the specified word or char
	bool Compare(const char *word, const wchar_t *w_word, int len);
	bool Compare(const char *word, const wchar_t *w_word, int start, int len);
	bool Compare(const char ch, const wchar_t wch);
	bool Compare(const char ch, const wchar_t wch, int start);

	static bool Compare(Token *token, const char *word, const wchar_t *w_word, int len);
	static bool Compare(Token *token, const char *word, const wchar_t *w_word, int start, int len);
	static bool CompareCS(Token *token, const char *word, const wchar_t *w_word, int start, int len);
	static bool Compare(Token *token, const char ch, const wchar_t wch);
	static bool Compare(Token *token, const char ch, const wchar_t wch, int start);
	static bool Compare(Token *first, Token *second);
	static bool Compare(Token *first, Token *second, int len);
	static bool Compare(Token *first, TokenStr *second);

	static bool CompareTarget(Token *token, const char *word, const wchar_t *w_word, int len);

	bool IsNumeric();
	bool IsNumericInString();
	bool IsSingleChar();
	bool IsBlank();
	bool IsRemoved() { return flags & TOKEN_REMOVED; }
	bool IsTargetSet() { return (t_len > 0) ? true : false; }

	static bool IsBlank(Token *token);

	// Change the token value
	static void Change(Token *token, const char *new_str, const wchar_t *new_wstr, int len, Token *format = NULL);
	static void Change(Token *token, int value);
	static void Change(Token *token, Token *values);

	static void ChangeNoFormat(Token *token, const char *new_str, const wchar_t *new_wstr, int len);
	static void ChangeNoFormat(Token *token, TokenStr &tstr);
	static void ChangeNoFormat(Token *token, Token *source, int start, int len);

	// Replace the token value with spaces to preserve indent
	static void ReplaceWithSpaces(Token *token);

	// Set Removed flag for the token
	static void Remove(Token *token, bool remove_spaces_before = true);
	static void Remove(Token *from, Token *last);

	// Get the target length of the token
	int GetTargetLength();

	// Remove the current target value
	static void ClearTarget(Token *token);

	// Append data to the string
	void AppendTarget(char *string, int *cur_len);

	// Format target value case
	static void FormatTargetValue(Token *token, Token *format = NULL);

	// Skip blanks, tabs and newlines
	static Token* SkipSpaces(Token *token);

	// Get value as int
	int GetInt();
	int GetInt(int start, int len);

	Token *prev;
	Token *next;
};

class TokenStr
{
public:
	std::string str;
	std::wstring wstr;

	int len;

	TokenStr() { len = 0; prev = NULL; next = NULL; }
	TokenStr(const char *s, const wchar_t *w, int l) { Set(s, w, l); prev = NULL; next = NULL; }
	TokenStr(Token *token);
	
	void Set(const char *str, const wchar_t *wstr, int len);
	void Set(TokenStr &src);
	
	// Append a string
	void Append(const char *str, const wchar_t *wstr, int len);
	void Append(Token *token, int start, int len);
	void Append(Token *token);
	void Append(TokenStr &str);
	void Append(TokenStr &str, int start, int len);
	void Append(int num);

	void Clear() { str.clear(); wstr.clear(); len = 0; }

	bool Compare(const char *word, const wchar_t *w_word, int len);
	bool Compare(const char ch, const wchar_t wch, int pos);

	static bool Compare(TokenStr &first, TokenStr &second);
	static bool Compare(TokenStr *tokenstr, const char *word, const wchar_t *w_word, int len);
	static bool Compare(TokenStr *tokenstr, const char ch, const wchar_t wch, int pos);

	TokenStr *prev;
	TokenStr *next;
};

#endif // sqlines_token_h