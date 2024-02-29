/*
 * MIT License
 *
 * Copyright (c) 2022 Davidson Francis <davidsondfgl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*
 * This is Aqua: a dumb syntax highlight for Python code written in C,
 * which uses ANSI Escape Sequences to color the output.
 *
 * Aqua tries to be as simple as possible:
 * - Cannot identify UTF-8 encoded keywords (it just ignores them).
 * - Number parsing is pretty silly (see state_number() for more info),
 *   but it 'works'.
 *
 * Which also brings several interesting advantages, such as:
 *
 * Portability:
 * ------------
 * - Uses only the 16 base colors of the ANSI Escape Sequence
 *
 * - Minimal use of libc, in fact Aqua makes no use of malloc (unless
 *   explicitly desired, via the AQUA_USE_MALLOC macro), I/O functions and
 *   not even regex: all functionality is done by a state machine capable to
 *   identify strings, keywords, numbers and comments. The result is saved
 *   in a static size buffer (or not, more below).
 *
 * Performance:
 * ------------
 * In my highly (un)scientific¹ tests, Aqua was able to highlight 586859
 * lines in 194ms, which is about 3M lines/second. Much more than you
 * could ever want, right?
 *
 * Notes):
 * 1: The test was done by artificially generating a 20M file from 91
 * consecutive concatenations of the _pydecimal.py file (found in any
 * CPython installation), which generates a file of almost 600k lines.
 *
 * You can reproduce the test with something like:
 * $ cp /usr/lib64/python3.8/_pydecimal.py .
 * $ for i in {1..91}; do cat _pydecimal.py >> big.py; done
 * $ wc -l big.py
 * 586859 big.py
 *
 * $ wc -c big.py
 * 20950748 big.py
 *
 * $ gcc aqua_test.c aqua.c -o aqua_test -O3
 * $ time ./aqua_test big.py > /dev/null
 *
 * real	0m0.194s
 * user	0m0.190s
 * sys	0m0.004s
 *
 * ==================================
 *              Usage
 * ==================================
 *
 * Using Aqua can be broken down into 3 simple steps:
 *
 * Step 1)
 * -------
 * Aqua requires the use of a context structure (which makes it thread-safe,
 * for different contexts) and its proper initialization with aqua_init,
 * something like:
 *
 * #include "aqua.sh"
 * struct aqua_sh sh;
 * aqua_init(&sh).
 *
 * (and this needs to be done only once).
 *
 * Step 2)
 * -------
 * After that, you can continually invoke aqua_highlight() to highlight a
 * null-terminated (or not) string. This string can be a single line,
 * multiple lines, or even the entire code in one go. Aqua is able to
 * maintain consistency of the current state between summons, so the order
 * doesn't matter.
 *
 * Something like:
 * char *buff = aqua_highlight(&sh, my_src, 0);
 * ('0' means that the length is guessed, a non-null terminated string)
 *  can be passed if the length is specified)
 *
 * aqua_highlight() returns a pointer to a null-terminated buffer with the
 * string highlighted or NULL on error.
 *
 * If you keep getting NULL from aqua_highlight(), try increasing the
 * buffer size (MAX_LINE in aqua.h) or enabling dynamic memory usage by
 * defining the AQUA_USE_MALLOC macro before including aqua.h.
 *
 * (The default buffer size (4096) should be more than enough to handle
 * just one line. If you want to handle more than one line in a single
 * invocation of aqua_highlight(), consider following the above
 * recommendation.)
 *
 * Note: the buffer returned by aqua_highlight() is reused in other
 * invocations (similar to what getline() does). If you don't want to
 * print the highlight right away, consider copying it somewhere else.
 *
 * Step 3)
 * -------
 * Release resources.
 * After you're done with Aqua, you can free up the resources with:
 *   aqua_finish(&sh).
 *
 * (this is not strictly necessary unless you are using Aqua with
 *  -DAQUA_USE_MALLOC).
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "aqua.h"

/* Colors. */
#define COLOR_RESET  "\e[0m"
#define COLOR_RED    "\e[31m"
#define COLOR_GREEN  "\e[32m"
#define COLOR_YELLOW "\e[33m"
#define COLOR_BLUE   "\e[34m"
#define COLOR_PURPLE "\e[35m"
#define COLOR_CYAN   "\e[36m"
#define COLOR_GRAY   "\e[37m"

