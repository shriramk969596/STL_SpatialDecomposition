#include <iostream>
#include "stl_utils.h"
using namespace std;

void Create_box_stl(std::string map_filename, double domain_min[3], double domain_max[3]) {
	std::ofstream file;


	boost::filesystem::path file_ex(map_filename);


	// if(boost::filesystem::exists(map_filename))
	// {
	//     boost::filesystem::remove(map_filename);
	// }

	// file.open(map_filename,std::ios::out|std::ios::app);
	file.open(map_filename,std::ios_base::app);
	file << "solid" << std::endl;

	file << " facet normal " << -1.0 << " " << 0.0 << " " << 0.0 << std::endl;
	file << "   outer loop" << std::endl;
	file << "     vertex " << domain_min[0] << " " << domain_max[1] << " " << domain_max[2] << std::endl;
	file << "     vertex " << domain_min[0] << " " << domain_max[1] << " " << domain_min[2] << std::endl;
	file << "     vertex " << domain_min[0] << " " << domain_min[1] << " " << domain_min[2] << std::endl;
	file << "   endloop" << std::endl;
	file << " endfacet" << std::endl;
	file << " facet normal " << -1.0 << " " << 0.0 << " " << 0.0 << std::endl;
	file << "   outer loop" << std::endl;
	file << "     vertex " << domain_min[0] << " " << domain_max[1] << " " << domain_max[2] << std::endl;
	file << "     vertex " << domain_min[0] << " " << domain_min[1] << " " << domain_min[2] << std::endl;
	file << "     vertex " << domain_min[0] << " " << domain_min[1] << " " << domain_max[2] << std::endl;
	file << "   endloop" << std::endl;
	file << " endfacet" << std::endl;

	file << " facet normal " << 1.0 << " " << 0.0 << " " << 0.0 << std::endl;
	file << "   outer loop" << std::endl;
	file << "     vertex " << domain_max[0] << " " << domain_min[1] << " " << domain_min[2] << std::endl;
	file << "     vertex " << domain_max[0] << " " << domain_max[1] << " " << domain_min[2] << std::endl;
	file << "     vertex " << domain_max[0] << " " << domain_max[1] << " " << domain_max[2] << std::endl;
	file << "   endloop" << std::endl;
	file << " endfacet" << std::endl;
	file << " facet normal " << 1.0 << " " << 0.0 << " " << 0.0 << std::endl;
	file << "   outer loop" << std::endl;
	file << "     vertex " << domain_max[0] << " " << domain_max[1] << " " << domain_max[2] << std::endl;
	file << "     vertex " << domain_max[0] << " " << domain_min[1] << " " << domain_max[2] << std::endl;
	file << "     vertex " << domain_max[0] << " " << domain_min[1] << " " << domain_min[2] << std::endl;
	file << "   endloop" << std::endl;
	file << " endfacet" << std::endl;


	file << " facet normal " << 0.0 << " " << -1.0 << " " << 0.0 << std::endl;
	file << "   outer loop" << std::endl;
	file << "     vertex " << domain_min[0] << " " << domain_min[1] << " " << domain_min[2] << std::endl;
	file << "     vertex " << domain_max[0] << " " << domain_min[1] << " " << domain_min[2] << std::endl;
	file << "     vertex " << domain_min[0] << " " << domain_min[1] << " " << domain_max[2] << std::endl;
	file << "   endloop" << std::endl;
	file << " endfacet" << std::endl;
	file << " facet normal " << 0.0 << " " << -1.0 << " " << 0.0 << std::endl;
	file << "   outer loop" << std::endl;
	file << "     vertex " << domain_max[0] << " " << domain_min[1] << " " << domain_max[2] << std::endl;
	file << "     vertex " << domain_min[0] << " " << domain_min[1] << " " << domain_max[2] << std::endl;
	file << "     vertex " << domain_max[0] << " " << domain_min[1] << " " << domain_min[2] << std::endl;
	file << "   endloop" << std::endl;
	file << " endfacet" << std::endl;

	file << " facet normal " << 0.0 << " " << 1.0 << " " << 0.0 << std::endl;
	file << "   outer loop" << std::endl;
	file << "     vertex " << domain_min[0] << " " << domain_max[1] << " " << domain_min[2] << std::endl;
	file << "     vertex " << domain_min[0] << " " << domain_max[1] << " " << domain_max[2] << std::endl;
	file << "     vertex " << domain_max[0] << " " << domain_max[1] << " " << domain_min[2] << std::endl;
	file << "   endloop" << std::endl;
	file << " endfacet" << std::endl;
	file << " facet normal " << 0.0 << " " << 1.0 << " " << 0.0 << std::endl;
	file << "   outer loop" << std::endl;
	file << "     vertex " << domain_max[0] << " " << domain_max[1] << " " << domain_max[2] << std::endl;
	file << "     vertex " << domain_max[0] << " " << domain_max[1] << " " << domain_min[2] << std::endl;
	file << "     vertex " << domain_min[0] << " " << domain_max[1] << " " << domain_max[2] << std::endl;
	file << "   endloop" << std::endl;

	file << " endfacet" << std::endl;

	file << " facet normal " << 0.0 << " " << 0.0 << " " << -1.0 << std::endl;
	file << "   outer loop" << std::endl;
	file << "     vertex " << domain_min[0] << " " << domain_min[1] << " " << domain_min[2] << std::endl;
	file << "     vertex " << domain_min[0] << " " << domain_max[1] << " " << domain_min[2] << std::endl;
	file << "     vertex " << domain_max[0] << " " << domain_min[1] << " " << domain_min[2] << std::endl;
	file << "   endloop" << std::endl;
	file << " endfacet" << std::endl;
	file << " facet normal " << 0.0 << " " << 0.0 << " " << -1.0 << std::endl;
	file << "   outer loop" << std::endl;
	file << "     vertex " << domain_max[0] << " " << domain_min[1] << " " << domain_min[2] << std::endl;
	file << "     vertex " << domain_max[0] << " " << domain_max[1] << " " << domain_min[2] << std::endl;
	file << "     vertex " << domain_min[0] << " " << domain_max[1] << " " << domain_min[2] << std::endl;
	file << "   endloop" << std::endl;
	file << " endfacet" << std::endl;

	file << " facet normal " << 0.0 << " " << 0.0 << " " << 1.0 << std::endl;
	file << "   outer loop" << std::endl;
	file << "     vertex " << domain_min[0] << " " << domain_min[1] << " " << domain_max[2] << std::endl;
	file << "     vertex " << domain_max[0] << " " << domain_min[1] << " " << domain_max[2] << std::endl;
	file << "     vertex " << domain_min[0] << " " << domain_max[1] << " " << domain_max[2] << std::endl;
	file << "   endloop" << std::endl;
	file << " endfacet" << std::endl;
	file << " facet normal " << 0.0 << " " << 0.0 << " " << 1.0 << std::endl;
	file << "   outer loop" << std::endl;
	file << "     vertex " << domain_max[0] << " " << domain_max[1] << " " << domain_max[2] << std::endl;
	file << "     vertex " << domain_min[0] << " " << domain_max[1] << " " << domain_max[2] << std::endl;
	file << "     vertex " << domain_max[0] << " " << domain_min[1] << " " << domain_max[2] << std::endl;
	file << "   endloop" << std::endl;
	file << " endfacet" << std::endl;


	file << "endsolid" << std::endl;
	file.close();
}

void Create_tria_stl(std::string map_filename, double v0[3], double v1[3], double v2[3], double facet_normal[3]) {
	std::ofstream file;

	boost::filesystem::path file_ex(map_filename);

	// if(boost::filesystem::exists(map_filename))
	// {
	//     boost::filesystem::remove(map_filename);
	// }

	// file.open(map_filename,std::ios::out|std::ios::app);
	file.open(map_filename,std::ios_base::app);
	file << "solid" << std::endl;

	file << " facet normal " << facet_normal[0] << " " << facet_normal[1] << " " << facet_normal[2] << std::endl;
	file << "   outer loop" << std::endl;
	file << "     vertex " << v0[0] << " " << v0[1] << " " << v0[2] << std::endl;
	file << "     vertex " << v1[0] << " " << v1[1] << " " << v1[2] << std::endl;
	file << "     vertex " << v2[0] << " " << v2[1] << " " << v2[2] << std::endl;
	file << "   endloop" << std::endl;
	file << " endfacet" << std::endl;

	file << "endsolid" << std::endl;
	file.close();

}
