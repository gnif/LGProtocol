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

#ifndef LGPROTOCOL_LINUX_KVMFR_H
#define LGPROTOCOL_LINUX_KVMFR_H

#include <linux/types.h>
#include <linux/ioctl.h>

#define KVMFR_DMABUF_FLAG_CLOEXEC 1

struct kvmfr_dmabuf_create
{
  __u8  flags;
  __u64 offset;
  __u64 size;
};

#define KVMFR_DMABUF_GETSIZE _IO('u', 68)
#define KVMFR_DMABUF_CREATE  _IOW('u', 66, struct kvmfr_dmabuf_create)

#endif
