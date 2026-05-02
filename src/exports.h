#ifndef EXPORT_CC4B_H
#define EXPORT_CC4B_H

#if defined(CC4B_EXPORT)
    #define CC4B_API __declspec(dllexport)
#elif defined(CC4B_STATIC)
    #define CC4B_API
#else
    #define CC4B_API __declspec(dllimport)
#endif

#endif