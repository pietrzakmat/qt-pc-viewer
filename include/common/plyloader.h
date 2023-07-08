#ifndef PLYLOADER_H
#define PLYLOADER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <memory>
#include <algorithm>
#include <thread>

#include <QVector3D>
#include <QVector4D>

#include <opengl_helper.hpp>

#include <tinyply.h>

namespace graphics {

inline std::vector<std::uint8_t> read_file_binary(const std::string & pathToFile)
{
    std::ifstream file(pathToFile, std::ios::binary);
    std::vector<std::uint8_t> fileBufferBytes;

    if (file.is_open())
    {
        file.seekg(0, std::ios::end);
        size_t sizeBytes = file.tellg();
        file.seekg(0, std::ios::beg);
        fileBufferBytes.resize(sizeBytes);
        if (file.read((char*)fileBufferBytes.data(), sizeBytes)) return fileBufferBytes;
    }
    else throw std::runtime_error("could not open binary ifstream to path " + pathToFile);
    return fileBufferBytes;
}

struct memory_buffer : public std::streambuf
{
    char * p_start {nullptr};
    char * p_end {nullptr};
    std::size_t size;

    memory_buffer(char const * first_elem, std::size_t size)
        : p_start(const_cast<char*>(first_elem)), p_end(p_start + size), size(size)
    {
        setg(p_start, p_start, p_end);
    }

    pos_type seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which) override
    {
        if (dir == std::ios_base::cur) gbump(static_cast<int>(off));
        else setg(p_start, (dir == std::ios_base::beg ? p_start : p_end) + off, p_end);
        return gptr() - p_start;
    }

    pos_type seekpos(pos_type pos, std::ios_base::openmode which) override
    {
        return seekoff(pos, std::ios_base::beg, which);
    }
};

struct memory_stream : virtual memory_buffer, public std::istream
{
    memory_stream(char const * first_elem, size_t size)
        : memory_buffer(first_elem, size), std::istream(static_cast<std::streambuf*>(this)) {}
};

class manual_timer
{
    std::chrono::high_resolution_clock::time_point t0;
    double timestamp{ 0.0 };
public:
    void start() { t0 = std::chrono::high_resolution_clock::now(); }
    void stop() { timestamp = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - t0).count() * 1000.0; }
    const double & get() { return timestamp; }
};

inline bool write_ply(const std::string& filename, const VertexData& vertex_data, const bool is_binary = true)
{
    std::setlocale(LC_ALL, "C");
    tinyply::PlyFile ply_file;

    ply_file.add_properties_to_element("vertex", { "x", "y", "z" },
//        tinyply::Type::FLOAT32, vertex_data.positions.size(), reinterpret_cast<std::uint8_t*>(vertex_data.positions.data()), tinyply::Type::INVALID, 0);
         tinyply::Type::FLOAT32, vertex_data.positions.size(), (std::uint8_t*)(vertex_data.positions.data()), tinyply::Type::INVALID, 0);

    ply_file.add_properties_to_element("vertex", { "red", "green", "blue", "alpha" },
          tinyply::Type::FLOAT32, vertex_data.colors.size(), (std::uint8_t*)(vertex_data.colors.data()), tinyply::Type::INVALID, 0);

    ply_file.add_properties_to_element("vertex", { "nx", "ny", "nz" },
          tinyply::Type::FLOAT32, vertex_data.normals.size(), (std::uint8_t*)(vertex_data.normals.data()), tinyply::Type::INVALID, 0);

    ply_file.add_properties_to_element("vertex", { "u", "v" },
          tinyply::Type::FLOAT32, vertex_data.tex_coords.size() , (std::uint8_t*)(vertex_data.tex_coords.data()), tinyply::Type::INVALID, 0);


    std::filebuf file;
    file.open(filename + ".ply", is_binary ? (std::ios::out | std::ios::binary) : std::ios::out);

    std::ostream outstream(&file);
    if (outstream.fail()) throw std::runtime_error("failed to open " + filename);

    ply_file.write(outstream, is_binary); // ASCII, is_binary = false
}

