#include "gumbo.h"

#include <cassert>
#include <sstream>
#include <stdexcept>
#include <string>
#include <fstream>
#include <iostream>
#include <regex>
#include <type_traits>
#include <vector>

namespace {

    std::string read_whole_file(std::string file_name) {
        std::fstream file(file_name);
        std::stringstream ss; ss << file.rdbuf();

        return ss.str();
    }

    GumboNode *gumbo_get_child(GumboNode *node, int index) {
        assert(node);
        assert(node->type == GUMBO_NODE_ELEMENT && "Only nodes can have children!");

        return (GumboNode*) node->v.element.children.data[index];
    }

    const char *gumbo_get_element_text(GumboNode *node) {
        return gumbo_get_child(node, 0)->v.text.text;
    }

    const char *gumbo_find_style(GumboNode *node) {
        if (node->type != GUMBO_NODE_ELEMENT)
            return nullptr;

        if (node->v.element.tag == GUMBO_TAG_STYLE)
            return gumbo_get_element_text(node);

        GumboVector vec = node->v.element.children;

        for (int i = 0; i < vec.length; ++ i) {
            const char *found = gumbo_find_style(gumbo_get_child(node, i));
            if (found)
                return found;
        }

        return nullptr;
    }

    std::string delete_comments(const std::string input) {
        std::string without_comments;

        bool is_comment_open = false;
        for (int i = 0; i < input.size(); ++ i) {
            if (is_comment_open) {
                if (input[i] == '*' && input[i + 1] == '/') {
                    is_comment_open = false;
                    ++ i; // skip '/'
                }

                // skip current symbol:
                continue;
            }

            // we're not in the comment currently
            if (input[i] == '/' && input[i + 1] == '*') {
                is_comment_open = true;
                ++ i; // skip '*'

                continue;
            }

            without_comments += input[i];
        }

        return without_comments;
    }

        
    struct css_property {
        std::string key;
        std::string value;
    };

    // Accepts input body of CSS class .<name> { <body> }
    //                                           ^~~~~~ only this should be passed

    // Returns passed css_properties (they have syntax 'key: value'),
    // for example you can:

    // .<class-name> { /* This line is for reference shouldn't be passed */
    //     color: #FF6188;
    //  /* ^~~~~~~~~~~~~~ example of this kind of property
    // }               /* This line is for reference shouldn't be passed */
    std::vector<css_property> parse_css_properties(const std::string &body) {
        std::vector<css_property> properties;

        const std::regex css_property_regex(
            R"(([a-zA-Z\-][a-zA-Z0-9\-]*): ([#a-zA-Z0-9\-]+);)", std::regex::extended);

        auto iter = std::sregex_iterator(body.begin(), body.end(), css_property_regex);
        for (; iter != std::sregex_iterator(); ++ iter) {
            std::smatch match = *iter;

            // read <key>: <value>; from capturing groups:
            std::string key   = match[1].str();
            std::string value = match[2].str();
                
            properties.push_back({ key, value });
        }

        return properties;
    }

    struct css_class {
        std::string name;
        std::vector<css_property> properties;
    };

    // Reads css class style properties from CSS file passed as string in /style/
    std::vector<css_class> parse_css_classes(const std::string &style) {
        std::vector<css_class> classes;

        const std::regex css_class_regex(
            R"((body|\.[a-zA-Z0-9\-]+) *\{([^\}]*)})", std::regex::extended);

        auto iter = std::sregex_iterator(style.begin(), style.end(), css_class_regex);
        for (; iter != std::sregex_iterator(); ++ iter) {
            std::smatch match = *iter;

            // read .<class_name> { <class_body> } from capturing groups:
            std::string class_name = match[1].str();

            // delete '.' at the beginning of class name:
            if (class_name[0] == '.')
                class_name.erase(class_name.begin());

            std::string class_body = match[2].str();

            auto properties = parse_css_properties(class_body);
            classes.push_back({ class_name, properties });
        }

        return classes;
    }

    std::string convert_css_font_property_to_emacs(css_property property) {
        if (property.key == "color")
            return ":foreground \"" + property.value + "\"";

        if (property.key == "background-color")
            return ":background \"" + property.value + "\"";

        if (property.key == "font-weight" && property.value == "bold")
            return ":bold t";

        if (property.key == "text-decoration" && property.value == "underline")
            return ":underline t";

        if (property.key == "font-style" && property.value == "italic")
            return ":slant italic";

        throw std::runtime_error("unsupported css property: '" +
                                 property.key + ": " + property.value + "'");
    }

    int fill_count(int value, int max_length) {
        int fill_count = max_length - value;
        if (fill_count < 0)
            return 0;

        return fill_count;
    }

    std::string generate_elisp_to_set_same_faces(std::vector<css_class> css_classes) {
        std::string simple_elisp_theme = "(custom-set-faces\n";

        for (auto css_class: css_classes) {
            std::string emacs_name = "font-lock-" + css_class.name;
            if (emacs_name == "font-lock-body")
                emacs_name = "default";

            constexpr std::size_t max_class_name_length = 38;

            simple_elisp_theme += "    '(" + emacs_name;
            simple_elisp_theme += std::string(fill_count(emacs_name.size(), max_class_name_length), ' ');

            simple_elisp_theme += "((t (";
            std::size_t total_length = 0;
            for (auto css_property: css_class.properties) {
                auto property = convert_css_font_property_to_emacs(css_property);

                simple_elisp_theme += " " + property;
                total_length += property.size() + 1;
            }

            constexpr std::size_t max_properties_length = 44;
            simple_elisp_theme += std::string(fill_count(total_length, max_properties_length), ' ');

            simple_elisp_theme += " )  ))\n";
        }

        simple_elisp_theme += " )\n";
        return simple_elisp_theme;
    }

}

int main(int argc, char *argv[]) {
    // TODO: ensure args
    std::string file_content = read_whole_file(argv[1]);
    GumboOutput *output = gumbo_parse(file_content.c_str());

    std::string style = gumbo_find_style(output->root);

    auto classes = parse_css_classes(delete_comments(style));
    std::cout << generate_elisp_to_set_same_faces(classes);
}
