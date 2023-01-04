#ifndef PairHash_h_INCLUDED
#define PairHash_h_INCLUDED

struct DumbPairHash {
public:
    template <typename T, typename U>
    std::size_t operator()(const std::pair<T, U> &x) const {
        return std::hash<T>()(x.first) ^ std::hash<U>()(x.second);
    }
};

#endif // PairHash_h_INCLUDED
