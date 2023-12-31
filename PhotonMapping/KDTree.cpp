#include "KDTree.h"

#include <nanoflann.hpp>

using namespace nanoflann;

using Index_t = KDTreeSingleIndexAdaptor<L2_Simple_Adaptor<float, PhotonMap>, PhotonMap, 3>;

KDTree::KDTree(std::vector<Photon> photons)
{
	map.photons = photons;

	index = std::make_shared<Index_t>(3, map, KDTreeSingleIndexAdaptorParams(10 /* max leaf */));

	static_cast<Index_t*>(index.get())->buildIndex();
}

std::vector<SearchResult> KDTree::search_radius(const Vector3& query, float radius_squared) const
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

std::vector<SearchResult> KDTree::search_nearest(const Vector3& query, unsigned int N) const
{
	size_t size = std::min((size_t)N, map.photons.size());
	if (size == 0) {
		return {};
	}
	std::vector<uint32_t> indices(size);
	std::vector<float>    distances(size);
	static_cast<Index_t*>(index.get())->knnSearch(&query.x, size, &indices[0], &distances[0]);

	std::vector<SearchResult> indices_and_distances(size);
	for (int i = 0; i < indices.size(); i++) {
		SearchResult sr = { indices[i], distances[i] };
		indices_and_distances.push_back(sr);
	}

	return indices_and_distances;
}

Photon KDTree::get_photon(int index) const
{
	return map.photons[index];
}
