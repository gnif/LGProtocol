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

#ifndef LGPROTOCOL_NETWORK_REASSEMBLY_H
#define LGPROTOCOL_NETWORK_REASSEMBLY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum LGNetReassemblyResult
{
  LG_NET_REASSEMBLY_ADDED,
  LG_NET_REASSEMBLY_COMPLETE,
  LG_NET_REASSEMBLY_DUPLICATE,
  LG_NET_REASSEMBLY_INVALID,
  LG_NET_REASSEMBLY_WRONG_OBJECT,
  LG_NET_REASSEMBLY_OUT_OF_BOUNDS,
  LG_NET_REASSEMBLY_OVERLAP,
  LG_NET_REASSEMBLY_RANGE_LIMIT,
  LG_NET_REASSEMBLY_FRAGMENT_LIMIT,
  LG_NET_REASSEMBLY_CHECKSUM_MISMATCH,
}
LGNetReassemblyResult;

typedef struct LGNetReassemblyRange
{
  uint32_t offset;
  uint32_t length;
}
LGNetReassemblyRange;

typedef struct LGNetReassemblyConfig
{
  uint64_t sessionEpoch;
  uint64_t componentEpoch;
  uint64_t objectID;
  uint32_t totalLength;
  uint32_t fragmentLimit;
  uint32_t checksum;
  bool     hasChecksum;
}
LGNetReassemblyConfig;

typedef struct LGNetReassembly
{
  uint8_t *              data;
  size_t                 capacity;
  LGNetReassemblyRange * ranges;
  size_t                 rangeCapacity;
  size_t                 rangeCount;
  uint64_t               sessionEpoch;
  uint64_t               componentEpoch;
  uint64_t               objectID;
  uint32_t               totalLength;
  uint32_t               receivedLength;
  uint32_t               fragmentLimit;
  uint32_t               fragmentCount;
  uint32_t               checksum;
  bool                   hasChecksum;
  bool                   checksumChecked;
  bool                   checksumValid;
  bool                   active;
}
LGNetReassembly;

/* Payload and range storage are caller-owned. Disjoint ranges are bounded by
 * rangeCapacity and every accepted fragment is bounded by fragmentLimit. */
bool lgNetReassemblyInit(LGNetReassembly * reassembly, void * data,
  size_t capacity, LGNetReassemblyRange * ranges, size_t rangeCapacity);
bool lgNetReassemblyBegin(LGNetReassembly * reassembly,
  const LGNetReassemblyConfig * config);
void lgNetReassemblyReset(LGNetReassembly * reassembly);
LGNetReassemblyResult lgNetReassemblyAdd(LGNetReassembly * reassembly,
  uint64_t sessionEpoch, uint64_t componentEpoch, uint64_t objectID,
  uint32_t offset, const void * data, uint32_t length);
bool lgNetReassemblyComplete(const LGNetReassembly * reassembly);
const uint8_t * lgNetReassemblyData(const LGNetReassembly * reassembly,
  uint32_t * size);

#ifdef __cplusplus
}
#endif

#endif
