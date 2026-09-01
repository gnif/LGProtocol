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

#ifndef LGPROTOCOL_KVMFR_AUDIO_H
#define LGPROTOCOL_KVMFR_AUDIO_H

#include "KVMFRStream.h"

#include <stddef.h>
#include <stdint.h>

#define KVMFR_AUDIO_PROTOCOL_MIN    2U
#define KVMFR_AUDIO_PROTOCOL_MAX    2U
#define KVMFR_AUDIO_CONTROL_VERSION 2U
#define KVMFR_AUDIO_MESSAGE_VERSION 1U
#define KVMFR_AUDIO_FORMAT_VERSION  1U
#define KVMFR_AUDIO_STATUS_VERSION  2U
#define KVMFR_AUDIO_VOLUME_VERSION  1U

#define KVMFR_AUDIO_MAX_CHANNELS          18U
#define KVMFR_AUDIO_MAX_SAMPLE_RATE       192000U
#define KVMFR_AUDIO_MAX_FRAMES_PER_PACKET 2048U
#define KVMFR_AUDIO_MAX_BYTES_PER_SAMPLE  4U
#define KVMFR_AUDIO_STREAM_MAX_CHANNELS   8U
#define KVMFR_AUDIO_STREAM_SLOT_COUNT     16U
#define KVMFR_AUDIO_MAX_PLAYBACK_READERS  8U
#define KVMFR_AUDIO_OWNER_LEASE_MS        1000U
#define KVMFR_AUDIO_KEEPALIVE_MS          200U
#define KVMFR_AUDIO_PCM_BYTES \
  (KVMFR_AUDIO_MAX_FRAMES_PER_PACKET * \
   KVMFR_AUDIO_STREAM_MAX_CHANNELS * \
   KVMFR_AUDIO_MAX_BYTES_PER_SAMPLE)

enum
{
  KVMFR_AUDIO_CAP_PLAYBACK       = 1U << 0,
  KVMFR_AUDIO_CAP_CAPTURE        = 1U << 1,
  KVMFR_AUDIO_CAP_CLOCK_FEEDBACK = 1U << 2,
};

typedef uint32_t KVMFRAudioCapabilities;

enum
{
  KVMFR_AUDIO_SUBSCRIBE_PLAYBACK = 1U << 0,
  KVMFR_AUDIO_SUBSCRIBE_CAPTURE  = 1U << 1,
};

typedef uint32_t KVMFRAudioSubscriptionFlags;

/* Transport-neutral wire copy of an exported LGMP SPMC descriptor. */
typedef struct KVMFRSPMCDescriptor
{
  uint32_t magic;
  uint16_t version;
  uint16_t size;
  uint32_t offset;
  uint32_t regionSize;
  uint32_t slotCount;
  uint32_t slotSize;
  uint32_t maxReaders;
  uint32_t reserved;
}
KVMFRSPMCDescriptor;

enum
{
  KVMFR_AUDIO_SAMPLE_S16_LE = 1,
  /* Signed little-endian samples packed into exactly three bytes. */
  KVMFR_AUDIO_SAMPLE_S24_LE = 2,
  KVMFR_AUDIO_SAMPLE_S32_LE = 3,
  /* IEEE 754 little-endian floating-point samples. */
  KVMFR_AUDIO_SAMPLE_F32_LE = 4,
};

typedef uint32_t KVMFRAudioSampleFormat;

enum
{
  KVMFR_AUDIO_SAMPLE_MASK_S16_LE = 1U << 0,
  KVMFR_AUDIO_SAMPLE_MASK_S24_LE = 1U << 1,
  KVMFR_AUDIO_SAMPLE_MASK_S32_LE = 1U << 2,
  KVMFR_AUDIO_SAMPLE_MASK_F32_LE = 1U << 3,
};

typedef uint32_t KVMFRAudioSampleFormatFlags;

