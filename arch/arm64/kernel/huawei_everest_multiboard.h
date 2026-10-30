/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Everest EMUI10 DTB-of-DTBs selector. Compatible with dtc's /incbin/().
 * No early allocations, no decompression, no EMUI9.1 device subtrees.
 *
 * Catalog DTS:
 *   / { compatible = "huawei,everest-dtb-catalog-v1";
 *       everest,dtb-catalog {
 *         board-8-4-6-2 {
 *           hisi,boardid = <8 4 6 2>;
 *           dtb-data = /incbin/("path/to/ever_tugl_vh.dtb");
 *         }; ...
 *       };
 *   };
 */
#ifndef __HUAWEI_EVEREST_MULTIBOARD_H
#define __HUAWEI_EVEREST_MULTIBOARD_H

#include <linux/errno.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/libfdt.h>
#include <linux/string.h>

#define EVR_MAX_CATALOG_BOARDS 15
#define EVR_RUNTIME_DTB_BYTES 2097152

/* 0 = valid catalog and unique board, >0 = legacy direct DTB, <0 = error. */
static int __init evr_catalog_find(const void *catalog, const void *boot,
				  const void **chosen, u32 *chosen_size)
{
	const fdt32_t *boot_id, *id, *embedded_id;
	const void *data, *winner = NULL;
	fdt32_t visited[EVR_MAX_CATALOG_BOARDS][4];
	const char *compat, *name;
	int cat_node, boot_root, blob_root;
	int child, count = 0, found = 0, i;
	int len, data_len, embedded_len, name_len;
	int ret;
	u32 size = 0;

	if (!catalog || !boot || !chosen || !chosen_size ||
	    fdt_check_header(catalog) || fdt_check_header(boot))
		return -EINVAL;
	*chosen = NULL;
	*chosen_size = 0;

	/* A normal DTB has no catalog signature; keep the proven one-board path. */
	compat = fdt_getprop(catalog, 0, "compatible", &len);
	if (!compat || len != sizeof("huawei,everest-dtb-catalog-v1") ||
	    memcmp(compat, "huawei,everest-dtb-catalog-v1", len))
		return 1;

	cat_node = fdt_path_offset(catalog, "/everest,dtb-catalog");
	if (cat_node < 0)
		return -EINVAL;
	boot_root = fdt_path_offset(boot, "/rootparam");
	if (boot_root < 0)
		return -EINVAL;
	boot_id = fdt_getprop(boot, boot_root, "hisi,boardid", &len);
	if (!boot_id || len != 4 * sizeof(fdt32_t) ||
	    fdt32_to_cpu(boot_id[0]) != 8 ||
	    fdt32_to_cpu(boot_id[1]) != 4)
		return -EINVAL;

	/* Inspect every entry; reject duplicate/mismatched/invalid DTBs even
	 * when an earlier entry already matched the bootloader board ID.
	 */
	for (child = fdt_first_subnode(catalog, cat_node);
	     child >= 0;
	     child = fdt_next_subnode(catalog, child)) {
		if (count >= EVR_MAX_CATALOG_BOARDS)
			return -E2BIG;

		id = fdt_getprop(catalog, child, "hisi,boardid", &len);
		if (!id || len != 4 * sizeof(fdt32_t) ||
		    fdt32_to_cpu(id[0]) != 8 ||
		    fdt32_to_cpu(id[1]) != 4)
			return -EINVAL;
		for (i = 0; i < count; i++)
			if (!memcmp(visited[i], id, 4 * sizeof(fdt32_t)))
				return -EEXIST;
		memcpy(visited[count], id, 4 * sizeof(fdt32_t));
		count++;

		data = fdt_getprop(catalog, child, "dtb-data", &data_len);
		if (!data || data_len < (int)sizeof(struct fdt_header) ||
		    data_len > EVR_RUNTIME_DTB_BYTES ||
		    fdt_check_header(data) || fdt_totalsize(data) != data_len)
			return -EINVAL;
		blob_root = fdt_path_offset(data, "/rootparam");
		if (blob_root < 0)
			return -EINVAL;
		embedded_id = fdt_getprop(data, blob_root, "hisi,boardid",
					  &embedded_len);
		if (!embedded_id || embedded_len != 4 * sizeof(fdt32_t) ||
		    memcmp(id, embedded_id, 4 * sizeof(fdt32_t)))
			return -EINVAL;

		/* Each entry's descriptive board-name must match its DTB. */
		name = fdt_getprop(catalog, child, "board-name", &name_len);
		if (!name || name_len < 6 ||
		    strnlen(name, name_len) != (size_t)(name_len - 1) ||
		    strncmp(name, "EVER_", 5))
			return -EINVAL;
		{
			const char *inner_name;
			int inner_len;

			inner_name = fdt_getprop(data, blob_root,
						 "hisi,boardname", &inner_len);
			if (!inner_name || inner_len != name_len ||
				memcmp(name, inner_name, name_len))
				return -EINVAL;
		}

		if (!memcmp(id, boot_id, 4 * sizeof(fdt32_t))) {
			winner = data;
			size = data_len;
			found++;
		}
	}
	if (child != -FDT_ERR_NOTFOUND)
		return child;
	if (!count || found != 1) {
		pr_crit("Everest catalog: boardid not uniquely found (entries=%d, matches=%d)\n",
			count, found);
		return -ENOENT;
	}

	*chosen = winner;
	*chosen_size = size;
	pr_info("Everest catalog: selected boardid %u.%u.%u.%u from %d entries (%u bytes)\n",
		fdt32_to_cpu(boot_id[0]), fdt32_to_cpu(boot_id[1]),
		fdt32_to_cpu(boot_id[2]), fdt32_to_cpu(boot_id[3]),
		count, size);
	ret = 0;
	return ret;
}

#endif /* __HUAWEI_EVEREST_MULTIBOARD_H */
