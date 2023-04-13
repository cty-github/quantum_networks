//
// Created by TianyaoChu on 2023/4/12.
//

#include "qelement.h"
#include "utils/rand.h"
#include <utility>
#include <iostream>

Qubit::Qubit(int q_id, int node_id, StateType state, PauliType pauli, double p_flip):
q_id(q_id), node_id(node_id), state(state), pauli(pauli), p_flip(p_flip), measured(false), value(0) {}

Qubit::~Qubit() = default;

int Qubit::get_q_id() const {
    return q_id;
}

int Qubit::get_node_id() const {
    return node_id;
}

StateType Qubit::get_state() const {
    return state;
}

void Qubit::set_state(StateType new_state) {
    state = new_state;
}

PauliType Qubit::get_pauli() const {
    return pauli;
}

double Qubit::get_p_flip() const {
    return p_flip;
}

bool Qubit::is_measured() const {
    return measured;
}

void Qubit::set_measured() {
    measured = true;
}

int Qubit::get_value() const {
    return value;
}

void Qubit::set_value(int new_value) {
    value = new_value;
}

QReg::QReg() = default;

QReg::QReg(const vector<Qubit*>& qubits) {
    for (auto qubit:qubits) {
        add_qubit(qubit);
    }
}

QReg::QReg(vector<Qubit*> qubits, map<unsigned int, int> state):
qubits(std::move(qubits)), state(std::move(state)) {}

QReg::~QReg() = default;

void QReg::add_qubit(Qubit* qubit) {
    qubits.push_back(qubit);
    StateType q_state = qubit->get_state();
    if (q_state == Etg) {
        cout << "Error: Single Qubit Entangled" << endl;
        return;
    }
    map<unsigned int, int> new_state;
    if (state.empty()) {
        switch (q_state) {
            case Zero: {
                new_state[0] += 1;
                break;
            }
            case One: {
                new_state[1] += 1;
                break;
            }
            case Pos: {
                new_state[0] += 1;
                new_state[1] += 1;
                break;
            }
            case Neg: {
                new_state[0] += 1;
                new_state[1] += -1;
                break;
            }
            default: {
                break;
            }
        }
    } else {
        for (auto it: state) {
            unsigned int base = it.first;
            int amp = it.second;
            switch (q_state) {
                case Zero: {
                    new_state[base * 2] += amp;
                    break;
                }
                case One: {
                    new_state[base * 2 + 1] += amp;
                    break;
                }
                case Pos: {
                    new_state[base * 2] += amp;
                    new_state[base * 2 + 1] += amp;
                    break;
                }
                case Neg: {
                    new_state[base * 2] += amp;
                    new_state[base * 2 + 1] += -amp;
                    break;
                }
                default: {
                    break;
                }
            }
        }
    }
    qubit->set_state(Etg);
    state = new_state;
}

int QReg::get_qubit(int node_id, PauliType pauli) {
    // return q_id of qubit with PauliType <pauli> in Node <node_id>
    for (auto & qubit : qubits) {
        if (node_id == qubit->get_node_id() && pauli == qubit->get_pauli()) {
            return qubit->get_q_id();
        }
    }
    cout << "Error: No Qubit with PauliType " << pauli << " in Node " << node_id << endl;
    return -1;
}

int QReg::get_r_index(int q_id) {
    for (int i = 0; i < qubits.size(); i++) {
        if (q_id == qubits[i]->get_q_id()) {
            return i;
        }
    }
    cout << "Error: No Qubit " << q_id << endl;
    return -1;
}

void QReg::tensor_product(const QReg& other) {
    qubits.insert(qubits.end(), other.qubits.begin(), other.qubits.end());
    int other_size = (int)other.qubits.size();
    map<unsigned int, int> new_state;
    for (auto it_a:state) {
        unsigned int base_a = it_a.first;
        int amp_a = it_a.second;
        for (auto it_b:other.state) {
            unsigned int base_b = it_b.first;
            int amp_b = it_b.second;
            new_state[base_a*(1<<other_size)+base_b] += amp_a * amp_b;
        }
    }
    state = new_state;
}

