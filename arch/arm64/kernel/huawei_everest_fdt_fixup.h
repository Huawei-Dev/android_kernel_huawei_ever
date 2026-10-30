/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Everest (Mate 20 X 4G / Kirin 980) early FDT compatibility fixups v4.
 *
 * EMUI10 merged DTB owns hardware nodes and phandles.  The EMUI9.1
 * bootloader DTB supplies only runtime facts (physical board identity,
 * detected panel, physical RAM layout and a handful of firmware fields).
 * Never transplant an EMUI9.1 LCD driver subtree into the EMUI10 tree.
 *
 * This is NOT a 15-board DTB selector.  The caller must supply the correct
 * EMUI10 DTB and enough writable space before calling this function.
 */
#ifndef __HUAWEI_EVEREST_FDT_FIXUP_H
#define __HUAWEI_EVEREST_FDT_FIXUP_H

#include <linux/errno.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/libfdt.h>
#include <linux/string.h>

static bool __init evr_fdt_single_string(const char *value, int len)
{
	return value && len > 1 && value[len - 1] == '\0' &&
		strnlen(value, len) == (size_t)(len - 1);
}

static int __init evr_fdt_copy_prop(void *dst, const void *src,
				   const char *dst_path,
				   const char *src_path,
				   const char *name)
{
	int src_node, dst_node, len;
	const void *value;

	src_node = fdt_path_offset(src, src_path);
	dst_node = fdt_path_offset(dst, dst_path);
	if (src_node < 0 || dst_node < 0)
		return -ENOENT;

	value = fdt_getprop(src, src_node, name, &len);
	if (!value || len < 0)
		return -ENOENT;

	return fdt_setprop(dst, dst_node, name, value, len);
}

/*
 * The 4-cell board ID is the authoritative selection key.  Product names,
 * regional labels and modem IDs can legitimately change between firmware
 * generations, so their mismatch is reported, not used to switch boards.
 * Both FDTs must nevertheless describe an Everest, never PANDA or ELE.
 */
static int __init evr_fdt_validate_and_copy_identity(void *dst,
						const void *src)
{
	static const char * const names[] = {
		"hisi,boardname", "hisi,product_id", "hisi,product_name",
		"hisi,modem_id", "hisi,boardid"
	};
	int s, d, slen, dlen, ret;
	const void *sv, *dv;
	const fdt32_t *sid, *did;
	unsigned char copy[128];
	size_t i;

	s = fdt_path_offset(src, "/rootparam");
	d = fdt_path_offset(dst, "/rootparam");
	if (s < 0 || d < 0)
		return -ENOENT;

	sid = fdt_getprop(src, s, "hisi,boardid", &slen);
	did = fdt_getprop(dst, d, "hisi,boardid", &dlen);
	if (!sid || !did || slen != 4 * sizeof(*sid) ||
	    dlen != 4 * sizeof(*did) || memcmp(sid, did, slen)) {
		pr_crit("Everest FDT: boardid absent, malformed or different\n");
		return -EINVAL;
	}
	if (fdt32_to_cpu(sid[0]) != 8 || fdt32_to_cpu(sid[1]) != 4)
		return -EINVAL;

	for (i = 0; i < ARRAY_SIZE(names); i++) {
		sv = fdt_getprop(src, s, names[i], &slen);
		dv = fdt_getprop(dst, d, names[i], &dlen);
		if (!sv || !dv)
			return -EINVAL;

		if (i == 0 || i == 2) {
			if (!evr_fdt_single_string(sv, slen) ||
			    !evr_fdt_single_string(dv, dlen))
				return -EINVAL;
			if (i == 0 &&
			    (strncmp(sv, "EVER_", 5) ||
			     strncmp(dv, "EVER_", 5)))
				return -EINVAL;
			if (i == 2 &&
			    (strncmp(sv, "EVR-", 4) ||
			     strncmp(dv, "EVR-", 4)))
				return -EINVAL;
		} else if (i == 1 || i == 3) {
			if (slen != sizeof(fdt32_t) ||
			    dlen != sizeof(fdt32_t))
				return -EINVAL;
		} else if (slen != 4 * sizeof(fdt32_t) ||
			   dlen != 4 * sizeof(fdt32_t)) {
			return -EINVAL;
		}

		if (slen != dlen || memcmp(sv, dv, slen)) {
			if (i == 4)
				return -EINVAL;
			pr_warn("Everest FDT: %s differs between BL and EMUI10 (boardid matches)\n",
				names[i]);
		}
	}

	/* Keep / and /rootparam consistent with the selected EMUI10 DTB.
	 * Copy via scratch storage: fdt_setprop() may relocate the source
	 * data if it lives inside the very same FDT being modified.
	 */
	for (i = 0; i < ARRAY_SIZE(names); i++) {
		int root = fdt_path_offset(dst, "/");

		d = fdt_path_offset(dst, "/rootparam");
		if (root < 0 || d < 0)
			return -ENOENT;
		dv = fdt_getprop(dst, d, names[i], &dlen);
		if (!dv || dlen < 1 || dlen > (int)sizeof(copy))
			return -EINVAL;
		memcpy(copy, dv, dlen);
		ret = fdt_setprop(dst, root, names[i], copy, dlen);
		if (ret)
			return ret;
	}

	pr_info("Everest FDT: matched boardid %u.%u.%u.%u\n",
		fdt32_to_cpu(sid[0]), fdt32_to_cpu(sid[1]),
		fdt32_to_cpu(sid[2]), fdt32_to_cpu(sid[3]));
	return 0;
}

