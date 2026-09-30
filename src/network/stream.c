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

#include <LGProtocol/NetworkStream.h>

#include <string.h>

static const LGNetStreamBindingFlags STREAM_BINDING_FLAGS =
  LG_NET_STREAM_BINDING_UNIDIRECTIONAL |
  LG_NET_STREAM_BINDING_BULK;

void lgNetStreamBindingInit(LGNetStreamBinding * binding,
    LGNetService service, LGNetRole originRole, uint64_t streamID,
    uint64_t sessionEpoch, LGNetStreamBindingFlags flags)
{
  if (!binding)
    return;

  memset(binding, 0, sizeof(*binding));
  binding->version      = LG_NET_STREAM_BINDING_VERSION;
  binding->headerSize   = LG_NET_STREAM_BINDING_WIRE_SIZE;
  binding->service      = service;
  binding->originRole   = originRole;
  binding->flags        = flags;
  binding->streamID     = streamID;
  binding->sessionEpoch = sessionEpoch;
}

bool lgNetStreamBindingValid(const LGNetStreamBinding * binding)
{
  return binding &&
    binding->version == LG_NET_STREAM_BINDING_VERSION &&
    binding->headerSize == LG_NET_STREAM_BINDING_WIRE_SIZE &&
    binding->service > LG_NET_SERVICE_CORE &&
    binding->service <= LG_NET_SERVICE_CONTROL &&
    (binding->originRole == LG_NET_ROLE_CLIENT ||
      binding->originRole == LG_NET_ROLE_SERVER) &&
    !(binding->flags & ~STREAM_BINDING_FLAGS) &&
    (binding->flags & LG_NET_STREAM_BINDING_UNIDIRECTIONAL) &&
    binding->streamID && binding->sessionEpoch;
}

bool lgNetStreamBindingEncode(void * data, size_t size,
    const LGNetStreamBinding * binding)
{
  if (!data || size < LG_NET_STREAM_BINDING_WIRE_SIZE ||
      !lgNetStreamBindingValid(binding))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU32(&writer, LG_NET_STREAM_BINDING_MAGIC) &&
    lgNetWriterU16(&writer, binding->version)            &&
    lgNetWriterU16(&writer, binding->headerSize)         &&
    lgNetWriterU16(&writer, binding->service)            &&
    lgNetWriterU16(&writer, binding->originRole)         &&
    lgNetWriterU32(&writer, binding->flags)              &&
    lgNetWriterU64(&writer, binding->streamID)           &&
    lgNetWriterU64(&writer, binding->sessionEpoch)       &&
    lgNetWriterSize(&writer) == LG_NET_STREAM_BINDING_WIRE_SIZE;
}

