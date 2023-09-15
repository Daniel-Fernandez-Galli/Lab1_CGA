#ifndef KDTREE_H
#define KDTREE_H

#include "Photon.h"
#include "nanoflann.hpp"

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
		return photons[idx].get_position().data[dim];
	}
};

using namespace nanoflann;

using Index_t = KDTreeSingleIndexAdaptor<L2_Simple_Adaptor<float, PhotonMap>, PhotonMap, 3>;

struct SearchResult {
	int index;
	float distance_squared;
};

class KDTree {

private:

	std::unique_ptr<Index_t> index;

	PhotonMap map;

public:

	void init(std::vector<Photon> photons);

	std::vector<SearchResult> search(const Vector& query, float radius_squared = std::numeric_limits<float>::infinity());

	Photon get_photon(int index);

};

#endif