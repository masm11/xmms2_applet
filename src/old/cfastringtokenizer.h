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
 * $Id: cfastringtokenizer.h 287 2005-06-25 12:03:54Z masm $
 */

#ifndef CFA_STRINGTOKENIZER_H__INCLUDED
#define CFA_STRINGTOKENIZER_H__INCLUDED

#include <common.h>
#include <cfasocket.h>
#include <cfasocketreceivebuffer.h>
#include <cfasocketsendbuffer.h>

G_BEGIN_DECLS

#define	CFA_TYPE_STRING_TOKENIZER				cfa_string_tokenizer_get_type()
#define CFA_STRING_TOKENIZER(string_tokenizer)		G_TYPE_CHECK_INSTANCE_CAST((string_tokenizer), CFA_TYPE_STRING_TOKENIZER, CfaStringTokenizer)
#define CFA_STRING_TOKENIZER_CLASS(klass)			G_TYPE_CHECK_CLASS_CAST((klass), CFA_TYPE_STRING_TOKENIZER, CfaStringTokenizerClass)
#define CFA_IS_STRING_TOKENIZER(string_tokenizer)		G_TYPE_CHECK_INSTANCE_TYPE((string_tokenizer), CFA_TYPE_STRING_TOKENIZER)
#define CFA_IS_STRING_TOKENIZER_CLASS(klass)		G_TYPE_CHECK_CLASS_TYPE((klass), CFA_TYPE_STRING_TOKENIZER)
#define	CFA_STRING_TOKENIZER_GET_CLASS(string_tokenizer)	G_TYPE_INSTANCE_GET_CLASS((string_tokenizer), CFA_TYPE_STRING_TOKENIZER, CfaStringTokenizerClass)

typedef struct _CfaStringTokenizer CfaStringTokenizer;
typedef struct _CfaStringTokenizerClass CfaStringTokenizerClass;

struct _CfaStringTokenizer {
    GObject parent_instance;
    
    GList *list;
};

struct _CfaStringTokenizerClass {
    CfaSocketClass parent_class;
};

GType			cfa_string_tokenizer_get_type	(void) G_GNUC_CONST;
CfaStringTokenizer	*cfa_string_tokenizer_new	(const gchar *string);
gchar			*cfa_string_tokenizer_next	(CfaStringTokenizer *stk);
const gchar		*cfa_string_tokenizer_peek	(CfaStringTokenizer *stk);
gint			cfa_string_tokenizer_count	(CfaStringTokenizer *stk);

G_END_DECLS

#endif /* ifndef CFA_STRINGTOKENIZER_H__INCLUDED */
