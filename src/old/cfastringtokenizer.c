/* CPUFreq Applet
 *  Copyright (C) 2004-2005 Yuuki Harano
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * $Id: cfastringtokenizer.c 312 2005-07-01 12:48:15Z masm $
 */

#include <common.h>
#include <cfastringtokenizer.h>
#include <free_string_list.h>
#include <string.h>
#include <ctype.h>

G_DEFINE_TYPE(CfaStringTokenizer, cfa_string_tokenizer, G_TYPE_OBJECT);

static void	cfa_string_tokenizer_finalize	(GObject *object);

static GList *parse_string(const gchar *str);
static gchar *get_string(const gchar **p0);

static void cfa_string_tokenizer_class_init(CfaStringTokenizerClass *class)
{
    GObjectClass *object_class = G_OBJECT_CLASS(class);
    
    object_class->finalize = cfa_string_tokenizer_finalize;
}

static void cfa_string_tokenizer_init(CfaStringTokenizer *stk)
{
    xlog_debug(1, "cfa_string_tokenizer_init:");
}

static void cfa_string_tokenizer_finalize(GObject *object)
{
    CfaStringTokenizer *stk;
    
    xlog_debug(1, "cfa_string_tokenizer_finalize:");
    
    g_return_if_fail(CFA_IS_STRING_TOKENIZER(object));
    
    stk = CFA_STRING_TOKENIZER(object);
    
    cfa_free_string_list(stk->list);
    stk->list = NULL;
    
    (*G_OBJECT_CLASS(cfa_string_tokenizer_parent_class)->finalize)(object);
}

/**
 * cfa_string_tokenizer_new:
 * @string: a string.
 * 
 * Parse the string to tokens.
 * 
 * Returns: a tokenizer.
 */
CfaStringTokenizer *cfa_string_tokenizer_new(const gchar *string)
{
    CfaStringTokenizer *stk;
    
    xlog_debug(1, "cfa_string_tokenizer_new: %s", string);
    
    stk = g_object_new(CFA_TYPE_STRING_TOKENIZER, NULL);
    
    stk->list = parse_string(string);
    
    return stk;
}

/**
 * cfa_string_tokenizer_next:
 * @stk: a tokenizer.
 * 
 * Get a token from the tokenizer.
 * 
 * Returns: a token.
 */
gchar *cfa_string_tokenizer_next(CfaStringTokenizer *stk)
{
    gchar *r;
    
    if (stk->list == NULL)
	return NULL;
    
    r = (gchar *) stk->list->data;
    stk->list = g_list_delete_link(stk->list, stk->list);
    
    return r;
}

/**
 * cfa_string_tokenizer_peek:
 * @stk: a tokenizer.
 * 
 * Peek a token from the tokenizer.
 * 
 * Returns: a token.
 */
const gchar *cfa_string_tokenizer_peek(CfaStringTokenizer *stk)
{
    if (stk->list == NULL)
	return NULL;
    return (gchar *) stk->list->data;
}

/**
 * cfa_string_tokenizer_count:
 * @stk: a tokenizer.
 *
 * Count the number of tokens in the tokenizer.
 *
 * Returns: the number of tokens.
 */
gint cfa_string_tokenizer_count(CfaStringTokenizer *stk)
{
    return g_list_length(stk->list);
}

static GList *parse_string(const gchar *str)
{
    GList *list = NULL;
    const gchar *p = str;
    
    while (1) {
	gchar *s;
	
	if (*p == '\0')
	    break;
	
	while (isspace(*p))
	    p++;
	
	if (*p == '\0')
	    break;
	
	s = get_string(&p);
	if (s == NULL)
	    break;
	
	list = g_list_append(list, s);
    }
    
    return list;
}

static gchar *get_string(const gchar **p0)
{
    const gchar *beg, *end;
    const gchar *p = *p0;
    gchar *newstr;
    
    beg = p;
    while (*p != '\0') {
	if (isspace(*p))
	    break;
	p++;
    }
    end = p;
    
    newstr = g_new(gchar, end - beg + 1);
    memcpy(newstr, beg, end - beg);
    newstr[end - beg] = '\0';
    
    *p0 = p;
    
    return newstr;
}

/*EOF*/
