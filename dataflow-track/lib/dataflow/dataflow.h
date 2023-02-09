#include <memory>
#include <vector>
#include <iostream>

#include "source-location.h"

class int_proxy;

struct int_flow {
    axp::source_location location;

    int last_value = 0;

    enum its_kind {
        copied, assigned,
        binary_operator,
        unary_operator,
        literal, no_init
    } kind = no_init;

    const int_flow *lhs = nullptr;
    const char *operator_name = nullptr;
    const int_flow *rhs = nullptr;
   
    void dump() const {
        std::cout << location.to_string() << "\n";
        if (lhs) lhs->dump();
        if (rhs) rhs->dump();
    }

};


static
class history_storage {
public:
    template <typename... arg_types>
    int_flow *new_flow(arg_types... args) {
        auto new_flow_element =
            std::unique_ptr<int_flow>(new int_flow { args... });
        m_flow_points.push_back(std::move(new_flow_element));

        return m_flow_points.back().get();
    }

private:
    std::vector<std::unique_ptr<int_flow>> m_flow_points;
} g_intoscope_vault;


template <typename type>
class source_location_proxy {
public:
    source_location_proxy(type &value, axp::source_location loc = axp::source_location::current())
        : passed(value), location(loc) {};

    type passed;
    axp::source_location location;
};


class int_proxy {
public:
    int_proxy(int value, axp::source_location loc = axp::source_location::current())
        : m_history(g_intoscope_vault.new_flow(loc, value, int_flow::literal)) {}

    int_proxy(const int_proxy& proxy, axp::source_location loc = axp::source_location::current())
        : m_history(g_intoscope_vault.new_flow(
                        loc, proxy.value(), int_flow::copied, proxy.m_history)) {}

    #define BIN_OP(op)                                                                         \
        int_proxy operator op(source_location_proxy<const int_proxy&> other) {                 \
            std::cout << other.location.to_string() << " <== \n";                              \
            return g_intoscope_vault.new_flow(other.location, other.passed.value() op value(), \
                                              int_flow::binary_operator,                       \
                                              m_history, #op, other.passed.m_history);         \
        }

    BIN_OP(+) BIN_OP(-) BIN_OP(*) BIN_OP(/)

    #undef BIN_OP

    int value() const { return m_history->last_value; }

    void dump() { m_history->dump(); }

private:
    int_flow *m_history;

    int_proxy(int_flow *history): m_history(history) {}
};
