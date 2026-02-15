#pragma once
#include "sequence.hpp"
//good amount LLM usage here :(
//Every plugin .dylib must export these two C functions
extern "C" {
    typedef Sequence* (*CreateSequenceFn)();
    typedef void (*DestroySequenceFn)(Sequence*);
}

#define EXPORT_SEQUENCE(ClassName) \
    extern "C" Sequence* CreateSequence() { \
        return new ClassName(); \
    } \
    extern "C" void DestroySequence(Sequence* seq) { \
        delete seq; \
    }