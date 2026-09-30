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

#ifndef LGPROTOCOL_NETWORK_CLIPBOARD_H
#define LGPROTOCOL_NETWORK_CLIPBOARD_H

#include "KVMFRClipboard.h"
#include "NetworkServices.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LG_NET_CLIPBOARD_MIME_TEXT       "text/plain;charset=utf-8"
#define LG_NET_CLIPBOARD_MIME_TEXT_ALIAS "text/plain"
#define LG_NET_CLIPBOARD_MIME_PNG        "image/png"
#define LG_NET_CLIPBOARD_MIME_BMP        "image/bmp"
#define LG_NET_CLIPBOARD_MIME_TIFF       "image/tiff"
#define LG_NET_CLIPBOARD_MIME_JPEG       "image/jpeg"
#define LG_NET_CLIPBOARD_MIME_FILES      \
  "application/x-looking-glass-file-dataset"

#define LG_NET_CLIPBOARD_MIME_TEXT_LENGTH       \
  (sizeof(LG_NET_CLIPBOARD_MIME_TEXT) - 1U)
#define LG_NET_CLIPBOARD_MIME_TEXT_ALIAS_LENGTH \
  (sizeof(LG_NET_CLIPBOARD_MIME_TEXT_ALIAS) - 1U)
#define LG_NET_CLIPBOARD_MIME_PNG_LENGTH        \
  (sizeof(LG_NET_CLIPBOARD_MIME_PNG) - 1U)
#define LG_NET_CLIPBOARD_MIME_BMP_LENGTH        \
  (sizeof(LG_NET_CLIPBOARD_MIME_BMP) - 1U)
#define LG_NET_CLIPBOARD_MIME_TIFF_LENGTH       \
  (sizeof(LG_NET_CLIPBOARD_MIME_TIFF) - 1U)
#define LG_NET_CLIPBOARD_MIME_JPEG_LENGTH       \
  (sizeof(LG_NET_CLIPBOARD_MIME_JPEG) - 1U)
#define LG_NET_CLIPBOARD_MIME_FILES_LENGTH      \
  (sizeof(LG_NET_CLIPBOARD_MIME_FILES) - 1U)

#define LG_NET_CLIPBOARD_KVMFR_FORMAT_COUNT 6U
#define LG_NET_FILE_KVMFR_ROOT_ENTRY_ID      UINT64_C(1)

/* These are the canonical MIME names used when the network service carries
 * the KVMFR clipboard formats. text/plain is accepted as a compatibility
 * alias, but the UTF-8 form is always emitted. */
static inline int lgNetClipboardMimeEqual(const void * mime,
    size_t mimeLength, const char * expected, size_t expectedLength)
{
  return mime && expected && mimeLength == expectedLength &&
    memcmp(mime, expected, expectedLength) == 0;
}

static inline int lgNetClipboardKVMFRFormatMime(
    KVMFRClipboardFormat format, const uint8_t ** mime,
    uint16_t * mimeLength)
{
  const char * value;
  size_t       length;

  if (!mime || !mimeLength)
    return 0;

  switch (format)
  {
    case KVMFR_CLIPBOARD_FORMAT_TEXT:
      value  = LG_NET_CLIPBOARD_MIME_TEXT;
      length = LG_NET_CLIPBOARD_MIME_TEXT_LENGTH;
      break;

    case KVMFR_CLIPBOARD_FORMAT_PNG:
      value  = LG_NET_CLIPBOARD_MIME_PNG;
      length = LG_NET_CLIPBOARD_MIME_PNG_LENGTH;
      break;

    case KVMFR_CLIPBOARD_FORMAT_BMP:
      value  = LG_NET_CLIPBOARD_MIME_BMP;
      length = LG_NET_CLIPBOARD_MIME_BMP_LENGTH;
      break;

    case KVMFR_CLIPBOARD_FORMAT_TIFF:
      value  = LG_NET_CLIPBOARD_MIME_TIFF;
      length = LG_NET_CLIPBOARD_MIME_TIFF_LENGTH;
      break;

    case KVMFR_CLIPBOARD_FORMAT_JPEG:
      value  = LG_NET_CLIPBOARD_MIME_JPEG;
      length = LG_NET_CLIPBOARD_MIME_JPEG_LENGTH;
      break;

    case KVMFR_CLIPBOARD_FORMAT_FILES:
      value  = LG_NET_CLIPBOARD_MIME_FILES;
      length = LG_NET_CLIPBOARD_MIME_FILES_LENGTH;
      break;

    default:
      return 0;
  }

  *mime       = (const uint8_t *)value;
  *mimeLength = (uint16_t)length;
  return 1;
}