LGNetParseResult lgNetStreamBindingDecode(LGNetStreamBinding * binding,
    const void * data, size_t size)
{
  if (!binding || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_STREAM_BINDING_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;
  if (size != LG_NET_STREAM_BINDING_WIRE_SIZE)
    return LG_NET_PARSE_INVALID_LENGTH;

  LGNetStreamBinding decoded;
  LGNetReader        reader;
  uint32_t           magic;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU32(&reader, &magic)                 ||
      !lgNetReaderU16(&reader, &decoded.version)       ||
      !lgNetReaderU16(&reader, &decoded.headerSize)    ||
      !lgNetReaderU16(&reader, &decoded.service)       ||
      !lgNetReaderU16(&reader, &decoded.originRole)    ||
      !lgNetReaderU32(&reader, &decoded.flags)         ||
      !lgNetReaderU64(&reader, &decoded.streamID)      ||
      !lgNetReaderU64(&reader, &decoded.sessionEpoch)  ||
      lgNetReaderConsumed(&reader) != LG_NET_STREAM_BINDING_WIRE_SIZE)
    return LG_NET_PARSE_INVALID_LENGTH;

  if (magic != LG_NET_STREAM_BINDING_MAGIC)
    return LG_NET_PARSE_INVALID_MAGIC;
  if (!lgNetStreamBindingValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *binding = decoded;
  return LG_NET_PARSE_OK;
}

static bool parserValid(const LGNetStreamParser * parser)
{
  return parser && parser->data &&
    parser->capacity >= LG_NET_ENVELOPE_WIRE_SIZE &&
    parser->head < parser->capacity && parser->used <= parser->capacity &&
    parser->pendingWireSize <= parser->used &&
    parser->maxPayloadLength <= LG_NET_MAX_PAYLOAD_LENGTH;
}

static size_t ringAdvance(size_t offset, size_t amount, size_t capacity)
{
  amount %= capacity;
  if (amount >= capacity - offset)
    return amount - (capacity - offset);

  return offset + amount;
}

static void ringRead(const LGNetStreamParser * parser, size_t offset,
  void * output, size_t size)
{
  const size_t start = ringAdvance(parser->head, offset, parser->capacity);
  size_t       first = parser->capacity - start;
  if (first > size)
    first = size;

  memcpy(output, parser->data + start, first);
  memcpy((uint8_t *)output + first, parser->data, size - first);
}

bool lgNetStreamParserInit(LGNetStreamParser * parser, void * buffer,
  size_t capacity, uint32_t maxPayloadLength)
{
  if (!parser || !buffer || capacity < LG_NET_ENVELOPE_WIRE_SIZE ||
      maxPayloadLength > LG_NET_MAX_PAYLOAD_LENGTH)
    return false;

  parser->data             = (uint8_t *)buffer;
  parser->capacity         = capacity;
  parser->head             = 0;
  parser->used             = 0;
  parser->pendingWireSize  = 0;
  parser->maxPayloadLength = maxPayloadLength;
  return true;
}

void lgNetStreamParserReset(LGNetStreamParser * parser)
{
  if (!parser)
    return;

  parser->head            = 0;
  parser->used            = 0;
  parser->pendingWireSize = 0;
}

size_t lgNetStreamParserBuffered(const LGNetStreamParser * parser)
{
  return parserValid(parser) ? parser->used : 0;
}

size_t lgNetStreamParserWritable(const LGNetStreamParser * parser)
{
  return parserValid(parser) ? parser->capacity - parser->used : 0;
}

bool lgNetStreamParserAppend(LGNetStreamParser * parser, const void * data,
  size_t size, size_t * accepted)
{
  if (!accepted || !parserValid(parser) || (size && !data))
    return false;

  size_t writeSize = parser->capacity - parser->used;
  if (writeSize > size)
    writeSize = size;

  const size_t tail  = ringAdvance(
    parser->head, parser->used, parser->capacity);
  size_t       first = parser->capacity - tail;
  if (first > writeSize)
    first = writeSize;

  if (writeSize)
  {
    memcpy(parser->data + tail, data, first);
    memcpy(parser->data, (const uint8_t *)data + first, writeSize - first);
  }

  parser->used += writeSize;
  *accepted     = writeSize;
  return true;
}

LGNetStreamResult lgNetStreamParserNext(LGNetStreamParser * parser,
  LGNetStreamPacket * packet, LGNetParseResult * parseResult)
{
  if (parseResult)
    *parseResult = LG_NET_PARSE_OK;

  if (!packet || !parserValid(parser))
  {
    if (parseResult)
      *parseResult = LG_NET_PARSE_INVALID_VALUE;
    return LG_NET_STREAM_INVALID_PACKET;
  }

  memset(packet, 0, sizeof(*packet));
  if (parser->used < LG_NET_ENVELOPE_WIRE_SIZE)
  {
    if (parseResult)
      *parseResult = LG_NET_PARSE_TRUNCATED;
    return LG_NET_STREAM_NEED_DATA;
  }

  uint8_t header[LG_NET_ENVELOPE_WIRE_SIZE];
  ringRead(parser, 0, header, sizeof(header));

  const uint16_t headerSize =
    (uint16_t)header[8] | (uint16_t)((uint16_t)header[9] << 8);
  if (headerSize != LG_NET_ENVELOPE_WIRE_SIZE)
  {
    if (parseResult)
      *parseResult = LG_NET_PARSE_INVALID_HEADER;
    return LG_NET_STREAM_INVALID_PACKET;
  }

  const LGNetParseResult result = lgNetEnvelopeDecode(
    &packet->envelope, header, sizeof(header));
  if (result != LG_NET_PARSE_OK)
  {
    if (parseResult)
      *parseResult = result;
    return LG_NET_STREAM_INVALID_PACKET;
  }

  size_t wireSize;
  if (!lgNetPacketSize(&packet->envelope, &wireSize))
  {
    if (parseResult)
      *parseResult = LG_NET_PARSE_INVALID_LENGTH;
    return LG_NET_STREAM_INVALID_PACKET;
  }

  if (packet->envelope.payloadLength > parser->maxPayloadLength ||
      wireSize > parser->capacity)
  {
    if (parseResult)
      *parseResult = LG_NET_PARSE_INVALID_LENGTH;
    return LG_NET_STREAM_TOO_LARGE;
  }

  if (parser->used < wireSize)
  {
    if (parseResult)
      *parseResult = LG_NET_PARSE_TRUNCATED;
    return LG_NET_STREAM_NEED_DATA;
  }

  const size_t payloadStart = ringAdvance(parser->head,
    LG_NET_ENVELOPE_WIRE_SIZE, parser->capacity);
  size_t first = parser->capacity - payloadStart;
  if (first > packet->envelope.payloadLength)
    first = packet->envelope.payloadLength;

  packet->payload[0].data = parser->data + payloadStart;
  packet->payload[0].size = first;
  packet->payload[1].data = parser->data;
  packet->payload[1].size = packet->envelope.payloadLength - first;
  packet->payloadLength   = packet->envelope.payloadLength;
  packet->wireSize        = wireSize;
  parser->pendingWireSize = wireSize;
  return LG_NET_STREAM_PACKET;
}

bool lgNetStreamParserConsume(LGNetStreamParser * parser)
{
  if (!parserValid(parser) || !parser->pendingWireSize)
    return false;

  parser->head             = ringAdvance(parser->head,
    parser->pendingWireSize, parser->capacity);
  parser->used             -= parser->pendingWireSize;
  parser->pendingWireSize  = 0;
  return true;
}
