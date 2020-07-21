#pragma once
#include <cstdint>
#include <ratio>
#include <type_traits>

#include <ctti/type_id.hpp>
#include <gcem.hpp>

namespace unt {



template<typename Id, intmax_t ExpNum=1, intmax_t ExpDen=1>
struct dim;

template<typename T>
struct is_dim : std::false_type {};

template<typename Id, intmax_t ExpNum, intmax_t ExpDen>
struct is_dim<dim<Id, ExpNum, ExpDen>> : std::true_type {};

template<typename Base, intmax_t ExpNum, intmax_t ExpDen>
struct dim {
	static_assert(ExpDen != 0, "Exponent denominator must be nonzero.");
	using exponent = std::ratio<ExpNum, ExpDen>;
	static_assert(std::ratio_multiply<exponent, std::ratio<1, 1>>::num == ExpNum, "Exponent must be in reduced rational form.");
	using base = Base;
	static constexpr intmax_t sort_order = base::sort_order;

	template<intmax_t Num, intmax_t Den>
	struct pow_helper {
		// static_assert(Den != 0, "Exponent denominator must be nonzero.");
		using new_ratio = std::ratio_multiply<exponent, std::ratio<Num, Den>>;
		using type = dim<base, new_ratio::num, new_ratio::den>;
	};

