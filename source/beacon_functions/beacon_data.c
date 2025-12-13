#include "beacon_compatibility.h"

VOID beacon_data_parse(datap* parser, PCHAR buffer, INT size) {
    if (parser == NULL || buffer == NULL) {
        return;
    }

    parser->original = buffer;
    parser->buffer = buffer;
    parser->length = size - 4;
    parser->size = size - 4;
    parser->buffer += 4;
    return;
}

INT beacon_data_int(datap* parser) {
    if (parser == NULL) {
        return 0;
    }

    INT32 fourbyteint = 0;
    if (parser->length < 4) {
        return 0;
    }
    RtlMoveMemory(&fourbyteint, parser->buffer, 4);
    parser->buffer += 4;
    parser->length -= 4;
    return (INT)fourbyteint;
}

SHORT beacon_data_short(datap* parser) {
    if (parser == NULL) {
        return 0;
    }

    INT16 retvalue = 0;
    if (parser->length < 2) {
        return 0;
    }
    RtlMoveMemory(&retvalue, parser->buffer, 2);
    parser->buffer += 2;
    parser->length -= 2;
    return (SHORT)retvalue;
}

INT beacon_data_length(datap* parser) {
    if (parser == NULL) {
        return 0;
    }

    return parser->length;
}

PCHAR beacon_data_extract(datap* parser, PINT size) {
    if (parser == NULL) {
        return NULL;
    }

    UINT32 length = 0;
    PCHAR outdata = NULL;
    /*Length prefixed binary blob, going to assume uint32_t for this.*/
    if (parser->length < 4) {
        return NULL;
    }
    RtlMoveMemory(&length, parser->buffer, 4);
    parser->buffer += 4;

    outdata = parser->buffer;
    if (outdata == NULL) {
        return NULL;
    }
    parser->length -= 4;
    parser->length -= length;
    parser->buffer += length;
    if (size != NULL && outdata != NULL) {
        *size = length;
    }
    return outdata;
}