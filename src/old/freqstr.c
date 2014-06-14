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
 * $Id: freqstr.c 298 2005-06-25 13:55:58Z masm $
 */

#include <common.h>
#include <glib.h>
#include <freqstr.h>
#include <stdlib.h>
#include <string.h>

/**
 * cfa_freqstr_to_value:
 * @str: a string.
 *
 * Converts the string to a frequency.
 * 
 * This function accepts numbers with a decimal point, and a unit.
 * 
 * Returns: frequency in Hz.
 */
freq_t cfa_freqstr_to_value(const gchar *str)
{
    gdouble val;
    gchar *ep;
    
    val = strtod(str, &ep);
    if (ep != NULL && *ep != '\0') {
	if (*ep == 'k' && *(ep + 1) == '\0') {
	    val *= 1000;
	} else if (*ep == 'K' && *(ep + 1) == '\0') {
	    val *= 1000;
	} else if (*ep == 'M' && *(ep + 1) == '\0') {
	    val *= 1000000;
	} else if (*ep == 'G' && *(ep + 1) == '\0') {
	    val *= 1000000000;
	} else if (*ep == 'T' && *(ep + 1) == '\0') {
	    val *= INT64_C(1000000000000);
	} else {
	    return -1;
	}
    } else {
    }
    
    return val + 0.5;
}

/**
 * cfa_freqstr_to_string:
 * @freq: a frequency in Hz.
 *
 * Converts the frequency to a string.
 * This function may uses a decimal point and a unit.
 * 
 * You own the returned string.
 * 
 * Returns: string.
 */
gchar *cfa_freqstr_to_string(freq_t freq)
{
    gchar geta[8];
    gdouble num;
    
    if (freq < 0)
	freq = 0;
    
    if (freq < 1000) {
	strcpy(geta, "");
	num = freq;
    } else if (freq < 1000000) {
	strcpy(geta, "k");
	num = freq / 1000.0;
    } else if (freq < 1000000000) {
	strcpy(geta, "M");
	num = freq / 1000000.0;
    } else {
	strcpy(geta, "G");
	num = freq / 1000000000.0;
    }
    
    if (num == 0) {
	return g_strdup_printf("%d%s", (gint) num, geta);
    } else if (num < 10.0) {
	return g_strdup_printf("%.2f%s", num, geta);
    } else if (num < 100.0) {
	return g_strdup_printf("%.1f%s", num, geta);
    } else {
	return g_strdup_printf("%d%s", (gint) num, geta);
    }
}

/*EOF*/