/* These values match the conventional Windows speaker-position mask. */
enum
{
  KVMFR_AUDIO_CHANNEL_FRONT_LEFT            = 1U << 0,
  KVMFR_AUDIO_CHANNEL_FRONT_RIGHT           = 1U << 1,
  KVMFR_AUDIO_CHANNEL_FRONT_CENTER          = 1U << 2,
  KVMFR_AUDIO_CHANNEL_LFE                   = 1U << 3,
  KVMFR_AUDIO_CHANNEL_BACK_LEFT             = 1U << 4,
  KVMFR_AUDIO_CHANNEL_BACK_RIGHT            = 1U << 5,
  KVMFR_AUDIO_CHANNEL_FRONT_LEFT_OF_CENTER  = 1U << 6,
  KVMFR_AUDIO_CHANNEL_FRONT_RIGHT_OF_CENTER = 1U << 7,
  KVMFR_AUDIO_CHANNEL_BACK_CENTER           = 1U << 8,
  KVMFR_AUDIO_CHANNEL_SIDE_LEFT             = 1U << 9,
  KVMFR_AUDIO_CHANNEL_SIDE_RIGHT            = 1U << 10,
  KVMFR_AUDIO_CHANNEL_TOP_CENTER            = 1U << 11,
  KVMFR_AUDIO_CHANNEL_TOP_FRONT_LEFT        = 1U << 12,
  KVMFR_AUDIO_CHANNEL_TOP_FRONT_CENTER      = 1U << 13,
  KVMFR_AUDIO_CHANNEL_TOP_FRONT_RIGHT       = 1U << 14,
  KVMFR_AUDIO_CHANNEL_TOP_BACK_LEFT         = 1U << 15,
  KVMFR_AUDIO_CHANNEL_TOP_BACK_CENTER       = 1U << 16,
  KVMFR_AUDIO_CHANNEL_TOP_BACK_RIGHT        = 1U << 17,
};

typedef uint32_t KVMFRAudioChannelMask;

#define KVMFR_AUDIO_CHANNEL_MONO \
  KVMFR_AUDIO_CHANNEL_FRONT_CENTER
#define KVMFR_AUDIO_CHANNEL_STEREO \
  (KVMFR_AUDIO_CHANNEL_FRONT_LEFT | KVMFR_AUDIO_CHANNEL_FRONT_RIGHT)
#define KVMFR_AUDIO_CHANNEL_5POINT1 \
  (KVMFR_AUDIO_CHANNEL_STEREO | KVMFR_AUDIO_CHANNEL_FRONT_CENTER | \
   KVMFR_AUDIO_CHANNEL_LFE | KVMFR_AUDIO_CHANNEL_SIDE_LEFT | \
   KVMFR_AUDIO_CHANNEL_SIDE_RIGHT)
#define KVMFR_AUDIO_CHANNEL_7POINT1 \
  (KVMFR_AUDIO_CHANNEL_5POINT1 | KVMFR_AUDIO_CHANNEL_BACK_LEFT | \
   KVMFR_AUDIO_CHANNEL_BACK_RIGHT)
#define KVMFR_AUDIO_CHANNEL_POSITION_MASK UINT32_C(0x0003FFFF)

/* Interleaved samples and volume entries use ascending set-bit order from
 * channelMask. channelCount must equal the number of set mask bits and may
 * cover all 18 standard positions even when an implementation advertises a
 * lower active channel limit. */
typedef struct KVMFRAudioFormat
{
  uint16_t                   version;
  uint16_t                   size;
  KVMFRAudioSampleFormat     sampleFormat;
  uint32_t                   sampleRate;
  KVMFRAudioChannelMask      channelMask;
  uint16_t                   channelCount;
  uint16_t                   validBits;
  uint16_t                   containerBits;
  uint16_t                   blockAlign;
  uint32_t                   reserved[2];
}
KVMFRAudioFormat;

enum
{
  KVMFR_AUDIO_MESSAGE_STATUS = 1,

