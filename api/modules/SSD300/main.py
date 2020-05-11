from pathlib import Path

import torch
import torch.optim as optim

from ssd import SSD300, Loss
from data.dataset import Dataset
from lib.augmentation import Augmentation
from utils import seed
from utils.executable import Executable
from utils.arguments import Arguments
from utils.config import Config


def main(args: Arguments.parse.Namespace, config: Config):

    executor = Executable.s[args.command]
    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    dataset = Dataset.get(args.type)(args.dataset,
                                     transform=Augmentation.get(args.type)(**config.data),
                                     eval_only=args.eval_only)

    if executor.name != 'train':
        args.batch = 1

    model = SSD300(dataset.num_classes, args.batch,
                   warping=args.warping, warping_mode=args.warping_mode,
                   config=config.data)

    model = executor.init(model, device, args)

    Path(args.dest).mkdir(exist_ok=True, parents=True)

    optimizer = optim.SGD(model.parameters(), lr=args.lr,
                          momentum=args.momentum, weight_decay=args.decay)
    criterion = Loss(dataset.num_classes, device=device)

    executor(model, dataset=dataset,
             criterion=criterion, optimizer=optimizer,              # train args
             transform=Augmentation.get('base')(**config.data),     # test args
             device=device, args=args)


if __name__ == '__main__':
    arguments = Arguments()
    seed(arguments.seed)
    main(arguments, Config(arguments.config))
