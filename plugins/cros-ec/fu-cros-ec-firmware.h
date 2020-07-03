/*
 * Copyright (C) 2020 Benson Leung <bleung@chromium.org>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 */

#pragma once

#include "fu-firmware.h"
#include "fu-fmap-firmware.h"

#define FU_TYPE_CROS_EC_FIRMWARE (fu_cros_ec_firmware_get_type ())
G_DECLARE_FINAL_TYPE (FuCrosEcFirmware, fu_cros_ec_firmware, FU, CROS_EC_FIRMWARE, FuFmapFirmware)

/*
 * Each RO or RW section of the new image can be in one of the following
 * states.
 */
typedef enum {
	FU_CROS_EC_FW_NOT_NEEDED= 0,	/* Version below or equal that on the target. */
	FU_CROS_EC_FW_NOT_POSSIBLE,	/*
					 * RO is newer, but can't be transferred due to
					 * target RW shortcomings.
					 */
	FU_CROS_EC_FW_NEEDED		/*
					 * This section needs to be transferred to the
					 * target.
					 */
} FuCrosEcFirmwareUpgradeStatus;

typedef struct {
	const gchar			*name;
	guint32				offset;
	gsize				size;
	FuCrosEcFirmwareUpgradeStatus	ustatus;
	gchar				version[FU_FMAP_FIRMWARE_STRLEN];
	gint32				rollback;
	guint32				key_version;
} FuCrosEcFirmwareSection;

FuFirmware			*fu_cros_ec_firmware_new		(void);
