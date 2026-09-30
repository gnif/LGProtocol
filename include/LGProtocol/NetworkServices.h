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

#ifndef LGPROTOCOL_NETWORK_SERVICES_H
#define LGPROTOCOL_NETWORK_SERVICES_H

#include "NetworkProtocol.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Variable-length decoders with pointer fields return borrowed views into
 * their input buffer. The caller must keep that buffer alive while using the
 * decoded payload. Fixed-capacity arrays, such as per-channel volume, are
 * copied into the decoded structure. */

/* Fixed boundaries must not move when a service's current version changes. */
#define LG_NET_VIDEO_PYROWAVE_INTRODUCED_SERVICE_VERSION  1U
#define LG_NET_CURSOR_SHAPE_INTRODUCED_SERVICE_VERSION    1U
#define LG_NET_CURSOR_COLOR_TRANSFORM_INTRODUCED_SERVICE_VERSION \
  2U
#define LG_NET_INPUT_LEDS_INTRODUCED_SERVICE_VERSION      1U
#define LG_NET_AUDIO_PCM_INTRODUCED_SERVICE_VERSION       1U
#define LG_NET_CLIPBOARD_CHUNK_INTRODUCED_SERVICE_VERSION 1U
#define LG_NET_FILE_CHUNK_INTRODUCED_SERVICE_VERSION      1U
#define LG_NET_USB_RESERVED_INTRODUCED_SERVICE_VERSION    1U
#define LG_NET_VIDEO_BLOCK_FRAGMENTATION_INTRODUCED_SERVICE_VERSION \
  2U

#define LG_NET_VIDEO_MAX_WIDTH                    16384U
#define LG_NET_VIDEO_MAX_HEIGHT                   16384U
#define LG_NET_VIDEO_MAX_PLANES                   4U
#define LG_NET_VIDEO_STREAM_CONFIG_WIRE_SIZE      60U
#define LG_NET_VIDEO_FRAME_HEADER_WIRE_SIZE       60U
#define LG_NET_VIDEO_FRAGMENT_HEADER_WIRE_SIZE    72U
#define LG_NET_VIDEO_FEEDBACK_WIRE_SIZE           56U
#define LG_NET_VIDEO_MAX_FRAME_LENGTH             \
  (LG_NET_MAX_PAYLOAD_LENGTH - LG_NET_VIDEO_FRAME_HEADER_WIRE_SIZE)
#define LG_NET_VIDEO_MAX_FRAGMENT_LENGTH          65535U
#define LG_NET_VIDEO_MAX_FRAGMENTS                1048576U
#define LG_NET_VIDEO_MAX_BLOCKS                   1048576U

#define LG_NET_VIDEO_PYROWAVE_CODEC_VERSION_INITIAL 1U
#define LG_NET_VIDEO_PYROWAVE_CODEC_VERSION_CURRENT 1U
#define LG_NET_VIDEO_PYROWAVE_CODEC_VERSION_MIN     \
  LG_NET_VIDEO_PYROWAVE_CODEC_VERSION_INITIAL
#define LG_NET_VIDEO_PYROWAVE_CODEC_VERSION_MAX     \
  LG_NET_VIDEO_PYROWAVE_CODEC_VERSION_CURRENT

#define LG_NET_CURSOR_MAX_WIDTH                   4096U
#define LG_NET_CURSOR_MAX_HEIGHT                  4096U
#define LG_NET_CURSOR_MAX_DESKTOP_WIDTH           32768U
#define LG_NET_CURSOR_MAX_DESKTOP_HEIGHT          32768U
#define LG_NET_CURSOR_MAX_SHAPE_LENGTH            (16U * 1024U * 1024U)
#define LG_NET_CURSOR_POSITION_WIRE_SIZE          40U
#define LG_NET_CURSOR_SHAPE_HEADER_WIRE_SIZE      40U
#define LG_NET_CURSOR_COLOR_TRANSFORM_HEADER_WIRE_SIZE   72U
#define LG_NET_CURSOR_COLOR_MATRIX_FLOATS                12U
#define LG_NET_CURSOR_COLOR_LUT_ENTRIES                  4096U
#define LG_NET_CURSOR_COLOR_LUT_CHANNELS                 4U
#define LG_NET_CURSOR_COLOR_LUT_FLOATS                   \
  (LG_NET_CURSOR_COLOR_LUT_ENTRIES * LG_NET_CURSOR_COLOR_LUT_CHANNELS)
#define LG_NET_CURSOR_COLOR_LUT_BYTES                    \
  (LG_NET_CURSOR_COLOR_LUT_FLOATS * sizeof(float))

#define LG_NET_INPUT_CLAIM_WIRE_SIZE              24U
#define LG_NET_INPUT_STATUS_WIRE_SIZE             32U
#define LG_NET_INPUT_RELATIVE_WIRE_SIZE           40U
#define LG_NET_INPUT_ABSOLUTE_WIRE_SIZE           48U
#define LG_NET_INPUT_KEYBOARD_WIRE_SIZE           32U
#define LG_NET_INPUT_LEDS_WIRE_SIZE               16U
#define LG_NET_INPUT_ABSOLUTE_MAX_WIDTH           32768U
#define LG_NET_INPUT_ABSOLUTE_MAX_HEIGHT          32768U
#define LG_NET_INPUT_KEYBOARD_MODIFIER_MASK       UINT32_C(0x000000FF)
#define LG_NET_INPUT_MAX_LEASE_MS                 10000U

#define LG_NET_AUDIO_MAX_CHANNELS                 32U
#define LG_NET_AUDIO_MAX_SAMPLE_RATE              768000U
#define LG_NET_AUDIO_MAX_PACKET_FRAMES            16384U
#define LG_NET_AUDIO_MAX_DATA_LENGTH              (4U * 1024U * 1024U)
#define LG_NET_AUDIO_FORMAT_WIRE_SIZE             40U
#define LG_NET_AUDIO_DATA_HEADER_WIRE_SIZE        48U
#define LG_NET_AUDIO_STATE_WIRE_SIZE              32U
#define LG_NET_AUDIO_CLOCK_FEEDBACK_WIRE_SIZE     48U

#define LG_NET_CLIPBOARD_MAX_MIME_LENGTH          255U
#define LG_NET_CLIPBOARD_MAX_FORMATS              256U
#define LG_NET_CLIPBOARD_MAX_CHUNK_LENGTH         (4U * 1024U * 1024U)
#define LG_NET_CLIPBOARD_CLAIM_WIRE_SIZE          24U
#define LG_NET_CLIPBOARD_OFFER_HEADER_WIRE_SIZE   40U
#define LG_NET_CLIPBOARD_REQUEST_WIRE_SIZE        32U
#define LG_NET_CLIPBOARD_CHUNK_HEADER_WIRE_SIZE   40U
#define LG_NET_CLIPBOARD_STATUS_WIRE_SIZE         32U

#define LG_NET_FILE_MAX_LABEL_LENGTH              1024U
#define LG_NET_FILE_MAX_PATH_LENGTH               4096U
#define LG_NET_FILE_MAX_LINK_TARGET_LENGTH        4096U
#define LG_NET_FILE_MAX_ENTRIES                   1048576U
#define LG_NET_FILE_MAX_CHUNK_LENGTH              (4U * 1024U * 1024U)
#define LG_NET_FILE_OFFER_HEADER_WIRE_SIZE        32U
#define LG_NET_FILE_ENTRY_HEADER_WIRE_SIZE        56U
#define LG_NET_FILE_REQUEST_WIRE_SIZE             40U
#define LG_NET_FILE_CHUNK_HEADER_WIRE_SIZE        48U
#define LG_NET_FILE_STATUS_WIRE_SIZE              40U

#define LG_NET_USB_RESERVED_HEADER_WIRE_SIZE      32U
#define LG_NET_USB_MAX_RESERVED_PAYLOAD_LENGTH    \
  (LG_NET_MAX_PAYLOAD_LENGTH - LG_NET_USB_RESERVED_HEADER_WIRE_SIZE)

#define LG_NET_CORE_MAX_NAME_LENGTH               255U
#define LG_NET_CORE_MAX_ERROR_TEXT_LENGTH         4096U
#define LG_NET_CORE_SESSION_INFO_HEADER_WIRE_SIZE 48U
#define LG_NET_CORE_STATUS_WIRE_SIZE              40U
#define LG_NET_CORE_ERROR_HEADER_WIRE_SIZE        32U

#define LG_NET_RECOVERY_MAX_VERSION_LENGTH        255U
#define LG_NET_RECOVERY_MAX_TIMEOUT_MS            120000U
#define LG_NET_RECOVERY_INFO_HEADER_WIRE_SIZE     48U
#define LG_NET_RECOVERY_REQUEST_WIRE_SIZE         32U
#define LG_NET_RECOVERY_STATUS_WIRE_SIZE          40U

#define LG_NET_VIDEO_SUBSCRIBE_WIRE_SIZE          32U
#define LG_NET_VIDEO_CONTROL_WIRE_SIZE            24U
#define LG_NET_VIDEO_SCHEDULE_WIRE_SIZE           48U
#define LG_NET_VIDEO_STATUS_WIRE_SIZE             40U

#define LG_NET_CURSOR_STATE_WIRE_SIZE             40U
#define LG_NET_CURSOR_TRANSFORM_WIRE_SIZE         48U
#define LG_NET_CURSOR_STATUS_WIRE_SIZE            40U

#define LG_NET_INPUT_CONTROL_WIRE_SIZE            32U
#define LG_NET_POINTER_BUTTON_MASK                UINT32_MAX

#define LG_NET_AUDIO_SUBSCRIBE_WIRE_SIZE          24U
#define LG_NET_AUDIO_CONTROL_WIRE_SIZE            32U
#define LG_NET_AUDIO_VOLUME_HEADER_WIRE_SIZE      24U
#define LG_NET_AUDIO_MUTE_WIRE_SIZE               24U
#define LG_NET_AUDIO_BARRIER_WIRE_SIZE            24U
#define LG_NET_AUDIO_VOLUME_MIN_MILLIBELS         (-9600)
#define LG_NET_AUDIO_VOLUME_MAX_MILLIBELS         1200

