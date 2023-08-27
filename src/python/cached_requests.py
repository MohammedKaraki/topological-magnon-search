import requests
import os
from re import fullmatch
from pathlib import Path
from subprocess import check_output

import log

logger = log.create_logger(__name__)


def cached_post(url, data, cache_filename):
    full_path = str(
        (Path(__file__) / "../../../cache/{}".format(cache_filename)).resolve()
    )

    # full_path = '{0}/{1}'.format(CACHE_DIRECTORY, cache_filename)
    assert fullmatch(r"[-_.+/(),0-9a-zA-Z]+", full_path), full_path

    try:
        with open(full_path, "r") as file:
            logger.info("Cache Hit: '{0}'".format(full_path))

            return file.read()

    except FileNotFoundError:
        logger.info("Cache Miss: '{0}'".format(full_path))

        response = requests.post(url, data=data)
        response.raise_for_status()

        result = response.text

        assert not os.path.exists(full_path)
        with open(full_path, "w") as file:
            file.write(result)
            logger.info("Cached: '{0}'".format(full_path))

        return result


def cached_get(url, params, cache_filename):
    full_path = str(
        (Path(__file__) / "../../../cache/{}".format(cache_filename)).resolve()
    )

    # full_path = '{0}/{1}'.format(CACHE_DIRECTORY, cache_filename)
    assert fullmatch(r"[-_.+/(),0-9a-zA-Z]+", full_path), full_path

    try:
        with open(full_path, "r") as file:
            logger.info("Cache Hit: '{0}'".format(full_path))

            return file.read()

    except FileNotFoundError:
        logger.info("Cache Miss: '{0}'".format(full_path))

        response = requests.get(url, params=params)
        response.raise_for_status()

        result = response.text

        assert not os.path.exists(full_path)
        with open(full_path, "w") as file:
            file.write(result)
            logger.info("Cached: '{0}'".format(full_path))

        return result


def cached_check_output(command, args, cache_filename):
    full_path = str(
        (Path(__file__) / "../../../cache/{}".format(cache_filename)).resolve()
    )

    # full_path = '{0}/{1}'.format(CACHE_DIRECTORY, cache_filename)
    assert fullmatch(r"[-_.+/(),0-9a-zA-Z]+", full_path), full_path

    try:
        with open(full_path, "r") as file:
            logger.info("Cache Hit: '{0}'".format(full_path))

            return file.read()

    except FileNotFoundError:
        logger.info("Cache Miss: '{0}'".format(full_path))

        result = check_output([command] + args).decode("utf-8")
        with open(full_path, "w") as file:
            file.write(result)
            logger.info("Cached: '{0}'".format(full_path))

        return result
