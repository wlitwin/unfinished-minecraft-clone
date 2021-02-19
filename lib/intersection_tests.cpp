#include "intersection_tests.hpp"

#include <algorithm>
#include <cstdio>

bool intersect_ray_aabb(const Aabb& aabb, const Ray& ray, float& tmin, float& tmax)
{
    using std::min;
    using std::max;

    float t1 = (aabb.min.x - ray.origin.x)*ray.dir_inv.x;
    float t2 = (aabb.max.x - ray.origin.x)*ray.dir_inv.x;

	//if (std::isnan(t1)) t1 = -INFINITY;
	//if (std::isnan(t2)) t2 = INFINITY;
	if (std::isnan(t1) || std::isnan(t2)) {
		t1 = -INFINITY;
		t2 = INFINITY;
	}

    tmin = min(t1, t2); 
    tmax = max(t1, t2);

    for (int i = 1; i < 3; ++i)
    {
        t1 = (aabb.min[i] - ray.origin[i])*ray.dir_inv[i];
        t2 = (aabb.max[i] - ray.origin[i])*ray.dir_inv[i];

		//if (std::isnan(t1)) t1 = -INFINITY;
		//if (std::isnan(t2)) t2 = INFINITY;
		if (std::isnan(t1) || std::isnan(t2)) {
			t1 = -INFINITY;
			t2 = INFINITY;
		}

        tmin = max(tmin, min(min(t1, t2), tmax));
        tmax = min(tmax, max(max(t1, t2), tmin));
    }

    return tmax > max(tmin, 0.f);
}
/*
bool intersect_ray_aabb(const Aabb& aabb, const Ray& r, float& tmin, float& tmax)
{
	printf("Intersection start\n");
	tmin = (aabb.min.x - r.origin.x) * r.dir_inv.x;
	tmax = (aabb.max.x - r.origin.x) * r.dir_inv.x;

	printf("aabb.min.x - r.origin.x %.20f dir_inv is %.20f\n", (aabb.min.x - r.origin.x), r.dir_inv.x);

	if (tmin > tmax) std::swap(tmin, tmax);
	printf("TMIN0 %.20f\n", tmin);

	for (int i = 1; i < 3; ++i)
	{
		float t1 = (aabb.min[i] - r.origin[i]) / r.dir[i];
		float t2 = (aabb.max[i] - r.origin[i]) / r.dir[i];
		printf("t1 %.20f t2 %.20f\n", t1, t2);

		if (t1 > t2) std::swap(t1, t2);
		if ((tmin > t2) || (t1 > tmax))
		{	
			printf("Failed\n");
			return false;
		}
		if (t1 > tmin) tmin = t1;
		if (t2 < tmax) tmax = t2;
		printf("TMIN%d %.20f\n", i, tmin);
	}
	return true; 
}
*/

/*
	float tymin = (aabb.min.y - r.origin.y) / r.dir.y;
	float tymax = (aabb.max.y - r.origin.y) / r.dir.y;

	if (tymin > tymax) std::swap(tymin, tymax);

	if ((tmin > tymax) || (tymin > tmax))
		return false;

	if (tymin > tmin)
		tmin = tymin;

	if (tymax < tmax)
		tmax = tymax;

	float tzmin = (aabb.min.z - r.origin.z) / r.dir.z;
	float tzmax = (aabb.max.z - r.origin.z) / r.dir.z;

	if (tzmin > tzmax) std::swap(tzmin, tzmax);

	if ((tmin > tzmax) || (tzmin > tmax))
		return false;

	if (tzmin > tmin)
		tmin = tzmin;

	if (tzmax < tmax)
		tmax = tzmax;
		*/


void intersect_ray_aabb(const Aabb& aabb, RayResult& result)
{
    result.intersected = intersect_ray_aabb(aabb, result.ray, result.tmin, result.tmax);
}