#define LG_NET_CLIPBOARD_CONTROL_WIRE_SIZE        32U
#define LG_NET_CLIPBOARD_TRANSFER_WIRE_SIZE       32U
#define LG_NET_CLIPBOARD_CLAIM_STATUS_WIRE_SIZE   32U
#define LG_NET_CLIPBOARD_MAX_LEASE_MS             60000U

#define LG_NET_FILE_LEASE_WIRE_SIZE               32U
#define LG_NET_FILE_TRANSFER_WIRE_SIZE            48U
#define LG_NET_FILE_MAX_LEASE_MS                  300000U

#define LG_NET_CONTROL_CURSOR_POSITION_WIRE_SIZE  16U
#define LG_NET_CONTROL_DISPLAY_SIZE_WIRE_SIZE     16U
#define LG_NET_CONTROL_FRAME_SCHEDULE_WIRE_SIZE   56U
#define LG_NET_CONTROL_STATUS_WIRE_SIZE           16U
#define LG_NET_CONTROL_MAX_LEASE_MS               60000U

typedef uint32_t LGNetCoreSessionFlags;

enum
{
  LG_NET_CORE_SESSION_AUTHENTICATED     = 1U << 0,
  LG_NET_CORE_SESSION_PASSWORD_REQUIRED = 1U << 1,
  LG_NET_CORE_SESSION_MULTI_CLIENT      = 1U << 2,
};

/* LG_NET_CORE_MESSAGE_SESSION_INFO. The name is UTF-8 without a trailing
 * NUL; decoders return it as a borrowed view into the input buffer. */
typedef struct LGNetCoreSessionInfo
{
  uint64_t              sessionID;
  uint64_t              connectedAtNs;
  uint64_t              serverTimeNs;
  uint32_t              clientID;
  uint32_t              activeClients;
  uint32_t              maxClients;
  LGNetCoreSessionFlags flags;
  uint16_t              nameLength;
  const uint8_t *       name;
}
LGNetCoreSessionInfo;

typedef uint32_t LGNetCoreState;

enum
{
  LG_NET_CORE_STATE_READY    = 1,
  LG_NET_CORE_STATE_DEGRADED = 2,
  LG_NET_CORE_STATE_STOPPING = 3,
  LG_NET_CORE_STATE_ERROR    = 4,
};

typedef uint32_t LGNetCoreStatusFlags;

enum
{
  LG_NET_CORE_STATUS_VIDEO_AVAILABLE     = 1U << 0,
  LG_NET_CORE_STATUS_INPUT_AVAILABLE     = 1U << 1,
  LG_NET_CORE_STATUS_AUDIO_AVAILABLE     = 1U << 2,
  LG_NET_CORE_STATUS_CLIPBOARD_AVAILABLE = 1U << 3,
  LG_NET_CORE_STATUS_FILE_AVAILABLE      = 1U << 4,
  LG_NET_CORE_STATUS_CURSOR_AVAILABLE    = 1U << 5,
  LG_NET_CORE_STATUS_RECOVERY_AVAILABLE  = 1U << 6,
  LG_NET_CORE_STATUS_CONTROL_AVAILABLE   = 1U << 7,
};

/* LG_NET_CORE_MESSAGE_STATUS. */
typedef struct LGNetCoreStatus
{
  uint64_t             sessionID;
  uint64_t             statusSequence;
  uint64_t             uptimeNs;
  LGNetCoreState       state;
  uint32_t             activeClients;
  LGNetCoreStatusFlags flags;
  uint32_t             detail;
}
LGNetCoreStatus;

typedef uint32_t LGNetCoreErrorCode;

enum
{
  LG_NET_CORE_ERROR_INVALID_REQUEST = 1,
  LG_NET_CORE_ERROR_UNAUTHORIZED    = 2,
  LG_NET_CORE_ERROR_UNSUPPORTED     = 3,
  LG_NET_CORE_ERROR_BUSY            = 4,
  LG_NET_CORE_ERROR_TIMEOUT         = 5,
  LG_NET_CORE_ERROR_PROTOCOL        = 6,
  LG_NET_CORE_ERROR_INTERNAL        = 7,
};

/* LG_NET_CORE_MESSAGE_ERROR. text is UTF-8 without a trailing NUL. */
typedef struct LGNetCoreError
{
  uint64_t           requestID;
  LGNetCoreErrorCode code;
  LGNetService       service;
  uint16_t           messageType;
  uint32_t           detail;
  uint32_t           retryAfterMs;
  uint16_t           textLength;
  const uint8_t *    text;
}
LGNetCoreError;

typedef uint32_t LGNetRecoveryCapabilities;

enum
{
  LG_NET_RECOVERY_CAP_DISPLAY = 1U << 0,
};

typedef uint32_t LGNetRecoveryState;

enum
{
  LG_NET_RECOVERY_STATE_UNKNOWN   = 0,
  LG_NET_RECOVERY_STATE_NORMAL    = 1,
  LG_NET_RECOVERY_STATE_SWITCHING = 2,
  LG_NET_RECOVERY_STATE_ACTIVE    = 3,
  LG_NET_RECOVERY_STATE_FAILED    = 4,
};

typedef uint32_t LGNetRecoveryError;

enum
{
  LG_NET_RECOVERY_ERROR_NONE                = 0,
  LG_NET_RECOVERY_ERROR_UNSUPPORTED         = 1,
  LG_NET_RECOVERY_ERROR_HELPER_UNAVAILABLE  = 2,
  LG_NET_RECOVERY_ERROR_TOPOLOGY_FAILED     = 3,
  LG_NET_RECOVERY_ERROR_NO_FALLBACK_DISPLAY = 4,
  LG_NET_RECOVERY_ERROR_BUSY                = 5,
  LG_NET_RECOVERY_ERROR_CAPACITY            = 6,
  LG_NET_RECOVERY_ERROR_TIMEOUT             = 7,
};

typedef uint32_t LGNetRecoveryFlags;

enum
{
  LG_NET_RECOVERY_SUPPORTED        = 1U << 0,
  LG_NET_RECOVERY_ACTIVE           = 1U << 1,
  LG_NET_RECOVERY_HELPER_AVAILABLE = 1U << 2,
  LG_NET_RECOVERY_DISPLAY_PRESENT  = 1U << 3,
};

/* LG_NET_RECOVERY_MESSAGE_GET_INFO has a zero-length payload.
 * LG_NET_RECOVERY_MESSAGE_INFO uses the payload below; version is UTF-8
 * without a trailing NUL. */
typedef struct LGNetRecoveryInfo
{
  uint64_t                  sessionID;
  uint64_t                  statusSequence;
  LGNetRecoveryCapabilities capabilities;
  LGNetRecoveryState        state;
  LGNetRecoveryError        error;
  LGNetRecoveryFlags        flags;
  uint32_t                  maxTransitionMs;
  uint16_t                  versionLength;
  const uint8_t *           version;
}
LGNetRecoveryInfo;

typedef uint32_t LGNetRecoveryRequestFlags;

enum
{
  LG_NET_RECOVERY_REQUEST_ACTIVE = 1U << 0,
  LG_NET_RECOVERY_REQUEST_FORCE  = 1U << 1,
};

/* LG_NET_RECOVERY_MESSAGE_REQUEST. ACTIVE selects recovery mode; clearing it
 * requests a return to normal mode. */
typedef struct LGNetRecoveryRequest
{
  uint64_t                  requestID;
  uint64_t                  sessionID;
  uint64_t                  expectedStatusSequence;
  uint32_t                  timeoutMs;
  LGNetRecoveryRequestFlags flags;
}
LGNetRecoveryRequest;

/* LG_NET_RECOVERY_MESSAGE_STATUS. A zero requestID identifies an unsolicited
 * state update. */
typedef struct LGNetRecoveryStatus
{
  uint64_t           requestID;
  uint64_t           sessionID;
  uint64_t           statusSequence;
  LGNetRecoveryState state;
  LGNetRecoveryError error;
  LGNetRecoveryFlags flags;
  uint32_t           detail;
}
LGNetRecoveryStatus;

typedef uint16_t LGNetVideoCodec;

enum
{
  LG_NET_VIDEO_CODEC_PYROWAVE = 1,
};

typedef uint16_t LGNetVideoPixelFormat;

/* Values 5 and 6 remain reserved. The PyroWave profile supports 4:2:0 and
 * 4:4:4 chroma only. */
enum
{
  LG_NET_VIDEO_PIXEL_FORMAT_NV12       = 1,
  LG_NET_VIDEO_PIXEL_FORMAT_P010       = 2,
  LG_NET_VIDEO_PIXEL_FORMAT_YUV420P8   = 3,
  LG_NET_VIDEO_PIXEL_FORMAT_YUV420P10  = 4,
  LG_NET_VIDEO_PIXEL_FORMAT_YUV444P8   = 7,
  LG_NET_VIDEO_PIXEL_FORMAT_YUV444P10  = 8,
  LG_NET_VIDEO_PIXEL_FORMAT_YUV444P16F = 9,
};

typedef uint8_t LGNetVideoChromaSubsampling;

/* Value 2 remains reserved for a future codec profile. */
enum
{
  LG_NET_VIDEO_CHROMA_420 = 1,
  LG_NET_VIDEO_CHROMA_444 = 3,
};

typedef uint8_t LGNetColorPrimaries;

enum
{
  LG_NET_COLOR_PRIMARIES_BT601  = 1,
  LG_NET_COLOR_PRIMARIES_BT709  = 2,
  LG_NET_COLOR_PRIMARIES_BT2020 = 3,
};

typedef uint8_t LGNetColorTransfer;

enum
{
  LG_NET_COLOR_TRANSFER_LINEAR = 1,
  LG_NET_COLOR_TRANSFER_SRGB   = 2,
  LG_NET_COLOR_TRANSFER_BT709  = 3,
  LG_NET_COLOR_TRANSFER_PQ     = 4,
  LG_NET_COLOR_TRANSFER_HLG    = 5,
};

typedef uint8_t LGNetColorMatrix;

