#include "bob_component.h"

namespace bob
{
    YAML::Node slcc::slcc_database;

    slcc::slcc( fs::path file_path )
    {
        parse_file(file_path);
    }

    void slcc::parse_file( fs::path file_path )
    {
        this->file_path = file_path;
        std::string path_string = file_path.generic_string( );
        std::clog << "Parsing '" << path_string << "'\n";

        try
        {
            yaml = YAML::LoadFile( path_string );
            const std::string id = (yaml["id"]) ? yaml["id"].as<std::string>() : yaml["name"].as<std::string>();
            yaml["id"] = id;
            for (const auto& p: yaml["provides"])
                slcc::slcc_database["features"][p["name"]] = id;

            slcc::slcc_database["components"][id] = yaml;
        }
        catch ( ... )
        {
            std::cerr << "Failed to load file: " << path_string << "\n";
            return;
        }

    }

    void slcc::apply_feature( std::string feature_name, component_list_t& new_components, feature_list_t& new_features )
    {
        // Nothing to do
    }

    void slcc::process_requirements(const YAML::Node& node, component_list_t& new_components, feature_list_t& new_features )
    {

    }

    component_list_t slcc::get_required_components( )
    {
        return {};
    }

    feature_list_t   slcc::get_required_features( )
    {
        feature_list_t list;
        for (const auto& f : yaml["requires"]["features"])
            list.insert(f.Scalar());

        return list;
    }

    std::vector<blueprint_match> slcc::get_blueprints( )
    {
        return {};
    }

    void slcc::convert_to_bob()
    {
        // Process 'provides'
        {
            YAML::Node provides;
            for (const auto& p : yaml["provides"] )
            {
                provides["features"].push_back( p["name"] );
            }
            yaml["provides"] = provides;
        }

        // Process 'requires'
        {
            YAML::Node requires;
            for ( const auto& p : yaml["requires"] )
            {
                requires["features"].push_back( p["name"] );
            }
            yaml["requires"] = requires;
        }

        // Process 'source'
        {
            YAML::Node sources;
            for ( const auto& p : yaml["source"] )
                sources.push_back(p["path"]);
            yaml["sources"] = sources;
            yaml.remove("source");
        }

        // Process 'include'
        {
            YAML::Node includes;
            for ( const auto& p : yaml["include"] )
            {
                if (p["condition"])
                    for (auto& c: p["condition"])
                        yaml["supports"][c.as<std::string>()]["includes"] = p["path"];
                else
                    includes["global"].push_back(p["path"]);
            }
            yaml["includes"] = includes;
            yaml.remove("include");
        }

    }
}
