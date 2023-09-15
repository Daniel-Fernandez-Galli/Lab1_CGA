#include "KDTree.h"

void KDTree::init(std::vector<Photon> photons) {

	map.photons = photons;

	index = std::make_unique<Index_t>(3, map, KDTreeSingleIndexAdaptorParams(10 /* max leaf */));

	index->buildIndex();

}

std::vector<SearchResult> KDTree::search(const Vector& query, float radius_squared)
{
	if (radius_squared < 0) {
		throw std::domain_error("Error: Search radius cannot be a negative number");
	}

	std::vector<nanoflann::ResultItem<uint32_t, float>> indices_and_distances;

	nanoflann::SearchParameters params;
	params.sorted = true; // Sort by distance

	index->radiusSearch(&query.data[0], radius_squared, indices_and_distances, params);

	return reinterpret_cast<std::vector<SearchResult>&>(indices_and_distances);
}

Photon KDTree::get_photon(int index)
{
	return map.photons[index];
}
