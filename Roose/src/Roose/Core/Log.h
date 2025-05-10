#pragma once

#define RS_INFO(...) fprintf(stdout, __VA_ARGS__), fprintf(stdout, "\n")
#define RS_ERROR(...) fprintf(stderr, __VA_ARGS__), fprintf(stderr, "\n")