  KVMFR_AUDIO_MESSAGE_PLAYBACK_START,
  KVMFR_AUDIO_MESSAGE_PLAYBACK_STOP,
  KVMFR_AUDIO_MESSAGE_PLAYBACK_DATA,
  KVMFR_AUDIO_MESSAGE_PLAYBACK_VOLUME,
  KVMFR_AUDIO_MESSAGE_PLAYBACK_MUTE,

  KVMFR_AUDIO_MESSAGE_CAPTURE_START,
  KVMFR_AUDIO_MESSAGE_CAPTURE_STOP,
  KVMFR_AUDIO_MESSAGE_CAPTURE_DATA,
  KVMFR_AUDIO_MESSAGE_CAPTURE_VOLUME,
  KVMFR_AUDIO_MESSAGE_CAPTURE_MUTE,

  KVMFR_AUDIO_MESSAGE_CLOCK_FEEDBACK,
  KVMFR_AUDIO_MESSAGE_KEEPALIVE,
  KVMFR_AUDIO_MESSAGE_RELEASE,

  KVMFR_AUDIO_MESSAGE_STATE_ACK,
  KVMFR_AUDIO_MESSAGE_PLAYBACK_SUBSCRIBE,
  KVMFR_AUDIO_MESSAGE_STATE_BARRIER,
};

typedef uint32_t KVMFRAudioMessageType;

enum
{
  KVMFR_AUDIO_MESSAGE_DISCONTINUITY = 1U << 0,
  KVMFR_AUDIO_MESSAGE_SILENT        = 1U << 1,
  KVMFR_AUDIO_MESSAGE_CLOCK_STABLE  = 1U << 2,
  KVMFR_AUDIO_MESSAGE_MUTED         = 1U << 3,
  KVMFR_AUDIO_MESSAGE_CLOCK_VALID   = 1U << 4,
};

typedef uint32_t KVMFRAudioMessageFlags;

/*
 * Control and capture records use three per-client LGMP streams:
 *
 * host-to-client control: personalized STATUS, PLAYBACK/CAPTURE state and
 *                         STATE_BARRIER;
 * client-to-host control: CLOCK_FEEDBACK, KEEPALIVE, RELEASE, STATE_ACK, and
 *                         PLAYBACK_SUBSCRIBE;
 * client-to-host capture: CAPTURE_DATA.
 *
 * PLAYBACK_DATA records use the shared LGMP SPMC stream. Each client has an
 * independent reader cursor, so a slow reader never delays another client or
 * the producer.
 *
 * Any payload immediately follows this fixed header. START carries one
 * KVMFRAudioFormat, VOLUME carries KVMFRAudioVolume, and DATA
 * carries frames * blockAlign bytes. The position and timeNs fields describe
 * the first frame of DATA or the correlated clock sample in CLOCK_FEEDBACK.
 * Times are monotonic nanoseconds in the publisher's clock domain and may be
 * compared only by difference unless that domain is otherwise known.
 */
typedef struct KVMFRAudioMessage
{
  uint16_t               version;
  uint16_t               size;
  KVMFRAudioMessageType  type;
  uint64_t               streamGeneration;
  KVMFRAudioMessageFlags flags;
  uint32_t               reserved0;
  uint64_t               sequence;
  uint64_t               position;
  int64_t                timeNs;
  /* Unsigned 32.32 frames per second; zero means unavailable. */
  uint64_t               rateQ32;
  uint32_t               frames;
  uint32_t               length;
}
KVMFRAudioMessage;

