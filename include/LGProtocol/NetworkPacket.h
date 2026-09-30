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

#ifndef LGPROTOCOL_NETWORK_PACKET_H
#define LGPROTOCOL_NETWORK_PACKET_H

#include "NetworkProtocol.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct LGNetPacketParameters
{
  LGNetService       service;
  uint16_t           messageType;
  uint16_t           serviceVersion;
  uint16_t           messageVersion;
  LGNetEnvelopeFlags flags;
  uint64_t           sessionEpoch;
  uint64_t           componentEpoch;
  uint64_t           sequence;
  uint64_t           requestID;
}
LGNetPacketParameters;

typedef struct LGNetPacketBuilder
{
  uint8_t *     data;
  size_t        capacity;
  LGNetEnvelope envelope;
  LGNetWriter   payload;
  bool          active;
}
LGNetPacketBuilder;

/* The builder writes into caller-owned storage. The payload writer remains
 * valid until Finish and is bounded by both capacity and the protocol limit. */
void lgNetPacketParametersInit(LGNetPacketParameters * parameters,
  LGNetService service, uint16_t messageType, uint16_t serviceVersion,
  uint16_t messageVersion);
bool lgNetPacketBuilderInit(LGNetPacketBuilder * builder, void * data,
  size_t capacity, const LGNetPacketParameters * parameters);
LGNetWriter * lgNetPacketBuilderPayload(LGNetPacketBuilder * builder);
bool lgNetPacketBuilderFinish(LGNetPacketBuilder * builder,
  size_t * wireSize);
bool lgNetPacketConstruct(void * data, size_t capacity, size_t * wireSize,
  const LGNetPacketParameters * parameters, const void * payload,
  size_t payloadSize);

#ifdef __cplusplus
}
#endif

#endif
