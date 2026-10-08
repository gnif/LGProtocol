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

#include <LGProtocol/NetworkServices.h>

#include <limits.h>
#include <string.h>

static const LGNetVideoStreamFlags VIDEO_STREAM_FLAGS =
  LG_NET_VIDEO_STREAM_HDR             |
  LG_NET_VIDEO_STREAM_GPU_PLANES_ONLY |
  LG_NET_VIDEO_STREAM_DATAGRAMS;

static const LGNetVideoFrameFlags VIDEO_FRAME_FLAGS =
  LG_NET_VIDEO_FRAME_KEYFRAME      |
  LG_NET_VIDEO_FRAME_DISCONTINUITY |
  LG_NET_VIDEO_FRAME_END_OF_STREAM |
  LG_NET_VIDEO_FRAME_HAS_CHECKSUM;

static const LGNetVideoFrameUpdateFlags VIDEO_FRAME_UPDATE_FLAGS =
  LG_NET_VIDEO_FRAME_UPDATE_HAS_CHECKSUM;

static const LGNetVideoFragmentFlags VIDEO_FRAGMENT_FLAGS =
  LG_NET_VIDEO_FRAGMENT_BLOCK_START |
  LG_NET_VIDEO_FRAGMENT_BLOCK_END   |
  LG_NET_VIDEO_FRAGMENT_FRAME_END   |
  LG_NET_VIDEO_FRAGMENT_RECOVERY    |
  LG_NET_VIDEO_FRAGMENT_CHECKSUM;

static const LGNetVideoFeedbackFlags VIDEO_FEEDBACK_FLAGS =
  LG_NET_VIDEO_FEEDBACK_REQUEST_KEYFRAME |
  LG_NET_VIDEO_FEEDBACK_CONGESTED        |
  LG_NET_VIDEO_FEEDBACK_DECODER_STALLED;

static const LGNetCursorPositionFlags CURSOR_POSITION_FLAGS =
  LG_NET_CURSOR_POSITION_VISIBLE |
  LG_NET_CURSOR_POSITION_RELATIVE;

static const LGNetCursorShapeFlags CURSOR_SHAPE_FLAGS =
  LG_NET_CURSOR_SHAPE_HIDDEN |
  LG_NET_CURSOR_SHAPE_ANIMATED;

static const LGNetInputClaimFlags INPUT_CLAIM_FLAGS =
  LG_NET_INPUT_CLAIM_KEYBOARD |
  LG_NET_INPUT_CLAIM_POINTER  |
  LG_NET_INPUT_CLAIM_EXCLUSIVE;

static const LGNetKeyboardFlags KEYBOARD_FLAGS =
  LG_NET_KEYBOARD_DOWN     |
  LG_NET_KEYBOARD_REPEAT   |
  LG_NET_KEYBOARD_EXTENDED |
  LG_NET_KEYBOARD_E1;

static const LGNetKeyboardLEDs KEYBOARD_LEDS =
  LG_NET_KEYBOARD_LED_NUM_LOCK    |
  LG_NET_KEYBOARD_LED_CAPS_LOCK   |
  LG_NET_KEYBOARD_LED_SCROLL_LOCK |
  LG_NET_KEYBOARD_LED_COMPOSE     |
  LG_NET_KEYBOARD_LED_KANA;

static const LGNetAudioFormatFlags AUDIO_FORMAT_FLAGS =
  LG_NET_AUDIO_FORMAT_INTERLEAVED;

static const LGNetAudioDataFlags AUDIO_DATA_FLAGS =
  LG_NET_AUDIO_DATA_DISCONTINUITY |
  LG_NET_AUDIO_DATA_SILENT        |
  LG_NET_AUDIO_DATA_END_OF_STREAM |
  LG_NET_AUDIO_DATA_CLOCK_VALID   |
  LG_NET_AUDIO_DATA_CLOCK_STABLE;

static const LGNetAudioStateFlags AUDIO_STATE_FLAGS =
  LG_NET_AUDIO_STATE_MUTED |
  LG_NET_AUDIO_STATE_FORMAT_ACTIVE;

static const LGNetClipboardClaimFlags CLIPBOARD_CLAIM_FLAGS =
  LG_NET_CLIPBOARD_CLAIM_READ |
  LG_NET_CLIPBOARD_CLAIM_WRITE;

static const LGNetClipboardOfferFlags CLIPBOARD_OFFER_FLAGS =
  LG_NET_CLIPBOARD_OFFER_LAZY |
  LG_NET_CLIPBOARD_OFFER_SENSITIVE;

static const LGNetClipboardChunkFlags CLIPBOARD_CHUNK_FLAGS =
  LG_NET_CLIPBOARD_CHUNK_FIRST |
  LG_NET_CLIPBOARD_CHUNK_FINAL;

static const LGNetFileOfferFlags FILE_OFFER_FLAGS =
  LG_NET_FILE_OFFER_COPY |
  LG_NET_FILE_OFFER_MOVE |
  LG_NET_FILE_OFFER_RECURSIVE;

static const LGNetFileEntryFlags FILE_ENTRY_FLAGS =
  LG_NET_FILE_ENTRY_EXECUTABLE |
  LG_NET_FILE_ENTRY_HIDDEN;

static const LGNetFileRequestFlags FILE_REQUEST_FLAGS =
  LG_NET_FILE_REQUEST_METADATA |
  LG_NET_FILE_REQUEST_DATA;

static const LGNetFileChunkFlags FILE_CHUNK_FLAGS =
  LG_NET_FILE_CHUNK_FIRST |
  LG_NET_FILE_CHUNK_FINAL;

static const LGNetUSBReservedFlags USB_RESERVED_FLAGS =
  LG_NET_USB_RESERVED_RESPONSE |
  LG_NET_USB_RESERVED_FINAL;

static const LGNetCoreSessionFlags CORE_SESSION_FLAGS =
  LG_NET_CORE_SESSION_AUTHENTICATED     |
  LG_NET_CORE_SESSION_PASSWORD_REQUIRED |
  LG_NET_CORE_SESSION_MULTI_CLIENT;

static const LGNetCoreStatusFlags CORE_STATUS_FLAGS =
  LG_NET_CORE_STATUS_VIDEO_AVAILABLE     |
  LG_NET_CORE_STATUS_INPUT_AVAILABLE     |
  LG_NET_CORE_STATUS_AUDIO_AVAILABLE     |
  LG_NET_CORE_STATUS_CLIPBOARD_AVAILABLE |
  LG_NET_CORE_STATUS_FILE_AVAILABLE      |
  LG_NET_CORE_STATUS_CURSOR_AVAILABLE    |
  LG_NET_CORE_STATUS_RECOVERY_AVAILABLE  |
  LG_NET_CORE_STATUS_CONTROL_AVAILABLE;

static const LGNetRecoveryCapabilities RECOVERY_CAPABILITIES =
  LG_NET_RECOVERY_CAP_DISPLAY;

static const LGNetRecoveryFlags RECOVERY_FLAGS =
  LG_NET_RECOVERY_SUPPORTED        |
  LG_NET_RECOVERY_ACTIVE           |
  LG_NET_RECOVERY_HELPER_AVAILABLE |
  LG_NET_RECOVERY_DISPLAY_PRESENT;

static const LGNetRecoveryRequestFlags RECOVERY_REQUEST_FLAGS =
  LG_NET_RECOVERY_REQUEST_ACTIVE |
  LG_NET_RECOVERY_REQUEST_FORCE;

static const LGNetVideoSubscribeFlags VIDEO_SUBSCRIBE_FLAGS =
  LG_NET_VIDEO_SUBSCRIBE_ALLOW_DATAGRAMS |
  LG_NET_VIDEO_SUBSCRIBE_REQUIRE_HDR     |
  LG_NET_VIDEO_SUBSCRIBE_LOW_LATENCY;

static const LGNetVideoScheduleFlags VIDEO_SCHEDULE_FLAGS =
  LG_NET_VIDEO_SCHEDULE_PRESENT          |
  LG_NET_VIDEO_SCHEDULE_DROP_IF_LATE     |
  LG_NET_VIDEO_SCHEDULE_REPEAT_PREVIOUS  |
  LG_NET_VIDEO_SCHEDULE_REQUEST_KEYFRAME;

static const LGNetVideoStatusFlags VIDEO_STATUS_FLAGS =
  LG_NET_VIDEO_STATUS_KEYFRAME_PENDING |
  LG_NET_VIDEO_STATUS_CONGESTED        |
  LG_NET_VIDEO_STATUS_RECONFIGURING;

static const LGNetCursorStateFlags CURSOR_STATE_FLAGS =
  LG_NET_CURSOR_STATE_VISIBLE         |
  LG_NET_CURSOR_STATE_SHAPE_VALID     |
  LG_NET_CURSOR_STATE_POSITION_VALID  |
  LG_NET_CURSOR_STATE_TRANSFORM_VALID;

static const LGNetCursorTransformFlags CURSOR_TRANSFORM_FLAGS =
  LG_NET_CURSOR_TRANSFORM_MIRROR_X |
  LG_NET_CURSOR_TRANSFORM_MIRROR_Y;

static const LGNetCursorColorTransformFlags CURSOR_COLOR_TRANSFORM_FLAGS =
  LG_NET_CURSOR_COLOR_TRANSFORM_MATRIX |
  LG_NET_CURSOR_COLOR_TRANSFORM_LUT;

static const LGNetControlFrameScheduleFlags CONTROL_FRAME_SCHEDULE_FLAGS =
  LG_NET_CONTROL_FRAME_SCHEDULE_ACTIVE    |
  LG_NET_CONTROL_FRAME_SCHEDULE_RELEASE   |
  LG_NET_CONTROL_FRAME_SCHEDULE_RESET     |
  LG_NET_CONTROL_FRAME_SCHEDULE_IMMEDIATE;

static const LGNetAudioDirectionMask AUDIO_DIRECTIONS =
  LG_NET_AUDIO_DIRECTIONS_PLAYBACK |
  LG_NET_AUDIO_DIRECTIONS_CAPTURE;

static const LGNetAudioSubscribeFlags AUDIO_SUBSCRIBE_FLAGS_V1 =
  LG_NET_AUDIO_SUBSCRIBE_LOW_LATENCY |
  LG_NET_AUDIO_SUBSCRIBE_EXCLUSIVE;

static const LGNetAudioSubscribeFlags AUDIO_SUBSCRIBE_FLAGS =
  LG_NET_AUDIO_SUBSCRIBE_LOW_LATENCY       |
  LG_NET_AUDIO_SUBSCRIBE_EXCLUSIVE_CAPTURE |
  LG_NET_AUDIO_SUBSCRIBE_CLOCK_FEEDBACK    |
  LG_NET_AUDIO_SUBSCRIBE_DATAGRAM_PCM      |
  LG_NET_AUDIO_SUBSCRIBE_RELIABLE_PCM;

static const LGNetAudioRoles AUDIO_ROLES =
  LG_NET_AUDIO_ROLE_PLAYBACK          |
  LG_NET_AUDIO_ROLE_CAPTURE           |
  LG_NET_AUDIO_ROLE_CLOCK_FEEDBACK    |
  LG_NET_AUDIO_ROLE_DATAGRAM_PCM      |
  LG_NET_AUDIO_ROLE_RELIABLE_PCM      |
  LG_NET_AUDIO_ROLE_EXCLUSIVE_CAPTURE;

static const LGNetAudioClockFlags AUDIO_CLOCK_FLAGS =
  LG_NET_AUDIO_CLOCK_VALID |
  LG_NET_AUDIO_CLOCK_STABLE;

static const LGNetAudioControlFlags AUDIO_CONTROL_FLAGS =
  LG_NET_AUDIO_CONTROL_GRACEFUL |
  LG_NET_AUDIO_CONTROL_FLUSH;

static const LGNetFileLeaseFlags FILE_LEASE_FLAGS =
  LG_NET_FILE_LEASE_READ      |
  LG_NET_FILE_LEASE_EXCLUSIVE |
  LG_NET_FILE_LEASE_ACQUIRED;

static uint32_t floatBits(float value)
{
  uint32_t bits;
  memcpy(&bits, &value, sizeof(bits));
  return bits;
}

static float bitsFloat(uint32_t bits)
{
  float value;
  memcpy(&value, &bits, sizeof(value));
  return value;
}

static bool finiteFloat(float value)
{
  return (floatBits(value) & UINT32_C(0x7f800000)) !=
    UINT32_C(0x7f800000);
}

static bool writerFloat(LGNetWriter * writer, float value)
{
  return finiteFloat(value) && lgNetWriterU32(writer, floatBits(value));
}

static bool readerFloat(LGNetReader * reader, float * value)
{
  uint32_t bits;
  if (!value || !lgNetReaderU32(reader, &bits))
    return false;
  *value = bitsFloat(bits);
  return finiteFloat(*value);
}

static bool variableSize(size_t headerSize, size_t payloadLength,
    size_t payloadLimit, size_t * wireSize)
{
  if (!wireSize || payloadLength > payloadLimit ||
      headerSize > SIZE_MAX - payloadLength ||
      headerSize + payloadLength > LG_NET_MAX_PAYLOAD_LENGTH)
    return false;

  *wireSize = headerSize + payloadLength;
  return true;
}

static uint16_t bitCount32(uint32_t value)
{
  uint16_t count = 0;
  while (value)
  {
    count += (uint16_t)(value & 1U);
    value >>= 1;
  }
  return count;
}

static LGNetParseResult fixedDecodeResult(
    const LGNetReader * reader, size_t expected, size_t actual)
{
  if (!reader || !lgNetReaderValid(reader))
    return LG_NET_PARSE_INVALID_VALUE;
  if (actual != expected || lgNetReaderConsumed(reader) != expected)
    return LG_NET_PARSE_INVALID_LENGTH;
  return LG_NET_PARSE_OK;
}

static LGNetParseResult variableDecodeSize(size_t headerSize,
    size_t payloadLength, size_t payloadLimit, size_t actual,
    size_t * expected)
{
  if (!variableSize(headerSize, payloadLength, payloadLimit, expected))
    return LG_NET_PARSE_INVALID_LENGTH;
  if (actual < *expected)
    return LG_NET_PARSE_TRUNCATED;
  if (actual != *expected)
    return LG_NET_PARSE_INVALID_LENGTH;
  return LG_NET_PARSE_OK;
}

static bool coreStateKnown(LGNetCoreState state)
{
  return state >= LG_NET_CORE_STATE_READY &&
    state <= LG_NET_CORE_STATE_ERROR;
}

static bool coreGuestOSKnown(LGNetCoreGuestOS os)
{
  return os <= LG_NET_CORE_GUEST_OS_OTHER;
}

static bool coreErrorKnown(LGNetCoreErrorCode code)
{
  return code >= LG_NET_CORE_ERROR_INVALID_REQUEST &&
    code <= LG_NET_CORE_ERROR_INTERNAL;
}

static bool serviceKnown(LGNetService service)
{
  return service >= LG_NET_SERVICE_CORE && service <= LG_NET_SERVICE_CONTROL;
}

size_t lgNetCoreSessionInfoSize(const LGNetCoreSessionInfo * info)
{
  size_t size;
  return info && variableSize(LG_NET_CORE_SESSION_INFO_HEADER_WIRE_SIZE,
    info->nameLength, LG_NET_CORE_MAX_NAME_LENGTH, &size) ? size : 0;
}

bool lgNetCoreSessionInfoValid(const LGNetCoreSessionInfo * info)
{
  return info && info->sessionID && info->serverTimeNs && info->clientID &&
    info->maxClients && info->activeClients &&
    info->activeClients <= info->maxClients &&
    !(info->flags & ~CORE_SESSION_FLAGS) &&
    info->nameLength <= LG_NET_CORE_MAX_NAME_LENGTH &&
    (!info->nameLength || info->name);
}

bool lgNetCoreSessionInfoEncode(
    void * data, size_t size, const LGNetCoreSessionInfo * info)
{
  const size_t needed = lgNetCoreSessionInfoSize(info);
  if (!data || !needed || size < needed || !lgNetCoreSessionInfoValid(info))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU64  (&writer, info->sessionID)       &&
    lgNetWriterU64  (&writer, info->connectedAtNs)   &&
    lgNetWriterU64  (&writer, info->serverTimeNs)    &&
    lgNetWriterU32  (&writer, info->clientID)        &&
    lgNetWriterU32  (&writer, info->activeClients)   &&
    lgNetWriterU32  (&writer, info->maxClients)      &&
    lgNetWriterU32  (&writer, info->flags)           &&
    lgNetWriterU16  (&writer, info->nameLength)      &&
    lgNetWriterZero (&writer, 6)                     &&
    lgNetWriterBytes(&writer, info->name, info->nameLength) &&
    lgNetWriterSize (&writer) == needed;
}

LGNetParseResult lgNetCoreSessionInfoDecode(
    LGNetCoreSessionInfo * info, const void * data, size_t size)
{
  if (!info || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_CORE_SESSION_INFO_HEADER_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetCoreSessionInfo decoded;
  LGNetReader          reader;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU64 (&reader, &decoded.sessionID)       ||
      !lgNetReaderU64 (&reader, &decoded.connectedAtNs)   ||
      !lgNetReaderU64 (&reader, &decoded.serverTimeNs)    ||
      !lgNetReaderU32 (&reader, &decoded.clientID)        ||
      !lgNetReaderU32 (&reader, &decoded.activeClients)   ||
      !lgNetReaderU32 (&reader, &decoded.maxClients)      ||
      !lgNetReaderU32 (&reader, &decoded.flags)           ||
      !lgNetReaderU16 (&reader, &decoded.nameLength)      ||
      !lgNetReaderZero(&reader, 6))
    return LG_NET_PARSE_INVALID_VALUE;

  size_t                 expected;
  const LGNetParseResult result = variableDecodeSize(
    LG_NET_CORE_SESSION_INFO_HEADER_WIRE_SIZE, decoded.nameLength,
    LG_NET_CORE_MAX_NAME_LENGTH, size, &expected);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetReaderView(&reader, &decoded.name, decoded.nameLength) ||
      lgNetReaderConsumed(&reader) != expected)
    return LG_NET_PARSE_INVALID_VALUE;
  if (!lgNetCoreSessionInfoValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *info = decoded;
  return LG_NET_PARSE_OK;
}

size_t lgNetCoreGuestInfoSize(const LGNetCoreGuestInfo * info)
{
  size_t variableLength;
  size_t size;
  if (!info || info->versionLength > LG_NET_CORE_GUEST_MAX_VERSION_LENGTH ||
      info->osNameLength > LG_NET_CORE_GUEST_MAX_OS_NAME_LENGTH ||
      info->captureLength > LG_NET_CORE_GUEST_MAX_CAPTURE_LENGTH ||
      info->cpuModelLength > LG_NET_CORE_GUEST_MAX_CPU_MODEL_LENGTH)
    return 0;

  variableLength = (size_t)info->versionLength + info->osNameLength +
    info->captureLength + info->cpuModelLength;
  return variableSize(LG_NET_CORE_GUEST_INFO_HEADER_WIRE_SIZE,
    variableLength, LG_NET_CORE_GUEST_INFO_MAX_VARIABLE_LENGTH, &size) ?
    size : 0;
}

bool lgNetCoreGuestInfoValid(const LGNetCoreGuestInfo * info)
{
  return info && coreGuestOSKnown(info->os) &&
    info->versionLength <= LG_NET_CORE_GUEST_MAX_VERSION_LENGTH &&
    (!info->versionLength || info->version) &&
    info->osNameLength <= LG_NET_CORE_GUEST_MAX_OS_NAME_LENGTH &&
    (!info->osNameLength || info->osName) &&
    info->captureLength <= LG_NET_CORE_GUEST_MAX_CAPTURE_LENGTH &&
    (!info->captureLength || info->capture) &&
    info->cpuModelLength <= LG_NET_CORE_GUEST_MAX_CPU_MODEL_LENGTH &&
    (!info->cpuModelLength || info->cpuModel) &&
    lgNetCoreGuestInfoSize(info) != 0;
}

bool lgNetCoreGuestInfoEncode(
    void * data, size_t size, const LGNetCoreGuestInfo * info)
{
  const size_t needed = lgNetCoreGuestInfoSize(info);
  if (!data || !needed || size < needed || !lgNetCoreGuestInfoValid(info))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterBytes(&writer, info->uuid, sizeof(info->uuid)) &&
    lgNetWriterU32  (&writer, info->os)                      &&
    lgNetWriterU8   (&writer, info->processors)              &&
    lgNetWriterU8   (&writer, info->cores)                   &&
    lgNetWriterU8   (&writer, info->sockets)                 &&
    lgNetWriterZero (&writer, 1)                             &&
    lgNetWriterU16  (&writer, info->versionLength)           &&
    lgNetWriterU16  (&writer, info->osNameLength)            &&
    lgNetWriterU16  (&writer, info->captureLength)           &&
    lgNetWriterU16  (&writer, info->cpuModelLength)          &&
    lgNetWriterBytes(&writer, info->version, info->versionLength) &&
    lgNetWriterBytes(&writer, info->osName, info->osNameLength)   &&
    lgNetWriterBytes(&writer, info->capture, info->captureLength) &&
    lgNetWriterBytes(&writer, info->cpuModel,
      info->cpuModelLength)                                  &&
    lgNetWriterSize (&writer) == needed;
}