	template<intmax_t Num, intmax_t Den = 1>
	using pow = typename pow_helper<Num, Den>::type;
};

template<typename DimA, typename DimB>
struct dim_mul_helper {
	struct result {
		struct exponent;
	};
};

template<typename Base, intmax_t ExpNumA, intmax_t ExpDenA, intmax_t ExpNumB, intmax_t ExpDenB>
struct dim_mul_helper<dim<Base, ExpNumA, ExpDenA>, dim<Base, ExpNumB, ExpDenB>> {
private:
	using new_ratio = std::ratio_add<std::ratio<ExpNumA, ExpNumB>, std::ratio<ExpDenA, ExpDenB>>;
public:
	using result = dim<Base, new_ratio::num, new_ratio::den>;
};

template<typename DimA, typename DimB>
using dim_mul = typename dim_mul_helper<DimA, DimB>::result;

template<typename DimA, typename DimB>
struct dim_cmp;

template<typename BaseA, typename BaseB, intmax_t... ExpA, intmax_t... ExpB>
struct dim_cmp<dim<BaseA, ExpA...>, dim<BaseB, ExpB...>> :
std::integral_constant<int,
	(std::is_same<BaseA, BaseB>{}) ? 0 :
	dim<BaseA, ExpA...>::sort_order != dim<BaseB, ExpB...>::sort_order ?
		(dim<BaseA, ExpA...>::sort_order < dim<BaseB, ExpB...>::sort_order ? -1 : 1) :
		(ctti::unnamed_type_id<BaseA>().hash() == ctti::unnamed_type_id<BaseB>().hash() ? 2 :
			(ctti::unnamed_type_id<BaseA>().hash() < ctti::unnamed_type_id<BaseB>().hash() ? -1 : 1)
		)
> {
private:
	using this_cmp = dim_cmp<dim<BaseA, ExpA...>, dim<BaseB, ExpB...>>;
	static_assert(this_cmp::value != 2, "ctti hash collision");
	static_assert(!std::is_same<BaseA, BaseB>{} ?
		ctti::unnamed_type_id<BaseA>().hash() != ctti::unnamed_type_id<BaseB>().hash() : true, "ctti hash collision");
public:
	static constexpr bool lt = this_cmp::value < 0;
	static constexpr bool gt = this_cmp::value > 0;
	static constexpr bool eq = this_cmp::value == 0;
	static constexpr bool le = lt || eq;
	static constexpr bool ge = gt || eq;
	static constexpr bool ne = !eq;
};


template<typename... Dims>
struct dim_set;

template<typename T>
struct is_dim_set : std::false_type {};

template<typename... Dims>
struct is_dim_set<dim_set<Dims...>> : std::true_type {};

template<typename... Dims>
struct dim_set;

template<>
struct dim_set<> {
	static constexpr bool sorted = true;
};

template<typename Dim0>
struct dim_set<Dim0> {
	static_assert(is_dim<Dim0>{}, "dim_set instantiated with non-dimension argument");
	static constexpr bool sorted = true;
};

template<typename Dim0, typename Dim1, typename... Dims>
struct dim_set<Dim0, Dim1, Dims...> {
	static_assert(is_dim<Dim0>{}, "dim_set instantiated with non-dimension argument");
	static_assert(is_dim<Dim1>{}, "dim_set instantiated with non-dimension argument");
	static constexpr bool sorted = dim_cmp<Dim0, Dim1>::lt && dim_set<Dim1, Dims...>::sorted;
	static_assert(sorted, "dim_set instantiated without sorted order.");
};

template<typename DimSet, intmax_t ExpNum, intmax_t ExpDen>
struct dim_set_pow_helper;

template<intmax_t ExpNum, intmax_t ExpDen, typename... Dims>
struct dim_set_pow_helper<dim_set<Dims...>, ExpNum, ExpDen> {
	using result = dim_set<typename Dims::template pow<ExpNum, ExpDen>...>;
};

template<typename DimSet, intmax_t ExpNum, intmax_t ExpDen = 1>
using dim_set_pow = typename dim_set_pow_helper<DimSet, ExpNum, ExpDen>::result;

template<typename SetA, typename SetB, typename SetP = dim_set<>>
struct dim_set_mul_helper;

template<typename... DimsP>
struct dim_set_mul_helper<dim_set<>, dim_set<>, dim_set<DimsP...>> {
	using result = dim_set<DimsP...>;
};

template<typename FirstDimA, typename... RestDimsA, typename... DimsP>
struct dim_set_mul_helper<dim_set<FirstDimA, RestDimsA...>, dim_set<>, dim_set<DimsP...>> {
	using result = dim_set<DimsP..., FirstDimA, RestDimsA...>;
};

template<typename FirstDimB, typename... RestDimsB, typename... DimsP>
struct dim_set_mul_helper<dim_set<>, dim_set<FirstDimB, RestDimsB...>, dim_set<DimsP...>> {
	using result = dim_set<DimsP..., FirstDimB, RestDimsB...>;
};

template<typename FirstDimA, typename... RestDimsA, typename FirstDimB, typename... RestDimsB, typename... DimsP>
struct dim_set_mul_helper<dim_set<FirstDimA, RestDimsA...>, dim_set<FirstDimB, RestDimsB...>, dim_set<DimsP...>> {
	using cmp_first = dim_cmp<FirstDimA, FirstDimB>;
	using result = typename std::conditional<cmp_first::eq,
		typename std::conditional<std::is_same<typename dim_mul<FirstDimA, FirstDimB>::exponent, std::ratio<0,1>>::value,
			typename dim_set_mul_helper<dim_set<RestDimsA...>, dim_set<RestDimsB...>, dim_set<DimsP..., dim_mul<FirstDimA, FirstDimB>>>::result,
			typename dim_set_mul_helper<dim_set<RestDimsA...>, dim_set<RestDimsB...>, dim_set<DimsP...>>::result
		>::type,
		typename std::conditional<cmp_first::lt,
			typename dim_set_mul_helper<dim_set<RestDimsA...>, dim_set<FirstDimB, RestDimsB...>, dim_set<DimsP..., FirstDimA>>::result,
			typename dim_set_mul_helper<dim_set<FirstDimA, RestDimsA...>, dim_set<RestDimsB...>, dim_set<DimsP..., FirstDimB>>::result
		>::type
	>::type;
};

template<typename SetA, typename SetB>
using dim_set_mul = typename dim_set_mul_helper<SetA, SetB>::result;

template<typename DimSet, intmax_t ScaleExpNum = 0, intmax_t ScaleExpDen = 1>
struct unit;

template<typename... T>
struct is_unit : std::false_type {};

template<typename... T>
struct is_unit<unit<T...>> : std::true_type {};

template<typename T>
using enforce_unit = typename std::enable_if<is_unit<T>::value, T>::type;

template<typename DimSet, intmax_t ScaleExpNum, intmax_t ScaleExpDen>
struct unit {
    using type = unit<DimSet, ScaleExpNum, ScaleExpDen>;

	static_assert(ScaleExpDen != 0, "Exponent denominator must be nonzero.");
	using log_10_scale_ratio = std::ratio<ScaleExpNum, ScaleExpDen>;
	static_assert(std::ratio_multiply<log_10_scale_ratio, std::ratio<1>>::num == ScaleExpNum, "Exponent must be in reduced rational form.");

	using dims = DimSet;

    static constexpr double scale_factor = gcem::pow(10.0, static_cast<double>(ScaleExpNum) / static_cast<double>(ScaleExpDen));

    template<typename OtherUnit>
    struct equal {
        static constexpr bool value = std::is_same<type, typename OtherUnit::type>::value;
    };

    // template<typename OtherUnit>
    // struct compatible {
    //     static constexpr bool value = std::is_same<unit_t, std::remove_cv<OtherUnit>>::value;
    // };

    template<intmax_t ExpNum, intmax_t ExpDen = 1>
    using scale = unit<
		DimSet,
		std::ratio_add<log_10_scale_ratio, std::ratio<ExpNum, ExpDen>>::num,
		std::ratio_add<log_10_scale_ratio, std::ratio<ExpNum, ExpDen>>::den
	>;

