#pragma once

struct MHD_Connection;

/// @brief Main connection method
extern 
enum MHD_Result 
answer_to_connection(void *cls,
    struct MHD_Connection *connection,
    const char *url,
    const char *method,
    const char *version,
    const char *upload_data,
    size_t *upload_data_size,
    void **con_cls);