#define COLOR_BRIGHT_RED    "\e[31;1m"
#define COLOR_BRIGHT_GREEN  "\e[32;1m"
#define COLOR_BRIGHT_YELLOW "\e[33;1m"
#define COLOR_BRIGHT_BLUE   "\e[34;1m"
#define COLOR_BRIGHT_PURPLE "\e[35;1m"
#define COLOR_BRIGHT_CYAN   "\e[36;1m"
#define COLOR_BRIGHT_GRAY   "\e[37;1m"

/* Color definitions. */
#define COLOR_SYMBOL    COLOR_BRIGHT_YELLOW
#define COLOR_KEYWORD   COLOR_BRIGHT_BLUE
#define COLOR_FUNCTION  COLOR_KEYWORD
#define COLOR_COMMENT   COLOR_BRIGHT_RED
#define COLOR_STRING    COLOR_BRIGHT_GREEN
#define COLOR_NUMBER    COLOR_BRIGHT_CYAN
#define COLOR_EXCEPTION COLOR_BRIGHT_PURPLE

/* Keyword. */
struct keyword
{
	const char *keyword;
	const char *color;
};

/*
 * Keywords list.
 *
 * In fact this list goes beyond keywords and maintains
 * a list of words that are useful for a Python programmer,
 * such as keywords, functions and exceptions.
 *
 * ---
 * Please keep this always sorted, since a binary
 * search will be performed on this list.
 *
 * Tip: this isn't hard to keep nice and clean, just
 * save this somewhere and run:
 *   $ column -t kw_list.txt | sort -u
 *
 * and then you will have the list already sorted and
 * formatted for you =).
 */