	template<intmax_t ExpNum, intmax_t ExpDen = 1>
    using power = unit<
		dim_set_pow<DimSet, ExpNum, ExpDen>,
		std::ratio_multiply<log_10_scale_ratio, std::ratio<ExpNum, ExpDen>>::num,
		std::ratio_multiply<log_10_scale_ratio, std::ratio<ExpNum, ExpDen>>::den
	>;

    template<typename OtherUnit>
    using mul = unit<
		dim_set_mul<dims, typename OtherUnit::dims>,
		std::ratio_add<log_10_scale_ratio, typename OtherUnit::log_10_scale_ratio>::num,
		std::ratio_add<log_10_scale_ratio, typename OtherUnit::log_10_scale_ratio>::den
	>;

    template<typename OtherUnit>
    using div = mul<typename OtherUnit::template power<-1>>;

    static constexpr type instance() {
        return type();
    }

    constexpr bool operator==(const type&) const {
        return true;
    }

    template<typename OtherDimSet, intmax_t OtherNum, intmax_t OtherDen>
    constexpr bool operator==(const unit<OtherDimSet, OtherNum, OtherDen>&) const {
        return false;
    }

	template<typename OtherDimSet, intmax_t OtherNum, intmax_t OtherDen>
    constexpr bool operator!=(const unit<OtherDimSet, OtherNum, OtherDen>& rhs) const {
        return !operator==(rhs);
    }

	template<typename OtherDimSet, intmax_t OtherNum, intmax_t OtherDen>
    constexpr mul<unit<OtherDimSet, OtherNum, OtherDen>> operator*(const unit<OtherDimSet, OtherNum, OtherDen>&) const {
        return mul<unit<OtherDimSet, OtherNum, OtherDen>>::instance();
    }

	template<typename OtherDimSet, intmax_t OtherNum, intmax_t OtherDen>
    constexpr div<unit<OtherDimSet, OtherNum, OtherDen>> operator/(const unit<OtherDimSet, OtherNum, OtherDen>&) const {
        return div<unit<OtherDimSet, OtherNum, OtherDen>>::instance();
    }

    template<intmax_t Num, intmax_t Den = 1>
    constexpr static power<Num, Den> pow = power<Num, Den>::instance();

    template<intmax_t Num, intmax_t Den = 1>
    constexpr static scale<Num, Den> scl = scale<Num, Den>::instance();
};

template<typename Unit, typename ValueType = double>
struct quantity {
	using type = quantity<Unit, ValueType>;
    using unit_t = Unit;
    using value_t = ValueType;

    const value_t value;

    constexpr quantity(value_t value) : value(value) {};

    constexpr quantity(const quantity& other) : value(other.value) {};

    template<typename RHSUnit, typename RHSValueType>
    constexpr bool operator==(const quantity<RHSUnit, RHSValueType>& rhs) const {
        static_assert(Unit::template equal<RHSUnit>::value == true, "Cannot compare different units.");
        return value == rhs.value;
    }

	template<typename RHSUnit, typename RHSValueType>
    constexpr bool operator<(const quantity<RHSUnit, RHSValueType>& rhs) const {
        static_assert(Unit::template equal<RHSUnit>::value == true, "Cannot compare different units.");
        return value < rhs.value;
    }

	template<typename RHSUnit, typename RHSValueType>
    constexpr bool operator>(const quantity<RHSUnit, RHSValueType>& rhs) const {
        static_assert(Unit::template equal<RHSUnit>::value == true, "Cannot compare different units.");
        return value > rhs.value;
    }

	template<typename RHSUnit, typename RHSValueType>
    constexpr bool operator<=(const quantity<RHSUnit, RHSValueType>& rhs) const {
        static_assert(Unit::template equal<RHSUnit>::value == true, "Cannot compare different units.");
        return value <= rhs.value;
    }

	template<typename RHSUnit, typename RHSValueType>
    constexpr bool operator>=(const quantity<RHSUnit, RHSValueType>& rhs) const {
        static_assert(Unit::template equal<RHSUnit>::value == true, "Cannot compare different units.");
        return value >= rhs.value;
    }

    template<typename RHSUnit, typename RHSValueType>
    constexpr auto operator+(const quantity<RHSUnit, RHSValueType>& rhs) -> quantity<Unit, typename std::remove_cv<decltype(value + rhs.value)>::type> const {
        static_assert(Unit::template equal<RHSUnit>::value == true, "Cannot add different units together.");
        return quantity<Unit, typename std::remove_cv<decltype(value + rhs.value)>::type>(value + rhs.value);
    }

