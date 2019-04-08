


#include "Vec3.h"


void to_json(json& j, const Vec3& v) {
	j = json{ {"x", v[0]}, {"y", v[1]}, {"z", v[2]} };
}
void from_json(const json& j, Vec3& v) {
	j.at("x").get_to(v[0]);
	j.at("y").get_to(v[1]);
	j.at("z").get_to(v[2]);
}