LGNetParseResult lgNetCoreGuestInfoDecode(
    LGNetCoreGuestInfo * info, const void * data, size_t size)
{
  if (!info || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_CORE_GUEST_INFO_HEADER_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetCoreGuestInfo decoded;
  LGNetParseResult   result;
  LGNetReader        reader;
  size_t             expected;
  size_t             variableLength;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderBytes(&reader, decoded.uuid, sizeof(decoded.uuid)) ||
      !lgNetReaderU32  (&reader, &decoded.os)                       ||
      !lgNetReaderU8   (&reader, &decoded.processors)               ||
      !lgNetReaderU8   (&reader, &decoded.cores)                    ||
      !lgNetReaderU8   (&reader, &decoded.sockets)                  ||
      !lgNetReaderZero (&reader, 1)                                 ||
      !lgNetReaderU16  (&reader, &decoded.versionLength)            ||
      !lgNetReaderU16  (&reader, &decoded.osNameLength)             ||
      !lgNetReaderU16  (&reader, &decoded.captureLength)            ||
      !lgNetReaderU16  (&reader, &decoded.cpuModelLength))
    return LG_NET_PARSE_INVALID_VALUE;

  if (decoded.versionLength > LG_NET_CORE_GUEST_MAX_VERSION_LENGTH ||
      decoded.osNameLength > LG_NET_CORE_GUEST_MAX_OS_NAME_LENGTH ||
      decoded.captureLength > LG_NET_CORE_GUEST_MAX_CAPTURE_LENGTH ||
      decoded.cpuModelLength > LG_NET_CORE_GUEST_MAX_CPU_MODEL_LENGTH)
    return LG_NET_PARSE_INVALID_LENGTH;
  variableLength = (size_t)decoded.versionLength + decoded.osNameLength +
    decoded.captureLength + decoded.cpuModelLength;
  result         = variableDecodeSize(LG_NET_CORE_GUEST_INFO_HEADER_WIRE_SIZE,
    variableLength, LG_NET_CORE_GUEST_INFO_MAX_VARIABLE_LENGTH,
    size, &expected);
  if (result != LG_NET_PARSE_OK)
    return result;

  if (!lgNetReaderView(&reader, &decoded.version, decoded.versionLength) ||
      !lgNetReaderView(&reader, &decoded.osName, decoded.osNameLength)   ||
      !lgNetReaderView(&reader, &decoded.capture,
        decoded.captureLength)                                         ||
      !lgNetReaderView(&reader, &decoded.cpuModel,
        decoded.cpuModelLength))
    return LG_NET_PARSE_INVALID_VALUE;
  result = fixedDecodeResult(&reader, expected, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetCoreGuestInfoValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *info = decoded;
  return LG_NET_PARSE_OK;
}

bool lgNetCoreStatusValid(const LGNetCoreStatus * status)
{
  return status && status->sessionID && status->statusSequence &&
    coreStateKnown(status->state) && !(status->flags & ~CORE_STATUS_FLAGS);
}

bool lgNetCoreStatusEncode(
    void * data, size_t size, const LGNetCoreStatus * status)
{
  if (!data || size < LG_NET_CORE_STATUS_WIRE_SIZE ||
      !lgNetCoreStatusValid(status))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU64(&writer, status->sessionID)      &&
    lgNetWriterU64(&writer, status->statusSequence) &&
    lgNetWriterU64(&writer, status->uptimeNs)       &&
    lgNetWriterU32(&writer, status->state)          &&
    lgNetWriterU32(&writer, status->activeClients)  &&
    lgNetWriterU32(&writer, status->flags)          &&
    lgNetWriterU32(&writer, status->detail)         &&
    lgNetWriterSize(&writer) == LG_NET_CORE_STATUS_WIRE_SIZE;
}

LGNetParseResult lgNetCoreStatusDecode(
    LGNetCoreStatus * status, const void * data, size_t size)
{
  if (!status || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_CORE_STATUS_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetCoreStatus decoded;
  LGNetReader     reader;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU64(&reader, &decoded.sessionID)      ||
      !lgNetReaderU64(&reader, &decoded.statusSequence) ||
      !lgNetReaderU64(&reader, &decoded.uptimeNs)       ||
      !lgNetReaderU32(&reader, &decoded.state)          ||
      !lgNetReaderU32(&reader, &decoded.activeClients)  ||
      !lgNetReaderU32(&reader, &decoded.flags)          ||
      !lgNetReaderU32(&reader, &decoded.detail))
    return LG_NET_PARSE_INVALID_VALUE;

  const LGNetParseResult result = fixedDecodeResult(
    &reader, LG_NET_CORE_STATUS_WIRE_SIZE, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetCoreStatusValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *status = decoded;
  return LG_NET_PARSE_OK;
}

size_t lgNetCoreErrorSize(const LGNetCoreError * error)
{
  size_t size;
  return error && variableSize(LG_NET_CORE_ERROR_HEADER_WIRE_SIZE,
    error->textLength, LG_NET_CORE_MAX_ERROR_TEXT_LENGTH, &size) ? size : 0;
}

bool lgNetCoreErrorValid(const LGNetCoreError * error)
{
  return error && coreErrorKnown(error->code) && serviceKnown(error->service) &&
    error->messageType &&
    error->textLength <= LG_NET_CORE_MAX_ERROR_TEXT_LENGTH &&
    (!error->textLength || error->text);
}

bool lgNetCoreErrorEncode(
    void * data, size_t size, const LGNetCoreError * error)
{
  const size_t needed = lgNetCoreErrorSize(error);
  if (!data || !needed || size < needed || !lgNetCoreErrorValid(error))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU64  (&writer, error->requestID)       &&
    lgNetWriterU32  (&writer, error->code)            &&
    lgNetWriterU16  (&writer, error->service)         &&
    lgNetWriterU16  (&writer, error->messageType)     &&
    lgNetWriterU32  (&writer, error->detail)          &&
    lgNetWriterU32  (&writer, error->retryAfterMs)    &&
    lgNetWriterU16  (&writer, error->textLength)      &&
    lgNetWriterZero (&writer, 6)                      &&
    lgNetWriterBytes(&writer, error->text, error->textLength) &&
    lgNetWriterSize (&writer) == needed;
}

LGNetParseResult lgNetCoreErrorDecode(
    LGNetCoreError * error, const void * data, size_t size)
{
  if (!error || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_CORE_ERROR_HEADER_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetCoreError decoded;
  LGNetReader    reader;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU64 (&reader, &decoded.requestID)    ||
      !lgNetReaderU32 (&reader, &decoded.code)         ||
      !lgNetReaderU16 (&reader, &decoded.service)      ||
      !lgNetReaderU16 (&reader, &decoded.messageType)  ||
      !lgNetReaderU32 (&reader, &decoded.detail)       ||
      !lgNetReaderU32 (&reader, &decoded.retryAfterMs) ||
      !lgNetReaderU16 (&reader, &decoded.textLength)   ||
      !lgNetReaderZero(&reader, 6))
    return LG_NET_PARSE_INVALID_VALUE;

  size_t                 expected;
  const LGNetParseResult result = variableDecodeSize(
    LG_NET_CORE_ERROR_HEADER_WIRE_SIZE, decoded.textLength,
    LG_NET_CORE_MAX_ERROR_TEXT_LENGTH, size, &expected);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetReaderView(&reader, &decoded.text, decoded.textLength) ||
      lgNetReaderConsumed(&reader) != expected)
    return LG_NET_PARSE_INVALID_VALUE;
  if (!lgNetCoreErrorValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *error = decoded;
  return LG_NET_PARSE_OK;
}

static bool recoveryStateKnown(LGNetRecoveryState state)
{
  return state <= LG_NET_RECOVERY_STATE_FAILED;
}

static bool recoveryErrorKnown(LGNetRecoveryError error)
{
  return error <= LG_NET_RECOVERY_ERROR_TIMEOUT;
}

static bool recoveryStateErrorValid(
    LGNetRecoveryState state, LGNetRecoveryError error)
{
  return recoveryStateKnown(state) && recoveryErrorKnown(error) &&
    ((state == LG_NET_RECOVERY_STATE_FAILED) ==
      (error != LG_NET_RECOVERY_ERROR_NONE));
}

size_t lgNetRecoveryInfoSize(const LGNetRecoveryInfo * info)
{
  size_t size;
  return info && variableSize(LG_NET_RECOVERY_INFO_HEADER_WIRE_SIZE,
    info->versionLength, LG_NET_RECOVERY_MAX_VERSION_LENGTH, &size) ? size : 0;
}

bool lgNetRecoveryInfoValid(const LGNetRecoveryInfo * info)
{
  return info && info->sessionID && info->statusSequence &&
    !(info->capabilities & ~RECOVERY_CAPABILITIES) &&
    recoveryStateErrorValid(info->state, info->error) &&
    !(info->flags & ~RECOVERY_FLAGS) &&
    (!(info->flags & LG_NET_RECOVERY_ACTIVE) ||
      info->state == LG_NET_RECOVERY_STATE_ACTIVE ||
      info->state == LG_NET_RECOVERY_STATE_SWITCHING) &&
    (info->state != LG_NET_RECOVERY_STATE_ACTIVE ||
      (info->flags & LG_NET_RECOVERY_ACTIVE)) &&
    info->versionLength <= LG_NET_RECOVERY_MAX_VERSION_LENGTH &&
    (!info->versionLength || info->version);
}

bool lgNetRecoveryInfoEncode(
    void * data, size_t size, const LGNetRecoveryInfo * info)
{
  const size_t needed = lgNetRecoveryInfoSize(info);
  if (!data || !needed || size < needed || !lgNetRecoveryInfoValid(info))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU64  (&writer, info->sessionID)        &&
    lgNetWriterU64  (&writer, info->statusSequence)   &&
    lgNetWriterU32  (&writer, info->capabilities)     &&
    lgNetWriterU32  (&writer, info->state)            &&
    lgNetWriterU32  (&writer, info->error)            &&
    lgNetWriterU32  (&writer, info->flags)            &&
    lgNetWriterU32  (&writer, info->maxTransitionMs)  &&
    lgNetWriterU16  (&writer, info->versionLength)    &&
    lgNetWriterZero (&writer, 10)                     &&
    lgNetWriterBytes(&writer, info->version, info->versionLength) &&
    lgNetWriterSize (&writer) == needed;
}

LGNetParseResult lgNetRecoveryInfoDecode(
    LGNetRecoveryInfo * info, const void * data, size_t size)
{
  if (!info || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_RECOVERY_INFO_HEADER_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetRecoveryInfo decoded;
  LGNetReader       reader;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU64 (&reader, &decoded.sessionID)       ||
      !lgNetReaderU64 (&reader, &decoded.statusSequence)  ||
      !lgNetReaderU32 (&reader, &decoded.capabilities)    ||
      !lgNetReaderU32 (&reader, &decoded.state)           ||
      !lgNetReaderU32 (&reader, &decoded.error)           ||
      !lgNetReaderU32 (&reader, &decoded.flags)           ||
      !lgNetReaderU32 (&reader, &decoded.maxTransitionMs) ||
      !lgNetReaderU16 (&reader, &decoded.versionLength)   ||
      !lgNetReaderZero(&reader, 10))
    return LG_NET_PARSE_INVALID_VALUE;

  size_t                 expected;
  const LGNetParseResult result = variableDecodeSize(
    LG_NET_RECOVERY_INFO_HEADER_WIRE_SIZE, decoded.versionLength,
    LG_NET_RECOVERY_MAX_VERSION_LENGTH, size, &expected);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetReaderView(&reader, &decoded.version, decoded.versionLength) ||
      lgNetReaderConsumed(&reader) != expected)
    return LG_NET_PARSE_INVALID_VALUE;
  if (!lgNetRecoveryInfoValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *info = decoded;
  return LG_NET_PARSE_OK;
}

bool lgNetRecoveryRequestValid(const LGNetRecoveryRequest * request)
{
  return request && request->requestID && request->sessionID &&
    request->timeoutMs && request->timeoutMs <= LG_NET_RECOVERY_MAX_TIMEOUT_MS &&
    !(request->flags & ~RECOVERY_REQUEST_FLAGS);
}

bool lgNetRecoveryRequestEncode(
    void * data, size_t size, const LGNetRecoveryRequest * request)
{
  if (!data || size < LG_NET_RECOVERY_REQUEST_WIRE_SIZE ||
      !lgNetRecoveryRequestValid(request))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU64(&writer, request->requestID)              &&
    lgNetWriterU64(&writer, request->sessionID)              &&
    lgNetWriterU64(&writer, request->expectedStatusSequence) &&
    lgNetWriterU32(&writer, request->timeoutMs)              &&
    lgNetWriterU32(&writer, request->flags)                  &&
    lgNetWriterSize(&writer) == LG_NET_RECOVERY_REQUEST_WIRE_SIZE;
}

LGNetParseResult lgNetRecoveryRequestDecode(
    LGNetRecoveryRequest * request, const void * data, size_t size)
{
  if (!request || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_RECOVERY_REQUEST_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetRecoveryRequest decoded;
  LGNetReader          reader;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU64(&reader, &decoded.requestID)              ||
      !lgNetReaderU64(&reader, &decoded.sessionID)              ||
      !lgNetReaderU64(&reader, &decoded.expectedStatusSequence) ||
      !lgNetReaderU32(&reader, &decoded.timeoutMs)              ||
      !lgNetReaderU32(&reader, &decoded.flags))
    return LG_NET_PARSE_INVALID_VALUE;

  const LGNetParseResult result = fixedDecodeResult(
    &reader, LG_NET_RECOVERY_REQUEST_WIRE_SIZE, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetRecoveryRequestValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *request = decoded;
  return LG_NET_PARSE_OK;
}

bool lgNetRecoveryStatusValid(const LGNetRecoveryStatus * status)
{
  return status && status->sessionID && status->statusSequence &&
    recoveryStateErrorValid(status->state, status->error) &&
    !(status->flags & ~RECOVERY_FLAGS) &&
    (!(status->flags & LG_NET_RECOVERY_ACTIVE) ||
      status->state == LG_NET_RECOVERY_STATE_ACTIVE ||
      status->state == LG_NET_RECOVERY_STATE_SWITCHING) &&
    (status->state != LG_NET_RECOVERY_STATE_ACTIVE ||
      (status->flags & LG_NET_RECOVERY_ACTIVE));
}

bool lgNetRecoveryStatusEncode(
    void * data, size_t size, const LGNetRecoveryStatus * status)
{
  if (!data || size < LG_NET_RECOVERY_STATUS_WIRE_SIZE ||
      !lgNetRecoveryStatusValid(status))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU64(&writer, status->requestID)      &&
    lgNetWriterU64(&writer, status->sessionID)      &&
    lgNetWriterU64(&writer, status->statusSequence) &&
    lgNetWriterU32(&writer, status->state)          &&
    lgNetWriterU32(&writer, status->error)          &&
    lgNetWriterU32(&writer, status->flags)          &&
    lgNetWriterU32(&writer, status->detail)         &&
    lgNetWriterSize(&writer) == LG_NET_RECOVERY_STATUS_WIRE_SIZE;
}

LGNetParseResult lgNetRecoveryStatusDecode(
    LGNetRecoveryStatus * status, const void * data, size_t size)
{
  if (!status || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_RECOVERY_STATUS_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetRecoveryStatus decoded;
  LGNetReader         reader;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU64(&reader, &decoded.requestID)      ||
      !lgNetReaderU64(&reader, &decoded.sessionID)      ||
      !lgNetReaderU64(&reader, &decoded.statusSequence) ||
      !lgNetReaderU32(&reader, &decoded.state)          ||
      !lgNetReaderU32(&reader, &decoded.error)          ||
      !lgNetReaderU32(&reader, &decoded.flags)          ||
      !lgNetReaderU32(&reader, &decoded.detail))
    return LG_NET_PARSE_INVALID_VALUE;

  const LGNetParseResult result = fixedDecodeResult(
    &reader, LG_NET_RECOVERY_STATUS_WIRE_SIZE, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetRecoveryStatusValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *status = decoded;
  return LG_NET_PARSE_OK;
}

static bool videoCodecKnown(LGNetVideoCodec codec)
{
  return codec == LG_NET_VIDEO_CODEC_PYROWAVE;
}

static bool videoPixelFormatKnown(LGNetVideoPixelFormat format)
{
  return format == LG_NET_VIDEO_PIXEL_FORMAT_NV12      ||
    format == LG_NET_VIDEO_PIXEL_FORMAT_P010           ||
    format == LG_NET_VIDEO_PIXEL_FORMAT_YUV420P8       ||
    format == LG_NET_VIDEO_PIXEL_FORMAT_YUV420P10      ||
    format == LG_NET_VIDEO_PIXEL_FORMAT_YUV444P8       ||
    format == LG_NET_VIDEO_PIXEL_FORMAT_YUV444P10      ||
    format == LG_NET_VIDEO_PIXEL_FORMAT_YUV444P16F;
}

static bool videoPixelFormatValid(const LGNetVideoStreamConfig * config)
{
  switch (config->pixelFormat)
  {
    case LG_NET_VIDEO_PIXEL_FORMAT_NV12:
      return config->chromaSubsampling == LG_NET_VIDEO_CHROMA_420 &&
        config->planeCount == 2 && config->bitDepth == 8;

    case LG_NET_VIDEO_PIXEL_FORMAT_P010:
      return config->chromaSubsampling == LG_NET_VIDEO_CHROMA_420 &&
        config->planeCount == 2 && config->bitDepth == 10;

    case LG_NET_VIDEO_PIXEL_FORMAT_YUV420P8:
      return config->chromaSubsampling == LG_NET_VIDEO_CHROMA_420 &&
        config->planeCount == 3 && config->bitDepth == 8;

    case LG_NET_VIDEO_PIXEL_FORMAT_YUV420P10:
      return config->chromaSubsampling == LG_NET_VIDEO_CHROMA_420 &&
        config->planeCount == 3 && config->bitDepth == 10;

    case LG_NET_VIDEO_PIXEL_FORMAT_YUV444P8:
      return config->chromaSubsampling == LG_NET_VIDEO_CHROMA_444 &&
        config->planeCount == 3 && config->bitDepth == 8;

    case LG_NET_VIDEO_PIXEL_FORMAT_YUV444P10:
      return config->chromaSubsampling == LG_NET_VIDEO_CHROMA_444 &&
        config->planeCount == 3 && config->bitDepth == 10;

    case LG_NET_VIDEO_PIXEL_FORMAT_YUV444P16F:
      return config->chromaSubsampling == LG_NET_VIDEO_CHROMA_444 &&
        config->planeCount == 3 && config->bitDepth == 16;
  }

  return false;
}

static bool videoChromaKnown(LGNetVideoChromaSubsampling chroma)
{
  return chroma == LG_NET_VIDEO_CHROMA_420 ||
    chroma == LG_NET_VIDEO_CHROMA_444;
}

static bool colorPrimariesKnown(LGNetColorPrimaries primaries)
{
  return primaries >= LG_NET_COLOR_PRIMARIES_BT601 &&
    primaries <= LG_NET_COLOR_PRIMARIES_BT2020;
}

static bool colorTransferKnown(LGNetColorTransfer transfer)
{
  return transfer >= LG_NET_COLOR_TRANSFER_LINEAR &&
    transfer <= LG_NET_COLOR_TRANSFER_HLG;
}

static bool colorMatrixKnown(LGNetColorMatrix matrix)
{
  return matrix >= LG_NET_COLOR_MATRIX_BT601 &&
    matrix <= LG_NET_COLOR_MATRIX_RGB;
}

static bool colorRangeKnown(LGNetColorRange range)
{
  return range == LG_NET_COLOR_RANGE_LIMITED ||
    range == LG_NET_COLOR_RANGE_FULL;
}

static bool videoControlReasonKnown(LGNetVideoControlReason reason)
{
  return reason >= LG_NET_VIDEO_CONTROL_USER &&
    reason <= LG_NET_VIDEO_CONTROL_RECONFIGURE;
}

bool lgNetVideoSubscribeValid(const LGNetVideoSubscribe * subscribe)
{
  return subscribe && subscribe->subscriptionID &&
    (!subscribe->preferredCodec || videoCodecKnown(subscribe->preferredCodec)) &&
    !(subscribe->flags & ~VIDEO_SUBSCRIBE_FLAGS) && subscribe->maxWidth &&
    subscribe->maxWidth <= LG_NET_VIDEO_MAX_WIDTH && subscribe->maxHeight &&
    subscribe->maxHeight <= LG_NET_VIDEO_MAX_HEIGHT &&
    subscribe->maxFrameLength &&
    subscribe->maxFrameLength <= LG_NET_VIDEO_MAX_FRAME_LENGTH &&
    subscribe->maxFrameLatencyMs && subscribe->maxFrameRate;
}

bool lgNetVideoSubscribeEncode(
    void * data, size_t size, const LGNetVideoSubscribe * subscribe)
{
  if (!data || size < LG_NET_VIDEO_SUBSCRIBE_WIRE_SIZE ||
      !lgNetVideoSubscribeValid(subscribe))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU64(&writer, subscribe->subscriptionID)  &&
    lgNetWriterU16(&writer, subscribe->preferredCodec)  &&
    lgNetWriterU16(&writer, subscribe->flags)           &&
    lgNetWriterU32(&writer, subscribe->maxWidth)        &&
    lgNetWriterU32(&writer, subscribe->maxHeight)       &&
    lgNetWriterU32(&writer, subscribe->maxFrameLength)  &&
    lgNetWriterU32(&writer, subscribe->maxFrameLatencyMs) &&
    lgNetWriterU32(&writer, subscribe->maxFrameRate)    &&
    lgNetWriterSize(&writer) == LG_NET_VIDEO_SUBSCRIBE_WIRE_SIZE;
}

LGNetParseResult lgNetVideoSubscribeDecode(
    LGNetVideoSubscribe * subscribe, const void * data, size_t size)
{
  if (!subscribe || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_VIDEO_SUBSCRIBE_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetVideoSubscribe decoded;
  LGNetReader         reader;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU64(&reader, &decoded.subscriptionID)  ||
      !lgNetReaderU16(&reader, &decoded.preferredCodec)  ||
      !lgNetReaderU16(&reader, &decoded.flags)           ||
      !lgNetReaderU32(&reader, &decoded.maxWidth)        ||
      !lgNetReaderU32(&reader, &decoded.maxHeight)       ||
      !lgNetReaderU32(&reader, &decoded.maxFrameLength)  ||
      !lgNetReaderU32(&reader, &decoded.maxFrameLatencyMs) ||
      !lgNetReaderU32(&reader, &decoded.maxFrameRate))
    return LG_NET_PARSE_INVALID_VALUE;

  const LGNetParseResult result = fixedDecodeResult(
    &reader, LG_NET_VIDEO_SUBSCRIBE_WIRE_SIZE, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetVideoSubscribeValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *subscribe = decoded;
  return LG_NET_PARSE_OK;
}

bool lgNetVideoControlValid(const LGNetVideoControl * control)
{
  return control && control->streamID &&
    videoControlReasonKnown(control->reason);
}

bool lgNetVideoControlEncode(
    void * data, size_t size, const LGNetVideoControl * control)
{
  if (!data || size < LG_NET_VIDEO_CONTROL_WIRE_SIZE ||
      !lgNetVideoControlValid(control))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU32(&writer, control->streamID)    &&
    lgNetWriterU32(&writer, control->reason)      &&
    lgNetWriterU64(&writer, control->configEpoch) &&
    lgNetWriterU64(&writer, control->frameID)     &&
    lgNetWriterSize(&writer) == LG_NET_VIDEO_CONTROL_WIRE_SIZE;
}

LGNetParseResult lgNetVideoControlDecode(
    LGNetVideoControl * control, const void * data, size_t size)
{
  if (!control || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_VIDEO_CONTROL_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetVideoControl decoded;
  LGNetReader       reader;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU32(&reader, &decoded.streamID)    ||
      !lgNetReaderU32(&reader, &decoded.reason)      ||
      !lgNetReaderU64(&reader, &decoded.configEpoch) ||
      !lgNetReaderU64(&reader, &decoded.frameID))
    return LG_NET_PARSE_INVALID_VALUE;

  const LGNetParseResult result = fixedDecodeResult(
    &reader, LG_NET_VIDEO_CONTROL_WIRE_SIZE, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetVideoControlValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *control = decoded;
  return LG_NET_PARSE_OK;
}

bool lgNetVideoStreamConfigValid(const LGNetVideoStreamConfig * config)
{
  return config && config->streamID && videoCodecKnown(config->codec) &&
    config->codecVersion >= LG_NET_VIDEO_PYROWAVE_CODEC_VERSION_MIN &&
    config->codecVersion <= LG_NET_VIDEO_PYROWAVE_CODEC_VERSION_MAX &&
    config->configEpoch && config->width &&
    config->width <= LG_NET_VIDEO_MAX_WIDTH && config->height &&
    config->height <= LG_NET_VIDEO_MAX_HEIGHT &&
    (config->refreshNumerator == 0) ==
      (config->refreshDenominator == 0) &&
    videoPixelFormatKnown(config->pixelFormat) &&
    videoChromaKnown(config->chromaSubsampling) &&
    (config->chromaSubsampling != LG_NET_VIDEO_CHROMA_420 ||
      (!(config->width & 1U) && !(config->height & 1U))) &&
    colorPrimariesKnown(config->colorPrimaries) &&
    colorTransferKnown(config->colorTransfer) &&
    colorMatrixKnown(config->colorMatrix) &&
    colorRangeKnown(config->colorRange) && config->planeCount &&
    config->planeCount <= LG_NET_VIDEO_MAX_PLANES && config->bitDepth >= 8 &&
    config->bitDepth <= 16 && !(config->flags & ~VIDEO_STREAM_FLAGS) &&
    (config->flags & LG_NET_VIDEO_STREAM_GPU_PLANES_ONLY) &&
    videoPixelFormatValid(config) &&
    config->maxFrameLength &&
    config->maxFrameLength <= LG_NET_VIDEO_MAX_FRAME_LENGTH &&
    config->maxFragmentLength &&
    config->maxFragmentLength <= LG_NET_VIDEO_MAX_FRAGMENT_LENGTH &&
    config->maxFragmentLength <= config->maxFrameLength &&
    config->maxFrameLatencyMs;
}

bool lgNetVideoStreamConfigEncode(void * data, size_t size,
    const LGNetVideoStreamConfig * config)
{
  if (!data || size < LG_NET_VIDEO_STREAM_CONFIG_WIRE_SIZE ||
      !lgNetVideoStreamConfigValid(config))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU32(&writer, config->streamID)             &&
    lgNetWriterU16(&writer, config->codec)                &&
    lgNetWriterU16(&writer, config->codecVersion)         &&
    lgNetWriterU64(&writer, config->configEpoch)          &&
    lgNetWriterU32(&writer, config->width)                &&
    lgNetWriterU32(&writer, config->height)               &&
    lgNetWriterU32(&writer, config->refreshNumerator)     &&
    lgNetWriterU32(&writer, config->refreshDenominator)   &&
    lgNetWriterU16(&writer, config->pixelFormat)          &&
    lgNetWriterU8 (&writer, config->chromaSubsampling)    &&
    lgNetWriterU8 (&writer, config->colorPrimaries)       &&
    lgNetWriterU8 (&writer, config->colorTransfer)        &&
    lgNetWriterU8 (&writer, config->colorMatrix)          &&
    lgNetWriterU8 (&writer, config->colorRange)           &&
    lgNetWriterU8 (&writer, config->planeCount)           &&
    lgNetWriterU8 (&writer, config->bitDepth)             &&
    lgNetWriterU8 (&writer, 0)                            &&
    lgNetWriterU16(&writer, config->flags)                &&
    lgNetWriterU16(&writer, 0)                            &&
    lgNetWriterU32(&writer, config->maxFrameLength)       &&
    lgNetWriterU32(&writer, config->maxFragmentLength)    &&
    lgNetWriterU32(&writer, config->maxFrameLatencyMs)    &&
    lgNetWriterZero(&writer, 2)                           &&
    lgNetWriterSize(&writer) == LG_NET_VIDEO_STREAM_CONFIG_WIRE_SIZE;
}

LGNetParseResult lgNetVideoStreamConfigDecode(
    LGNetVideoStreamConfig * config, const void * data, size_t size)
{
  if (!config || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_VIDEO_STREAM_CONFIG_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetVideoStreamConfig decoded;
  LGNetReader            reader;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU32(&reader, &decoded.streamID)             ||
      !lgNetReaderU16(&reader, &decoded.codec)                ||
      !lgNetReaderU16(&reader, &decoded.codecVersion)         ||
      !lgNetReaderU64(&reader, &decoded.configEpoch)          ||
      !lgNetReaderU32(&reader, &decoded.width)                ||
      !lgNetReaderU32(&reader, &decoded.height)               ||
      !lgNetReaderU32(&reader, &decoded.refreshNumerator)     ||
      !lgNetReaderU32(&reader, &decoded.refreshDenominator)   ||
      !lgNetReaderU16(&reader, &decoded.pixelFormat)          ||
      !lgNetReaderU8 (&reader, &decoded.chromaSubsampling)    ||
      !lgNetReaderU8 (&reader, &decoded.colorPrimaries)       ||
      !lgNetReaderU8 (&reader, &decoded.colorTransfer)        ||
      !lgNetReaderU8 (&reader, &decoded.colorMatrix)          ||
      !lgNetReaderU8 (&reader, &decoded.colorRange)           ||
      !lgNetReaderU8 (&reader, &decoded.planeCount)           ||
      !lgNetReaderU8 (&reader, &decoded.bitDepth)             ||
      !lgNetReaderZero(&reader, 1)                            ||
      !lgNetReaderU16(&reader, &decoded.flags)                ||
      !lgNetReaderZero(&reader, 2)                            ||
      !lgNetReaderU32(&reader, &decoded.maxFrameLength)       ||
      !lgNetReaderU32(&reader, &decoded.maxFragmentLength)    ||
      !lgNetReaderU32(&reader, &decoded.maxFrameLatencyMs)    ||
      !lgNetReaderZero(&reader, 2))
    return LG_NET_PARSE_INVALID_VALUE;

  const LGNetParseResult result = fixedDecodeResult(
    &reader, LG_NET_VIDEO_STREAM_CONFIG_WIRE_SIZE, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetVideoStreamConfigValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *config = decoded;
  return LG_NET_PARSE_OK;
}

size_t lgNetVideoFrameSize(const LGNetVideoFrame * frame)
{
  size_t size;
  return frame && variableSize(LG_NET_VIDEO_FRAME_HEADER_WIRE_SIZE,
    frame->encodedLength, LG_NET_VIDEO_MAX_FRAME_LENGTH, &size) ? size : 0;
}

bool lgNetVideoFrameValid(const LGNetVideoFrame * frame)
{
  return frame && frame->streamID && videoCodecKnown(frame->codec) &&
    !(frame->flags & ~VIDEO_FRAME_FLAGS) && frame->configEpoch &&
    frame->frameID && frame->captureTimestampNs &&
    frame->presentationTimestampNs && frame->encodedLength && frame->data &&
    frame->blockCount && frame->blockCount <= LG_NET_VIDEO_MAX_BLOCKS &&
    frame->fragmentCount &&
    frame->fragmentCount <= LG_NET_VIDEO_MAX_FRAGMENTS && frame->deadlineMs &&
    ((frame->flags & LG_NET_VIDEO_FRAME_HAS_CHECKSUM) || !frame->checksum) &&
    lgNetVideoFrameSize(frame) != 0;
}

bool lgNetVideoFrameEncode(
    void * data, size_t size, const LGNetVideoFrame * frame)
{
  const size_t wireSize = lgNetVideoFrameSize(frame);
  if (!data || !wireSize || size < wireSize || !lgNetVideoFrameValid(frame))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU32(&writer, frame->streamID)                 &&
    lgNetWriterU16(&writer, frame->codec)                    &&
    lgNetWriterU16(&writer, frame->flags)                    &&
    lgNetWriterU64(&writer, frame->configEpoch)              &&
    lgNetWriterU64(&writer, frame->frameID)                  &&
    lgNetWriterU64(&writer, frame->captureTimestampNs)       &&
    lgNetWriterU64(&writer, frame->presentationTimestampNs)  &&
    lgNetWriterU32(&writer, frame->encodedLength)            &&
    lgNetWriterU32(&writer, frame->blockCount)               &&
    lgNetWriterU32(&writer, frame->fragmentCount)            &&
    lgNetWriterU32(&writer, frame->deadlineMs)               &&
    lgNetWriterU32(&writer, frame->checksum)                 &&
    lgNetWriterBytes(&writer, frame->data, frame->encodedLength) &&
    lgNetWriterSize(&writer) == wireSize;
}

LGNetParseResult lgNetVideoFrameDecode(
    LGNetVideoFrame * frame, const void * data, size_t size)
{
  if (!frame || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_VIDEO_FRAME_HEADER_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetVideoFrame decoded;
  LGNetReader     reader;
  size_t          expected;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU32(&reader, &decoded.streamID)                ||
      !lgNetReaderU16(&reader, &decoded.codec)                   ||
      !lgNetReaderU16(&reader, &decoded.flags)                   ||
      !lgNetReaderU64(&reader, &decoded.configEpoch)             ||
      !lgNetReaderU64(&reader, &decoded.frameID)                 ||
      !lgNetReaderU64(&reader, &decoded.captureTimestampNs)      ||
      !lgNetReaderU64(&reader, &decoded.presentationTimestampNs) ||
      !lgNetReaderU32(&reader, &decoded.encodedLength)           ||
      !lgNetReaderU32(&reader, &decoded.blockCount)              ||
      !lgNetReaderU32(&reader, &decoded.fragmentCount)           ||
      !lgNetReaderU32(&reader, &decoded.deadlineMs)              ||
      !lgNetReaderU32(&reader, &decoded.checksum))
    return LG_NET_PARSE_INVALID_VALUE;

  LGNetParseResult result = variableDecodeSize(
    LG_NET_VIDEO_FRAME_HEADER_WIRE_SIZE, decoded.encodedLength,
    LG_NET_VIDEO_MAX_FRAME_LENGTH, size, &expected);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetReaderView(&reader, &decoded.data, decoded.encodedLength))
    return LG_NET_PARSE_INVALID_VALUE;
  result = fixedDecodeResult(&reader, expected, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetVideoFrameValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *frame = decoded;
  return LG_NET_PARSE_OK;
}

size_t lgNetVideoDamageSize(const LGNetVideoDamage * damage)
{
  return damage && damage->count <= LG_NET_VIDEO_MAX_DAMAGE_RECTS ?
    LG_NET_VIDEO_DAMAGE_HEADER_WIRE_SIZE +
      (size_t)damage->count * LG_NET_VIDEO_DAMAGE_RECT_WIRE_SIZE : 0;
}

static bool videoDamageRectValid(const LGNetVideoDamageRect * rect,
    uint32_t frameWidth, uint32_t frameHeight)
{
  return rect && rect->width && rect->height && rect->x < frameWidth &&
    rect->y < frameHeight && rect->width <= frameWidth - rect->x &&
    rect->height <= frameHeight - rect->y;
}

bool lgNetVideoDamageValid(const LGNetVideoDamage * damage,
    uint32_t frameWidth, uint32_t frameHeight)
{
  if (!damage || !frameWidth || frameWidth > LG_NET_VIDEO_MAX_WIDTH ||
      !frameHeight || frameHeight > LG_NET_VIDEO_MAX_HEIGHT ||
      damage->count > LG_NET_VIDEO_MAX_DAMAGE_RECTS ||
      (damage->flags & ~LG_NET_VIDEO_DAMAGE_FULL) ||
      ((damage->flags & LG_NET_VIDEO_DAMAGE_FULL) && damage->count))
    return false;

  for (uint16_t i = 0; i < damage->count; ++i)
    if (!videoDamageRectValid(&damage->rects[i], frameWidth, frameHeight))
      return false;

  return true;
}

bool lgNetVideoDamageEncode(void * data, size_t size,
    const LGNetVideoDamage * damage, uint32_t frameWidth,
    uint32_t frameHeight)
{
  const size_t wireSize = lgNetVideoDamageSize(damage);
  if (!data || !wireSize || size < wireSize ||
      !lgNetVideoDamageValid(damage, frameWidth, frameHeight))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  if (!lgNetWriterU16(&writer, damage->flags) ||
      !lgNetWriterU16(&writer, damage->count))
    return false;

  for (uint16_t i = 0; i < damage->count; ++i)
  {
    const LGNetVideoDamageRect * rect = &damage->rects[i];
    if (!lgNetWriterU16(&writer, rect->x)     ||
        !lgNetWriterU16(&writer, rect->y)     ||
        !lgNetWriterU16(&writer, rect->width) ||
        !lgNetWriterU16(&writer, rect->height))
      return false;
  }

  return lgNetWriterSize(&writer) == wireSize;
}

LGNetParseResult lgNetVideoDamageDecode(LGNetVideoDamage * damage,
    const void * data, size_t size, uint32_t frameWidth,
    uint32_t frameHeight, size_t * consumed)
{
  if (!damage || !data || !consumed)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_VIDEO_DAMAGE_HEADER_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetVideoDamage decoded;
  LGNetReader      reader;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU16(&reader, &decoded.flags) ||
      !lgNetReaderU16(&reader, &decoded.count))
    return LG_NET_PARSE_INVALID_VALUE;
  if (decoded.count > LG_NET_VIDEO_MAX_DAMAGE_RECTS)
    return LG_NET_PARSE_INVALID_VALUE;

  const size_t wireSize = lgNetVideoDamageSize(&decoded);
  if (size < wireSize)
    return LG_NET_PARSE_TRUNCATED;

  for (uint16_t i = 0; i < decoded.count; ++i)
  {
    LGNetVideoDamageRect * rect = &decoded.rects[i];
    if (!lgNetReaderU16(&reader, &rect->x)     ||
        !lgNetReaderU16(&reader, &rect->y)     ||
        !lgNetReaderU16(&reader, &rect->width) ||
        !lgNetReaderU16(&reader, &rect->height))
      return LG_NET_PARSE_INVALID_VALUE;
  }

  if (!lgNetReaderValid(&reader) || lgNetReaderConsumed(&reader) != wireSize)
    return LG_NET_PARSE_INVALID_LENGTH;
  if (!lgNetVideoDamageValid(&decoded, frameWidth, frameHeight))
    return LG_NET_PARSE_INVALID_VALUE;

  *damage   = decoded;
  *consumed = wireSize;
  return LG_NET_PARSE_OK;
}

size_t lgNetVideoFrameUpdateSize(const LGNetVideoFrameUpdate * update)
{
  size_t size;
  return update && variableSize(LG_NET_VIDEO_FRAME_UPDATE_HEADER_WIRE_SIZE,
    update->encodedLength, LG_NET_VIDEO_MAX_FRAME_UPDATE_LENGTH, &size) ?
    size : 0;
}

bool lgNetVideoFrameUpdateValid(const LGNetVideoFrameUpdate * update)
{
  return update && update->streamID && videoCodecKnown(update->codec) &&
    !(update->flags & ~VIDEO_FRAME_UPDATE_FLAGS) && update->configEpoch &&
    update->frameID && update->baseFrameID &&
    update->baseFrameID < update->frameID && update->captureTimestampNs &&
    update->presentationTimestampNs && update->encodedLength && update->data &&
    update->blockCount && update->blockCount <= LG_NET_VIDEO_MAX_BLOCKS &&
    update->fragmentCount &&
    update->fragmentCount <= LG_NET_VIDEO_MAX_FRAGMENTS &&
    update->deadlineMs &&
    ((update->flags & LG_NET_VIDEO_FRAME_UPDATE_HAS_CHECKSUM) ||
      !update->checksum) &&
    lgNetVideoFrameUpdateSize(update) != 0;
}

bool lgNetVideoFrameUpdateEncode(
    void * data, size_t size, const LGNetVideoFrameUpdate * update)
{
  const size_t wireSize = lgNetVideoFrameUpdateSize(update);
  if (!data || !wireSize || size < wireSize ||
      !lgNetVideoFrameUpdateValid(update))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU32(&writer, update->streamID)                 &&
    lgNetWriterU16(&writer, update->codec)                    &&
    lgNetWriterU16(&writer, update->flags)                    &&
    lgNetWriterU64(&writer, update->configEpoch)              &&
    lgNetWriterU64(&writer, update->frameID)                  &&
    lgNetWriterU64(&writer, update->baseFrameID)              &&
    lgNetWriterU64(&writer, update->captureTimestampNs)       &&
    lgNetWriterU64(&writer, update->presentationTimestampNs)  &&
    lgNetWriterU32(&writer, update->encodedLength)            &&
    lgNetWriterU32(&writer, update->blockCount)               &&
    lgNetWriterU32(&writer, update->fragmentCount)            &&
    lgNetWriterU32(&writer, update->deadlineMs)               &&
    lgNetWriterU32(&writer, update->checksum)                 &&
    lgNetWriterZero(&writer, 4)                               &&
    lgNetWriterBytes(&writer, update->data,
      update->encodedLength)                                  &&
    lgNetWriterSize(&writer) == wireSize;
}

LGNetParseResult lgNetVideoFrameUpdateDecode(
    LGNetVideoFrameUpdate * update, const void * data, size_t size)
{
  if (!update || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_VIDEO_FRAME_UPDATE_HEADER_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetVideoFrameUpdate decoded;
  LGNetReader           reader;
  size_t                expected;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU32(&reader, &decoded.streamID)                ||
      !lgNetReaderU16(&reader, &decoded.codec)                   ||
      !lgNetReaderU16(&reader, &decoded.flags)                   ||
      !lgNetReaderU64(&reader, &decoded.configEpoch)             ||
      !lgNetReaderU64(&reader, &decoded.frameID)                 ||
      !lgNetReaderU64(&reader, &decoded.baseFrameID)             ||
      !lgNetReaderU64(&reader, &decoded.captureTimestampNs)      ||
      !lgNetReaderU64(&reader, &decoded.presentationTimestampNs) ||
      !lgNetReaderU32(&reader, &decoded.encodedLength)           ||
      !lgNetReaderU32(&reader, &decoded.blockCount)              ||
      !lgNetReaderU32(&reader, &decoded.fragmentCount)           ||
      !lgNetReaderU32(&reader, &decoded.deadlineMs)              ||
      !lgNetReaderU32(&reader, &decoded.checksum)                ||
      !lgNetReaderZero(&reader, 4))
    return LG_NET_PARSE_INVALID_VALUE;

  LGNetParseResult result = variableDecodeSize(
    LG_NET_VIDEO_FRAME_UPDATE_HEADER_WIRE_SIZE, decoded.encodedLength,
    LG_NET_VIDEO_MAX_FRAME_UPDATE_LENGTH, size, &expected);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetReaderView(&reader, &decoded.data, decoded.encodedLength))
    return LG_NET_PARSE_INVALID_VALUE;
  result = fixedDecodeResult(&reader, expected, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetVideoFrameUpdateValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *update = decoded;
  return LG_NET_PARSE_OK;
}

size_t lgNetVideoFragmentSize(const LGNetVideoFragment * fragment)
{
  size_t size;
  return fragment && variableSize(LG_NET_VIDEO_FRAGMENT_HEADER_WIRE_SIZE,
    fragment->payloadLength, LG_NET_VIDEO_MAX_FRAGMENT_LENGTH, &size) ?
    size : 0;
}

bool lgNetVideoFragmentValid(const LGNetVideoFragment * fragment)
{
  return fragment && fragment->streamID && fragment->configEpoch &&
    fragment->frameID && fragment->frameLength &&
    fragment->frameLength <= LG_NET_VIDEO_MAX_FRAME_LENGTH &&
    fragment->fragmentCount &&
    fragment->fragmentCount <= LG_NET_VIDEO_MAX_FRAGMENTS &&
    fragment->fragmentIndex < fragment->fragmentCount &&
    fragment->blockCount && fragment->blockCount <= LG_NET_VIDEO_MAX_BLOCKS &&
    fragment->blockIndex < fragment->blockCount && fragment->blockLength &&
    fragment->payloadLength &&
    fragment->payload && !(fragment->flags & ~VIDEO_FRAGMENT_FLAGS) &&
    fragment->frameOffset <= fragment->frameLength &&
    fragment->payloadLength <= fragment->frameLength - fragment->frameOffset &&
    fragment->blockOffset <= fragment->blockLength &&
    fragment->payloadLength <= fragment->blockLength - fragment->blockOffset &&
    fragment->frameOffset >= fragment->blockOffset &&
    fragment->blockLength <= fragment->frameLength -
      (fragment->frameOffset - fragment->blockOffset) &&
    (!!(fragment->flags & LG_NET_VIDEO_FRAGMENT_BLOCK_START) ==
      (fragment->blockOffset == 0)) &&
    (!!(fragment->flags & LG_NET_VIDEO_FRAGMENT_BLOCK_END) ==
      (fragment->blockOffset + fragment->payloadLength ==
        fragment->blockLength)) &&
    (!(fragment->flags & LG_NET_VIDEO_FRAGMENT_FRAME_END) ||
      fragment->frameOffset + fragment->payloadLength ==
        fragment->frameLength) &&
    (!!(fragment->flags & LG_NET_VIDEO_FRAGMENT_RECOVERY) ==
      !!fragment->recoveryCount) &&
    fragment->recoveryType <= LG_NET_VIDEO_RECOVERY_DUPLICATE &&
    (!!(fragment->flags & LG_NET_VIDEO_FRAGMENT_RECOVERY) ==
      (fragment->recoveryType != LG_NET_VIDEO_RECOVERY_NONE)) &&
    (!(fragment->flags & LG_NET_VIDEO_FRAGMENT_RECOVERY) ||
      (fragment->recoveryGroup && fragment->recoveryCount > 1 &&
        fragment->recoveryIndex < fragment->recoveryCount)) &&
    ((fragment->flags & LG_NET_VIDEO_FRAGMENT_RECOVERY) ||
      (!fragment->recoveryGroup && !fragment->recoveryIndex)) &&
    ((fragment->flags & LG_NET_VIDEO_FRAGMENT_CHECKSUM) ||
      !fragment->checksum) &&
    lgNetVideoFragmentSize(fragment) != 0;
}

bool lgNetVideoFragmentEncode(
    void * data, size_t size, const LGNetVideoFragment * fragment)
{
  const size_t wireSize = lgNetVideoFragmentSize(fragment);
  if (!data || !wireSize || size < wireSize ||
      !lgNetVideoFragmentValid(fragment))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU32 (&writer, fragment->streamID)       &&
    lgNetWriterU16 (&writer, fragment->flags)          &&
    lgNetWriterU16 (&writer, fragment->recoveryType)   &&
    lgNetWriterU64 (&writer, fragment->configEpoch)    &&
    lgNetWriterU64 (&writer, fragment->frameID)        &&
    lgNetWriterU32 (&writer, fragment->frameOffset)    &&
    lgNetWriterU32 (&writer, fragment->frameLength)    &&
    lgNetWriterU32 (&writer, fragment->blockIndex)     &&
    lgNetWriterU32 (&writer, fragment->blockOffset)    &&
    lgNetWriterU32 (&writer, fragment->blockLength)    &&
    lgNetWriterU32 (&writer, fragment->blockCount)     &&
    lgNetWriterU32 (&writer, fragment->fragmentIndex)  &&
    lgNetWriterU32 (&writer, fragment->fragmentCount)  &&
    lgNetWriterU32 (&writer, fragment->recoveryGroup)  &&
    lgNetWriterU16 (&writer, fragment->recoveryIndex)  &&
    lgNetWriterU16 (&writer, fragment->recoveryCount)  &&
    lgNetWriterU32 (&writer, fragment->payloadLength)  &&
    lgNetWriterU32 (&writer, fragment->checksum)       &&
    lgNetWriterBytes(&writer, fragment->payload,
      fragment->payloadLength)                         &&
    lgNetWriterSize(&writer) == wireSize;
}

LGNetParseResult lgNetVideoFragmentDecode(
    LGNetVideoFragment * fragment, const void * data, size_t size)
{
  if (!fragment || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_VIDEO_FRAGMENT_HEADER_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetVideoFragment decoded;
  LGNetReader        reader;
  size_t             expected;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU32 (&reader, &decoded.streamID)       ||
      !lgNetReaderU16 (&reader, &decoded.flags)          ||
      !lgNetReaderU16 (&reader, &decoded.recoveryType)   ||
      !lgNetReaderU64 (&reader, &decoded.configEpoch)    ||
      !lgNetReaderU64 (&reader, &decoded.frameID)        ||
      !lgNetReaderU32 (&reader, &decoded.frameOffset)    ||
      !lgNetReaderU32 (&reader, &decoded.frameLength)    ||
      !lgNetReaderU32 (&reader, &decoded.blockIndex)     ||
      !lgNetReaderU32 (&reader, &decoded.blockOffset)    ||
      !lgNetReaderU32 (&reader, &decoded.blockLength)    ||
      !lgNetReaderU32 (&reader, &decoded.blockCount)     ||
      !lgNetReaderU32 (&reader, &decoded.fragmentIndex)  ||
      !lgNetReaderU32 (&reader, &decoded.fragmentCount)  ||
      !lgNetReaderU32 (&reader, &decoded.recoveryGroup)  ||
      !lgNetReaderU16 (&reader, &decoded.recoveryIndex)  ||
      !lgNetReaderU16 (&reader, &decoded.recoveryCount)  ||
      !lgNetReaderU32 (&reader, &decoded.payloadLength)  ||
      !lgNetReaderU32 (&reader, &decoded.checksum))
    return LG_NET_PARSE_INVALID_VALUE;

  LGNetParseResult result = variableDecodeSize(
    LG_NET_VIDEO_FRAGMENT_HEADER_WIRE_SIZE, decoded.payloadLength,
    LG_NET_VIDEO_MAX_FRAGMENT_LENGTH, size, &expected);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetReaderView(&reader, &decoded.payload, decoded.payloadLength))
    return LG_NET_PARSE_INVALID_VALUE;
  result = fixedDecodeResult(&reader, expected, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetVideoFragmentValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *fragment = decoded;
  return LG_NET_PARSE_OK;
}

size_t lgNetVideoFrameUpdateFragmentSize(
    const LGNetVideoFrameUpdateFragment * fragment)
{
  size_t size;
  return fragment && variableSize(
    LG_NET_VIDEO_FRAME_UPDATE_FRAGMENT_HEADER_WIRE_SIZE,
    fragment->payloadLength,
    LG_NET_VIDEO_MAX_FRAME_UPDATE_FRAGMENT_LENGTH, &size) ? size : 0;
}

bool lgNetVideoFrameUpdateFragmentValid(
    const LGNetVideoFrameUpdateFragment * fragment)
{
  return fragment && fragment->streamID && fragment->configEpoch &&
    fragment->frameID && fragment->baseFrameID &&
    fragment->baseFrameID < fragment->frameID && fragment->frameLength &&
    fragment->frameLength <= LG_NET_VIDEO_MAX_FRAME_UPDATE_LENGTH &&
    fragment->fragmentCount &&
    fragment->fragmentCount <= LG_NET_VIDEO_MAX_FRAGMENTS &&
    fragment->fragmentIndex < fragment->fragmentCount &&
    fragment->blockCount && fragment->blockCount <= LG_NET_VIDEO_MAX_BLOCKS &&
    fragment->blockIndex < fragment->blockCount && fragment->blockLength &&
    fragment->payloadLength && fragment->payload &&
    !(fragment->flags & ~VIDEO_FRAGMENT_FLAGS) &&
    fragment->frameOffset <= fragment->frameLength &&
    fragment->payloadLength <= fragment->frameLength - fragment->frameOffset &&
    fragment->blockOffset <= fragment->blockLength &&
    fragment->payloadLength <= fragment->blockLength - fragment->blockOffset &&
    fragment->frameOffset >= fragment->blockOffset &&
    fragment->blockLength <= fragment->frameLength -
      (fragment->frameOffset - fragment->blockOffset) &&
    (!!(fragment->flags & LG_NET_VIDEO_FRAGMENT_BLOCK_START) ==
      (fragment->blockOffset == 0)) &&
    (!!(fragment->flags & LG_NET_VIDEO_FRAGMENT_BLOCK_END) ==
      (fragment->blockOffset + fragment->payloadLength ==
        fragment->blockLength)) &&
    (!(fragment->flags & LG_NET_VIDEO_FRAGMENT_FRAME_END) ||
      fragment->frameOffset + fragment->payloadLength ==
        fragment->frameLength) &&
    (!!(fragment->flags & LG_NET_VIDEO_FRAGMENT_RECOVERY) ==
      !!fragment->recoveryCount) &&
    fragment->recoveryType <= LG_NET_VIDEO_RECOVERY_DUPLICATE &&
    (!!(fragment->flags & LG_NET_VIDEO_FRAGMENT_RECOVERY) ==
      (fragment->recoveryType != LG_NET_VIDEO_RECOVERY_NONE)) &&
    (!(fragment->flags & LG_NET_VIDEO_FRAGMENT_RECOVERY) ||
      (fragment->recoveryGroup && fragment->recoveryCount > 1 &&
        fragment->recoveryIndex < fragment->recoveryCount)) &&
    ((fragment->flags & LG_NET_VIDEO_FRAGMENT_RECOVERY) ||
      (!fragment->recoveryGroup && !fragment->recoveryIndex)) &&
    ((fragment->flags & LG_NET_VIDEO_FRAGMENT_CHECKSUM) ||
      !fragment->checksum) &&
    lgNetVideoFrameUpdateFragmentSize(fragment) != 0;
}

bool lgNetVideoFrameUpdateFragmentEncode(void * data, size_t size,
    const LGNetVideoFrameUpdateFragment * fragment)
{
  const size_t wireSize = lgNetVideoFrameUpdateFragmentSize(fragment);
  if (!data || !wireSize || size < wireSize ||
      !lgNetVideoFrameUpdateFragmentValid(fragment))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU32 (&writer, fragment->streamID)       &&
    lgNetWriterU16 (&writer, fragment->flags)          &&
    lgNetWriterU16 (&writer, fragment->recoveryType)   &&
    lgNetWriterU64 (&writer, fragment->configEpoch)    &&
    lgNetWriterU64 (&writer, fragment->frameID)        &&
    lgNetWriterU64 (&writer, fragment->baseFrameID)    &&
    lgNetWriterU32 (&writer, fragment->frameOffset)    &&
    lgNetWriterU32 (&writer, fragment->frameLength)    &&
    lgNetWriterU32 (&writer, fragment->blockIndex)     &&
    lgNetWriterU32 (&writer, fragment->blockOffset)    &&
    lgNetWriterU32 (&writer, fragment->blockLength)    &&
    lgNetWriterU32 (&writer, fragment->blockCount)     &&
    lgNetWriterU32 (&writer, fragment->fragmentIndex)  &&
    lgNetWriterU32 (&writer, fragment->fragmentCount)  &&
    lgNetWriterU32 (&writer, fragment->recoveryGroup)  &&
    lgNetWriterU16 (&writer, fragment->recoveryIndex)  &&
    lgNetWriterU16 (&writer, fragment->recoveryCount)  &&
    lgNetWriterU32 (&writer, fragment->payloadLength)  &&
    lgNetWriterU32 (&writer, fragment->checksum)       &&
    lgNetWriterBytes(&writer, fragment->payload,
      fragment->payloadLength)                         &&
    lgNetWriterSize(&writer) == wireSize;
}

LGNetParseResult lgNetVideoFrameUpdateFragmentDecode(
    LGNetVideoFrameUpdateFragment * fragment, const void * data, size_t size)
{
  if (!fragment || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_VIDEO_FRAME_UPDATE_FRAGMENT_HEADER_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetVideoFrameUpdateFragment decoded;
  LGNetReader                  reader;
  size_t                       expected;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU32 (&reader, &decoded.streamID)       ||
      !lgNetReaderU16 (&reader, &decoded.flags)          ||
      !lgNetReaderU16 (&reader, &decoded.recoveryType)   ||
      !lgNetReaderU64 (&reader, &decoded.configEpoch)    ||
      !lgNetReaderU64 (&reader, &decoded.frameID)        ||
      !lgNetReaderU64 (&reader, &decoded.baseFrameID)    ||
      !lgNetReaderU32 (&reader, &decoded.frameOffset)    ||
      !lgNetReaderU32 (&reader, &decoded.frameLength)    ||
      !lgNetReaderU32 (&reader, &decoded.blockIndex)     ||
      !lgNetReaderU32 (&reader, &decoded.blockOffset)    ||
      !lgNetReaderU32 (&reader, &decoded.blockLength)    ||
      !lgNetReaderU32 (&reader, &decoded.blockCount)     ||
      !lgNetReaderU32 (&reader, &decoded.fragmentIndex)  ||
      !lgNetReaderU32 (&reader, &decoded.fragmentCount)  ||
      !lgNetReaderU32 (&reader, &decoded.recoveryGroup)  ||
      !lgNetReaderU16 (&reader, &decoded.recoveryIndex)  ||
      !lgNetReaderU16 (&reader, &decoded.recoveryCount)  ||
      !lgNetReaderU32 (&reader, &decoded.payloadLength)  ||
      !lgNetReaderU32 (&reader, &decoded.checksum))
    return LG_NET_PARSE_INVALID_VALUE;

  LGNetParseResult result = variableDecodeSize(
    LG_NET_VIDEO_FRAME_UPDATE_FRAGMENT_HEADER_WIRE_SIZE,
    decoded.payloadLength, LG_NET_VIDEO_MAX_FRAME_UPDATE_FRAGMENT_LENGTH,
    size, &expected);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetReaderView(&reader, &decoded.payload, decoded.payloadLength))
    return LG_NET_PARSE_INVALID_VALUE;
  result = fixedDecodeResult(&reader, expected, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetVideoFrameUpdateFragmentValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *fragment = decoded;
  return LG_NET_PARSE_OK;
}

bool lgNetVideoFeedbackValid(const LGNetVideoFeedback * feedback)
{
  return feedback && feedback->streamID &&
    !(feedback->flags & ~VIDEO_FEEDBACK_FLAGS) &&
    feedback->lastCompleteFrameID <= feedback->highestSeenFrameID &&
    feedback->lastDecodedFrameID <= feedback->lastCompleteFrameID;
}

bool lgNetVideoFeedbackEncode(
    void * data, size_t size, const LGNetVideoFeedback * feedback)
{
  if (!data || size < LG_NET_VIDEO_FEEDBACK_WIRE_SIZE ||
      !lgNetVideoFeedbackValid(feedback))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU32(&writer, feedback->streamID)            &&
    lgNetWriterU32(&writer, feedback->flags)               &&
    lgNetWriterU64(&writer, feedback->lastCompleteFrameID) &&
    lgNetWriterU64(&writer, feedback->highestSeenFrameID)  &&
    lgNetWriterU64(&writer, feedback->lastDecodedFrameID)  &&
    lgNetWriterU32(&writer, feedback->lostFrames)          &&
    lgNetWriterU32(&writer, feedback->lostFragments)       &&
    lgNetWriterU32(&writer, feedback->receiveQueueUs)      &&
    lgNetWriterU32(&writer, feedback->decodeQueueUs)       &&
    lgNetWriterU32(&writer, feedback->roundTripUs)         &&
    lgNetWriterU32(&writer, 0)                             &&
    lgNetWriterSize(&writer) == LG_NET_VIDEO_FEEDBACK_WIRE_SIZE;
}

LGNetParseResult lgNetVideoFeedbackDecode(
    LGNetVideoFeedback * feedback, const void * data, size_t size)
{
  if (!feedback || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_VIDEO_FEEDBACK_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetVideoFeedback decoded;
  LGNetReader        reader;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU32(&reader, &decoded.streamID)            ||
      !lgNetReaderU32(&reader, &decoded.flags)               ||
      !lgNetReaderU64(&reader, &decoded.lastCompleteFrameID) ||
      !lgNetReaderU64(&reader, &decoded.highestSeenFrameID)  ||
      !lgNetReaderU64(&reader, &decoded.lastDecodedFrameID)  ||
      !lgNetReaderU32(&reader, &decoded.lostFrames)          ||
      !lgNetReaderU32(&reader, &decoded.lostFragments)       ||
      !lgNetReaderU32(&reader, &decoded.receiveQueueUs)      ||
      !lgNetReaderU32(&reader, &decoded.decodeQueueUs)       ||
      !lgNetReaderU32(&reader, &decoded.roundTripUs)         ||
      !lgNetReaderZero(&reader, 4))
    return LG_NET_PARSE_INVALID_VALUE;

  const LGNetParseResult result = fixedDecodeResult(
    &reader, LG_NET_VIDEO_FEEDBACK_WIRE_SIZE, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetVideoFeedbackValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *feedback = decoded;
  return LG_NET_PARSE_OK;
}

bool lgNetVideoScheduleValid(const LGNetVideoSchedule * schedule)
{
  return schedule && schedule->streamID && schedule->configEpoch &&
    schedule->frameID && schedule->captureTimestampNs &&
    schedule->presentationTimestampNs && schedule->deadlineTimestampNs &&
    schedule->captureTimestampNs <= schedule->presentationTimestampNs &&
    schedule->captureTimestampNs <= schedule->deadlineTimestampNs &&
    schedule->deadlineTimestampNs <= schedule->presentationTimestampNs &&
    schedule->flags && !(schedule->flags & ~VIDEO_SCHEDULE_FLAGS);
}

bool lgNetVideoScheduleEncode(
    void * data, size_t size, const LGNetVideoSchedule * schedule)
{
  if (!data || size < LG_NET_VIDEO_SCHEDULE_WIRE_SIZE ||
      !lgNetVideoScheduleValid(schedule))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU32(&writer, schedule->streamID)                &&
    lgNetWriterU32(&writer, schedule->flags)                   &&
    lgNetWriterU64(&writer, schedule->configEpoch)             &&
    lgNetWriterU64(&writer, schedule->frameID)                 &&
    lgNetWriterU64(&writer, schedule->captureTimestampNs)      &&
    lgNetWriterU64(&writer, schedule->presentationTimestampNs) &&
    lgNetWriterU64(&writer, schedule->deadlineTimestampNs)     &&
    lgNetWriterSize(&writer) == LG_NET_VIDEO_SCHEDULE_WIRE_SIZE;
}

LGNetParseResult lgNetVideoScheduleDecode(
    LGNetVideoSchedule * schedule, const void * data, size_t size)
{
  if (!schedule || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_VIDEO_SCHEDULE_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetVideoSchedule decoded;
  LGNetReader        reader;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU32(&reader, &decoded.streamID)                ||
      !lgNetReaderU32(&reader, &decoded.flags)                   ||
      !lgNetReaderU64(&reader, &decoded.configEpoch)             ||
      !lgNetReaderU64(&reader, &decoded.frameID)                 ||
      !lgNetReaderU64(&reader, &decoded.captureTimestampNs)      ||
      !lgNetReaderU64(&reader, &decoded.presentationTimestampNs) ||
      !lgNetReaderU64(&reader, &decoded.deadlineTimestampNs))
    return LG_NET_PARSE_INVALID_VALUE;

  const LGNetParseResult result = fixedDecodeResult(
    &reader, LG_NET_VIDEO_SCHEDULE_WIRE_SIZE, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetVideoScheduleValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *schedule = decoded;
  return LG_NET_PARSE_OK;
}

static bool videoStateKnown(LGNetVideoState state)
{
  return state >= LG_NET_VIDEO_STATE_SUBSCRIBED &&
    state <= LG_NET_VIDEO_STATE_ERROR;
}

bool lgNetVideoStatusValid(const LGNetVideoStatus * status)
{
  return status && status->streamID && status->statusSequence &&
    videoStateKnown(status->state) && !(status->flags & ~VIDEO_STATUS_FLAGS);
}

bool lgNetVideoStatusEncode(
    void * data, size_t size, const LGNetVideoStatus * status)
{
  if (!data || size < LG_NET_VIDEO_STATUS_WIRE_SIZE ||
      !lgNetVideoStatusValid(status))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU32 (&writer, status->streamID)       &&
    lgNetWriterU16 (&writer, status->state)          &&
    lgNetWriterZero(&writer, 2)                      &&
    lgNetWriterU64 (&writer, status->configEpoch)    &&
    lgNetWriterU64 (&writer, status->lastFrameID)    &&
    lgNetWriterU64 (&writer, status->statusSequence) &&
    lgNetWriterU32 (&writer, status->flags)          &&
    lgNetWriterU32 (&writer, status->detail)         &&
    lgNetWriterSize(&writer) == LG_NET_VIDEO_STATUS_WIRE_SIZE;
}

LGNetParseResult lgNetVideoStatusDecode(
    LGNetVideoStatus * status, const void * data, size_t size)
{
  if (!status || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_VIDEO_STATUS_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetVideoStatus decoded;
  LGNetReader      reader;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU32 (&reader, &decoded.streamID)       ||
      !lgNetReaderU16 (&reader, &decoded.state)          ||
      !lgNetReaderZero(&reader, 2)                       ||
      !lgNetReaderU64 (&reader, &decoded.configEpoch)    ||
      !lgNetReaderU64 (&reader, &decoded.lastFrameID)    ||
      !lgNetReaderU64 (&reader, &decoded.statusSequence) ||
      !lgNetReaderU32 (&reader, &decoded.flags)          ||
      !lgNetReaderU32 (&reader, &decoded.detail))
    return LG_NET_PARSE_INVALID_VALUE;

  const LGNetParseResult result = fixedDecodeResult(
    &reader, LG_NET_VIDEO_STATUS_WIRE_SIZE, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetVideoStatusValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *status = decoded;
  return LG_NET_PARSE_OK;
}

bool lgNetCursorPositionValid(const LGNetCursorPosition * position)
{
  return position && position->updateID && position->timestampNs &&
    position->desktopWidth && position->desktopHeight &&
    position->desktopWidth <= LG_NET_CURSOR_MAX_DESKTOP_WIDTH &&
    position->desktopHeight <= LG_NET_CURSOR_MAX_DESKTOP_HEIGHT &&
    !(position->flags & ~CURSOR_POSITION_FLAGS);
}

bool lgNetCursorPositionEncode(
    void * data, size_t size, const LGNetCursorPosition * position)
{
  if (!data || size < LG_NET_CURSOR_POSITION_WIRE_SIZE ||
      !lgNetCursorPositionValid(position))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU64(&writer, position->updateID)      &&
    lgNetWriterU64(&writer, position->timestampNs)   &&
    lgNetWriterI32(&writer, position->x)             &&
    lgNetWriterI32(&writer, position->y)             &&
    lgNetWriterU32(&writer, position->desktopWidth)  &&
    lgNetWriterU32(&writer, position->desktopHeight) &&
    lgNetWriterU32(&writer, position->flags)         &&
    lgNetWriterU32(&writer, position->displayID)     &&
    lgNetWriterSize(&writer) == LG_NET_CURSOR_POSITION_WIRE_SIZE;
}

LGNetParseResult lgNetCursorPositionDecode(
    LGNetCursorPosition * position, const void * data, size_t size)
{
  if (!position || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_CURSOR_POSITION_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetCursorPosition decoded;
  LGNetReader         reader;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU64(&reader, &decoded.updateID)      ||
      !lgNetReaderU64(&reader, &decoded.timestampNs)   ||
      !lgNetReaderI32(&reader, &decoded.x)             ||
      !lgNetReaderI32(&reader, &decoded.y)             ||
      !lgNetReaderU32(&reader, &decoded.desktopWidth)  ||
      !lgNetReaderU32(&reader, &decoded.desktopHeight) ||
      !lgNetReaderU32(&reader, &decoded.flags)         ||
      !lgNetReaderU32(&reader, &decoded.displayID))
    return LG_NET_PARSE_INVALID_VALUE;

  const LGNetParseResult result = fixedDecodeResult(
    &reader, LG_NET_CURSOR_POSITION_WIRE_SIZE, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetCursorPositionValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *position = decoded;
  return LG_NET_PARSE_OK;
}

static bool cursorFormatKnown(LGNetCursorFormat format)
{
  return format >= LG_NET_CURSOR_FORMAT_BGRA8_PREMULTIPLIED &&
    format <= LG_NET_CURSOR_FORMAT_MONOCHROME;
}

static bool cursorShapeLayoutValid(const LGNetCursorShape * shape)
{
  uint32_t minimumPitch;
  switch (shape->format)
  {
    case LG_NET_CURSOR_FORMAT_BGRA8_PREMULTIPLIED:
    case LG_NET_CURSOR_FORMAT_RGBA8_PREMULTIPLIED:
    case LG_NET_CURSOR_FORMAT_MASKED_COLOR:
      minimumPitch = shape->width * 4U;
      break;

    case LG_NET_CURSOR_FORMAT_MONOCHROME:
      minimumPitch = (shape->width + 7U) / 8U;
      break;

    default:
      return false;
  }

  return shape->pitch >= minimumPitch &&
    shape->height <= shape->dataLength / shape->pitch;
}

size_t lgNetCursorShapeSize(const LGNetCursorShape * shape)
{
  size_t size;
  return shape && variableSize(LG_NET_CURSOR_SHAPE_HEADER_WIRE_SIZE,
    shape->dataLength, LG_NET_CURSOR_MAX_SHAPE_LENGTH, &size) ? size : 0;
}

bool lgNetCursorShapeValid(const LGNetCursorShape * shape)
{
  return shape && shape->shapeID && shape->width &&
    shape->width <= LG_NET_CURSOR_MAX_WIDTH && shape->height &&
    shape->height <= LG_NET_CURSOR_MAX_HEIGHT && shape->pitch &&
    shape->dataLength && shape->data && cursorFormatKnown(shape->format) &&
    !(shape->flags & ~CURSOR_SHAPE_FLAGS) && shape->hotspotX < shape->width &&
    shape->hotspotY < shape->height &&
    cursorShapeLayoutValid(shape) &&
    ((shape->flags & LG_NET_CURSOR_SHAPE_ANIMATED) ?
      shape->frameDurationMs != 0 : shape->frameDurationMs == 0) &&
    lgNetCursorShapeSize(shape) != 0;
}

bool lgNetCursorShapeEncode(
    void * data, size_t size, const LGNetCursorShape * shape)
{
  const size_t wireSize = lgNetCursorShapeSize(shape);
  if (!data || !wireSize || size < wireSize || !lgNetCursorShapeValid(shape))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU64(&writer, shape->shapeID)         &&
    lgNetWriterU32(&writer, shape->width)           &&
    lgNetWriterU32(&writer, shape->height)          &&
    lgNetWriterU32(&writer, shape->pitch)           &&
    lgNetWriterU32(&writer, shape->dataLength)      &&
    lgNetWriterU16(&writer, shape->format)          &&
    lgNetWriterU16(&writer, shape->flags)           &&
    lgNetWriterU32(&writer, shape->hotspotX)        &&
    lgNetWriterU32(&writer, shape->hotspotY)        &&
    lgNetWriterU32(&writer, shape->frameDurationMs) &&
    lgNetWriterBytes(&writer, shape->data, shape->dataLength) &&
    lgNetWriterSize(&writer) == wireSize;
}

LGNetParseResult lgNetCursorShapeDecode(
    LGNetCursorShape * shape, const void * data, size_t size)
{
  if (!shape || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_CURSOR_SHAPE_HEADER_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetCursorShape decoded;
  LGNetReader      reader;
  size_t           expected;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU64(&reader, &decoded.shapeID)         ||
      !lgNetReaderU32(&reader, &decoded.width)           ||
      !lgNetReaderU32(&reader, &decoded.height)          ||
      !lgNetReaderU32(&reader, &decoded.pitch)           ||
      !lgNetReaderU32(&reader, &decoded.dataLength)      ||
      !lgNetReaderU16(&reader, &decoded.format)          ||
      !lgNetReaderU16(&reader, &decoded.flags)           ||
      !lgNetReaderU32(&reader, &decoded.hotspotX)        ||
      !lgNetReaderU32(&reader, &decoded.hotspotY)        ||
      !lgNetReaderU32(&reader, &decoded.frameDurationMs))
    return LG_NET_PARSE_INVALID_VALUE;

  LGNetParseResult result = variableDecodeSize(
    LG_NET_CURSOR_SHAPE_HEADER_WIRE_SIZE, decoded.dataLength,
    LG_NET_CURSOR_MAX_SHAPE_LENGTH, size, &expected);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetReaderView(&reader, &decoded.data, decoded.dataLength))
    return LG_NET_PARSE_INVALID_VALUE;
  result = fixedDecodeResult(&reader, expected, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetCursorShapeValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *shape = decoded;
  return LG_NET_PARSE_OK;
}

bool lgNetCursorStateValid(const LGNetCursorState * state)
{
  return state && state->stateSequence &&
    !(state->flags & ~CURSOR_STATE_FLAGS) &&
    (!!state->shapeID ==
      !!(state->flags & LG_NET_CURSOR_STATE_SHAPE_VALID)) &&
    (!!state->positionID ==
      !!(state->flags & LG_NET_CURSOR_STATE_POSITION_VALID)) &&
    (!!state->transformID ==
      !!(state->flags & LG_NET_CURSOR_STATE_TRANSFORM_VALID));
}

bool lgNetCursorStateEncode(
    void * data, size_t size, const LGNetCursorState * state)
{
  if (!data || size < LG_NET_CURSOR_STATE_WIRE_SIZE ||
      !lgNetCursorStateValid(state))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU64(&writer, state->stateSequence) &&
    lgNetWriterU64(&writer, state->shapeID)       &&
    lgNetWriterU64(&writer, state->positionID)    &&
    lgNetWriterU64(&writer, state->transformID)   &&
    lgNetWriterU32(&writer, state->flags)         &&
    lgNetWriterU32(&writer, state->displayID)     &&
    lgNetWriterSize(&writer) == LG_NET_CURSOR_STATE_WIRE_SIZE;
}

LGNetParseResult lgNetCursorStateDecode(
    LGNetCursorState * state, const void * data, size_t size)
{
  if (!state || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_CURSOR_STATE_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetCursorState decoded;
  LGNetReader      reader;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU64(&reader, &decoded.stateSequence) ||
      !lgNetReaderU64(&reader, &decoded.shapeID)       ||
      !lgNetReaderU64(&reader, &decoded.positionID)    ||
      !lgNetReaderU64(&reader, &decoded.transformID)   ||
      !lgNetReaderU32(&reader, &decoded.flags)         ||
      !lgNetReaderU32(&reader, &decoded.displayID))
    return LG_NET_PARSE_INVALID_VALUE;

  const LGNetParseResult result = fixedDecodeResult(
    &reader, LG_NET_CURSOR_STATE_WIRE_SIZE, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetCursorStateValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *state = decoded;
  return LG_NET_PARSE_OK;
}

static bool cursorRotationKnown(LGNetCursorRotation rotation)
{
  return rotation >= LG_NET_CURSOR_ROTATION_0 &&
    rotation <= LG_NET_CURSOR_ROTATION_270;
}

bool lgNetCursorTransformValid(const LGNetCursorTransform * transform)
{
  return transform && transform->transformID &&
    cursorRotationKnown(transform->rotation) &&
    !(transform->flags & ~CURSOR_TRANSFORM_FLAGS) &&
    transform->sourceWidth && transform->sourceWidth <= LG_NET_VIDEO_MAX_WIDTH &&
    transform->sourceHeight &&
    transform->sourceHeight <= LG_NET_VIDEO_MAX_HEIGHT &&
    transform->targetWidth &&
    transform->targetWidth <= LG_NET_CURSOR_MAX_DESKTOP_WIDTH &&
    transform->targetHeight &&
    transform->targetHeight <= LG_NET_CURSOR_MAX_DESKTOP_HEIGHT &&
    transform->scaleNumerator && transform->scaleDenominator;
}

bool lgNetCursorTransformEncode(
    void * data, size_t size, const LGNetCursorTransform * transform)
{
  if (!data || size < LG_NET_CURSOR_TRANSFORM_WIRE_SIZE ||
      !lgNetCursorTransformValid(transform))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU64(&writer, transform->transformID)      &&
    lgNetWriterU32(&writer, transform->displayID)        &&
    lgNetWriterU16(&writer, transform->rotation)         &&
    lgNetWriterU16(&writer, transform->flags)            &&
    lgNetWriterU32(&writer, transform->sourceWidth)      &&
    lgNetWriterU32(&writer, transform->sourceHeight)     &&
    lgNetWriterU32(&writer, transform->targetWidth)      &&
    lgNetWriterU32(&writer, transform->targetHeight)     &&
    lgNetWriterI32(&writer, transform->offsetX)          &&
    lgNetWriterI32(&writer, transform->offsetY)          &&
    lgNetWriterU32(&writer, transform->scaleNumerator)   &&
    lgNetWriterU32(&writer, transform->scaleDenominator) &&
    lgNetWriterSize(&writer) == LG_NET_CURSOR_TRANSFORM_WIRE_SIZE;
}

LGNetParseResult lgNetCursorTransformDecode(
    LGNetCursorTransform * transform, const void * data, size_t size)
{
  if (!transform || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_CURSOR_TRANSFORM_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetCursorTransform decoded;
  LGNetReader          reader;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU64(&reader, &decoded.transformID)      ||
      !lgNetReaderU32(&reader, &decoded.displayID)        ||
      !lgNetReaderU16(&reader, &decoded.rotation)         ||
      !lgNetReaderU16(&reader, &decoded.flags)            ||
      !lgNetReaderU32(&reader, &decoded.sourceWidth)      ||
      !lgNetReaderU32(&reader, &decoded.sourceHeight)     ||
      !lgNetReaderU32(&reader, &decoded.targetWidth)      ||
      !lgNetReaderU32(&reader, &decoded.targetHeight)     ||
      !lgNetReaderI32(&reader, &decoded.offsetX)          ||
      !lgNetReaderI32(&reader, &decoded.offsetY)          ||
      !lgNetReaderU32(&reader, &decoded.scaleNumerator)   ||
      !lgNetReaderU32(&reader, &decoded.scaleDenominator))
    return LG_NET_PARSE_INVALID_VALUE;

  const LGNetParseResult result = fixedDecodeResult(
    &reader, LG_NET_CURSOR_TRANSFORM_WIRE_SIZE, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetCursorTransformValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *transform = decoded;
  return LG_NET_PARSE_OK;
}

bool lgNetCursorColorLUTEncode(void * data, size_t size,
    const float lut[LG_NET_CURSOR_COLOR_LUT_FLOATS])
{
  if (!data || size < LG_NET_CURSOR_COLOR_LUT_BYTES || !lut)
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  for (size_t index = 0; index < LG_NET_CURSOR_COLOR_LUT_FLOATS; ++index)
    if (!writerFloat(&writer, lut[index]))
      return false;
  return lgNetWriterSize(&writer) == LG_NET_CURSOR_COLOR_LUT_BYTES;
}

bool lgNetCursorColorLUTDecode(
    float lut[LG_NET_CURSOR_COLOR_LUT_FLOATS], const void * data, size_t size)
{
  if (!lut || !data || size != LG_NET_CURSOR_COLOR_LUT_BYTES)
    return false;

  LGNetReader reader;
  lgNetReaderInit(&reader, data, size);
  for (size_t index = 0; index < LG_NET_CURSOR_COLOR_LUT_FLOATS; ++index)
    if (!readerFloat(&reader, &lut[index]))
      return false;
  return lgNetReaderConsumed(&reader) == LG_NET_CURSOR_COLOR_LUT_BYTES;
}

static bool cursorColorLUTValid(const uint8_t * data)
{
  if (!data)
    return false;

  LGNetReader reader;
  lgNetReaderInit(&reader, data, LG_NET_CURSOR_COLOR_LUT_BYTES);
  for (size_t index = 0; index < LG_NET_CURSOR_COLOR_LUT_FLOATS; ++index)
  {
    float value;
    if (!readerFloat(&reader, &value))
      return false;
  }
  return lgNetReaderConsumed(&reader) == LG_NET_CURSOR_COLOR_LUT_BYTES;
}

size_t lgNetCursorColorTransformSize(
    const LGNetCursorColorTransform * transform)
{
  size_t wireSize;
  const size_t lutSize = transform &&
    (transform->flags & LG_NET_CURSOR_COLOR_TRANSFORM_LUT) ?
      LG_NET_CURSOR_COLOR_LUT_BYTES : 0;
  return transform && variableSize(
      LG_NET_CURSOR_COLOR_TRANSFORM_HEADER_WIRE_SIZE,
      lutSize, LG_NET_CURSOR_COLOR_LUT_BYTES, &wireSize) ? wireSize : 0;
}

bool lgNetCursorColorTransformValid(
    const LGNetCursorColorTransform * transform)
{
  if (!transform || !transform->updateID || !transform->sdrWhiteLevel ||
      transform->sdrWhiteLevel > 10000U ||
      (transform->flags & ~CURSOR_COLOR_TRANSFORM_FLAGS) ||
      !finiteFloat(transform->scalar) ||
      (!!transform->lut !=
        !!(transform->flags & LG_NET_CURSOR_COLOR_TRANSFORM_LUT)))
    return false;

  for (size_t index = 0; index < LG_NET_CURSOR_COLOR_MATRIX_FLOATS; ++index)
    if (!finiteFloat(transform->matrix[index]))
      return false;
  return !(transform->flags & LG_NET_CURSOR_COLOR_TRANSFORM_LUT) ||
    cursorColorLUTValid(transform->lut);
}

bool lgNetCursorColorTransformEncode(void * data, size_t size,
    const LGNetCursorColorTransform * transform)
{
  const size_t wireSize = lgNetCursorColorTransformSize(transform);
  if (!data || !wireSize || size < wireSize ||
      !lgNetCursorColorTransformValid(transform))
    return false;

  const uint32_t lutLength =
    (transform->flags & LG_NET_CURSOR_COLOR_TRANSFORM_LUT) ?
      LG_NET_CURSOR_COLOR_LUT_BYTES : 0;
  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  if (!lgNetWriterU64(&writer, transform->updateID)       ||
      !lgNetWriterU32(&writer, transform->flags)          ||
      !lgNetWriterU32(&writer, transform->sdrWhiteLevel))
    return false;
  for (size_t index = 0; index < LG_NET_CURSOR_COLOR_MATRIX_FLOATS; ++index)
    if (!writerFloat(&writer, transform->matrix[index]))
      return false;
  return
    writerFloat(&writer, transform->scalar)              &&
    lgNetWriterU32(&writer, lutLength)                   &&
    (!lutLength || lgNetWriterBytes(
      &writer, transform->lut, lutLength))               &&
    lgNetWriterSize(&writer) == wireSize;
}

LGNetParseResult lgNetCursorColorTransformDecode(
    LGNetCursorColorTransform * transform, const void * data, size_t size)
{
  if (!transform || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_CURSOR_COLOR_TRANSFORM_HEADER_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetCursorColorTransform decoded;
  LGNetReader               reader;
  uint32_t                  lutLength;
  size_t                    expected;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU64(&reader, &decoded.updateID)       ||
      !lgNetReaderU32(&reader, &decoded.flags)          ||
      !lgNetReaderU32(&reader, &decoded.sdrWhiteLevel))
    return LG_NET_PARSE_INVALID_VALUE;
  for (size_t index = 0; index < LG_NET_CURSOR_COLOR_MATRIX_FLOATS; ++index)
    if (!readerFloat(&reader, &decoded.matrix[index]))
      return LG_NET_PARSE_INVALID_VALUE;
  if (!readerFloat(&reader, &decoded.scalar) ||
      !lgNetReaderU32(&reader, &lutLength))
    return LG_NET_PARSE_INVALID_VALUE;

  const uint32_t requiredLength =
    (decoded.flags & LG_NET_CURSOR_COLOR_TRANSFORM_LUT) ?
      LG_NET_CURSOR_COLOR_LUT_BYTES : 0;
  if (lutLength != requiredLength)
    return LG_NET_PARSE_INVALID_VALUE;
  LGNetParseResult result = variableDecodeSize(
    LG_NET_CURSOR_COLOR_TRANSFORM_HEADER_WIRE_SIZE, lutLength,
    LG_NET_CURSOR_COLOR_LUT_BYTES, size, &expected);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (lutLength && !lgNetReaderView(&reader, &decoded.lut, lutLength))
    return LG_NET_PARSE_INVALID_VALUE;
  result = fixedDecodeResult(&reader, expected, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetCursorColorTransformValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *transform = decoded;
  return LG_NET_PARSE_OK;
}

static bool cursorStatusKnown(LGNetCursorStatusCode status)
{
  return status >= LG_NET_CURSOR_STATUS_APPLIED &&
    status <= LG_NET_CURSOR_STATUS_ERROR;
}

bool lgNetCursorStatusValid(const LGNetCursorStatus * status)
{
  return status && status->stateSequence && cursorStatusKnown(status->status);
}

bool lgNetCursorStatusEncode(
    void * data, size_t size, const LGNetCursorStatus * status)
{
  if (!data || size < LG_NET_CURSOR_STATUS_WIRE_SIZE ||
      !lgNetCursorStatusValid(status))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU64(&writer, status->stateSequence)      &&
    lgNetWriterU64(&writer, status->appliedShapeID)     &&
    lgNetWriterU64(&writer, status->appliedPositionID)  &&
    lgNetWriterU64(&writer, status->appliedTransformID) &&
    lgNetWriterU32(&writer, status->status)             &&
    lgNetWriterU32(&writer, status->detail)             &&
    lgNetWriterSize(&writer) == LG_NET_CURSOR_STATUS_WIRE_SIZE;
}

LGNetParseResult lgNetCursorStatusDecode(
    LGNetCursorStatus * status, const void * data, size_t size)
{
  if (!status || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_CURSOR_STATUS_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetCursorStatus decoded;
  LGNetReader       reader;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU64(&reader, &decoded.stateSequence)      ||
      !lgNetReaderU64(&reader, &decoded.appliedShapeID)     ||
      !lgNetReaderU64(&reader, &decoded.appliedPositionID)  ||
      !lgNetReaderU64(&reader, &decoded.appliedTransformID) ||
      !lgNetReaderU32(&reader, &decoded.status)             ||
      !lgNetReaderU32(&reader, &decoded.detail))
    return LG_NET_PARSE_INVALID_VALUE;

  const LGNetParseResult result = fixedDecodeResult(
    &reader, LG_NET_CURSOR_STATUS_WIRE_SIZE, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetCursorStatusValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *status = decoded;
  return LG_NET_PARSE_OK;
}

bool lgNetControlCursorPositionValid(
    const LGNetControlCursorPosition * position)
{
  return position && position->controlID;
}

bool lgNetControlCursorPositionEncode(void * data, size_t size,
    const LGNetControlCursorPosition * position)
{
  if (!data || size < LG_NET_CONTROL_CURSOR_POSITION_WIRE_SIZE ||
      !lgNetControlCursorPositionValid(position))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU64(&writer, position->controlID) &&
    lgNetWriterI32(&writer, position->x)         &&
    lgNetWriterI32(&writer, position->y)         &&
    lgNetWriterSize(&writer) == LG_NET_CONTROL_CURSOR_POSITION_WIRE_SIZE;
}

LGNetParseResult lgNetControlCursorPositionDecode(
    LGNetControlCursorPosition * position, const void * data, size_t size)
{
  if (!position || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_CONTROL_CURSOR_POSITION_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetControlCursorPosition decoded;
  LGNetReader                reader;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU64(&reader, &decoded.controlID) ||
      !lgNetReaderI32(&reader, &decoded.x)         ||
      !lgNetReaderI32(&reader, &decoded.y))
    return LG_NET_PARSE_INVALID_VALUE;

  const LGNetParseResult result = fixedDecodeResult(
    &reader, LG_NET_CONTROL_CURSOR_POSITION_WIRE_SIZE, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetControlCursorPositionValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *position = decoded;
  return LG_NET_PARSE_OK;
}

bool lgNetControlDisplaySizeValid(const LGNetControlDisplaySize * display)
{
  return display && display->controlID && display->width && display->height &&
    display->width  <= LG_NET_CURSOR_MAX_DESKTOP_WIDTH &&
    display->height <= LG_NET_CURSOR_MAX_DESKTOP_HEIGHT;
}

bool lgNetControlDisplaySizeEncode(void * data, size_t size,
    const LGNetControlDisplaySize * display)
{
  if (!data || size < LG_NET_CONTROL_DISPLAY_SIZE_WIRE_SIZE ||
      !lgNetControlDisplaySizeValid(display))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU64(&writer, display->controlID) &&
    lgNetWriterU32(&writer, display->width)     &&
    lgNetWriterU32(&writer, display->height)    &&
    lgNetWriterSize(&writer) == LG_NET_CONTROL_DISPLAY_SIZE_WIRE_SIZE;
}

LGNetParseResult lgNetControlDisplaySizeDecode(
    LGNetControlDisplaySize * display, const void * data, size_t size)
{
  if (!display || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_CONTROL_DISPLAY_SIZE_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetControlDisplaySize decoded;
  LGNetReader             reader;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU64(&reader, &decoded.controlID) ||
      !lgNetReaderU32(&reader, &decoded.width)     ||
      !lgNetReaderU32(&reader, &decoded.height))
    return LG_NET_PARSE_INVALID_VALUE;

  const LGNetParseResult result = fixedDecodeResult(
    &reader, LG_NET_CONTROL_DISPLAY_SIZE_WIRE_SIZE, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetControlDisplaySizeValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *display = decoded;
  return LG_NET_PARSE_OK;
}

bool lgNetControlFrameScheduleValid(
    const LGNetControlFrameSchedule * schedule)
{
  if (!schedule || !schedule->controlID || !schedule->flags ||
      (schedule->flags & ~CONTROL_FRAME_SCHEDULE_FLAGS) ||
      schedule->leaseMs > LG_NET_CONTROL_MAX_LEASE_MS)
    return false;

  if (schedule->flags & LG_NET_CONTROL_FRAME_SCHEDULE_ACTIVE)
    return schedule->generation && schedule->periodNs && schedule->leaseMs;
  return !(schedule->flags & LG_NET_CONTROL_FRAME_SCHEDULE_IMMEDIATE);
}

bool lgNetControlFrameScheduleEncode(void * data, size_t size,
    const LGNetControlFrameSchedule * schedule)
{
  if (!data || size < LG_NET_CONTROL_FRAME_SCHEDULE_WIRE_SIZE ||
      !lgNetControlFrameScheduleValid(schedule))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU64(&writer, schedule->controlID)              &&
    lgNetWriterU32(&writer, schedule->generation)             &&
    lgNetWriterU32(&writer, schedule->flags)                  &&
    lgNetWriterU64(&writer, schedule->periodNs)               &&
    lgNetWriterU64(&writer, schedule->targetSlackNs)          &&
    lgNetWriterI64(&writer, schedule->phaseErrorNs)           &&
    lgNetWriterU32(&writer, schedule->feedbackFrameSerial)    &&
    lgNetWriterU32(&writer, schedule->feedbackScheduleEpoch)  &&
    lgNetWriterU32(&writer, schedule->feedbackDeadlineSerial) &&
    lgNetWriterU32(&writer, schedule->leaseMs)                &&
    lgNetWriterSize(&writer) == LG_NET_CONTROL_FRAME_SCHEDULE_WIRE_SIZE;
}

LGNetParseResult lgNetControlFrameScheduleDecode(
    LGNetControlFrameSchedule * schedule, const void * data, size_t size)
{
  if (!schedule || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_CONTROL_FRAME_SCHEDULE_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetControlFrameSchedule decoded;
  LGNetReader               reader;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU64(&reader, &decoded.controlID)              ||
      !lgNetReaderU32(&reader, &decoded.generation)             ||
      !lgNetReaderU32(&reader, &decoded.flags)                  ||
      !lgNetReaderU64(&reader, &decoded.periodNs)               ||
      !lgNetReaderU64(&reader, &decoded.targetSlackNs)          ||
      !lgNetReaderI64(&reader, &decoded.phaseErrorNs)           ||
      !lgNetReaderU32(&reader, &decoded.feedbackFrameSerial)    ||
      !lgNetReaderU32(&reader, &decoded.feedbackScheduleEpoch)  ||
      !lgNetReaderU32(&reader, &decoded.feedbackDeadlineSerial) ||
      !lgNetReaderU32(&reader, &decoded.leaseMs))
    return LG_NET_PARSE_INVALID_VALUE;

  const LGNetParseResult result = fixedDecodeResult(
    &reader, LG_NET_CONTROL_FRAME_SCHEDULE_WIRE_SIZE, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetControlFrameScheduleValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *schedule = decoded;
  return LG_NET_PARSE_OK;
}

static bool controlStatusKnown(LGNetControlStatusCode status)
{
  return status >= LG_NET_CONTROL_STATUS_APPLIED &&
    status <= LG_NET_CONTROL_STATUS_ERROR;
}

bool lgNetControlStatusValid(const LGNetControlStatus * status)
{
  return status && status->controlID && controlStatusKnown(status->status);
}

bool lgNetControlStatusEncode(
    void * data, size_t size, const LGNetControlStatus * status)
{
  if (!data || size < LG_NET_CONTROL_STATUS_WIRE_SIZE ||
      !lgNetControlStatusValid(status))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU64(&writer, status->controlID) &&
    lgNetWriterU32(&writer, status->status)    &&
    lgNetWriterU32(&writer, status->detail)    &&
    lgNetWriterSize(&writer) == LG_NET_CONTROL_STATUS_WIRE_SIZE;
}

LGNetParseResult lgNetControlStatusDecode(
    LGNetControlStatus * status, const void * data, size_t size)
{
  if (!status || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_CONTROL_STATUS_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetControlStatus decoded;
  LGNetReader        reader;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU64(&reader, &decoded.controlID) ||
      !lgNetReaderU32(&reader, &decoded.status)    ||
      !lgNetReaderU32(&reader, &decoded.detail))
    return LG_NET_PARSE_INVALID_VALUE;

  const LGNetParseResult result = fixedDecodeResult(
    &reader, LG_NET_CONTROL_STATUS_WIRE_SIZE, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetControlStatusValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *status = decoded;
  return LG_NET_PARSE_OK;
}

bool lgNetInputClaimValid(const LGNetInputClaim * claim)
{
  return claim && claim->claimantID && claim->claimEpoch && claim->leaseMs &&
    claim->leaseMs <= LG_NET_INPUT_MAX_LEASE_MS &&
    claim->flags && !(claim->flags & ~INPUT_CLAIM_FLAGS) &&
    (!(claim->flags & LG_NET_INPUT_CLAIM_EXCLUSIVE) ||
      (claim->flags & (LG_NET_INPUT_CLAIM_KEYBOARD |
        LG_NET_INPUT_CLAIM_POINTER)));
}

bool lgNetInputClaimEncode(
    void * data, size_t size, const LGNetInputClaim * claim)
{
  if (!data || size < LG_NET_INPUT_CLAIM_WIRE_SIZE ||
      !lgNetInputClaimValid(claim))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU64(&writer, claim->claimantID) &&
    lgNetWriterU64(&writer, claim->claimEpoch) &&
    lgNetWriterU32(&writer, claim->leaseMs)    &&
    lgNetWriterU32(&writer, claim->flags)      &&
    lgNetWriterSize(&writer) == LG_NET_INPUT_CLAIM_WIRE_SIZE;
}

LGNetParseResult lgNetInputClaimDecode(
    LGNetInputClaim * claim, const void * data, size_t size)
{
  if (!claim || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_INPUT_CLAIM_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetInputClaim decoded;
  LGNetReader     reader;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU64(&reader, &decoded.claimantID) ||
      !lgNetReaderU64(&reader, &decoded.claimEpoch) ||
      !lgNetReaderU32(&reader, &decoded.leaseMs)    ||
      !lgNetReaderU32(&reader, &decoded.flags))
    return LG_NET_PARSE_INVALID_VALUE;

  const LGNetParseResult result = fixedDecodeResult(
    &reader, LG_NET_INPUT_CLAIM_WIRE_SIZE, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetInputClaimValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *claim = decoded;
  return LG_NET_PARSE_OK;
}

static bool inputStatusKnown(LGNetInputStatusCode status)
{
  return status >= LG_NET_INPUT_STATUS_ACCEPTED &&
    status <= LG_NET_INPUT_STATUS_ERROR;
}

bool lgNetInputStatusValid(const LGNetInputStatus * status)
{
  return status && status->claimEpoch && inputStatusKnown(status->status) &&
    !(status->flags & ~INPUT_CLAIM_FLAGS) &&
    (!(status->flags & LG_NET_INPUT_CLAIM_EXCLUSIVE) ||
      (status->flags & (LG_NET_INPUT_CLAIM_KEYBOARD |
        LG_NET_INPUT_CLAIM_POINTER))) &&
    status->leaseRemainingMs <= LG_NET_INPUT_MAX_LEASE_MS;
}

bool lgNetInputStatusEncode(
    void * data, size_t size, const LGNetInputStatus * status)
{
  if (!data || size < LG_NET_INPUT_STATUS_WIRE_SIZE ||
      !lgNetInputStatusValid(status))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU64(&writer, status->claimEpoch)       &&
    lgNetWriterU64(&writer, status->lastSequence)     &&
    lgNetWriterU32(&writer, status->status)           &&
    lgNetWriterU32(&writer, status->flags)            &&
    lgNetWriterU32(&writer, status->leaseRemainingMs) &&
    lgNetWriterU32(&writer, status->detail)           &&
    lgNetWriterSize(&writer) == LG_NET_INPUT_STATUS_WIRE_SIZE;
}

LGNetParseResult lgNetInputStatusDecode(
    LGNetInputStatus * status, const void * data, size_t size)
{
  if (!status || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_INPUT_STATUS_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetInputStatus decoded;
  LGNetReader      reader;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU64(&reader, &decoded.claimEpoch)       ||
      !lgNetReaderU64(&reader, &decoded.lastSequence)     ||
      !lgNetReaderU32(&reader, &decoded.status)           ||
      !lgNetReaderU32(&reader, &decoded.flags)            ||
      !lgNetReaderU32(&reader, &decoded.leaseRemainingMs) ||
      !lgNetReaderU32(&reader, &decoded.detail))
    return LG_NET_PARSE_INVALID_VALUE;

  const LGNetParseResult result = fixedDecodeResult(
    &reader, LG_NET_INPUT_STATUS_WIRE_SIZE, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetInputStatusValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *status = decoded;
  return LG_NET_PARSE_OK;
}

bool lgNetInputControlValid(const LGNetInputControl * control)
{
  return control && control->claimantID && control->claimEpoch &&
    control->sequence && !(control->flags & ~INPUT_CLAIM_FLAGS) &&
    control->leaseMs <= LG_NET_INPUT_MAX_LEASE_MS;
}

bool lgNetInputControlEncode(
    void * data, size_t size, const LGNetInputControl * control)
{
  if (!data || size < LG_NET_INPUT_CONTROL_WIRE_SIZE ||
      !lgNetInputControlValid(control))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU64(&writer, control->claimantID) &&
    lgNetWriterU64(&writer, control->claimEpoch) &&
    lgNetWriterU64(&writer, control->sequence)   &&
    lgNetWriterU32(&writer, control->flags)      &&
    lgNetWriterU32(&writer, control->leaseMs)    &&
    lgNetWriterSize(&writer) == LG_NET_INPUT_CONTROL_WIRE_SIZE;
}

LGNetParseResult lgNetInputControlDecode(
    LGNetInputControl * control, const void * data, size_t size)
{
  if (!control || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_INPUT_CONTROL_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetInputControl decoded;
  LGNetReader       reader;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU64(&reader, &decoded.claimantID) ||
      !lgNetReaderU64(&reader, &decoded.claimEpoch) ||
      !lgNetReaderU64(&reader, &decoded.sequence)   ||
      !lgNetReaderU32(&reader, &decoded.flags)      ||
      !lgNetReaderU32(&reader, &decoded.leaseMs))
    return LG_NET_PARSE_INVALID_VALUE;

  const LGNetParseResult result = fixedDecodeResult(
    &reader, LG_NET_INPUT_CONTROL_WIRE_SIZE, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetInputControlValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *control = decoded;
  return LG_NET_PARSE_OK;
}

bool lgNetInputRelativeValid(const LGNetInputRelative * relative)
{
  return relative && relative->sequence && relative->timestampNs &&
    !(relative->buttons & ~LG_NET_POINTER_BUTTON_MASK) &&
    !(relative->changedButtons & ~LG_NET_POINTER_BUTTON_MASK);
}

bool lgNetInputRelativeEncode(
    void * data, size_t size, const LGNetInputRelative * relative)
{
  if (!data || size < LG_NET_INPUT_RELATIVE_WIRE_SIZE ||
      !lgNetInputRelativeValid(relative))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU64(&writer, relative->sequence)       &&
    lgNetWriterU64(&writer, relative->timestampNs)    &&
    lgNetWriterI64(&writer, relative->cumulativeX)    &&
    lgNetWriterI64(&writer, relative->cumulativeY)    &&
    lgNetWriterI32(&writer, relative->wheelX)         &&
    lgNetWriterI32(&writer, relative->wheelY)         &&
    lgNetWriterU32(&writer, relative->buttons)        &&
    lgNetWriterU32(&writer, relative->changedButtons) &&
    lgNetWriterSize(&writer) == LG_NET_INPUT_RELATIVE_WIRE_SIZE;
}

LGNetParseResult lgNetInputRelativeDecode(
    LGNetInputRelative * relative, const void * data, size_t size)
{
  if (!relative || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_INPUT_RELATIVE_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetInputRelative decoded;
  LGNetReader        reader;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU64(&reader, &decoded.sequence)        ||
      !lgNetReaderU64(&reader, &decoded.timestampNs)     ||
      !lgNetReaderI64(&reader, &decoded.cumulativeX)     ||
      !lgNetReaderI64(&reader, &decoded.cumulativeY)     ||
      !lgNetReaderI32(&reader, &decoded.wheelX)          ||
      !lgNetReaderI32(&reader, &decoded.wheelY)          ||
      !lgNetReaderU32(&reader, &decoded.buttons)         ||
      !lgNetReaderU32(&reader, &decoded.changedButtons))
    return LG_NET_PARSE_INVALID_VALUE;

  const LGNetParseResult result = fixedDecodeResult(
    &reader, LG_NET_INPUT_RELATIVE_WIRE_SIZE, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetInputRelativeValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *relative = decoded;
  return LG_NET_PARSE_OK;
}

bool lgNetInputAbsoluteValid(const LGNetInputAbsolute * absolute)
{
  return absolute && absolute->sequence && absolute->timestampNs &&
    absolute->width &&
    absolute->width <= LG_NET_INPUT_ABSOLUTE_MAX_WIDTH &&
    absolute->height &&
    absolute->height <= LG_NET_INPUT_ABSOLUTE_MAX_HEIGHT &&
    absolute->x >= 0 && absolute->y >= 0 &&
    (uint32_t)absolute->x < absolute->width &&
    (uint32_t)absolute->y < absolute->height &&
    !(absolute->buttons & ~LG_NET_POINTER_BUTTON_MASK) &&
    !(absolute->changedButtons & ~LG_NET_POINTER_BUTTON_MASK);
}

bool lgNetInputAbsoluteEncode(
    void * data, size_t size, const LGNetInputAbsolute * absolute)
{
  if (!data || size < LG_NET_INPUT_ABSOLUTE_WIRE_SIZE ||
      !lgNetInputAbsoluteValid(absolute))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU64(&writer, absolute->sequence)        &&
    lgNetWriterU64(&writer, absolute->timestampNs)     &&
    lgNetWriterI64(&writer, absolute->cumulativeX)     &&
    lgNetWriterI64(&writer, absolute->cumulativeY)     &&
    lgNetWriterI32(&writer, absolute->x)               &&
    lgNetWriterI32(&writer, absolute->y)               &&
    lgNetWriterU32(&writer, absolute->width)           &&
    lgNetWriterU32(&writer, absolute->height)          &&
    lgNetWriterI32(&writer, absolute->wheelX)          &&
    lgNetWriterI32(&writer, absolute->wheelY)          &&
    lgNetWriterU32(&writer, absolute->buttons)         &&
    lgNetWriterU32(&writer, absolute->changedButtons)  &&
    lgNetWriterSize(&writer) == LG_NET_INPUT_ABSOLUTE_WIRE_SIZE;
}

LGNetParseResult lgNetInputAbsoluteDecode(
    LGNetInputAbsolute * absolute, const void * data, size_t size)
{
  if (!absolute || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_INPUT_ABSOLUTE_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetInputAbsolute decoded;
  LGNetReader        reader;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU64(&reader, &decoded.sequence)        ||
      !lgNetReaderU64(&reader, &decoded.timestampNs)     ||
      !lgNetReaderI64(&reader, &decoded.cumulativeX)     ||
      !lgNetReaderI64(&reader, &decoded.cumulativeY)     ||
      !lgNetReaderI32(&reader, &decoded.x)               ||
      !lgNetReaderI32(&reader, &decoded.y)               ||
      !lgNetReaderU32(&reader, &decoded.width)           ||
      !lgNetReaderU32(&reader, &decoded.height)          ||
      !lgNetReaderI32(&reader, &decoded.wheelX)          ||
      !lgNetReaderI32(&reader, &decoded.wheelY)          ||
      !lgNetReaderU32(&reader, &decoded.buttons)         ||
      !lgNetReaderU32(&reader, &decoded.changedButtons))
    return LG_NET_PARSE_INVALID_VALUE;

  const LGNetParseResult result = fixedDecodeResult(
    &reader, LG_NET_INPUT_ABSOLUTE_WIRE_SIZE, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetInputAbsoluteValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *absolute = decoded;
  return LG_NET_PARSE_OK;
}

bool lgNetInputKeyboardValid(const LGNetInputKeyboard * keyboard)
{
  return keyboard && keyboard->sequence && keyboard->timestampNs &&
    (keyboard->usage || keyboard->scanCode) &&
    !(keyboard->flags & ~KEYBOARD_FLAGS) &&
    (!(keyboard->flags & LG_NET_KEYBOARD_REPEAT) ||
      (keyboard->flags & LG_NET_KEYBOARD_DOWN)) &&
    !(keyboard->modifiers & ~LG_NET_INPUT_KEYBOARD_MODIFIER_MASK);
}

bool lgNetInputKeyboardEncode(
    void * data, size_t size, const LGNetInputKeyboard * keyboard)
{
  if (!data || size < LG_NET_INPUT_KEYBOARD_WIRE_SIZE ||
      !lgNetInputKeyboardValid(keyboard))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU64(&writer, keyboard->sequence)    &&
    lgNetWriterU64(&writer, keyboard->timestampNs) &&
    lgNetWriterU16(&writer, keyboard->usagePage)   &&
    lgNetWriterU16(&writer, keyboard->usage)       &&
    lgNetWriterU16(&writer, keyboard->scanCode)    &&
    lgNetWriterU16(&writer, keyboard->flags)       &&
    lgNetWriterU32(&writer, keyboard->modifiers)   &&
    lgNetWriterU32(&writer, 0)                     &&
    lgNetWriterSize(&writer) == LG_NET_INPUT_KEYBOARD_WIRE_SIZE;
}

LGNetParseResult lgNetInputKeyboardDecode(
    LGNetInputKeyboard * keyboard, const void * data, size_t size)
{
  if (!keyboard || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_INPUT_KEYBOARD_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetInputKeyboard decoded;
  LGNetReader        reader;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU64(&reader, &decoded.sequence)    ||
      !lgNetReaderU64(&reader, &decoded.timestampNs) ||
      !lgNetReaderU16(&reader, &decoded.usagePage)   ||
      !lgNetReaderU16(&reader, &decoded.usage)       ||
      !lgNetReaderU16(&reader, &decoded.scanCode)    ||
      !lgNetReaderU16(&reader, &decoded.flags)       ||
      !lgNetReaderU32(&reader, &decoded.modifiers)   ||
      !lgNetReaderZero(&reader, 4))
    return LG_NET_PARSE_INVALID_VALUE;

  const LGNetParseResult result = fixedDecodeResult(
    &reader, LG_NET_INPUT_KEYBOARD_WIRE_SIZE, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetInputKeyboardValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *keyboard = decoded;
  return LG_NET_PARSE_OK;
}

bool lgNetInputLEDsValid(const LGNetInputLEDs * leds)
{
  return leds && leds->sequence && leds->validMask &&
    !(leds->state & ~KEYBOARD_LEDS) && !(leds->validMask & ~KEYBOARD_LEDS) &&
    !(leds->state & ~leds->validMask);
}

bool lgNetInputLEDsEncode(
    void * data, size_t size, const LGNetInputLEDs * leds)
{
  if (!data || size < LG_NET_INPUT_LEDS_WIRE_SIZE ||
      !lgNetInputLEDsValid(leds))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU64(&writer, leds->sequence)  &&
    lgNetWriterU32(&writer, leds->state)     &&
    lgNetWriterU32(&writer, leds->validMask) &&
    lgNetWriterSize(&writer) == LG_NET_INPUT_LEDS_WIRE_SIZE;
}

LGNetParseResult lgNetInputLEDsDecode(
    LGNetInputLEDs * leds, const void * data, size_t size)
{
  if (!leds || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_INPUT_LEDS_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetInputLEDs decoded;
  LGNetReader    reader;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU64(&reader, &decoded.sequence)  ||
      !lgNetReaderU32(&reader, &decoded.state)     ||
      !lgNetReaderU32(&reader, &decoded.validMask))
    return LG_NET_PARSE_INVALID_VALUE;

  const LGNetParseResult result = fixedDecodeResult(
    &reader, LG_NET_INPUT_LEDS_WIRE_SIZE, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetInputLEDsValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *leds = decoded;
  return LG_NET_PARSE_OK;
}

static bool audioDirectionKnown(LGNetAudioDirection direction)
{
  return direction == LG_NET_AUDIO_DIRECTION_PLAYBACK ||
    direction == LG_NET_AUDIO_DIRECTION_CAPTURE;
}

static bool audioRolesValid(LGNetAudioRoles roles)
{
  const LGNetAudioRoles directions =
    roles & (LG_NET_AUDIO_ROLE_PLAYBACK | LG_NET_AUDIO_ROLE_CAPTURE);
  const LGNetAudioRoles delivery   =
    roles & (LG_NET_AUDIO_ROLE_DATAGRAM_PCM |
      LG_NET_AUDIO_ROLE_RELIABLE_PCM);

  return roles && !(roles & ~AUDIO_ROLES) && directions && delivery &&
    (!(roles & LG_NET_AUDIO_ROLE_EXCLUSIVE_CAPTURE) ||
      (roles & LG_NET_AUDIO_ROLE_CAPTURE));
}

bool lgNetAudioSubscribeValid(const LGNetAudioSubscribe * subscribe)
{
  return subscribe && subscribe->subscriberID && subscribe->directions &&
    !(subscribe->directions & ~AUDIO_DIRECTIONS) &&
    subscribe->targetLatencyUs && subscribe->maxPacketFrames &&
    subscribe->maxPacketFrames <= LG_NET_AUDIO_MAX_PACKET_FRAMES &&
    !(subscribe->flags & ~AUDIO_SUBSCRIBE_FLAGS);
}

LGNetAudioRoles lgNetAudioSubscribeRoles(
    const LGNetAudioSubscribe * subscribe)
{
  if (!subscribe)
    return 0;

  LGNetAudioRoles roles = 0;
  if (subscribe->directions & LG_NET_AUDIO_DIRECTIONS_PLAYBACK)
    roles |= LG_NET_AUDIO_ROLE_PLAYBACK;
  if (subscribe->directions & LG_NET_AUDIO_DIRECTIONS_CAPTURE)
    roles |= LG_NET_AUDIO_ROLE_CAPTURE;
  if (subscribe->flags & LG_NET_AUDIO_SUBSCRIBE_CLOCK_FEEDBACK)
    roles |= LG_NET_AUDIO_ROLE_CLOCK_FEEDBACK;
  if (subscribe->flags & LG_NET_AUDIO_SUBSCRIBE_DATAGRAM_PCM)
    roles |= LG_NET_AUDIO_ROLE_DATAGRAM_PCM;
  if (subscribe->flags & LG_NET_AUDIO_SUBSCRIBE_RELIABLE_PCM)
    roles |= LG_NET_AUDIO_ROLE_RELIABLE_PCM;
  if (subscribe->flags & LG_NET_AUDIO_SUBSCRIBE_EXCLUSIVE_CAPTURE)
    roles |= LG_NET_AUDIO_ROLE_EXCLUSIVE_CAPTURE;

  return roles;
}

bool lgNetAudioSubscribeValidForVersion(
    const LGNetAudioSubscribe * subscribe, uint16_t serviceVersion)
{
  if (!lgNetAudioSubscribeValid(subscribe))
    return false;

  if (serviceVersion == LG_NET_AUDIO_VERSION_INITIAL)
    return !(subscribe->flags & ~AUDIO_SUBSCRIBE_FLAGS_V1);

  if (serviceVersion ==
      LG_NET_AUDIO_OWNERSHIP_INTRODUCED_SERVICE_VERSION)
    return audioRolesValid(lgNetAudioSubscribeRoles(subscribe));

  return false;
}

bool lgNetAudioSubscribeEncode(
    void * data, size_t size, const LGNetAudioSubscribe * subscribe)
{
  if (!data || size < LG_NET_AUDIO_SUBSCRIBE_WIRE_SIZE ||
      !lgNetAudioSubscribeValid(subscribe))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU64(&writer, subscribe->subscriberID)    &&
    lgNetWriterU32(&writer, subscribe->directions)      &&
    lgNetWriterU32(&writer, subscribe->targetLatencyUs) &&
    lgNetWriterU32(&writer, subscribe->maxPacketFrames) &&
    lgNetWriterU32(&writer, subscribe->flags)           &&
    lgNetWriterSize(&writer) == LG_NET_AUDIO_SUBSCRIBE_WIRE_SIZE;
}

bool lgNetAudioSubscribeEncodeForVersion(void * data, size_t size,
    const LGNetAudioSubscribe * subscribe, uint16_t serviceVersion)
{
  return lgNetAudioSubscribeValidForVersion(subscribe, serviceVersion) &&
    lgNetAudioSubscribeEncode(data, size, subscribe);
}

LGNetParseResult lgNetAudioSubscribeDecode(
    LGNetAudioSubscribe * subscribe, const void * data, size_t size)
{
  if (!subscribe || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_AUDIO_SUBSCRIBE_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetAudioSubscribe decoded;
  LGNetReader         reader;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU64(&reader, &decoded.subscriberID)    ||
      !lgNetReaderU32(&reader, &decoded.directions)      ||
      !lgNetReaderU32(&reader, &decoded.targetLatencyUs) ||
      !lgNetReaderU32(&reader, &decoded.maxPacketFrames) ||
      !lgNetReaderU32(&reader, &decoded.flags))
    return LG_NET_PARSE_INVALID_VALUE;

  const LGNetParseResult result = fixedDecodeResult(
    &reader, LG_NET_AUDIO_SUBSCRIBE_WIRE_SIZE, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetAudioSubscribeValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *subscribe = decoded;
  return LG_NET_PARSE_OK;
}

LGNetParseResult lgNetAudioSubscribeDecodeForVersion(
    LGNetAudioSubscribe * subscribe, uint16_t serviceVersion,
    const void * data, size_t size)
{
  const LGNetParseResult result =
    lgNetAudioSubscribeDecode(subscribe, data, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetAudioSubscribeValidForVersion(subscribe, serviceVersion))
    return LG_NET_PARSE_INVALID_VALUE;
  return LG_NET_PARSE_OK;
}

static bool audioGrantStatusKnown(LGNetAudioGrantStatus status)
{
  return status >= LG_NET_AUDIO_GRANT_GRANTED &&
    status <= LG_NET_AUDIO_GRANT_UNSUPPORTED;
}

bool lgNetAudioSubscriptionGrantValid(
    const LGNetAudioSubscriptionGrant * grant)
{
  if (!grant || !grant->subscriberID || !grant->subscriptionEpoch ||
      !audioGrantStatusKnown(grant->status) ||
      !audioRolesValid(grant->requestedRoles) ||
      (grant->grantedRoles & ~grant->requestedRoles))
    return false;

  if (grant->status == LG_NET_AUDIO_GRANT_GRANTED ||
      grant->status == LG_NET_AUDIO_GRANT_PARTIAL)
  {
    if (!audioRolesValid(grant->grantedRoles) || !grant->targetLatencyUs ||
        !grant->maxPacketFrames ||
        grant->maxPacketFrames > LG_NET_AUDIO_MAX_PACKET_FRAMES)
      return false;

    return grant->status == LG_NET_AUDIO_GRANT_GRANTED ?
      grant->grantedRoles == grant->requestedRoles :
      grant->grantedRoles != grant->requestedRoles;
  }

  return !grant->grantedRoles && !grant->targetLatencyUs &&
    !grant->maxPacketFrames;
}

bool lgNetAudioSubscriptionGrantEncode(void * data, size_t size,
    const LGNetAudioSubscriptionGrant * grant)
{
  if (!data || size < LG_NET_AUDIO_SUBSCRIPTION_GRANT_WIRE_SIZE ||
      !lgNetAudioSubscriptionGrantValid(grant))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU64 (&writer, grant->subscriberID)      &&
    lgNetWriterU64 (&writer, grant->subscriptionEpoch) &&
    lgNetWriterU32 (&writer, grant->requestedRoles)    &&
    lgNetWriterU32 (&writer, grant->grantedRoles)      &&
    lgNetWriterU32 (&writer, grant->targetLatencyUs)   &&
    lgNetWriterU32 (&writer, grant->maxPacketFrames)   &&
    lgNetWriterU16 (&writer, grant->status)            &&
    lgNetWriterZero(&writer, 2)                        &&
    lgNetWriterU32 (&writer, grant->detail)            &&
    lgNetWriterSize(&writer) ==
      LG_NET_AUDIO_SUBSCRIPTION_GRANT_WIRE_SIZE;
}

LGNetParseResult lgNetAudioSubscriptionGrantDecode(
    LGNetAudioSubscriptionGrant * grant, const void * data, size_t size)
{
  if (!grant || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_AUDIO_SUBSCRIPTION_GRANT_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetAudioSubscriptionGrant decoded;
  LGNetReader                 reader;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU64 (&reader, &decoded.subscriberID)      ||
      !lgNetReaderU64 (&reader, &decoded.subscriptionEpoch) ||
      !lgNetReaderU32 (&reader, &decoded.requestedRoles)    ||
      !lgNetReaderU32 (&reader, &decoded.grantedRoles)      ||
      !lgNetReaderU32 (&reader, &decoded.targetLatencyUs)   ||
      !lgNetReaderU32 (&reader, &decoded.maxPacketFrames)   ||
      !lgNetReaderU16 (&reader, &decoded.status)            ||
      !lgNetReaderZero(&reader, 2)                          ||
      !lgNetReaderU32 (&reader, &decoded.detail))
    return LG_NET_PARSE_INVALID_VALUE;

  const LGNetParseResult result = fixedDecodeResult(&reader,
    LG_NET_AUDIO_SUBSCRIPTION_GRANT_WIRE_SIZE, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetAudioSubscriptionGrantValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *grant = decoded;
  return LG_NET_PARSE_OK;
}

bool lgNetAudioEnvelopeMatchesSubscription(
    const LGNetEnvelope * envelope, uint64_t subscriptionEpoch)
{
  return envelope && subscriptionEpoch &&
    envelope->service == LG_NET_SERVICE_AUDIO &&
    envelope->serviceVersion >=
      LG_NET_AUDIO_OWNERSHIP_INTRODUCED_SERVICE_VERSION &&
    envelope->messageType != LG_NET_AUDIO_MESSAGE_SUBSCRIBE &&
    envelope->componentEpoch == subscriptionEpoch;
}

static bool audioSampleFormatKnown(LGNetAudioSampleFormat format)
{
  return format >= LG_NET_AUDIO_SAMPLE_S16_LE &&
    format <= LG_NET_AUDIO_SAMPLE_F32_LE;
}

static size_t audioSampleSize(LGNetAudioSampleFormat format)
{
  switch (format)
  {
    case LG_NET_AUDIO_SAMPLE_S16_LE:
      return 2;

    case LG_NET_AUDIO_SAMPLE_S24_LE:
      return 3;

    case LG_NET_AUDIO_SAMPLE_S32_LE:
    case LG_NET_AUDIO_SAMPLE_F32_LE:
      return 4;
  }

  return 0;
}

bool lgNetAudioFormatValid(const LGNetAudioFormat * format)
{
  return format && format->streamID &&
    audioDirectionKnown(format->direction) &&
    audioSampleFormatKnown(format->sampleFormat) && format->formatEpoch &&
    format->sampleRate && format->sampleRate <= LG_NET_AUDIO_MAX_SAMPLE_RATE &&
    format->channels && format->channels <= LG_NET_AUDIO_MAX_CHANNELS &&
    format->framesPerPacket &&
    format->framesPerPacket <= LG_NET_AUDIO_MAX_PACKET_FRAMES &&
    format->maxPacketFrames >= format->framesPerPacket &&
    format->maxPacketFrames <= LG_NET_AUDIO_MAX_PACKET_FRAMES &&
    (format->flags & LG_NET_AUDIO_FORMAT_INTERLEAVED) &&
    (!format->channelMask ||
      bitCount32(format->channelMask) == format->channels) &&
    !(format->flags & ~AUDIO_FORMAT_FLAGS);
}

bool lgNetAudioFormatEncode(
    void * data, size_t size, const LGNetAudioFormat * format)
{
  if (!data || size < LG_NET_AUDIO_FORMAT_WIRE_SIZE ||
      !lgNetAudioFormatValid(format))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU32(&writer, format->streamID)        &&
    lgNetWriterU16(&writer, format->direction)       &&
    lgNetWriterU16(&writer, format->sampleFormat)    &&
    lgNetWriterU64(&writer, format->formatEpoch)     &&
    lgNetWriterU32(&writer, format->sampleRate)      &&
    lgNetWriterU16(&writer, format->channels)        &&
    lgNetWriterU16(&writer, format->framesPerPacket) &&
    lgNetWriterU32(&writer, format->channelMask)     &&
    lgNetWriterU32(&writer, format->flags)           &&
    lgNetWriterU32(&writer, format->maxPacketFrames) &&
    lgNetWriterU32(&writer, 0)                       &&
    lgNetWriterSize(&writer) == LG_NET_AUDIO_FORMAT_WIRE_SIZE;
}

LGNetParseResult lgNetAudioFormatDecode(
    LGNetAudioFormat * format, const void * data, size_t size)
{
  if (!format || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_AUDIO_FORMAT_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetAudioFormat decoded;
  LGNetReader      reader;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU32(&reader, &decoded.streamID)        ||
      !lgNetReaderU16(&reader, &decoded.direction)       ||
      !lgNetReaderU16(&reader, &decoded.sampleFormat)    ||
      !lgNetReaderU64(&reader, &decoded.formatEpoch)     ||
      !lgNetReaderU32(&reader, &decoded.sampleRate)      ||
      !lgNetReaderU16(&reader, &decoded.channels)        ||
      !lgNetReaderU16(&reader, &decoded.framesPerPacket) ||
      !lgNetReaderU32(&reader, &decoded.channelMask)     ||
      !lgNetReaderU32(&reader, &decoded.flags)           ||
      !lgNetReaderU32(&reader, &decoded.maxPacketFrames) ||
      !lgNetReaderZero(&reader, 4))
    return LG_NET_PARSE_INVALID_VALUE;

  const LGNetParseResult result = fixedDecodeResult(
    &reader, LG_NET_AUDIO_FORMAT_WIRE_SIZE, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetAudioFormatValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *format = decoded;
  return LG_NET_PARSE_OK;
}

size_t lgNetAudioDataSize(const LGNetAudioData * audio)
{
  size_t size;
  return audio && variableSize(LG_NET_AUDIO_DATA_HEADER_WIRE_SIZE,
    audio->dataLength, LG_NET_AUDIO_MAX_DATA_LENGTH, &size) ? size : 0;
}

bool lgNetAudioDataValid(const LGNetAudioData * audio)
{
  return audio && audio->streamID &&
    audioDirectionKnown(audio->direction) &&
    !(audio->flags & ~AUDIO_DATA_FLAGS) && audio->formatEpoch &&
    audio->packetID && audio->frameCount &&
    audio->frameCount <= LG_NET_AUDIO_MAX_PACKET_FRAMES &&
    audio->startFrame <= UINT64_MAX - (audio->frameCount - 1U) &&
    (!(audio->flags & LG_NET_AUDIO_DATA_CLOCK_STABLE) ||
      (audio->flags & LG_NET_AUDIO_DATA_CLOCK_VALID)) &&
    ((audio->flags & LG_NET_AUDIO_DATA_CLOCK_VALID) ?
      audio->timestampNs && audio->timestampNs <= (uint64_t)INT64_MAX &&
        audio->rateQ32 && audio->rateQ32 <= LG_NET_AUDIO_MAX_RATE_Q32 :
      !audio->timestampNs && !audio->rateQ32) &&
    ((audio->flags & LG_NET_AUDIO_DATA_SILENT) ?
      !audio->dataLength && !audio->data : audio->dataLength && audio->data) &&
    lgNetAudioDataSize(audio) != 0;
}

bool lgNetAudioDataMatchesFormat(
    const LGNetAudioData * audio, const LGNetAudioFormat * format)
{
  if (!lgNetAudioDataValid(audio) || !lgNetAudioFormatValid(format) ||
      audio->streamID != format->streamID ||
      audio->direction != format->direction ||
      audio->formatEpoch != format->formatEpoch ||
      audio->frameCount > format->maxPacketFrames)
    return false;

  if (audio->flags & LG_NET_AUDIO_DATA_SILENT)
    return true;

  const size_t sampleSize = audioSampleSize(format->sampleFormat);
  if (!sampleSize || format->channels > SIZE_MAX / sampleSize)
    return false;

  const size_t frameSize = sampleSize * format->channels;
  return audio->frameCount <= SIZE_MAX / frameSize &&
    audio->dataLength == (size_t)audio->frameCount * frameSize;
}

bool lgNetAudioDataEncode(
    void * data, size_t size, const LGNetAudioData * audio)
{
  const size_t wireSize = lgNetAudioDataSize(audio);
  if (!data || !wireSize || size < wireSize || !lgNetAudioDataValid(audio))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU32(&writer, audio->streamID)       &&
    lgNetWriterU16(&writer, audio->direction)      &&
    lgNetWriterU16(&writer, audio->flags)          &&
    lgNetWriterU64(&writer, audio->formatEpoch)    &&
    lgNetWriterU64(&writer, audio->packetID)       &&
    lgNetWriterU64(&writer, audio->timestampNs)    &&
    lgNetWriterU64(&writer, audio->startFrame)     &&
    lgNetWriterU64(&writer, audio->rateQ32)        &&
    lgNetWriterU32(&writer, audio->frameCount)     &&
    lgNetWriterU32(&writer, audio->dataLength)     &&
    lgNetWriterBytes(&writer, audio->data, audio->dataLength) &&
    lgNetWriterSize(&writer) == wireSize;
}

LGNetParseResult lgNetAudioDataDecode(
    LGNetAudioData * audio, const void * data, size_t size)
{
  if (!audio || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_AUDIO_DATA_HEADER_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetAudioData decoded;
  LGNetReader    reader;
  size_t         expected;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU32(&reader, &decoded.streamID)    ||
      !lgNetReaderU16(&reader, &decoded.direction)   ||
      !lgNetReaderU16(&reader, &decoded.flags)       ||
      !lgNetReaderU64(&reader, &decoded.formatEpoch) ||
      !lgNetReaderU64(&reader, &decoded.packetID)    ||
      !lgNetReaderU64(&reader, &decoded.timestampNs) ||
      !lgNetReaderU64(&reader, &decoded.startFrame)  ||
      !lgNetReaderU64(&reader, &decoded.rateQ32)     ||
      !lgNetReaderU32(&reader, &decoded.frameCount)  ||
      !lgNetReaderU32(&reader, &decoded.dataLength))
    return LG_NET_PARSE_INVALID_VALUE;

  LGNetParseResult result = variableDecodeSize(
    LG_NET_AUDIO_DATA_HEADER_WIRE_SIZE, decoded.dataLength,
    LG_NET_AUDIO_MAX_DATA_LENGTH, size, &expected);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetReaderView(&reader, &decoded.data, decoded.dataLength))
    return LG_NET_PARSE_INVALID_VALUE;
  result = fixedDecodeResult(&reader, expected, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetAudioDataValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *audio = decoded;
  return LG_NET_PARSE_OK;
}

static bool audioStateKnown(LGNetAudioStateCode state)
{
  return state >= LG_NET_AUDIO_STATE_STOPPED &&
    state <= LG_NET_AUDIO_STATE_ERROR;
}

bool lgNetAudioStateValid(const LGNetAudioState * state)
{
  return state && state->streamID &&
    audioDirectionKnown(state->direction) && audioStateKnown(state->state) &&
    state->stateSequence && !(state->flags & ~AUDIO_STATE_FLAGS) &&
    state->volumeMillibels >= LG_NET_AUDIO_VOLUME_MIN_MILLIBELS &&
    state->volumeMillibels <= LG_NET_AUDIO_VOLUME_MAX_MILLIBELS &&
    (!(state->flags & LG_NET_AUDIO_STATE_FORMAT_ACTIVE) || state->formatEpoch);
}

bool lgNetAudioStateEncode(
    void * data, size_t size, const LGNetAudioState * state)
{
  if (!data || size < LG_NET_AUDIO_STATE_WIRE_SIZE ||
      !lgNetAudioStateValid(state))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU32(&writer, state->streamID)         &&
    lgNetWriterU16(&writer, state->direction)        &&
    lgNetWriterU16(&writer, state->state)            &&
    lgNetWriterU64(&writer, state->stateSequence)    &&
    lgNetWriterU64(&writer, state->formatEpoch)      &&
    lgNetWriterI32(&writer, state->volumeMillibels)  &&
    lgNetWriterU32(&writer, state->flags)            &&
    lgNetWriterSize(&writer) == LG_NET_AUDIO_STATE_WIRE_SIZE;
}

LGNetParseResult lgNetAudioStateDecode(
    LGNetAudioState * state, const void * data, size_t size)
{
  if (!state || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_AUDIO_STATE_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetAudioState decoded;
  LGNetReader     reader;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU32(&reader, &decoded.streamID)        ||
      !lgNetReaderU16(&reader, &decoded.direction)       ||
      !lgNetReaderU16(&reader, &decoded.state)           ||
      !lgNetReaderU64(&reader, &decoded.stateSequence)   ||
      !lgNetReaderU64(&reader, &decoded.formatEpoch)     ||
      !lgNetReaderI32(&reader, &decoded.volumeMillibels) ||
      !lgNetReaderU32(&reader, &decoded.flags))
    return LG_NET_PARSE_INVALID_VALUE;

  const LGNetParseResult result = fixedDecodeResult(
    &reader, LG_NET_AUDIO_STATE_WIRE_SIZE, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetAudioStateValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *state = decoded;
  return LG_NET_PARSE_OK;
}

bool lgNetAudioClockFeedbackValid(
    const LGNetAudioClockFeedback * feedback)
{
  return feedback && feedback->streamID &&
    audioDirectionKnown(feedback->direction) && feedback->formatEpoch &&
    feedback->packetID && feedback->timestampNs;
}

bool lgNetAudioClockFeedbackEncode(void * data, size_t size,
    const LGNetAudioClockFeedback * feedback)
{
  if (!data || size < LG_NET_AUDIO_CLOCK_FEEDBACK_WIRE_SIZE ||
      !lgNetAudioClockFeedbackValid(feedback))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU32(&writer, feedback->streamID)      &&
    lgNetWriterU16(&writer, feedback->direction)     &&
    lgNetWriterZero(&writer, 2)                      &&
    lgNetWriterU64(&writer, feedback->formatEpoch)   &&
    lgNetWriterU64(&writer, feedback->packetID)      &&
    lgNetWriterU64(&writer, feedback->framePosition) &&
    lgNetWriterU64(&writer, feedback->timestampNs)   &&
    lgNetWriterI32(&writer, feedback->queuedFrames)  &&
    lgNetWriterI32(&writer, feedback->driftPpm)      &&
    lgNetWriterSize(&writer) == LG_NET_AUDIO_CLOCK_FEEDBACK_WIRE_SIZE;
}

LGNetParseResult lgNetAudioClockFeedbackDecode(
    LGNetAudioClockFeedback * feedback, const void * data, size_t size)
{
  if (!feedback || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_AUDIO_CLOCK_FEEDBACK_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetAudioClockFeedback decoded;
  LGNetReader             reader;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU32(&reader, &decoded.streamID)      ||
      !lgNetReaderU16(&reader, &decoded.direction)     ||
      !lgNetReaderZero(&reader, 2)                     ||
      !lgNetReaderU64(&reader, &decoded.formatEpoch)   ||
      !lgNetReaderU64(&reader, &decoded.packetID)      ||
      !lgNetReaderU64(&reader, &decoded.framePosition) ||
      !lgNetReaderU64(&reader, &decoded.timestampNs)   ||
      !lgNetReaderI32(&reader, &decoded.queuedFrames)  ||
      !lgNetReaderI32(&reader, &decoded.driftPpm))
    return LG_NET_PARSE_INVALID_VALUE;

  const LGNetParseResult result = fixedDecodeResult(
    &reader, LG_NET_AUDIO_CLOCK_FEEDBACK_WIRE_SIZE, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetAudioClockFeedbackValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *feedback = decoded;
  return LG_NET_PARSE_OK;
}

bool lgNetAudioClockStateValid(const LGNetAudioClockState * state)
{
  if (!state || !state->streamID ||
      !audioDirectionKnown(state->direction) ||
      (state->flags & ~AUDIO_CLOCK_FLAGS) || !state->formatEpoch ||
      !state->subscriptionEpoch || !state->packetID ||
      state->rateQ32 > LG_NET_AUDIO_MAX_RATE_Q32 ||
      state->targetRateQ32 > LG_NET_AUDIO_MAX_RATE_Q32 ||
      ((state->flags & LG_NET_AUDIO_CLOCK_STABLE) &&
        !(state->flags & LG_NET_AUDIO_CLOCK_VALID)))
    return false;

  return (state->flags & LG_NET_AUDIO_CLOCK_VALID) || !state->timeNs;
}

bool lgNetAudioClockStateEncode(
    void * data, size_t size, const LGNetAudioClockState * state)
{
  if (!data || size < LG_NET_AUDIO_CLOCK_STATE_WIRE_SIZE ||
      !lgNetAudioClockStateValid(state))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU32(&writer, state->streamID)          &&
    lgNetWriterU16(&writer, state->direction)         &&
    lgNetWriterU16(&writer, state->flags)             &&
    lgNetWriterU64(&writer, state->formatEpoch)       &&
    lgNetWriterU64(&writer, state->subscriptionEpoch) &&
    lgNetWriterU64(&writer, state->packetID)          &&
    lgNetWriterU64(&writer, state->framePosition)     &&
    lgNetWriterI64(&writer, state->timeNs)            &&
    lgNetWriterU64(&writer, state->rateQ32)           &&
    lgNetWriterU64(&writer, state->targetRateQ32)     &&
    lgNetWriterI32(&writer, state->queuedFrames)      &&
    lgNetWriterI32(&writer, state->driftPpm)          &&
    lgNetWriterSize(&writer) == LG_NET_AUDIO_CLOCK_STATE_WIRE_SIZE;
}

LGNetParseResult lgNetAudioClockStateDecode(
    LGNetAudioClockState * state, const void * data, size_t size)
{
  if (!state || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_AUDIO_CLOCK_STATE_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetAudioClockState decoded;
  LGNetReader          reader;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU32(&reader, &decoded.streamID)          ||
      !lgNetReaderU16(&reader, &decoded.direction)         ||
      !lgNetReaderU16(&reader, &decoded.flags)             ||
      !lgNetReaderU64(&reader, &decoded.formatEpoch)       ||
      !lgNetReaderU64(&reader, &decoded.subscriptionEpoch) ||
      !lgNetReaderU64(&reader, &decoded.packetID)          ||
      !lgNetReaderU64(&reader, &decoded.framePosition)     ||
      !lgNetReaderI64(&reader, &decoded.timeNs)            ||
      !lgNetReaderU64(&reader, &decoded.rateQ32)           ||
      !lgNetReaderU64(&reader, &decoded.targetRateQ32)     ||
      !lgNetReaderI32(&reader, &decoded.queuedFrames)      ||
      !lgNetReaderI32(&reader, &decoded.driftPpm))
    return LG_NET_PARSE_INVALID_VALUE;

  const LGNetParseResult result = fixedDecodeResult(
    &reader, LG_NET_AUDIO_CLOCK_STATE_WIRE_SIZE, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetAudioClockStateValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *state = decoded;
  return LG_NET_PARSE_OK;
}

bool lgNetAudioControlValid(const LGNetAudioControl * control)
{
  return control && control->streamID &&
    audioDirectionKnown(control->direction) &&
    !(control->flags & ~AUDIO_CONTROL_FLAGS) && control->sequence;
}

bool lgNetAudioControlEncode(
    void * data, size_t size, const LGNetAudioControl * control)
{
  if (!data || size < LG_NET_AUDIO_CONTROL_WIRE_SIZE ||
      !lgNetAudioControlValid(control))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU32 (&writer, control->streamID)    &&
    lgNetWriterU16 (&writer, control->direction)   &&
    lgNetWriterU16 (&writer, control->flags)       &&
    lgNetWriterU64 (&writer, control->sequence)    &&
    lgNetWriterU64 (&writer, control->formatEpoch) &&
    lgNetWriterU32 (&writer, control->detail)      &&
    lgNetWriterZero(&writer, 4)                    &&
    lgNetWriterSize(&writer) == LG_NET_AUDIO_CONTROL_WIRE_SIZE;
}

LGNetParseResult lgNetAudioControlDecode(
    LGNetAudioControl * control, const void * data, size_t size)
{
  if (!control || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_AUDIO_CONTROL_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetAudioControl decoded;
  LGNetReader       reader;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU32 (&reader, &decoded.streamID)    ||
      !lgNetReaderU16 (&reader, &decoded.direction)   ||
      !lgNetReaderU16 (&reader, &decoded.flags)       ||
      !lgNetReaderU64 (&reader, &decoded.sequence)    ||
      !lgNetReaderU64 (&reader, &decoded.formatEpoch) ||
      !lgNetReaderU32 (&reader, &decoded.detail)      ||
      !lgNetReaderZero(&reader, 4))
    return LG_NET_PARSE_INVALID_VALUE;

  const LGNetParseResult result = fixedDecodeResult(
    &reader, LG_NET_AUDIO_CONTROL_WIRE_SIZE, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetAudioControlValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *control = decoded;
  return LG_NET_PARSE_OK;
}

size_t lgNetAudioVolumeSize(const LGNetAudioVolume * volume)
{
  size_t size;
  return volume && variableSize(LG_NET_AUDIO_VOLUME_HEADER_WIRE_SIZE,
    (size_t)volume->channelCount * sizeof(volume->volumeMillibels[0]),
    sizeof(volume->volumeMillibels), &size) ? size : 0;
}

bool lgNetAudioVolumeValid(const LGNetAudioVolume * volume)
{
  if (!volume || !volume->streamID ||
      !audioDirectionKnown(volume->direction) || !volume->channelCount ||
      volume->channelCount > LG_NET_AUDIO_MAX_CHANNELS || !volume->sequence ||
      (volume->channelMask &&
        bitCount32(volume->channelMask) != volume->channelCount))
    return false;

  for (uint16_t i = 0; i < volume->channelCount; ++i)
  {
    if (volume->volumeMillibels[i] < LG_NET_AUDIO_VOLUME_MIN_MILLIBELS ||
        volume->volumeMillibels[i] > LG_NET_AUDIO_VOLUME_MAX_MILLIBELS)
      return false;
  }

  return true;
}

bool lgNetAudioVolumeEncode(
    void * data, size_t size, const LGNetAudioVolume * volume)
{
  const size_t needed = lgNetAudioVolumeSize(volume);
  if (!data || !needed || size < needed || !lgNetAudioVolumeValid(volume))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  if (!lgNetWriterU32 (&writer, volume->streamID)     ||
      !lgNetWriterU16 (&writer, volume->direction)    ||
      !lgNetWriterU16 (&writer, volume->channelCount) ||
      !lgNetWriterU64 (&writer, volume->sequence)     ||
      !lgNetWriterU32 (&writer, volume->channelMask)  ||
      !lgNetWriterZero(&writer, 4))
    return false;

  for (uint16_t i = 0; i < volume->channelCount; ++i)
  {
    if (!lgNetWriterI32(&writer, volume->volumeMillibels[i]))
      return false;
  }

  return lgNetWriterSize(&writer) == needed;
}

LGNetParseResult lgNetAudioVolumeDecode(
    LGNetAudioVolume * volume, const void * data, size_t size)
{
  if (!volume || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_AUDIO_VOLUME_HEADER_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetAudioVolume decoded;
  LGNetReader      reader;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU32 (&reader, &decoded.streamID)     ||
      !lgNetReaderU16 (&reader, &decoded.direction)    ||
      !lgNetReaderU16 (&reader, &decoded.channelCount) ||
      !lgNetReaderU64 (&reader, &decoded.sequence)     ||
      !lgNetReaderU32 (&reader, &decoded.channelMask)  ||
      !lgNetReaderZero(&reader, 4))
    return LG_NET_PARSE_INVALID_VALUE;

  size_t                 expected;
  const LGNetParseResult result = variableDecodeSize(
    LG_NET_AUDIO_VOLUME_HEADER_WIRE_SIZE,
    (size_t)decoded.channelCount * sizeof(decoded.volumeMillibels[0]),
    sizeof(decoded.volumeMillibels), size, &expected);
  if (result != LG_NET_PARSE_OK)
    return result;
  for (uint16_t i = 0; i < decoded.channelCount; ++i)
  {
    if (!lgNetReaderI32(&reader, &decoded.volumeMillibels[i]))
      return LG_NET_PARSE_INVALID_VALUE;
  }
  if (lgNetReaderConsumed(&reader) != expected ||
      !lgNetAudioVolumeValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *volume = decoded;
  return LG_NET_PARSE_OK;
}

bool lgNetAudioMuteValid(const LGNetAudioMute * mute)
{
  return mute && mute->streamID && audioDirectionKnown(mute->direction) &&
    mute->muted <= 1 && mute->sequence;
}

bool lgNetAudioMuteEncode(
    void * data, size_t size, const LGNetAudioMute * mute)
{
  if (!data || size < LG_NET_AUDIO_MUTE_WIRE_SIZE ||
      !lgNetAudioMuteValid(mute))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU32 (&writer, mute->streamID)    &&
    lgNetWriterU16 (&writer, mute->direction)   &&
    lgNetWriterU8  (&writer, mute->muted)       &&
    lgNetWriterZero(&writer, 1)                 &&
    lgNetWriterU64 (&writer, mute->sequence)    &&
    lgNetWriterU32 (&writer, mute->channelMask) &&
    lgNetWriterZero(&writer, 4)                 &&
    lgNetWriterSize(&writer) == LG_NET_AUDIO_MUTE_WIRE_SIZE;
}

LGNetParseResult lgNetAudioMuteDecode(
    LGNetAudioMute * mute, const void * data, size_t size)
{
  if (!mute || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_AUDIO_MUTE_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetAudioMute decoded;
  LGNetReader    reader;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU32 (&reader, &decoded.streamID)    ||
      !lgNetReaderU16 (&reader, &decoded.direction)   ||
      !lgNetReaderU8  (&reader, &decoded.muted)       ||
      !lgNetReaderZero(&reader, 1)                    ||
      !lgNetReaderU64 (&reader, &decoded.sequence)    ||
      !lgNetReaderU32 (&reader, &decoded.channelMask) ||
      !lgNetReaderZero(&reader, 4))
    return LG_NET_PARSE_INVALID_VALUE;

  const LGNetParseResult result = fixedDecodeResult(
    &reader, LG_NET_AUDIO_MUTE_WIRE_SIZE, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetAudioMuteValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *mute = decoded;
  return LG_NET_PARSE_OK;
}

bool lgNetAudioBarrierValid(const LGNetAudioBarrier * barrier)
{
  return barrier && barrier->streamID &&
    audioDirectionKnown(barrier->direction) && barrier->barrierID &&
    barrier->stateSequence;
}

bool lgNetAudioBarrierEncode(
    void * data, size_t size, const LGNetAudioBarrier * barrier)
{
  if (!data || size < LG_NET_AUDIO_BARRIER_WIRE_SIZE ||
      !lgNetAudioBarrierValid(barrier))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU32 (&writer, barrier->streamID)      &&
    lgNetWriterU16 (&writer, barrier->direction)     &&
    lgNetWriterZero(&writer, 2)                      &&
    lgNetWriterU64 (&writer, barrier->barrierID)     &&
    lgNetWriterU64 (&writer, barrier->stateSequence) &&
    lgNetWriterSize(&writer) == LG_NET_AUDIO_BARRIER_WIRE_SIZE;
}

LGNetParseResult lgNetAudioBarrierDecode(
    LGNetAudioBarrier * barrier, const void * data, size_t size)
{
  if (!barrier || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_AUDIO_BARRIER_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetAudioBarrier decoded;
  LGNetReader       reader;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU32 (&reader, &decoded.streamID)      ||
      !lgNetReaderU16 (&reader, &decoded.direction)     ||
      !lgNetReaderZero(&reader, 2)                      ||
      !lgNetReaderU64 (&reader, &decoded.barrierID)     ||
      !lgNetReaderU64 (&reader, &decoded.stateSequence))
    return LG_NET_PARSE_INVALID_VALUE;

  const LGNetParseResult result = fixedDecodeResult(
    &reader, LG_NET_AUDIO_BARRIER_WIRE_SIZE, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetAudioBarrierValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *barrier = decoded;
  return LG_NET_PARSE_OK;
}

bool lgNetClipboardClaimValid(const LGNetClipboardClaim * claim)
{
  return claim && claim->ownerID && claim->claimEpoch && claim->leaseMs &&
    claim->leaseMs <= LG_NET_CLIPBOARD_MAX_LEASE_MS &&
    claim->flags && !(claim->flags & ~CLIPBOARD_CLAIM_FLAGS);
}

bool lgNetClipboardClaimEncode(
    void * data, size_t size, const LGNetClipboardClaim * claim)
{
  if (!data || size < LG_NET_CLIPBOARD_CLAIM_WIRE_SIZE ||
      !lgNetClipboardClaimValid(claim))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU64(&writer, claim->ownerID)    &&
    lgNetWriterU64(&writer, claim->claimEpoch) &&
    lgNetWriterU32(&writer, claim->leaseMs)    &&
    lgNetWriterU32(&writer, claim->flags)      &&
    lgNetWriterSize(&writer) == LG_NET_CLIPBOARD_CLAIM_WIRE_SIZE;
}

LGNetParseResult lgNetClipboardClaimDecode(
    LGNetClipboardClaim * claim, const void * data, size_t size)
{
  if (!claim || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_CLIPBOARD_CLAIM_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetClipboardClaim decoded;
  LGNetReader         reader;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU64(&reader, &decoded.ownerID)    ||
      !lgNetReaderU64(&reader, &decoded.claimEpoch) ||
      !lgNetReaderU32(&reader, &decoded.leaseMs)    ||
      !lgNetReaderU32(&reader, &decoded.flags))
    return LG_NET_PARSE_INVALID_VALUE;

  const LGNetParseResult result = fixedDecodeResult(
    &reader, LG_NET_CLIPBOARD_CLAIM_WIRE_SIZE, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetClipboardClaimValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *claim = decoded;
  return LG_NET_PARSE_OK;
}

bool lgNetClipboardControlValid(const LGNetClipboardControl * control)
{
  return control && control->ownerID && control->claimEpoch && control->serial;
}

bool lgNetClipboardControlEncode(
    void * data, size_t size, const LGNetClipboardControl * control)
{
  if (!data || size < LG_NET_CLIPBOARD_CONTROL_WIRE_SIZE ||
      !lgNetClipboardControlValid(control))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU64(&writer, control->ownerID)    &&
    lgNetWriterU64(&writer, control->claimEpoch) &&
    lgNetWriterU64(&writer, control->serial)     &&
    lgNetWriterU64(&writer, control->offerID)    &&
    lgNetWriterSize(&writer) == LG_NET_CLIPBOARD_CONTROL_WIRE_SIZE;
}

LGNetParseResult lgNetClipboardControlDecode(
    LGNetClipboardControl * control, const void * data, size_t size)
{
  if (!control || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_CLIPBOARD_CONTROL_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetClipboardControl decoded;
  LGNetReader           reader;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU64(&reader, &decoded.ownerID)    ||
      !lgNetReaderU64(&reader, &decoded.claimEpoch) ||
      !lgNetReaderU64(&reader, &decoded.serial)     ||
      !lgNetReaderU64(&reader, &decoded.offerID))
    return LG_NET_PARSE_INVALID_VALUE;

  const LGNetParseResult result = fixedDecodeResult(
    &reader, LG_NET_CLIPBOARD_CONTROL_WIRE_SIZE, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetClipboardControlValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *control = decoded;
  return LG_NET_PARSE_OK;
}

static bool clipboardClaimCodeKnown(LGNetClipboardClaimCode status)
{
  return status >= LG_NET_CLIPBOARD_CLAIM_ACCEPTED &&
    status <= LG_NET_CLIPBOARD_CLAIM_ERROR;
}

bool lgNetClipboardClaimStatusValid(
    const LGNetClipboardClaimStatus * status)
{
  return status && status->claimEpoch && status->serial &&
    clipboardClaimCodeKnown(status->status) &&
    !(status->flags & ~CLIPBOARD_CLAIM_FLAGS) &&
    status->leaseRemainingMs <= LG_NET_CLIPBOARD_MAX_LEASE_MS;
}

bool lgNetClipboardClaimStatusEncode(
    void * data, size_t size, const LGNetClipboardClaimStatus * status)
{
  if (!data || size < LG_NET_CLIPBOARD_CLAIM_STATUS_WIRE_SIZE ||
      !lgNetClipboardClaimStatusValid(status))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU64(&writer, status->claimEpoch)       &&
    lgNetWriterU64(&writer, status->serial)           &&
    lgNetWriterU32(&writer, status->status)           &&
    lgNetWriterU32(&writer, status->flags)            &&
    lgNetWriterU32(&writer, status->leaseRemainingMs) &&
    lgNetWriterU32(&writer, status->detail)           &&
    lgNetWriterSize(&writer) == LG_NET_CLIPBOARD_CLAIM_STATUS_WIRE_SIZE;
}

LGNetParseResult lgNetClipboardClaimStatusDecode(
    LGNetClipboardClaimStatus * status, const void * data, size_t size)
{
  if (!status || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_CLIPBOARD_CLAIM_STATUS_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetClipboardClaimStatus decoded;
  LGNetReader              reader;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU64(&reader, &decoded.claimEpoch)       ||
      !lgNetReaderU64(&reader, &decoded.serial)           ||
      !lgNetReaderU32(&reader, &decoded.status)           ||
      !lgNetReaderU32(&reader, &decoded.flags)            ||
      !lgNetReaderU32(&reader, &decoded.leaseRemainingMs) ||
      !lgNetReaderU32(&reader, &decoded.detail))
    return LG_NET_PARSE_INVALID_VALUE;

  const LGNetParseResult result = fixedDecodeResult(
    &reader, LG_NET_CLIPBOARD_CLAIM_STATUS_WIRE_SIZE, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetClipboardClaimStatusValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *status = decoded;
  return LG_NET_PARSE_OK;
}

size_t lgNetClipboardOfferSize(const LGNetClipboardOffer * offer)
{
  size_t size;
  return offer && variableSize(LG_NET_CLIPBOARD_OFFER_HEADER_WIRE_SIZE,
    offer->mimeLength, LG_NET_CLIPBOARD_MAX_MIME_LENGTH, &size) ? size : 0;
}

bool lgNetClipboardOfferValid(const LGNetClipboardOffer * offer)
{
  return offer && offer->offerID && offer->serial && offer->formatCount &&
    offer->formatCount <= LG_NET_CLIPBOARD_MAX_FORMATS &&
    offer->formatIndex < offer->formatCount && offer->mimeLength &&
    offer->mime && !(offer->flags & ~CLIPBOARD_OFFER_FLAGS) &&
    lgNetClipboardOfferSize(offer) != 0;
}

bool lgNetClipboardOfferEncode(
    void * data, size_t size, const LGNetClipboardOffer * offer)
{
  const size_t wireSize = lgNetClipboardOfferSize(offer);
  if (!data || !wireSize || size < wireSize ||
      !lgNetClipboardOfferValid(offer))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU64(&writer, offer->offerID)        &&
    lgNetWriterU64(&writer, offer->serial)         &&
    lgNetWriterU64(&writer, offer->estimatedSize)  &&
    lgNetWriterU32(&writer, offer->formatIndex)    &&
    lgNetWriterU32(&writer, offer->formatCount)    &&
    lgNetWriterU16(&writer, offer->mimeLength)     &&
    lgNetWriterU16(&writer, offer->flags)          &&
    lgNetWriterU32(&writer, 0)                     &&
    lgNetWriterBytes(&writer, offer->mime, offer->mimeLength) &&
    lgNetWriterSize(&writer) == wireSize;
}

LGNetParseResult lgNetClipboardOfferDecode(
    LGNetClipboardOffer * offer, const void * data, size_t size)
{
  if (!offer || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_CLIPBOARD_OFFER_HEADER_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetClipboardOffer decoded;
  LGNetReader         reader;
  size_t              expected;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU64(&reader, &decoded.offerID)       ||
      !lgNetReaderU64(&reader, &decoded.serial)        ||
      !lgNetReaderU64(&reader, &decoded.estimatedSize) ||
      !lgNetReaderU32(&reader, &decoded.formatIndex)   ||
      !lgNetReaderU32(&reader, &decoded.formatCount)   ||
      !lgNetReaderU16(&reader, &decoded.mimeLength)    ||
      !lgNetReaderU16(&reader, &decoded.flags)         ||
      !lgNetReaderZero(&reader, 4))
    return LG_NET_PARSE_INVALID_VALUE;

  LGNetParseResult result = variableDecodeSize(
    LG_NET_CLIPBOARD_OFFER_HEADER_WIRE_SIZE, decoded.mimeLength,
    LG_NET_CLIPBOARD_MAX_MIME_LENGTH, size, &expected);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetReaderView(&reader, &decoded.mime, decoded.mimeLength))
    return LG_NET_PARSE_INVALID_VALUE;
  result = fixedDecodeResult(&reader, expected, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetClipboardOfferValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *offer = decoded;
  return LG_NET_PARSE_OK;
}

bool lgNetClipboardRequestValid(const LGNetClipboardRequest * request)
{
  return request && request->offerID && request->requestID &&
    request->formatIndex < LG_NET_CLIPBOARD_MAX_FORMATS &&
    request->maxChunkLength &&
    request->maxChunkLength <= LG_NET_CLIPBOARD_MAX_CHUNK_LENGTH;
}

bool lgNetClipboardRequestEncode(
    void * data, size_t size, const LGNetClipboardRequest * request)
{
  if (!data || size < LG_NET_CLIPBOARD_REQUEST_WIRE_SIZE ||
      !lgNetClipboardRequestValid(request))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU64(&writer, request->offerID)        &&
    lgNetWriterU64(&writer, request->requestID)      &&
    lgNetWriterU32(&writer, request->formatIndex)    &&
    lgNetWriterU32(&writer, request->maxChunkLength) &&
    lgNetWriterU64(&writer, request->offset)         &&
    lgNetWriterSize(&writer) == LG_NET_CLIPBOARD_REQUEST_WIRE_SIZE;
}

LGNetParseResult lgNetClipboardRequestDecode(
    LGNetClipboardRequest * request, const void * data, size_t size)
{
  if (!request || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_CLIPBOARD_REQUEST_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetClipboardRequest decoded;
  LGNetReader           reader;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU64(&reader, &decoded.offerID)        ||
      !lgNetReaderU64(&reader, &decoded.requestID)      ||
      !lgNetReaderU32(&reader, &decoded.formatIndex)    ||
      !lgNetReaderU32(&reader, &decoded.maxChunkLength) ||
      !lgNetReaderU64(&reader, &decoded.offset))
    return LG_NET_PARSE_INVALID_VALUE;

  const LGNetParseResult result = fixedDecodeResult(
    &reader, LG_NET_CLIPBOARD_REQUEST_WIRE_SIZE, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetClipboardRequestValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *request = decoded;
  return LG_NET_PARSE_OK;
}

size_t lgNetClipboardChunkSize(const LGNetClipboardChunk * chunk)
{
  size_t size;
  return chunk && variableSize(LG_NET_CLIPBOARD_CHUNK_HEADER_WIRE_SIZE,
    chunk->dataLength, LG_NET_CLIPBOARD_MAX_CHUNK_LENGTH, &size) ? size : 0;
}

bool lgNetClipboardChunkValid(const LGNetClipboardChunk * chunk)
{
  if (!chunk || !chunk->requestID || !chunk->offerID ||
      (chunk->flags & ~CLIPBOARD_CHUNK_FLAGS) ||
      lgNetClipboardChunkSize(chunk) == 0)
    return false;

  if (chunk->offset > chunk->totalLength ||
      chunk->dataLength > chunk->totalLength - chunk->offset)
    return false;

  const bool first = chunk->offset == 0;
  const bool final = chunk->dataLength == chunk->totalLength - chunk->offset;
  return (!!(chunk->flags & LG_NET_CLIPBOARD_CHUNK_FIRST) == first) &&
    (!!(chunk->flags & LG_NET_CLIPBOARD_CHUNK_FINAL) == final) &&
    (chunk->totalLength ? chunk->dataLength && chunk->data :
      !chunk->dataLength && !chunk->data);
}

bool lgNetClipboardChunkEncode(
    void * data, size_t size, const LGNetClipboardChunk * chunk)
{
  const size_t wireSize = lgNetClipboardChunkSize(chunk);
  if (!data || !wireSize || size < wireSize ||
      !lgNetClipboardChunkValid(chunk))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU64(&writer, chunk->requestID)   &&
    lgNetWriterU64(&writer, chunk->offerID)     &&
    lgNetWriterU64(&writer, chunk->offset)      &&
    lgNetWriterU64(&writer, chunk->totalLength) &&
    lgNetWriterU32(&writer, chunk->dataLength)  &&
    lgNetWriterU16(&writer, chunk->flags)       &&
    lgNetWriterZero(&writer, 2)                 &&
    lgNetWriterBytes(&writer, chunk->data, chunk->dataLength) &&
    lgNetWriterSize(&writer) == wireSize;
}

LGNetParseResult lgNetClipboardChunkDecode(
    LGNetClipboardChunk * chunk, const void * data, size_t size)
{
  if (!chunk || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_CLIPBOARD_CHUNK_HEADER_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetClipboardChunk decoded;
  LGNetReader         reader;
  size_t              expected;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU64(&reader, &decoded.requestID)   ||
      !lgNetReaderU64(&reader, &decoded.offerID)     ||
      !lgNetReaderU64(&reader, &decoded.offset)      ||
      !lgNetReaderU64(&reader, &decoded.totalLength) ||
      !lgNetReaderU32(&reader, &decoded.dataLength)  ||
      !lgNetReaderU16(&reader, &decoded.flags)       ||
      !lgNetReaderZero(&reader, 2))
    return LG_NET_PARSE_INVALID_VALUE;

  LGNetParseResult result = variableDecodeSize(
    LG_NET_CLIPBOARD_CHUNK_HEADER_WIRE_SIZE, decoded.dataLength,
    LG_NET_CLIPBOARD_MAX_CHUNK_LENGTH, size, &expected);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetReaderView(&reader, &decoded.data, decoded.dataLength))
    return LG_NET_PARSE_INVALID_VALUE;
  result = fixedDecodeResult(&reader, expected, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetClipboardChunkValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *chunk = decoded;
  return LG_NET_PARSE_OK;
}

bool lgNetClipboardTransferValid(const LGNetClipboardTransfer * transfer)
{
  return transfer && transfer->requestID && transfer->offerID &&
    transfer->processedLength <= transfer->totalLength;
}

bool lgNetClipboardTransferEncode(
    void * data, size_t size, const LGNetClipboardTransfer * transfer)
{
  if (!data || size < LG_NET_CLIPBOARD_TRANSFER_WIRE_SIZE ||
      !lgNetClipboardTransferValid(transfer))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU64(&writer, transfer->requestID)       &&
    lgNetWriterU64(&writer, transfer->offerID)         &&
    lgNetWriterU64(&writer, transfer->totalLength)     &&
    lgNetWriterU64(&writer, transfer->processedLength) &&
    lgNetWriterSize(&writer) == LG_NET_CLIPBOARD_TRANSFER_WIRE_SIZE;
}

LGNetParseResult lgNetClipboardTransferDecode(
    LGNetClipboardTransfer * transfer, const void * data, size_t size)
{
  if (!transfer || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_CLIPBOARD_TRANSFER_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetClipboardTransfer decoded;
  LGNetReader            reader;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU64(&reader, &decoded.requestID)       ||
      !lgNetReaderU64(&reader, &decoded.offerID)         ||
      !lgNetReaderU64(&reader, &decoded.totalLength)     ||
      !lgNetReaderU64(&reader, &decoded.processedLength))
    return LG_NET_PARSE_INVALID_VALUE;

  const LGNetParseResult result = fixedDecodeResult(
    &reader, LG_NET_CLIPBOARD_TRANSFER_WIRE_SIZE, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetClipboardTransferValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *transfer = decoded;
  return LG_NET_PARSE_OK;
}

static bool clipboardStatusKnown(LGNetClipboardStatusCode status)
{
  return status >= LG_NET_CLIPBOARD_STATUS_OK &&
    status <= LG_NET_CLIPBOARD_STATUS_ERROR;
}

bool lgNetClipboardStatusValid(const LGNetClipboardStatus * status)
{
  return status && status->requestID && status->offerID &&
    clipboardStatusKnown(status->status);
}

bool lgNetClipboardStatusEncode(
    void * data, size_t size, const LGNetClipboardStatus * status)
{
  if (!data || size < LG_NET_CLIPBOARD_STATUS_WIRE_SIZE ||
      !lgNetClipboardStatusValid(status))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU64(&writer, status->requestID)       &&
    lgNetWriterU64(&writer, status->offerID)         &&
    lgNetWriterU32(&writer, status->status)          &&
    lgNetWriterU32(&writer, status->detail)          &&
    lgNetWriterU64(&writer, status->processedLength) &&
    lgNetWriterSize(&writer) == LG_NET_CLIPBOARD_STATUS_WIRE_SIZE;
}

LGNetParseResult lgNetClipboardStatusDecode(
    LGNetClipboardStatus * status, const void * data, size_t size)
{
  if (!status || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_CLIPBOARD_STATUS_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetClipboardStatus decoded;
  LGNetReader          reader;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU64(&reader, &decoded.requestID)       ||
      !lgNetReaderU64(&reader, &decoded.offerID)         ||
      !lgNetReaderU32(&reader, &decoded.status)          ||
      !lgNetReaderU32(&reader, &decoded.detail)          ||
      !lgNetReaderU64(&reader, &decoded.processedLength))
    return LG_NET_PARSE_INVALID_VALUE;

  const LGNetParseResult result = fixedDecodeResult(
    &reader, LG_NET_CLIPBOARD_STATUS_WIRE_SIZE, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetClipboardStatusValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *status = decoded;
  return LG_NET_PARSE_OK;
}

size_t lgNetFileOfferSize(const LGNetFileOffer * offer)
{
  size_t size;
  return offer && variableSize(LG_NET_FILE_OFFER_HEADER_WIRE_SIZE,
    offer->labelLength, LG_NET_FILE_MAX_LABEL_LENGTH, &size) ? size : 0;
}

bool lgNetFileOfferValid(const LGNetFileOffer * offer)
{
  const LGNetFileOfferFlags operation = offer ? offer->flags &
    (LG_NET_FILE_OFFER_COPY | LG_NET_FILE_OFFER_MOVE) : 0;
  return offer && offer->offerID && offer->entryCount &&
    offer->entryCount <= LG_NET_FILE_MAX_ENTRIES &&
    !(offer->flags & ~FILE_OFFER_FLAGS) &&
    (operation == LG_NET_FILE_OFFER_COPY ||
      operation == LG_NET_FILE_OFFER_MOVE) &&
    (!offer->labelLength || offer->label) && lgNetFileOfferSize(offer) != 0;
}

bool lgNetFileOfferEncode(
    void * data, size_t size, const LGNetFileOffer * offer)
{
  const size_t wireSize = lgNetFileOfferSize(offer);
  if (!data || !wireSize || size < wireSize || !lgNetFileOfferValid(offer))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU64(&writer, offer->offerID)       &&
    lgNetWriterU64(&writer, offer->totalBytes)    &&
    lgNetWriterU32(&writer, offer->entryCount)    &&
    lgNetWriterU32(&writer, offer->flags)         &&
    lgNetWriterU16(&writer, offer->labelLength)   &&
    lgNetWriterZero(&writer, 6)                   &&
    lgNetWriterBytes(&writer, offer->label, offer->labelLength) &&
    lgNetWriterSize(&writer) == wireSize;
}

LGNetParseResult lgNetFileOfferDecode(
    LGNetFileOffer * offer, const void * data, size_t size)
{
  if (!offer || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_FILE_OFFER_HEADER_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetFileOffer decoded;
  LGNetReader    reader;
  size_t         expected;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU64(&reader, &decoded.offerID)     ||
      !lgNetReaderU64(&reader, &decoded.totalBytes)  ||
      !lgNetReaderU32(&reader, &decoded.entryCount)  ||
      !lgNetReaderU32(&reader, &decoded.flags)       ||
      !lgNetReaderU16(&reader, &decoded.labelLength) ||
      !lgNetReaderZero(&reader, 6))
    return LG_NET_PARSE_INVALID_VALUE;

  LGNetParseResult result = variableDecodeSize(
    LG_NET_FILE_OFFER_HEADER_WIRE_SIZE, decoded.labelLength,
    LG_NET_FILE_MAX_LABEL_LENGTH, size, &expected);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetReaderView(&reader, &decoded.label, decoded.labelLength))
    return LG_NET_PARSE_INVALID_VALUE;
  result = fixedDecodeResult(&reader, expected, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetFileOfferValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *offer = decoded;
  return LG_NET_PARSE_OK;
}

bool lgNetFileLeaseValid(const LGNetFileLease * lease)
{
  if (!lease || !lease->clientID || !lease->offerID ||
      !(lease->flags & LG_NET_FILE_LEASE_READ) ||
      (lease->flags & ~FILE_LEASE_FLAGS))
    return false;

  if (lease->flags & LG_NET_FILE_LEASE_ACQUIRED)
    return lease->leaseEpoch && lease->leaseMs &&
      lease->leaseMs <= LG_NET_FILE_MAX_LEASE_MS;

  return (!lease->leaseEpoch && lease->leaseMs &&
      lease->leaseMs <= LG_NET_FILE_MAX_LEASE_MS) ||
    (lease->leaseEpoch && !lease->leaseMs);
}

bool lgNetFileLeaseEncode(
    void * data, size_t size, const LGNetFileLease * lease)
{
  if (!data || size < LG_NET_FILE_LEASE_WIRE_SIZE ||
      !lgNetFileLeaseValid(lease))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU64(&writer, lease->clientID)   &&
    lgNetWriterU64(&writer, lease->offerID)    &&
    lgNetWriterU64(&writer, lease->leaseEpoch) &&
    lgNetWriterU32(&writer, lease->leaseMs)    &&
    lgNetWriterU32(&writer, lease->flags)      &&
    lgNetWriterSize(&writer) == LG_NET_FILE_LEASE_WIRE_SIZE;
}

LGNetParseResult lgNetFileLeaseDecode(
    LGNetFileLease * lease, const void * data, size_t size)
{
  if (!lease || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_FILE_LEASE_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetFileLease decoded;
  LGNetReader    reader;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU64(&reader, &decoded.clientID)   ||
      !lgNetReaderU64(&reader, &decoded.offerID)    ||
      !lgNetReaderU64(&reader, &decoded.leaseEpoch) ||
      !lgNetReaderU32(&reader, &decoded.leaseMs)    ||
      !lgNetReaderU32(&reader, &decoded.flags))
    return LG_NET_PARSE_INVALID_VALUE;

  const LGNetParseResult result = fixedDecodeResult(
    &reader, LG_NET_FILE_LEASE_WIRE_SIZE, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetFileLeaseValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *lease = decoded;
  return LG_NET_PARSE_OK;
}

static bool fileEntryTypeKnown(LGNetFileEntryType type)
{
  return type >= LG_NET_FILE_ENTRY_REGULAR &&
    type <= LG_NET_FILE_ENTRY_SYMLINK;
}

size_t lgNetFileEntrySize(const LGNetFileEntry * entry)
{
  size_t variableLength;
  size_t size;
  if (!entry || entry->pathLength > LG_NET_FILE_MAX_PATH_LENGTH ||
      entry->linkTargetLength > LG_NET_FILE_MAX_LINK_TARGET_LENGTH)
    return 0;

  variableLength = (size_t)entry->pathLength + entry->linkTargetLength;
  return variableSize(LG_NET_FILE_ENTRY_HEADER_WIRE_SIZE, variableLength,
    LG_NET_FILE_MAX_PATH_LENGTH + LG_NET_FILE_MAX_LINK_TARGET_LENGTH,
    &size) ? size : 0;
}

bool lgNetFileEntryValid(const LGNetFileEntry * entry)
{
  return entry && entry->offerID && entry->entryID &&
    fileEntryTypeKnown(entry->type) && !(entry->flags & ~FILE_ENTRY_FLAGS) &&
    entry->pathLength && entry->pathLength <= LG_NET_FILE_MAX_PATH_LENGTH &&
    entry->path &&
    entry->linkTargetLength <= LG_NET_FILE_MAX_LINK_TARGET_LENGTH &&
    ((entry->type == LG_NET_FILE_ENTRY_SYMLINK) ?
      entry->linkTargetLength && entry->linkTarget :
      !entry->linkTargetLength && !entry->linkTarget) &&
    lgNetFileEntrySize(entry) != 0;
}

bool lgNetFileEntryEncode(
    void * data, size_t size, const LGNetFileEntry * entry)
{
  const size_t wireSize = lgNetFileEntrySize(entry);
  if (!data || !wireSize || size < wireSize || !lgNetFileEntryValid(entry))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU64(&writer, entry->offerID)          &&
    lgNetWriterU64(&writer, entry->entryID)          &&
    lgNetWriterU64(&writer, entry->parentID)         &&
    lgNetWriterU64(&writer, entry->size)             &&
    lgNetWriterU64(&writer, entry->modifiedNs)       &&
    lgNetWriterU32(&writer, entry->mode)             &&
    lgNetWriterU16(&writer, entry->type)             &&
    lgNetWriterU16(&writer, entry->flags)            &&
    lgNetWriterU16(&writer, entry->pathLength)       &&
    lgNetWriterU16(&writer, entry->linkTargetLength) &&
    lgNetWriterZero(&writer, 4)                      &&
    lgNetWriterBytes(&writer, entry->path, entry->pathLength) &&
    lgNetWriterBytes(&writer, entry->linkTarget,
      entry->linkTargetLength)                       &&
    lgNetWriterSize(&writer) == wireSize;
}

LGNetParseResult lgNetFileEntryDecode(
    LGNetFileEntry * entry, const void * data, size_t size)
{
  if (!entry || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_FILE_ENTRY_HEADER_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetFileEntry decoded;
  LGNetReader    reader;
  size_t         expected;
  size_t         variableLength;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU64(&reader, &decoded.offerID)          ||
      !lgNetReaderU64(&reader, &decoded.entryID)          ||
      !lgNetReaderU64(&reader, &decoded.parentID)         ||
      !lgNetReaderU64(&reader, &decoded.size)             ||
      !lgNetReaderU64(&reader, &decoded.modifiedNs)       ||
      !lgNetReaderU32(&reader, &decoded.mode)             ||
      !lgNetReaderU16(&reader, &decoded.type)             ||
      !lgNetReaderU16(&reader, &decoded.flags)            ||
      !lgNetReaderU16(&reader, &decoded.pathLength)       ||
      !lgNetReaderU16(&reader, &decoded.linkTargetLength) ||
      !lgNetReaderZero(&reader, 4))
    return LG_NET_PARSE_INVALID_VALUE;

  if (decoded.pathLength > LG_NET_FILE_MAX_PATH_LENGTH ||
      decoded.linkTargetLength > LG_NET_FILE_MAX_LINK_TARGET_LENGTH)
    return LG_NET_PARSE_INVALID_LENGTH;
  variableLength = (size_t)decoded.pathLength + decoded.linkTargetLength;
  LGNetParseResult result = variableDecodeSize(
    LG_NET_FILE_ENTRY_HEADER_WIRE_SIZE, variableLength,
    LG_NET_FILE_MAX_PATH_LENGTH + LG_NET_FILE_MAX_LINK_TARGET_LENGTH,
    size, &expected);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetReaderView(&reader, &decoded.path, decoded.pathLength) ||
      !lgNetReaderView(&reader, &decoded.linkTarget,
        decoded.linkTargetLength))
    return LG_NET_PARSE_INVALID_VALUE;
  result = fixedDecodeResult(&reader, expected, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetFileEntryValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *entry = decoded;
  return LG_NET_PARSE_OK;
}

bool lgNetFileRequestValid(const LGNetFileRequest * request)
{
  return request && request->requestID && request->offerID &&
    request->entryID && request->flags &&
    !(request->flags & ~FILE_REQUEST_FLAGS) &&
    ((request->flags & LG_NET_FILE_REQUEST_DATA) ?
      request->length &&
        request->length <= LG_NET_FILE_MAX_CHUNK_LENGTH &&
        request->offset <= UINT64_MAX - request->length :
      !request->offset && !request->length);
}

bool lgNetFileRequestEncode(
    void * data, size_t size, const LGNetFileRequest * request)
{
  if (!data || size < LG_NET_FILE_REQUEST_WIRE_SIZE ||
      !lgNetFileRequestValid(request))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU64(&writer, request->requestID) &&
    lgNetWriterU64(&writer, request->offerID)   &&
    lgNetWriterU64(&writer, request->entryID)   &&
    lgNetWriterU64(&writer, request->offset)    &&
    lgNetWriterU32(&writer, request->length)    &&
    lgNetWriterU32(&writer, request->flags)     &&
    lgNetWriterSize(&writer) == LG_NET_FILE_REQUEST_WIRE_SIZE;
}

LGNetParseResult lgNetFileRequestDecode(
    LGNetFileRequest * request, const void * data, size_t size)
{
  if (!request || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_FILE_REQUEST_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetFileRequest decoded;
  LGNetReader      reader;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU64(&reader, &decoded.requestID) ||
      !lgNetReaderU64(&reader, &decoded.offerID)   ||
      !lgNetReaderU64(&reader, &decoded.entryID)   ||
      !lgNetReaderU64(&reader, &decoded.offset)    ||
      !lgNetReaderU32(&reader, &decoded.length)    ||
      !lgNetReaderU32(&reader, &decoded.flags))
    return LG_NET_PARSE_INVALID_VALUE;

  const LGNetParseResult result = fixedDecodeResult(
    &reader, LG_NET_FILE_REQUEST_WIRE_SIZE, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetFileRequestValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *request = decoded;
  return LG_NET_PARSE_OK;
}

size_t lgNetFileChunkSize(const LGNetFileChunk * chunk)
{
  size_t size;
  return chunk && variableSize(LG_NET_FILE_CHUNK_HEADER_WIRE_SIZE,
    chunk->dataLength, LG_NET_FILE_MAX_CHUNK_LENGTH, &size) ? size : 0;
}

bool lgNetFileChunkValid(const LGNetFileChunk * chunk)
{
  if (!chunk || !chunk->requestID || !chunk->offerID || !chunk->entryID ||
      (chunk->flags & ~FILE_CHUNK_FLAGS) || lgNetFileChunkSize(chunk) == 0 ||
      chunk->offset > chunk->totalLength ||
      chunk->dataLength > chunk->totalLength - chunk->offset)
    return false;

  const bool first = chunk->offset == 0;
  const bool final = chunk->dataLength == chunk->totalLength - chunk->offset;
  return (!!(chunk->flags & LG_NET_FILE_CHUNK_FIRST) == first) &&
    (!!(chunk->flags & LG_NET_FILE_CHUNK_FINAL) == final) &&
    (chunk->totalLength ? chunk->dataLength && chunk->data :
      !chunk->dataLength && !chunk->data);
}

bool lgNetFileChunkEncode(
    void * data, size_t size, const LGNetFileChunk * chunk)
{
  const size_t wireSize = lgNetFileChunkSize(chunk);
  if (!data || !wireSize || size < wireSize || !lgNetFileChunkValid(chunk))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU64(&writer, chunk->requestID)   &&
    lgNetWriterU64(&writer, chunk->offerID)     &&
    lgNetWriterU64(&writer, chunk->entryID)     &&
    lgNetWriterU64(&writer, chunk->offset)      &&
    lgNetWriterU64(&writer, chunk->totalLength) &&
    lgNetWriterU32(&writer, chunk->dataLength)  &&
    lgNetWriterU16(&writer, chunk->flags)       &&
    lgNetWriterZero(&writer, 2)                 &&
    lgNetWriterBytes(&writer, chunk->data, chunk->dataLength) &&
    lgNetWriterSize(&writer) == wireSize;
}

LGNetParseResult lgNetFileChunkDecode(
    LGNetFileChunk * chunk, const void * data, size_t size)
{
  if (!chunk || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_FILE_CHUNK_HEADER_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetFileChunk decoded;
  LGNetReader    reader;
  size_t         expected;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU64(&reader, &decoded.requestID)   ||
      !lgNetReaderU64(&reader, &decoded.offerID)     ||
      !lgNetReaderU64(&reader, &decoded.entryID)     ||
      !lgNetReaderU64(&reader, &decoded.offset)      ||
      !lgNetReaderU64(&reader, &decoded.totalLength) ||
      !lgNetReaderU32(&reader, &decoded.dataLength)  ||
      !lgNetReaderU16(&reader, &decoded.flags)       ||
      !lgNetReaderZero(&reader, 2))
    return LG_NET_PARSE_INVALID_VALUE;

  LGNetParseResult result = variableDecodeSize(
    LG_NET_FILE_CHUNK_HEADER_WIRE_SIZE, decoded.dataLength,
    LG_NET_FILE_MAX_CHUNK_LENGTH, size, &expected);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetReaderView(&reader, &decoded.data, decoded.dataLength))
    return LG_NET_PARSE_INVALID_VALUE;
  result = fixedDecodeResult(&reader, expected, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetFileChunkValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *chunk = decoded;
  return LG_NET_PARSE_OK;
}

bool lgNetFileTransferValid(const LGNetFileTransfer * transfer)
{
  return transfer && transfer->requestID && transfer->offerID &&
    transfer->entryID && transfer->offset <= transfer->totalLength &&
    transfer->processedLength <= transfer->totalLength - transfer->offset;
}

bool lgNetFileTransferEncode(
    void * data, size_t size, const LGNetFileTransfer * transfer)
{
  if (!data || size < LG_NET_FILE_TRANSFER_WIRE_SIZE ||
      !lgNetFileTransferValid(transfer))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU64(&writer, transfer->requestID)       &&
    lgNetWriterU64(&writer, transfer->offerID)         &&
    lgNetWriterU64(&writer, transfer->entryID)         &&
    lgNetWriterU64(&writer, transfer->offset)          &&
    lgNetWriterU64(&writer, transfer->totalLength)     &&
    lgNetWriterU64(&writer, transfer->processedLength) &&
    lgNetWriterSize(&writer) == LG_NET_FILE_TRANSFER_WIRE_SIZE;
}

LGNetParseResult lgNetFileTransferDecode(
    LGNetFileTransfer * transfer, const void * data, size_t size)
{
  if (!transfer || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_FILE_TRANSFER_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetFileTransfer decoded;
  LGNetReader       reader;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU64(&reader, &decoded.requestID)       ||
      !lgNetReaderU64(&reader, &decoded.offerID)         ||
      !lgNetReaderU64(&reader, &decoded.entryID)         ||
      !lgNetReaderU64(&reader, &decoded.offset)          ||
      !lgNetReaderU64(&reader, &decoded.totalLength)     ||
      !lgNetReaderU64(&reader, &decoded.processedLength))
    return LG_NET_PARSE_INVALID_VALUE;

  const LGNetParseResult result = fixedDecodeResult(
    &reader, LG_NET_FILE_TRANSFER_WIRE_SIZE, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetFileTransferValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *transfer = decoded;
  return LG_NET_PARSE_OK;
}

static bool fileStatusKnown(LGNetFileStatusCode status)
{
  return status >= LG_NET_FILE_STATUS_OK &&
    status <= LG_NET_FILE_STATUS_ERROR;
}

bool lgNetFileStatusValid(const LGNetFileStatus * status)
{
  return status && status->requestID && status->offerID &&
    fileStatusKnown(status->status);
}

bool lgNetFileStatusEncode(
    void * data, size_t size, const LGNetFileStatus * status)
{
  if (!data || size < LG_NET_FILE_STATUS_WIRE_SIZE ||
      !lgNetFileStatusValid(status))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU64(&writer, status->requestID)         &&
    lgNetWriterU64(&writer, status->offerID)           &&
    lgNetWriterU64(&writer, status->entryID)           &&
    lgNetWriterU32(&writer, status->status)            &&
    lgNetWriterU32(&writer, status->detail)            &&
    lgNetWriterU64(&writer, status->transferredLength) &&
    lgNetWriterSize(&writer) == LG_NET_FILE_STATUS_WIRE_SIZE;
}

LGNetParseResult lgNetFileStatusDecode(
    LGNetFileStatus * status, const void * data, size_t size)
{
  if (!status || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_FILE_STATUS_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetFileStatus decoded;
  LGNetReader     reader;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU64(&reader, &decoded.requestID)         ||
      !lgNetReaderU64(&reader, &decoded.offerID)           ||
      !lgNetReaderU64(&reader, &decoded.entryID)           ||
      !lgNetReaderU32(&reader, &decoded.status)            ||
      !lgNetReaderU32(&reader, &decoded.detail)            ||
      !lgNetReaderU64(&reader, &decoded.transferredLength))
    return LG_NET_PARSE_INVALID_VALUE;

  const LGNetParseResult result = fixedDecodeResult(
    &reader, LG_NET_FILE_STATUS_WIRE_SIZE, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetFileStatusValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *status = decoded;
  return LG_NET_PARSE_OK;
}

size_t lgNetUSBReservedSize(const LGNetUSBReserved * reserved)
{
  size_t size;
  return reserved && variableSize(LG_NET_USB_RESERVED_HEADER_WIRE_SIZE,
    reserved->payloadLength, LG_NET_USB_MAX_RESERVED_PAYLOAD_LENGTH,
    &size) ? size : 0;
}

bool lgNetUSBReservedValid(const LGNetUSBReserved * reserved)
{
  return reserved &&
    reserved->version == LG_NET_USB_RESERVED_INTRODUCED_SERVICE_VERSION &&
    reserved->headerSize == LG_NET_USB_RESERVED_HEADER_WIRE_SIZE &&
    reserved->messageType && !(reserved->flags & ~USB_RESERVED_FLAGS) &&
    (!reserved->payloadLength || reserved->payload) &&
    lgNetUSBReservedSize(reserved) != 0;
}

bool lgNetUSBReservedEncode(
    void * data, size_t size, const LGNetUSBReserved * reserved)
{
  const size_t wireSize = lgNetUSBReservedSize(reserved);
  if (!data || !wireSize || size < wireSize ||
      !lgNetUSBReservedValid(reserved))
    return false;

  LGNetWriter writer;
  lgNetWriterInit(&writer, data, size);
  return
    lgNetWriterU16(&writer, reserved->version)       &&
    lgNetWriterU16(&writer, reserved->headerSize)    &&
    lgNetWriterU16(&writer, reserved->messageType)   &&
    lgNetWriterU16(&writer, reserved->flags)         &&
    lgNetWriterU64(&writer, reserved->requestID)     &&
    lgNetWriterU64(&writer, reserved->sequence)      &&
    lgNetWriterU32(&writer, reserved->payloadLength) &&
    lgNetWriterZero(&writer, 4)                      &&
    lgNetWriterBytes(&writer, reserved->payload,
      reserved->payloadLength)                       &&
    lgNetWriterSize(&writer) == wireSize;
}

LGNetParseResult lgNetUSBReservedDecode(
    LGNetUSBReserved * reserved, const void * data, size_t size)
{
  if (!reserved || !data)
    return LG_NET_PARSE_INVALID_VALUE;
  if (size < LG_NET_USB_RESERVED_HEADER_WIRE_SIZE)
    return LG_NET_PARSE_TRUNCATED;

  LGNetUSBReserved decoded;
  LGNetReader      reader;
  size_t           expected;
  memset(&decoded, 0, sizeof(decoded));
  lgNetReaderInit(&reader, data, size);
  if (!lgNetReaderU16(&reader, &decoded.version)       ||
      !lgNetReaderU16(&reader, &decoded.headerSize)    ||
      !lgNetReaderU16(&reader, &decoded.messageType)   ||
      !lgNetReaderU16(&reader, &decoded.flags)         ||
      !lgNetReaderU64(&reader, &decoded.requestID)     ||
      !lgNetReaderU64(&reader, &decoded.sequence)      ||
      !lgNetReaderU32(&reader, &decoded.payloadLength) ||
      !lgNetReaderZero(&reader, 4))
    return LG_NET_PARSE_INVALID_VALUE;

  LGNetParseResult result = variableDecodeSize(
    LG_NET_USB_RESERVED_HEADER_WIRE_SIZE, decoded.payloadLength,
    LG_NET_USB_MAX_RESERVED_PAYLOAD_LENGTH, size, &expected);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetReaderView(&reader, &decoded.payload, decoded.payloadLength))
    return LG_NET_PARSE_INVALID_VALUE;
  result = fixedDecodeResult(&reader, expected, size);
  if (result != LG_NET_PARSE_OK)
    return result;
  if (!lgNetUSBReservedValid(&decoded))
    return LG_NET_PARSE_INVALID_VALUE;

  *reserved = decoded;
  return LG_NET_PARSE_OK;
}