enum
{
  LG_NET_COLOR_MATRIX_BT601  = 1,
  LG_NET_COLOR_MATRIX_BT709  = 2,
  LG_NET_COLOR_MATRIX_BT2020 = 3,
  LG_NET_COLOR_MATRIX_RGB    = 4,
};

typedef uint8_t LGNetColorRange;

enum
{
  LG_NET_COLOR_RANGE_LIMITED = 1,
  LG_NET_COLOR_RANGE_FULL    = 2,
};

typedef uint16_t LGNetVideoSubscribeFlags;

enum
{
  LG_NET_VIDEO_SUBSCRIBE_ALLOW_DATAGRAMS = 1U << 0,
  LG_NET_VIDEO_SUBSCRIBE_REQUIRE_HDR     = 1U << 1,
  LG_NET_VIDEO_SUBSCRIBE_LOW_LATENCY     = 1U << 2,
};

/* LG_NET_VIDEO_MESSAGE_SUBSCRIBE. A zero preferredCodec permits any codec
 * negotiated for the video service. */
typedef struct LGNetVideoSubscribe
{
  uint64_t                 subscriptionID;
  LGNetVideoCodec          preferredCodec;
  LGNetVideoSubscribeFlags flags;
  uint32_t                 maxWidth;
  uint32_t                 maxHeight;
  uint32_t                 maxFrameLength;
  uint32_t                 maxFrameLatencyMs;
  uint32_t                 maxFrameRate;
}
LGNetVideoSubscribe;

typedef uint32_t LGNetVideoControlReason;

enum
{
  LG_NET_VIDEO_CONTROL_USER        = 1,
  LG_NET_VIDEO_CONTROL_RECOVERY    = 2,
  LG_NET_VIDEO_CONTROL_STALLED     = 3,
  LG_NET_VIDEO_CONTROL_CORRUPT     = 4,
  LG_NET_VIDEO_CONTROL_RECONFIGURE = 5,
};

/* LG_NET_VIDEO_MESSAGE_UNSUBSCRIBE and
 * LG_NET_VIDEO_MESSAGE_KEYFRAME_REQUEST. configEpoch and frameID may be zero
 * when the request does not target a particular configuration or frame. */
typedef struct LGNetVideoControl
{
  uint32_t                streamID;
  LGNetVideoControlReason reason;
  uint64_t                configEpoch;
  uint64_t                frameID;
}
LGNetVideoControl;

typedef uint16_t LGNetVideoStreamFlags;

enum
{
  LG_NET_VIDEO_STREAM_HDR              = 1U << 0,
  LG_NET_VIDEO_STREAM_RESERVED_ALPHA   = 1U << 1,
  LG_NET_VIDEO_STREAM_GPU_PLANES_ONLY  = 1U << 2,
  LG_NET_VIDEO_STREAM_DATAGRAMS        = 1U << 3,
};

typedef struct LGNetVideoStreamConfig
{
  uint32_t                    streamID;
  LGNetVideoCodec             codec;
  uint16_t                    codecVersion;
  uint64_t                    configEpoch;
  uint32_t                    width;
  uint32_t                    height;
  uint32_t                    refreshNumerator;
  uint32_t                    refreshDenominator;
  LGNetVideoPixelFormat       pixelFormat;
  LGNetVideoChromaSubsampling chromaSubsampling;
  LGNetColorPrimaries         colorPrimaries;
  LGNetColorTransfer          colorTransfer;
  LGNetColorMatrix            colorMatrix;
  LGNetColorRange             colorRange;
  uint8_t                     planeCount;
  uint8_t                     bitDepth;
  LGNetVideoStreamFlags       flags;
  uint32_t                    maxFrameLength;
  uint32_t                    maxFragmentLength;
  uint32_t                    maxFrameLatencyMs;
}
LGNetVideoStreamConfig;

typedef uint16_t LGNetVideoFrameFlags;

enum
{
  LG_NET_VIDEO_FRAME_KEYFRAME       = 1U << 0,
  LG_NET_VIDEO_FRAME_DISCONTINUITY  = 1U << 1,
  LG_NET_VIDEO_FRAME_END_OF_STREAM  = 1U << 2,
  LG_NET_VIDEO_FRAME_HAS_CHECKSUM   = 1U << 3,
};

/* checksum is CRC-32C (Castagnoli), reflected polynomial 0x82f63b78, with
 * initial and final XOR 0xffffffff, over exactly encodedLength bytes of data.
 * It is zero when LG_NET_VIDEO_FRAME_HAS_CHECKSUM is clear. */
typedef struct LGNetVideoFrame
{
  uint32_t             streamID;
  LGNetVideoCodec      codec;
  LGNetVideoFrameFlags flags;
  uint64_t             configEpoch;
  uint64_t             frameID;
  uint64_t             captureTimestampNs;
  uint64_t             presentationTimestampNs;
  uint32_t             encodedLength;
  uint32_t             blockCount;
  uint32_t             fragmentCount;
  uint32_t             deadlineMs;
  uint32_t             checksum;
  const uint8_t *      data;
}
LGNetVideoFrame;

typedef uint16_t LGNetVideoFragmentFlags;

enum
{
  LG_NET_VIDEO_FRAGMENT_BLOCK_START = 1U << 0,
  LG_NET_VIDEO_FRAGMENT_BLOCK_END   = 1U << 1,
  LG_NET_VIDEO_FRAGMENT_FRAME_END   = 1U << 2,
  LG_NET_VIDEO_FRAGMENT_RECOVERY    = 1U << 3,
  LG_NET_VIDEO_FRAGMENT_CHECKSUM    = 1U << 4,
};

typedef uint16_t LGNetVideoRecoveryType;

enum
{
  LG_NET_VIDEO_RECOVERY_NONE            = 0,
  LG_NET_VIDEO_RECOVERY_DUPLICATE       = 1,
  LG_NET_VIDEO_RECOVERY_RESERVED_PARITY = 2,
};

/* One encoded codec block may span multiple fragments. frameOffset locates
 * payload in the complete encoded frame while blockOffset locates the same
 * bytes within blockIndex. Version 2 supports exact duplicate recovery
 * fragments only; the parity value is reserved for a future wire version.
 * recoveryType and recoveryGroup are non-zero for recovery fragments;
 * recoveryIndex/recoveryCount identify that fragment's position in the
 * recovery group. checksum is CRC-32C over payloadLength bytes and is zero
 * unless LG_NET_VIDEO_FRAGMENT_CHECKSUM is set. */
typedef struct LGNetVideoFragment
{
  uint32_t                streamID;
  LGNetVideoFragmentFlags flags;
  LGNetVideoRecoveryType  recoveryType;
  uint64_t                configEpoch;
  uint64_t                frameID;
  uint32_t                frameOffset;
  uint32_t                frameLength;
  uint32_t                blockIndex;
  uint32_t                blockOffset;
  uint32_t                blockLength;
  uint32_t                blockCount;
  uint32_t                fragmentIndex;
  uint32_t                fragmentCount;
  uint32_t                recoveryGroup;
  uint16_t                recoveryIndex;
  uint16_t                recoveryCount;
  uint32_t                payloadLength;
  uint32_t                checksum;
  const uint8_t *         payload;
}
LGNetVideoFragment;

typedef uint32_t LGNetVideoFeedbackFlags;

enum
{
  LG_NET_VIDEO_FEEDBACK_REQUEST_KEYFRAME = 1U << 0,
  LG_NET_VIDEO_FEEDBACK_CONGESTED        = 1U << 1,
  LG_NET_VIDEO_FEEDBACK_DECODER_STALLED  = 1U << 2,
};

typedef struct LGNetVideoFeedback
{
  uint32_t                streamID;
  LGNetVideoFeedbackFlags flags;
  uint64_t                lastCompleteFrameID;
  uint64_t                highestSeenFrameID;
  uint64_t                lastDecodedFrameID;
  uint32_t                lostFrames;
  uint32_t                lostFragments;
  uint32_t                receiveQueueUs;
  uint32_t                decodeQueueUs;
  uint32_t                roundTripUs;
}
LGNetVideoFeedback;

typedef uint32_t LGNetVideoScheduleFlags;

enum
{
  LG_NET_VIDEO_SCHEDULE_PRESENT          = 1U << 0,
  LG_NET_VIDEO_SCHEDULE_DROP_IF_LATE     = 1U << 1,
  LG_NET_VIDEO_SCHEDULE_REPEAT_PREVIOUS  = 1U << 2,
  LG_NET_VIDEO_SCHEDULE_REQUEST_KEYFRAME = 1U << 3,
};

/* LG_NET_VIDEO_MESSAGE_SCHEDULE. deadlineTimestampNs is the latest useful
 * arrival time in the sender's timestamp domain. */
typedef struct LGNetVideoSchedule
{
  uint32_t                streamID;
  LGNetVideoScheduleFlags flags;
  uint64_t                configEpoch;
  uint64_t                frameID;
  uint64_t                captureTimestampNs;
  uint64_t                presentationTimestampNs;
  uint64_t                deadlineTimestampNs;
}
LGNetVideoSchedule;

typedef uint16_t LGNetVideoState;

enum
{
  LG_NET_VIDEO_STATE_SUBSCRIBED = 1,
  LG_NET_VIDEO_STATE_CONFIGURED = 2,
  LG_NET_VIDEO_STATE_STREAMING  = 3,
  LG_NET_VIDEO_STATE_STALLED    = 4,
  LG_NET_VIDEO_STATE_STOPPED    = 5,
  LG_NET_VIDEO_STATE_ERROR      = 6,
};

typedef uint32_t LGNetVideoStatusFlags;

enum
{
  LG_NET_VIDEO_STATUS_KEYFRAME_PENDING = 1U << 0,
  LG_NET_VIDEO_STATUS_CONGESTED        = 1U << 1,
  LG_NET_VIDEO_STATUS_RECONFIGURING    = 1U << 2,
};

/* LG_NET_VIDEO_MESSAGE_STATUS. */
typedef struct LGNetVideoStatus
{
  uint32_t              streamID;
  LGNetVideoState       state;
  uint64_t              configEpoch;
  uint64_t              lastFrameID;
  uint64_t              statusSequence;
  LGNetVideoStatusFlags flags;
  uint32_t              detail;
}
LGNetVideoStatus;

