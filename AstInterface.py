import SystemInfo

class AstInterface():
    def eval(self):
        pass
    def print_tree(self):
        pass

class Assign(AstInterface):
    def __init__(self, nodetype, left, right):
        self.nodetype = nodetype
        self.left = left
        self.right = right

    def eval(self):
        global memory
        if self.left.nodetype == 'm':
            #todo: figure out how to handle this with different number of bytes
            pass
        elif self.left.nodetype == 'r':
            self.left.sym.val = right.eval()

    def print_tree(self):
        print("[" + self.nodetype + "]")
        self.left.print_tree()
        self.right.print_tree()

class Arith(AstInterface):
    def __init__(self, nodetype, left, right):
        self.nodetype = nodetype
        self.left = left
        self.right = right

    def eval(self):
        if self.nodetype == '+':
            self.left.eval() + self.right.eval()
        elif self.nodetype == '-':
            self.left.eval() - self.right.eval()
        elif self.nodetype == '*':
            self.left.eval() * self.right.eval()
        elif self.nodetype == '/':
            self.left.eval() / self.right.eval()

    def print_tree(self):
        print("[" + self.nodetype + "]")
        self.left.print_tree()
        self.right.print_tree()
        

class Cmp(AstInterface):
    def __init__(self, nodetype, left, right):
        self.nodetype = nodetype
        self.left = left
        self.right = right

    def eval(self):
        if self.nodetype == '1':
            return self.left.eval() > self.right.eval()
        elif self.nodetype == '2':
            return self.left.eval() < self.right.eval()
        elif self.nodetype == '3':
            return self.left.eval() == self.right.eval()
        elif self.nodetype == '4':
            return self.left.eval() >= self.right.eval()
        elif self.nodetype == '5':
            return self.left.eval() <= self.right.eval()

    def print_tree(self):
        print("[" + self.nodetype + "]")
        self.left.print_tree()
        self.right.print_tree()

class SymRef(AstInterface):
    def __init__(self, nodetype, sym):
        self.sym = sym
        self.nodetype = nodetype

    def eval(self):
        return self.sym.value

    def print_tree(self):
        print("[symref " + self.nodetype + " " + self.sym.name + "]")

class MemRef(AstInterface):
    def __init__(self, nodetype, loc):
        self.loc = loc
        self.nodetype = nodetype

    def eval(self):

        addr = self.loc.eval()

        # todo: fix second condition to make sure full value is within memory
        # add else which throws exception (?) or just somehow signals an error
        if (addr >= 0 and addr < SystemInfo.memory_size):
            return SystemInfo.memory[addr]

    def print_tree(self):
        print("[m]")
        self.loc.print_tree()
        

class Num(AstInterface):
    def __init__(self, nodetype, number):
        self.number = number
        self.nodetype = nodetype

    def eval(self):
        return self.number

    def print_tree(self):
        print("[n " + str(self.number) + "]")


class Flow(AstInterface):
    def __init__(self, nodetype, cond, then):
        self.cond = cond
        self.then = then
        self.nodetype = nodetype

    def eval(self):
        if (self.cond.eval()):
            self.then.eval()

    def print_tree(self):
        print("[i]")
        self.cond.print_tree()
        self.then.print_tree()

class Symbol():
    # symtype is 0 for variable or register, 1 for register only
    def __init__(self, name, value, symtype):
        self.name = name
        self.value = value
        self.symtype = symtype

class Instruction():
    def __init__(self, name, args, actions):
        self.name = name
        self.args = args
        self.actions = actions

class Iterator():
    def __init__(self, val):
        self.val = val