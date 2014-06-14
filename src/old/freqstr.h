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
 * $Id: freqstr.h 287 2005-06-25 12:03:54Z masm $
 */

#ifndef FREQSTR_H__INCLUDED
#define FREQSTR_H__INCLUDED

#include <common.h>
#include <glib.h>

freq_t cfa_freqstr_to_value(const gchar *str);
gchar *cfa_freqstr_to_string(freq_t freq);

#endif	/* ifndef FREQSTR_H__INCLUDED */
