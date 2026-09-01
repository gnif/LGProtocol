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

#include <linux/kvmfr.h>

#include <stddef.h>

#define UAPI_ASSERT(condition) _Static_assert((condition), #condition)

UAPI_ASSERT(KVMFR_DMABUF_FLAG_CLOEXEC == 1);
UAPI_ASSERT(offsetof(struct kvmfr_dmabuf_create, flags) == 0);
UAPI_ASSERT(offsetof(struct kvmfr_dmabuf_create, offset) ==
  _Alignof(__u64));
UAPI_ASSERT(offsetof(struct kvmfr_dmabuf_create, size) ==
  offsetof(struct kvmfr_dmabuf_create, offset) + sizeof(__u64));
UAPI_ASSERT(sizeof(struct kvmfr_dmabuf_create) ==
  offsetof(struct kvmfr_dmabuf_create, offset) + 2u * sizeof(__u64));
UAPI_ASSERT(_Alignof(struct kvmfr_dmabuf_create) == _Alignof(__u64));

UAPI_ASSERT(_IOC_TYPE(KVMFR_DMABUF_GETSIZE) == 'u');
UAPI_ASSERT(_IOC_NR(KVMFR_DMABUF_GETSIZE) == 68);
UAPI_ASSERT(_IOC_DIR(KVMFR_DMABUF_GETSIZE) == _IOC_NONE);
UAPI_ASSERT(_IOC_SIZE(KVMFR_DMABUF_GETSIZE) == 0);
UAPI_ASSERT(_IOC_TYPE(KVMFR_DMABUF_CREATE) == 'u');
UAPI_ASSERT(_IOC_NR(KVMFR_DMABUF_CREATE) == 66);
UAPI_ASSERT(_IOC_DIR(KVMFR_DMABUF_CREATE) == _IOC_WRITE);
UAPI_ASSERT(_IOC_SIZE(KVMFR_DMABUF_CREATE) ==
  sizeof(struct kvmfr_dmabuf_create));

#if defined(__x86_64__)
UAPI_ASSERT(offsetof(struct kvmfr_dmabuf_create, offset) == 8);
UAPI_ASSERT(offsetof(struct kvmfr_dmabuf_create, size) == 16);
UAPI_ASSERT(sizeof(struct kvmfr_dmabuf_create) == 24);
UAPI_ASSERT(_Alignof(struct kvmfr_dmabuf_create) == 8);
UAPI_ASSERT(KVMFR_DMABUF_GETSIZE == 0x00007544UL);
UAPI_ASSERT(KVMFR_DMABUF_CREATE == 0x40187542UL);
#elif defined(__i386__)
UAPI_ASSERT(offsetof(struct kvmfr_dmabuf_create, offset) == 4);
UAPI_ASSERT(offsetof(struct kvmfr_dmabuf_create, size) == 12);
UAPI_ASSERT(sizeof(struct kvmfr_dmabuf_create) == 20);
UAPI_ASSERT(KVMFR_DMABUF_CREATE == 0x40147542UL);
#endif

int main(void)
{
  return 0;
}
