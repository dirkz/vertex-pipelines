#include "Vertex.h"

namespace dxultra
{

bool Vertex::Equal::operator()(const Vertex &v1, const Vertex &v2) const
{
    return v1.Values() == v2.Values();
}

template <class T> inline void hash_combine(std::size_t &seed, const T &v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

std::size_t Vertex::Hash::operator()(const Vertex &v) const
{
    std::size_t hashValue = 23;
    for (float f : v.Values())
    {
        hash_combine(hashValue, f);
    }
    return hashValue;
}

} // namespace dxultra