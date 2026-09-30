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

#include <LGProtocol/NetworkReassembly.h>

#include <LGProtocol/NetworkCRC32C.h>

#include <string.h>

static bool storageValid(const LGNetReassembly * reassembly)
{
  return reassembly && reassembly->data && reassembly->capacity &&
    reassembly->ranges && reassembly->rangeCapacity;
}

static uint32_t rangeEnd(const LGNetReassemblyRange * range)
{
  return range->offset + range->length;
}

static bool dataComplete(const LGNetReassembly * reassembly)
{
  return reassembly->receivedLength == reassembly->totalLength;
}

static bool validateChecksum(LGNetReassembly * reassembly)
{
  if (!dataComplete(reassembly))
    return false;
  if (reassembly->checksumChecked)
    return reassembly->checksumValid;

  reassembly->checksumChecked = true;
  reassembly->checksumValid   = true;
  if (!reassembly->hasChecksum)
    return true;

  uint32_t checksum;
  if (!lgNetCRC32C(reassembly->data, reassembly->totalLength, &checksum))
  {
    reassembly->checksumValid = false;
    return false;
  }

  reassembly->checksumValid = checksum == reassembly->checksum;
  return reassembly->checksumValid;
}

bool lgNetReassemblyInit(LGNetReassembly * reassembly, void * data,
  size_t capacity, LGNetReassemblyRange * ranges, size_t rangeCapacity)
{
  if (!reassembly || !data || !capacity || !ranges || !rangeCapacity)
    return false;

  memset(reassembly, 0, sizeof(*reassembly));
  reassembly->data          = (uint8_t *)data;
  reassembly->capacity      = capacity;
  reassembly->ranges        = ranges;
  reassembly->rangeCapacity = rangeCapacity;
  return true;
}

bool lgNetReassemblyBegin(LGNetReassembly * reassembly,
  const LGNetReassemblyConfig * config)
{
  const uint32_t emptyChecksum =
    lgNetCRC32CFinish(LG_NET_CRC32C_INITIAL);
  if (!storageValid(reassembly) || !config || !config->sessionEpoch ||
      !config->componentEpoch || !config->objectID ||
      config->totalLength > reassembly->capacity ||
      !config->fragmentLimit ||
      (!config->totalLength && config->hasChecksum &&
        config->checksum != emptyChecksum))
    return false;

  reassembly->rangeCount       = 0;
  reassembly->sessionEpoch     = config->sessionEpoch;
  reassembly->componentEpoch   = config->componentEpoch;
  reassembly->objectID         = config->objectID;
  reassembly->totalLength      = config->totalLength;
  reassembly->receivedLength   = 0;
  reassembly->fragmentLimit    = config->fragmentLimit;
  reassembly->fragmentCount    = 0;
  reassembly->checksum         = config->checksum;
  reassembly->hasChecksum      = config->hasChecksum;
  reassembly->checksumChecked  = !config->totalLength;
  reassembly->checksumValid    = !config->totalLength &&
    (!config->hasChecksum || config->checksum == emptyChecksum);
  reassembly->active           = true;
  return true;
}

void lgNetReassemblyReset(LGNetReassembly * reassembly)
{
  if (!reassembly)
    return;

  reassembly->rangeCount      = 0;
  reassembly->sessionEpoch    = 0;
  reassembly->componentEpoch  = 0;
  reassembly->objectID        = 0;
  reassembly->totalLength     = 0;
  reassembly->receivedLength  = 0;
  reassembly->fragmentLimit   = 0;
  reassembly->fragmentCount   = 0;
  reassembly->checksum        = 0;
  reassembly->hasChecksum     = false;
  reassembly->checksumChecked = false;
  reassembly->checksumValid   = false;
  reassembly->active          = false;
}

static LGNetReassemblyResult duplicateResult(
  LGNetReassembly * reassembly, uint32_t offset, const void * data,
  uint32_t length)
{
  ++reassembly->fragmentCount;
  if (memcmp(reassembly->data + offset, data, length) != 0)
    return LG_NET_REASSEMBLY_OVERLAP;
  return LG_NET_REASSEMBLY_DUPLICATE;
}

