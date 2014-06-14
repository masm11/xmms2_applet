/* CPUFreqApplet
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
 * $Id: appconf.h 287 2005-06-25 12:03:54Z masm $
 */

#ifndef APPCONF_H__INCLUDED
#define APPCONF_H__INCLUDED

#include <common.h>
#include "cfapreferences.h"

void appconf_load(struct app_t *app);
void appconf_save(struct app_t *app);

#endif	/* ifndef APPCONF_H__INCLUDED */