static inline KVMFRClipboardFormat lgNetClipboardKVMFRFormatFromMime(
    const void * mime, size_t mimeLength)
{
  if (lgNetClipboardMimeEqual(mime, mimeLength,
        LG_NET_CLIPBOARD_MIME_TEXT, LG_NET_CLIPBOARD_MIME_TEXT_LENGTH) ||
      lgNetClipboardMimeEqual(mime, mimeLength,
        LG_NET_CLIPBOARD_MIME_TEXT_ALIAS,
        LG_NET_CLIPBOARD_MIME_TEXT_ALIAS_LENGTH))
    return KVMFR_CLIPBOARD_FORMAT_TEXT;

  if (lgNetClipboardMimeEqual(mime, mimeLength,
        LG_NET_CLIPBOARD_MIME_PNG, LG_NET_CLIPBOARD_MIME_PNG_LENGTH))
    return KVMFR_CLIPBOARD_FORMAT_PNG;

  if (lgNetClipboardMimeEqual(mime, mimeLength,
        LG_NET_CLIPBOARD_MIME_BMP, LG_NET_CLIPBOARD_MIME_BMP_LENGTH))
    return KVMFR_CLIPBOARD_FORMAT_BMP;

  if (lgNetClipboardMimeEqual(mime, mimeLength,
        LG_NET_CLIPBOARD_MIME_TIFF, LG_NET_CLIPBOARD_MIME_TIFF_LENGTH))
    return KVMFR_CLIPBOARD_FORMAT_TIFF;

  if (lgNetClipboardMimeEqual(mime, mimeLength,
        LG_NET_CLIPBOARD_MIME_JPEG, LG_NET_CLIPBOARD_MIME_JPEG_LENGTH))
    return KVMFR_CLIPBOARD_FORMAT_JPEG;

  if (lgNetClipboardMimeEqual(mime, mimeLength,
        LG_NET_CLIPBOARD_MIME_FILES, LG_NET_CLIPBOARD_MIME_FILES_LENGTH))
    return KVMFR_CLIPBOARD_FORMAT_FILES;

  return KVMFR_CLIPBOARD_FORMAT_NONE;
}

static inline int lgNetFileEntryIDFromKVMFRNode(
    uint64_t node, uint64_t * entryID)
{
  if (!entryID || node == UINT64_MAX)
    return 0;

  *entryID = node + UINT64_C(1);
  return 1;
}

/* The KVMFR compatibility profile publishes one virtual root and transports
 * its lazy LIST and READ result bytes through the normal file data messages.
 * Adding one preserves KVMFR's root node zero while reserving entry ID zero
 * for offer-level protocol status. */
static inline int lgNetFileKVMFRNodeFromEntryID(
    uint64_t entryID, uint64_t * node)
{
  if (!entryID || !node)
    return 0;

  *node = entryID - UINT64_C(1);
  return 1;
}

static inline LGNetFileRequestFlags lgNetFileRequestFlagsFromKVMFROperation(
    KVMFRClipboardFileOperation operation)
{
  switch (operation)
  {
    case KVMFR_CLIPBOARD_FILE_OP_LIST:
      return LG_NET_FILE_REQUEST_METADATA;

    case KVMFR_CLIPBOARD_FILE_OP_READ:
      return LG_NET_FILE_REQUEST_DATA;

    default:
      return 0;
  }
}

