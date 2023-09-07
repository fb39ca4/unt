#include <iostream>
#include <units.hpp>
#include <si.hpp>

/**
 * Operations:
 * unit * unit
 * unit / unit
 *
 * number * unit
 * unit * number
 * number / unit
 * unit / number
 *
 * quantity * unit
 * unit * quantity
 * quantity / unit
 * unit / quantity
 *
 * quantity * number
 * number * quantity
 * quantity / number
 * number / quantity
 *
 * quantity + quantity (must be same units)
 * quantity - quantity (must be same units)
 * quantity * quantity
 * quantity / quantity
 * quantity <=> 0
 * 0 <=> quantity
 *
 *
 */




int main(void) {
	using namespace si::instances;
	// constexpr auto force = 10.0 * newton;
	// constexpr auto length = 0.1 * meter;
	// constexpr auto area = length * length;
	// constexpr auto pressure = force / area;
	// constexpr auto pressure_2 = pressure + 1.0 * pascal;

	// constexpr auto a = meter;
	// constexpr auto b = meter * meter;

	// using M = decltype(a)::dims;
	// using A = decltype(a)::power<2>::dims;
	// using A2 = unt::dim_set<unt::dim<si::base::meter, 2, 1>>;


	// static_assert(std::is_same<A, A2>::value, "");


	std::cout << "Everything interesting happens at compile time for now." << std::endl;
	return 0;
}