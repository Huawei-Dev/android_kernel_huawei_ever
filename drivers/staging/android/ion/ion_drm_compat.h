/* Kirin980 DRM Gralloc EMUI10 -> ION/TZMP2 EMUI9.1 compatibility. */
#ifndef _ION_DRM_COMPAT_H
#define _ION_DRM_COMPAT_H

#include <linux/ioctl.h>
#include <linux/types.h>

/* Fixed-width layout, identical for 32-bit and 64-bit callers. */
struct ion_drm_map_data {
	__s32 fd;
	__u32 requested_size;
	__u32 iova;
	__u32 mapped_size;
	__u32 flags;
};

/* Extension opcode: check locally for conflicts before integrating. */
#define ION_IOC_DRM_MAP _IOWR('I', 0x40, struct ion_drm_map_data)

#ifdef __KERNEL__
struct dma_buf;
int ion_drm_map_fd(int fd, u32 requested_size,
                   u32 *iova, u32 *mapped_size);
#ifdef CONFIG_ION_HISI_SECSG
int ion_secsg_map_iommu_dmabuf(struct dma_buf *dmabuf, u32 *iova);
#endif
#endif

#endif /* _ION_DRM_COMPAT_H */
