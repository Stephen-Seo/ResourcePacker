
#ifndef RESOURCE_PACKER_INTERNAL_HPP
#define RESOURCE_PACKER_INTERNAL_HPP

#include <iostream>

namespace RP
{
    bool isLittleEndian();

    template <typename T>
    void writeLittleEndian(std::ofstream& o, T data)
    {
        union
        {
            T i;
            char c[sizeof(T)];
        } bint;
        bint.i = data;

        if(!isLittleEndian() && sizeof(T) > 1)
        {
            char s;
            for(unsigned int i = 0; i < sizeof(T) / 2; ++i)
            {
                s = bint.c[i];
                bint.c[i] = bint.c[sizeof(T) - 1 - i];
                bint.c[sizeof(T) - 1 - i] = s;
            }
        }

        o.write((char*) &bint.i, sizeof(T));
    }

    template <typename T>
    T readLittleEndian(std::ifstream& i)
    {
        union
        {
            T i;
            char c[sizeof(T)];
        } bint;

        i.read((char*) &bint.i, sizeof(T));

        if(!isLittleEndian())
        {
            char s;
            for(unsigned int i = 0; i < sizeof(T) / 2; ++i)
            {
                s = bint.c[i];
                bint.c[i] = bint.c[sizeof(T) - 1 - i];
                bint.c[sizeof(T) - 1 - i] = s;
            }
        }

        return bint.i;
    }
}

#endif

