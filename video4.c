#include "video.h"
#define VIDEO4_LEN 800884
struct video video4 = {
	.name = "Saint Saya Intro",
	.len  = VIDEO4_LEN,
	.data = {
#include "video4data.h"
		}
};
