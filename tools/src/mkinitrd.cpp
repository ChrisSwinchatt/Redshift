// Copyright (c) 2012 Chris Swinchatt.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
// documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
// Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#include <exception>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <sys/stat.h>

#define REDSHIFT_KERNEL_REDSHIFT_H 1 // Don't include <kernel/redshift.h>
extern "C" {
#include <kernel/initrd.h>
}

// Get the size of a file.
static initrd_size_t get_file_size(const std::string& path)
{
    static struct stat fileinfo;
    if (stat(path.c_str(), &fileinfo) < 0) {
        return 0;
    }
    return static_cast<initrd_size_t>(fileinfo.st_size);
}

// Get the index of a node.
static initrd_index_t get_node_index(std::vector<initrd_node>& nodes, std::vector<std::string>& strings,
                                     const std::string& name)
{
    if (name == "/") {
        // Root directory is index 0.
        return 0;
    }
    for (initrd_index_t i = 0; i < static_cast<initrd_index_t>(nodes.size()); ++i) {
        if (strings[nodes[i].name] == name) {
            return i;
        }
    }
    return 0;
}

// Generate node and string table entries for a file. Also recursively generates parent nodes.
// Return value is the node's index.
static initrd_index_t generate_node(std::vector<initrd_node>& nodes, std::vector<std::string>& strings,
                                   const std::string& filename, bool dir = false)
{
    static initrd_offset_t offset = 0;
    static initrd_offset_t name_offset = 0;
    initrd_size_t size = 0;
    initrd_size_t length = 0;
    std::string leaf;
    initrd_node node;
    std::size_t pos = 0;
    if (filename.size() == 0 || filename == "/") {
        // Root directory: return 0.
        return 0;
    } else if ((pos = filename.rfind("/")) != std::string::npos && pos != 0) {
        std::cerr << "A" << std::endl;
        // File in a directory: recursively generate parent nodes and set node.parent appropriately.
        std::string parent = filename.substr(0, pos);
        leaf = filename.substr(pos + 1);
        node.parent = generate_node(nodes, strings, parent, true);
    } else {
        std::cerr << "B" << std::endl;
        // File in the root directory.
        leaf = (pos == 0 ? filename.substr(1) : filename);
        node.parent = 0;
    }
    // Create the node.
    if (dir) {
        initrd_index_t i = 0;
        if ((i = get_node_index(nodes, strings, leaf)) != 0 && nodes[i].parent == node.parent) {
            // Directory exists!
            return i;
        }
        node.offset = 0;
        node.size   = 0;
    } else {
        size = get_file_size(filename);
        node.offset   = offset;
        node.size     = size;
    }
    node.name     = name_offset;
    node.name_len = leaf.size();
    nodes.push_back(node);
    // Add the string data.
    length += leaf.size();
    strings.push_back(leaf);
    std::cerr << "Node "       << nodes.size() << " (" << filename << "):\n"
              << " * Parent: " << node.parent  << '\n'
              << " * Offset: " << node.offset  << '\n'
              << " * Size:   " << node.size    << '\n'
              << " * Name:   " << node.name    << ':' << node.name_len << " (" << strings[node.name] << ")\n";
    offset      += size;
    name_offset += leaf.size();
    return static_cast<initrd_index_t>(nodes.size());
}

// Generate node table and string table.
static bool generate_tables(std::vector<initrd_node>& nodes, std::vector<std::string>& strings,
                                     const std::vector<std::string>& files)
{
    for (const auto& filename : files) {
        generate_node(nodes, strings, filename);
    }
    return true;
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << "<output file> <input file(s)>\n"
                  << "Note: <output file> has to be an absolute path, while <input file(s)> must be relative paths.\n"
                  << std::endl;
        return 1;
    }
    std::vector<initrd_node> nodes;
    std::vector<std::string> strings;
    std::vector<std::string> files(argc - 2);
    for (int i = 2; i < argc; ++i) {
        files.push_back(argv[i]);
    }
    generate_tables(nodes, strings, files);
    return 0;
}
