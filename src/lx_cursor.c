/* Copyright (c) 2003-2008 Advanced Micro Devices, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 * Neither the name of the Advanced Micro Devices, Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 * */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "xf86.h"
#include "xf86Crtc.h"
#include "geode.h"

Bool
LXCursorInit(ScreenPtr pScrn)
{
    return xf86_cursors_init(pScrn, 32, 32,
	HARDWARE_CURSOR_BIT_ORDER_MSBFIRST |
	HARDWARE_CURSOR_TRUECOLOR_AT_8BPP |
	HARDWARE_CURSOR_SOURCE_MASK_NOT_INTERLEAVED);
}

void
LXLoadCursorImage(ScrnInfoPtr pScrni, unsigned char *src)
{
    int i, n, x, y, newX, newY;
    unsigned long andMask[32], xorMask[32];
    GeodeRec *pGeode = GEODEPTR(pScrni);
    unsigned long mskb, rowb;
    unsigned char *rowp = &src[0];
    unsigned char *mskp = &src[128];

    if (src != NULL) {
	mskb = rowb = 0;
	for (y = 32; --y >= 0;)
	    andMask[y] = xorMask[y] = 0;
	for (y = 0; y < 32; ++y) {
	    for (x = 0; x < 32; ++x) {
		if ((i = x & 7) == 0) {
		    rowb = (*rowp & *mskp);
		    mskb = ~(*mskp);
		    ++rowp;
		    ++mskp;
		}

		switch (pGeode->rotation) {
		default:
		    ErrorF("%s:%d invalid rotation %d\n", __func__, __LINE__,
			pGeode->rotation);
		case RR_Rotate_0:
		    newX = x;
		    newY = y;
		    break;
		case RR_Rotate_270:
		    newX = y;
		    newY = 31 - x;
		    break;
		case RR_Rotate_180:
		    newX = 31 - x;
		    newY = 31 - y;
		    break;
		case RR_Rotate_90:
		    newX = 31 - y;
		    newY = x;
		    break;
		}

		i = 7 - i;
		n = 31 - newX;
		andMask[newY] |= (((mskb >> i) & 1) << n);
		xorMask[newY] |= (((rowb >> i) & 1) << n);
	    }
	}
    } else {
	for (y = 32; --y >= 0;) {
	    andMask[y] = ~0;
	    xorMask[y] = 0;
	}
    }

    vg_set_mono_cursor_shape32(pGeode->CursorStartOffset, &andMask[0],
	&xorMask[0], 31, 31);
}
