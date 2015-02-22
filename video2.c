#include "video.h"
#define VIDEO2_LEN 714481
struct video video2 = {
	.name = "Picapau rules",
	.len  = VIDEO2_LEN,
	.data = {
#include "video2data.h"
		}
};
