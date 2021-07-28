#ifndef COMPILER_H
#define COMPILER_H

#ifdef EXPORT
#define export __attribute__((used)) __attribute__ ((visibility ("default")))
#else
#define export
#endif

#endif