static const struct keyword kw_list[] = {
	{"ArithmeticError",      COLOR_EXCEPTION},
	{"AssertionError",       COLOR_EXCEPTION},
	{"AttributeError",       COLOR_EXCEPTION},
	{"EOFError",             COLOR_EXCEPTION},
	{"EnvironmentError",     COLOR_EXCEPTION},
	{"Exception",            COLOR_EXCEPTION},
	{"False",                COLOR_KEYWORD},
	{"FloatingPointError",   COLOR_EXCEPTION},
	{"IOError",              COLOR_EXCEPTION},
	{"ImportError",          COLOR_EXCEPTION},
	{"IndexError",           COLOR_EXCEPTION},
	{"KeyError",             COLOR_EXCEPTION},
	{"KeyboardInterrupt",    COLOR_EXCEPTION},
	{"LookupError",          COLOR_EXCEPTION},
	{"MemoryError",          COLOR_EXCEPTION},
	{"NameError",            COLOR_EXCEPTION},
	{"None",                 COLOR_KEYWORD},
	{"NotImplementedError",  COLOR_EXCEPTION},
	{"OSError",              COLOR_EXCEPTION},
	{"OverflowError",        COLOR_EXCEPTION},
	{"RuntimeError",         COLOR_EXCEPTION},
	{"StandardError",        COLOR_EXCEPTION},
	{"SyntaxError",          COLOR_EXCEPTION},
	{"SystemError",          COLOR_EXCEPTION},
	{"SystemExit",           COLOR_EXCEPTION},
	{"True",                 COLOR_KEYWORD},
	{"TypeError",            COLOR_EXCEPTION},
	{"UnboundLocalError",    COLOR_EXCEPTION},
	{"UnicodeError",         COLOR_EXCEPTION},
	{"ValueError",           COLOR_EXCEPTION},
	{"WindowsError",         COLOR_EXCEPTION},
	{"ZeroDivisionError",    COLOR_EXCEPTION},
	{"abs",                  COLOR_FUNCTION},
	{"and",                  COLOR_KEYWORD},
	{"append",               COLOR_FUNCTION},
	{"apply",                COLOR_FUNCTION},
	{"as",                   COLOR_KEYWORD},
	{"assert",               COLOR_KEYWORD},
	{"bool",                 COLOR_FUNCTION},
	{"break",                COLOR_KEYWORD},
	{"buffer",               COLOR_FUNCTION},
	{"callable",             COLOR_FUNCTION},
	{"chr",                  COLOR_FUNCTION},
	{"class",                COLOR_KEYWORD},
	{"clear",                COLOR_FUNCTION},
	{"close",                COLOR_FUNCTION},
	{"closed",               COLOR_FUNCTION},
	{"cmp",                  COLOR_FUNCTION},
	{"coerce",               COLOR_FUNCTION},
	{"compile",              COLOR_FUNCTION},
	{"complex",              COLOR_FUNCTION},
	{"conjugate",            COLOR_FUNCTION},
	{"continue",             COLOR_KEYWORD},
	{"copy",                 COLOR_FUNCTION},
	{"count",                COLOR_FUNCTION},
	{"def",                  COLOR_KEYWORD},
	{"del",                  COLOR_KEYWORD},
	{"delattr",              COLOR_FUNCTION},
	{"dir",                  COLOR_FUNCTION},
	{"divmod",               COLOR_FUNCTION},
	{"elif",                 COLOR_KEYWORD},
	{"else",                 COLOR_KEYWORD},
	{"eval",                 COLOR_FUNCTION},
	{"except",               COLOR_KEYWORD},
	{"exec",                 COLOR_FUNCTION},
	{"execfile",             COLOR_FUNCTION},
	{"extend",               COLOR_FUNCTION},
	{"fileno",               COLOR_FUNCTION},
	{"filter",               COLOR_FUNCTION},
	{"finally",              COLOR_KEYWORD},
	{"float",                COLOR_FUNCTION},
	{"flush",                COLOR_FUNCTION},
	{"for",                  COLOR_KEYWORD},
	{"from",                 COLOR_KEYWORD},
	{"get",                  COLOR_FUNCTION},
	{"getattr",              COLOR_FUNCTION},
	{"global",               COLOR_KEYWORD},
	{"globals",              COLOR_FUNCTION},
	{"has_key",              COLOR_FUNCTION},
	{"hasattr",              COLOR_FUNCTION},
	{"hash",                 COLOR_FUNCTION},
	{"hex",                  COLOR_FUNCTION},
	{"id",                   COLOR_FUNCTION},
	{"if",                   COLOR_KEYWORD},
	{"import",               COLOR_KEYWORD},
	{"in",                   COLOR_KEYWORD},
	{"index",                COLOR_FUNCTION},
	{"input",                COLOR_FUNCTION},
	{"insert",               COLOR_FUNCTION},
	{"int",                  COLOR_FUNCTION},
	{"intern",               COLOR_FUNCTION},
	{"is",                   COLOR_KEYWORD},
	{"isatty",               COLOR_FUNCTION},
	{"isinstance",           COLOR_FUNCTION},
	{"issubclass",           COLOR_FUNCTION},
	{"items",                COLOR_FUNCTION},
	{"keys",                 COLOR_FUNCTION},
	{"lambda",               COLOR_KEYWORD},
	{"len",                  COLOR_FUNCTION},
	{"list",                 COLOR_FUNCTION},
	{"locals",               COLOR_FUNCTION},
	{"long",                 COLOR_FUNCTION},
	{"map",                  COLOR_FUNCTION},
	{"max",                  COLOR_FUNCTION},
	{"min",                  COLOR_FUNCTION},
	{"mode",                 COLOR_FUNCTION},
	{"name",                 COLOR_FUNCTION},
	{"nonlocal",             COLOR_KEYWORD},
	{"not",                  COLOR_KEYWORD},
	{"oct",                  COLOR_FUNCTION},
	{"open",                 COLOR_FUNCTION},
	{"or",                   COLOR_KEYWORD},
	{"ord",                  COLOR_FUNCTION},
	{"pass",                 COLOR_KEYWORD},
	{"pop",                  COLOR_FUNCTION},
	{"pow",                  COLOR_FUNCTION},
	{"print",                COLOR_FUNCTION},
	{"raise",                COLOR_KEYWORD},
	{"range",                COLOR_FUNCTION},
	{"raw_input",            COLOR_FUNCTION},
	{"read",                 COLOR_FUNCTION},
	{"readline",             COLOR_FUNCTION},
	{"readlines",            COLOR_FUNCTION},
	{"reduce",               COLOR_FUNCTION},
	{"reload",               COLOR_FUNCTION},
	{"remove",               COLOR_FUNCTION},
	{"repr",                 COLOR_FUNCTION},
	{"return",               COLOR_KEYWORD},
	{"reverse",              COLOR_FUNCTION},
	{"round",                COLOR_FUNCTION},
	{"seek",                 COLOR_FUNCTION},
	{"self",                 COLOR_KEYWORD},
	{"setattr",              COLOR_FUNCTION},
	{"slice",                COLOR_FUNCTION},
	{"softspace",            COLOR_FUNCTION},
	{"sort",                 COLOR_FUNCTION},
	{"str",                  COLOR_FUNCTION},
	{"tell",                 COLOR_FUNCTION},
	{"truncate",             COLOR_FUNCTION},
	{"try",                  COLOR_KEYWORD},
	{"tuple",                COLOR_FUNCTION},
	{"type",                 COLOR_FUNCTION},
	{"unichr",               COLOR_FUNCTION},
	{"unicode",              COLOR_FUNCTION},
	{"update",               COLOR_FUNCTION},
	{"values",               COLOR_FUNCTION},
	{"vars",                 COLOR_FUNCTION},
	{"while",                COLOR_KEYWORD},
	{"with",                 COLOR_KEYWORD},
	{"write",                COLOR_FUNCTION},
	{"writelines",           COLOR_FUNCTION},
	{"xrange",               COLOR_FUNCTION},
	{"yield",                COLOR_KEYWORD},
	{"zip",                  COLOR_FUNCTION},
};

