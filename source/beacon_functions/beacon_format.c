#include "beacon_compatibility.h"

/* format API */

VOID beacon_format_alloc(formatp* format, INT maxsz) {
    if (format == NULL) {
        return;
    }

    format->original = mcalloc(maxsz);
    format->buffer   = format->original;
    format->length   = 0;
    format->size     = maxsz;
    return;
}

VOID beacon_format_reset(formatp* format) {
    if (format == NULL) {
        return;
    }

    RtlZeroMemory(format->original, format->size);
    format->buffer = format->original;
    format->length = format->size;
    return;
}

VOID beacon_format_free(formatp* format) {
    if (format == NULL) {
        return;
    }
    if (format->original) {
        mcfree(format->original);
        format->original = NULL;
    }
    format->buffer = NULL;
    format->length = 0;
    format->size = 0;
    return;
}

VOID beacon_format_append(formatp* format, PCHAR text, INT len) {
    if (format == NULL || text == NULL) {
        return;
    }

    RtlMoveMemory(format->buffer, text, len);
    format->buffer += len;
    format->length += len;
    return;
}

VOID beacon_format_printf(formatp* format, PCHAR fmt, ...) {
    if (format == NULL || fmt == NULL) {
        return;
    }

    va_list args;
    INT length = 0;
    INT remaining = format->size - format->length;

    if (remaining <= 0) {
        return;
    }

    va_start(args, fmt);
    length = wvsprintfA(format->buffer, fmt, args);
    va_end(args);

    if (length < 0 || length >= remaining) {
        return;
    }

    format->length += length;
    format->buffer += length;
    return;
}

PCHAR beacon_format_to_string(formatp* format, PINT size) {
    if (format == NULL || size == NULL) {
        return NULL;
    }

    *size = format->length;
    return format->original;
}

VOID beacon_format_int(formatp* format, INT value) {
    if (format == NULL) {
        return;
    }

    UINT32 indata = value;
    UINT32 outdata = 0;
    if (format->length + 4 > format->size) {
        return;
    }
    outdata = swap_endianess(indata);
    RtlMoveMemory(format->buffer, &outdata, 4);
    format->length += 4;
    format->buffer += 4;
    return;
}