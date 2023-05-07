//
// Created by TianyaoChu on 2023/4/12.
//

#ifndef QUANTUM_NETWORKS_QELEMENT_H
#define QUANTUM_NETWORKS_QELEMENT_H

#include <vector>
#include <map>

using namespace std;

enum StateType {Zero, One, Pos, Neg, Etg};
enum PauliType {Z, X};

class Qubit {
private:
    int q_id;
    int node_id;
    StateType state;
    PauliType pauli;
    double p_flip;
    bool measured;
    int value;
public:
    Qubit(int q_id, int node_id, StateType state, PauliType pauli, double p_flip);
    ~Qubit();
    int get_q_id() const;
    int get_node_id() const;
    StateType get_state() const;
    void set_state(StateType new_state);
    PauliType get_pauli() const;
    double get_p_flip() const;
    bool is_measured() const;
    void set_measured();
    int get_value() const;
    void set_value(int new_value);
};

class QReg {
private:
    vector<Qubit*> qubits;
    map<unsigned int, int> state;
public:
    QReg();
    explicit QReg(const vector<Qubit*>& qubits);
    QReg(vector<Qubit*> qubits, map<unsigned int, int> state);
    ~QReg();
    void add_qubit(Qubit* qubit);
    int get_qubit(int node_id, PauliType pauli);
    int get_r_index(int q_id);
    void tensor_product(const QReg& other);
    void pauli_x(int r_index);
    void pauli_z(int r_index);
    void hadamard(int r_index);
    void cnot(int c_index, int r_index);
    Qubit* measure(int q_id, double fidelity);
};

unsigned int get_bit(unsigned int base, int digit);
unsigned int set_bit(unsigned int base, int digit);
unsigned int clr_bit(unsigned int base, int digit);
unsigned int flip_bit(unsigned int base, int digit);
unsigned int del_bit(unsigned int base, int digit);

//class BellRes {
//public:
//    int pauli_z;
//    int pauli_x;
//    BellRes(int pauli_z, int pauli_x): pauli_z(pauli_z), pauli_x(pauli_x) {}
//    ~BellRes() = default;
//    BellRes& operator= (const BellRes& other) = default;
//    BellRes operator+ (const BellRes& other) const {
//        return {(pauli_z+other.pauli_z)%2, (pauli_x+other.pauli_x)%2};
//    }
//};

#endif //QUANTUM_NETWORKS_QELEMENT_H
