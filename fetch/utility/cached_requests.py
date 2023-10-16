import requests
import json
from os import path
from pathlib import Path
from hashlib import sha256

from magnon.common.logger import create_logger
from magnon.config.read_global_config_python import read_global_config

_logger = create_logger(__name__)

_cache_dir = read_global_config().cache_dir


def _cache_filename_from_args(func, *args, **kwargs):
    """Creates a filename for caching, reproducibly for a given input."""
    return sha256(json.dumps([func.__name__, args, kwargs]).encode()).hexdigest()


def _cached_call(function, *args, **kwargs):
    cache_filename = _cache_filename_from_args(function, *args, **kwargs)
    assert len(_cache_dir) > 0
    full_path = str((Path(_cache_dir) / cache_filename).resolve())
    try:
        with open(full_path, "r") as file:
            _logger.info("Cache Hit: '{0}'".format(full_path))
            return file.read()
    except FileNotFoundError:
        _logger.info("Cache Miss: '{0}'".format(full_path))
        response = function(*args, **kwargs)
        response.raise_for_status()
        result = response.text

        assert not path.exists(full_path)
        with open(full_path, "w") as file:
            file.write(result)
            _logger.info("Cached: '{0}'".format(full_path))

        return result


def cached_post(url, data):
    return _cached_call(requests.post, url, data=data)


def cached_get(url, params):
    return _cached_call(requests.get, url, params=params)
