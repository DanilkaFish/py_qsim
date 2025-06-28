import unittest


import sys
import os
current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.abspath(os.path.join(current_dir, '..'))
sys.path.insert(0, parent_dir)


from cpp_lib.cpp_kernel import Qubits, Shape, Tensor, DiagonalTensor

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
        self.assertEqual(Shape([0],[]) * Shape([],[1]), Shape([0],[1]))

class TestTensor(unittest.TestCase):
    def test_init(self):
        n = 3
        i = [Tensor(Shape([i],[i]), [1,0,0,1]) for i in range(n)]
        x = [Tensor(Shape([i],[i]), [0,1,1,0]) for i in range(n)]
        y = [Tensor(Shape([i],[i]), [0,-1j,1j,0]) for i in range(n)]
        z = [Tensor(Shape([i],[i]), [1,0,0,-1]) for i in range(n)]

    def test_non_throw_product(self):
        n = 15
        state_down = [Tensor(Shape([],[i]), [1,0]) for i in range(15)]
        state_up = [Tensor(Shape([i],[]), [1,0]) for i in range(15)]
        matrices = [Tensor(Shape([i],[i]), [1,0,0,1]) for i in range(15)]
        new_matrices = []
        for i in range(n):
            for j in range(n):
                new_matrices.append(matrices[i]*matrices[j])
                new_matrices.append(state_down[i]*state_up[i])
                new_matrices.append(state_up[i]*state_down[j])
        for i in range(n):
            for m2 in new_matrices:
                matrices[i]*m2
                m2 * matrices[i]
                Tensor(2) * m2
                m2 * Tensor(2)

    def test_clifford_product(self):
        n = 15
        zz = [1,0,0,0,0,-1,0,0,0,0,-1,0,0,0,0,1]
        xx = [0,0,0,1,0,0,1,0,0,1,0,0,1,0,0,0]
        yy = [500,0,0,-1,0,0,1,0,0,1,0,0,-1,0,0,0]
        id = [Tensor(Shape([i],[i]), [1,0,0,1]) for i in range(n)]
        x = [Tensor(Shape([i],[i]), [0,1,1,0]) for i in range(n)]
        y = [Tensor(Shape([i],[i]), [0,-1j,1j,0]) for i in range(n)]
        z = [Tensor(Shape([i],[i]), [1,0,0,-1]) for i in range(n)]
        for i in range(n):
            for j in range(n):
                if i == j:
                    self.assertEqual(x[i]*x[j], id[i], msg=str(i) + ":\n " + str(x[i] * x[j]) + "\n" + str(id[i]))
                    self.assertEqual(z[i]*z[j], id[i], msg=str(z[i] * z[j]) + "\n" + str(id[i]))
                    self.assertEqual(y[i]*y[j], id[i], msg=str(y[i] * y[j]) + "\n" + str(id[i]))
                else:
                    self.assertEqual(x[i] * x[j], Tensor(Shape([i,j],[i,j]), xx), msg=str(x[i] * x[j]))
                    self.assertFalse(y[i] * y[j] == Tensor(Shape([i,j],[i,j]), yy), msg=str(y[i] * y[j]))
                    self.assertEqual(z[i] * z[j], Tensor(Shape([i,j],[i,j]), zz), msg=str(z[i] * z[j]))

    def test_diag_prod(self):
        def diag_x(qub):
            return DiagonalTensor([qub], [1,1], [1,0])
        def diag_y(qub):
            return DiagonalTensor([qub], [-1j, 1j], [1,0])
        def diag_z(qub):
            return DiagonalTensor([qub], [1, -1], [0,1])
        def diag_id(qub):
            return DiagonalTensor([qub], [1, 1], [0,1])

        def x(qub):
            return Tensor(Shape([qub],[qub]), [0,1,1,0])
        def y(qub):
            return Tensor(Shape([qub],[qub]), [0,-1j,1j,0])
        def z(qub):
            return Tensor(Shape([qub],[qub]), [1,0,0,-1])
        def id(qub):
            return Tensor(Shape([qub],[qub]), [1,0,0,1])
        
        n = 15
        for i in range(n):
            self.assertEqual(diag_x(i) * x(i), x(i)*x(i), msg=str(diag_x(i) * x(i)) + str(x(i)*x(i)))
            self.assertEqual(diag_x(i) * y(i), x(i)*y(i))
            self.assertEqual(diag_x(i) * z(i), x(i)*z(i))
            self.assertEqual(diag_y(i) * x(i), y(i)*x(i), msg=str(diag_x(i) * x(i)) + str(x(i)*x(i)))
            self.assertEqual(diag_y(i) * y(i), y(i)*y(i))
            self.assertEqual(diag_y(i) * z(i), y(i)*z(i))
            self.assertEqual(diag_z(i) * x(i), z(i)*x(i), msg=str(diag_x(i) * x(i)) + str(x(i)*x(i)))
            self.assertEqual(diag_z(i) * y(i), z(i)*y(i))
            self.assertEqual(diag_z(i) * z(i), z(i)*z(i))
        for i in range(n):
            self.assertEqual(x(i) * diag_x(i), x(i) * x(i), msg=str(x(i) * diag_x(i)) + str(x(i) * x(i)))
            self.assertEqual(y(i) * diag_x(i), y(i) * x(i), msg=str(x(i) * diag_x(i)) + str(x(i) * x(i)))
            self.assertEqual(z(i) * diag_x(i), z(i) * x(i), msg=str(z(i) * diag_x(i)) + str(z(i) * x(i)))
            self.assertEqual(x(i) * diag_y(i), x(i) * y(i), msg=str(x(i) * diag_x(i)) + str(x(i)*x(i)))
            self.assertEqual(y(i) * diag_y(i), y(i) * y(i), msg=str(x(i) * diag_x(i)) + str(x(i)*x(i)))
            self.assertEqual(z(i) * diag_y(i), z(i) * y(i), msg=str(x(i) * diag_x(i)) + str(x(i)*x(i)))
            self.assertEqual(x(i) * diag_z(i), x(i) * z(i), msg=str(x(i) * diag_x(i)) + str(x(i)*x(i)))
            self.assertEqual(y(i) * diag_z(i), y(i) * z(i), msg=str(x(i) * diag_x(i)) + str(x(i)*x(i)))
            self.assertEqual(z(i) * diag_z(i), z(i) * z(i), msg=str(x(i) * diag_x(i)) + str(x(i)*x(i)))
        for i in range(15):
            for j in range(15):
                self.assertEqual(diag_x(i) * diag_x(j), x(i) * x(j))
                self.assertEqual(diag_y(i) * diag_x(j), y(i) * x(j) )
                self.assertEqual(diag_z(i) * diag_x(j), z(i) * x(j) )
                self.assertEqual(diag_x(i) * diag_y(j), x(i) * y(j) )
                self.assertEqual(diag_y(i) * diag_y(j), y(i) * y(j) )
                self.assertEqual(diag_z(i) * diag_y(j), z(i) * y(j) )
                self.assertEqual(diag_x(i) * diag_z(j), x(i) * z(j) )
                self.assertEqual(diag_y(i) * diag_z(j), y(i) * z(j) )
                self.assertEqual(diag_z(i) * diag_z(j), z(i) * z(j) )
if __name__ == "__main__":
    unittest.main() 