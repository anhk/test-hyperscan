

#include "hs.h"

#include <stdio.h>
#include <string.h>

static const char *patterns[] = {
    "http://foo",
    "http://.*bar",
};

char buff[] = "333http://foobar";

#define dd(fmt, ...) do {               \
    fprintf(stderr, "[%s:%d] "fmt"\n",  \
            __FUNCTION__, __LINE__, ##__VA_ARGS__); \
} while (0)



/**
 * HS_FLAG_CASELESS     :
 * HS_FLAG_MULTILINE    :
 * HS_FLAG_DOTALL       :
 * HS_FLAG_SINGLEMATCH  :
 * HS_FLAG_ALLOWEMPTY   :
 * HS_FLAG_UTF8         :
 * HS_FLAG_UCP          :
 **/
static const unsigned int flags[] = {
    HS_FLAG_CASELESS,
    HS_FLAG_CASELESS
};


static const unsigned int ids[] = {
    10000,
    10001,
};

static int onMatch(unsigned int id, unsigned long long from, unsigned long long to,
        unsigned int flags, void *ctx)
{
    dd("id=%d, from=%llx, to=%llu, flags=%x, ctx=%p",
            id, from, to, flags, ctx);
    return 0; // continue matching
}


int main(int argc, char **argv)
{
    hs_database_t *db_block = NULL;
    hs_compile_error_t *compileErr = NULL;
    hs_error_t err;

    hs_scratch_t *scratch = NULL;

    err = hs_compile_multi(patterns, flags, ids,
            sizeof(patterns) / sizeof(patterns[0]),
            HS_MODE_BLOCK, NULL, &db_block, &compileErr);

    if (err != HS_SUCCESS) {
        if (compileErr->expression < 0) {
            dd("ERROR: %s", compileErr->message);
        } else {
            dd("ERROR: Pattern '%s' failed compilation with error: %s",
                    patterns[compileErr->expression],
                    compileErr->message);
        }
        hs_free_compile_error(compileErr);
        exit(-1);
    }

    err = hs_alloc_scratch(db_block, &scratch);
    if (err != HS_SUCCESS) {
        dd("ERROR: could not allocate scratch space. Exiting.");
        exit(-1);
    }

    err = hs_scan(db_block, buff, strlen(buff), 0, scratch, onMatch, NULL);
    if (err != HS_SUCCESS) {
        dd("ERROR: Unable to scan. Exiting.");
        exit(-1);
    }

    hs_free_scratch(scratch);
    hs_free_database(db_block);

    return 0;
}