inline VertexData read_ply(const std::string& file_name, const bool preload_into_memory = true)
{
    std::setlocale(LC_ALL, "C");
    VertexData vertex_data;

    std::cout << "........................................................................\n";
    std::cout << "Now Reading: " << file_name << std::endl;

    std::unique_ptr<std::istream> file_stream;
    std::vector<std::uint8_t> byte_buffer;

    try
    {
        bool has_alpha = false;
        // For most files < 1gb, pre-loading the entire file upfront and wrapping it into a
        // stream is a net win for parsing speed, about 40% faster.
        if (preload_into_memory)
        {
            byte_buffer = read_file_binary(file_name);
            file_stream.reset(new memory_stream((char*)byte_buffer.data(), byte_buffer.size()));
        }
        else
        {
            file_stream.reset(new std::ifstream(file_name, std::ios::binary));
        }

        if (!file_stream || file_stream->fail()) throw std::runtime_error("file_stream failed to open " + file_name);

        file_stream->seekg(0, std::ios::end);
        const float size_mb = file_stream->tellg() * float(1e-6);
        file_stream->seekg(0, std::ios::beg);

        tinyply::PlyFile file;
        file.parse_header(*file_stream);

        std::cout << "\t[ply_header] Type: " << (file.is_binary_file() ? "binary" : "ascii") << std::endl;
        for (const auto & c : file.get_comments()) std::cout << "\t[ply_header] Comment: " << c << std::endl;
        for (const auto & c : file.get_info()) std::cout << "\t[ply_header] Info: " << c << std::endl;

        for (const auto & e : file.get_elements())
        {
            std::cout << "\t[ply_header] element: " << e.name << " (" << e.size << ")" << std::endl;
            for (const auto & p : e.properties)
            {
                std::cout << "\t[ply_header] \tproperty: " << p.name << " (type=" << tinyply::PropertyTable[p.propertyType].str << ")";
                if (p.isList) std::cout << " (list_type=" << tinyply::PropertyTable[p.listType].str << ")";
                std::cout << std::endl;
                has_alpha = p.name=="alpha" || p.name=="a";
            }
        }

        // Because most people have their own mesh types, tinyply treats parsed data as structured/typed byte buffers.
        // See examples below on how to marry your own application-specific data structures with this one.
        std::shared_ptr<tinyply::PlyData> vertices, normals, colors, texcoords, faces, tripstrip;

        // The header information can be used to programmatically extract properties on elements
        // known to exist in the header prior to reading the data. For brevity of this sample, properties
        // like vertex position are hard-coded:
        try { vertices = file.request_properties_from_element("vertex", { "x", "y", "z" }); }
        catch (const std::exception & e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }

        try { normals = file.request_properties_from_element("vertex", { "nx", "ny", "nz" }); }
        catch (const std::exception & e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }

        try { colors = file.request_properties_from_element("vertex", { "red", "green", "blue", "alpha" }); }
        catch (const std::exception & e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }

        try { colors = file.request_properties_from_element("vertex", { "r", "g", "b", "a" }); }
        catch (const std::exception & e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }

        try { texcoords = file.request_properties_from_element("vertex", { "u", "v" }); }
        catch (const std::exception & e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }

        // Providing a list size hint (the last argument) is a 2x performance improvement. If you have
        // arbitrary ply files, it is best to leave this 0.
        try { faces = file.request_properties_from_element("face", { "vertex_indices" }, 3); }
        catch (const std::exception & e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }

        // Tristrips must always be read with a 0 list size hint (unless you know exactly how many elements
        // are specifically in the file, which is unlikely);
        try { tripstrip = file.request_properties_from_element("tristrips", { "vertex_indices" }, 0); }
        catch (const std::exception & e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }

        manual_timer read_timer;

        read_timer.start();
        file.read(*file_stream);
        read_timer.stop();

        const float parsing_time = static_cast<float>(read_timer.get()) / 1000.f;
        std::cout << "\tparsing " << size_mb << "mb in " << parsing_time << " seconds [" << (size_mb / parsing_time) << " MBps]" << std::endl;

        if (vertices) {
            std::cerr << "\tRead " << vertices->count  << " total vertices "<< std::endl;
            vertex_data.positions.resize(vertices->count);
            std::memcpy(vertex_data.positions.data(), vertices->buffer.get(), vertices->buffer.size_bytes());
        }
        if (colors) {
            std::cerr << "\tRead " << colors->count << " total vertex colors " << std::endl;
            std::cerr << "\thas_alpha= " << has_alpha << std::endl;

            if (colors->t == tinyply::Type::UINT8) {
                vertex_data.colors.reserve(colors->count);
                if (has_alpha) {
                    for (std::size_t i=0; i<colors->count*4; i+=4) {
                        vertex_data.colors.emplace_back(static_cast<float>(colors->buffer.get_const()[i]) / 255.F,
                                                        static_cast<float>(colors->buffer.get_const()[i+1]/ 255.F),
                                                        static_cast<float>(colors->buffer.get_const()[i+2]/ 255.F),
                                                        static_cast<float>(colors->buffer.get_const()[i+3]/ 255.F));
                    }
                } else {
                    for (std::size_t i=0; i<colors->count*3; i+=3) {
                        vertex_data.colors.emplace_back(static_cast<float>(colors->buffer.get_const()[i])  / 255.F,
                                                        static_cast<float>(colors->buffer.get_const()[i+1])/ 255.F,
                                                        static_cast<float>(colors->buffer.get_const()[i+2])/ 255.F,
                                                        1.F);
                    }
                }
            }
            else if (colors->t == tinyply::Type::FLOAT32) {
                vertex_data.colors.resize(colors->count);
                std::memcpy(vertex_data.colors.data(), vertices->buffer.get(), vertices->buffer.size_bytes());
            }
        }
        if (normals) {
            std::cerr << "\tRead " << normals->count   << " total vertex normals " << std::endl;
            vertex_data.normals.resize(normals->count);
            std::memcpy(vertex_data.normals.data(), normals->buffer.get(), normals->buffer.size_bytes());
        }

        if (texcoords) {
            std::cerr << "\tRead " << texcoords->count << " total vertex texcoords " << std::endl;
            vertex_data.tex_coords.resize(texcoords->count);
            std::memcpy(vertex_data.tex_coords.data(), texcoords->buffer.get(), texcoords->buffer.size_bytes());
        }
    }
    catch (const std::exception & e)
    {
        std::cerr << "Caught tinyply exception: " << e.what() << std::endl;
    }

    return vertex_data;
}


}


#endif // PLYLOADER_H
