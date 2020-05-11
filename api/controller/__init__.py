from pathlib import Path

from controller.model import Model


class Controller:

    @staticmethod
    def init(app):
        module_prefix = Path('modules')

        Model.weights = Path(app.config['WEIGHTS'].get('path', 'weights'))
        Model.weights.mkdir(exist_ok=True, parents=True)

        for module in app.config.get('MODULES', []):
            Model.s[module['name'].lower()] = Model(
                module['name'], str(module_prefix.joinpath(module['path'])), module['file']
            )
