#pragma once

#include <numeric>
#include <cassert>

template<typename T>
class rational {
private:
	inline static constexpr T sign(T x) {
		return (x == 0) ? 0 : (x > 0 ? 1 : -1);
	}

	inline static constexpr T abs(T x) {
		return x < 0 ? -x : x;
	}

	inline static constexpr T pow(T x, T exponent) {
		if (exponent <= 0) {
			throw std::invalid_argument("x must be positive");
		}
		if (exponent == 1) {
			return x;
		}
		else if (exponent % 2 == 0) {
			return pow(x, exponent / 2) * pow(x, exponent / 2);
		}
		else {
			return x * pow(x, (exponent - 1) / 2) * pow(x, (exponent - 1) / 2);
		}
	}

	// Constructor without canonicalization for internal use
	inline constexpr rational<T>(bool unused, T num, T den)
		: num(num), den(den)
	{
		(void)unused;
	}

public:
	const T num;
	const T den;

	inline constexpr rational<T>(T num, T den)
		: num(sign(den) * num / std::gcd(num, den)), den(sign(den) * den / std::gcd(num, den))
	{
		if (den == 0) throw std::invalid_argument("Zero denominator in rational");
	}

	inline constexpr bool operator==(rational<T> other) const {
		return (num == other.num) && (den == other.den);
	}

	inline constexpr rational<T> operator+(rational<T> other) const {
		const T den_lcm = std::lcm(den, other.den);
		return rational<T>(true, (num * (den_lcm / den)) + (other.num * (den_lcm / other.den)), den_lcm);
	}

	inline constexpr rational<T> operator-() const {
		return rational<T>(true, -num, den);
	}

	inline constexpr rational<T> operator-(rational<T> other) const {
		return *this + -other;
	}

	inline constexpr rational<T> operator*(rational<T> other) const {
		const T gcd = std::gcd(num * other.num, den * other.den);
		return rational<T>(true, (num * other.num) / gcd, (den * other.den) / gcd);
	}

	inline constexpr rational<T> inv() const {
		return rational<T>(true, den * sign(num), num * sign(num));
	}

	inline constexpr rational<T> operator/(rational<T> other) const {
		const T gcd = std::gcd(num * other.den, den * other.num);
		return rational<T>(true, (num * other.den) / gcd, (den * other.num) / gcd);
	}

	inline constexpr rational<T> pow(T exponent) const {
		if (exponent == 0) {
			if (num != 0) {
				return rational<T>(1, 1);
			}
			else {
				throw std::invalid_argument("Cannot raise 0 to the 0th power");
			}
		}
		else if (exponent < 0) {
			return inv().pow(-exponent);
		}
		else {
			return rational<T>(true, pow(num, exponent), pow(den, exponent));
		}
	}

	inline constexpr T sign() const {
		return sign(num);
	}

	inline constexpr rational<T> abs() const {
		return rational<T>(true, std::abs(num), std::abs(den));
	}

	inline constexpr rational<T> num_only() const {
		return rational<T>(true, num, 1);
	}

	inline constexpr rational<T> den_only() const {
		return rational<T>(true, 1, den);
	}
};

namespace rational_tests {
	using q = rational<long int>;

	constexpr q a(1,3);
	constexpr q b(1,4);

	static_assert(a + b == q(7,12));
	static_assert(a - b == q(1,12));
	static_assert(a * b == q(1,12));
	static_assert(a / b == q(4,3));
	static_assert(a.pow(3) == q(1,27));
}
/*
template<typename T>
class rational_radical {
private:
	constexpr rational_radical(true,  T radical, rational<T> base) : radical(radical), base(base) {};
public:
	T radical;
	rational<T> base;

	constexpr rational_radical(T radical, rational<T> base) : radical(radical), base(base) {};
	// constexpr rational_radical(T radical, T base_num, T base_den) : radical(radical), base(base_num, base_den) {};

	constexpr rational_radical<T> simplify() const {
		if (radical < 0) {
			return rational_radical<T>(true, -radical, base.inv()).simplify();
		}
		else {
			return *this;
		}
	}

	constexpr rational_radical<T> pow(rational<T> exponent) const {
		return rational_radical<T>(true, radical * exponent.den, base.pow(exponent.num));
	}

	constexpr rational_radical<T> mul(rational_radical<T> other) const {
		const T lcm_radical = std::lcm(radical, other.radical);
		return rational_radical<T>(true, lcm_radical, base.pow(lcm_radical / radical) * other.base.pow(lcm_radical / other.radical));
	}
};

namespace rational_radical_tests{
	using q = rational<long int>;
	using r = rational_radical<long int>;

	constexpr r a(1, q(2,1));
	constexpr r b = a.simplify();
}*/