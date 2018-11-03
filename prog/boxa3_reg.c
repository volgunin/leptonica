/*====================================================================*
 -  Copyright (C) 2001 Leptonica.  All rights reserved.
 -
 -  Redistribution and use in source and binary forms, with or without
 -  modification, are permitted provided that the following conditions
 -  are met:
 -  1. Redistributions of source code must retain the above copyright
 -     notice, this list of conditions and the following disclaimer.
 -  2. Redistributions in binary form must reproduce the above
 -     copyright notice, this list of conditions and the following
 -     disclaimer in the documentation and/or other materials
 -     provided with the distribution.
 -
 -  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 -  ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 -  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 -  A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL ANY
 -  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 -  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 -  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 -  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 -  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 -  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 -  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *====================================================================*/

/*
 *  boxa3_reg.c
 *
 *  Higher-level operations that can search for anomalous sized boxes
 *  in a boxa, where the widths and heights of the boxes are expected
 *  to be similar.  These can be corrected by moving the appropriate
 *  sides of the anomalous boxes.
 */

#include "allheaders.h"

static const char  *boxafiles[3] = {"boxap1.ba", "boxap2.ba", "boxap3.ba"};

void static TestBoxa(L_REGPARAMS *rp, l_int32 index);
static l_float32  devwidth[3] = {0.0864, 0.0895, 0.1174};
static l_float32  devheight[3] = {0.0048, 0.0294, 0.0023};


l_int32 main(int    argc,
             char **argv)
{
l_int32       i;
L_REGPARAMS  *rp;

    if (regTestSetup(argc, argv, &rp))
        return 1;

    for (i = 0; i < 3; i++)
        TestBoxa(rp, i);
    return regTestCleanup(rp);
}


void static
TestBoxa(L_REGPARAMS  *rp,
         l_int32       index)
{
l_uint8   *data;
l_int32    w, h, medw, medh;
size_t     size;
l_float32  scalefact, devw, devh, ratiowh;
BOXA      *boxa1, *boxa2, *boxa3;
PIX       *pix1;

    boxa1 = boxaRead(boxafiles[index]);

        /* Read and display initial boxa */
    boxaGetExtent(boxa1, &w, &h, NULL);
    scalefact = 100.0 / (l_float32)w;
    boxa2 = boxaTransform(boxa1, 0, 0, scalefact, scalefact);
    boxaWriteMem(&data, &size, boxa2);
    regTestWriteDataAndCheck(rp, data, size, "ba");  /* 0, 10, 20 */
    lept_free(data);
    pix1 = boxaDisplayTiled(boxa2, NULL, 0, -1, 2200, 2, 1.0, 0, 3, 2);
    regTestWritePixAndCheck(rp, pix1, IFF_PNG);   /* 1, 11, 21 */
    pixDisplayWithTitle(pix1, 0, 0, NULL, rp->display);
    pixDestroy(&pix1);

        /* Find the median sizes */
    boxaMedianDimensions(boxa2, &medw, &medh, NULL, NULL, NULL, NULL,
                         NULL, NULL);
    if (rp->display)
        fprintf(stderr, "median width = %d, median height = %d\n", medw, medh);

        /* Check for deviations from median by pairs */
    boxaEvalSizeConsistency(boxa2, &devw, &devh, 0);
    regTestCompareValues(rp, devwidth[index], devw, 0.001);  /* 2, 12, 22 */
    regTestCompareValues(rp, devheight[index], devh, 0.001);  /* 3, 13, 23 */
    if (rp->display)
        fprintf(stderr, "dev width = %7.4f, dev height = %7.4f\n", devw, devh);

        /* Reconcile widths */
    boxa3 = boxaReconcileSizeByMedian(boxa2, L_CHECK_WIDTH, 0.05, 1.03,
                                      NULL, NULL, &ratiowh);
    boxaWriteMem(&data, &size, boxa3);
    regTestWriteDataAndCheck(rp, data, size, "ba");  /* 4, 14, 24 */
    lept_free(data);
    pix1 = boxaDisplayTiled(boxa3, NULL, 0, -1, 2200, 2, 1.0, 0, 3, 2);
    regTestWritePixAndCheck(rp, pix1, IFF_PNG);   /* 5, 15, 25 */
    pixDisplayWithTitle(pix1, 500, 0, NULL, rp->display);
    if (rp->display)
        fprintf(stderr, "ratio median width/height = %6.3f\n", ratiowh);
    boxaDestroy(&boxa3);
    pixDestroy(&pix1);

        /* Reconcile heights */
    boxa3 = boxaReconcileSizeByMedian(boxa2, L_CHECK_HEIGHT, 0.05, 1.03,
                                      NULL, NULL, NULL);
    boxaWriteMem(&data, &size, boxa3);
    regTestWriteDataAndCheck(rp, data, size, "ba");  /* 6, 16, 26 */
    lept_free(data);
    pix1 = boxaDisplayTiled(boxa3, NULL, 0, -1, 2200, 2, 1.0, 0, 3, 2);
    regTestWritePixAndCheck(rp, pix1, IFF_PNG);   /* 7, 17, 27 */
    pixDisplayWithTitle(pix1, 1000, 0, NULL, rp->display);
    boxaDestroy(&boxa3);
    pixDestroy(&pix1);

        /* Reconcile both widths and heights */
    boxa3 = boxaReconcileSizeByMedian(boxa2, L_CHECK_BOTH, 0.05, 1.03,
                                      NULL, NULL, NULL);
    boxaWriteMem(&data, &size, boxa3);
    regTestWriteDataAndCheck(rp, data, size, "ba");  /* 8, 18, 28 */
    lept_free(data);
    pix1 = boxaDisplayTiled(boxa3, NULL, 0, -1, 2200, 2, 1.0, 0, 3, 2);
    regTestWritePixAndCheck(rp, pix1, IFF_PNG);   /* 9, 19, 29 */
    pixDisplayWithTitle(pix1, 1500, 0, NULL, rp->display);
    boxaDestroy(&boxa3);
    pixDestroy(&pix1);
    boxaDestroy(&boxa1);
    boxaDestroy(&boxa2);
    boxaDestroy(&boxa3);
}
