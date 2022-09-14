#pragma once

struct std_string // might remove in the future
{
    char* buffer;
    char sso_buffer[12];
    size_t size;
    size_t capacity;
};