    template<typename RHSUnit, typename RHSValueType>
    constexpr auto operator-(const quantity<RHSUnit, RHSValueType>& rhs) -> quantity<Unit, typename std::remove_cv<decltype(value - rhs.value)>::type> const {
        static_assert(Unit::template equal<RHSUnit>::value == true, "Cannot subtract different units together.");
        return quantity<Unit, typename std::remove_cv<decltype(value - rhs.value)>::type>(value - rhs.value);
    }

    template<typename RHSUnit, typename RHSValueType>
    constexpr auto operator*(const quantity<RHSUnit, RHSValueType>& rhs) const -> quantity<typename Unit::template mul<RHSUnit>, typename std::remove_cv<decltype(value * rhs.value)>::type> {
        return quantity<typename Unit::template mul<RHSUnit>, typename std::remove_cv<decltype(value * rhs.value)>::type>(value * rhs.value);
    }

    template<typename RHSUnit, typename RHSValueType>
    constexpr auto operator/(const quantity<RHSUnit, RHSValueType>& rhs) const -> quantity<typename Unit::template div<RHSUnit>, typename std::remove_cv<decltype(value / rhs.value)>::type> {
        return quantity<typename Unit::template div<RHSUnit>, typename std::remove_cv<decltype(value / rhs.value)>::type>(value / rhs.value);
    }

    template<typename RHSDimTuple, intmax_t RHSExpNum, intmax_t RHSExpDen>
    constexpr auto operator*(const unit<RHSDimTuple, RHSExpNum, RHSExpDen>& rhs) -> quantity<typename Unit::template mul<unit<RHSDimTuple, RHSExpNum, RHSExpDen>>, ValueType> const {
        return quantity<typename Unit::template mul<unit<RHSDimTuple, RHSExpNum, RHSExpDen>>, ValueType>(value);
    }

    template<typename RHSDimTuple, intmax_t RHSExpNum, intmax_t RHSExpDen>
    constexpr auto operator/(const unit<RHSDimTuple, RHSExpNum, RHSExpDen>& rhs) -> quantity<typename Unit::template mul<unit<RHSDimTuple, RHSExpNum, RHSExpDen>>, ValueType> const {
        return quantity<typename Unit::template div<unit<RHSDimTuple, RHSExpNum, RHSExpDen>>, ValueType>(value);
    }

};

template<typename... T>
struct is_quantity : std::false_type {};

template<typename... T>
struct is_quantity<quantity<T...>> : std::true_type{};

template<typename T>
using enforce_quantity = typename std::enable_if<is_quantity<T>::value, T>::type;

template<typename T>
struct is_undimensioned : std::integral_constant<bool, !(is_quantity<T>{} || is_unit<T>{})> {};

template<typename T>
using enforce_undimensioned = typename std::enable_if<is_undimensioned<T>::value, T>::type;

template<typename RHSDimTuple, intmax_t RHSScaleNum, intmax_t RHScaleDen, typename ValueType, typename = enforce_undimensioned<ValueType>>
constexpr auto operator*(const ValueType& lhs, const unit<RHSDimTuple, RHSScaleNum, RHScaleDen>& rhs) -> quantity<unit<RHSDimTuple, RHSScaleNum, RHScaleDen>, ValueType> {
    return quantity<unit<RHSDimTuple, RHSScaleNum, RHScaleDen>, ValueType>(lhs);
}

template<typename RHSDimTuple, intmax_t RHSScaleNum, intmax_t RHSScaleDen, typename ValueType, typename = enforce_undimensioned<ValueType>>
constexpr auto operator/(const ValueType& lhs, const unit<RHSDimTuple, RHSScaleNum, RHSScaleDen>& rhs) -> quantity<typename unit<RHSDimTuple, RHSScaleNum, RHSScaleDen>::template power<-1>, ValueType> {
    return quantity<typename unit<RHSDimTuple, RHSScaleNum, RHSScaleDen>::template power<-1>, ValueType>(lhs);
}

template<typename RHSUnit, typename ValueType, typename = enforce_undimensioned<ValueType>>
constexpr auto operator*(const ValueType& lhs, const quantity<RHSUnit, ValueType>& rhs) -> quantity<RHSUnit, ValueType> {
    return quantity<RHSUnit, ValueType>(lhs * rhs.value);
}

template<typename RHSUnit, typename ValueType, typename = enforce_undimensioned<ValueType>>
constexpr auto operator/(const ValueType& lhs, const quantity<RHSUnit, ValueType>& rhs) -> quantity<RHSUnit, ValueType> {
    return quantity<RHSUnit, ValueType>(lhs / rhs.value);
}


}