/*
 * strlen of bigger keyword.
 *
 * Please also keep this up-to-date.
 *
 * Tip: You can do this by saving the keyword list above
 * somewhere and then run:
 * $ cut -d'"' -f2 kw_list | awk '{print length}' | sort -g | tail -n1
 */
#define MAX_KEYWORD_SIZE 19

/*
 * Allowed symbols table.
 *
 * This is a simple lookup table that contains all symbols that
 * will be highlighted.
 */
static const unsigned char is_symbol[256] = {
	['['] = 1, [']'] = 1, ['('] = 1, [')'] = 1, ['{'] = 1, ['}'] = 1,
	['*'] = 1, [':'] = 1, ['='] = 1, [';'] = 1, ['-'] = 1, ['>'] = 1,
	['&'] = 1, ['+'] = 1, ['~'] = 1, ['!'] = 1, ['/'] = 1, ['%'] = 1,
	['<'] = 1, ['^'] = 1, ['|'] = 1, ['?'] = 1, ['@'] = 1, ['.'] = 1,
	[','] = 1
};

/*
 * Is keyword/identifier character lookup table
 *
 * This is simple and faster than using isalnum(c) || c == '_',
 * so I'll use this instead.
 */
static const unsigned char is_char_keyword[256] = {
	['0'] = 1, ['1'] = 1, ['2'] = 1, ['3'] = 1, ['4'] = 1, ['5'] = 1,
	['6'] = 1, ['7'] = 1, ['8'] = 1, ['9'] = 1, ['A'] = 1, ['B'] = 1,
	['C'] = 1, ['D'] = 1, ['E'] = 1, ['F'] = 1, ['G'] = 1, ['H'] = 1,
	['I'] = 1, ['J'] = 1, ['K'] = 1, ['L'] = 1, ['M'] = 1, ['N'] = 1,
	['O'] = 1, ['P'] = 1, ['Q'] = 1, ['R'] = 1, ['S'] = 1, ['T'] = 1,
	['U'] = 1, ['V'] = 1, ['X'] = 1, ['W'] = 1, ['Y'] = 1, ['Z'] = 1,
	['_'] = 1, ['a'] = 1, ['b'] = 1, ['c'] = 1, ['d'] = 1, ['e'] = 1,
	['f'] = 1, ['g'] = 1, ['h'] = 1, ['i'] = 1, ['j'] = 1, ['k'] = 1,
	['l'] = 1, ['m'] = 1, ['n'] = 1, ['o'] = 1, ['p'] = 1, ['q'] = 1,
	['r'] = 1, ['s'] = 1, ['t'] = 1, ['u'] = 1, ['v'] = 1, ['x'] = 1,
	['w'] = 1, ['y'] = 1, ['z'] = 1,
};

/*
 * My isdigit()... maybe someone could argue that I should rely
 * on libc instead of doing it myself, but... this is fast...
 * and I swear that I will only pass chars as indexes here =).
 */
static const unsigned char is_digit[256] = {
	['0'] = 1, ['1'] = 1, ['2'] = 1, ['3'] = 1, ['4'] = 1, ['5'] = 1,
	['6'] = 1, ['7'] = 1, ['8'] = 1, ['9'] = 1
};

