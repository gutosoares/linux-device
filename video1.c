#include "video.h"
#define VIDEO1_LEN 1910025
struct video video1 = {
	.name = "Santa is a Modafoker",
	.len  = VIDEO1_LEN,
	.data = {
#include "video1data.h"
		}
};
