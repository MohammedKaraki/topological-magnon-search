import logging

_FORMAT = "%(asctime)s [%(name)s] [%(levelname)s] %(message)s"

_DEFAULT_LOG_FILENAME = "/tmp/output.log"


def create_root_logger(filename=_DEFAULT_LOG_FILENAME):
    logger = logging.getLogger()
    logger.setLevel(logging.DEBUG)

    class ColoredFormatter(logging.Formatter):
        _grey = "\x1b[38;20m"
        _bold_blue = "\x1b[1;34m"
        _blue = "\x1b[36m"
        _magenta = "\x1b[95m"
        _yellow = "\x1b[91;1;44;20m"
        _red = "\x1b[31;20m"
        _bold_red = "\x1b[31;1m"
        _reset = "\x1b[0m"

        _FORMATS = {
            logging.DEBUG: _blue + _FORMAT + _reset,
            logging.INFO: _magenta + _FORMAT + _reset,
            logging.WARNING: _yellow + _FORMAT + _reset,
            logging.ERROR: _bold_red + _FORMAT + _reset,
            logging.CRITICAL: _bold_red + _FORMAT + _reset,
        }

        def format(self, record):
            log_fmt = self._FORMATS.get(record.levelno)
            formatter = logging.Formatter(log_fmt)
            return formatter.format(record)

    file_handler = logging.FileHandler(filename)
    file_handler.setFormatter(logging.Formatter(_FORMAT))
    file_handler.setLevel(logging.DEBUG)
    logger.addHandler(file_handler)

    console_handler = logging.StreamHandler()
    console_handler.setFormatter(ColoredFormatter())
    console_handler.setLevel(logging.DEBUG)
    logger.addHandler(console_handler)

    return logger


def create_logger(name):
    logger = logging.getLogger(name)
    logger.setLevel(logging.DEBUG)

    return logger
