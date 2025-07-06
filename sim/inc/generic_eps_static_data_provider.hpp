#include <sim_i_data_provider.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

namespace Nos3 {

    class GenericStaticDataProvider : public SimIDataProvider {
    public:
        GenericStaticDataProvider(const boost::property_tree::ptree& config)
                : SimIDataProvider(config), _index(0)
        {
            std::string filename = config.get<std::string>("simulator.hardware-model.static-data-file", "solar_vectors.csv");
            load_solar_vectors(filename);
            if (_solar_vectors.empty())
                std::cerr << "[GenericStaticDataProvider] Warning: No solar vector data loaded from " << filename << std::endl;
        }

        boost::shared_ptr<SimIDataPoint> get_data_point(void) const override {
            if (_solar_vectors.empty())
                return boost::shared_ptr<SimIDataPoint>(new Generic_epsDataPoint(0.0, 0.0, 0.0));
            // Clamp to last index if out of data
            const auto& v = _solar_vectors[std::min(_index, _solar_vectors.size()-1)];
            ++_index;
            return boost::shared_ptr<SimIDataPoint>(new Generic_epsDataPoint(v[0], v[1], v[2]));
        }

    private:
        mutable size_t _index;
        std::vector<std::vector<double>> _solar_vectors;

        void load_solar_vectors(const std::string& filename) {
            std::ifstream f(filename);
            std::string line;
            while (std::getline(f, line)) {
                std::istringstream ss(line);
                std::vector<double> v(3,0.0);
                if (!(ss >> v[0] >> v[1] >> v[2])) continue; // Skip bad lines
                _solar_vectors.push_back(v);
            }
        }
    };

}
