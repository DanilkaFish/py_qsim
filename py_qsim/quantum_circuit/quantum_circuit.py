from __future__ import annotations
from typing import List, Tuple, Union, Generator

from abc import abstractmethod, ABC

from cpp_lib.cpp_kernel import Variable, Expression 

# class Qubits(cppQubits):
#     pass
class OperatorOne:
    id = "I"
    x = "X"
    y = "Y"
    z = "Z"
    h = "H"
    sdag = "Sdag"
    s = "S"
    rx = "rx"
    ry = "ry"
    rz = "rz"
    u = "U"


class OperatorTwo:
    cz = "cz"
    cu = "cu"
    cx = "cx"


class Instruction(ABC):
    @property
    @abstractmethod
    def name(self):
        pass
    
    @property
    def meta(self)->dict:
        return {"name": self.name}
    
    @property
    @abstractmethod
    def qubs(self)->List[int]:
        pass
    
    def apply(self, provider, qubs_l=None):
        if qubs_l is None:
            self.qubs_l = list(range(len(self.qubits)))
        getattr(provider, self.name)(**self.meta)

    def decompose(self) -> List[Instruction]:
        return list([self])
    

class MagicInstruction(Instruction):
    def __init__(self, qc: QuantumCircuit, qubs: List[int]):
        self.qc = qc
        self._qubs = qubs

    def qubs(self)->List[int]:
        return self._qubs

    @property
    def name(self):
        return "qc"
    
    
    def apply(self, provider) -> None:
        for ins in self.qc:
            ins.apply(provider, self.qubs)
    
    def decompose(self) -> List[Instruction]:
        return self.qc.instrs
    

class QuantumCircuit:
    def __init__(self, n_qubits):
        self.instrs = []
        self._qubs = list(range(n_qubits))

    def __iter__(self):
        return self.instrs.__iter__()
        
    def __next__(self) -> Generator[Instruction]:
        for ins in self.instrs:
            yield ins
    
    def compose(self, qc: QuantumCircuit, qubs: List[int]):
        for qub in qubs:
            if qub >= len(self._qubs):
                raise ValueError(f"qubit {qub} is not in the circuit")
        self.instrs.append(MagicInstruction(qc, qubs))
        
    def add(self, instr: Instruction) -> None:
        for qub in instr.qubs:
            if qub not in self._qubs:
                return 
        self.instrs.append(instr)

    @property
    def qubs(self):
        return self._qubs

    def run(self, provider):
        for ins in self:
            ins.apply(provider)


class SingleQubitGate(Instruction):
    def __init__(self, qub: int):
        self.qub = qub

    @property
    def qubs(self)->List[int]:
        return [self.qub]
    
    @property        
    def meta(self):
        return {**super().meta, "name": self.name, "qub": self.qub}
    
    @property
    def qubits(self):
        return [self.qub]

class U1(SingleQubitGate):
    #TODO type of data
    def __init__(self, qub: int, data):
        self.qub = qub
        self._data = data

    @property        
    def meta(self):
        return {**super().meta, "qub": self.qub, "data": self._data}
        
class X(SingleQubitGate):
    @property
    def name(self):
        return OperatorOne.x

class Y(SingleQubitGate):
    @property
    def name(self):
        return OperatorOne.y

class Z(SingleQubitGate):
    @property
    def name(self):
        return OperatorOne.z
        
class I(SingleQubitGate):
    @property
    def name(self):
        return OperatorOne.id
        
class H(SingleQubitGate):
    @property
    def name(self):
        return OperatorOne.h

class S(SingleQubitGate):
    @property
    def name(self):
        return OperatorOne.s
        
class Sdag(SingleQubitGate):
    @property
    def name(self):
        return OperatorOne.sdag


class Rx(SingleQubitGate):
    def __init__(self, qub: int, param: Union[float, Variable, Expression]):
        self.param = param
        self.qub = qub

    @property 
    def name(self):
        return OperatorOne.rx


    @property
    def meta(self):
        return {**super().meta, "qub" : self.qub, "param": self.param} 


class Ry(SingleQubitGate):
    def __init__(self, qub: int, param: Union[float, Variable, Expression]):
        self.param = param
        self.qub = qub

    @property 
    def name(self):
        return OperatorOne.ry


    @property
    def meta(self):
        return {**super().meta, "qub" : self.qub, "param": self.param} 


class Rz(SingleQubitGate):
    def __init__(self, qub: int, param: Union[float, Variable, Expression]):
        self.param = param
        self.qub = qub

    @property 
    def name(self):
        return OperatorOne.rz


    @property
    def meta(self):
        return {**super().meta, "qub" : self.qub, "param": self.param} 


class TwoQubitControlGate(Instruction):
    def __init__(self, ctrl: int, trg: int):
        self.ctrl = ctrl
        self.trg = trg
        
    @property
    def qubs(self)->List[int]:
        return [self.ctrl, self.trg]

    @property
    def name(self):
        return OperatorOne.u
    
    @property
    def meta(self):
        return {**super().meta, "ctrl" : self.ctrl, "trg": self.trg} 

    @property
    def qubits(self):
        return [self.ctrl, self.trg]
    
    
class CX(TwoQubitControlGate):
    def __init__(self, ctrl: int, trg: int):
        self.ctrl = ctrl
        self.trg = trg
    @property
    def name(self):
        return OperatorTwo.cx


class CU(TwoQubitControlGate):
    def __init__(self, ctrl: int, trg: int, u: SingleQubitGate):
        self.ctrl = ctrl
        self.trg = trg
        self.u = u

    @property
    def meta(self):
        return {**super().meta, "ctrl" : self.ctrl, "trg": self.trg, "u_meta": self.u.meta} 

    @property
    def name(self):
        return OperatorTwo.cu
    

class CZ(TwoQubitControlGate):
    def __init__(self, ctrl: int, trg: int):
        self.ctrl = ctrl
        self.trg = trg

    @property
    def name(self):
        return OperatorTwo.cx
