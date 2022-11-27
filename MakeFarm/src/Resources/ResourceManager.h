#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <map>
#include <memory>

// It may not hold sf::Music as it is different starting with openFromFile()
// most because this type is rather streamed than stored

template<typename Resource, typename Identifier>
class ResourceManager
{
public:
    /**
     * \brief Checks an individual id and returns the resource assigned to it
     * \param id Identifier identifying a previously saved resource
     * \return the resource stored for the given id.
     *
     * Returns reference to Resource inside ResourceMap corresponding to given Identifier
     */
    Resource& getResourceReference(Identifier id);

    /**
     * \brief Checks an individual id and returns the resource assigned to it
     * \param id Identifier identifying a previously saved resource
     * \return the resource stored for the given id.
     *
     * Returns reference to Resource inside ResourceMap corresponding to given Identifier
     */
    const Resource& getResourceReference(Identifier id) const;

    /**
     * \brief Assigns a given id to a specific resource retrieved from the specified file
     * \param id Identifier to which the resource is to be assigned
     * \param path_to_file File path specifying the resource
     *
     * Stores the given texture resource inside the ResourceMap
     */
    void storeResource(Identifier id, const std::string& path_to_file);

    /**
     * \brief Assigns a given id to a specific resource retrieved from the specified file
     * \tparam Additional_Parameter Type of additional parameter (for example sf::Shared::Type)
     * \param id Identifier to which the resource is to be assigned
     * \param path_to_file File path specifying the resource
     * \param parameter Additional argument (fragment shader file path, or sf::IntRect)
     *
     * Stores the given texture resource inside the ResourceMap
     *
     * Specialized version of this function to carry one of the methods that sf::Shader
     * define -- which is loadFromFile() containing additional Fragment Shader File Path
     * or sf::Shader::Type. It also adds some other features which needs to be described
     * when using loadFromFile().
     */
    template<typename Additional_Parameter>
    void storeResource(Identifier id, const std::string& path_to_file,
                       const Additional_Parameter& parameter);


private:
    /**
     * \brief Map of id to given resource.
     *
     * Some object are really heavy, so it is better to store them just once
     * and to not load it multiple times.
     */
    std::map<Identifier, std::unique_ptr<Resource>> ResourceMap;
};


// ---------- Inline ------------ //

#include <cassert>
#include <stdexcept>

template<typename Resource, typename Identifier>
Resource& ResourceManager<Resource, Identifier>::getResourceReference(Identifier id)
{
    // To avoid some code redundance I use that ugly casts to perform
    // const version of getResourceReference, and then remove it's "constness"
    // back again. Ugly... I know, but redundant code is ugly too! :>
    return const_cast<Resource&>(
        static_cast<const ResourceManager&>(*this).getResourceReference(id));
}

template<typename Resource, typename Identifier>
const Resource& ResourceManager<Resource, Identifier>::getResourceReference(Identifier id) const
{
    // Tries to find a given Identifier, but throws an runtime_error
    // if there is no such one in a map. Ignoring it may lead to
    // errors as programmer probably does not know that it is valid
    auto found_texture = ResourceMap.find(id);

    // if (found_texture == ResourceMap.cend())
    //     throw std::runtime_error("Resource with given ID does not exist: " +
    //     std::to_string(static_cast<int>(id)));

    assert(found_texture != ResourceMap.cend());// Resource with given ID does not exist
    // I found this way better as probably end-user should not see such an errors that are
    // meant for the programmer. This error as it is assert occurs only in _DEBUG, and
    // for Release version of the program is optimized as it ignores this line.

    return *found_texture->second;
}

template<typename Resource, typename Identifier>
void ResourceManager<Resource, Identifier>::storeResource(Identifier id,
                                                          const std::string& path_to_file)
{
    // Stores a unique_pointer to new resource.
    std::unique_ptr<Resource> resource = std::make_unique<Resource>();

    // Loads the resource from the given filename
    if (!resource->loadFromFile(path_to_file))
    {
        throw std::runtime_error("This file does not exist: " + path_to_file);
    }

    // Inserts the resource to the map of Identifier to std::string by passing
    // the unique pointer by its rvalue (so the map takes the ownership from
    // the variable "resource" in it's value part of the pair transfered to it).
    auto inserted_resource = ResourceMap.insert(std::make_pair(id, std::move(resource)));

    // if (!inserted_resource.second)
    //     throw std::logic_error("Tried to insert resource multiple times: " + path_to_file);

    assert(inserted_resource.second);// Tried to insert resource multiple times
    // I found this way better as probably end-user should not see such an errors that are
    // meant for the programmer. This error as it is assert occurs only in _DEBUG, and
    // for Release version of the program is optimized as it ignores this line.
}

template<typename Resource, typename Identifier>
template<typename Additional_Parameter>
void ResourceManager<Resource, Identifier>::storeResource(Identifier id,
                                                          const std::string& path_to_file,
                                                          const Additional_Parameter& parameter)
{
    // Stores a unique_pointer to new resource.
    std::unique_ptr<Resource> resource = std::make_unique<Resource>();

    // Loads the resource from the given filename
    if (!resource->loadFromFile(path_to_file, parameter))
    {
        throw std::runtime_error("This file does not exist: " + path_to_file);
    }

    // Inserts the resource to the map of Identifier to std::string by passing
    // the unique pointer by its rvalue (so the map takes the ownership from
    // the variable "resource" in it's value part of the pair transfered to it).
    auto inserted_resource = ResourceMap.insert(std::make_pair(id, std::move(resource)));

    // if (!inserted_resource.second)
    //     throw std::logic_error("Tried to insert resource multiple times: " + path_to_file);

    assert(inserted_resource.second);// Tried to insert resource multiple times
    // I found this way better as probably end-user should not see such an errors that are
    // meant for the programmer. This error as it is assert occurs only in _DEBUG, and
    // for Release version of the program is optimized as it ignores this line.
}


#endif