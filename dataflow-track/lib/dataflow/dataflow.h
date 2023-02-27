#include <memory>
#include <utility>
#include <vector>
#include <iostream>

#include "source-location.h"
#include "axp-utility.h"

struct int_flow {
    axp::source_location location;

    enum its_kind {
        copied, assigned,
        binary_operator,
        unary_operator,
        literal, no_init
    } kind = no_init;

    const int_flow *lhs = nullptr;
    const int_flow *rhs = nullptr;

    void old_print() const {
        if (lhs) {
            lhs->old_print();
            print_old_flow(lhs);
        }

        if (rhs) {
            rhs->old_print();
            print_old_flow(rhs);
        }
    }

    void print(int& initial_id) const {
        switch (kind) {
        case copied:
            std::cout << "  copy: ";
            break;

        case assigned:
            std::cout << "assign: ";
            break;

        case binary_operator:
            std::cout << "bin-op: ";
            break;

        case unary_operator:
            std::cout << " un-op: ";
            break;

        case literal:
            std::cout << "create: ";
            break;

        case no_init:
            std::cout << "uninit: ";
            break;
        }

        print_location();

        std::cout << "\t(" << (initial_id ++) << ")" << "\n";

        if (lhs) lhs->print(initial_id);
        if (rhs) rhs->print(initial_id);
    }

    void print_graph(int& initial_id) const {
        int current_id = initial_id ++;
        if (lhs) print_flow(current_id, initial_id, lhs);
        if (rhs) print_flow(current_id, initial_id, rhs);
    }

private:
    void print_location() const {
        std::cout << location.line << ":" << location.column;
    }

    void print_flow(int current_id, int &id_counter, const int_flow *flow) const {
        std::cout << current_id << " " << id_counter << "\n";
        flow->print_graph(id_counter);
    }

    void print_old_flow(const int_flow *to) const {
        print_location();
        std::cout << " ";
        to->print_location();
        std::cout << "\n";
    }
};


static
class history_storage {
public:
    template <typename... arg_types>
    int_flow *new_flow(axp::source_location loc, int_flow::its_kind kind,
                       const int_flow *lhs = nullptr, const int_flow *rhs = nullptr) {

        auto new_flow_element = std::unique_ptr<int_flow>(new int_flow { loc, kind, lhs, rhs });
        m_flow_points.push_back(std::move(new_flow_element));

        return m_flow_points.back().get();
    }

private:
    std::vector<std::unique_ptr<int_flow>> m_flow_points;
} g_intoscope_vault;


template <typename type>
class source_location_proxy {
public:
    source_location_proxy(type value, axp::source_location loc = axp::source_location::current())
        : passed(value), location(loc) {};

    type passed;
    axp::source_location location;
};


class int_proxy {
public:
    int_proxy(int value, axp::source_location loc = axp::source_location::current())
        : int_proxy { value, loc, int_flow::literal } {}

    int_proxy(axp::source_location loc = axp::source_location::current())
        : int_proxy { 0, loc, int_flow::no_init } {} // Default initialization to 0! Hooray!

    int_proxy(const int_proxy &proxy, axp::source_location loc = axp::source_location::current())
        : int_proxy { proxy.m_value, loc, int_flow::copied, proxy.m_history } {}

#define BIN_OP(op)                                                                           \
    int_proxy operator op(source_location_proxy<const int_proxy&> rhs) const {               \
        return { m_value op rhs.passed.m_value,                                              \
                 rhs.location, int_flow::binary_operator, m_history, rhs.passed.m_history }; \
    }                                                                                        \
                                                                                             \
    friend int_proxy operator op(int lhs, source_location_proxy<const int_proxy&> rhs) {     \
        return { lhs op rhs.passed.m_value,                                                  \
                 rhs.location, int_flow::binary_operator, rhs.passed.m_history };            \
    }                                                                                        \
                                                                                             \
    friend int_proxy operator op(const int_proxy &lhs, source_location_proxy<int> rhs) {     \
        return { lhs.m_value op rhs.passed,                                                  \
                 rhs.location, int_flow::binary_operator, lhs.m_history };                   \
    }

    BIN_OP(+) BIN_OP(-) BIN_OP(*) BIN_OP(/) BIN_OP(%)
    BIN_OP(&) BIN_OP(|) BIN_OP(^) BIN_OP(<<) BIN_OP(>>)
#undef BIN_OP

#define ASSIGN(op)                                                                           \
    int_proxy& operator op(source_location_proxy<const int_proxy&> rhs) {                    \
        m_value op rhs.passed.m_value;                                                       \
        return assign(rhs.location, int_flow::assigned, rhs.passed.m_history);               \
    }                                                                                        \
                                                                                             \
    int_proxy& operator op(source_location_proxy<int> rhs) {                                 \
        m_value op rhs.passed;                                                               \
        return assign(rhs.location, int_flow::assigned);                                     \
    }                                                                                        \

    ASSIGN(+=) ASSIGN(-=) ASSIGN(*=) ASSIGN(/=) ASSIGN(=) ASSIGN(%=)
    ASSIGN(&=) ASSIGN(|=) ASSIGN(^=) ASSIGN(<<=) ASSIGN(>>=)
#undef ASSIGN

    auto operator<=>(const int_proxy &other) const { return m_value <=> other.m_value; }
    auto operator<=>(int other) const { return m_value <=> other; }

    // Derive all comparison operators from threeway comparison:
#define CMP_OP(op)                                                                           \
    bool operator op(const int_proxy&) const = default;                                      \
    bool operator op(int other) const { return ((*this) <=> other) op 0; }

    CMP_OP(<) CMP_OP(>) CMP_OP(<=) CMP_OP(>=) CMP_OP(==) CMP_OP(!=)
#undef CMP_OP

    void dump_history() const {
        // int id = 0;
        // m_history->print(id);
        // m_history->print_graph(id = 0);
        m_history->old_print();
    }

private:
    int m_value;
    int_flow *m_history;

    int_proxy(int value, int_flow *history): m_value(value), m_history(history) {}

    int_proxy(int value, axp::source_location loc, int_flow::its_kind kind,
              const int_flow *lhs = nullptr, const int_flow *rhs = nullptr)
        : int_proxy(value, g_intoscope_vault.new_flow(loc, kind, lhs, rhs)) {}

    int_proxy& assign(axp::source_location loc, int_flow::its_kind kind,
                      const int_flow *history = nullptr) {

        int_flow *updated_history = g_intoscope_vault.new_flow(loc, kind, history, m_history);
        m_history = updated_history;                                                         

        return *this;                                                                        
    }
};
