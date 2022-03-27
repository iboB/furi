#include <stdint.h>
#include <stddef.h>

#include <furi/furi.h>

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    const char* sdata = (const char*)data;
    furi_sv in = furi_make_sv(sdata, sdata + size);
    furi_uri_split s = furi_split_uri(in);
    (void)s;
    return 0;
}