typedef uint32_t LGNetCursorPositionFlags;

enum
{
  LG_NET_CURSOR_POSITION_VISIBLE  = 1U << 0,
  LG_NET_CURSOR_POSITION_RELATIVE = 1U << 1,
};

typedef struct LGNetCursorPosition
{
  uint64_t                 updateID;
  uint64_t                 timestampNs;
  int32_t                  x;
  int32_t                  y;
  uint32_t                 desktopWidth;
  uint32_t                 desktopHeight;
  LGNetCursorPositionFlags flags;
  uint32_t                 displayID;
}
LGNetCursorPosition;

typedef uint16_t LGNetCursorFormat;

enum
{
  LG_NET_CURSOR_FORMAT_BGRA8_PREMULTIPLIED = 1,
  LG_NET_CURSOR_FORMAT_RGBA8_PREMULTIPLIED = 2,
  LG_NET_CURSOR_FORMAT_MASKED_COLOR        = 3,
  LG_NET_CURSOR_FORMAT_MONOCHROME          = 4,
};

typedef uint16_t LGNetCursorShapeFlags;

enum
{
  LG_NET_CURSOR_SHAPE_HIDDEN   = 1U << 0,
  LG_NET_CURSOR_SHAPE_ANIMATED = 1U << 1,
};

typedef struct LGNetCursorShape
{
  uint64_t              shapeID;
  uint32_t              width;
  uint32_t              height;
  uint32_t              pitch;
  uint32_t              dataLength;
  LGNetCursorFormat     format;
  LGNetCursorShapeFlags flags;
  uint32_t              hotspotX;
  uint32_t              hotspotY;
  uint32_t              frameDurationMs;
  const uint8_t *       data;
}
LGNetCursorShape;

typedef uint32_t LGNetCursorStateFlags;

enum
{
  LG_NET_CURSOR_STATE_VISIBLE         = 1U << 0,
  LG_NET_CURSOR_STATE_SHAPE_VALID     = 1U << 1,
  LG_NET_CURSOR_STATE_POSITION_VALID  = 1U << 2,
  LG_NET_CURSOR_STATE_TRANSFORM_VALID = 1U << 3,
};

/* LG_NET_CURSOR_MESSAGE_STATE. Each referenced ID is zero when its matching
 * valid flag is clear. */
typedef struct LGNetCursorState
{
  uint64_t              stateSequence;
  uint64_t              shapeID;
  uint64_t              positionID;
  uint64_t              transformID;
  LGNetCursorStateFlags flags;
  uint32_t              displayID;
}
LGNetCursorState;

typedef uint16_t LGNetCursorRotation;

enum
{
  LG_NET_CURSOR_ROTATION_0   = 1,
  LG_NET_CURSOR_ROTATION_90  = 2,
  LG_NET_CURSOR_ROTATION_180 = 3,
  LG_NET_CURSOR_ROTATION_270 = 4,
};

typedef uint16_t LGNetCursorTransformFlags;

enum
{
  LG_NET_CURSOR_TRANSFORM_MIRROR_X = 1U << 0,
  LG_NET_CURSOR_TRANSFORM_MIRROR_Y = 1U << 1,
};

/* LG_NET_CURSOR_MESSAGE_TRANSFORM. scaleNumerator/scaleDenominator applies
 * after rotation and before the signed target-space offset. */
typedef struct LGNetCursorTransform
{
  uint64_t                  transformID;
  uint32_t                  displayID;
  LGNetCursorRotation       rotation;
  LGNetCursorTransformFlags flags;
  uint32_t                  sourceWidth;
  uint32_t                  sourceHeight;
  uint32_t                  targetWidth;
  uint32_t                  targetHeight;
  int32_t                   offsetX;
  int32_t                   offsetY;
  uint32_t                  scaleNumerator;
  uint32_t                  scaleDenominator;
}
LGNetCursorTransform;

typedef uint32_t LGNetCursorColorTransformFlags;

enum
{
  LG_NET_CURSOR_COLOR_TRANSFORM_MATRIX = 1U << 0,
  LG_NET_CURSOR_COLOR_TRANSFORM_LUT    = 1U << 1,
};

/* LG_NET_CURSOR_MESSAGE_COLOR_TRANSFORM. Matrix is a row-major 3x4 matrix.
 * The optional LUT is 4096 RGBA float entries in entry-major order. Float
 * values are encoded as their IEEE-754 binary32 bit patterns in little-endian
 * order. An update with no transform flags still carries the cursor SDR white
 * level. */
typedef struct LGNetCursorColorTransform
{
  uint64_t                       updateID;
  LGNetCursorColorTransformFlags flags;
  uint32_t                       sdrWhiteLevel;
  float                          matrix[LG_NET_CURSOR_COLOR_MATRIX_FLOATS];
  float                          scalar;
  const uint8_t                * lut;
}
LGNetCursorColorTransform;

typedef uint32_t LGNetCursorStatusCode;

enum
{
  LG_NET_CURSOR_STATUS_APPLIED     = 1,
  LG_NET_CURSOR_STATUS_UNSUPPORTED = 2,
  LG_NET_CURSOR_STATUS_INVALID     = 3,
  LG_NET_CURSOR_STATUS_ERROR       = 4,
};

/* LG_NET_CURSOR_MESSAGE_STATUS. */
typedef struct LGNetCursorStatus
{
  uint64_t              stateSequence;
  uint64_t              appliedShapeID;
  uint64_t              appliedPositionID;
  uint64_t              appliedTransformID;
  LGNetCursorStatusCode status;
  uint32_t              detail;
}
LGNetCursorStatus;

typedef uint32_t LGNetControlFrameScheduleFlags;

enum
{
  LG_NET_CONTROL_FRAME_SCHEDULE_ACTIVE    = 1U << 0,
  LG_NET_CONTROL_FRAME_SCHEDULE_RELEASE   = 1U << 1,
  LG_NET_CONTROL_FRAME_SCHEDULE_RESET     = 1U << 2,
  LG_NET_CONTROL_FRAME_SCHEDULE_IMMEDIATE = 1U << 3,
};

/* Client-to-server control requests. controlID must match the envelope
 * request ID and is returned by LGNetControlStatus. */
typedef struct LGNetControlCursorPosition
{
  uint64_t controlID;
  int32_t  x;
  int32_t  y;
}
LGNetControlCursorPosition;

typedef struct LGNetControlDisplaySize
{
  uint64_t controlID;
  uint32_t width;
  uint32_t height;
}
LGNetControlDisplaySize;

typedef struct LGNetControlFrameSchedule
{
  uint64_t                       controlID;
  uint32_t                       generation;
  LGNetControlFrameScheduleFlags flags;
  uint64_t                       periodNs;
  uint64_t                       targetSlackNs;
  int64_t                        phaseErrorNs;
  uint32_t                       feedbackFrameSerial;
  uint32_t                       feedbackScheduleEpoch;
  uint32_t                       feedbackDeadlineSerial;
  uint32_t                       leaseMs;
}
LGNetControlFrameSchedule;

typedef uint32_t LGNetControlStatusCode;

enum
{
  LG_NET_CONTROL_STATUS_APPLIED     = 1,
  LG_NET_CONTROL_STATUS_PENDING     = 2,
  LG_NET_CONTROL_STATUS_BUSY        = 3,
  LG_NET_CONTROL_STATUS_UNSUPPORTED = 4,
  LG_NET_CONTROL_STATUS_STALE       = 5,
  LG_NET_CONTROL_STATUS_INVALID     = 6,
  LG_NET_CONTROL_STATUS_ERROR       = 7,
};

typedef struct LGNetControlStatus
{
  uint64_t               controlID;
  LGNetControlStatusCode status;
  uint32_t               detail;
}
LGNetControlStatus;

typedef uint32_t LGNetInputClaimFlags;

enum
{
  LG_NET_INPUT_CLAIM_KEYBOARD  = 1U << 0,
  LG_NET_INPUT_CLAIM_POINTER   = 1U << 1,
  LG_NET_INPUT_CLAIM_EXCLUSIVE = 1U << 2,
};

typedef struct LGNetInputClaim
{
  uint64_t             claimantID;
  uint64_t             claimEpoch;
  uint32_t             leaseMs;
  LGNetInputClaimFlags flags;
}
LGNetInputClaim;

typedef uint32_t LGNetInputStatusCode;

enum
{
  LG_NET_INPUT_STATUS_ACCEPTED    = 1,
  LG_NET_INPUT_STATUS_REJECTED    = 2,
  LG_NET_INPUT_STATUS_RELEASED    = 3,
  LG_NET_INPUT_STATUS_EXPIRED     = 4,
  LG_NET_INPUT_STATUS_UNSUPPORTED = 5,
  LG_NET_INPUT_STATUS_ERROR       = 6,
};

/* LG_NET_INPUT_MESSAGE_CLAIM_RESULT and LG_NET_INPUT_MESSAGE_STATUS. */
typedef struct LGNetInputStatus
{
  uint64_t             claimEpoch;
  uint64_t             lastSequence;
  LGNetInputStatusCode status;
  LGNetInputClaimFlags flags;
  uint32_t             leaseRemainingMs;
  uint32_t             detail;
}
LGNetInputStatus;

typedef uint32_t LGNetPointerButtons;