static inline KVMFRClipboardFileOperation
lgNetFileKVMFROperationFromRequestFlags(LGNetFileRequestFlags flags)
{
  switch (flags)
  {
    case LG_NET_FILE_REQUEST_METADATA:
      return KVMFR_CLIPBOARD_FILE_OP_LIST;

    case LG_NET_FILE_REQUEST_DATA:
      return KVMFR_CLIPBOARD_FILE_OP_READ;

    default:
      return 0;
  }
}

static inline LGNetFileStatusCode lgNetFileStatusFromKVMFRError(
    KVMFRClipboardFileError error)
{
  switch (error)
  {
    case KVMFR_CLIPBOARD_FILE_ERROR_NONE:
      return LG_NET_FILE_STATUS_OK;

    case KVMFR_CLIPBOARD_FILE_ERROR_NOT_FOUND:
      return LG_NET_FILE_STATUS_NOT_FOUND;

    case KVMFR_CLIPBOARD_FILE_ERROR_ACCESS:
      return LG_NET_FILE_STATUS_DENIED;

    case KVMFR_CLIPBOARD_FILE_ERROR_NOT_DIRECTORY:
    case KVMFR_CLIPBOARD_FILE_ERROR_IS_DIRECTORY:
    case KVMFR_CLIPBOARD_FILE_ERROR_INVALID:
    case KVMFR_CLIPBOARD_FILE_ERROR_STALE:
      return LG_NET_FILE_STATUS_INVALID;

    case KVMFR_CLIPBOARD_FILE_ERROR_DISCONNECTED:
    case KVMFR_CLIPBOARD_FILE_ERROR_CANCELLED:
      return LG_NET_FILE_STATUS_CANCELLED;

    case KVMFR_CLIPBOARD_FILE_ERROR_NOT_SUPPORTED:
      return LG_NET_FILE_STATUS_UNSUPPORTED;

    case KVMFR_CLIPBOARD_FILE_ERROR_IO:
    case KVMFR_CLIPBOARD_FILE_ERROR_NO_MEMORY:
    case KVMFR_CLIPBOARD_FILE_ERROR_NO_SPACE:
      return LG_NET_FILE_STATUS_ERROR;
  }

  return LG_NET_FILE_STATUS_ERROR;
}

static inline KVMFRClipboardFileError lgNetFileKVMFRErrorFromStatus(
    LGNetFileStatusCode status, uint32_t detail)
{
  const KVMFRClipboardFileError exact =
    (KVMFRClipboardFileError)detail;
  if (kvmfrClipboardFileErrorValid(exact) &&
      lgNetFileStatusFromKVMFRError(exact) == status)
    return exact;

  switch (status)
  {
    case LG_NET_FILE_STATUS_OK:
      return KVMFR_CLIPBOARD_FILE_ERROR_NONE;

    case LG_NET_FILE_STATUS_NOT_FOUND:
      return KVMFR_CLIPBOARD_FILE_ERROR_NOT_FOUND;

    case LG_NET_FILE_STATUS_DENIED:
      return KVMFR_CLIPBOARD_FILE_ERROR_ACCESS;

    case LG_NET_FILE_STATUS_CANCELLED:
      return KVMFR_CLIPBOARD_FILE_ERROR_CANCELLED;

    case LG_NET_FILE_STATUS_INVALID:
      return KVMFR_CLIPBOARD_FILE_ERROR_INVALID;

    case LG_NET_FILE_STATUS_UNSUPPORTED:
      return KVMFR_CLIPBOARD_FILE_ERROR_NOT_SUPPORTED;

    case LG_NET_FILE_STATUS_ERROR:
    default:
      return KVMFR_CLIPBOARD_FILE_ERROR_IO;
  }
}

#ifdef __cplusplus
}
#endif

#endif
