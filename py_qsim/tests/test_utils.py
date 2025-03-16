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

class TestQubits(unittest.TestCase):
    def test_int_generations(self):
        self.assertEqual(Qubits(1).to_int, 1)
        for i in [0,2,3,4,7,31]:
            self.assertEqual(Qubits(i).to_int, list_to_qubs_int(range(i)))


    def test_list_generation(self):
        ls = []
        for i in [0,2,3,4,7,31]:
            ls.append(i)
            for j in range(i + 1):
                rd.Random().shuffle(x=ls)
                self.assertEqual(Qubits.from_list(ls).to_int, list_to_qubs_int(ls))

    def test_insert(self):
        qubs = Qubits(0)
        list_to_insert = [0,0,1,2,3,1,2,3,8]
        for index, el in enumerate(list_to_insert):
            qubs.insert(el)
            self.assertEqual(qubs.to_int, list_to_qubs_int(set(list_to_insert[:index + 1])), msg=index)


class TestVariable(unittest.TestCase):
    def test_constructor(self):
        # Проверка конструктора с дефолтными значениями
        param_default = Variable()
        self.assertEqual(param_default.name, "t")
        self.assertAlmostEqual(param_default.value, 0.0)

        # Проверка конструктора с указанием имени и значения
        param_custom = Variable("custom_param", 42.0)
        self.assertEqual(param_custom.name, "custom_param")
        self.assertAlmostEqual(param_custom.value, 42.0)

    def test_set_value(self):
        param = Variable()
        param.value = 10.0
        self.assertAlmostEqual(param.value, 10.0)