/* ========================================================================= */
/*                            BUFFER ROUTINES                                */
/* ========================================================================= */

#ifdef AQUA_USE_MALLOC
/**
 * @brief Rounds up to the next power of two.
 *
 * @param target Target number to be rounded.
 *
 * @return Returns the next power of two.
 */
static size_t next_power(size_t target)
{
	target--;
	target |= target >> 1;
	target |= target >> 2;
	target |= target >> 4;
	target |= target >> 8;
	target |= target >> 16;
	target++;
	return (target);
}
#endif

/**
 * @brief Checks if the new size fits in the append buffer, if not,
 * reallocates the buffer size by @p incr bytes.
 *
 * If the macro AQUA_USE_MALLOC is not defined (default), this only
 * checks if the new size fits the buffer.
 *
 * @param sh Aqua highlight context.
 * @param incr Size (in bytes) to be incremented.
 *
 * @return Returns 0 if success, -1 otherwise.
 *
 * @note The new size is the next power of two, that is capable
 * to hold the required buffer size.
 */
static int increase_buff(struct aqua_sh *sh, size_t incr)
{
#ifndef AQUA_USE_MALLOC
	if (sh->pos + incr >= MAX_LINE)
		return (-1);
#else
	char *new;
	size_t new_size;
	if (sh->pos + incr > sh->buff_len)
	{
		new_size = next_power(sh->buff_len + incr);
		new = realloc(sh->buff, new_size);
		if (new == NULL)
		{
			free(sh->buff);
			sh->buff = NULL;
			return (-1);
		}
		sh->buff_len = new_size;
		sh->buff = new;
	}
#endif
	return (0);
}

/**
 * @brief Append a given char @p c into the buffer.
 *
 * @param sh Aqua highlight context.
 * @param c Char to be appended.
 *
 * @return Returns 0 if success, -1 otherwise.
 */
static int append_char(struct aqua_sh *sh, char c)
{
	if (increase_buff(sh, 1) < 0)
		return (-1);

	sh->buff[sh->pos] = c;
	sh->pos++;
	return (0);
}

/**
 * @brief Appends a given string pointed by @p s of size @p len
 * into the current buffer.
 *
 * If @p len is 0, the string is assumed to be null-terminated
 * and its length is obtained.
 *
 * @param sh Aqua highlight context.
 * @param s String to be append into the buffer.
 * @param len String size, if 0, it's length is obtained.
 *
 * @return Returns 0 if success, -1 otherwise.
 */
static int append_string(struct aqua_sh *sh, const char *s, size_t len)
{
	if (!len)
		len = strlen(s);

	if (increase_buff(sh, len) < 0)
		return (-1);

	memcpy(sh->buff + sh->pos, s, len);
	sh->pos += len;
	return (0);
}

/* ========================================================================= */
/*                           HIGHLIGHT ROUTINES                              */
/* ========================================================================= */

/**
 * @brief Add a new symbol (like [, ], (, ), &, ||) to the buffer.
 *
 * @param sh Aqua highlight context.
 * @param c Symbol character.
 *
 * @return Returns 0 if success, -1 otherwise.
 */
static int add_symbol(struct aqua_sh *sh, char c)
{
	append_string(sh, COLOR_SYMBOL, 0);
	append_char(sh, c);
	return append_string(sh, COLOR_RESET, 0);
}

/**
 * @brief Add a new keyword to the buffer.
 *
 * @param sh Aqua highlight context.
 *
 * @return Returns 0 if success, -1 otherwise.
 */
static int add_keyword(struct aqua_sh *sh)
{
	char keyword[MAX_KEYWORD_SIZE + 1] = {0};
	const char *color = NULL;
	int low, high, mid;
	size_t len;
	int ret;

	len = (size_t)(sh->kw_end - sh->kw_start + 1);
	if (len > MAX_KEYWORD_SIZE)
		goto out;

	memcpy(keyword, sh->kw_start, len);

	low  = 0;
	high = sizeof(kw_list)/sizeof(kw_list[0]);

	/* Do a binary search to find the keyword color. */
	while (low <= high)
	{
		mid = low + (high - low) / 2;
		ret = strcmp(kw_list[mid].keyword, keyword);

		/* Found. */
		if (!ret)
		{
			color = kw_list[mid].color;
			break;
		}

		if (ret < 0)
			low  = mid + 1;
		else
			high = mid - 1;
	}

out:
	if (color)
		append_string(sh, color, 0);

	ret = append_string(sh, sh->kw_start, len);
	if (color)
		ret = append_string(sh, COLOR_RESET, 0);

	return (0);
}

