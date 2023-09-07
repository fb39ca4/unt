#pragma once

#include "si.hpp"

namespace si {

	#define UNITS_SI_DECLARE_BASE(name, order) \
	namespace base { \
		struct name { \
			static constexpr intmax_t sort_order = (order); \
		}; \
	} \
	namespace base_dim { \
		using name = unt::dim<base::name>; \
	} \
	namespace units { \
		using name = unt::unit<unt::dim_set<unt::dim<base::name>>>; \
	} \
	namespace instances { \
		constexpr auto name = units::name::instance(); \
	}

	namespace units {
		using dimensionless = unt::unit<unt::dim_set<>>;
	}



	#define UNITS_SI_DECLARE_PREFIX(name, base_10_power) \
	namespace units { \
		using name = unt::unit<unt::dim_set<>, base_10_power>; \
	} \
	namespace instances { \
		constexpr auto name = units::name::instance(); \
	}

	#define UNITS_SI_DECLARE_DERIVED(name, unit_expression) \
	namespace instances { \
		constexpr auto name = decltype(unit_expression)::instance(); \
	} \
	namespace units { \
		using name = decltype(instances::name)::type; \
	}


	UNITS_SI_DECLARE_BASE(meter,    0);
	UNITS_SI_DECLARE_BASE(kilogram, 1);
	UNITS_SI_DECLARE_BASE(second,   2);
	UNITS_SI_DECLARE_BASE(ampere,   3);
	UNITS_SI_DECLARE_BASE(kelvin,   4);
	UNITS_SI_DECLARE_BASE(mole,     5);
	UNITS_SI_DECLARE_BASE(candela,  6);

	UNITS_SI_DECLARE_PREFIX(yotta,  24);
	UNITS_SI_DECLARE_PREFIX(zetta,  21);
	UNITS_SI_DECLARE_PREFIX(exa,    18);
	UNITS_SI_DECLARE_PREFIX(peta,   15);
	UNITS_SI_DECLARE_PREFIX(tera,   12);
	UNITS_SI_DECLARE_PREFIX(giga,    9);
	UNITS_SI_DECLARE_PREFIX(mega,    6);
	UNITS_SI_DECLARE_PREFIX(kilo,    3);
	UNITS_SI_DECLARE_PREFIX(hecto,   2);
	UNITS_SI_DECLARE_PREFIX(deka,    1);
	UNITS_SI_DECLARE_PREFIX(dimensionless, 0);
	UNITS_SI_DECLARE_PREFIX(deci,   -1);
	UNITS_SI_DECLARE_PREFIX(centi,  -2);
	UNITS_SI_DECLARE_PREFIX(milli,  -3);
	UNITS_SI_DECLARE_PREFIX(micro,  -6);
	UNITS_SI_DECLARE_PREFIX(nano,   -9);
	UNITS_SI_DECLARE_PREFIX(pico,  -12);
	UNITS_SI_DECLARE_PREFIX(femto, -15);
	UNITS_SI_DECLARE_PREFIX(atto,  -18);
	UNITS_SI_DECLARE_PREFIX(zepto, -21);
	UNITS_SI_DECLARE_PREFIX(yocto, -24);

	UNITS_SI_DECLARE_DERIVED(radian,    meter / meter);
	UNITS_SI_DECLARE_DERIVED(steradian, meter.pow<2> / meter.pow<2>)
	UNITS_SI_DECLARE_DERIVED(hertz,     second.pow<-1>);
	UNITS_SI_DECLARE_DERIVED(newton,    kilogram * meter / second.pow<2>);
	UNITS_SI_DECLARE_DERIVED(pascal,    newton / meter.pow<2>);
	UNITS_SI_DECLARE_DERIVED(joule,     newton * meter);
	UNITS_SI_DECLARE_DERIVED(watt,      joule / second);
	UNITS_SI_DECLARE_DERIVED(coulomb,   ampere * second);
	UNITS_SI_DECLARE_DERIVED(volt,      joule / coulomb);
	UNITS_SI_DECLARE_DERIVED(farad,     coulomb / volt);
	UNITS_SI_DECLARE_DERIVED(ohm,       volt / ampere);
	UNITS_SI_DECLARE_DERIVED(siemen,    dimensionless / ohm);
	UNITS_SI_DECLARE_DERIVED(weber,     joule / ampere);
	UNITS_SI_DECLARE_DERIVED(tesla,     volt * second / meter.pow<2>);
	UNITS_SI_DECLARE_DERIVED(henry,     volt * second / ampere);
	UNITS_SI_DECLARE_DERIVED(lumen,     candela * steradian);
	UNITS_SI_DECLARE_DERIVED(lux,       lumen / meter.pow<2>);
	UNITS_SI_DECLARE_DERIVED(gray,      joule / kilogram);
	UNITS_SI_DECLARE_DERIVED(sievert,   joule / kilogram);
	UNITS_SI_DECLARE_DERIVED(katal,     mole / second);

	UNITS_SI_DECLARE_DERIVED(bar,   hecto * kilo * pascal);
	UNITS_SI_DECLARE_DERIVED(gram,  milli * kilogram);

}

namespace test {
	using namespace unt;
	using namespace si;
	using namespace si::instances;

	//Test dimension comparison
	static_assert(unt::dim_cmp<base_dim::kilogram, base_dim::meter>::lt == false, "");
	static_assert(unt::dim_cmp<base_dim::kilogram, base_dim::meter>::le == false, "");
	static_assert(unt::dim_cmp<base_dim::kilogram, base_dim::meter>::gt == true,  "");
	static_assert(unt::dim_cmp<base_dim::kilogram, base_dim::meter>::ge == true,  "");
	static_assert(unt::dim_cmp<base_dim::kilogram, base_dim::meter>::eq == false, "");
	static_assert(unt::dim_cmp<base_dim::kilogram, base_dim::meter>::ne == true,  "");

	static_assert(unt::dim_cmp<base_dim::second, base_dim::second>::lt == false, "");
	static_assert(unt::dim_cmp<base_dim::second, base_dim::second>::le == true,  "");
	static_assert(unt::dim_cmp<base_dim::second, base_dim::second>::gt == false, "");
	static_assert(unt::dim_cmp<base_dim::second, base_dim::second>::ge == true,  "");
	static_assert(unt::dim_cmp<base_dim::second, base_dim::second>::eq == true,  "");
	static_assert(unt::dim_cmp<base_dim::second, base_dim::second>::ne == false, "");

	static_assert(kilogram == kilo * gram, "");


	using namespace si::instances;
	constexpr unt::quantity<units::meter, double> displacement = 0.5;
	constexpr unt::quantity<units::second, double> time = 2.0;

	constexpr auto velocity = displacement / time;

	constexpr auto length = 40 * centi * meter;
	static_assert(std::is_same<unt::quantity<decltype(centi * meter)::type, int>, decltype(length)::type>::value,"");

	// Not possible yet
	// static_assert(2 * meter < 300 * centi * meter, "");

}