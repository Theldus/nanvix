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

#ifndef AQUA_H
#define AQUA_H

#ifdef __cplusplus
extern "C" {
#endif

	/* Enable to disable malloc support and enable dinamically
	 * allocated buffer. */
#if 0
	#define AQUA_USE_MALLOC
#endif

	/* Maximum highlighted line len, when built without malloc. */
	#define MAX_LINE 4095

	/* Syntax highlight context. */
	struct aqua_sh
	{
		int (*state)(struct aqua_sh *);
#ifndef AQUA_USE_MALLOC
		char buff[MAX_LINE + 1];
#else
		char  *buff;
		size_t buff_len;
#endif
		const char *kw_start;
		const char *kw_end;
		const char *s;
		const char *c;   /* ptr of current position. */
		const char *e;   /* end of line ('\0').      */
		char start_char; /* start char of a string.  */
		size_t pos;
	};

	extern int aqua_init(struct aqua_sh *sh);
	extern int aqua_reset_state(struct aqua_sh *sh);
	extern char *aqua_highlight(struct aqua_sh *sh, const char *line,
		size_t len);
	extern void aqua_finish(struct aqua_sh *sh);

#ifdef __cplusplus
}
#endif

#endif /* AQUA_H */
