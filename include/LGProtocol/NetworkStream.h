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

#ifndef LGPROTOCOL_NETWORK_STREAM_H
#define LGPROTOCOL_NETWORK_STREAM_H

#include "NetworkProtocol.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum LGNetStreamResult
{
  LG_NET_STREAM_PACKET,
  LG_NET_STREAM_NEED_DATA,
  LG_NET_STREAM_INVALID_PACKET,
  LG_NET_STREAM_TOO_LARGE,
}
LGNetStreamResult;

typedef struct LGNetSpan
{
  const uint8_t * data;
  size_t          size;
}
LGNetSpan;

/* A packet payload can wrap once in the parser's caller-owned ring buffer. */
typedef struct LGNetStreamPacket
{
  LGNetEnvelope envelope;
  LGNetSpan     payload[2];
  size_t        payloadLength;
  size_t        wireSize;
}
LGNetStreamPacket;

typedef struct LGNetStreamParser
{
  uint8_t * data;
  size_t    capacity;
  size_t    head;
  size_t    used;
  size_t    pendingWireSize;
  uint32_t  maxPayloadLength;
}
LGNetStreamParser;

/* Append can accept a prefix when the ring is full. Packet spans remain valid
 * until Consume or Reset; parsing never shifts buffered packet bytes. */
bool lgNetStreamParserInit(LGNetStreamParser * parser, void * buffer,
  size_t capacity, uint32_t maxPayloadLength);
void lgNetStreamParserReset(LGNetStreamParser * parser);
size_t lgNetStreamParserBuffered(const LGNetStreamParser * parser);
size_t lgNetStreamParserWritable(const LGNetStreamParser * parser);
bool lgNetStreamParserAppend(LGNetStreamParser * parser, const void * data,
  size_t size, size_t * accepted);
LGNetStreamResult lgNetStreamParserNext(LGNetStreamParser * parser,
  LGNetStreamPacket * packet, LGNetParseResult * parseResult);
bool lgNetStreamParserConsume(LGNetStreamParser * parser);

#ifdef __cplusplus
}
#endif

#endif