enum
{
  LG_NET_POINTER_BUTTON_LEFT    = 1U << 0,
  LG_NET_POINTER_BUTTON_RIGHT   = 1U << 1,
  LG_NET_POINTER_BUTTON_MIDDLE  = 1U << 2,
  LG_NET_POINTER_BUTTON_BACK    = 1U << 3,
  LG_NET_POINTER_BUTTON_FORWARD = 1U << 4,
  LG_NET_POINTER_BUTTON_6       = 1U << 5,
  LG_NET_POINTER_BUTTON_7       = 1U << 6,
  LG_NET_POINTER_BUTTON_8       = 1U << 7,
  LG_NET_POINTER_BUTTON_9       = 1U << 8,
  LG_NET_POINTER_BUTTON_10      = 1U << 9,
  LG_NET_POINTER_BUTTON_11      = 1U << 10,
  LG_NET_POINTER_BUTTON_12      = 1U << 11,
  LG_NET_POINTER_BUTTON_13      = 1U << 12,
  LG_NET_POINTER_BUTTON_14      = 1U << 13,
  LG_NET_POINTER_BUTTON_15      = 1U << 14,
  LG_NET_POINTER_BUTTON_16      = 1U << 15,
  LG_NET_POINTER_BUTTON_17      = 1U << 16,
  LG_NET_POINTER_BUTTON_18      = 1U << 17,
  LG_NET_POINTER_BUTTON_19      = 1U << 18,
  LG_NET_POINTER_BUTTON_20      = 1U << 19,
  LG_NET_POINTER_BUTTON_21      = 1U << 20,
  LG_NET_POINTER_BUTTON_22      = 1U << 21,
  LG_NET_POINTER_BUTTON_23      = 1U << 22,
  LG_NET_POINTER_BUTTON_24      = 1U << 23,
  LG_NET_POINTER_BUTTON_25      = 1U << 24,
  LG_NET_POINTER_BUTTON_26      = 1U << 25,
  LG_NET_POINTER_BUTTON_27      = 1U << 26,
  LG_NET_POINTER_BUTTON_28      = 1U << 27,
  LG_NET_POINTER_BUTTON_29      = 1U << 28,
  LG_NET_POINTER_BUTTON_30      = 1U << 29,
  LG_NET_POINTER_BUTTON_31      = 1U << 30,
  LG_NET_POINTER_BUTTON_32      = 1U << 31,
};

/* LG_NET_INPUT_MESSAGE_KEEPALIVE, LG_NET_INPUT_MESSAGE_RELEASE and
 * LG_NET_INPUT_MESSAGE_RESET. For RESET, flags identifies the input classes
 * whose held state must be cleared. */
typedef struct LGNetInputControl
{
  uint64_t             claimantID;
  uint64_t             claimEpoch;
  uint64_t             sequence;
  LGNetInputClaimFlags flags;
  uint32_t             leaseMs;
}
LGNetInputControl;

/* Mouse payloads carry the complete post-event button state in buttons and
 * the transition mask in changedButtons, including button-only events where
 * every motion and wheel field is zero. */
typedef struct LGNetInputRelative
{
  uint64_t            sequence;
  uint64_t            timestampNs;
  int32_t             deltaX;
  int32_t             deltaY;
  int32_t             wheelX;
  int32_t             wheelY;
  LGNetPointerButtons buttons;
  LGNetPointerButtons changedButtons;
}
LGNetInputRelative;

typedef struct LGNetInputAbsolute
{
  uint64_t            sequence;
  uint64_t            timestampNs;
  int32_t             x;
  int32_t             y;
  uint32_t            width;
  uint32_t            height;
  int32_t             wheelX;
  int32_t             wheelY;
  LGNetPointerButtons buttons;
  LGNetPointerButtons changedButtons;
}
LGNetInputAbsolute;

typedef uint16_t LGNetKeyboardFlags;

enum
{
  LG_NET_KEYBOARD_DOWN     = 1U << 0,
  LG_NET_KEYBOARD_REPEAT   = 1U << 1,
  LG_NET_KEYBOARD_EXTENDED = 1U << 2,
  LG_NET_KEYBOARD_E1       = 1U << 3,
};

/* modifiers uses the USB HID keyboard modifier byte in its low eight bits;
 * all other bits are reserved and must be zero. */
typedef struct LGNetInputKeyboard
{
  uint64_t           sequence;
  uint64_t           timestampNs;
  uint16_t           usagePage;
  uint16_t           usage;
  uint16_t           scanCode;
  LGNetKeyboardFlags flags;
  uint32_t           modifiers;
}
LGNetInputKeyboard;

typedef uint32_t LGNetKeyboardLEDs;

enum
{
  LG_NET_KEYBOARD_LED_NUM_LOCK    = 1U << 0,
  LG_NET_KEYBOARD_LED_CAPS_LOCK   = 1U << 1,
  LG_NET_KEYBOARD_LED_SCROLL_LOCK = 1U << 2,
  LG_NET_KEYBOARD_LED_COMPOSE     = 1U << 3,
  LG_NET_KEYBOARD_LED_KANA        = 1U << 4,
};

/* LG_NET_INPUT_MESSAGE_KEYBOARD_LEDS. validMask identifies the LEDs whose
 * state is authoritative in this update. */
typedef struct LGNetInputLEDs
{
  uint64_t          sequence;
  LGNetKeyboardLEDs state;
  LGNetKeyboardLEDs validMask;
}
LGNetInputLEDs;

typedef uint16_t LGNetAudioDirection;

enum
{
  LG_NET_AUDIO_DIRECTION_PLAYBACK = 1,
  LG_NET_AUDIO_DIRECTION_CAPTURE  = 2,
};

typedef uint32_t LGNetAudioDirectionMask;

enum
{
  LG_NET_AUDIO_DIRECTIONS_PLAYBACK = 1U << 0,
  LG_NET_AUDIO_DIRECTIONS_CAPTURE  = 1U << 1,
};

typedef uint32_t LGNetAudioSubscribeFlags;

enum
{
  LG_NET_AUDIO_SUBSCRIBE_LOW_LATENCY = 1U << 0,
  LG_NET_AUDIO_SUBSCRIBE_EXCLUSIVE   = 1U << 1,
};

/* LG_NET_AUDIO_MESSAGE_SUBSCRIBE. */
typedef struct LGNetAudioSubscribe
{
  uint64_t                 subscriberID;
  LGNetAudioDirectionMask  directions;
  uint32_t                 targetLatencyUs;
  uint32_t                 maxPacketFrames;
  LGNetAudioSubscribeFlags flags;
}
LGNetAudioSubscribe;

typedef uint16_t LGNetAudioSampleFormat;

enum
{
  LG_NET_AUDIO_SAMPLE_S16_LE = 1,
  LG_NET_AUDIO_SAMPLE_S24_LE = 2,
  LG_NET_AUDIO_SAMPLE_S32_LE = 3,
  LG_NET_AUDIO_SAMPLE_F32_LE = 4,
};

typedef uint32_t LGNetAudioFormatFlags;

enum
{
  LG_NET_AUDIO_FORMAT_INTERLEAVED = 1U << 0,
};

typedef struct LGNetAudioFormat
{
  uint32_t               streamID;
  LGNetAudioDirection    direction;
  LGNetAudioSampleFormat sampleFormat;
  uint64_t               formatEpoch;
  uint32_t               sampleRate;
  uint16_t               channels;
  uint16_t               framesPerPacket;
  uint32_t               channelMask;
  LGNetAudioFormatFlags  flags;
  uint32_t               maxPacketFrames;
}
LGNetAudioFormat;

typedef uint16_t LGNetAudioDataFlags;

enum
{
  LG_NET_AUDIO_DATA_DISCONTINUITY = 1U << 0,
  LG_NET_AUDIO_DATA_SILENT        = 1U << 1,
  LG_NET_AUDIO_DATA_END_OF_STREAM = 1U << 2,
};

typedef struct LGNetAudioData
{
  uint32_t            streamID;
  LGNetAudioDirection direction;
  LGNetAudioDataFlags flags;
  uint64_t            formatEpoch;
  uint64_t            packetID;
  uint64_t            timestampNs;
  uint64_t            startFrame;
  uint32_t            frameCount;
  uint32_t            dataLength;
  const uint8_t *     data;
}
LGNetAudioData;

typedef uint16_t LGNetAudioStateCode;

enum
{
  LG_NET_AUDIO_STATE_STOPPED = 1,
  LG_NET_AUDIO_STATE_RUNNING = 2,
  LG_NET_AUDIO_STATE_PAUSED  = 3,
  LG_NET_AUDIO_STATE_ERROR   = 4,
};

typedef uint32_t LGNetAudioStateFlags;

enum
{
  LG_NET_AUDIO_STATE_MUTED         = 1U << 0,
  LG_NET_AUDIO_STATE_FORMAT_ACTIVE = 1U << 1,
};

typedef struct LGNetAudioState
{
  uint32_t             streamID;
  LGNetAudioDirection  direction;
  LGNetAudioStateCode  state;
  uint64_t             stateSequence;
  uint64_t             formatEpoch;
  int32_t              volumeMillibels;
  LGNetAudioStateFlags flags;
}
LGNetAudioState;

typedef struct LGNetAudioClockFeedback
{
  uint32_t            streamID;
  LGNetAudioDirection direction;
  uint64_t            formatEpoch;
  uint64_t            packetID;
  uint64_t            framePosition;
  uint64_t            timestampNs;
  int32_t             queuedFrames;
  int32_t             driftPpm;
}
LGNetAudioClockFeedback;

typedef uint16_t LGNetAudioControlFlags;

enum
{
  LG_NET_AUDIO_CONTROL_GRACEFUL = 1U << 0,
  LG_NET_AUDIO_CONTROL_FLUSH    = 1U << 1,
};

/* LG_NET_AUDIO_MESSAGE_PLAYBACK_STOP, LG_NET_AUDIO_MESSAGE_CAPTURE_STOP,
 * LG_NET_AUDIO_MESSAGE_KEEPALIVE and LG_NET_AUDIO_MESSAGE_RELEASE. The start
 * messages use LGNetAudioFormat as their payload. */
typedef struct LGNetAudioControl
{
  uint32_t               streamID;
  LGNetAudioDirection    direction;
  LGNetAudioControlFlags flags;
  uint64_t               sequence;
  uint64_t               formatEpoch;
  uint32_t               detail;
}
LGNetAudioControl;

/* LG_NET_AUDIO_MESSAGE_PLAYBACK_VOLUME and
 * LG_NET_AUDIO_MESSAGE_CAPTURE_VOLUME. Values are signed millibels. A zero
 * channelMask means the first channelCount channels in stream order; a
 * non-zero mask must contain exactly channelCount bits and values follow its
 * set bits in ascending bit order. */
typedef struct LGNetAudioVolume
{
  uint32_t            streamID;
  LGNetAudioDirection direction;
  uint16_t            channelCount;
  uint64_t            sequence;
  uint32_t            channelMask;
  int32_t             volumeMillibels[LG_NET_AUDIO_MAX_CHANNELS];
}
LGNetAudioVolume;

