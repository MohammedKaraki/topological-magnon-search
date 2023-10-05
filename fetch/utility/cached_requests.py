import requests
import json
from os import path
from pathlib import Path
from hashlib import sha256

from magnon.common.logger import create_logger

logger = create_logger(__name__)


def _cache_filename_from_args(func, *args, **kwargs):
    """Creates a filename for caching, reproducibly for a given input."""
    return sha256(json.dumps([func.__name__, args, kwargs]).encode()).hexdigest()


def _cached_call(cache_dir, function, *args, **kwargs):
    cache_filename = _cache_filename_from_args(function, *args, **kwargs)
    full_path = str((Path(cache_dir) / cache_filename).resolve())
    try:
        with open(full_path, "r") as file:
            logger.info("Cache Hit: '{0}'".format(full_path))
            return file.read()
    except FileNotFoundError:
        logger.info("Cache Miss: '{0}'".format(full_path))
        response = function(*args, **kwargs)
        response.raise_for_status()
        result = response.text

        assert not path.exists(full_path)
        with open(full_path, "w") as file:
            file.write(result)
            logger.info("Cached: '{0}'".format(full_path))

        return result


def cached_post(url, data, cache_dir):
    return _cached_call(cache_dir, requests.post, url, data=data)


def cached_get(url, params, cache_dir):
    return _cached_call(cache_dir, requests.get, url, params=params)