/* Full compatible string-list parser: rejects malformed trailing strings. */
static int __init evr_fdt_compatible_match(const char *compatible, int len,
					  const char *wanted, int wanted_len)
{
	bool found = false;

	if (!compatible || len <= 0 || !wanted || wanted_len <= 0)
		return -EINVAL;

	while (len > 0) {
		size_t n = strnlen(compatible, len);

		if (n >= (size_t)len || !n)
			return -EINVAL;
		if (n == (size_t)wanted_len &&
		    !memcmp(compatible, wanted, n))
			found = true;
		compatible += n + 1;
		len -= n + 1;
	}
	return found ? 1 : 0;
}

/* 0 = disabled, 1 = enabled, negative = malformed DT status. */
static int __init evr_fdt_panel_enabled(const char *status, int len)
{
	if (!status)
		return len == -FDT_ERR_NOTFOUND ? 1 : -EINVAL;
	if (!evr_fdt_single_string(status, len))
		return -EINVAL;
	if (!strcmp(status, "disabled") || !strcmp(status, "fail") ||
	    !strncmp(status, "fail-", 5))
		return 0;
	if (!strcmp(status, "ok") || !strcmp(status, "okay"))
		return 1;
	return -EINVAL;
}

#define EVR_LCD_NAME_MAX 128
#define EVR_LCD_TYPE_MAX 128

/*
 * The bootloader knows which physical LCD is installed.  Find its one
 * enabled EMUI9.1 lcd_config child, and confirm lcd_panel_type refers
 * to THAT child within the SAME bootloader FDT.  Use its stable
 * lcd-kit,panel-name as the cross-firmware identity.
 *
 * We never use a stock EMUI9.1 compatible to select an EMUI10 node.
 */
