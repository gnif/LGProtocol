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

#include <LGProtocol/NetworkCRC32C.h>

static const uint32_t CRC32C_TABLE[16] =
{
  UINT32_C(0x00000000), UINT32_C(0x105EC76F),
  UINT32_C(0x20BD8EDE), UINT32_C(0x30E349B1),
  UINT32_C(0x417B1DBC), UINT32_C(0x5125DAD3),
  UINT32_C(0x61C69362), UINT32_C(0x7198540D),
  UINT32_C(0x82F63B78), UINT32_C(0x92A8FC17),
  UINT32_C(0xA24BB5A6), UINT32_C(0xB21572C9),
  UINT32_C(0xC38D26C4), UINT32_C(0xD3D3E1AB),
  UINT32_C(0xE330A81A), UINT32_C(0xF36E6F75),
};

bool lgNetCRC32CUpdate(uint32_t * state, const void * data, size_t size)
{
  if (!state || (size && !data))
    return false;

  const uint8_t * bytes = (const uint8_t *)data;
  uint32_t        crc   = *state;

  for (size_t index = 0; index < size; ++index)
  {
    crc  ^= bytes[index];
    crc  = (crc >> 4) ^ CRC32C_TABLE[crc & UINT32_C(0x0F)];
    crc  = (crc >> 4) ^ CRC32C_TABLE[crc & UINT32_C(0x0F)];
  }

  *state = crc;
  return true;
}

uint32_t lgNetCRC32CFinish(uint32_t state)
{
  return state ^ UINT32_C(0xFFFFFFFF);
}

bool lgNetCRC32C(const void * data, size_t size, uint32_t * checksum)
{
  if (!checksum)
    return false;

  uint32_t state = LG_NET_CRC32C_INITIAL;
  if (!lgNetCRC32CUpdate(&state, data, size))
    return false;

  *checksum = lgNetCRC32CFinish(state);
  return true;
}
