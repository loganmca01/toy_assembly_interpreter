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
        if left.nodetype == 'm':
            #todo: figure out how to handle this with different number of bytes
            pass
        elif left.nodetype == 'r':
            left.sym.val = right.eval()

    def print_tree(self):
        print("[" + nodetype + "]" + left.print_tree() + right.print_tree())

class Arith(AstInterface):
    def __init__(self, nodetype, left, right):
        self.nodetype = nodetype
        self.left = left
        self.right = right

    def eval(self):
        if nodetype == '+':
            left.eval() + right.eval()
        elif nodetype == '-':
            left.eval() - right.eval()
        elif nodetype == '*':
            left.eval() * right.eval()
        elif nodetype == '/':
            left.eval() / right.eval()

    def print_tree(self):
        print("[" + nodetype + "]" + left.print_tree() + right.print_tree())
        

class Cmp(AstInterface):
    def __init__(self, nodetype, left, right):
        self.nodetype = nodetype
        self.left = left
        self.right = right

    def eval(self):
        if nodetype == '1':
            return left.eval() > right.eval()
        elif nodetype == '2':
            return left.eval() < right.eval()
        elif nodetype == '3':
            return left.eval() == right.eval()
        elif nodetype == '4':
            return left.eval() >= right.eval()
        elif nodetype == '5':
            return left.eval() <= right.eval()

    def print_tree(self):
        print("[" + nodetype + "]" + left.print_tree() + right.print_tree())

class SymRef(AstInterface):
    def __init__(self, sym):
        self.sym = sym

    def eval(self):
        return sym.value

    def print_tree(self):
        print("[symref " + sym.name + "]")

class MemRef(AstInterface):
    def __init__(self, loc):
        self.loc = loc

    def eval(self):
        global memory
        global memory_size

        addr = loc.eval()

        # todo: fix second condition to make sure full value is within memory
        # add else which throws exception (?) or just somehow signals an error
        if (addr >= 0 and addr < memory_size):
            return memory[addr]

    def print_tree(self):
        print("[m]")
        

class Num(AstInterface):
    def __init__(self, number):
        self.number = number

    def eval(self):
        return number

    def print_tree(self):
        print("[n " + number + "]")


class Flow(AstInterface):
    def __init__(self, cond, then):
        self.cond = cond
        self.then = then

    def eval(self):
        if (cond.eval()):
            then.eval()

    def print_tree(self):
        print("[i]" + cond.print_tree() + then.print_tree())

class Symbol():
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