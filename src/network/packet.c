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

#include <LGProtocol/NetworkPacket.h>

#include <limits.h>
#include <string.h>

void lgNetPacketParametersInit(LGNetPacketParameters * parameters,
  LGNetService service, uint16_t messageType, uint16_t serviceVersion,
  uint16_t messageVersion)
{
  if (!parameters)
    return;

  memset(parameters, 0, sizeof(*parameters));
  parameters->service        = service;
  parameters->messageType    = messageType;
  parameters->serviceVersion = serviceVersion;
  parameters->messageVersion = messageVersion;
}

bool lgNetPacketBuilderInit(LGNetPacketBuilder * builder, void * data,
  size_t capacity, const LGNetPacketParameters * parameters)
{
  if (!builder || !data || !parameters ||
      capacity < LG_NET_ENVELOPE_WIRE_SIZE ||
      !parameters->service || !parameters->messageType ||
      !parameters->serviceVersion || !parameters->messageVersion)
    return false;

  memset(builder, 0, sizeof(*builder));
  builder->data     = (uint8_t *)data;
  builder->capacity = capacity;
  builder->active   = true;

  lgNetEnvelopeInit(&builder->envelope, parameters->service,
    parameters->messageType, parameters->serviceVersion,
    parameters->messageVersion);
  builder->envelope.flags          = parameters->flags;
  builder->envelope.sessionEpoch   = parameters->sessionEpoch;
  builder->envelope.componentEpoch = parameters->componentEpoch;
  builder->envelope.sequence       = parameters->sequence;
  builder->envelope.requestID      = parameters->requestID;

  size_t payloadCapacity = capacity - LG_NET_ENVELOPE_WIRE_SIZE;
  if (payloadCapacity > LG_NET_MAX_PAYLOAD_LENGTH)
    payloadCapacity = LG_NET_MAX_PAYLOAD_LENGTH;

  lgNetWriterInit(&builder->payload,
    builder->data + LG_NET_ENVELOPE_WIRE_SIZE, payloadCapacity);
  return true;
}

LGNetWriter * lgNetPacketBuilderPayload(LGNetPacketBuilder * builder)
{
  if (!builder || !builder->active)
    return NULL;

  return &builder->payload;
}

bool lgNetPacketBuilderFinish(LGNetPacketBuilder * builder,
  size_t * wireSize)
{
  if (!builder || !builder->active || !wireSize ||
      !lgNetWriterValid(&builder->payload))
    return false;

  const size_t payloadSize = lgNetWriterSize(&builder->payload);
  if (payloadSize > UINT32_MAX)
    return false;

  builder->envelope.payloadLength = (uint32_t)payloadSize;

  size_t packetSize;
  if (!lgNetPacketSize(&builder->envelope, &packetSize) ||
      packetSize > builder->capacity ||
      !lgNetEnvelopeEncode(builder->data, LG_NET_ENVELOPE_WIRE_SIZE,
        &builder->envelope))
    return false;

  builder->active = false;
  *wireSize       = packetSize;
  return true;
}

bool lgNetPacketConstruct(void * data, size_t capacity, size_t * wireSize,
  const LGNetPacketParameters * parameters, const void * payload,
  size_t payloadSize)
{
  if ((payloadSize && !payload) || payloadSize > UINT32_MAX)
    return false;

  LGNetPacketBuilder builder;
  if (!lgNetPacketBuilderInit(&builder, data, capacity, parameters) ||
      !lgNetWriterBytes(&builder.payload, payload, payloadSize))
    return false;

  return lgNetPacketBuilderFinish(&builder, wireSize);
}