/*
 * Type-specific payload and header rules:
 *
 * STATUS:
 *   host-to-client, KVMFRAudioStatus payload, zero streamGeneration/frames.
 * PLAYBACK_START / CAPTURE_START:
 *   host-to-client, KVMFRAudioFormat payload, nonzero streamGeneration and
 *   zero frames. CLOCK_VALID marks an optional source-clock correlation.
 * PLAYBACK_STOP / CAPTURE_STOP:
 *   host-to-client, no payload, nonzero streamGeneration and zero frames.
 * PLAYBACK_DATA / CAPTURE_DATA:
 *   the direction implied by the name, nonzero streamGeneration, and between
 *   one and KVMFR_AUDIO_MAX_FRAMES_PER_PACKET frames. Without SILENT, length
 *   is frames * blockAlign, does not exceed KVMFR_AUDIO_PCM_BYTES, and the
 *   payload is interleaved PCM. With SILENT, length is zero and no PCM payload
 *   is present.
 * PLAYBACK_VOLUME / CAPTURE_VOLUME:
 *   host-to-client, KVMFRAudioVolume payload, nonzero streamGeneration and
 *   zero frames.
 * PLAYBACK_MUTE / CAPTURE_MUTE:
 *   host-to-client, no payload, nonzero streamGeneration, zero frames, and
 *   MUTED set or clear for the new state.
 * CLOCK_FEEDBACK:
 *   client-to-host, KVMFRAudioClockFeedback payload, the playback generation,
 *   zero frames, and CLOCK_VALID set.
 * KEEPALIVE / RELEASE:
 *   client-to-host, no payload and zero streamGeneration/frames.
 * STATE_BARRIER / STATE_ACK:
 *   respectively host-to-client and client-to-host, a
 *   KVMFRAudioStateBarrier payload, zero frames, and the current playback
 *   generation (or zero while playback is stopped). STATE_ACK echoes the
 *   complete barrier after the client installs the preceding reliable state
 *   and synchronizes its SPMC reader.
 * PLAYBACK_SUBSCRIBE:
 *   client-to-host, a KVMFRAudioSubscription payload and otherwise empty.
 *
 * Sequence starts at zero independently on each reliable stream while bound.
 * PLAYBACK_DATA sequence equals its SPMC publication sequence; a cancelled
 * reservation or lapped reader observes a gap and treats the next record as a
 * discontinuity.
 */

/* Payload for CLOCK_FEEDBACK. The message clock describes the playback
 * device; targetRateQ32 is the requested source rate. */
typedef struct KVMFRAudioClockFeedback
{
  uint64_t targetRateQ32;
  uint32_t bindingEpoch;
  uint32_t roleEpoch;
}
KVMFRAudioClockFeedback;

/* A personalized control barrier binds SPMC playback consumption to the
 * state and authority snapshot delivered on the reliable control stream. */
typedef struct KVMFRAudioStateBarrier
{
  uint32_t bindingEpoch;
  uint32_t roleEpoch;
  uint64_t stateSerial;
}
KVMFRAudioStateBarrier;

/* Granted capabilities describe admission and authority. This independently
 * selects which admitted directions should currently deliver stream state. */
typedef struct KVMFRAudioSubscription
{
  uint32_t                    bindingEpoch;
  uint32_t                    roleEpoch;
  KVMFRAudioSubscriptionFlags subscriptions;
  uint32_t                    reserved;
}
KVMFRAudioSubscription;

/* Fixed-size payload for PLAYBACK_VOLUME and CAPTURE_VOLUME. Values use the
 * full unsigned 16-bit range, where zero is silent and UINT16_MAX is unity.
 * Entries beyond channelCount must be zero. */
typedef struct KVMFRAudioVolume
{
  uint16_t version;
  uint16_t size;
  uint16_t channelCount;
  uint16_t reserved;
  uint16_t values[KVMFR_AUDIO_MAX_CHANNELS];
}
KVMFRAudioVolume;

#define KVMFR_AUDIO_PLAYBACK_SLOT_BYTES       \
  (sizeof(KVMFRAudioMessage) + KVMFR_AUDIO_PCM_BYTES)
#define KVMFR_AUDIO_CAPTURE_SLOT_BYTES        KVMFR_AUDIO_PLAYBACK_SLOT_BYTES
#define KVMFR_AUDIO_HOST_CONTROL_SLOT_BYTES   \
  (sizeof(KVMFRAudioMessage) + sizeof(KVMFRAudioStatus))