void QReg::pauli_x(int r_index) {
    // bit flip gate
    int q_num = (int)qubits.size();
    if (r_index >= q_num || r_index < 0) {
        cout << "Error: No Qubit " << r_index << endl;
        return;
    }
    map<unsigned int, int> new_state;
    int r_digit = q_num - r_index - 1;
    for (auto it: state) {
        unsigned int base = it.first;
        int amp = it.second;
        new_state[flip_bit(base, r_digit)] += amp;
    }
    state = new_state;
}

void QReg::pauli_z(int r_index) {
    // phase flip gate
    int q_num = (int)qubits.size();
    if (r_index >= q_num || r_index < 0) {
        cout << "Error: No Qubit " << r_index << endl;
        return;
    }
    map<unsigned int, int> new_state;
    int r_digit = q_num - r_index - 1;
    for (auto it: state) {
        unsigned int base = it.first;
        int amp = it.second;
        if (get_bit(base, r_digit) == 1) {
            new_state[base] += -amp;
        } else {
            new_state[base] += amp;
        }
    }
    state = new_state;
}

void QReg::hadamard(int r_index) {
    int q_num = (int)qubits.size();
    if (r_index >= q_num || r_index < 0) {
        cout << "Error: No Qubit " << r_index << endl;
        return;
    }
    map<unsigned int, int> new_state;
    int r_digit = q_num - r_index - 1;
    for (auto it: state) {
        unsigned int base = it.first;
        int amp = it.second;
        if (get_bit(base, r_digit) == 0) {
            new_state[base] += amp;
            new_state[set_bit(base, r_digit)] += amp;
        } else {
            new_state[clr_bit(base, r_digit)] += amp;
            new_state[base] += -amp;
        }
    }
    state = new_state;
}

void QReg::cnot(int c_index, int r_index) {
    int q_num = (int)qubits.size();
    if (c_index >= q_num || c_index < 0) {
        cout << "Error: No Control Qubit " << c_index << endl;
        return;
    }
    if (r_index >= q_num || r_index < 0) {
        cout << "Error: No Operate Qubit " << r_index << endl;
        return;
    }
    map<unsigned int, int> new_state;
    int c_digit = q_num - c_index - 1;
    int r_digit = q_num - r_index - 1;
    for (auto it: state) {
        unsigned int base = it.first;
        int amp = it.second;
        if (get_bit(base, c_digit) == 1) {
            new_state[flip_bit(base, r_digit)] += amp;
        } else {
            new_state[base] += amp;
        }
    }
    state = new_state;
}

Qubit* QReg::measure(int q_id, double fidelity) {
    int r_index = get_r_index(q_id);
    int q_num = (int)qubits.size();
    if (r_index >= q_num || r_index < 0) {
        cout << "Error: No Qubit " << r_index << endl;
        return nullptr;
    }
    int zero_cnt = 0;
    int r_digit = q_num - r_index - 1;
    for (auto it: state) {
        unsigned int base = it.first;
        if (get_bit(base, r_digit) == 0) {
            zero_cnt++;
        }
    }
    uniform_real_distribution<double> rand_double(0.0,1.0);
    int new_value;
    if ((double)state.size()*rand_double(rand_eng) < zero_cnt) {
        new_value = 0;
    } else {
        new_value = 1;
    }
    Qubit* qubit = qubits[r_index];
    qubit->set_measured();
    qubit->set_value(new_value);
    map<unsigned int, int> new_state;
    for (auto it: state) {
        unsigned int base = it.first;
        int amp = it.second;
        if (get_bit(base, r_digit) == new_value) {
            new_state[del_bit(base, r_digit)] += amp;
        }
    }
    qubits.erase(qubits.begin() + r_index);
    state = new_state;
    return qubit;
}

unsigned int get_bit(unsigned int base, int digit) {
    return (base >> digit) % 2;
}

unsigned int set_bit(unsigned int base, int digit) {
    if (get_bit(base, digit) == 0) {
        return base + (1 << digit);
    } else {
        return base;
    }
}

unsigned int clr_bit(unsigned int base, int digit) {
    if (get_bit(base, digit) == 1) {
        return base - (1 << digit);
    } else {
        return base;
    }
}

unsigned int flip_bit(unsigned int base, int digit) {
    if (get_bit(base, digit) == 0) {
        return base + (1 << digit);
    } else {
        return base - (1 << digit);
    }
}

unsigned int del_bit(unsigned int base, int digit) {
    return ((base >> (digit+1)) << digit) + base % (1 << digit);
}
