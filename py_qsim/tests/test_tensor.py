import unittest


import sys
import os
current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.abspath(os.path.join(current_dir, '..'))
sys.path.insert(0, parent_dir)


from cpp_lib.cpp_kernel import Qubits, Shape, Tensor

def list_to_qubs_int(ls):
    res = 0
    for num in ls:
        res += 1 << num
    return res
from quantum_circuit.quantum_circuit import *


class TestShape(unittest.TestCase):
    def test_init(self):
        shape = Shape(Qubits.from_list([]), Qubits.from_list([1]))
        shape = Shape(Qubits.from_list([0,2,3]), Qubits.from_list([0,2,5]))
        shape = Shape([0,2,3], [0,1]) 
        shape_copy = Shape(shape)
        shape = Shape(Qubits(15), Qubits(0))

    def test_shape_equality(self):
        shape = Shape()
        shape = Shape(Qubits.from_list([0,2,3]), Qubits.from_list([0,2,5]))
        shapes = [shape, Shape(Qubits(3), Qubits(0)), Shape(Qubits(0), Qubits(3)), Shape(Qubits(3), Qubits(3)), Shape(Qubits(0), Qubits(0))]
        self.assertTrue(shape == shape)
        for i, shl in enumerate(shapes):
            for j, shr in enumerate(shapes):
                self.assertEqual(i == j, shl == shr)
                self.assertEqual(i != j, shl != shr)

    def test_shape_prod(self):
        shape012 = Shape(Qubits.from_list([0,1,2]), Qubits.from_list([0,1,2]))       
        shape01 = Shape(Qubits.from_list([0,1]), Qubits.from_list([]))   
        inv_shape01 = Shape([], [0,1])   
        self.assertEqual(shape012*shape012, shape012)
        self.assertEqual(shape012*shape01, Shape([0,1,2],[2]), msg=str(shape012*shape01))
        self.assertEqual(shape01*inv_shape01, Shape([0,1],[0,1]))
        self.assertEqual(inv_shape01*shape01, Shape())
    

class TestTensor(unittest.TestCase):
    def test_init(self):
        x = Tensor(Shape([0],[0]), [0,1,1,0])
        y = Tensor(Shape([0],[0]), [0,-1j,1j,0])
        z = Tensor(Shape([0],[0]), [1,0,0,-1])
        print(x)
        print(y)
        print(z)
        print(x*z)

    def test_repr(self):
        pass
    
if __name__ == "__main__":
    unittest.main() 