/* LG_NET_AUDIO_MESSAGE_PLAYBACK_MUTE and
 * LG_NET_AUDIO_MESSAGE_CAPTURE_MUTE. A zero channelMask selects the master
 * mute control; otherwise each set bit selects an affected channel. */
typedef struct LGNetAudioMute
{
  uint32_t            streamID;
  LGNetAudioDirection direction;
  uint8_t             muted;
  uint64_t            sequence;
  uint32_t            channelMask;
}
LGNetAudioMute;

/* LG_NET_AUDIO_MESSAGE_STATE_BARRIER and LG_NET_AUDIO_MESSAGE_STATE_ACK. */
typedef struct LGNetAudioBarrier
{
  uint32_t            streamID;
  LGNetAudioDirection direction;
  uint64_t            barrierID;
  uint64_t            stateSequence;
}
LGNetAudioBarrier;

typedef uint32_t LGNetClipboardClaimFlags;

enum
{
  LG_NET_CLIPBOARD_CLAIM_READ  = 1U << 0,
  LG_NET_CLIPBOARD_CLAIM_WRITE = 1U << 1,
};

typedef struct LGNetClipboardClaim
{
  uint64_t                 ownerID;
  uint64_t                 claimEpoch;
  uint32_t                 leaseMs;
  LGNetClipboardClaimFlags flags;
}
LGNetClipboardClaim;

/* LG_NET_CLIPBOARD_MESSAGE_KEEPALIVE, LG_NET_CLIPBOARD_MESSAGE_RELEASE and
 * LG_NET_CLIPBOARD_MESSAGE_CLEAR. A zero offerID clears all offers owned by
 * this claim; otherwise it targets one offer. */
typedef struct LGNetClipboardControl
{
  uint64_t ownerID;
  uint64_t claimEpoch;
  uint64_t serial;
  uint64_t offerID;
}
LGNetClipboardControl;

typedef uint32_t LGNetClipboardClaimCode;

enum
{
  LG_NET_CLIPBOARD_CLAIM_ACCEPTED = 1,
  LG_NET_CLIPBOARD_CLAIM_REJECTED = 2,
  LG_NET_CLIPBOARD_CLAIM_RELEASED = 3,
  LG_NET_CLIPBOARD_CLAIM_EXPIRED  = 4,
  LG_NET_CLIPBOARD_CLAIM_ERROR    = 5,
};

/* LG_NET_CLIPBOARD_MESSAGE_CLAIM_RESULT. */
typedef struct LGNetClipboardClaimStatus
{
  uint64_t                 claimEpoch;
  uint64_t                 serial;
  LGNetClipboardClaimCode  status;
  LGNetClipboardClaimFlags flags;
  uint32_t                 leaseRemainingMs;
  uint32_t                 detail;
}
LGNetClipboardClaimStatus;

typedef uint16_t LGNetClipboardOfferFlags;

enum
{
  LG_NET_CLIPBOARD_OFFER_LAZY      = 1U << 0,
  LG_NET_CLIPBOARD_OFFER_SENSITIVE = 1U << 1,
};

typedef struct LGNetClipboardOffer
{
  uint64_t                 offerID;
  uint64_t                 serial;
  uint64_t                 estimatedSize;
  uint32_t                 formatIndex;
  uint32_t                 formatCount;
  uint16_t                 mimeLength;
  LGNetClipboardOfferFlags flags;
  const uint8_t *          mime;
}
LGNetClipboardOffer;

typedef struct LGNetClipboardRequest
{
  uint64_t offerID;
  uint64_t requestID;
  uint32_t formatIndex;
  uint32_t maxChunkLength;
  uint64_t offset;
}
LGNetClipboardRequest;

typedef uint16_t LGNetClipboardChunkFlags;

enum
{
  LG_NET_CLIPBOARD_CHUNK_FIRST = 1U << 0,
  LG_NET_CLIPBOARD_CHUNK_FINAL = 1U << 1,
};

typedef struct LGNetClipboardChunk
{
  uint64_t                 requestID;
  uint64_t                 offerID;
  uint64_t                 offset;
  uint64_t                 totalLength;
  uint32_t                 dataLength;
  LGNetClipboardChunkFlags flags;
  const uint8_t *          data;
}
LGNetClipboardChunk;

/* LG_NET_CLIPBOARD_MESSAGE_DATA_BEGIN, DATA_END, DATA_READY and CANCEL.
 * processedLength is zero for BEGIN/READY and contains the terminal byte
 * count for END/CANCEL. */
typedef struct LGNetClipboardTransfer
{
  uint64_t requestID;
  uint64_t offerID;
  uint64_t totalLength;
  uint64_t processedLength;
}
LGNetClipboardTransfer;

typedef uint32_t LGNetClipboardStatusCode;

enum
{
  LG_NET_CLIPBOARD_STATUS_OK          = 1,
  LG_NET_CLIPBOARD_STATUS_NOT_FOUND   = 2,
  LG_NET_CLIPBOARD_STATUS_UNSUPPORTED = 3,
  LG_NET_CLIPBOARD_STATUS_CANCELLED   = 4,
  LG_NET_CLIPBOARD_STATUS_BUSY        = 5,
  LG_NET_CLIPBOARD_STATUS_ERROR       = 6,
};

typedef struct LGNetClipboardStatus
{
  uint64_t                 requestID;
  uint64_t                 offerID;
  LGNetClipboardStatusCode status;
  uint32_t                 detail;
  uint64_t                 processedLength;
}
LGNetClipboardStatus;

typedef uint32_t LGNetFileOfferFlags;

enum
{
  LG_NET_FILE_OFFER_COPY      = 1U << 0,
  LG_NET_FILE_OFFER_MOVE      = 1U << 1,
  LG_NET_FILE_OFFER_RECURSIVE = 1U << 2,
};

typedef struct LGNetFileOffer
{
  uint64_t            offerID;
  uint64_t            totalBytes;
  uint32_t            entryCount;
  LGNetFileOfferFlags flags;
  uint16_t            labelLength;
  const uint8_t *     label;
}
LGNetFileOffer;

typedef uint32_t LGNetFileLeaseFlags;

enum
{
  LG_NET_FILE_LEASE_READ      = 1U << 0,
  LG_NET_FILE_LEASE_EXCLUSIVE = 1U << 1,
  LG_NET_FILE_LEASE_ACQUIRED  = 1U << 2,
};

/* LG_NET_FILE_MESSAGE_ACQUIRE, ACQUIRED and RELEASE. ACQUIRE sets the desired
 * lease flags, ACQUIRED adds LG_NET_FILE_LEASE_ACQUIRED, and RELEASE carries
 * the previously issued leaseEpoch. */
typedef struct LGNetFileLease
{
  uint64_t            clientID;
  uint64_t            offerID;
  uint64_t            leaseEpoch;
  uint32_t            leaseMs;
  LGNetFileLeaseFlags flags;
}
LGNetFileLease;

typedef uint16_t LGNetFileEntryType;

enum
{
  LG_NET_FILE_ENTRY_REGULAR   = 1,
  LG_NET_FILE_ENTRY_DIRECTORY = 2,
  LG_NET_FILE_ENTRY_SYMLINK   = 3,
};

typedef uint16_t LGNetFileEntryFlags;

enum
{
  LG_NET_FILE_ENTRY_EXECUTABLE = 1U << 0,
  LG_NET_FILE_ENTRY_HIDDEN     = 1U << 1,
};

/* File path fields are strict UTF-8 without a trailing NUL. They are relative
 * to a receiver-selected transfer root and use '/' as their only separator.
 * Receivers must reject absolute paths, empty, '.' or '..' components, and
 * platform-native alternate separators before accessing the filesystem. The
 * same traversal rules apply to symbolic-link targets. */
typedef struct LGNetFileEntry
{
  uint64_t            offerID;
  uint64_t            entryID;
  uint64_t            parentID;
  uint64_t            size;
  uint64_t            modifiedNs;
  uint32_t            mode;
  LGNetFileEntryType  type;
  LGNetFileEntryFlags flags;
  uint16_t            pathLength;
  uint16_t            linkTargetLength;
  const uint8_t *     path;
  const uint8_t *     linkTarget;
}
LGNetFileEntry;

typedef uint32_t LGNetFileRequestFlags;

enum
{
  LG_NET_FILE_REQUEST_METADATA = 1U << 0,
  LG_NET_FILE_REQUEST_DATA     = 1U << 1,
};

typedef struct LGNetFileRequest
{
  uint64_t              requestID;
  uint64_t              offerID;
  uint64_t              entryID;
  uint64_t              offset;
  uint32_t              length;
  LGNetFileRequestFlags flags;
}
LGNetFileRequest;

typedef uint16_t LGNetFileChunkFlags;

enum
{
  LG_NET_FILE_CHUNK_FIRST = 1U << 0,
  LG_NET_FILE_CHUNK_FINAL = 1U << 1,
};

typedef struct LGNetFileChunk
{
  uint64_t            requestID;
  uint64_t            offerID;
  uint64_t            entryID;
  uint64_t            offset;
  uint64_t            totalLength;
  uint32_t            dataLength;
  LGNetFileChunkFlags flags;
  const uint8_t *     data;
}
LGNetFileChunk;

/* LG_NET_FILE_MESSAGE_DATA_BEGIN, DATA_END, DATA_READY and CANCEL.
 * processedLength is relative to offset and must fit within totalLength. */
typedef struct LGNetFileTransfer
{
  uint64_t requestID;
  uint64_t offerID;
  uint64_t entryID;
  uint64_t offset;
  uint64_t totalLength;
  uint64_t processedLength;
}
LGNetFileTransfer;

typedef uint32_t LGNetFileStatusCode;

enum
{
  LG_NET_FILE_STATUS_OK          = 1,
  LG_NET_FILE_STATUS_NOT_FOUND   = 2,
  LG_NET_FILE_STATUS_DENIED      = 3,
  LG_NET_FILE_STATUS_CANCELLED   = 4,
  LG_NET_FILE_STATUS_INVALID     = 5,
  LG_NET_FILE_STATUS_UNSUPPORTED = 6,
  LG_NET_FILE_STATUS_ERROR       = 7,
};