static int __init evr_fdt_bootloader_panel_name(const void *src,
						char *saved_name,
						int saved_size)
{
	int cfg, panel, child, type_len, len, status_len, matches = 0;
	const char *type, *compat, *name, *status;
	int enabled, comp_len, ret;

	panel = fdt_path_offset(src, "/huawei,lcd_panel");
	cfg = fdt_path_offset(src, "/huawei,lcd_config");
	if (panel < 0 || cfg < 0)
		return -ENOENT;

	type = fdt_getprop(src, panel, "lcd_panel_type", &type_len);
	if (!evr_fdt_single_string(type, type_len) ||
	    type_len > EVR_LCD_TYPE_MAX)
		return -EINVAL;

	for (child = fdt_first_subnode(src, cfg);
	     child >= 0; child = fdt_next_subnode(src, child)) {
		status = fdt_getprop(src, child, "status", &status_len);
		enabled = evr_fdt_panel_enabled(status, status_len);
		if (enabled < 0)
			return enabled;
		if (!enabled)
			continue;
		matches++;
		/* Exactly one enabled source LCD must match its own type. */
		compat = fdt_getprop(src, child, "compatible", &comp_len);
		ret = evr_fdt_compatible_match(compat, comp_len,
					     type, type_len - 1);
		if (ret != 1)
			return ret < 0 ? ret : -EINVAL;

		name = fdt_getprop(src, child, "lcd-kit,panel-name", &len);
		if (!evr_fdt_single_string(name, len) || len > saved_size)
			return -EINVAL;
		memcpy(saved_name, name, len);
	}
	if (child != -FDT_ERR_NOTFOUND)
		return child;
	if (matches != 1) {
		pr_crit("Everest FDT: bootloader enabled LCD count=%d (expected 1)\n",
			matches);
		return -EINVAL;
	}
	return 0;
}

/*
 * Inspect the selected EMUI10 DTB, but do NOT reuse old node offsets
 * after fdt_setprop(): the blob may be relocated inside its buffer.
 * Return the unique panel offset only BEFORE mutation.  On the final
 * pass verify that it is the only enabled LCD node.
 */
static int __init evr_fdt_scan_emui10_panels(const void *dst,
						const char *wanted_name,
						bool after_fixup,
						int *selected,
						char *selected_type,
						int selected_size)
{
	int cfg, child, nmatch = 0, nenabled = 0;
	int len, status_len, compat_len, enabled, ret;
	const char *name, *status, *compatible;

	cfg = fdt_path_offset(dst, "/huawei,lcd_config");
	if (cfg < 0)
		return -ENOENT;

	for (child = fdt_first_subnode(dst, cfg);
	     child >= 0; child = fdt_next_subnode(dst, child)) {
		bool match = false;

		name = fdt_getprop(dst, child, "lcd-kit,panel-name", &len);
		if (name) {
			if (!evr_fdt_single_string(name, len))
				return -EINVAL;
			match = !strcmp(name, wanted_name);
		} else if (len != -FDT_ERR_NOTFOUND) {
			return -EINVAL;
		}

		status = fdt_getprop(dst, child, "status", &status_len);
		enabled = evr_fdt_panel_enabled(status, status_len);
		if (enabled < 0)
			return enabled;
		nenabled += enabled;
		if (enabled && !match)
			return -EBUSY;

		if (!match)
			continue;
		nmatch++;
		if (selected)
			*selected = child;

		compatible = fdt_getprop(dst, child, "compatible", &compat_len);
		if (!compatible || compat_len <= 1)
			return -EINVAL;
		len = strnlen(compatible, compat_len);
		if (!len || len >= compat_len || len + 1 > selected_size)
			return -EINVAL;
		/* Validate the WHOLE compatible string-list. */
		ret = evr_fdt_compatible_match(compatible, compat_len,
					     compatible, len);
		if (ret != 1)
			return -EINVAL;
		if (selected_type)
			memcpy(selected_type, compatible, len + 1);
	}
	if (child != -FDT_ERR_NOTFOUND)
		return child;
	if (nmatch != 1 || (after_fixup && nenabled != 1)) {
		pr_crit("Everest FDT: panel-name '%s' matches=%d enabled=%d\n",
			wanted_name, nmatch, nenabled);
		return -EINVAL;
	}
	return 0;
}