/**
 * @brief Add a 'generic' highlight (like string, comment, number...)
 * to the buffer with the specified @p color.
 *
 * @param sh Aqua highlight context.
 * @param Highlight color.
 *
 * @return Returns 0 if success, -1 otherwise.
 */
static int add_highlight(struct aqua_sh *sh, const char *color)
{
	size_t len = (size_t)(sh->kw_end - sh->kw_start + 1);
	append_string(sh, color, 0);
	append_string(sh, sh->kw_start, len);
	return append_string(sh, COLOR_RESET, 0);
}

/* ========================================================================= */
/*                                 STATES                                    */
/* ========================================================================= */
static int state_default(struct aqua_sh *sh);
static int state_keyword(struct aqua_sh *sh);
static int state_number(struct aqua_sh *sh);
static int state_string_single_line(struct aqua_sh *sh);
static int state_string_multi_line(struct aqua_sh *sh);
static int state_comment(struct aqua_sh *sh);

/**
 * @brief State 'dispatcher'.
 *
 * This routine is responsible to guess any other states that should
 * execute from the default state. Each state is handled separately
 * in its own function.
 *
 * @param sh Aqua highlight context.
 *
 * @return Returns 0 if success, -1 otherwise.
 */
static int state_default(struct aqua_sh *sh)
{
	const char *c = sh->c;

	/*
	 * If potential keyword.
	 *
	 * Obs: This is a valid *C* keyword, not Python keyword....
	 */
	if (is_char_keyword[(int)*c] && !is_digit[(int)*c])
	{
		sh->kw_start = c;
		sh->state = state_keyword;
		return (0);
	}

	/* If potential number. */
	if (is_digit[(int)*c])
	{
		sh->kw_start = c;
		sh->state = state_number;
		return (0);
	}

	/* Char or string. */
	if (*c == '\'' || *c == '"')
	{
		sh->kw_start   =  c;
		sh->start_char = *c;
		sh->state = state_string_single_line;

		/*
		 * Check if multi-line string, if so,
		 * skip these chars too.
		 */
		if (c + 2 < sh->e &&
			((c[1] == '"'  && c[2] == '"') ||
			(c[1] == '\'' && c[2] == '\'')))
		{
			sh->state = state_string_multi_line;
			sh->c += 2;
		}
		return (0);
	}

	/* Comment. */
	if (*c == '#')
	{
		sh->kw_start = c;
		sh->state = state_comment;
		return (0);
	}

	/* Symbol?. */
	if (is_symbol[(int)*c])
		return add_symbol(sh, *c);

	/* Anything else, add to buffer. */
	return append_char(sh, *c);
}

/**
 * @brief Keyword state.
 * Ends when a non-keyword char is detected.
 *
 * @param sh Aqua highlight context.
 *
 * @return Returns 0 if success, -1 otherwise.
 */
static int state_keyword(struct aqua_sh *sh)
{
	/* End of keyword/identifier. */
	if (sh->c == sh->e || !is_char_keyword[(int)*sh->c])
	{
		sh->kw_end = sh->c - 1;
		sh->state = state_default;
		sh->c--; /* come back one position to parse again. */
		return add_keyword(sh);
	}
	return (0);
}

/**
 * @brief Number state.
 * Ends when a not-valid number char is detected.
 *
 * It should be noted that although this might work for valid
 * numbers, this will also works for invalid numbers as well.
 *
 * @param sh Aqua highlight context.
 *
 * @return Returns 0 if success, -1 otherwise.
 */
