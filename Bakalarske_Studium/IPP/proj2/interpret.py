import re
import argparse
import xml.etree.ElementTree as ET
import sys


class Program:
    def __init__(self):
        self.ins = []

    def add_ins(self, ins):  # Add instruction object to program
        self.ins.append(ins)


class Instruction:
    def __init__(self, opcode, order):
        self.opcode = opcode
        self.order = int(order)
        self.args = []

    def add_arg(self, index, arg):  # Add argument object to instruction
        self.args.insert(index, arg)


class Argument:
    def __init__(self, type, value):
        self.type = type
        self.value = value


class Label:
    def __init__(self, order, name):
        self.order = order
        self.name = name


class Variable:
    def __init__(self, name, type, value):
        self.name = name
        self.type = type
        self.value = value


class Local_Frame:
    def __init__(self):
        self.stack = []

    def push_var(self, var):  # Add variable to top frame of LF
        if self.stack != []:
            self.stack[len(self.stack)-1].stack.append(var)
        else:
            print("Error 55", file=sys.stderr)  # Error 55 if LF is empty
            exit(55)

    def pop_frame(self):  # Returns and pops top frame of LF
        if self.stack != []:
            return self.stack.pop()
        else:
            print("Error 55", file=sys.stderr)  # Error 55 if LF is empty
            exit(55)

    def push_frame(self, frame):  # Adds frame to top of LF
        self.stack.append(frame)

    def top_frame(self):
        # Returns top frame stack for finding variables
        return self.stack[len(self.stack)-1].stack


class Temporary_Frame:
    def __init__(self):
        self.stack = []
        self.active = False

    def push_var(self, var):  # Adds variables to TF
        if self.active:
            self.stack.append(var)
        else:
            # If TF is not active (hasnt been created or recently pushed) Error 55
            print("Error 55", file=sys.stderr)
            exit(55)


# Checks if variable is in frame (if NotThrow = true, function doesnt throw errors)
def check_var(Frame, name, NotThrow):
    if Frame == "GF":
        for var in GF:
            if var.name == name:
                return True
        if NotThrow:
            return False
        # Error 54 if variable is not in frame and NotThrow is false
        print("Error 54", file=sys.stderr)
        exit(54)
    elif Frame == "LF":
        if LF.stack != []:
            for var in LF.top_frame():
                if var.name == name:
                    return True
            if NotThrow:
                return False
            print("Error 54", file=sys.stderr)
            exit(54)
        else:
            print("Error 55", file=sys.stderr)  # If LF is empty Error 55
            exit(55)
    elif Frame == "TF":
        if TF.active:
            for var in TF.stack:
                if var.name == name:
                    return True
            if NotThrow:
                return False
            print("Error 54", file=sys.stderr)
            exit(54)
        else:
            print("Error 55", file=sys.stderr)  # If TF is empty Error 55
            exit(55)


def Get_Var(var):  # Returns variable from frame
    global GF
    global LF
    global TF
    var_f = var[:2]
    var_n = var[3:]
    check_var(var_f, var_n, False)  # If variable is not found raises Error
    if var_f == "GF":
        my_var = [x for x in GF if x.name == var_n]
    elif var_f == "LF":
        my_var = [x for x in LF.top_frame() if x.name == var_n]
    elif var_f == "TF":
        my_var = [x for x in TF.stack if x.name == var_n]
    return my_var[0]


def Put2Var(var, value):  # Puts new value to variable (var = target variable, value = new value)
    var_f = var[:2]
    var_n = var[3:]
    # If target variable is not found raises Error
    check_var(var_f, var_n, False)
    if var_f == "GF":
        for x in GF:
            if x.name == var_n:
                x.type = value.type
                x.value = value.value
                return
    elif var_f == "LF":
        for x in LF.top_frame():
            if x.name == var_n:
                x.type = value.type
                x.value = value.value
                return
    elif var_f == "TF":
        for x in TF.stack:
            if x.name == var_n:
                x.type = value.type
                x.value = value.value
                return


