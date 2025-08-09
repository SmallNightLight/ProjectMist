#pragma once

template<typename... Ts>
struct TypeList { };

//Count
template<typename List>
struct Count;

template<typename... Ts>
struct Count<TypeList<Ts...>>
{
    static constexpr std::size_t value = sizeof...(Ts);
};

template<typename List>
constexpr std::size_t Count_v = Count<List>::value;

//Total size (bytes)
template<typename... Ts>
constexpr size_t TotalSize_v = (sizeof(Ts) + ... + 0);

//IndexOf
template<typename T, typename List>
struct IndexOf;

template<typename T, typename... Ts>
struct IndexOf<T, TypeList<T, Ts...>>
{
    static constexpr std::size_t value = 0;
};

template<typename T, typename U, typename... Ts>
struct IndexOf<T, TypeList<U, Ts...>>
{
    static constexpr std::size_t value = 1 + IndexOf<T, TypeList<Ts...>>::value;
};

template<typename T, typename List>
constexpr std::size_t IndexOf_v = IndexOf<T, List>::value;

//Contains
template<typename T, typename List>
struct Contains;

template<typename T>
struct Contains<T, TypeList<>> : std::false_type { };

template<typename T, typename... Ts>
struct Contains<T, TypeList<T, Ts...>> : std::true_type { };

template<typename T, typename U, typename... Ts>
struct Contains<T, TypeList<U, Ts...>> : Contains<T, TypeList<Ts...>> { };

template<typename T, typename List>
constexpr bool Contains_v = Contains<T, List>::value;

//Alignment todo use
// template<typename T> struct MaxAlignImpl { static constexpr size_t value = alignof(T); };
// template<typename T, typename U, typename... Ts>
// struct MaxAlignImpl<T, U, Ts...> {
//     static constexpr size_t value = alignof(T) > MaxAlignImpl<U, Ts...>::value ? alignof(T) : MaxAlignImpl<U, Ts...>::value;
// };
// static constexpr size_t MaxAlign = MaxAlignImpl<System...>::value;

//GetOffsets
template<typename... Ts>
constexpr auto GetOffsets()
{
    std::array<std::size_t, sizeof...(Ts)> result { };

    std::size_t offset = 0;
    std::size_t i = 0;
    ((result[i++] = offset, offset += sizeof(Ts)), ...);

    return result;
}