class TestExpression(unittest.TestCase):
    def setUp(self):
        self.vals = [0,2,-1]
        self.mults = [0, 1, 5, 10, -2]
        self.funcs_prod = [
            lambda x, mult: mult * x, 
            lambda x, mult: x * mult, 
            lambda x, mult: mult * mult * x, 
            lambda x, mult: mult * x * mult
        ]
        self.funcs_sum = [
            lambda x, add: add + x, 
            lambda x, add: x + add, 
            lambda x, add: add + add + x, 
            lambda x, add: add + x + add
        ]
        return super().setUp()
    
    def test_doubleprod(self):
        for val in self.vals:
            t = Variable(value=val)
            for mult in self.mults:
                for func in self.funcs_prod:
                    self.assertEqual(func(t, mult).evaluate(), func(val, mult))

    def test_arbitrary_prod(self):
        for val in self.vals:
            t = Variable(value=val)
            for mult in self.mults:
                t2 = mult * t
                val2 = mult * val
                for func in self.funcs_prod:
                    self.assertEqual(func(t, t2).evaluate(), func(val, val2))
    
    def test_sum(self):
        for val in self.vals:
            t = Variable(value=val)
            for mult in self.mults:
                t2 = mult * t
                val2 = mult * val
                for func in self.funcs_sum:
                    self.assertEqual(func(t, t2).evaluate(), func(val, val2))

    def test_multi_expression(self):
        vals = [1, 5, -1.23, 2]
        funcs = [
            lambda t1, t2 : t1 + t2,
            lambda t1, t2 : t1 * t2,
            lambda t1, t2 : t1 + t2  + (-1.)
            ]
        funcss = it.permutations(funcs, 3)
        vars = [Variable(value=val) for val in vals]
        for el in funcss:
            val = 1
            expr = 1.
            for index, func in enumerate(el):
                val  = func(val, vals[index])
                expr = func(expr, vars[index])
            
            self.assertEqual(expr.evaluate(), val)

    def test_cos_sin(self):
        vars = [Variable(value=val) for val in self.vals]
        funcs = [
            lambda t1: sin(t1),
            lambda t1: cos(t1),
            lambda t: sin(t)*cos(t*t) + t + 1
            ]
        funcs_val = [
            lambda t1: mh.sin(t1),
            lambda t1: mh.cos(t1),
            lambda t: mh.sin(t) * mh.cos(t*t) + t + 1
            ]
        for var in vars:
            i = 0
            for func, func_val in zip(funcs, funcs_val):
                self.assertEqual(func(var).evaluate(), func_val(var.value), 
                                 str(func(var)) + str(var.value) + f" {i} " + str(func_val(var.value)) )
                i += 1

    def test_pow(self):
        vars = [Variable(value=val) for val in self.vals]
        for var, val in zip(vars, self.vals):
            self.assertEqual(pow(var, val).evaluate(), var.value**val)
    
    def test_grad(self):
        x = Variable("x")
        _xs = [1,10,23]
        func_grad = [
            (lambda x: Expression(x), lambda x: Expression(1) ),
            (lambda x: x + 1, lambda x: Expression(1)),
            (lambda x: 2*x + 1, lambda x: Expression(2)),
            (lambda x: 2*x*x + 1, lambda x: 4*x),
            (lambda x: 2*x*x*x, lambda x: 6*x*x),
            (lambda x: 2*pow(x,2), lambda x: 4*x),
            (lambda x: 2*pow(x,3), lambda x: 6*x*x),
            (lambda x: 2*pow(2*x*x,3), lambda x: 16*6*pow(x,5)),
            (lambda x: 2*pow(2*sin(2*x),3), lambda x: 6*pow(2*sin(2*x),2) * 4*cos(2*x)),
            (lambda x: 2*sin(x)*cos(x), lambda x: 2*cos(2*x)),
            (lambda x: 2*sin(x) + cos(4*x), lambda x: 2*cos(x) - 4*sin(4*x)),
            ]
        for funcs in func_grad:
            expr = funcs[0](x)
            grad_expr = funcs[1](x)
            for _x in _xs:
                x.value = _x
                expr.evaluate()
                expr.backward()
                self.assertAlmostEqual(x.get_grad(), grad_expr.evaluate(), msg=str(expr))
    
    def test_multi_grad(self):
        x = Variable("x")
        y = Variable("y")
        # z = Variable("z")
        _xs = [1,10,23]
        _ys = [0,-1,1.23]
        func_grad = [
            (lambda x,y: x + y + 1, lambda x, y: Expression(1), lambda x,y: Expression(1)),
            (lambda x,y: x * y , lambda x, y: Expression(y), lambda x,y: Expression(x)),
            (lambda x,y: x * y + 1, lambda x, y: Expression(y), lambda x,y: Expression(x)),
            (lambda x,y: x * y * y * y + 1, lambda x,y: y*y*y, lambda x,y: 3*x*y*y),
            (lambda x,y: sin(x * y + 1), lambda x, y: y * cos(x * y + 1), lambda x,y: x * cos(x * y + 1)),
            (lambda x,y: cos(x * y + 1), lambda x, y: (-1) * y * sin(x * y + 1), lambda x,y: (-1) *x * sin(x * y + 1)),
            ]
        for funcs in func_grad:
            expr = funcs[0](x,y)
            xgrad_expr = funcs[1](x,y)
            ygrad_expr = funcs[2](x,y)

            for _x, _y in it.product(_xs, _ys):
                x.value = _x
                y.value = _y
                xgr = xgrad_expr.evaluate()
                ygr = ygrad_expr.evaluate()
                expr.evaluate()
                expr.backward()
                self.assertAlmostEqual(x.get_grad(), xgr, msg=f"{expr} {x.value} {y.value}")
                self.assertAlmostEqual(y.get_grad(), ygr, msg=f"{expr} {x.value} {y.value}")
class TestVariableVector(unittest.TestCase):

    def test_init(self):
        n = 5
        vc1 = VariableVector()
        vc2 = VariableVector(size=n)

        for i in range(n):
            vc1.generate_variable()
            self.assertAlmostEqual(vc1[i].value, 0)
            self.assertAlmostEqual(vc2[i].value, 0)

    def test_set_value(self):
        n = 10
        vc = VariableVector(size=n)
        vc.set_values(list(range(n)))
        for i in range(n):
            self.assertAlmostEqual(vc[i].value, i)
        for i in range(n):
            vc[i].value = i + 2
            self.assertAlmostEqual(vc[i].value, i + 2)

if __name__ == "__main__":
    unittest.main() 