import torch


class TorchScript:
    def __init__(self, name: str, num_classes: int, batch_size: int = 1):
        self.name = name
        self.num_classes = num_classes
        self.batch_size = batch_size
        self.module = None

    def load(self, file: str):
        print(file)
        self.module = torch.jit.load(file)
    
    def __call__(self, inputs):
        if self.module is not None:
            return self.module(inputs)
        return []

    def __getattr__(self, attr):
        try:
            return super().getattr(attr)
        except AttributeError:
            return getattr(self.module, attr)
