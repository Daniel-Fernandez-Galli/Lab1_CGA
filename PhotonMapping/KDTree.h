#ifndef KDTREE_H
#define KDTREE_H

#include "Photon.h"

struct PhotonMap {

	std::vector<Photon> photons;

	/* Inlined functions must be defined in header */
	inline size_t kdtree_get_point_count() const {
		return photons.size();
	}
	template <class B>
	inline bool kdtree_get_bbox(B& bbox) const
	{
		return false; //default bounding box provided by nanoflann
	}
	inline float kdtree_get_pt(const size_t idx, const size_t dim) const
	{
		switch (dim) {
		case 0:
			return photons[idx].get_position().x;
		case 1:
			return photons[idx].get_position().y;
		case 2:
			return photons[idx].get_position().z;
		default:
			return 0;
		}
	}
};

struct SearchResult {
	int index;
	float distance_squared;
};

class KDTree {

private:

	std::shared_ptr<void> index; //Implementation dependent

	PhotonMap map;

public:

	void init(std::vector<Photon> photons);

	std::vector<SearchResult> search(const Vector3& query, float radius_squared = std::numeric_limits<float>::infinity());

	Photon get_photon(int index);

};

#endif