static int state_number(struct aqua_sh *sh)
{
	char c = tolower(*sh->c);

	/*
	 * All alphabet accepted as 'is_number'.
	 */
	static const int is_number[256] = {
		/* Numbers. */
		['0'] = 1, ['1'] = 1, ['2'] = 1, ['3'] = 1, ['4'] = 1, ['5'] = 1,
		['6'] = 1, ['7'] = 1, ['8'] = 1, ['9'] = 1,
		/* Hexa. */
		['a'] = 1, ['b'] = 1, ['c'] = 1, ['d'] = 1, ['e'] = 1, ['f'] = 1,
		/* jlx. */
		['j'] = 1, ['l'] = 1, ['x'] = 1,
		/* Punctuators. */
		['.'] = 1,
	};

	/*
	 * Should we end the state?
	 *
	 * The laziest number validation ever:
	 * I do not validate as a state machine, but only validates
	 * if the current char matches the valid alphabet for a number.
	 *
	 * This works for any valid numbers, but will work for invalid
	 * numbers as well..... I'm too lazy to implement this correctly.
	 */
	if (sh->c == sh->e || !is_number[(int)c])
	{
		sh->kw_end = sh->c - 1;
		sh->state = state_default;
		sh->c--; /* come back one position to parse again. */
		return add_highlight(sh, COLOR_NUMBER);
	}
	return (0);
}

/**
 * @brief Single-line string (" " and ' ') state.
 * Ends when:
 * - End of line ('\0') or
 * - Closing (" or ') char is detected.
 *
 * @param sh Aqua highlight context.
 *
 * @return Returns 0 if success, -1 otherwise.
 */
static int state_string_single_line(struct aqua_sh *sh)
{
	const char *c = sh->c;

	/*
	 * End string.
	 *
	 * A string must end only if:
	 * - Its in the end of line ('\0') (although state is nor reset)
	 * - Encounter the start character (', ").
	 */
	if (*c == '\0' || c == sh->e - 1 ||
		(*c == sh->start_char && c[-1] != '\\'))
	{
		if (*c == '\0')
			sh->kw_end = sh->c - 1;
		else
			sh->kw_end = sh->c;

		sh->state = state_default;
		return add_highlight(sh, COLOR_STRING);
	}
	return (0);
}

/**
 * @brief Multi-line string state.
 * Ends when a closing (""" or ''') char is detected.
 *
 * @param sh Aqua highlight context.
 *
 * @return Returns 0 if success, -1 otherwise.
 */
static int state_string_multi_line(struct aqua_sh *sh)
{
	const char *c = sh->c;

	/*
	 * End multi-line string.
	 *
	 * A string must end only if:
	 * - Its in the end of line ('\0') (although state is nor reset)
	 * - Encounter the start character (''', """).
	 */
	if (*c == '\0' || c == sh->e || (*c == sh->start_char))
	{
		/* If not in the start of the line. */
		if (c > sh->s)
		{
			/* Check if the char is not escaped: if so, skip. */
			if (c[-1] == '\\')
				return (0);
		}

		if (*c == '\0' || c == sh->e)
			sh->kw_end = sh->c - 1;
		else if (c + 2 < sh->e)
		{
			/* Check if the next 2 chars are equal to the start char,
			 * if not, continue. */
			if (c[1] != sh->start_char || c[2] != sh->start_char)
				return (0);

			sh->state  = state_default;
			sh->kw_end = sh->c + 2;
			sh->c += 2;
		}
		else
			return (0);

		return add_highlight(sh, COLOR_COMMENT);
	}
	return (0);
}

/**
 * @brief Comment state.
 * Ends when the last character is found.
 *
 * @param sh Aqua highlight context.
 *
 * @return Returns 0 if success, -1 otherwise.
 */
static int state_comment(struct aqua_sh *sh)
{
	/* End of line? */
	if (sh->c == sh->e - 1)
	{
		sh->kw_end = sh->c;
		sh->state  = state_default;
		return add_highlight(sh, COLOR_COMMENT);
	}
	return (0);
}

/* ========================================================================= */
/*                             PUBLIC ROUTINES                               */
/* ========================================================================= */

/**
 * @brief Initializes the Aqua syntax highlight context.
 *
 * @param sh Aqua highlight context.
 *
 * @return Returns 0 if success, -1 otherwise.
 */
int aqua_init(struct aqua_sh *sh)
{
	if (!sh)
		return (-1);

	memset(sh, 0, sizeof(*sh));
	sh->state = state_default;

#ifdef AQUA_USE_MALLOC
	sh->buff = calloc(1024, sizeof(char));
	if (!sh->buff)
		return (-1);
	sh->buff_len = 1024;
#endif

	return (0);
}

/**
 * @brief Reset the state-machine.
 *
 * Sometimes a user might want to reset the state-machine
 * without re-initializing everything.
 *
 * @param sh Aqua highlight context.
 *
 * @returns Returns 0 if success, -1 otherwise.
 */