/* An entryID of zero identifies an offer-level ACQUIRE or RELEASE status. */
typedef struct LGNetFileStatus
{
  uint64_t            requestID;
  uint64_t            offerID;
  uint64_t            entryID;
  LGNetFileStatusCode status;
  uint32_t            detail;
  uint64_t            transferredLength;
}
LGNetFileStatus;

/* This header reserves framing only. It deliberately defines no USB
 * redirection operations or device semantics. */
typedef uint16_t LGNetUSBReservedFlags;

enum
{
  LG_NET_USB_RESERVED_RESPONSE = 1U << 0,
  LG_NET_USB_RESERVED_FINAL    = 1U << 1,
};

typedef struct LGNetUSBReserved
{
  uint16_t              version;
  uint16_t              headerSize;
  uint16_t              messageType;
  LGNetUSBReservedFlags flags;
  uint64_t              requestID;
  uint64_t              sequence;
  uint32_t              payloadLength;
  const uint8_t *       payload;
}
LGNetUSBReserved;

size_t lgNetCoreSessionInfoSize(const LGNetCoreSessionInfo * info);
bool lgNetCoreSessionInfoValid(const LGNetCoreSessionInfo * info);
bool lgNetCoreSessionInfoEncode(
  void * data, size_t size, const LGNetCoreSessionInfo * info);
LGNetParseResult lgNetCoreSessionInfoDecode(
  LGNetCoreSessionInfo * info, const void * data, size_t size);

bool lgNetCoreStatusValid(const LGNetCoreStatus * status);
bool lgNetCoreStatusEncode(
  void * data, size_t size, const LGNetCoreStatus * status);
LGNetParseResult lgNetCoreStatusDecode(
  LGNetCoreStatus * status, const void * data, size_t size);

size_t lgNetCoreErrorSize(const LGNetCoreError * error);
bool lgNetCoreErrorValid(const LGNetCoreError * error);
bool lgNetCoreErrorEncode(
  void * data, size_t size, const LGNetCoreError * error);
LGNetParseResult lgNetCoreErrorDecode(
  LGNetCoreError * error, const void * data, size_t size);

size_t lgNetRecoveryInfoSize(const LGNetRecoveryInfo * info);
bool lgNetRecoveryInfoValid(const LGNetRecoveryInfo * info);
bool lgNetRecoveryInfoEncode(
  void * data, size_t size, const LGNetRecoveryInfo * info);
LGNetParseResult lgNetRecoveryInfoDecode(
  LGNetRecoveryInfo * info, const void * data, size_t size);

bool lgNetRecoveryRequestValid(const LGNetRecoveryRequest * request);
bool lgNetRecoveryRequestEncode(
  void * data, size_t size, const LGNetRecoveryRequest * request);
LGNetParseResult lgNetRecoveryRequestDecode(
  LGNetRecoveryRequest * request, const void * data, size_t size);

bool lgNetRecoveryStatusValid(const LGNetRecoveryStatus * status);
bool lgNetRecoveryStatusEncode(
  void * data, size_t size, const LGNetRecoveryStatus * status);
LGNetParseResult lgNetRecoveryStatusDecode(
  LGNetRecoveryStatus * status, const void * data, size_t size);

bool lgNetVideoSubscribeValid(const LGNetVideoSubscribe * subscribe);
bool lgNetVideoSubscribeEncode(
  void * data, size_t size, const LGNetVideoSubscribe * subscribe);
LGNetParseResult lgNetVideoSubscribeDecode(
  LGNetVideoSubscribe * subscribe, const void * data, size_t size);

bool lgNetVideoControlValid(const LGNetVideoControl * control);
bool lgNetVideoControlEncode(
  void * data, size_t size, const LGNetVideoControl * control);
LGNetParseResult lgNetVideoControlDecode(
  LGNetVideoControl * control, const void * data, size_t size);

bool lgNetVideoStreamConfigValid(const LGNetVideoStreamConfig * config);
bool lgNetVideoStreamConfigEncode(void * data, size_t size,
  const LGNetVideoStreamConfig * config);
LGNetParseResult lgNetVideoStreamConfigDecode(
  LGNetVideoStreamConfig * config, const void * data, size_t size);

size_t lgNetVideoFrameSize(const LGNetVideoFrame * frame);
bool lgNetVideoFrameValid(const LGNetVideoFrame * frame);
bool lgNetVideoFrameEncode(
  void * data, size_t size, const LGNetVideoFrame * frame);
LGNetParseResult lgNetVideoFrameDecode(
  LGNetVideoFrame * frame, const void * data, size_t size);

size_t lgNetVideoFragmentSize(const LGNetVideoFragment * fragment);
bool lgNetVideoFragmentValid(const LGNetVideoFragment * fragment);
bool lgNetVideoFragmentEncode(
  void * data, size_t size, const LGNetVideoFragment * fragment);
LGNetParseResult lgNetVideoFragmentDecode(
  LGNetVideoFragment * fragment, const void * data, size_t size);

bool lgNetVideoFeedbackValid(const LGNetVideoFeedback * feedback);
bool lgNetVideoFeedbackEncode(
  void * data, size_t size, const LGNetVideoFeedback * feedback);
LGNetParseResult lgNetVideoFeedbackDecode(
  LGNetVideoFeedback * feedback, const void * data, size_t size);

bool lgNetVideoScheduleValid(const LGNetVideoSchedule * schedule);
bool lgNetVideoScheduleEncode(
  void * data, size_t size, const LGNetVideoSchedule * schedule);
LGNetParseResult lgNetVideoScheduleDecode(
  LGNetVideoSchedule * schedule, const void * data, size_t size);

bool lgNetVideoStatusValid(const LGNetVideoStatus * status);
bool lgNetVideoStatusEncode(
  void * data, size_t size, const LGNetVideoStatus * status);
LGNetParseResult lgNetVideoStatusDecode(
  LGNetVideoStatus * status, const void * data, size_t size);

bool lgNetCursorPositionValid(const LGNetCursorPosition * position);
bool lgNetCursorPositionEncode(
  void * data, size_t size, const LGNetCursorPosition * position);
LGNetParseResult lgNetCursorPositionDecode(
  LGNetCursorPosition * position, const void * data, size_t size);

size_t lgNetCursorShapeSize(const LGNetCursorShape * shape);
bool lgNetCursorShapeValid(const LGNetCursorShape * shape);
bool lgNetCursorShapeEncode(
  void * data, size_t size, const LGNetCursorShape * shape);
LGNetParseResult lgNetCursorShapeDecode(
  LGNetCursorShape * shape, const void * data, size_t size);

bool lgNetCursorStateValid(const LGNetCursorState * state);
bool lgNetCursorStateEncode(
  void * data, size_t size, const LGNetCursorState * state);
LGNetParseResult lgNetCursorStateDecode(
  LGNetCursorState * state, const void * data, size_t size);

bool lgNetCursorTransformValid(const LGNetCursorTransform * transform);
bool lgNetCursorTransformEncode(
  void * data, size_t size, const LGNetCursorTransform * transform);
LGNetParseResult lgNetCursorTransformDecode(
  LGNetCursorTransform * transform, const void * data, size_t size);

size_t lgNetCursorColorTransformSize(
  const LGNetCursorColorTransform * transform);
bool lgNetCursorColorLUTEncode(void * data, size_t size,
  const float lut[LG_NET_CURSOR_COLOR_LUT_FLOATS]);
bool lgNetCursorColorLUTDecode(
  float lut[LG_NET_CURSOR_COLOR_LUT_FLOATS], const void * data, size_t size);
bool lgNetCursorColorTransformValid(
  const LGNetCursorColorTransform * transform);
bool lgNetCursorColorTransformEncode(void * data, size_t size,
  const LGNetCursorColorTransform * transform);
LGNetParseResult lgNetCursorColorTransformDecode(
  LGNetCursorColorTransform * transform, const void * data, size_t size);

bool lgNetCursorStatusValid(const LGNetCursorStatus * status);
bool lgNetCursorStatusEncode(
  void * data, size_t size, const LGNetCursorStatus * status);
LGNetParseResult lgNetCursorStatusDecode(
  LGNetCursorStatus * status, const void * data, size_t size);

bool lgNetControlCursorPositionValid(
  const LGNetControlCursorPosition * position);
bool lgNetControlCursorPositionEncode(void * data, size_t size,
  const LGNetControlCursorPosition * position);
LGNetParseResult lgNetControlCursorPositionDecode(
  LGNetControlCursorPosition * position, const void * data, size_t size);

bool lgNetControlDisplaySizeValid(const LGNetControlDisplaySize * display);
bool lgNetControlDisplaySizeEncode(void * data, size_t size,
  const LGNetControlDisplaySize * display);
LGNetParseResult lgNetControlDisplaySizeDecode(
  LGNetControlDisplaySize * display, const void * data, size_t size);

bool lgNetControlFrameScheduleValid(
  const LGNetControlFrameSchedule * schedule);
bool lgNetControlFrameScheduleEncode(void * data, size_t size,
  const LGNetControlFrameSchedule * schedule);
LGNetParseResult lgNetControlFrameScheduleDecode(
  LGNetControlFrameSchedule * schedule, const void * data, size_t size);

bool lgNetControlStatusValid(const LGNetControlStatus * status);
bool lgNetControlStatusEncode(
  void * data, size_t size, const LGNetControlStatus * status);
LGNetParseResult lgNetControlStatusDecode(
  LGNetControlStatus * status, const void * data, size_t size);

bool lgNetInputClaimValid(const LGNetInputClaim * claim);
bool lgNetInputClaimEncode(
  void * data, size_t size, const LGNetInputClaim * claim);
LGNetParseResult lgNetInputClaimDecode(
  LGNetInputClaim * claim, const void * data, size_t size);

bool lgNetInputStatusValid(const LGNetInputStatus * status);
bool lgNetInputStatusEncode(
  void * data, size_t size, const LGNetInputStatus * status);
