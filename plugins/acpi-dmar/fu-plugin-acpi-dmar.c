/*
 * Copyright (C) 2020 Richard Hughes <richard@hughsie.com>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 */

#include "config.h"

#include "fu-plugin-vfuncs.h"
#include "fu-hash.h"
#include "fu-acpi-dmar.h"

void
fu_plugin_init (FuPlugin *plugin)
{
	fu_plugin_set_build_hash (plugin, FU_BUILD_HASH);
}

void
fu_plugin_add_security_attrs (FuPlugin *plugin, FuSecurityAttrs *attrs)
{
	g_autofree gchar *fn = NULL;
	g_autofree gchar *path = NULL;
	g_autoptr(FuAcpiDmar) dmar = NULL;
	g_autoptr(FwupdSecurityAttr) attr = NULL;
	g_autoptr(GBytes) blob = NULL;
	g_autoptr(GError) error_local = NULL;

	/* only Intel */
	if (!fu_common_is_cpu_intel ())
		return;

	/* create attr */
	attr = fwupd_security_attr_new ("org.uefi.ACPI.Dmar");
	fwupd_security_attr_set_level (attr, FWUPD_SECURITY_ATTR_LEVEL_THEORETICAL);
	fwupd_security_attr_set_name (attr, "Pre-boot kernel DMA protection");
	fu_security_attrs_append (attrs, attr);

	/* load DMAR table */
	path = fu_common_get_path (FU_PATH_KIND_ACPI_TABLES);
	fn = g_build_filename (path, "DMAR", NULL);
	blob = fu_common_get_contents_bytes (fn, &error_local);
	if (blob == NULL) {
		g_warning ("failed to load %s: %s", fn, error_local->message);
		fwupd_security_attr_set_result (attr, "Could not load DMAR");
		return;
	}
	dmar = fu_acpi_dmar_new (blob, &error_local);
	if (dmar == NULL) {
		g_warning ("failed to parse %s: %s", fn, error_local->message);
		fwupd_security_attr_set_result (attr, "Could not parse DMAR");
		return;
	}
	if (!fu_acpi_dmar_get_opt_in (dmar)) {
		fwupd_security_attr_set_result (attr, "Unavailable");
		return;
	}

	/* success */
	fwupd_security_attr_add_flag (attr, FWUPD_SECURITY_ATTR_FLAG_SUCCESS);
}
