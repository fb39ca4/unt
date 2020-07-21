#pragma once

#include <type_traits>

template <int... X>
struct type_list;

template<typename T>
struct is_type_list : std::false_type {};

template <int... X>
struct is_type_list<type_list<X...>> : std::true_type {};

template <int... X>
struct type_list {
	template <int best_value, size_t best_back_idx>
	struct min_helper {
		static constexpr int value = best_value;
		static constexpr int back_idx = best_back_idx;
	};

	template<typename OtherList, typename = typename std::enable_if<is_type_list<OtherList>::value, OtherList>::type >
	using concat = typename std::enable_if<is_type_list<OtherList>::value, OtherList>::type;

	template<size_t remove_back_idx>
	struct remove_at_helper {
		using result = type_list<>;
	};

	using sorted = type_list<>;
};

template <int X0, int... X>
struct type_list<X0, X...> {
	static constexpr size_t size = sizeof...(X) + 1;

	using tail = type_list<X...>;

	template <int best_value, size_t best_back_idx>
	struct min_helper {
		static constexpr int value = (best_value < X0) ? tail::template min_helper<best_value, best_back_idx>::value : tail::template min_helper<X0, size>::value;
		static constexpr int back_idx = (best_value < X0) ? tail::template min_helper<best_value, best_back_idx>::back_idx : tail::template min_helper<X0, size>::back_idx;
	};

	static constexpr int min = min_helper<X0, size>::value;
	static constexpr int min_idx = size - min_helper<X0, size>::back_idx;

	template<typename OtherList>
	struct concat_helper {};

	template<int... OtherX>
	struct concat_helper<type_list<OtherX...>> {
		using result = type_list<X0, X..., OtherX...>;
	};

	template<typename OtherList>
	using concat = typename concat_helper<OtherList>::result;

	template<size_t remove_back_idx>
	struct remove_at_helper {
		using result = typename std::conditional<
			remove_back_idx == sizeof...(X) + 1,
			type_list<X...>,
			typename type_list<X0>::template concat<typename tail::template remove_at_helper<remove_back_idx>::result>
		>::type;
	};

	template<size_t idx>
	using remove_at = typename remove_at_helper<size - idx>::result;

	using sorted = typename type_list<min>::template concat<typename remove_at<min_idx>::sorted>;
};

// struct type_list_append

using l = type_list<4,3,1,5,6,2>;
static_assert(l::min == 1, "");
static_assert(l::min_idx == 2, "");

using l1 = type_list<1,2,3>;
using l2 = type_list<4,5,6>;
using l3 = type_list<1,2,3,4,5,6>;

static_assert(std::is_same<l1::concat<l2>::concat<type_list<>>, type_list<>::concat<l3>>::value, "");
static_assert(std::is_same<l1::remove_at<0>, type_list<2,3>>::value, "");
static_assert(std::is_same<l1::remove_at<1>, type_list<1,3>>::value, "");
static_assert(std::is_same<l1::remove_at<2>, type_list<1,2>>::value, "");

static_assert(std::is_same<l::sorted, l3>::value, "");

// type_list<2,1>::sorted;
using a = type_list<2,1>;
using b = a::remove_at<1>;
constexpr auto min = a::min;
constexpr auto min_idx = a::min_idx;
static_assert(min_idx == 1, "");
static_assert(a::remove_at<1>::min == 2, "");