LGNetParseResult lgNetInputStatusDecode(
  LGNetInputStatus * status, const void * data, size_t size);

bool lgNetInputControlValid(const LGNetInputControl * control);
bool lgNetInputControlEncode(
  void * data, size_t size, const LGNetInputControl * control);
LGNetParseResult lgNetInputControlDecode(
  LGNetInputControl * control, const void * data, size_t size);

bool lgNetInputRelativeValid(const LGNetInputRelative * relative);
bool lgNetInputRelativeEncode(
  void * data, size_t size, const LGNetInputRelative * relative);
LGNetParseResult lgNetInputRelativeDecode(
  LGNetInputRelative * relative, const void * data, size_t size);

bool lgNetInputAbsoluteValid(const LGNetInputAbsolute * absolute);
bool lgNetInputAbsoluteEncode(
  void * data, size_t size, const LGNetInputAbsolute * absolute);
LGNetParseResult lgNetInputAbsoluteDecode(
  LGNetInputAbsolute * absolute, const void * data, size_t size);

bool lgNetInputKeyboardValid(const LGNetInputKeyboard * keyboard);
bool lgNetInputKeyboardEncode(
  void * data, size_t size, const LGNetInputKeyboard * keyboard);
LGNetParseResult lgNetInputKeyboardDecode(
  LGNetInputKeyboard * keyboard, const void * data, size_t size);

bool lgNetInputLEDsValid(const LGNetInputLEDs * leds);
bool lgNetInputLEDsEncode(
  void * data, size_t size, const LGNetInputLEDs * leds);
LGNetParseResult lgNetInputLEDsDecode(
  LGNetInputLEDs * leds, const void * data, size_t size);

bool lgNetAudioSubscribeValid(const LGNetAudioSubscribe * subscribe);
bool lgNetAudioSubscribeEncode(
  void * data, size_t size, const LGNetAudioSubscribe * subscribe);
LGNetParseResult lgNetAudioSubscribeDecode(
  LGNetAudioSubscribe * subscribe, const void * data, size_t size);

bool lgNetAudioFormatValid(const LGNetAudioFormat * format);
bool lgNetAudioFormatEncode(
  void * data, size_t size, const LGNetAudioFormat * format);
LGNetParseResult lgNetAudioFormatDecode(
  LGNetAudioFormat * format, const void * data, size_t size);

size_t lgNetAudioDataSize(const LGNetAudioData * audio);
bool lgNetAudioDataValid(const LGNetAudioData * audio);
bool lgNetAudioDataMatchesFormat(
  const LGNetAudioData * audio, const LGNetAudioFormat * format);
bool lgNetAudioDataEncode(
  void * data, size_t size, const LGNetAudioData * audio);
LGNetParseResult lgNetAudioDataDecode(
  LGNetAudioData * audio, const void * data, size_t size);

bool lgNetAudioStateValid(const LGNetAudioState * state);
bool lgNetAudioStateEncode(
  void * data, size_t size, const LGNetAudioState * state);
LGNetParseResult lgNetAudioStateDecode(
  LGNetAudioState * state, const void * data, size_t size);

bool lgNetAudioClockFeedbackValid(
  const LGNetAudioClockFeedback * feedback);
bool lgNetAudioClockFeedbackEncode(void * data, size_t size,
  const LGNetAudioClockFeedback * feedback);
LGNetParseResult lgNetAudioClockFeedbackDecode(
  LGNetAudioClockFeedback * feedback, const void * data, size_t size);

bool lgNetAudioControlValid(const LGNetAudioControl * control);
bool lgNetAudioControlEncode(
  void * data, size_t size, const LGNetAudioControl * control);
LGNetParseResult lgNetAudioControlDecode(
  LGNetAudioControl * control, const void * data, size_t size);

size_t lgNetAudioVolumeSize(const LGNetAudioVolume * volume);
bool lgNetAudioVolumeValid(const LGNetAudioVolume * volume);
bool lgNetAudioVolumeEncode(
  void * data, size_t size, const LGNetAudioVolume * volume);
LGNetParseResult lgNetAudioVolumeDecode(
  LGNetAudioVolume * volume, const void * data, size_t size);

bool lgNetAudioMuteValid(const LGNetAudioMute * mute);
bool lgNetAudioMuteEncode(
  void * data, size_t size, const LGNetAudioMute * mute);
LGNetParseResult lgNetAudioMuteDecode(
  LGNetAudioMute * mute, const void * data, size_t size);

bool lgNetAudioBarrierValid(const LGNetAudioBarrier * barrier);
bool lgNetAudioBarrierEncode(
  void * data, size_t size, const LGNetAudioBarrier * barrier);
LGNetParseResult lgNetAudioBarrierDecode(
  LGNetAudioBarrier * barrier, const void * data, size_t size);

bool lgNetClipboardClaimValid(const LGNetClipboardClaim * claim);
bool lgNetClipboardClaimEncode(
  void * data, size_t size, const LGNetClipboardClaim * claim);
LGNetParseResult lgNetClipboardClaimDecode(
  LGNetClipboardClaim * claim, const void * data, size_t size);

bool lgNetClipboardControlValid(const LGNetClipboardControl * control);
bool lgNetClipboardControlEncode(
  void * data, size_t size, const LGNetClipboardControl * control);
LGNetParseResult lgNetClipboardControlDecode(
  LGNetClipboardControl * control, const void * data, size_t size);

bool lgNetClipboardClaimStatusValid(
  const LGNetClipboardClaimStatus * status);
bool lgNetClipboardClaimStatusEncode(
  void * data, size_t size, const LGNetClipboardClaimStatus * status);
LGNetParseResult lgNetClipboardClaimStatusDecode(
  LGNetClipboardClaimStatus * status, const void * data, size_t size);

size_t lgNetClipboardOfferSize(const LGNetClipboardOffer * offer);
bool lgNetClipboardOfferValid(const LGNetClipboardOffer * offer);
bool lgNetClipboardOfferEncode(
  void * data, size_t size, const LGNetClipboardOffer * offer);
LGNetParseResult lgNetClipboardOfferDecode(
  LGNetClipboardOffer * offer, const void * data, size_t size);

bool lgNetClipboardRequestValid(const LGNetClipboardRequest * request);
bool lgNetClipboardRequestEncode(
  void * data, size_t size, const LGNetClipboardRequest * request);
LGNetParseResult lgNetClipboardRequestDecode(
  LGNetClipboardRequest * request, const void * data, size_t size);

size_t lgNetClipboardChunkSize(const LGNetClipboardChunk * chunk);
bool lgNetClipboardChunkValid(const LGNetClipboardChunk * chunk);
bool lgNetClipboardChunkEncode(
  void * data, size_t size, const LGNetClipboardChunk * chunk);
LGNetParseResult lgNetClipboardChunkDecode(
  LGNetClipboardChunk * chunk, const void * data, size_t size);

bool lgNetClipboardTransferValid(const LGNetClipboardTransfer * transfer);
bool lgNetClipboardTransferEncode(
  void * data, size_t size, const LGNetClipboardTransfer * transfer);
LGNetParseResult lgNetClipboardTransferDecode(
  LGNetClipboardTransfer * transfer, const void * data, size_t size);

bool lgNetClipboardStatusValid(const LGNetClipboardStatus * status);
bool lgNetClipboardStatusEncode(
  void * data, size_t size, const LGNetClipboardStatus * status);
LGNetParseResult lgNetClipboardStatusDecode(
  LGNetClipboardStatus * status, const void * data, size_t size);

size_t lgNetFileOfferSize(const LGNetFileOffer * offer);
bool lgNetFileOfferValid(const LGNetFileOffer * offer);
bool lgNetFileOfferEncode(
  void * data, size_t size, const LGNetFileOffer * offer);
LGNetParseResult lgNetFileOfferDecode(
  LGNetFileOffer * offer, const void * data, size_t size);

bool lgNetFileLeaseValid(const LGNetFileLease * lease);
bool lgNetFileLeaseEncode(
  void * data, size_t size, const LGNetFileLease * lease);
LGNetParseResult lgNetFileLeaseDecode(
  LGNetFileLease * lease, const void * data, size_t size);

size_t lgNetFileEntrySize(const LGNetFileEntry * entry);
bool lgNetFileEntryValid(const LGNetFileEntry * entry);
bool lgNetFileEntryEncode(
  void * data, size_t size, const LGNetFileEntry * entry);
LGNetParseResult lgNetFileEntryDecode(
  LGNetFileEntry * entry, const void * data, size_t size);

bool lgNetFileRequestValid(const LGNetFileRequest * request);
bool lgNetFileRequestEncode(
  void * data, size_t size, const LGNetFileRequest * request);
LGNetParseResult lgNetFileRequestDecode(
  LGNetFileRequest * request, const void * data, size_t size);

size_t lgNetFileChunkSize(const LGNetFileChunk * chunk);
bool lgNetFileChunkValid(const LGNetFileChunk * chunk);
bool lgNetFileChunkEncode(
  void * data, size_t size, const LGNetFileChunk * chunk);
LGNetParseResult lgNetFileChunkDecode(
  LGNetFileChunk * chunk, const void * data, size_t size);

bool lgNetFileTransferValid(const LGNetFileTransfer * transfer);
bool lgNetFileTransferEncode(
  void * data, size_t size, const LGNetFileTransfer * transfer);
LGNetParseResult lgNetFileTransferDecode(
  LGNetFileTransfer * transfer, const void * data, size_t size);

bool lgNetFileStatusValid(const LGNetFileStatus * status);
bool lgNetFileStatusEncode(
  void * data, size_t size, const LGNetFileStatus * status);
LGNetParseResult lgNetFileStatusDecode(
  LGNetFileStatus * status, const void * data, size_t size);

size_t lgNetUSBReservedSize(const LGNetUSBReserved * reserved);
bool lgNetUSBReservedValid(const LGNetUSBReserved * reserved);
bool lgNetUSBReservedEncode(
  void * data, size_t size, const LGNetUSBReserved * reserved);
LGNetParseResult lgNetUSBReservedDecode(
  LGNetUSBReserved * reserved, const void * data, size_t size);

#ifdef __cplusplus
}
#endif

#endif
