import unittest


import sys
import os
current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.abspath(os.path.join(current_dir, '..'))
sys.path.insert(0, parent_dir)


import itertools as it
from cpp_lib.cpp_kernel import Qubits, Variable, sin, cos, pow, Expression, VariableVector
import math as mh
import random as rd

def list_to_qubs_int(ls):
    res = 0
    for num in ls:
        res += 1 << num
    return res
from quantum_circuit.quantum_circuit import *

class MockProvider:
    def __init__(self):
        self.s = ""
        for op, name in vars(OperatorOne).items():
            if not op.startswith('__'):
                self.__setattr__(name, self.expand_1q) 
        for op, name in vars(OperatorTwo).items():
            if not op.startswith('__'):
                self.__setattr__(name, self.expand_2q)

    def expand_1q(self, name, qub, **kwargs):
        self.s += name + str(qub)

    def expand_2q(self, name, ctrl, trg, **kwargs):
        self.s += name + str(ctrl) + str(trg)
        

class TestQuantumCircuit(unittest.TestCase):
    def test_int_generations(self):
        qc = QuantumCircuit(5)
        qc.add(X(1))
        qc.add(CX(1,2))
        mp = MockProvider()
        qc.run(mp)
        self.assertEqual(mp.s, "X1cx12")

    def test_compose(self):
        mp_magic = MockProvider()
        qc = QuantumCircuit(5)
        qc_magic = QuantumCircuit(4)
        qc_magic.add(X(1))
        qc_magic.add(Y(2))
        qc_magic.add(Z(3))
        qc_magic.add(CZ(0,3))
        for i in range(5):
            mp = MockProvider()
            qc.compose(qc_magic, [0,1,2,3])
            qc_magic.run(mp_magic)
            qc.run(mp)
            self.assertEqual(mp_magic.s, mp.s)

if __name__ == "__main__":
    unittest.main() 