def appendVar(var):  # Adds new variable to given frame
    var_f = var[:2]
    var_n = var[3:]
    if var_f == "GF":
        GF.append(Variable(var_n, "", ""))
        return
    elif var_f == "LF":
        LF.push_var(Variable(var_n, "", ""))
        return
    elif var_f == "TF":
        TF.push_var(Variable(var_n, "", ""))
        return


def makeVar(type, value):  # Function either returns var from frames or create new one from instruction arguments
    if type == "var":
        return Get_Var(value)
    else:
        if not(value):  # If value is not set
            value = ""
        if type == "int" and (is_integer(value) or value == ""):
            return Variable("", type, value)
        elif type == "bool" and (value == "true" or value == "false" or value == ""):
            return Variable("", type, value)
        elif type == "string":
            # re.sub changes unicode in string to chars
            return Variable("", type, re.sub(r'\\([0-9]{3})', lambda x: chr(int(x[1])), value))
        elif type == "nil" and value == "nil":
            return Variable("", type, value)
        else:
            # Error 52 if type is not suported
            print("Error 52", file=sys.stderr)
            exit(52)


def QuickMafs(ins, op):  # Make arithmethic operation and puts result to target variable
    var1 = makeVar(ins.args[1].type, ins.args[1].value)
    var2 = makeVar(ins.args[2].type, ins.args[2].value)

    if var1.type == "" or var2.type == "":  # if operands are not initialized
        print("Error 56", file=sys.stderr)
        exit(56)

    if var1.type == "int" and var2.type == "int":
        if op == "+":
            Put2Var(ins.args[0].value, Variable(
                "", "int", str(int(var1.value)+int(var2.value))))
        if op == "-":
            Put2Var(ins.args[0].value, Variable(
                "", "int", str(int(var1.value)-int(var2.value))))
        if op == "*":
            Put2Var(ins.args[0].value, Variable(
                "", "int", str(int(var1.value)*int(var2.value))))
        if op == "/":
            if int(var2.value) != 0:
                Put2Var(ins.args[0].value, Variable(
                    "", "int", str(int(var1.value)//int(var2.value))))
            else:
                print("Error 57", file=sys.stderr)  # Division with 0
                exit(57)
    else:
        print("Error 53", file=sys.stderr)  # Wrong type of operands
        exit(53)


def JumpEQ(ins):  # Function used for Jumps (Returns true of false)
    var1 = makeVar(ins.args[1].type, ins.args[1].value)
    var2 = makeVar(ins.args[2].type, ins.args[2].value)

    if var1.type == "" or var2.type == "":  # unitiliased operands
        print("Error 56", file=sys.stderr)
        exit(56)

    if var1.type == var2.type or (var1.type == "nil" or var2.type == "nil"):
        if var1.type != "nil" and var2.type != "nil":
            if var1.type == "int":
                return (int(var1.value) == int(var2.value))
            elif var1.type == "string":
                return var1.value == var2.value
            elif var1.type == "bool":
                if var1.value.upper() == "TRUE":
                    v1 = True
                else:
                    v1 = False
                if var2.value.upper() == "TRUE":
                    v2 = True
                else:
                    v2 = False
                return v1 == v2
        else:
            if var1.type == var2.type:
                return True
            else:
                return False
    else:
        # Wrong type or combination of operands
        print("Error 53", file=sys.stderr)
        exit(53)


def QuickRel(ins, op):  # Does relation operation and puts result to variable
    var1 = makeVar(ins.args[1].type, ins.args[1].value)
    var2 = makeVar(ins.args[2].type, ins.args[2].value)

    if var1.type == "" or var2.type == "":  # unitiliased operands
        print("Error 56", file=sys.stderr)
        exit(56)
    if var1.type == var2.type or (var1.type == "nil" or var2.type == "nil"):
        if var1.type != "nil" and var2.type != "nil":
            if var1.type == "int":
                if op == "<":
                    Put2Var(ins.args[0].value, Variable(
                        "", "bool", str(int(var1.value) < int(var2.value))))
                elif op == ">":
                    Put2Var(ins.args[0].value, Variable(
                        "", "bool", str(int(var1.value) > int(var2.value))))
                elif op == "=":
                    Put2Var(ins.args[0].value, Variable(
                        "", "bool", str(int(var1.value) == int(var2.value))))
            elif var1.type == "string":
                if op == "<":
                    Put2Var(ins.args[0].value, Variable(
                        "", "bool", str(var1.value < var2.value)))
                elif op == ">":
                    Put2Var(ins.args[0].value, Variable(
                        "", "bool", str(var1.value > var2.value)))
                elif op == "=":
                    Put2Var(ins.args[0].value, Variable(
                        "", "bool", str(var1.value == var2.value)))
            elif var1.type == "bool":
                if var1.value == "true" or var1.value == "True":
                    v1 = True
                else:
                    v1 = False
                if var2.value == "true" or var2.value == "True":
                    v2 = True
                else:
                    v2 = False
                if op == "<":
                    Put2Var(ins.args[0].value, Variable(
                        "", "bool", str(v1 < v2)))
                elif op == ">":
                    Put2Var(ins.args[0].value, Variable(
                        "", "bool", str(v1 > v2)))
                elif op == "=":
                    Put2Var(ins.args[0].value, Variable(
                        "", "bool", str(v1 == v2)))
        else:
            if op == "=":
                if var1.type == var2.type:
                    Put2Var(ins.args[0].value, Variable(
                        "", "bool", "True"))
                else:
                    Put2Var(ins.args[0].value, Variable(
                        "", "bool", "False"))
            else:
                # Wrong type or combination of operands
                print("Error 53", file=sys.stderr)
                exit(53)
    else:
        # Wrong type or combination of operands
        print("Error 53", file=sys.stderr)
        exit(53)


def QuickBool(ins, op):  # Does boolean operation and puts result to var
    var1 = makeVar(ins.args[1].type, ins.args[1].value)
    var2 = makeVar(ins.args[2].type, ins.args[2].value)

    if var1.type == "" or var2.type == "":  # unitiliased operands
        print("Error 56", file=sys.stderr)
        exit(56)

    if var1.type == "bool" and var2.type == "bool":
        if var1.value == "true" or var1.value == "True":
            v1 = True
        else:
            v1 = False
        if var2.value == "true" or var2.value == "True":
            v2 = True
        else:
            v2 = False
        if op == "AND":
            Put2Var(ins.args[0].value, Variable(
                "", "bool", str(v1 and v2)))
        if op == "OR":
            Put2Var(ins.args[0].value, Variable(
                "", "bool", str(v1 or v2)))
    else:
        # Wrong type or combination of operands
        print("Error 53", file=sys.stderr)
        exit(53)


def QuickNot(ins):  # NOT boolean operation
    var1 = makeVar(ins.args[1].type, ins.args[1].value)

    if var1.type == "":  # unitiliased operand
        print("Error 56", file=sys.stderr)
        exit(56)

    if var1.type == "bool":
        if var1.value == "true" or var1.value == "True":
            v1 = True
        else:
            v1 = False
        Put2Var(ins.args[0].value, Variable(
            "", "bool", str(not(v1))))
    else:
        # Wrong type or combination of operands
        print("Error 53", file=sys.stderr)
        exit(53)


def is_integer(n):  # Funtion checks if string could be integer
    try:
        float(n)
    except ValueError:
        return False
    else:
        return float(n).is_integer()


def interpret(ins):
    global i, n
    global TF, LF, GF, stack, label, calls
    # Prace s ramci, volani funkci
    if ins.opcode == "MOVE":  # Moves value from symb or var to var
        # Get variable from argument
        var1 = makeVar(ins.args[1].type, ins.args[1].value)

        if var1.type == "":  # unitiliased variable
            print("Error 56", file=sys.stderr)
            exit(56)

        Put2Var(ins.args[0].value, var1)  # Puts value to variable
    elif ins.opcode == "CALL":  # Saves order and jumps to labled function
        calls.append(ins.order)
        my_label = [x for x in label if x.name == ins.args[0].value]
        if my_label != []:
            i = my_label[0].order-1
        else:
            print("Error 52", file=sys.stderr)  # Label was not found
            exit(52)
    elif ins.opcode == "RETURN":  # Return to latest call
        if calls != []:
            i = calls.pop()
        else:
            print("Error 56", file=sys.stderr)  # No calls saved
            exit(56)
    elif ins.opcode == "CREATEFRAME":  # creates new TF
        TF.stack = []
        TF.active = True  # Cleans stack and set TF as active
    elif ins.opcode == "PUSHFRAME":  # PUshes TF to LF
        if TF.active:
            LF.push_frame(TF)
            TF = Temporary_Frame()  # TF is reseted
        else:
            # Error if TF is not active (created)
            print("Error 55", file=sys.stderr)
            exit(55)
    elif ins.opcode == "POPFRAME":  # Pop frame from LF to TF
        TF = LF.pop_frame()
        TF.active = True
    elif ins.opcode == "DEFVAR":  # Defines new variable
        # Check if variable name is already in use (NotThrow is True so it returns false or true)
        if check_var(ins.args[0].value[:2], ins.args[0].value[3:], True):
            print("Error 52", file=sys.stderr)  # Variable name is used
            exit(52)
        else:
            appendVar(ins.args[0].value)  # Appends new variable
    # Prace s datovym zasobnikem
    elif ins.opcode == "PUSHS":  # Adds variable to stack
        var1 = makeVar(ins.args[0].type, ins.args[0].value)
        if var1.type == "":  # unitialised var
            print("Error 56", file=sys.stderr)
            exit(56)
        stack.append(var1)
    elif ins.opcode == "POPS":  # Pops variable from stack
        if stack != []:
            Put2Var(ins.args[0].value, stack.pop())
        else:
            print("Error 56", file=sys.stderr)  # Stack is empty
            exit(56)
    elif ins.opcode == "CLEARS":  # Clears stack
        stack = []
    # Aritmeticke, relacni, booleovske a konverzni instrukce
    elif ins.opcode == "ADD":
        QuickMafs(ins, "+")
    elif ins.opcode == "SUB":
        QuickMafs(ins, "-")
    elif ins.opcode == "MUL":
        QuickMafs(ins, "*")
    elif ins.opcode == "IDIV":
        QuickMafs(ins, "/")
    elif ins.opcode == "LT":
        QuickRel(ins, "<")
    elif ins.opcode == "GT":
        QuickRel(ins, ">")
    elif ins.opcode == "EQ":
        QuickRel(ins, "=")
    elif ins.opcode == "AND":
        QuickBool(ins, "AND")
    elif ins.opcode == "OR":
        QuickBool(ins, "OR")
    elif ins.opcode == "NOT":
        QuickNot(ins)
    elif ins.opcode == "INT2CHAR":  # Saves char based on unicode value
        var1 = makeVar(ins.args[1].type, ins.args[1].value)

        if var1.type == "":  # Not set variable
            print("Error 56", file=sys.stderr)
            exit(56)

        if var1.type == "int":
            if int(var1.value) >= 0 and int(var1.value) <= 1114111:  # int in unicode range
                Put2Var(ins.args[0].value, Variable(
                    "", "string", chr(int(var1.value))))
            else:
                print("Error 58", file=sys.stderr)
                exit(58)
        else:
            print("Error 53", file=sys.stderr)  # Wrong type
            exit(53)
    elif ins.opcode == "STRI2INT":  # Saves unicode value from char in string
        var1 = makeVar(ins.args[1].type, ins.args[1].value)
        var2 = makeVar(ins.args[2].type, ins.args[2].value)

        if var1.type == "" or var2.type == "":  # Not set values
            print("Error 56", file=sys.stderr)
            exit(56)

        if var1.type == "string" and var2.type == "int":
            # Check if int index is in string
            if len(var1.value)-1 >= int(var2.value) and int(var2.value) >= 0:
                Put2Var(ins.args[0].value, Variable(
                    "", "int", ord(var1.value[int(var2.value)])))
            else:
                print("Error 58", file=sys.stderr)
                exit(58)
        else:
            print("Error 53", file=sys.stderr)  # wrong type
            exit(53)
    # Vstupne-vystupne instrukce
    elif ins.opcode == "READ":  # Reads line from input
        var_type = ins.args[1].value
        var_value = input_1.readline()
        if not(var_value):
            Put2Var(ins.args[0].value, Variable("", "nil", "nil"))
        else:
            # substitute ends of line in string
            var_value = re.sub("\n", "", var_value)
            if var_type == "int":
                if is_integer(var_value):
                    Put2Var(ins.args[0].value, Variable("", "int", var_value))
                else:
                    Put2Var(ins.args[0].value, Variable("", "nil", "nil"))
            elif var_type == "string":
                var_value = re.sub(
                    r'\\([0-9]{3})', lambda x: chr(int(x[1])), var_value)
                Put2Var(ins.args[0].value, Variable("", "string", var_value))
            elif var_type == "bool":
                if var_value.upper() == "TRUE":
                    Put2Var(ins.args[0].value, Variable("", "bool", "True"))
                else:
                    Put2Var(ins.args[0].value, Variable("", "bool", "False"))
            else:
                Put2Var(ins.args[0].value, Variable("", "nil", "nil"))
    elif ins.opcode == "WRITE":  # Write value to stdout
        var1 = makeVar(ins.args[0].type, ins.args[0].value)

        if var1.type == "":  # Not set value
            print("Error 56", file=sys.stderr)
            exit(56)

        if var1.type == "bool":
            if var1.value.upper() == "TRUE":
                print("true", end='')
            else:
                print("false", end='')
        elif var1.type == "nil":
            print("", end='')
        elif var1.type == "string":
            print(
                re.sub(r'\\([0-9]{3})', lambda x: chr(int(x[1])), var1.value), end='')
        else:
            print(var1.value, end='')
    # Prace s retezci
    elif ins.opcode == "CONCAT":  # Concat two strings
        var1 = makeVar(ins.args[1].type, ins.args[1].value)
        var2 = makeVar(ins.args[2].type, ins.args[2].value)

        if var1.type == "" or var2.type == "":
            print("Error 56", file=sys.stderr)
            exit(56)

        if var1.type == "string" and var2.type == "string":
            Put2Var(ins.args[0].value, Variable(
                "", "string", var1.value + var2.value))
        else:
            print("Error 53", file=sys.stderr)
            exit(53)
    elif ins.opcode == "STRLEN":  # Save lenght of string
        var1 = makeVar(ins.args[1].type, ins.args[1].value)

        if var1.type == "":
            print("Error 56", file=sys.stderr)
            exit(56)

        if var1.type == "string":
            Put2Var(ins.args[0].value, Variable(
                "", "int", len(var1.value)))
        else:
            print("Error 53", file=sys.stderr)
            exit(53)
    elif ins.opcode == "GETCHAR":  # Save char from string
        var1 = makeVar(ins.args[1].type, ins.args[1].value)
        var2 = makeVar(ins.args[2].type, ins.args[2].value)

        if var1.type == "" or var2.type == "":
            print("Error 56", file=sys.stderr)
            exit(56)

        if var1.type == "string" and var2.type == "int":
            if len(var1.value)-1 >= int(var2.value) and int(var2.value) >= 0:
                Put2Var(ins.args[0].value, Variable(
                    "", "string", var1.value[int(var2.value)]))
            else:
                print("Error 58", file=sys.stderr)
                exit(58)
        else:
            print("Error 53", file=sys.stderr)
            exit(53)
    elif ins.opcode == "SETCHAR":  # Chenge letter in var1 on index at var2 to first letter of var3
        var1 = makeVar(ins.args[0].type, ins.args[0].value)
        var2 = makeVar(ins.args[1].type, ins.args[1].value)
        var3 = makeVar(ins.args[2].type, ins.args[2].value)

        if var1.type == "" or var2.type == "" or var3.type == "":
            print("Error 56", file=sys.stderr)
            exit(56)

        if var1.type == "string" and var2.type == "int" and var3.type == "string":
            if len(var1.value)-1 >= int(var2.value) and int(var2.value) >= 0 and var3.value != "":
                # concat two halves of first string (minus letter at index) with first letter of second string
                Put2Var(ins.args[0].value, Variable("", "string", var1.value[:int(
                    var2.value)]+var3.value[0]+var1.value[int(var2.value)+1:]))
            else:
                print("Error 58", file=sys.stderr)
                exit(58)
        else:
            print("Error 53", file=sys.stderr)
            exit(53)
    # Prace s typy
    elif ins.opcode == "TYPE":  # Gets type from symb or variable
        var1 = makeVar(ins.args[1].type, ins.args[1].value)
        Put2Var(ins.args[0].value, Variable("", "string", var1.type))
    # Instrukce pro rizeni toku programu
    elif ins.opcode == "LABEL":  # Labels are resolved before other instructions
        pass
    elif ins.opcode == "JUMP":  # if label is found then jump on its order
        my_label = [x for x in label if x.name == ins.args[0].value]
        if my_label != []:
            i = my_label[0].order-1
        else:
            print("Error 52", file=sys.stderr)
            exit(52)
    elif ins.opcode == "EXIT":  # Exit program on given error code
        var1 = makeVar(ins.args[0].type, ins.args[0].value)
        if var1.type == "":
            print("Error 56", file=sys.stderr)
            exit(56)
        if var1.type == "int":
            if int(var1.value) >= 0 and int(var1.value) <= 49:  # value must be in this range
                exit(int(var1.value))
            else:
                print("Error 57", file=sys.stderr)
                exit(57)
        else:
            print("Error 53", file=sys.stderr)
            exit(53)
    elif ins.opcode == "JUMPIFEQ":  # Finds Label and jumps to it if operands are equal
        my_label = [x for x in label if x.name == ins.args[0].value]
        if my_label == []:
            print("Error 52", file=sys.stderr)
            exit(52)
        if JumpEQ(ins):
            i = my_label[0].order-1
        else:
            pass
    elif ins.opcode == "JUMPIFNEQ":  # Finds Label and jumps to it if operands are not equal
        my_label = [x for x in label if x.name == ins.args[0].value]
        if my_label == []:
            print("Error 52", file=sys.stderr)
            exit(52)
        if not(JumpEQ(ins)):
            i = my_label[0].order-1
        else:
            pass
        # Ladici instrukce
    elif ins.opcode == "DPRINT":  # prints symb or value to stderr
        var1 = makeVar(ins.args[0].type, ins.args[0].value)
        if var1.type == "bool":
            if var1.value.upper() == "TRUE":
                print("true", end='', file=sys.stderr)
            else:
                print("false", end='', file=sys.stderr)
        elif var1.type == "nil":
            print("", end='')
        else:
            print(var1.value, end='', file=sys.stderr)
    elif ins.opcode == "BREAK":  # prints state of the interpret
        print("Interpret condition", file=sys.stderr)
        print("-----------------------------------", file=sys.stderr)
        print("Instruction order in code: " + str(ins.order), file=sys.stderr)
        print("Number of instructions executed: " + str(n), file=sys.stderr)
        print("-----------------------------------", file=sys.stderr)
        print("Variables in frames (name, type, value)", file=sys.stderr)
        print("GF: ", file=sys.stderr, end='')
        for x in GF:
            print("(" + x.name + "," + x.type + "," +
                  x.value + ") ", file=sys.stderr, end='')
        print("", file=sys.stderr)
        print("LF: ", file=sys.stderr, end='')
        if len(LF.stack) > 0:
            for x in LF.top_frame():
                print("(" + x.name + "," + x.type + "," +
                      x.value + ") ", file=sys.stderr, end='')
        print(" Number of frames in LF: " +
              str(len(LF.stack)), file=sys.stderr)
        print("TF: ", file=sys.stderr, end='')
        if TF.active:
            for x in TF.stack:
                print("(" + x.name + "," + x.type + "," +
                      x.value + ") ", file=sys.stderr, end='')
        else:
            print("Not Active", file=sys.stderr, end='')
        print("", file=sys.stderr)
        print("Labels: ", file=sys.stderr, end='')
        for x in label:
            print("(" + x.name + "," + str(x.order) + ") ",
                  file=sys.stderr, end='')
        print("", file=sys.stderr)
        print("Stack: ", file=sys.stderr, end='')
        for x in stack:
            print("(" + x.name + "," + x.type + "," +
                  x.value + ") ", file=sys.stderr, end='')
        print("", file=sys.stderr)
    else:
        print("Error 32", file=sys.stderr)
        exit(32)


        # Parsovanie argumentov
parser = argparse.ArgumentParser()
parser.add_argument("--source",
                    help="Select source file from which XML will be read")
parser.add_argument("--input",
                    help="Select input file for the program")

args = parser.parse_args()
source = ""
input_1 = ""

# Save arguments to variables based on if they are set or not
if args.source and not(args.input):
    source = open(args.source, "r")
    input_1 = sys.stdin
elif not(args.source) and args.input:
    input_1 = open(args.input, "r")
    source = sys.stdin
elif args.source and args.input:
    input_1 = open(args.input, "r")
    source = open(args.source, "r")
else:
    print("Error 10", file=sys.stderr)
    exit(10)

# xml load
try:  # Try to load tree if fail error 31
    tree = ET.parse(source)
except:
    print("Error 31", file=sys.stderr)
    exit(31)
program = tree.getroot()

# xml check
order_list = []
# Check language of code
if program.tag != "program" and program.attrib["language"] != "IPPCode22":
    print("Error 32", file=sys.stderr)
    exit(32)
for ins in program:
    if ins.tag != 'instruction':
        print("Error 32", file=sys.stderr)
        exit(32)
    try:  # Check if instruction has correct attributes
        order = int(ins.attrib["order"])
        ins.attrib["opcode"]
        if not(is_integer(ins.attrib["order"])):
            print("Error 32", file=sys.stderr)
            exit(32)
    except:
        print("Error 32", file=sys.stderr)
        exit(32)
    if not(order in order_list) and order > 0:  # Check if there arent duplicate orders in code
        order_list.append(order)
    else:
        print("Error 32", file=sys.stderr)
        exit(32)
    for arg in ins:  # check if args are correctly tagged
        if not(re.match(r"arg[123]", arg.tag)):
            print("Error 32", file=sys.stderr)
            exit(32)
# Sort program by order
program[:] = sorted(program, key=lambda child: (
    child.tag, int(child.get('order'))))

# xml to ins
p = Program()
# Adds instruction and Arguments to program and reorder instructions so there arent any gaps
new_order = 1
for ins in program:
    i = Instruction(ins.attrib["opcode"].upper(), new_order)
    for arg in ins:
        # Sorts arguments based by their number
        a = Argument(arg.attrib["type"], arg.text)
        arg_num = arg.tag[3:]
        i.add_arg(int(arg_num)-1, a)
    p.add_ins(i)
    new_order += 1
# Create memory objects
GF = []
LF = Local_Frame()
TF = Temporary_Frame()
label = []
stack = []
calls = []
# Fill labels before running code
for ins in p.ins:
    if ins.opcode == "LABEL":
        # Check duplicate label name
        if any(x for x in label if x.name == ins.args[0].value):
            print("Error 52", file=sys.stderr)
            exit(52)
        else:
            label.append(Label(ins.order, ins.args[0].value))

try:  # Empty program wont be interpreted
    max_ins = max(ins.order for ins in p.ins)
except:
    exit(0)
i = 1  # instruction index
n = 0  # number of executed instruction (used in BREAK)
while i <= max_ins:  # While loop based on ins.order
    interpret(p.ins[i-1])  # interpret current instruction based on i
    i += 1
    n += 1