static int __init evr_fdt_select_panel(void *dst, const void *src)
{
	char panel_name[EVR_LCD_NAME_MAX];
	char emui10_compatible[EVR_LCD_TYPE_MAX];
	char verified_compatible[EVR_LCD_TYPE_MAX];
	int selected = -1, dst_panel, ret, len;
	const char *type;

	ret = evr_fdt_bootloader_panel_name(src, panel_name,
					      sizeof(panel_name));
	if (ret)
		return ret;

	dst_panel = fdt_path_offset(dst, "/huawei,lcd_panel");
	if (dst_panel < 0)
		return -ENOENT;

	ret = evr_fdt_scan_emui10_panels(dst, panel_name, false, &selected,
						emui10_compatible,
						sizeof(emui10_compatible));
	if (ret || selected < 0)
		return ret ? ret : -EINVAL;

	/* The enabled node is an EMUI10 node, not a transplanted EMUI9 node. */
	ret = fdt_setprop(dst, selected, "status", "ok", sizeof("ok"));
	if (ret)
		return ret;

	/* Driver expects the selected EMUI10 panel's own compatible. */
	dst_panel = fdt_path_offset(dst, "/huawei,lcd_panel");
	if (dst_panel < 0)
		return -ENOENT;
	ret = fdt_setprop(dst, dst_panel, "lcd_panel_type",
			  emui10_compatible, strlen(emui10_compatible) + 1);
	if (ret)
		return ret;

	/* Re-scan AFTER writes; neither offsets nor pointers are reusable. */
	ret = evr_fdt_scan_emui10_panels(dst, panel_name, true, NULL,
						verified_compatible,
						sizeof(verified_compatible));
	if (ret || strcmp(verified_compatible, emui10_compatible))
		return ret ? ret : -EINVAL;
	dst_panel = fdt_path_offset(dst, "/huawei,lcd_panel");
	type = fdt_getprop(dst, dst_panel, "lcd_panel_type", &len);
	if (!evr_fdt_single_string(type, len) ||
	    strcmp(type, emui10_compatible))
		return -EINVAL;

	pr_info("Everest FDT: LCD matched by panel-name '%s'; EMUI10 compatible '%s'\n",
		panel_name, emui10_compatible);
	return 0;
}

static int __init evr_fdt_validate_memory(const void *src, const void *dst)
{
	const void *tree[2] = { src, dst };
	const fdt32_t *cells, *reg;
	u64 base[8], size[8];
	int i, j, k, root, mem, len, count;

	/* Both trees must use the same 64-bit address + 64-bit size encoding. */
	for (k = 0; k < 2; k++) {
		root = fdt_path_offset(tree[k], "/");
		if (root < 0)
			return -ENOENT;
		cells = fdt_getprop(tree[k], root, "#address-cells", &len);
		if (!cells || len != sizeof(fdt32_t) || fdt32_to_cpu(*cells) != 2)
			return -EINVAL;
		cells = fdt_getprop(tree[k], root, "#size-cells", &len);
		if (!cells || len != sizeof(fdt32_t) || fdt32_to_cpu(*cells) != 2)
			return -EINVAL;
	}

	mem = fdt_path_offset(src, "/memory@0");
	if (mem < 0 || fdt_path_offset(dst, "/memory@0") < 0)
		return -ENOENT;
	reg = fdt_getprop(src, mem, "reg", &len);
	if (!reg || len < 16 || len > 128 || (len % 16))
		return -EINVAL;
	count = len / 16;
	for (i = 0; i < count; i++) {
		base[i] = ((u64)fdt32_to_cpu(reg[i * 4]) << 32) |
			fdt32_to_cpu(reg[i * 4 + 1]);
		size[i] = ((u64)fdt32_to_cpu(reg[i * 4 + 2]) << 32) |
			fdt32_to_cpu(reg[i * 4 + 3]);
		if (!size[i] || base[i] > (~(u64)0) - size[i] ||
		    (base[i] & 0xfff) || (size[i] & 0xfff))
			return -EINVAL;
		for (j = 0; j < i; j++) {
			if (base[i] < base[j] + size[j] &&
			    base[j] < base[i] + size[i])
				return -EINVAL;
		}
	}
	return 0;
}

