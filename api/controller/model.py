import numpy as np
import torch
from torch.autograd import Variable

from utils import io
from utils.lazy import lazy
from utils.moduletools import import_module


class Model:
    s = {}
    instance = None
    weights = None

    @classmethod
    @lazy
    def w(cls):
        return list(map(lambda weight: weight.name, cls.weights.glob('*.pt*')))

    @classmethod
    def init(cls, weight, num_classes, expire):
        prefix, *_ = weight.split('-')

        if prefix not in cls.s.keys():
            raise FileNotFoundError

        cls.instance = cls.s[prefix]
        cls.instance.load(weight, num_classes, expire)


    def __init__(self, name: str, path: str, file: str):
        self.num_classes = 2
        self.batch_size = 1

        self.device = torch.device('cpu')
        self.model_name = name
        self.model_constructor = import_module(path, file, name)
        self.model = None
        self.weight = ''

    def __call__(self, image):
        if not getattr(self, 'model', None):
            raise Exception("Load model first")

        h, w, *c = image.shape
        scale = np.array([w, h, w, h])
        image = io.transform(image)
        image = Variable(torch.from_numpy(image).permute(
            2, 0, 1).unsqueeze(0)).to(self.device)

        detections, *_ = self.model(image).data

        classes = np.empty((0), dtype=np.int)
        scores = np.empty((0), dtype=np.float32)
        boxes = np.empty((0, 4), dtype=np.float32)

        for klass, candidates in enumerate(detections):
            candidates = candidates[candidates[:, 0] >= .3]

            if candidates.size(0) == 0:
                continue

            candidates = candidates.cpu().detach().numpy()

            classes = np.concatenate((
                classes,
                np.full(np.size(candidates, 0), klass, dtype=np.uint8),
            ))
            scores = np.concatenate((
                scores,
                candidates[:, 0],
            ))
            boxes = np.concatenate((
                boxes,
                candidates[:, 1:] * scale,
            ))

        boxes[:, 2] = boxes[:, 2] - boxes[:, 0]
        boxes[:, 3] = boxes[:, 3] - boxes[:, 1]

        return {
            "classes": classes,
            "scores": scores,
            "boxes": boxes,
        }

    def load(self, weight, num_classes=2, expire=600):
        self.num_classes = num_classes
        self.model = getattr(self.model_constructor, self.model_name)(
            self.num_classes, self.batch_size)

        self.weight = weight
        self.model.load(io.join(self.weights, weight))
        self.model.eval()
        self.model.to(self.device)

    def release(self):
        self.model = None