#define KVMFR_AUDIO_CLIENT_CONTROL_SLOT_BYTES \
  (sizeof(KVMFRAudioMessage) + sizeof(KVMFRAudioClockFeedback))
/* Retained as the PCM stream geometry name used by existing integrations. */
#define KVMFR_AUDIO_STREAM_SLOT_BYTES         KVMFR_AUDIO_PLAYBACK_SLOT_BYTES

enum
{
  KVMFR_AUDIO_STATUS_AVAILABLE          = 1U << 0,
  KVMFR_AUDIO_STATUS_BOUND              = 1U << 1,
  KVMFR_AUDIO_STATUS_HAS_OWNER          = KVMFR_AUDIO_STATUS_BOUND,
  KVMFR_AUDIO_STATUS_PLAYBACK_AVAILABLE = 1U << 2,
  KVMFR_AUDIO_STATUS_CAPTURE_AVAILABLE  = 1U << 3,
  KVMFR_AUDIO_STATUS_PLAYBACK_ACTIVE    = 1U << 4,
  KVMFR_AUDIO_STATUS_CAPTURE_ACTIVE     = 1U << 5,
};

typedef uint32_t KVMFRAudioStatusFlags;

enum
{
  KVMFR_AUDIO_STREAM_STOPPED  = 0,
  KVMFR_AUDIO_STREAM_ACQUIRED = 1,
  KVMFR_AUDIO_STREAM_PAUSED   = 2,
  KVMFR_AUDIO_STREAM_RUNNING  = 3,
};

typedef uint32_t KVMFRAudioStreamState;

typedef struct KVMFRAudioStatus
{
  uint16_t               version;
  uint16_t               size;
  KVMFRAudioStatusFlags  flags;
  uint32_t               protocolVersion;
  uint32_t               requestGeneration;
  KVMFRAudioCapabilities capabilities;
  KVMFRAudioCapabilities grantedCapabilities;
  uint32_t               bindingEpoch;
  uint32_t               roleEpoch;
  uint32_t               boundClientID;
  uint32_t               feedbackAuthorityClientID;
  uint32_t               feedbackAuthorityEpoch;
  uint32_t               captureOwnerClientID;
  uint32_t               reserved0;
  uint32_t               reserved1;
  uint64_t               endpointGeneration;
  uint64_t               playbackGeneration;
  uint64_t               captureGeneration;
  KVMFRAudioStreamState  playbackState;
  KVMFRAudioStreamState  captureState;
  KVMFRAudioFormat       playbackFormat;
  KVMFRAudioFormat       captureFormat;
}
KVMFRAudioStatus;

enum
{
  KVMFR_AUDIO_CLAIM_BIND = 1U << 0,
};

typedef uint32_t KVMFRAudioClaimFlags;

/* Sent through the existing LGMP client-control message path. flags must be
 * exactly KVMFR_AUDIO_CLAIM_BIND. clientCapabilities requests the roles this
 * client can perform; subscriptions requests its initial active directions.
 * Audio support is statically advertised in KVMFR features, so runtime
 * availability is not a prerequisite for binding these streams. */
typedef struct KVMFRAudioClaim
{
  uint32_t                    messageType;
  uint16_t                    version;
  uint16_t                    size;
  uint32_t                    requestGeneration;
  KVMFRAudioClaimFlags        flags;
  uint64_t                    claimToken;
  uint32_t                    protocolMin;
  uint32_t                    protocolMax;
  KVMFRAudioCapabilities      clientCapabilities;
  KVMFRAudioSubscriptionFlags subscriptions;
  uint64_t                    reserved[3];
}
KVMFRAudioClaim;

/*
 * Descriptor responses share the existing pointer queue. The high udata word
 * identifies this record; the low word remains zero for cursor compatibility.
 */
