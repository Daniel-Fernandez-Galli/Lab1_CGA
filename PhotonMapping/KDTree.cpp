#include "KDTree.h"

#include <nanoflann.hpp>

using namespace nanoflann;

using Index_t = KDTreeSingleIndexAdaptor<L2_Simple_Adaptor<float, PhotonMap>, PhotonMap, 3>;

void KDTree::init(std::vector<Photon> photons) {

	map.photons = photons;

	index = std::make_shared<Index_t>(3, map, KDTreeSingleIndexAdaptorParams(10 /* max leaf */));

	static_cast<Index_t*>(index.get())->buildIndex();

}

std::vector<SearchResult> KDTree::search(const Vector3& query, float radius_squared) const
{
	if (radius_squared < 0) {
		throw std::domain_error("Error: Search radius cannot be a negative number");
	}

	std::vector<nanoflann::ResultItem<uint32_t, float>> indices_and_distances;

	nanoflann::SearchParameters params;
	params.sorted = true; // Sort by distance

	static_cast<Index_t*>(index.get())->radiusSearch(&query.x, radius_squared, indices_and_distances, params);
	
	return reinterpret_cast<std::vector<SearchResult>&>(indices_and_distances);
}

Photon KDTree::get_photon(int index) const
{
	return map.photons[index];
}
