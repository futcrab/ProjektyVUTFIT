import numpy as np


class Tensor:
    def __init__(self, value, back_op=None):
        self.value = value
        self.grad = np.zeros_like(value)
        self.back_op = back_op

    def __str__(self):
        str_val = str(self.value)
        str_val = '\t' + '\n\t'.join(str_val.split('\n'))
        str_bwd = str(self.back_op.__class__.__name__)
        return 'Tensor(\n' + str_val + '\n\tbwd: ' + str_bwd + '\n)'

    @property
    def shape(self):
        return self.value.shape

    def backward(self, deltas=None):
        if deltas is not None:
            assert deltas.shape == self.value.shape, f'Expected gradient with shape {self.value.shape}, got {deltas.shape}'

            # Accumulate deltas for this tensor
            self.grad += deltas

            # Check if back operation is available
            if self.back_op is not None:
                # Backpropagate the deltas to the previous tensor
                self.back_op.backward(deltas)
        else:
            if self.shape != tuple() and np.prod(self.shape) != 1:
                raise ValueError(f'Can only backpropagate a scalar, got shape {self.shape}')

            if self.back_op is None:
                raise ValueError(f'Cannot start backpropagation from a leaf!')

            # Initialize the gradient with ones
            self.grad = np.ones_like(self.value)

            # Backpropagate the gradient to the previous tensor
            self.back_op.backward(self.grad)



def sui_sum(tensor):
    result = np.sum(tensor.value)

    # Create a back operation
    class sumBackOp:
        def __init__(self, input_tensor):
            self.input_tensor = input_tensor
        
        def backward(self, grad_out):
            # Calculate the gradient of the sum operation
            grad_in = np.ones_like(self.input_tensor.value) * grad_out
            self.input_tensor.grad += grad_in
            # Backpropagate the gradient to the previous tensor
            if self.input_tensor.back_op is not None:
                self.input_tensor.back_op.backward(grad_in)

    # Create a new tensor with the result and the back operation
    return Tensor(np.array(result), back_op=sumBackOp(tensor))



def add(a, b):
    result = np.add(a.value, b.value)

    # Create a back operation
    class addBackOp:
        def __init__(self, a, b):
            self.a = a
            self.b = b
        
        def backward(self, grad_out):
            if self.a.shape == grad_out.shape:
                self.a.grad += grad_out
            else:
                # Broadcasting if shapes are different
                self.a.grad += np.sum(grad_out, axis=tuple(range(grad_out.ndim - self.a.value.ndim)))
            if self.b.shape == grad_out.shape:
                self.b.grad += grad_out
            else:
                # Broadcasting if shapes are different
                self.b.grad += np.sum(grad_out, axis=tuple(range(grad_out.ndim - self.b.value.ndim)))
            
            # Backpropagate the gradient to the previous tensors
            if self.a.back_op is not None:
                self.a.back_op.backward(self.a.grad)
            if self.b.back_op is not None:
                self.b.back_op.backward(self.b.grad)

    # Create a new tensor with the result and the back operation
    return Tensor(np.array(result), back_op=addBackOp(a, b))


def subtract(a, b):
    result = np.subtract(a.value, b.value)

    # Create a back operation
    class SubtractBackOp:
        def __init__(self, a, b):
            self.a = a
            self.b = b
        
        def backward(self, grad_out):
            if self.a.shape == grad_out.shape:
                self.a.grad += grad_out
            else:
                # Broadcasting if shapes are different
                self.a.grad += np.sum(grad_out, axis=tuple(range(grad_out.ndim - self.a.value.ndim)))
            # In the case of subtraction, the gradient of b is negative
            if self.b.shape == grad_out.shape:
                self.b.grad -= grad_out
            else:
                # Broadcasting if shapes are different
                self.b.grad -= np.sum(grad_out, axis=tuple(range(grad_out.ndim - self.b.value.ndim)))

            # Backpropagate the gradient to the previous tensors
            if self.a.back_op is not None:
                self.a.back_op.backward(self.a.grad)
            if self.b.back_op is not None:
                self.b.back_op.backward(self.b.grad)

    # Create a new tensor with the result and the back operation
    return Tensor(np.array(result), back_op=SubtractBackOp(a, b))


def multiply(a, b):
    result = np.multiply(a.value, b.value)

    # Create a back operation
    class MultiplyBackOp:
        def __init__(self, a, b):
            self.a = a
            self.b = b
        
        def backward(self, grad_out):
            if self.a.shape == grad_out.shape:
                # For multiplication, the gradient is the value of the other tensor
                self.a.grad += grad_out * self.b.value
            else:
                # Broadcasting if shapes are different
                self.a.grad += np.sum(grad_out * self.b.value, axis=tuple(range(grad_out.ndim - self.a.value.ndim)))
            if self.b.shape == grad_out.shape:
                self.b.grad += grad_out * self.a.value
            else:
                # Broadcasting if shapes are different
                self.b.grad += np.sum(grad_out * self.a.value, axis=tuple(range(grad_out.ndim - self.b.value.ndim)))

            # Backpropagate the gradient to the previous tensors
            if self.a.back_op is not None:
                self.a.back_op.backward(self.a.grad)
            if self.b.back_op is not None:
                self.b.back_op.backward(self.b.grad)

    # Create a new tensor with the result and the back operation
    return Tensor(np.array(result), back_op=MultiplyBackOp(a, b))


def relu(tensor):
    result = np.maximum(tensor.value, 0)

    # Create a back operation
    class ReluBackOp:
        def __init__(self, input_tensor):
            self.input_tensor = input_tensor
        
        def backward(self, grad_out):
            # If value is greater than 0, the gradient is the same as the output gradient, other it is 0
            grad_in = (self.input_tensor.value > 0) * grad_out
            self.input_tensor.grad += grad_in
            
            # Backpropagate the gradient to the previous tensor
            if self.input_tensor.back_op is not None:
                self.input_tensor.back_op.backward(grad_in)

    # Create a new tensor with the result and the back operation
    return Tensor(np.array(result), back_op=ReluBackOp(tensor))


def dot_product(a, b):
    result = np.dot(a.value, b.value)

    # Create a back operation
    class DotProductBackOp:
        def __init__(self, a, b):
            self.a = a
            self.b = b
        
        def backward(self, grad_out):
            # Gradient for a is the dot product of the output gradient and the transpose of b
            grad_a = np.dot(grad_out, self.b.value.T)
            if self.a.shape == grad_a.shape:
                self.a.grad += grad_a
            else:
                # Broadcasting if shapes are different
                self.a.grad += np.sum(grad_a, axis=tuple(range(grad_a.ndim - self.a.value.ndim)))

            # Gradient for b is the dot product of the transpose of a and the output gradient
            grad_b = np.dot(self.a.value.T, grad_out)
            if self.b.shape == grad_b.shape:
                self.b.grad += grad_b
            else:
                # Broadcasting if shapes are different
                self.b.grad += np.sum(grad_b, axis=tuple(range(grad_b.ndim - self.b.value.ndim)))

            # Backpropagate the gradient to the previous tensors
            if self.a.back_op is not None:
                self.a.back_op.backward(self.a.grad)
            if self.b.back_op is not None:
                self.b.back_op.backward(self.b.grad)
    
    # Create a new tensor with the result and the back operation
    return Tensor(np.array(result), back_op=DotProductBackOp(a, b))