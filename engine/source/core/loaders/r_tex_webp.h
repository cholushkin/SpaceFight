#ifndef r_tex_webp_h__
#define r_tex_webp_h__

#ifdef __cplusplus
extern "C" {
#endif

extern unsigned char* webp_load_from_memory(unsigned char const *buffer, int len, int *x, int *y, int *bpp);
extern void webp_free(unsigned char*);

#ifdef __cplusplus
}
#endif

#endif // r_tex_webp_h__