int aqua_reset_state(struct aqua_sh *sh)
{
	if (!sh)
		return (-1);
	sh->state = state_default;
	return (0);
}

/**
 * @brief Given a context @p sh, highlights a null-terminated
 * (or not, provided that the lenght is specified) string
 * pointed by @p buff using ANSI Escape Sequences and
 * returns an equivalent null-terminated string highlighted.
 *
 * @param sh Aqua highlight context.
 *
 * @param buff Buffer to be highlighted. This buffer might be
 * a single line, or an entire source code. The state is
 * kept across invocations, so it's OK to call multiple
 * times to highlight an entire source.
 *
 * @param len If the buffer supplied is in @p buff not null-
 * terminated, it's size must be specified here. Otherwise,
 * len may be 0.
 *
 * It's also OK to highlight an entire source code at once.
 * Providing the buffer *must* be null-terminated.
 *
 * @return If success, returns a buffer containing the
 * highlighted source. Otherwise, returns NULL.
 *
 * @note A couple of notes should be made:
 * Note 1) The buffer is reused across invocations, so a user
 * that wants the keep the returned result between multiple
 * calls to this function *must* make a copy to somewhere else.
 *
 * Note 2) There is also no need to call @ref aqua_finish()
 * every time, but only when you're done highlighting stuff.
 *
 * Note 3) This function is thread-safe, provided that
 * different contexts (struct aqua_sh) are used.
 */
char *aqua_highlight(struct aqua_sh *sh, const char *buff, size_t len)
{
	const char *end;

	/* Guess buffer length. */
	if (!len)
		len = strlen(buff);

	end     = buff + len;
	sh->s   = buff;
	sh->c   = buff;
	sh->e   = end;
	sh->pos = 0;
	sh->kw_start = buff;
	sh->kw_end   = NULL;

	if (!sh || !buff)
		return (NULL);

	for (sh->c = buff; sh->c <= end; sh->c++)
		if (sh->state(sh))
			return (NULL);

	/* Reset states: the only state that should be kept across
	 * multiples invocations is multi-line strings. */
	if (sh->state != state_string_multi_line)
		sh->state = state_default;

	/* Add null terminator. */
	append_char(sh, '\0');

	return (sh->buff);
}

/**
 * @brief Finishes an Aqua context.
 * This should be called only after you're done highlighting.
 *
 * @param sh Aqua highlight context.
 *
 * @note There is no need to call this function
 * if Aqua is built without malloc support, i.e: without
 * defining the macro AQUA_USE_MALLOC.
 */
void aqua_finish(struct aqua_sh *sh)
{
#ifdef AQUA_USE_MALLOC
	free(sh->buff);
#endif
	memset(sh, 0, sizeof(*sh));
}

/* Random tests below, enable to check if the highlight
 * makes sense. */

#if 0
#include <stdio.h>

#define highlight_and_print(sh, str) \
	do {  \
		aqua_highlight((sh),(str), 0); \
		puts((sh)->buff); \
	} while (0) \

int main(void)
{
	struct aqua_sh sh;
	aqua_init(&sh);

	/* Some operators, numbers, strings, comments, escaped strings
	 * keywords... */
	const char STR_1[] = \
		"( ) [ ] ; : < >  456// \"foo\" 'abc\"def' 123 \"\"\"123\"\"\" " \
		"\"\\\"bar\\\"\" print(bar) #deadbeef ( ) ;";

	/* Some 'real' code. */
	const char STR_2[] = \
		"data = ' '.join([str(elem) for elem in contents])";

	highlight_and_print(&sh, STR_1);
	highlight_and_print(&sh, STR_2);

	/* Non-identified keyword, recognizable function, operators
	 * and numbers. */
	highlight_and_print(&sh, "hello print(1+0xdeadbeef)");

	/* Multi-line comments with mixed terminator (just to be sure)
	 * the ending should match the beginning. */
	highlight_and_print(&sh, "\"\"\"");
	highlight_and_print(&sh, "Hello ''' World");
	highlight_and_print(&sh, "123");
	highlight_and_print(&sh, "\"\"\"");

	/* Some keyword again. */
	highlight_and_print(&sh, "import");

	aqua_finish(&sh);
	return (0);
}
#endif
