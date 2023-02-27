#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

struct location {
    std::size_t line, column;
    auto operator<=>(const location &other) const = default;
};

std::ostream& operator<<(std::ostream &ofs, location loc) {
    ofs << loc.line << ":" << loc.column;
    return ofs;
}

location parse_location(std::istream& input) {
    location result;
    input >> result.line;
    input.ignore();
    input >> result.column;

    return result;
}

std::map<location, location> parse_translation_map(std::istream& input) {
    std::map<location, location> translation;

    while (input) {
        location src = parse_location(input);
        location dst = parse_location(input);

        translation[src] = dst;
    }

    return translation;
}

location parse_translated(std::istream &input, const std::map<location, location> &translation_map) {
    location target = parse_location(input);
    if (!input)
        return {};

    if (!translation_map.contains(target)) {
        std::cerr << "Skipped: " << target << std::endl;
        return {};
    }

    return translation_map.at(target);
}

std::string translate(std::istream& input, const std::map<location, location> &translation_map) {
    std::stringstream result;

    std::set<std::pair<location, location>> transitions;
    while (true) {
        location src = parse_translated(input, translation_map);
        location dst = parse_translated(input, translation_map);

        if (!input)
            break;

        if (src == dst)
            continue;

        if (!transitions.contains({ src, dst }))
            result << src << " " << dst << "\n";
        else {
            transitions.insert({ src, dst });
        }
    }

    return result.str();
}

int main(int argc, char *argv[]) {
    if (argc != 3)
        return EXIT_FAILURE;

    const char* translation_map_file = argv[1];
    const char*    to_translate_file = argv[2];

    std::ifstream translation_map_input(translation_map_file);
    std::ifstream    to_translate_input(to_translate_file);

    std::cout << translate(to_translate_input,
                           parse_translation_map(translation_map_input));
}

    