LGNetReassemblyResult lgNetReassemblyAdd(LGNetReassembly * reassembly,
  uint64_t sessionEpoch, uint64_t componentEpoch, uint64_t objectID,
  uint32_t offset, const void * data, uint32_t length)
{
  if (!storageValid(reassembly) || !reassembly->active || !data || !length)
    return LG_NET_REASSEMBLY_INVALID;
  if (sessionEpoch != reassembly->sessionEpoch ||
      componentEpoch != reassembly->componentEpoch ||
      objectID != reassembly->objectID)
    return LG_NET_REASSEMBLY_WRONG_OBJECT;
  if (offset > reassembly->totalLength ||
      length > reassembly->totalLength - offset)
    return LG_NET_REASSEMBLY_OUT_OF_BOUNDS;
  if (reassembly->fragmentCount >= reassembly->fragmentLimit)
    return LG_NET_REASSEMBLY_FRAGMENT_LIMIT;

  const uint32_t end       = offset + length;
  size_t         insertion = 0;
  for (; insertion < reassembly->rangeCount; ++insertion)
  {
    const LGNetReassemblyRange * range  = &reassembly->ranges[insertion];
    const uint32_t               oldEnd = rangeEnd(range);
    if (end <= range->offset)
      break;
    if (offset >= oldEnd)
      continue;
    if (offset >= range->offset && end <= oldEnd)
      return duplicateResult(reassembly, offset, data, length);
    return LG_NET_REASSEMBLY_OVERLAP;
  }

  const bool joinLeft  = insertion &&
    rangeEnd(&reassembly->ranges[insertion - 1U]) == offset;
  const bool joinRight = insertion < reassembly->rangeCount &&
    end == reassembly->ranges[insertion].offset;
  if (!joinLeft && !joinRight &&
      reassembly->rangeCount >= reassembly->rangeCapacity)
    return LG_NET_REASSEMBLY_RANGE_LIMIT;

  memmove(reassembly->data + offset, data, length);

  if (joinLeft)
  {
    LGNetReassemblyRange * left = &reassembly->ranges[insertion - 1U];
    left->length += length;
    if (joinRight)
    {
      left->length += reassembly->ranges[insertion].length;
      memmove(&reassembly->ranges[insertion],
        &reassembly->ranges[insertion + 1U],
        (reassembly->rangeCount - insertion - 1U) *
          sizeof(reassembly->ranges[0]));
      --reassembly->rangeCount;
    }
  }
  else if (joinRight)
  {
    LGNetReassemblyRange * right = &reassembly->ranges[insertion];
    right->offset  = offset;
    right->length  += length;
  }
  else
  {
    memmove(&reassembly->ranges[insertion + 1U],
      &reassembly->ranges[insertion],
      (reassembly->rangeCount - insertion) *
        sizeof(reassembly->ranges[0]));
    reassembly->ranges[insertion].offset = offset;
    reassembly->ranges[insertion].length = length;
    ++reassembly->rangeCount;
  }

  reassembly->receivedLength += length;
  ++reassembly->fragmentCount;
  if (!dataComplete(reassembly))
    return LG_NET_REASSEMBLY_ADDED;
  if (!validateChecksum(reassembly))
    return LG_NET_REASSEMBLY_CHECKSUM_MISMATCH;
  return LG_NET_REASSEMBLY_COMPLETE;
}

bool lgNetReassemblyComplete(const LGNetReassembly * reassembly)
{
  return storageValid(reassembly) && reassembly->active &&
    dataComplete(reassembly) && reassembly->checksumChecked &&
    reassembly->checksumValid;
}

const uint8_t * lgNetReassemblyData(const LGNetReassembly * reassembly,
  uint32_t * size)
{
  if (!size || !lgNetReassemblyComplete(reassembly))
    return NULL;

  *size = reassembly->totalLength;
  return reassembly->data;
}
