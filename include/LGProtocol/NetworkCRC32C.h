/**
 * LGProtocol - Looking Glass Communication Protocol
 * Copyright © 2017-2026 Geoffrey McRae <geoff@hostfission.com>
 * https://github.com/gnif/LGProtocol
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc., 59
 * Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef LGPROTOCOL_NETWORK_CRC32C_H
#define LGPROTOCOL_NETWORK_CRC32C_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LG_NET_CRC32C_INITIAL UINT32_C(0xFFFFFFFF)

/* Update operates on an unfinalized Castagnoli state. Begin with INITIAL and
 * call Finish after the final byte. The one-shot helper performs both steps. */
bool lgNetCRC32CUpdate(uint32_t * state, const void * data, size_t size);
uint32_t lgNetCRC32CFinish(uint32_t state);
bool lgNetCRC32C(const void * data, size_t size, uint32_t * checksum);

#ifdef __cplusplus
}
#endif

#endif
