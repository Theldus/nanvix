#include "pdcnanvix.h"

/* global clipboard contents, should be NULL if none set */

static char *pdc_NANVIX_clipboard = NULL;

int PDC_getclipboard(char **contents, long *length)
{
    int len;

    PDC_LOG(("PDC_getclipboard() - called\n"));

    if (!pdc_NANVIX_clipboard)
        return PDC_CLIP_EMPTY;

    len = strlen(pdc_NANVIX_clipboard);
    *contents = malloc(len + 1);
    if (!*contents)
        return PDC_CLIP_MEMORY_ERROR;

    strcpy(*contents, pdc_NANVIX_clipboard);
    *length = len;

    return PDC_CLIP_SUCCESS;
}

int PDC_setclipboard(const char *contents, long length)
{
    PDC_LOG(("PDC_setclipboard() - called\n"));

    if (pdc_NANVIX_clipboard)
    {
        free(pdc_NANVIX_clipboard);
        pdc_NANVIX_clipboard = NULL;
    }

    if (contents)
    {
        pdc_NANVIX_clipboard = malloc(length + 1);
        if (!pdc_NANVIX_clipboard)
            return PDC_CLIP_MEMORY_ERROR;

        strcpy(pdc_NANVIX_clipboard, contents);
    }

    return PDC_CLIP_SUCCESS;
}

int PDC_freeclipboard(char *contents)
{
    PDC_LOG(("PDC_freeclipboard() - called\n"));

    /* should we also free empty the system clipboard? probably not */

    if (contents)
    {
        /* NOTE: We free the memory, but we can not set caller's pointer
           to NULL, so if caller calls again then will try to access
           free'd memory.  We 1st overwrite memory with a string so if
           caller tries to use free memory they won't get what they
           expect & hopefully notice. */

        /* memset(contents, 0xFD, strlen(contents)); */

        if (strlen(contents) >= strlen("PDCURSES"))
            strcpy(contents, "PDCURSES");

        free(contents);
    }

    return PDC_CLIP_SUCCESS;
}

int PDC_clearclipboard(void)
{
    PDC_LOG(("PDC_clearclipboard() - called\n"));

    if (pdc_NANVIX_clipboard)
    {
        free(pdc_NANVIX_clipboard);
        pdc_NANVIX_clipboard = NULL;
    }

    return PDC_CLIP_SUCCESS;
}
