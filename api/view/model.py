from controller import Model
from view import Router, render, request
from utils import io
from utils import network

model = Router('model')


@model.route('/', methods=['GET'])
def index():
    weights = Model.w()
    return render({
        'modules': {
            key: [w for w in weights if w.startswith(key)]
            for key in Model.s.keys()
        },
    })


@model.route('/', methods=['POST'])
def load():
    response = {'status': 'pending'}

    try:
        Model.init(**{
            'num_classes': request.form.get('class', 2),
            'expire': request.form.get('expire', 600),
            'weight': request.form.get('weight', '').lower(),
        })
        response.update({'status': 'ok'})

    except (FileNotFoundError, RuntimeError) as e:
        response.update({
            'status': 'failed',
            'message': str(e),
        })

    finally:
        return render(response)


@model.route('/eval', methods=['POST'])
def inference():
    response = {'status': 'pending'}
    url = request.form.get('url', '')

    try:
        image = io.load(url)

        response.update({
            k: v.tolist() for k, v in Model.instance(image).items()
        })
        response.update({
            'status': 'ok',
            'size': image.shape
        })

    except Exception as e:
        response.update({
            'status': 'failed',
            'message': str(e),
        })

    finally:
        return render(response)


@model.route('/', methods=['DELETE'])
def release():
    response = {'status': 'pending'}

    try:
        Model.instance.release()
        response.update({'status': 'ok'})

    except Exception as e:
        response.update({
            'status': 'failed',
            'message': str(e),
        })

    finally:
        return render(response)