static int __init evr_fdt_copy_validated_status(void *dst, const void *src,
						 const char *path)
{
	int node, len, enabled;
	const char *status;

	node = fdt_path_offset(src, path);
	if (node < 0)
		return -ENOENT;
	status = fdt_getprop(src, node, "status", &len);
	if (!status)
		return -EINVAL;
	enabled = evr_fdt_panel_enabled(status, len);
	if (enabled < 0)
		return enabled;
	return evr_fdt_copy_prop(dst, src, path, path, "status");
}

static int __init evr_fdt_enable_hota(void *dst)
{
	int node, len;
	const fdt32_t *reg;
	const void *nomap;

	node = fdt_path_offset(dst, "/reserved-memory/hota");
	if (node < 0)
		return -ENOENT;
	reg = fdt_getprop(dst, node, "reg", &len);
	if (!reg || len != 4 * sizeof(*reg) ||
	    fdt32_to_cpu(reg[0]) != 0 ||
	    fdt32_to_cpu(reg[1]) != 0xaa200000 ||
	    fdt32_to_cpu(reg[2]) != 0 ||
	    fdt32_to_cpu(reg[3]) != 0x10000000)
		return -EINVAL;

	nomap = fdt_getprop(dst, node, "no-map", &len);
	if (!nomap || len != 0)
		return -EINVAL;
	return fdt_setprop(dst, node, "status", "ok", sizeof("ok"));
}

static int __init huawei_everest_adapt_fdt(void *dst, const void *src)
{
	int ret, node, len;
	const void *value;

	if (!src || !dst || fdt_check_header(src) || fdt_check_header(dst))
		return -EINVAL;

	ret = evr_fdt_validate_and_copy_identity(dst, src);
	if (ret)
		return ret;

	ret = evr_fdt_select_panel(dst, src);
	if (ret)
		return ret;

	ret = evr_fdt_validate_memory(src, dst);
	if (ret)
		return ret;
	ret = evr_fdt_copy_prop(dst, src, "/memory@0", "/memory@0", "reg");
	if (ret)
		return ret;

	ret = evr_fdt_copy_validated_status(dst, src,
					"/reserved-memory/logo-buffer");
	if (ret)
		return ret;

	node = fdt_path_offset(src, "/framebuffer@E8600000");
	if (node < 0)
		return -ENOENT;
	value = fdt_getprop(src, node, "fastboot_enable_flag", &len);
	if (!value || len != sizeof(fdt32_t) ||
	    fdt32_to_cpu(*(const fdt32_t *)value) > 1)
		return -EINVAL;
	ret = evr_fdt_copy_prop(dst, src, "/framebuffer@E8600000",
				"/framebuffer@E8600000", "fastboot_enable_flag");
	if (ret)
		return ret;

	node = fdt_path_offset(src, "/psci");
	if (node < 0)
		return -ENOENT;
	value = fdt_getprop(src, node, "method", &len);
	if (!evr_fdt_single_string(value, len) ||
	    (strcmp(value, "hvc") && strcmp(value, "smc")))
		return -EINVAL;
	ret = evr_fdt_copy_prop(dst, src, "/psci", "/psci", "method");
	if (ret)
		return ret;

	/* Always apply the tested Kirin980 mixed-firmware HOTA reservation. */
	ret = evr_fdt_enable_hota(dst);
	if (ret)
		return ret;

	pr_info("Everest EMUI10 FDT adapted from bootloader; LCD/RAM/HOTA validated\n");
	return 0;
}

#endif /* __HUAWEI_EVEREST_FDT_FIXUP_H */