#define KVMFR_AUDIO_CONTROL_TAG         UINT32_C(0x41554431)
#define KVMFR_AUDIO_CONTROL_UDATA       \
  (UINT64_C(0x41554431) << 32)
#define KVMFR_AUDIO_RESPONSE_MAGIC      UINT32_C(0x41554452)
#define KVMFR_AUDIO_CURSOR_COMPAT_BYTES 28U

enum
{
  KVMFR_AUDIO_RESPONSE_BOUND        = 1,
  KVMFR_AUDIO_RESPONSE_BUSY         = 2,
  KVMFR_AUDIO_RESPONSE_INCOMPATIBLE = 3,
  KVMFR_AUDIO_RESPONSE_ERROR        = 4,
};

typedef uint16_t KVMFRAudioResponseResult;

/* Exactly one response result is returned. A BOUND response selects protocol
 * v2 and makes all four descriptors and the SPMC reader identity valid. */
typedef struct KVMFRAudioStreamResponse
{
  uint8_t                   cursorCompatibility[28];
  uint32_t                  magic;
  uint16_t                  version;
  uint16_t                  size;
  uint32_t                  requestGeneration;
  uint32_t                  boundClientID;
  KVMFRAudioResponseResult  result;
  uint16_t                  protocolVersion;
  uint64_t                  endpointGeneration;
  uint64_t                  claimToken;
  KVMFRAudioCapabilities    grantedCapabilities;
  uint32_t                  bindingEpoch;
  uint32_t                  roleEpoch;
  uint32_t                  playbackReaderID;
  KVMFRSPMCDescriptor       playback;
  KVMFRStreamDescriptor     hostControl;
  KVMFRStreamDescriptor     clientControl;
  KVMFRStreamDescriptor     capture;
}
KVMFRAudioStreamResponse;

#if defined(__cplusplus)
static_assert(sizeof(KVMFRAudioFormat) == 32,
  "KVMFR audio format layout changed");
static_assert(sizeof(KVMFRAudioMessage) == 64,
  "KVMFR audio message layout changed");
static_assert(sizeof(KVMFRAudioClockFeedback) == 16,
  "KVMFR audio clock feedback layout changed");
static_assert(sizeof(KVMFRAudioStateBarrier) == 16,
  "KVMFR audio state barrier layout changed");
static_assert(sizeof(KVMFRAudioSubscription) == 16,
  "KVMFR audio subscription layout changed");
static_assert(sizeof(KVMFRAudioVolume) == 44,
  "KVMFR audio volume layout changed");
static_assert(sizeof(KVMFRSPMCDescriptor) == 32,
  "KVMFR SPMC descriptor layout changed");
static_assert(offsetof(KVMFRSPMCDescriptor, offset) == 8,
  "KVMFR SPMC descriptor offset layout changed");
static_assert(offsetof(KVMFRSPMCDescriptor, maxReaders) == 24,
  "KVMFR SPMC descriptor geometry layout changed");
static_assert(sizeof(KVMFRAudioStatus) == 152,
  "KVMFR audio status layout changed");
static_assert(offsetof(KVMFRAudioStatus, grantedCapabilities) == 20,
  "KVMFR audio status grant layout changed");
static_assert(offsetof(KVMFRAudioStatus, bindingEpoch) == 24,
  "KVMFR audio status binding layout changed");
static_assert(offsetof(KVMFRAudioStatus, feedbackAuthorityClientID) == 36,
  "KVMFR audio status authority layout changed");
static_assert(offsetof(KVMFRAudioStatus, endpointGeneration) == 56,
  "KVMFR audio endpoint generation layout changed");
static_assert(offsetof(KVMFRAudioStatus, playbackFormat) == 88,
  "KVMFR audio status format layout changed");
static_assert(sizeof(KVMFRAudioClaim) == 64,
  "KVMFR audio claim must fit one LGMP control message");
static_assert(offsetof(KVMFRAudioStreamResponse, magic) == 28,
  "KVMFR audio pointer compatibility layout changed");
static_assert(offsetof(KVMFRAudioStreamResponse, endpointGeneration) == 48,
  "KVMFR audio response generation layout changed");
static_assert(offsetof(KVMFRAudioStreamResponse, claimToken) == 56,
  "KVMFR audio response token layout changed");
static_assert(offsetof(KVMFRAudioStreamResponse, grantedCapabilities) == 64,
  "KVMFR audio response grant layout changed");
static_assert(offsetof(KVMFRAudioStreamResponse, playback) == 80,
  "KVMFR audio stream discovery layout changed");
static_assert(offsetof(KVMFRAudioStreamResponse, capture) == 176,
  "KVMFR audio capture discovery layout changed");
static_assert(sizeof(KVMFRAudioStreamResponse) == 208,
  "KVMFR audio stream response layout changed");
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
_Static_assert(sizeof(KVMFRAudioFormat) == 32,
  "KVMFR audio format layout changed");
_Static_assert(sizeof(KVMFRAudioMessage) == 64,
  "KVMFR audio message layout changed");
_Static_assert(sizeof(KVMFRAudioClockFeedback) == 16,
  "KVMFR audio clock feedback layout changed");
_Static_assert(sizeof(KVMFRAudioStateBarrier) == 16,
  "KVMFR audio state barrier layout changed");
_Static_assert(sizeof(KVMFRAudioSubscription) == 16,
  "KVMFR audio subscription layout changed");
_Static_assert(sizeof(KVMFRAudioVolume) == 44,
  "KVMFR audio volume layout changed");
_Static_assert(sizeof(KVMFRSPMCDescriptor) == 32,
  "KVMFR SPMC descriptor layout changed");
_Static_assert(offsetof(KVMFRSPMCDescriptor, offset) == 8,
  "KVMFR SPMC descriptor offset layout changed");
_Static_assert(offsetof(KVMFRSPMCDescriptor, maxReaders) == 24,
  "KVMFR SPMC descriptor geometry layout changed");
_Static_assert(sizeof(KVMFRAudioStatus) == 152,
  "KVMFR audio status layout changed");
_Static_assert(offsetof(KVMFRAudioStatus, grantedCapabilities) == 20,
  "KVMFR audio status grant layout changed");
_Static_assert(offsetof(KVMFRAudioStatus, bindingEpoch) == 24,
  "KVMFR audio status binding layout changed");
_Static_assert(offsetof(KVMFRAudioStatus, feedbackAuthorityClientID) == 36,
  "KVMFR audio status authority layout changed");
_Static_assert(offsetof(KVMFRAudioStatus, endpointGeneration) == 56,
  "KVMFR audio endpoint generation layout changed");
_Static_assert(offsetof(KVMFRAudioStatus, playbackFormat) == 88,
  "KVMFR audio status format layout changed");
_Static_assert(sizeof(KVMFRAudioClaim) == 64,
  "KVMFR audio claim must fit one LGMP control message");
_Static_assert(offsetof(KVMFRAudioStreamResponse, magic) == 28,
  "KVMFR audio pointer compatibility layout changed");
_Static_assert(offsetof(KVMFRAudioStreamResponse, endpointGeneration) == 48,
  "KVMFR audio response generation layout changed");
_Static_assert(offsetof(KVMFRAudioStreamResponse, claimToken) == 56,
  "KVMFR audio response token layout changed");
_Static_assert(offsetof(KVMFRAudioStreamResponse, grantedCapabilities) == 64,
  "KVMFR audio response grant layout changed");
_Static_assert(offsetof(KVMFRAudioStreamResponse, playback) == 80,
  "KVMFR audio stream discovery layout changed");
_Static_assert(offsetof(KVMFRAudioStreamResponse, capture) == 176,
  "KVMFR audio capture discovery layout changed");
_Static_assert(sizeof(KVMFRAudioStreamResponse) == 208,
  "KVMFR audio stream response layout changed");
#endif

#endif
